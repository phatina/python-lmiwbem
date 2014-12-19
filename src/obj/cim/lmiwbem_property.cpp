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
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMValue.h>
#include "lmiwbem_exception.h"
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_property.h"
#include "obj/cim/lmiwbem_qualifier.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

CIMProperty::CIMProperty()
    : m_name()
    , m_type()
    , m_class_origin()
    , m_reference_class()
    , m_is_array(false)
    , m_is_propagated(false)
    , m_array_size(-1)
    , m_value()
    , m_qualifiers()
    , m_rc_prop_value()
    , m_rc_prop_qualifiers()
{
}

CIMProperty::CIMProperty(
    const bp::object &name,
    const bp::object &value,
    const bp::object &type,
    const bp::object &class_origin,
    const bp::object &array_size,
    const bp::object &propagated,
    const bp::object &qualifiers,
    const bp::object &is_array,
    const bp::object &reference_class)
{
    m_name = StringConv::asString(name, "name");
    if (!isnone(type)) {
        m_type = StringConv::asString(type, "type");
        m_is_array = isnone(is_array) ?
            static_cast<bool>(PyList_Check(value.ptr())) :
            Conv::as<bool>(is_array, "is_array");
        m_array_size = Conv::as<int>(array_size, "array_size");
    } else {
        // Deduce the value type
        String value_type(CIMTypeConv::asString(value));
        if (!value_type.empty())
            m_type = value_type;
        m_is_array = static_cast<bool>(isarray(value));
        m_array_size = m_is_array ? bp::len(value) : 0;
    }
    m_class_origin = StringConv::asString(class_origin, "class_origin");
    m_reference_class = StringConv::asString(reference_class, "reference_class");
    m_is_propagated = Conv::as<bool>(propagated, "propagated");
    m_value = value;
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(qualifiers, "qualifiers");
}

void CIMProperty::init_type()
{
    CIMBase<CIMProperty>::init_type(bp::class_<CIMProperty>("CIMProperty", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("name"),
                bp::arg("value"),
                bp::arg("type") = None,
                bp::arg("class_origin") = String(),
                bp::arg("array_size") = 0,
                bp::arg("propagated") = false,
                bp::arg("qualifiers") = NocaseDict::create(),
                bp::arg("is_array") = None,
                bp::arg("reference_class") = String()),
                "Property of a CIM object.\n\n"
                ":param str name: String containing the property's name\n"
                ":param value: Property's value\n"
                ":param str type: String containing the property's type\n"
                ":param str class_origin: String containing property's class origin\n"
                ":param int array_size: Array size\n"
                ":param bool propagated: True, if the property is propagated;\n"
                "\tFalse otherwise"
                ":param NocaseDict qualifiers: Dictionary containing propert's qualifiers\n"
                ":param bool is_array: True, if the property's value is array;\n"
                "\tFalse otherwise"
                ":param str reference_class: String containing property's reference class"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMProperty::cmp)
#  else
        .def("__eq__", &CIMProperty::eq)
        .def("__gt__", &CIMProperty::gt)
        .def("__lt__", &CIMProperty::lt)
        .def("__ge__", &CIMProperty::ge)
        .def("__le__", &CIMProperty::le)
#  endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMProperty::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMProperty::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMProperty`")
        .add_property("name",
            &CIMProperty::getPyName,
            &CIMProperty::setPyName,
            "Property storing name of the property.\n\n"
            ":rtype: unicode")
        .add_property("value",
            &CIMProperty::getPyValue,
            &CIMProperty::setPyValue,
            "Property storing value of the property.\n\n"
            ":returns: property's value")
        .add_property("type",
            &CIMProperty::getPyType,
            &CIMProperty::setPyType,
            "Property storing type of the property.\n\n"
            ":rtype: unicode")
        .add_property("class_origin",
            &CIMProperty::getPyClassOrigin,
            &CIMProperty::setPyClassOrigin,
            "Property storing class origin of the property.\n\n"
            ":rtype: unicode")
        .add_property("array_size",
            &CIMProperty::getPyArraySize,
            &CIMProperty::setPyArraySize,
            "Property storing array size of the property.\n\n"
            ":rtype: int")
        .add_property("propagated",
            &CIMProperty::getPyIsPropagated,
            &CIMProperty::setPyIsPropagated,
            "Property storing propagation flag of the property.\n\n"
            ":rtype: bool")
        .add_property("qualifiers",
            &CIMProperty::getPyQualifiers,
            &CIMProperty::setPyQualifiers,
            "Property storing qualifiers of the property.\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("is_array",
            &CIMProperty::getPyIsArray,
            &CIMProperty::setPyIsArray,
            "Property storing flag, which indicates, if the property's value is\n"
            "\tarray.\n\n"
            ":rtype: bool")
        .add_property("reference_class",
            &CIMProperty::getPyReferenceClass,
            &CIMProperty::setPyReferenceClass,
            "Property storing reference class of the property.\n\n"
            ":rtype: unicode"));
}

