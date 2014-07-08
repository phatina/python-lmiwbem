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

#ifndef   LMIWBEM_MUTEX_H
#  define LMIWBEM_MUTEX_H

extern "C" {
#  include <pthread.h>
}

class Mutex
{
public:
    Mutex()
        : m_good(false)
        , m_locked(false)
    {
        m_good = pthread_mutex_init(&m_mutex, NULL) == 0;
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock()
    {
        // We can't lock the mutex, initialization failed.
        if (!m_good)
            return false;

        if (pthread_mutex_lock(&m_mutex) == 0)
            m_locked = true;

        return m_locked;
    }

    bool unlock()
    {
        // We can't unlock the mutex, initialization failed.
        if (!m_good)
            return false;

        if (pthread_mutex_unlock(&m_mutex) == 0)
            m_locked = false;

        return m_locked;
    }

    bool isLocked() const { return m_locked; }

private:
    bool m_good;
    bool m_locked;
    pthread_mutex_t m_mutex;
};

class ScopedMutex
{
public:
    ScopedMutex(Mutex &m): m_mutex(m) { m_mutex.lock(); }
    ~ScopedMutex() { m_mutex.unlock(); }

    bool lock()   { return m_mutex.lock(); }
    bool unlock() { return m_mutex.unlock(); }
    bool isLocked() const { return m_mutex.isLocked(); }

private:
    Mutex &m_mutex;
};

#endif // LMIWBEM_MUTEX_H
