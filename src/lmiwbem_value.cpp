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

#include <boost/python/list.hpp>
#include <boost/python/str.hpp>
#include <boost/python/object_attributes.hpp>
#include "lmiwbem_class.h"
#include "lmiwbem_class_name.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_types.h"
#include "lmiwbem_util.h"
#include "lmiwbem_value.h"

namespace {

template <typename T>
bp::object getPegasusValueCore(const T &value)
{
    return incref(bp::object(value));
}

template <>
bp::object getPegasusValueCore<Pegasus::CIMObject>(const Pegasus::CIMObject &value)
{
    return incref(CIMInstance::create(value));
}

// We could not use to_python converters here.
#define DEF_PEGASUS_VALUE_CORE(type) \
    template<> \
    bp::object getPegasusValueCore(const Pegasus::type &value) \
    { \
        return incref(type::create(value)); \
    }

DEF_PEGASUS_VALUE_CORE(Uint8)
DEF_PEGASUS_VALUE_CORE(Sint8)
DEF_PEGASUS_VALUE_CORE(Uint16)
DEF_PEGASUS_VALUE_CORE(Sint16)
DEF_PEGASUS_VALUE_CORE(Uint32)
DEF_PEGASUS_VALUE_CORE(Sint32)
DEF_PEGASUS_VALUE_CORE(Uint64)
DEF_PEGASUS_VALUE_CORE(Sint64)
DEF_PEGASUS_VALUE_CORE(Real32)
DEF_PEGASUS_VALUE_CORE(Real64)

template <typename T>
bp::object getPegasusValue(const Pegasus::CIMValue &value)
{
    // We return None for every empty CIMValue except of lists,
    // so we can reconstruct CIMValue out of Python's objects.
    if (value.isNull() && !value.isArray())
        return bp::object();

    if (!value.isArray()) {
        T raw_value;
        value.get(raw_value);
        bp::object obj(getPegasusValueCore<T>(raw_value));
        return incref(obj);
    } else {
        bp::list array;
        Pegasus::Array<T> raw_array;
        value.get(raw_array);
        const Pegasus::Uint32 cnt = value.getArraySize();
        for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
            const T &raw_value = raw_array[i];
            array.append(getPegasusValueCore<T>(raw_value));
        }
        return incref(array);
    }
}

template <typename T, typename R>
R setPegasusValueCore(const bp::object &value)
{
    return R(lmi::extract<T>(value));
}

template <>
Pegasus::String setPegasusValueCore<
    Pegasus::CIMDateTime,
    Pegasus::String>(const bp::object &value)
{
    return Pegasus::String(object_as_std_string(value).c_str());
}

template <>
Pegasus::CIMObjectPath setPegasusValueCore<
    Pegasus::CIMObjectPath,
    Pegasus::CIMObjectPath>(const bp::object &value)
{
    const CIMInstanceName &path = lmi::extract<CIMInstanceName&>(value);
    return path.asPegasusCIMObjectPath();
}

template <>
Pegasus::CIMObject setPegasusValueCore<
    Pegasus::CIMClass,
    Pegasus::CIMObject>(const bp::object &value)
{
    CIMClass &cls = lmi::extract<CIMClass&>(value);
    return Pegasus::CIMObject(cls.asPegasusCIMClass());
}

template <>
Pegasus::CIMObject setPegasusValueCore<
    Pegasus::CIMInstance,
    Pegasus::CIMObject>(const bp::object &value)
{
    CIMInstance &instance = lmi::extract<CIMInstance&>(value);
    return Pegasus::CIMObject(instance.asPegasusCIMInstance());
}

template <>
Pegasus::String setPegasusValueCore<
    Pegasus::String,
    Pegasus::String>(const bp::object &value)
{
    std::string std_value = lmi::extract<std::string>(value);
    return Pegasus::String(std_value.c_str());
}

template <typename T, typename R>
Pegasus::CIMValue setPegasusValue(
    const bp::object &value,
    const bool is_array = false)
{
    if (!is_array)
        return Pegasus::CIMValue(setPegasusValueCore<T, R>(value));

    bp::list list(value);
    Pegasus::Array<R> array;
    const int cnt = bp::len(list);
    for (int i = 0; i < cnt; ++i)
        array.append(setPegasusValueCore<T, R>(list[i]));

    return Pegasus::CIMValue(array);
}

// We are compiling under C++98, which does not support template's
// default parameters. So we define the same template function with
// "S" suffix; for shorter code used below.
template <typename T>
Pegasus::CIMValue setPegasusValueS(
    const bp::object &value,
    const bool is_array = false)
{
    return setPegasusValue<T, T>(value, is_array);
}

} // unnamed namespace