bp::object CIMProperty::create(const Pegasus::CIMConstProperty &property)
{
    bp::object py_inst = CIMBase<CIMProperty>::create();
    CIMProperty &fake_this = CIMProperty::asNative(py_inst);
    fake_this.m_name = property.getName().getString();
    fake_this.m_type = CIMTypeConv::asString(property.getType());
    fake_this.m_class_origin = property.getClassOrigin().getString();
    fake_this.m_array_size = static_cast<int>(property.getArraySize());
    fake_this.m_is_propagated = property.getPropagated();
    fake_this.m_is_array = property.isArray();
    fake_this.m_reference_class = property.getReferenceClassName().getString();

    // Store value for lazy evaluation
    fake_this.m_rc_prop_value.set(property.getValue());

    // Store qualifiers for lazy evaluation
    fake_this.m_rc_prop_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    const Pegasus::Uint32 cnt = property.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_prop_qualifiers.get()->push_back(property.getQualifier(i));
    return py_inst;
}

bp::object CIMProperty::create(
    const bp::object &name,
    const bp::object &value)
{
    return CIMBase<CIMProperty>::create(name, value);
}

Pegasus::CIMProperty CIMProperty::asPegasusCIMProperty() try
{
    Pegasus::CIMValue peg_value(CIMValue::asPegasusCIMValue(getPyValue(), m_type));

    return Pegasus::CIMProperty(
        Pegasus::CIMName(m_name),
        peg_value,
        peg_value.isNull() ? 0 : static_cast<Pegasus::Uint32>(m_array_size),
        m_reference_class.empty() ? Pegasus::CIMName() : Pegasus::CIMName(m_reference_class),
        m_class_origin.empty() ? Pegasus::CIMName() : Pegasus::CIMName(m_class_origin),
        m_is_propagated);
} catch (const Pegasus::TypeMismatchException &e) {
    String msg(m_name);
    msg += ": ";
    msg += e.getMessage();
    throw Pegasus::TypeMismatchException(msg);
    return Pegasus::CIMProperty();
}

