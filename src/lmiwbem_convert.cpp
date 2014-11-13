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
#include <Pegasus/Common/CIMType.h>
#  include <Pegasus/Common/Char16.h>
#  include <Pegasus/Common/CIMDateTime.h>
#  include <Pegasus/Common/CIMName.h>
#  include <Pegasus/Common/CIMObjectPath.h>
#  include <Pegasus/Common/CIMPropertyList.h>
#  include <Pegasus/Common/CIMValue.h>
#  include <Pegasus/Common/String.h>
#include "lmiwbem_class.h"
#include "lmiwbem_class_name.h"
#include "lmiwbem_convert.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_types.h"
#include "lmiwbem_util.h"

boost::shared_ptr<CIMTypeConv::CIMTypeHolder> CIMTypeConv::CIMTypeHolder::s_instance;

namespace Conv {

namespace detail {

extract<std::string>::extract(const bp::object &obj)
    : m_good(true)
    , m_str()
{
    if (isunicode(obj)) {
#  if PY_MAJOR_VERSION < 3
        m_str = std::string(
            PyString_AsString(
                PyUnicode_EncodeUTF8(
                    PyUnicode_AsUnicode(obj.ptr()),
                    PyUnicode_GetSize(obj.ptr()),
                    NULL)));
    } else if (isstring(obj)) {
        m_str = std::string(PyString_AsString(obj.ptr()));
#  else
        m_str = std::string(PyUnicode_AsUTF8(obj.ptr()));
#  endif // PY_MAJOR_VERSION
    } else {
        m_good = false;
    }
}

} // namespace detail

} // namespace Conv

std::string CIMTypeConv::asStdString(const bp::object &obj)
{
    if (isnone(obj))
        return std::string();

    bool is_array = isarray(obj);
    if (is_array && !bp::len(obj))
        return std::string();

    bp::object value_type_check = is_array ? obj[0] : obj;

    if (isinstance(value_type_check, CIMType::type()))
        return StringConv::asStdString(value_type_check.attr("cimtype"));
    else if (isinstance(value_type_check, CIMInstance::type()))
        return std::string("string"); // XXX: instance?
    else if (isinstance(value_type_check, CIMClass::type()))
        return std::string("object");
    else if (isinstance(value_type_check, CIMInstanceName::type()))
        return std::string("reference");
    else if (isinstance(value_type_check, CIMClassName::type()))
        throw_TypeError("CIMClassName: Unsupported TOG-Pegasus type");
    else if (isbasestring(value_type_check))
        return std::string("string");
    else if (isbool(value_type_check))
        return std::string("boolean");
    // CIM types for numeric values can't be easily determined.

    return std::string();
}

std::string CIMTypeConv::asStdString(Pegasus::CIMType type)
{
    return CIMTypeConv::CIMTypeHolder::instance()->get(type);
}

Pegasus::CIMType CIMTypeConv::asCIMType(const std::string &type)
{
    return CIMTypeConv::CIMTypeHolder::instance()->get(type);
}

CIMTypeConv::CIMTypeHolder *CIMTypeConv::CIMTypeHolder::instance()
{
    if (!s_instance)
        s_instance.reset(new CIMTypeHolder());
    return s_instance.get();
}

