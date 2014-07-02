/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014, Peter Hatina <phatina@redhat.com>
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 2.1 of the
 *   License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *   MA 02110-1301 USA
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef   LMIWBEM_REFCOUNTEDPTR_H
#  define LMIWBEM_REFCOUNTEDPTR_H

#  include "lmiwbem_mutex.h"

template <typename T>
class RefCountedPtrValue
{
public:
    RefCountedPtrValue()
        : m_refcnt(0)
        , m_value(NULL)
        , m_mutex()
    {
    }

    RefCountedPtrValue(T *value)
        : m_refcnt(1)
        , m_value(value)
        , m_mutex()
    {
    }

    size_t ref()
    {
        ScopedMutex sm(m_mutex);
        return ++m_refcnt;
    }

    size_t unref()
    {
        ScopedMutex sm(m_mutex);
        if (m_refcnt > 0 && --m_refcnt == 0) {
            delete m_value;
            m_value = NULL;
        }
        return m_refcnt;
    }

    size_t refcnt() const { return m_refcnt; }
    bool empty() const { return m_refcnt == 0; }

    void set(T *value)
    {
        m_refcnt = 1;
        m_value = value;
    }

    T *get() const { return m_value; }

private:
    RefCountedPtrValue(const RefCountedPtrValue &copy);
    RefCountedPtrValue &operator=(const RefCountedPtrValue &rhs);

    size_t m_refcnt;
    T *m_value;
    Mutex m_mutex;
};

template <typename T>
class RefCountedPtr
{
public:
    RefCountedPtr(): m_value(new RefCountedPtrValue<T>()) { }
    RefCountedPtr(const RefCountedPtr &copy)
        : m_value(copy.m_value)
    {
        if (m_value)
            m_value->ref();
    }

    ~RefCountedPtr() { release(); }

    void set(const T &value)
    {
        if (!m_value || m_value->unref() != 0)
            m_value = new RefCountedPtrValue<T>();

        m_value->set(new T(value));
    }

    T *get() { return m_value->get(); }

    bool empty() { return m_value == NULL || m_value->get() == NULL; }

    void release()
    {
        if (m_value && m_value->unref() == 0)
            delete m_value;

        m_value = NULL;
    }

private:
    RefCountedPtr &operator=(const RefCountedPtr &rhs);

    RefCountedPtrValue<T> *m_value;
};

#endif // LMIWBEM_REFCOUNTEDPTR_H
