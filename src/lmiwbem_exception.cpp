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
#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include <boost/python/tuple.hpp>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/CIMClientException.h>
#include "lmiwbem.h"
#include "lmiwbem_exception.h"
#include "obj/cim/lmiwbem_constants.h"

namespace bp = boost::python;

extern bp::object CIMErrorExc;
extern bp::object ConnectionErrorExc;
extern bp::object SLPErrorExc;

namespace {

inline void throw_core(PyObject *exc, const String &message)
{
    PyErr_SetString(exc, message.c_str());
    bp::throw_error_already_set();
}

inline void throw_core(
    PyObject *exc,
    const String &message,
    int code)
{
    PyErr_SetObject(exc, bp::make_tuple(code, bp::str(message)).ptr());
    bp::throw_error_already_set();
}

inline void throw_core(const bp::object &exc, const String &message)
{
    throw_core(exc.ptr(), message);
}

inline void throw_core(
    const bp::object &exc,
    const String &message,
    int code)
{
    throw_core(exc.ptr(), message, code);
}

} // unnamed namespace

void throw_Exception(const Pegasus::Exception &e)
{
    throw_core(CIMErrorExc, String(
        (Pegasus::String("Pegasus: ") + e.getMessage())));
}

void throw_Exception(const String &message)
{
    throw_core(CIMErrorExc, String("Pegasus: ") + message);
}

void throw_ConnectionError(const String &message, int code)
{
    throw_core(ConnectionErrorExc, message, code);
}

void throw_CIMError(const Pegasus::CIMException &e)
{
    throw_core(
        CIMErrorExc,
        String(e.getMessage()),
        static_cast<int>(e.getCode()));
}

void throw_CIMError(const String &message, int code)
{
    throw_core(CIMErrorExc, message, code);
    bp::throw_error_already_set();
}

void throw_SLPError(const String &message, int code)
{
    throw_core(SLPErrorExc, message, code);
    bp::throw_error_already_set();
}

void throw_ValueError(const String &message)
{
    throw_core(PyExc_ValueError, message);
}

void throw_KeyError(const String &message)
{
    throw_core(PyExc_KeyError, message);
}

void throw_MemoryError(const String &message)
{
    throw_core(PyExc_MemoryError, message);
}

void throw_StopIteration(const String &message)
{
    throw_core(PyExc_StopIteration, message);
}

void throw_TypeError(const String &message)
{
    throw_core(PyExc_TypeError, message);
}

void throw_RuntimeError(const String &message)
{
    throw_core(PyExc_RuntimeError, message);
}

void handle_all_exceptions(const String &prefix)
{
    std::stringstream ss(prefix);
    handle_all_exceptions(ss);
}

void handle_all_exceptions(std::stringstream &prefix)
{
    if (!prefix.str().empty())
        prefix << ": ";

    try {
        try {
            // Re-throw. If we got Pegasus exception, we also append the
            // exception's message to the Python's exception message.
            throw;
        } catch (const Pegasus::Exception &e) {
            prefix << e.getMessage();
        }

        // Re-throw to raise proper Python exception.
        throw;
    } catch (const Pegasus::AlreadyConnectedException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_ALREADY_CONNECTED);
    } catch (const Pegasus::NotConnectedException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_NOT_CONNECTED);
    } catch (const Pegasus::InvalidLocatorException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_INVALID_LOCATOR);
    } catch (const Pegasus::CannotCreateSocketException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_CANNOT_CREATE_SOCKET);
    } catch (const Pegasus::CannotConnectException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_CANNOT_CONNECT);
    } catch (const Pegasus::ConnectionTimeoutException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_CONNECTION_TIMEOUT);
    } catch (const Pegasus::CIMClientHTTPErrorException &e) {
        throw_ConnectionError(
            prefix.str(),
            static_cast<int>(e.getCode()));
    } catch (const Pegasus::CIMException &e) {
        throw_CIMError(
            prefix.str(),
            static_cast<int>(e.getCode()));
    } catch (const Pegasus::BindFailedException &e) {
        throw_ConnectionError(
            prefix.str(),
            CIMConstants::CON_ERR_BIND);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(prefix.str());
    }
}
