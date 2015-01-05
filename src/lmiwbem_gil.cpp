/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014-2015, Peter Hatina <phatina@redhat.com>
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
#include <Python.h>
#include "lmiwbem_gil.h"

class ScopedGILAcquire::ScopedGILAcquireRep
{
public:
    ScopedGILAcquireRep();

    PyGILState_STATE m_state;
};

class ScopedGILRelease::ScopedGILReleaseRep
{
public:
    ScopedGILReleaseRep();

    PyThreadState *m_thread_state;
};

ScopedGILAcquire::ScopedGILAcquireRep::ScopedGILAcquireRep()
{
}

ScopedGILRelease::ScopedGILReleaseRep::ScopedGILReleaseRep()
    : m_thread_state(NULL)
{
}

ScopedGILAcquire::ScopedGILAcquire()
    : m_rep(new ScopedGILAcquireRep)
{
    m_rep->m_state = PyGILState_Ensure();
}

ScopedGILAcquire::~ScopedGILAcquire()
{
    PyGILState_Release(m_rep->m_state);
}

ScopedGILRelease::ScopedGILRelease()
{
    m_rep->m_thread_state = PyEval_SaveThread();
}

ScopedGILRelease::~ScopedGILRelease()
{
    PyEval_RestoreThread(m_rep->m_thread_state);
}
