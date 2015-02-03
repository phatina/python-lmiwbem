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
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include <Pegasus/Common/CIMValue.h>
#include <Pegasus/Common/String.h>
#include "obj/cim/lmiwbem_class.h"
#include "obj/cim/lmiwbem_class_name.h"
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_types.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

boost::shared_ptr<CIMTypeConv::CIMTypeHolder> CIMTypeConv::CIMTypeHolder::s_instance;

namespace Conv {

namespace detail {

extract<String>::extract(const bp::object &obj)
    : m_good(true)
    , m_str()
{
    if (isunicode(obj)) {
#  if PY_MAJOR_VERSION < 3
        m_str = String(
            PyString_AsString(
                PyUnicode_EncodeUTF8(
                    PyUnicode_AsUnicode(obj.ptr()),
                    PyUnicode_GetSize(obj.ptr()),
                    NULL)));
    } else if (isstring(obj)) {
        m_str = String(PyString_AsString(obj.ptr()));
#  else
        m_str = String(PyUnicode_AsUTF8(obj.ptr()));
#  endif // PY_MAJOR_VERSION
    } else {
        m_good = false;
    }
}

} // namespace detail

} // namespace Conv

String CIMTypeConv::asString(const bp::object &obj)
{
    if (isnone(obj))
        return String();

    bool is_array = isarray(obj);
    if (is_array && !bp::len(obj))
        return String();

    bp::object py_value_type_check = is_array ? obj[0] : obj;

    if (isinstance(py_value_type_check, CIMType::type()))
        return StringConv::asString(py_value_type_check.attr("cimtype"));
    else if (isinstance(py_value_type_check, CIMInstance::type()))
        return String("string"); // XXX: instance?
    else if (isinstance(py_value_type_check, CIMClass::type()))
        return String("object");
    else if (isinstance(py_value_type_check, CIMInstanceName::type()))
        return String("reference");
    else if (isinstance(py_value_type_check, CIMClassName::type()))
        throw_TypeError("CIMClassName: Unsupported TOG-Pegasus type");
    else if (isbasestring(py_value_type_check))
        return String("string");
    else if (isbool(py_value_type_check))
        return String("boolean");
    // CIM types for numeric values can't be easily determined.

    return String();
}

String CIMTypeConv::asString(Pegasus::CIMType type)
{
    return CIMTypeConv::CIMTypeHolder::instance()->get(type);
}

Pegasus::CIMType CIMTypeConv::asCIMType(const String &type)
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

String CIMTypeConv::CIMTypeHolder::get(Pegasus::CIMType type)
{
    if (m_type_string.find(type) == m_type_string.end())
        return String();
    return m_type_string[type];
}

Pegasus::CIMType CIMTypeConv::CIMTypeHolder::get(const String &type)
{
    return m_string_type[type];
}

ListConv::PyFunctor::PyFunctor()
    : m_ns()
    , m_hostname()
{
}

ListConv::PyFunctor::PyFunctor(
    const String &ns,
    const String &hostname)
    : m_ns(ns)
    , m_hostname(hostname)
{
}

ListConv::PyFunctorCIMInstance::PyFunctorCIMInstance(
    const String &ns,
    const String &hostname)
    : ListConv::PyFunctor(ns, hostname)
{
}

bp::object ListConv::PyFunctorCIMInstance::operator()(
    Pegasus::CIMInstance instance) const
{
    if (!m_ns.empty())
        CIMInstance::updatePegasusCIMInstanceNamespace(instance, m_ns);
    if (!m_hostname.empty())
        CIMInstance::updatePegasusCIMInstanceHostname(instance, m_hostname);
    return CIMInstance::create(instance);
}

bp::object ListConv::PyFunctorCIMInstance::operator()(
    const Pegasus::CIMObject object) const
{
    Pegasus::CIMInstance peg_instance(object);
    return (*this)(peg_instance);
}

ListConv::PyFunctorCIMInstanceName::PyFunctorCIMInstanceName(
    const String &ns,
    const String &hostname)
    : ListConv::PyFunctor(ns, hostname)
{
}

bp::object ListConv::PyFunctorCIMInstanceName::operator()(
    const Pegasus::CIMObjectPath &instance_name) const
{
    return CIMInstanceName::create(instance_name, m_ns, m_hostname);
}

bp::object ListConv::PyFunctorCIMClass::operator()(
    const Pegasus::CIMClass &class_) const
{
    return CIMClass::create(class_);
}

