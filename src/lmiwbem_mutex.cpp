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

#include <config.h>
#include "lmiwbem_mutex.h"

Mutex::Mutex()
    : m_good(false)
    , m_locked(false)
{
    m_good = pthread_mutex_init(&m_mutex, NULL) == 0;
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}

bool Mutex::lock()
{
    // We can't lock the mutex, initialization failed.
    if (!m_good)
        return false;

    if (pthread_mutex_lock(&m_mutex) == 0)
        m_locked = true;

    return m_locked;
}

bool Mutex::unlock()
{
    // We can't unlock the mutex, initialization failed.
    if (!m_good)
        return false;

    if (pthread_mutex_unlock(&m_mutex) == 0)
        m_locked = false;

    return m_locked;
}

bool Mutex::isLocked() const
{
    return m_locked;
}

ScopedMutex::ScopedMutex(Mutex &m)
    : m_mutex(m)
{
    m_mutex.lock();
}

ScopedMutex::~ScopedMutex()
{
    m_mutex.unlock();
}

bool ScopedMutex::lock()
{
    return m_mutex.lock();
}

bool ScopedMutex::unlock()
{
    return m_mutex.unlock();
}

bool ScopedMutex::isLocked() const
{
    return m_mutex.isLocked();
}