CIMTypeConv::CIMTypeHolder::CIMTypeHolder()
{
    m_type_string[Pegasus::CIMTYPE_BOOLEAN] = "boolean";
    m_type_string[Pegasus::CIMTYPE_UINT8] = "uint8";
    m_type_string[Pegasus::CIMTYPE_SINT8] = "sint8";
    m_type_string[Pegasus::CIMTYPE_UINT16] = "uint16";
    m_type_string[Pegasus::CIMTYPE_SINT16] = "sint16";
    m_type_string[Pegasus::CIMTYPE_UINT32] = "uint32";
    m_type_string[Pegasus::CIMTYPE_SINT32] = "sint32";
    m_type_string[Pegasus::CIMTYPE_UINT64] = "uint64";
    m_type_string[Pegasus::CIMTYPE_SINT64] = "sint64";
    m_type_string[Pegasus::CIMTYPE_REAL32] = "real32";
    m_type_string[Pegasus::CIMTYPE_REAL64] = "real64";
    m_type_string[Pegasus::CIMTYPE_CHAR16] = "char16";
    m_type_string[Pegasus::CIMTYPE_STRING] = "string";
    m_type_string[Pegasus::CIMTYPE_DATETIME] = "CIMDateTime";
    m_type_string[Pegasus::CIMTYPE_REFERENCE] = "reference";
    m_type_string[Pegasus::CIMTYPE_OBJECT] = "object";
    m_type_string[Pegasus::CIMTYPE_INSTANCE] = "instance";

    m_string_type["bool"] = Pegasus::CIMTYPE_BOOLEAN;
    m_string_type["uint8"] = Pegasus::CIMTYPE_UINT8;
    m_string_type["sint8"] = Pegasus::CIMTYPE_SINT8;
    m_string_type["uint16"] = Pegasus::CIMTYPE_UINT16;
    m_string_type["sint16"] = Pegasus::CIMTYPE_SINT16;
    m_string_type["uint32"] = Pegasus::CIMTYPE_UINT32;
    m_string_type["sint32"] = Pegasus::CIMTYPE_SINT32;
    m_string_type["uint64"] = Pegasus::CIMTYPE_UINT64;
    m_string_type["sint64"] = Pegasus::CIMTYPE_SINT64;
    m_string_type["real32"] = Pegasus::CIMTYPE_REAL32;
    m_string_type["real64"] = Pegasus::CIMTYPE_REAL64;
    m_string_type["char16"] = Pegasus::CIMTYPE_CHAR16;
    m_string_type["string"] = Pegasus::CIMTYPE_STRING;
    m_string_type["CIMDateTime"] = Pegasus::CIMTYPE_DATETIME;
    m_string_type["reference"] = Pegasus::CIMTYPE_REFERENCE;
    m_string_type["object"] = Pegasus::CIMTYPE_OBJECT;
    m_string_type["instance"] = Pegasus::CIMTYPE_INSTANCE;
}

std::string CIMTypeConv::CIMTypeHolder::get(Pegasus::CIMType type)
{
    if (m_type_string.find(type) == m_type_string.end())
        return std::string();
    return m_type_string[type];
}

Pegasus::CIMType CIMTypeConv::CIMTypeHolder::get(const std::string &type)
{
    return m_string_type[type];
}

Pegasus::CIMPropertyList ListConv::asPegasusPropertyList(
    const bp::object &property_list,
    const std::string &message)
{
    Pegasus::CIMPropertyList cim_property_list;

    if (!isnone(property_list)) {
        bp::list py_property_list(Conv::get<bp::list>(property_list, message));
        const int cnt = bp::len(py_property_list);
        Pegasus::Array<Pegasus::CIMName> property_arr(cnt);
        for (int i = 0; i < cnt; ++i) {
            std::string property = StringConv::asStdString(py_property_list[i]);
            property_arr[i] = Pegasus::CIMName(property.c_str());
        }
        cim_property_list.set(property_arr);
    }

    return cim_property_list;
}

std::string ObjectConv::asStdString(const bp::object &obj)
{
    PyObject *str = PyObject_Str(obj.ptr());
    if (!str)
        return std::string();
#  if PY_MAJOR_VERSION < 3
    return std::string(PyString_AsString(str));
#  else
    return std::string(PyUnicode_AsUTF8(str));
#  endif // PY_MAJOR_VERSION
}

bp::object ObjectConv::asPyUnicode(const bp::object &obj)
{
    return bp::object(bp::handle<>(PyObject_Unicode(obj.ptr())));
}

std::string StringConv::asStdString(const bp::object &obj)
{
    return Conv::as<std::string>(obj);
}