Pegasus::CIMPropertyList ListConv::asPegasusPropertyList(
    const bp::object &property_list,
    const String &message)
{
    Pegasus::CIMPropertyList peg_property_list;

    if (!isnone(property_list)) {
        bp::list py_property_list(Conv::get<bp::list>(property_list, message));
        const int cnt = bp::len(py_property_list);
        Pegasus::Array<Pegasus::CIMName> property_arr(cnt);
        for (int i = 0; i < cnt; ++i) {
            String c_property = StringConv::asString(py_property_list[i]);
            property_arr[i] = Pegasus::CIMName(c_property);
        }
        peg_property_list.set(property_arr);
    }

    return peg_property_list;
}

bp::object ListConv::asPyCIMInstanceList(
    const Pegasus::Array<Pegasus::CIMInstance> &arr,
    const String &ns,
    const String &hostname)
{
    return asPyListCore<Pegasus::CIMInstance>(
        arr,
        PyFunctorCIMInstance(ns, hostname));
}

bp::object ListConv::asPyCIMInstanceList(
    const Pegasus::Array<Pegasus::CIMObject> &arr,
    const String &ns,
    const String &hostname)
{
    return asPyListCore<Pegasus::CIMObject>(
        arr,
        PyFunctorCIMInstance(ns, hostname));
}

bp::object ListConv::asPyCIMInstanceNameList(
    const Pegasus::Array<Pegasus::CIMObjectPath> &arr,
    const String &ns,
    const String &hostname)
{
    return asPyListCore<Pegasus::CIMObjectPath, PyFunctorCIMInstanceName>(
        arr, PyFunctorCIMInstanceName(ns, hostname));
}

bp::object ListConv::asPyCIMClassList(
    const Pegasus::Array<Pegasus::CIMClass> &arr)
{
    return asPyListCore<Pegasus::CIMClass, PyFunctorCIMClass>(
        arr,
        PyFunctorCIMClass());
}

String ObjectConv::asString(const bp::object &obj)
{
    PyObject *py_str_ptr = PyObject_Str(obj.ptr());
    if (!py_str_ptr)
        return String();
#  if PY_MAJOR_VERSION < 3
    return String(PyString_AsString(py_str_ptr));
#  else
    return String(PyUnicode_AsUTF8(py_str_ptr));
#  endif // PY_MAJOR_VERSION
}

bp::object ObjectConv::asPyUnicode(const bp::object &obj)
{
    return bp::object(bp::handle<>(PyObject_Unicode(obj.ptr())));
}

String StringConv::asString(const bp::object &obj)
{
    return Conv::as<String>(obj);
}

String StringConv::asString(
    const bp::object &obj,
    const String &member)
{
    return Conv::as<String>(obj, member);
}

Pegasus::String StringConv::asPegasusString(const bp::object &obj)
{
    return Pegasus::String(Conv::as<const char*>(obj));
}

Pegasus::String StringConv::asPegasusString(
    const bp::object &obj,
    const String &member)
{
    return Pegasus::String(Conv::as<const char*>(obj, member));
}

bp::object StringConv::asPyUnicode(const char *str)
{
    return bp::object(bp::handle<>(PyUnicode_FromString(str)));
}

bp::object StringConv::asPyUnicode(const String &str)
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

bp::object StringConv::asPyBool(const String &str)
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
    bp::object py_int(bp::handle<>(PyInt_FromString(s, NULL, 10)));
    free(static_cast<void*>(s));
    return py_int;
}

bp::object StringConv::asPyInt(const String &str)
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

bp::object StringConv::asPyFloat(const String &str)
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
    bp::object py_long(bp::handle<>(PyLong_FromString(s, NULL, 10)));
    free(static_cast<void*>(s));
    return py_long;
}

bp::object StringConv::asPyLong(const String &str)
{
    return asPyLong(str.c_str());
}

bp::object StringConv::asPyLong(const Pegasus::String &str)
{
    return asPyLong(str.getCString());
}

DEFINE_TO_CONVERTER(StringToPythonString, String)
{
    return bp::incref(StringConv::asPyUnicode(value).ptr());
}

DEFINE_TO_CONVERTER(PegasusStringToPythonString, Pegasus::String)
{
    return bp::incref(
        StringConv::asPyUnicode(
            String(value.getCString())).ptr());
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
