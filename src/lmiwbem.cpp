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
#include <boost/python/def.hpp>
#include <boost/python/docstring_options.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/module.hpp>
#include <boost/python/object.hpp>
#include <boost/python/scope.hpp>
#include "lmiwbem_exception.h"
#include "obj/lmiwbem_config.h"
#include "obj/lmiwbem_connection.h"
#ifdef HAVE_PEGASUS_LISTENER
#  include "obj/lmiwbem_listener.h"
#endif // HAVE_PEGASUS_LISTENER
#ifdef HAVE_SLP
#  include "obj/lmiwbem_slp.h"
#endif // HAVE_SLP
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_class.h"
#include "obj/cim/lmiwbem_class_name.h"
#include "obj/cim/lmiwbem_constants.h"
#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
#  include "obj/cim/lmiwbem_enum_ctx.h"
#endif // HAVE_PEGASUS_ENUMERATION_CONTEXT
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_method.h"
#include "obj/cim/lmiwbem_parameter.h"
#include "obj/cim/lmiwbem_property.h"
#include "obj/cim/lmiwbem_qualifier.h"
#include "obj/cim/lmiwbem_types.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

#include <list>

namespace bp = boost::python;

bp::object CIMErrorExc;
bp::object ConnectionErrorExc;
bp::object SLPErrorExc;

BOOST_PYTHON_MODULE(lmiwbem_core) {
    // Initialize Python threads
    PyEval_InitThreads();

    CIMErrorExc = bp::object(
        bp::handle<>(PyErr_NewException((char*) "lmiwbem_core.CIMError", NULL, NULL)));
    ConnectionErrorExc = bp::object(
        bp::handle<>(PyErr_NewException((char*) "lmiwbem_core.ConnectionError", NULL, NULL)));
    SLPErrorExc = bp::object(
        bp::handle<>(PyErr_NewException((char*) "lmiwbem_core.SLPError", NULL, NULL)));

    // Disable C++ signatures in Python's doc-strings
    bp::docstring_options doc_options;
    doc_options.disable_py_signatures();
    doc_options.disable_cpp_signatures();

    // Fill module's dictionary with exceptions
    bp::scope().attr("CIMError") = CIMErrorExc;
    bp::scope().attr("ConnectionError") = ConnectionErrorExc;
    bp::scope().attr("SLPError") = SLPErrorExc;

    // Register type converts
    StringToPythonString::register_converter();
    PegasusStringToPythonString::register_converter();
    PegasusCIMNameToPythonString::register_converter();
    PegasusCIMDateteTimeToPythonDateTime::register_converter();
    PegasusChar16ToPythonUint16::register_converter();
    PegasusCIMObjectPathToPythonCIMInstanceName::register_converter();
    CIMConstantsCIMErrorToPythonInt::register_converter();

    // Initialize module functions
    def("is_subclass",
        cim_issubclass,
        "Determine if one class is a subclass of another one.\n\n"
        ":param WBEMConnection ch: CIMOM handle\n"
        ":param string ns: namespace where to look for :py:class:`.CIMClass`-es\n"
        ":param string superclass: super class name\n"
        ":param subclass: either string containing sub class name of\n"
        "\t:py:class:`.CIMClass` instance");
    def("is_error",
        is_error,
        "Checks, if the input value equals to a CIM or connection error code.\n\n"
        ":param int value: integer to check\n"
        ":returns: True, if value equals to a error code; False otherwise");

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
    Config::init_type();
    CIMInstance::init_type();
    CIMInstanceName::init_type();
    CIMMethod::init_type();
    CIMParameter::init_type();
    CIMProperty::init_type();
    CIMQualifier::init_type();
    CIMClass::init_type();
    CIMClassName::init_type();
    CIMConstants::init_type();
#  ifdef HAVE_PEGASUS_LISTENER
    CIMIndicationListener::init_type();
#  endif // HAVE_PEGASUS_LISTENER
#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    CIMEnumerationContext::init_type();
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT
#  ifdef HAVE_SLP
    SLP::init_type();
    SLPResult::init_type();
#  endif // HAVE_SLP
}
