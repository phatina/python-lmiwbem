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
#include "lmiwbem_extract.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_property.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"
#include "lmiwbem_value.h"

CIMProperty::CIMProperty()
    : m_name()
    , m_type()
    , m_class_origin()
    , m_reference_class()
    , m_is_array(false)
    , m_propagated(false)
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
    m_name = lmi::extract_or_throw<std::string>(name, "name");
    if (!isnone(type)) {
        m_type = lmi::extract_or_throw<std::string>(type, "type");
        m_is_array = isnone(is_array) ?
            static_cast<bool>(PyList_Check(value.ptr())) :
            lmi::extract_or_throw<bool>(is_array, "is_array");
        m_array_size = lmi::extract_or_throw<int>(array_size, "array_size");
    } else {
        // Deduce the value type
        m_type = CIMValue::LMIWbemCIMValueType(value);
        m_is_array = static_cast<bool>(PyList_Check(value.ptr()));
        m_array_size = m_is_array ? bp::len(value) : 0;
    }
    m_class_origin = lmi::extract_or_throw<std::string>(
        class_origin, "class_origin");
    m_reference_class = lmi::extract_or_throw<std::string>(
        reference_class, "reference_class");
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");
    m_value = value;
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(
        qualifiers, "qualifiers");
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
                bp::arg("type") = bp::object(),
                bp::arg("class_origin") = std::string(),
                bp::arg("array_size") = 0,
                bp::arg("propagated") = false,
                bp::arg("qualifiers") = NocaseDict::create(),
                bp::arg("is_array") = bp::object(),
                bp::arg("reference_class") = std::string()),
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
            &CIMProperty::getName,
            &CIMProperty::setName,
            "Property storing name of the property.\n\n"
            ":rtype: str")
        .add_property("value",
            &CIMProperty::getValue,
            &CIMProperty::setValue,
            "Property storing value of the property.\n\n"
            ":returns: property's value")
        .add_property("type",
            &CIMProperty::getType,
            &CIMProperty::setType,
            "Property storing type of the property.\n\n"
            ":rtype: str")
        .add_property("class_origin",
            &CIMProperty::getClassOrigin,
            &CIMProperty::setClassOrigin,
            "Property storing class origin of the property.\n\n"
            ":rtype: str")
        .add_property("array_size",
            &CIMProperty::getArraySize,
            &CIMProperty::setArraySize,
            "Property storing array size of the property.\n\n"
            ":rtype: int")
        .add_property("propagated",
            &CIMProperty::getPropagated,
            &CIMProperty::setPropagated,
            "Property storing propagation flag of the property.\n\n"
            ":rtype: bool")
        .add_property("qualifiers",
            &CIMProperty::getQualifiers,
            &CIMProperty::setQualifiers,
            "Property storing qualifiers of the property.\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("is_array",
            &CIMProperty::m_is_array,
            "Property storing flag, which indicates, if the property's value is\n"
            "\tarray.\n\n"
            ":rtype: bool")
        .add_property("reference_class",
            &CIMProperty::m_reference_class,
            &CIMProperty::setReferenceClass,
            "Property storing reference class of the property.\n\n"
            ":rtype: str"));
}

bp::object CIMProperty::create(const Pegasus::CIMConstProperty &property)
{
    bp::object inst = CIMBase<CIMProperty>::create();
    CIMProperty &fake_this = lmi::extract<CIMProperty&>(inst);
    fake_this.m_name = property.getName().getString().getCString();
    fake_this.m_type = CIMTypeConv::asStdString(property.getType());
    fake_this.m_class_origin = property.getClassOrigin().getString().getCString();
    fake_this.m_array_size = static_cast<int>(property.getArraySize());
    fake_this.m_propagated = property.getPropagated();
    fake_this.m_is_array = property.isArray();
    fake_this.m_reference_class = property.getReferenceClassName().getString().getCString();

    // Store value for lazy evaluation
    fake_this.m_rc_prop_value.set(property.getValue());

    // Store qualifiers for lazy evaluation
    fake_this.m_rc_prop_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    const Pegasus::Uint32 cnt = property.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_prop_qualifiers.get()->push_back(property.getQualifier(i));
    return inst;
}

bp::object CIMProperty::create(
    const bp::object &name,
    const bp::object &value)
{
    return CIMBase<CIMProperty>::create(name, value);
}

Pegasus::CIMProperty CIMProperty::asPegasusCIMProperty()
{
    Pegasus::CIMValue value = CIMValue::asPegasusCIMValue(getValue());
    return Pegasus::CIMProperty(
        Pegasus::CIMName(m_name.c_str()),
        value,
        value.isNull() ? 0 : static_cast<Pegasus::Uint32>(m_array_size),
        m_reference_class.empty() ? Pegasus::CIMName() : Pegasus::CIMName(m_reference_class.c_str()),
        m_class_origin.empty() ? Pegasus::CIMName() : Pegasus::CIMName(m_class_origin.c_str()),
        m_propagated);
}

