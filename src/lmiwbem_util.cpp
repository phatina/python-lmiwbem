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
#include <algorithm>
#include <cstring>
#include <boost/python/borrowed.hpp>
#include <boost/python/handle.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include "lmiwbem.h"
#include "lmiwbem_class.h"
#include "lmiwbem_connection.h"
#include "lmiwbem_constants.h"
#include "lmiwbem_convert.h"
#include "lmiwbem_util.h"

// Define a Python's equivalent of None
const bp::object None;

bp::object incref(const bp::object &obj)
{
    bp::incref(obj.ptr());
    return obj;
}

bp::object this_module()
{
    return bp::object(bp::handle<>(bp::borrowed(PyImport_AddModule(PACKAGE_NAME))));
}

bool isnone(const bp::object &obj)
{
#ifdef BOOST_PYTHON_OBJECT_HAS_IS_NONE
    return obj.is_none();
#else
    // Older boost implementations lack api::object_base::is_none(). We need
    // to check for Py_None by empty boost::python::object().
    return obj == bp::object();
#endif // BOOST_PYTHON_OBJECT_HAS_IS_NONE
}

bool isinstance(const bp::object &inst, const bp::object &cls)
{
    return static_cast<bool>(PyObject_IsInstance(inst.ptr(), cls.ptr()));
}

#  if PY_MAJOR_VERSION < 3
bool isstring(const bp::object &obj)
{
    return static_cast<bool>(PyString_Check(obj.ptr()));
}
#  endif // PY_MAJOR_VERSION

bool isunicode(const bp::object &obj)
{
    return static_cast<bool>(PyUnicode_Check(obj.ptr()));
}

bool isbasestring(const bp::object &obj)
{
#  if PY_MAJOR_VERSION < 3
    return isstring(obj) || isunicode(obj);
#  else
    return isunicode(obj);
#  endif // PY_MAJOR_VERSION
}

bool isbool(const bp::object &obj)
{
    return static_cast<bool>(PyBool_Check(obj.ptr()));
}

#  if PY_MAJOR_VERSION < 3
bool isint(const bp::object &obj)
{
    return static_cast<bool>(PyInt_Check(obj.ptr()));
}
#  endif // PY_MAJOR_VERSION

bool islong(const bp::object &obj)
{
    return static_cast<bool>(PyLong_Check(obj.ptr()));
}

bool isfloat(const bp::object &obj)
{
    return static_cast<bool>(PyFloat_Check(obj.ptr()));
}

bool isdict(const bp::object &obj)
{
    return PyDict_Check(obj.ptr());
}

bool islist(const bp::object &obj)
{
    return PyList_Check(obj.ptr());
}

bool istuple(const bp::object &obj)
{
    return PyTuple_Check(obj.ptr());
}

bool iscallable(const bp::object &obj)
{
    return static_cast<bool>(PyCallable_Check(obj.ptr()));
}

bool cim_issubclass(
    const bp::object &ch,
    const bp::object &ns,
    const bp::object &superclass,
    const bp::object &subclass)
{
    WBEMConnection &conn = WBEMConnection::asNative(ch, "ch");
    std::string std_ns = StringConv::asStdString(ns, "ns");
    std::string std_superclass = StringConv::asStdString(
        superclass, "superclass");

    std::string std_subclass;
    std::string std_subsuperclass;
    std::string std_lsubclass;
    std::string std_lsuperclass(std_superclass);
    std::transform(std_lsuperclass.begin(), std_lsuperclass.end(),
        std_lsuperclass.begin(), ::tolower);

    if (isinstance(subclass, CIMClass::type())) {
        const CIMClass &cim_subclass = CIMClass::asNative(subclass);
        std_subclass = cim_subclass.getClassname();
        std_subsuperclass = cim_subclass.getSuperClassname();
    } else {
        std_subclass = StringConv::asStdString(subclass, "subclass");
    }

    while (1) {
        // Matching is case insensitive.
        std_lsubclass = std_subclass;
        std::transform(std_lsubclass.begin(), std_lsubclass.end(),
            std_lsubclass.begin(), ::tolower);

        if (std_lsubclass == std_lsuperclass) {
            // Do subclass and superclass match?
            return true;
        } else if (std_subsuperclass.empty()) {
            // Get minimal subclass.
            bp::object cls = conn.getClass(bp::str(std_subclass.c_str()),
                bp::str(std_ns.c_str()), true, false, false, bp::list());

            const CIMClass &cim_subclass = CIMClass::asNative(cls);
            std_subsuperclass = cim_subclass.getSuperClassname();
        }

        if (std_subsuperclass.empty()) {
            // We got a CIMClass without super class.
            return false;
        }

        std_subclass = std_subsuperclass;
        std_subsuperclass.clear();
    }

    return false;
}

