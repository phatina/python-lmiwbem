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

#ifndef LMIWBEM_REFCOUNTEDPTR_H
#define LMIWBEM_REFCOUNTEDPTR_H

template <typename T>
class RefCountedPtr
{
public:
    RefCountedPtr()
        : m_value(NULL)
        , m_refcnt(0)
    {
    }

    RefCountedPtr(const T &value, size_t refcnt = 1)
        : m_value(new T(value))
        , m_refcnt(refcnt)
    {
    }

    ~RefCountedPtr()
    {
        if (m_refcnt)
            delete m_value;
    }

    void set(const T &value, size_t refcnt = 1)
    {
        if (m_refcnt)
            delete m_value;

        m_value = new T(value);
        m_refcnt = refcnt;
    }

    T *get() { return m_value; }

    void unref()
    {
        if (m_refcnt && --m_refcnt == 0) {
            delete m_value;
            m_value = NULL;
        }
    }

private:
    T *m_value;
    size_t m_refcnt;
};

#endif // LMIWBEM_REFCOUNTEDPTR_H