#  if PY_MAJOR_VERSION < 3
int CIMProperty::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return 1;

    CIMProperty &cim_other = CIMProperty::asNative(other);

    int rval;
    if ((rval = m_name.compare(cim_other.m_name)) != 0 ||
        (rval = m_type.compare(cim_other.m_type)) != 0 ||
        (rval = m_class_origin.compare(cim_other.m_class_origin)) != 0 ||
        (rval = m_reference_class.compare(cim_other.m_reference_class)) != 0 ||
        (rval = compare(bp::object(m_is_array),
            bp::object(cim_other.m_is_array))) != 0 ||
        (rval = compare(bp::object(m_is_propagated),
            bp::object(cim_other.m_is_propagated))) != 0 ||
        (rval = compare(bp::object(m_array_size),
            bp::object(cim_other.m_array_size))) != 0 ||
        (rval = compare(getPyValue(), cim_other.getPyValue())) != 0 ||
        (rval = compare(getPyQualifiers(), cim_other.getPyQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMProperty::eq(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return false;

    CIMProperty &cim_other = CIMProperty::asNative(other);

    return m_name == cim_other.m_name &&
        m_type == cim_other.m_type &&
        m_class_origin == cim_other.m_class_origin &&
        m_reference_class == cim_other.m_reference_class &&
        m_is_array == cim_other.m_is_array &&
        m_is_propagated == cim_other.m_is_propagated &&
        m_array_size == cim_other.m_array_size &&
        compare(getValue(), cim_other.getValue(), Py_EQ) &&
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_EQ);
}

bool CIMProperty::gt(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return false;

    CIMProperty &cim_other = CIMProperty::asNative(other);

    return m_name > cim_other.m_name ||
        m_type > cim_other.m_type ||
        m_class_origin > cim_other.m_class_origin ||
        m_reference_class > cim_other.m_reference_class ||
        m_is_array > cim_other.m_is_array ||
        m_is_propagated > cim_other.m_is_propagated ||
        m_array_size > cim_other.m_array_size ||
        compare(getValue(), cim_other.getValue(), Py_GT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_GT);
}

bool CIMProperty::lt(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return false;

    CIMProperty &cim_other = CIMProperty::asNative(other);

    return m_name < cim_other.m_name ||
        m_type < cim_other.m_type ||
        m_class_origin < cim_other.m_class_origin ||
        m_reference_class < cim_other.m_reference_class ||
        m_is_array < cim_other.m_is_array ||
        m_is_propagated < cim_other.m_is_propagated ||
        m_array_size < cim_other.m_array_size ||
        compare(getValue(), cim_other.getValue(), Py_LT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_LT);
}

bool CIMProperty::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMProperty::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

bp::object CIMProperty::repr()
{
    std::stringstream ss;
    ss << "CIMProperty(name=u'" << m_name
        << "', type=u'" << m_type
        << "', value='" << ObjectConv::asString(getPyValue())
        << "', is_array=" << (m_is_array ? "True" : "False") << ", ...)";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMProperty::copy()
{
    bp::object py_inst = CIMBase<CIMProperty>::create();
    CIMProperty &cim_property = CIMProperty::asNative(py_inst);
    NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());

    cim_property.m_name = m_name;
    cim_property.m_type = m_type;
    cim_property.m_class_origin = m_class_origin;
    cim_property.m_reference_class = m_reference_class;
    cim_property.m_is_array = m_is_array;
    cim_property.m_is_propagated = m_is_propagated;
    cim_property.m_array_size = m_array_size;
    cim_property.m_value = m_value;
    cim_property.m_qualifiers = cim_qualifiers.copy();

    return py_inst;
}

String CIMProperty::getName() const
{
    return m_name;
}

String CIMProperty::getType() const
{
    return m_name;
}

String CIMProperty::getClassOrigin() const
{
    return m_class_origin;
}

String CIMProperty::getReferenceClass() const
{
    return m_reference_class;
}

int CIMProperty::getArraySize() const
{
    return m_array_size;
}

bool CIMProperty::getIsArray() const
{
    return m_is_array;
}

bool CIMProperty::getIsPropagated() const
{
    return m_is_propagated;
}

bp::object CIMProperty::getPyName() const
{
    return StringConv::asPyUnicode(m_name);
}

bp::object CIMProperty::getPyType() const
{
    return StringConv::asPyUnicode(m_type);
}

bp::object CIMProperty::getPyClassOrigin() const
{
    return StringConv::asPyUnicode(m_class_origin);
}

bp::object CIMProperty::getPyReferenceClass() const
{
    return StringConv::asPyUnicode(m_reference_class);
}

bp::object CIMProperty::getPyArraySize() const
{
    return bp::object(m_array_size);
}

bp::object CIMProperty::getPyIsArray() const
{
    return bp::object(m_is_array);
}

bp::object CIMProperty::getPyIsPropagated() const
{
    return bp::object(m_is_propagated);
}

bp::object CIMProperty::getPyValue()
{
    if (!m_rc_prop_value.empty()) {
        m_value = CIMValue::asLMIWbemCIMValue(*m_rc_prop_value.get());
        m_rc_prop_value.release();
    }

    return m_value;
}

bp::object CIMProperty::getPyQualifiers()
{
    if (!m_rc_prop_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = m_rc_prop_qualifiers.get()->begin();
             it != m_rc_prop_qualifiers.get()->end(); ++it)
        {
            m_qualifiers[bp::object(it->getName())] = CIMQualifier::create(*it);
        }

        m_rc_prop_qualifiers.release();
    }

    return m_qualifiers;
}

void CIMProperty::setName(const String &name)
{
    m_name = name;
}

void CIMProperty::setType(const String &type)
{
    m_type = type;
}

void CIMProperty::setClassOrigin(const String &class_origin)
{
    m_class_origin = class_origin;
}

void CIMProperty::setReferenceClass(const String &reference_class)
{
    m_reference_class = reference_class;
}

void CIMProperty::setArraySize(int array_size)
{
    m_array_size = array_size;
}

void CIMProperty::setIsArray(bool is_array)
{
    m_is_array = is_array;
}

void CIMProperty::setIsPropagated(bool is_propagated)
{
    m_is_propagated = is_propagated;
}

void CIMProperty::setPyName(const bp::object &name)
{
    m_name = StringConv::asString(name, "name");
}

void CIMProperty::setPyType(const bp::object &type)
{
    m_type = StringConv::asString(type, "type");
}

void CIMProperty::setPyValue(const bp::object &value)
{
    m_value = value;

    // Unref cached resource, it will never be used
    m_rc_prop_value.release();
}

void CIMProperty::setPyClassOrigin(const bp::object &class_origin)
{
    m_class_origin = StringConv::asString(class_origin, "class_origin");
}

void CIMProperty::setPyReferenceClass(const bp::object &reference_class)
{
    m_reference_class = StringConv::asString(
        reference_class, "reference_class");
}

void CIMProperty::setPyArraySize(const bp::object &array_size)
{
    m_array_size = Conv::as<int>(array_size, "array_size");
}

void CIMProperty::setPyIsArray(const bp::object &is_array)
{
    m_is_array = Conv::as<bool>(is_array, "is_array");
}

void CIMProperty::setPyIsPropagated(const bp::object &propagated)
{
    m_is_propagated = Conv::as<bool>(propagated, "propagated");
}

void CIMProperty::setPyQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_prop_qualifiers.release();
}
