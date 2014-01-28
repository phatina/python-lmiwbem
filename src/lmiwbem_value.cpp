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
#include "lmiwbem_class.h"
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
    if (value.isNull())
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

} // unnamed namespace

bp::object CIMValue::create(const Pegasus::CIMValue &value)
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
