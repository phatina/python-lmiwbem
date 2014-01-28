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

#include <sstream>
#include <boost/python/object.hpp>
#include "lmiwbem.h"
#include "lmiwbem_exception.h"

namespace bp = boost::python;

extern bp::object CIMError;
extern bp::object ConnectionError;

namespace {

inline void throw_core(PyObject *exc, const std::string &message)
{
    PyErr_SetString(exc, message.c_str());
    bp::throw_error_already_set();
}

inline void throw_core(const bp::object &exc, const std::string &message)
{
    throw_core(exc.ptr(), message);
}

} // unnamed namespace

void throw_Exception(const Pegasus::Exception &e)
{
    throw_core(CIMError, std::string(
        (Pegasus::String("Pegasus: ") + e.getMessage()).getCString()));
}

void throw_Exception(const Pegasus::CannotConnectException &e)
{
    throw_core(ConnectionError, std::string(e.getMessage().getCString()));
}

void throw_CIMError(const std::string &message)
{
    throw_core(CIMError.ptr(), message);
}

void throw_ValueError(const std::string &message)
{
    throw_core(PyExc_ValueError, message);
}

void throw_KeyError(const std::string &message)
{
    throw_core(PyExc_KeyError, message);
}

void throw_StopIteration(const std::string &message)
{
    throw_core(PyExc_StopIteration, message);
}

void throw_TypeError(const std::string &message)
{
    throw_core(PyExc_TypeError, message);
}