std::string StringConv::asStdString(
    const bp::object &obj,
    const std::string &member)
{
    return Conv::as<std::string>(obj, member);
}

bp::object StringConv::asPyUnicode(const char *str)
{
    return bp::object(bp::handle<>(PyUnicode_FromString(str)));
}

bp::object StringConv::asPyUnicode(const std::string &str)
{
    return asPyUnicode(str.c_str());
}

bp::object StringConv::asPyUnicode(const Pegasus::String &str)
{
    return asPyUnicode(str.getCString());
}

bp::object StringConv::asPyBool(const char *str)
{
    long int b = strtol(str, NULL, 10);
    return bp::object(bp::handle<>(PyBool_FromLong(b)));
}

bp::object StringConv::asPyBool(const std::string &str)
{
    return asPyBool(str.c_str());
}

bp::object StringConv::asPyBool(const Pegasus::String &str)
{
    return asPyBool(str.getCString());
}

#  if PY_MAJOR_VERSION < 3
bp::object StringConv::asPyInt(const char *str)
{
    char *s = strdup(str);
    bp::object pyint(bp::handle<>(PyInt_FromString(s, NULL, 10)));
    free(static_cast<void*>(s));
    return pyint;
}

bp::object StringConv::asPyInt(const std::string &str)
{
    return asPyInt(str.c_str());
}

bp::object StringConv::asPyInt(const Pegasus::String &str)
{
    return asPyInt(str.getCString());
}
#  endif // PY_MAJOR_VERSION

bp::object StringConv::asPyFloat(const char *str)
{
    double d = strtod(str, NULL);
    return bp::object(bp::handle<>(PyFloat_FromDouble(d)));
}

bp::object StringConv::asPyFloat(const std::string &str)
{
    return asPyFloat(str.c_str());
}

bp::object StringConv::asPyFloat(const Pegasus::String &str)
{
    return asPyFloat(str.getCString());
}

bp::object StringConv::asPyLong(const char *str)
{
    char *s = strdup(str);
    bp::object pylong(bp::handle<>(PyLong_FromString(s, NULL, 10)));
    free(static_cast<void*>(s));
    return pylong;
}

bp::object StringConv::asPyLong(const std::string &str)
{
    return asPyLong(str.c_str());
}

bp::object StringConv::asPyLong(const Pegasus::String &str)
{
    return asPyLong(str.getCString());
}

DEFINE_TO_CONVERTER(PegasusStringToPythonString, Pegasus::String)
{
    return bp::incref(
        StringConv::asPyUnicode(
            std::string(value.getCString())).ptr());
}

DEFINE_TO_CONVERTER(PegasusCIMNameToPythonString, Pegasus::CIMName)
{
    return PegasusStringToPythonString::convert(value.getString());
}

DEFINE_TO_CONVERTER(PegasusCIMDateteTimeToPythonDateTime, Pegasus::CIMDateTime)
{
    return bp::incref(CIMDateTime::create(bp::object(value.toString())).ptr());
}

DEFINE_TO_CONVERTER(PegasusChar16ToPythonUint16, Pegasus::Char16)
{
    // Pegasus::Char16 stores its data as Uint16
    return bp::incref(Uint16::create(value).ptr());
}

DEFINE_TO_CONVERTER(
    PegasusCIMObjectPathToPythonCIMInstanceName,
    Pegasus::CIMObjectPath)
{
    return bp::incref(CIMInstanceName::create(value).ptr());
}

DEFINE_TO_CONVERTER(PegasusCIMObjectToPythonCIMObject, Pegasus::CIMObject)
{
    if (value.isInstance())
        return bp::incref(CIMInstance::create(value).ptr());
    else
        return bp::incref(CIMClass::create(value).ptr());
}

DEFINE_TO_CONVERTER(CIMConstantsCIMErrorToPythonInt, CIMConstants::CIMError)
{
    return bp::incref(bp::object(static_cast<int>(value)).ptr());
}