bool is_error(const bp::object &value)
{
    int ivalue = Conv::as<int>(value, "value");

    switch (ivalue) {
    case CIMConstants::CIM_ERR_FAILED:
    case CIMConstants::CIM_ERR_ACCESS_DENIED:
    case CIMConstants::CIM_ERR_INVALID_NAMESPACE:
    case CIMConstants::CIM_ERR_INVALID_PARAMETER:
    case CIMConstants::CIM_ERR_INVALID_CLASS:
    case CIMConstants::CIM_ERR_NOT_FOUND:
    case CIMConstants::CIM_ERR_NOT_SUPPORTED:
    case CIMConstants::CIM_ERR_CLASS_HAS_CHILDREN:
    case CIMConstants::CIM_ERR_CLASS_HAS_INSTANCES:
    case CIMConstants::CIM_ERR_INVALID_SUPERCLASS:
    case CIMConstants::CIM_ERR_ALREADY_EXISTS:
    case CIMConstants::CIM_ERR_NO_SUCH_PROPERTY:
    case CIMConstants::CIM_ERR_TYPE_MISMATCH:
    case CIMConstants::CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
    case CIMConstants::CIM_ERR_INVALID_QUERY:
    case CIMConstants::CIM_ERR_METHOD_NOT_AVAILABLE:
    case CIMConstants::CIM_ERR_METHOD_NOT_FOUND:
    case CIMConstants::CON_ERR_OTHER:
    case CIMConstants::CON_ERR_ALREADY_CONNECTED:
    case CIMConstants::CON_ERR_NOT_CONNECTED:
    case CIMConstants::CON_ERR_INVALID_LOCATOR:
    case CIMConstants::CON_ERR_CANNOT_CREATE_SOCKET:
    case CIMConstants::CON_ERR_CANNOT_CONNECT:
    case CIMConstants::CON_ERR_CONNECTION_TIMEOUT:
    case CIMConstants::CON_ERR_SSL_EXCEPTION:
    case CIMConstants::CON_ERR_BIND:
#ifdef HAVE_SLP
    case CIMConstants::SLP_ERR_LANGUAGE_NOT_SUPPORTED:
    case CIMConstants::SLP_ERR_PARSE_ERROR:
    case CIMConstants::SLP_ERR_INVALID_REGISTRATION:
    case CIMConstants::SLP_ERR_SCOPE_NOT_SUPPORTED:
    case CIMConstants::SLP_ERR_AUTHENTICATION_ABSENT:
    case CIMConstants::SLP_ERR_AUTHENTICATION_FAILED:
    case CIMConstants::SLP_ERR_INVALID_UPDATE:
    case CIMConstants::SLP_ERR_REFRESH_REJECTED:
    case CIMConstants::SLP_ERR_NOT_IMPLEMENTED:
    case CIMConstants::SLP_ERR_BUFFER_OVERFLOW:
    case CIMConstants::SLP_ERR_NETWORK_TIMED_OUT:
    case CIMConstants::SLP_ERR_NETWORK_INIT_FAILED:
    case CIMConstants::SLP_ERR_MEMORY_ALLOC_FAILED:
    case CIMConstants::SLP_ERR_PARAMETER_BAD:
    case CIMConstants::SLP_ERR_NETWORK_ERROR:
    case CIMConstants::SLP_ERR_INTERNAL_SYSTEM_ERROR:
    case CIMConstants::SLP_ERR_HANDLE_IN_USE:
    case CIMConstants::SLP_ERR_TYPE_ERROR:
#  if !defined(UNICAST_NOT_SUPPORTED) && defined(SLP_RETRY_UNICAST)
    case CIMConstants::SLP_ERR_RETRY_UNICAST:
#  endif // UNICAST_NOT_SUPPORTED
#endif // HAVE_SLP
        return true;
    default:
        return false;
    }
}

#  if PY_MAJOR_VERSION < 3
int compare(const bp::object &o1, const bp::object &o2)
{
    int result = 1;
    PyObject_Cmp(o1.ptr(), o2.ptr(), &result);
    return result;
}
#  else
bool compare(const bp::object &o1, const bp::object &o2, int cmp_type)
{
    int rval = PyObject_RichCompareBool(o1.ptr(), o2.ptr(), cmp_type);
    if (rval == -1)
        throw bp::error_already_set();
    return static_cast<bool>(rval);
}
#  endif // PY_MAJOR_VERSION