bp::object CIMValue::asLMIWbemCIMValue(const Pegasus::CIMValue &value)
{
    switch (value.getType()) {
    case Pegasus::CIMTYPE_BOOLEAN:
        return getPegasusValue<Pegasus::Boolean>(value);
    case Pegasus::CIMTYPE_UINT8:
        return getPegasusValue<Pegasus::Uint8>(value);
    case Pegasus::CIMTYPE_SINT8:
        return getPegasusValue<Pegasus::Sint8>(value);
    case Pegasus::CIMTYPE_UINT16:
        return getPegasusValue<Pegasus::Uint16>(value);
    case Pegasus::CIMTYPE_SINT16:
        return getPegasusValue<Pegasus::Sint16>(value);
    case Pegasus::CIMTYPE_UINT32:
        return getPegasusValue<Pegasus::Uint32>(value);
    case Pegasus::CIMTYPE_SINT32:
        return getPegasusValue<Pegasus::Sint32>(value);
    case Pegasus::CIMTYPE_UINT64:
        return getPegasusValue<Pegasus::Uint64>(value);
    case Pegasus::CIMTYPE_SINT64:
        return getPegasusValue<Pegasus::Sint64>(value);
    case Pegasus::CIMTYPE_CHAR16:
        return getPegasusValue<Pegasus::Char16>(value);
    case Pegasus::CIMTYPE_STRING:
        return getPegasusValue<Pegasus::String>(value);
    case Pegasus::CIMTYPE_DATETIME:
        return getPegasusValue<Pegasus::CIMDateTime>(value);
    case Pegasus::CIMTYPE_REFERENCE:
        return getPegasusValue<Pegasus::CIMObjectPath>(value);
    case Pegasus::CIMTYPE_OBJECT:
        return getPegasusValue<Pegasus::CIMObject>(value);
    default:
        PyErr_SetString(PyExc_TypeError, "Unknown CIMValue type");
        bp::throw_error_already_set();
        return bp::object();
    }
}

Pegasus::CIMValue CIMValue::asPegasusCIMValue(const bp::object &value)
{
    bool is_array = PyList_Check(value.ptr());
    if (value.is_none() || (is_array && bp::len(value) == 0))
        return Pegasus::CIMValue();

    bp::object value_type_check = is_array ? value[0] : value;

    if (isinstance(value_type_check, CIMType::type())) {
        std::string type = lmi::extract<std::string>(value_type_check.attr("cimtype"));
        if (type == "uint8")
            return setPegasusValueS<Pegasus::Uint8>(value, is_array);
        else if (type == "sint8")
            return setPegasusValueS<Pegasus::Sint8>(value, is_array);
        else if (type == "uint16")
            return setPegasusValueS<Pegasus::Uint16>(value, is_array);
        else if (type == "sint16")
            return setPegasusValueS<Pegasus::Sint16>(value, is_array);
        else if (type == "uint32")
            return setPegasusValueS<Pegasus::Uint32>(value, is_array);
        else if (type == "sint32")
            return setPegasusValueS<Pegasus::Sint32>(value, is_array);
        else if (type == "uint64")
            return setPegasusValueS<Pegasus::Uint64>(value, is_array);
        else if (type == "sint64")
            return setPegasusValueS<Pegasus::Sint64>(value, is_array);
        else if (type == "real32")
            return setPegasusValueS<Pegasus::Real32>(value, is_array);
        else if (type == "real64")
            return setPegasusValueS<Pegasus::Real32>(value, is_array);
        else if (type == "datetime")
            return setPegasusValue<Pegasus::CIMDateTime, Pegasus::String>(value, is_array);
    } else if (isinstance(value_type_check, CIMInstance::type())) {
        return setPegasusValue<Pegasus::CIMInstance, Pegasus::CIMObject>(value, is_array);
    } else if (isinstance(value_type_check, CIMClass::type())) {
        return setPegasusValue<Pegasus::CIMClass, Pegasus::CIMObject>(value, is_array);
    } else if (isinstance(value_type_check, CIMInstanceName::type())) {
        return setPegasusValueS<Pegasus::CIMObjectPath>(value, is_array);
    } else if (isinstance(value_type_check, CIMClassName::type())) {
        throw_TypeError("CIMClassName: Unsupported TOG-Pegasus type");
    } else if (isbasestring(value_type_check)) {
        return setPegasusValueS<Pegasus::String>(value, is_array);
    } else if (isbool(value_type_check)) {
        return setPegasusValueS<bool>(value, is_array);
    } else if (isint(value_type_check)) {
        return setPegasusValueS<Pegasus::Sint32>(value, is_array);
    } else if (islong(value_type_check)) {
        return setPegasusValueS<Pegasus::Sint64>(value, is_array);
    } else if (isfloat(value_type_check)) {
        return setPegasusValueS<float>(value, is_array);
    }

    throw_TypeError("CIMValue: Unsupported TOG-Pegasus type");
    return Pegasus::CIMValue();
}

std::string CIMValue::LMIWbemCIMValueType(const bp::object &value)
{
    if (value.is_none())
        return std::string();

    bool is_array = PyList_Check(value.ptr());
    if (is_array && !bp::len(value))
        throw_TypeError("CIMValue: empty array must have a type");

    bp::object value_type_check = is_array ? value[0] : value;

    if (isinstance(value_type_check, CIMType::type()))
        return lmi::extract<std::string>(value_type_check.attr("cimtype"));
    else if (isinstance(value_type_check, CIMInstance::type()))
        return std::string("string"); // XXX: instance?
    else if (isinstance(value_type_check, CIMClass::type()))
        return std::string("object");
    else if (isinstance(value_type_check, CIMInstanceName::type()))
        return std::string("reference");
    else if (isinstance(value_type_check, CIMClassName::type()))
        throw_TypeError("CIMClassName: Unsupported TOG-Pegasus type");
    else if (isbasestring(value))
        return std::string("string");
    else if (isbool(value_type_check))
        return std::string("boolean");

    throw_TypeError("CIMValue: Invalid CIM type");
    return std::string();
}