#  if PY_MAJOR_VERSION < 3
int CIMProperty::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return 1;

    CIMProperty &other_property = lmi::extract<CIMProperty&>(other);

    int rval;
    if ((rval = m_name.compare(other_property.m_name)) != 0 ||
        (rval = m_type.compare(other_property.m_type)) != 0 ||
        (rval = m_class_origin.compare(other_property.m_class_origin)) != 0 ||
        (rval = m_reference_class.compare(other_property.m_reference_class)) != 0 ||
        (rval = compare(bp::object(m_is_array),
            bp::object(other_property.m_is_array))) != 0 ||
        (rval = compare(bp::object(m_propagated),
            bp::object(other_property.m_propagated))) != 0 ||
        (rval = compare(bp::object(m_array_size),
            bp::object(other_property.m_array_size))) != 0 ||
        (rval = compare(getValue(), other_property.getValue())) != 0 ||
        (rval = compare(getQualifiers(), other_property.getQualifiers())) != 0)
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

    CIMProperty &other_property = lmi::extract<CIMProperty&>(other);

    return m_name == other_property.m_name &&
        m_type == other_property.m_type &&
        m_class_origin == other_property.m_class_origin &&
        m_reference_class == other_property.m_reference_class &&
        m_is_array == other_property.m_is_array &&
        m_propagated == other_property.m_propagated &&
        m_array_size == other_property.m_array_size &&
        compare(getValue(), other_property.getValue(), Py_EQ) &&
        compare(getQualifiers(), other_property.getQualifiers(), Py_EQ);
}

bool CIMProperty::gt(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return false;

    CIMProperty &other_property = lmi::extract<CIMProperty&>(other);

    return m_name > other_property.m_name ||
        m_type > other_property.m_type ||
        m_class_origin > other_property.m_class_origin ||
        m_reference_class > other_property.m_reference_class ||
        m_is_array > other_property.m_is_array ||
        m_propagated > other_property.m_propagated ||
        m_array_size > other_property.m_array_size ||
        compare(getValue(), other_property.getValue(), Py_GT) ||
        compare(getQualifiers(), other_property.getQualifiers(), Py_GT);
}

bool CIMProperty::lt(const bp::object &other)
{
    if (!isinstance(other, CIMProperty::type()))
        return false;

    CIMProperty &other_property = lmi::extract<CIMProperty&>(other);

    return m_name < other_property.m_name ||
        m_type < other_property.m_type ||
        m_class_origin < other_property.m_class_origin ||
        m_reference_class < other_property.m_reference_class ||
        m_is_array < other_property.m_is_array ||
        m_propagated < other_property.m_propagated ||
        m_array_size < other_property.m_array_size ||
        compare(getValue(), other_property.getValue(), Py_LT) ||
        compare(getQualifiers(), other_property.getQualifiers(), Py_LT);
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

std::string CIMProperty::repr()
{
    std::stringstream ss;
    ss << "CIMProperty(name='" << m_name
        << "', type='" << m_type
        << "', value='" << object_as_std_string(getValue())
        << "', is_array=" << (m_is_array ? "True" : "False") << ", ...)";
    return ss.str();
}

bp::object CIMProperty::copy()
{
    bp::object obj = CIMBase<CIMProperty>::create();
    CIMProperty &property = lmi::extract<CIMProperty&>(obj);
    NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());

    property.m_name = m_name;
    property.m_type = m_type;
    property.m_class_origin = m_class_origin;
    property.m_reference_class = m_reference_class;
    property.m_is_array = m_is_array;
    property.m_propagated = m_propagated;
    property.m_array_size = m_array_size;
    property.m_value = m_value;
    property.m_qualifiers = qualifiers.copy();

    return obj;
}

bp::object CIMProperty::getValue()
{
    if (!m_rc_prop_value.empty()) {
        m_value = CIMValue::asLMIWbemCIMValue(*m_rc_prop_value.get());
        m_rc_prop_value.release();
    }

    return m_value;
}

bp::object CIMProperty::getQualifiers()
{
    if (!m_rc_prop_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = m_rc_prop_qualifiers.get()->begin();
             it != m_rc_prop_qualifiers.get()->end(); ++it)
        {
            bp::object name = std_string_as_pyunicode(
                std::string(it->getName().getString().getCString()));
            m_qualifiers[name] = CIMQualifier::create(*it);
        }

        m_rc_prop_qualifiers.release();
    }

    return m_qualifiers;
}

void CIMProperty::setName(const bp::object &name)
{
    m_name = lmi::extract_or_throw<std::string>(name, "name");
}

void CIMProperty::setType(const bp::object &type)
{
    m_type = lmi::extract_or_throw<std::string>(type, "type");
}

void CIMProperty::setValue(const bp::object &value)
{
    m_value = value;

    // Unref cached resource, it will never be used
    m_rc_prop_value.release();
}

void CIMProperty::setClassOrigin(const bp::object &class_origin)
{
    m_class_origin = lmi::extract_or_throw<std::string>(class_origin,
        "class_origin");
}

void CIMProperty::setArraySize(const bp::object &array_size)
{
    m_array_size = lmi::extract_or_throw<int>(array_size, "array_size");
}

void CIMProperty::setPropagated(const bp::object &propagated)
{
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");
}

void CIMProperty::setQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_prop_qualifiers.release();
}

void CIMProperty::setReferenceClass(const bp::object &reference_class)
{
    m_reference_class = lmi::extract_or_throw<std::string>(reference_class,
        "reference_class");
}
