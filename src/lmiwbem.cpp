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
#include <boost/python/def.hpp>
#include <boost/python/docstring_options.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/module.hpp>
#include <boost/python/object.hpp>
#include <boost/python/scope.hpp>
#include "lmiwbem_class.h"
#include "lmiwbem_class_name.h"
#include "lmiwbem_connection.h"
#include "lmiwbem_constants.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_listener.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_method.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_parameter.h"
#include "lmiwbem_property.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_types.h"
#include "lmiwbem_util.h"

#include <list>

namespace bp = boost::python;

bp::object CIMError;
bp::object ConnectionError;

inline void translatePegasusCannotConnectException(const Pegasus::CannotConnectException &e)
{
    PyErr_SetString(ConnectionError.ptr(), e.getMessage().getCString());
}

BOOST_PYTHON_MODULE(lmiwbem_core) {
    // Initialize Python threads
    PyEval_InitThreads();

    CIMError = bp::object(
        bp::handle<>(PyErr_NewException((char*) "lmiwbem.CIMError", NULL, NULL))
    );
    ConnectionError = bp::object(
        bp::handle<>(PyErr_NewException((char*) "lmiwbem.ConnectionError", NULL, NULL))
    );

    // Disable C++ signatures in Python's doc-strings
    bp::docstring_options doc_options;
    doc_options.disable_py_signatures();
    doc_options.disable_cpp_signatures();

    // Fill module's dictionary with exceptions
    bp::scope().attr("CIMError") = CIMError;
    bp::scope().attr("ConnectionError") = ConnectionError;

    // Register exception translator
    bp::register_exception_translator
        <Pegasus::CannotConnectException>
        (translatePegasusCannotConnectException);

    // Register type converts
    PegasusStringToPythonString::register_converter();
    PegasusCIMNameToPythonString::register_converter();
    PegasusCIMDateteTimeToPythonDateTime::register_converter();
    PegasusChar16ToPythonUint16::register_converter();
    PegasusCIMObjectPathToPythonCIMInstanceName::register_converter();

    // Initialize module functions
    def("is_subclass",
        cim_issubclass,
        "Determine if one class is a subclass of another one.\n\n"
        ":param WBEMConnection ch: CIMOM handle\n"
        ":param string ns: namespace where to look for :py:class:`CIMClass`-es\n"
        ":param string superclass: super class name\n"
        ":param subclass: either string containing sub class name of\n"
        "\t:py:class:`CIMClass` instance");

    // Initialize Python classes
    MinutesFromUTC::init_type();
    CIMType::init_type();
    CIMDateTime::init_type();
    CIMInt::init_type();
    Uint8::init_type();
    Sint8::init_type();
    Uint16::init_type();
    Sint16::init_type();
    Uint32::init_type();
    Sint32::init_type();
    Uint64::init_type();
    Sint64::init_type();
    CIMFloat::init_type();
    Real32::init_type();
    Real64::init_type();

    // Initialize own classes
    WBEMConnection::init_type();
    NocaseDict::init_type();
    NocaseDictKeyIterator::init_type();
    NocaseDictValueIterator::init_type();
    NocaseDictItemIterator::init_type();
    CIMInstance::init_type();
    CIMInstanceName::init_type();
    CIMMethod::init_type();
    CIMParameter::init_type();
    CIMProperty::init_type();
    CIMQualifier::init_type();
    CIMClass::init_type();
    CIMClassName::init_type();
    CIMConstants::init_type();
    CIMIndicationListener::init_type();
}
