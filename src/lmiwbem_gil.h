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

#ifndef   LMIWBEM_GIL_H
#  define LMIWBEM_GILH_

#  include <Python.h>

class ScopedGILAcquire
{
public:
    ScopedGILAcquire()
    {
        m_state = PyGILState_Ensure();
    }

    ~ScopedGILAcquire()
    {
        PyGILState_Release(m_state);
    }

private:
    PyGILState_STATE m_state;
};

class ScopedGILRelease
{
public:
    ScopedGILRelease()
    {
        m_thread_state = PyEval_SaveThread();
    }

    ~ScopedGILRelease()
    {
        PyEval_RestoreThread(m_thread_state);
    }

private:
    PyThreadState *m_thread_state;
};

#endif
