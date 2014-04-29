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
#include <boost/python/class.hpp>
#include "lmiwbem_extract.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"
#include "lmiwbem_value.h"

CIMQualifier::CIMQualifier()
    : m_name()
    , m_type()
    , m_value()
    , m_propagated(false)
    , m_overridable(false)
    , m_tosubclass(false)
    , m_toinstance(false)
    , m_translatable(false)
{
}

CIMQualifier::CIMQualifier(
    const bp::object &name,
    const bp::object &value,
    const bp::object &type,
    const bp::object &propagated,
    const bp::object &overridable,
    const bp::object &tosubclass,
    const bp::object &toinstance,
    const bp::object &translatable)
{
    m_name = lmi::extract_or_throw<std::string>(name, "name");
    m_type = lmi::extract_or_throw<std::string>(type, "type");
    m_value = value;
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");
    m_overridable = lmi::extract_or_throw<bool>(overridable, "overridable");
    m_tosubclass = lmi::extract_or_throw<bool>(tosubclass, "tosubclass");
    m_toinstance = lmi::extract_or_throw<bool>(toinstance, "toinstance");
    m_translatable = lmi::extract_or_throw<bool>(translatable, "translatable");
}

void CIMQualifier::init_type()
{
    CIMBase<CIMQualifier>::init_type(bp::class_<CIMQualifier>("CIMQualifier", bp::init<>())
        .def(bp::init<
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
                bp::arg("type") = std::string(),
                bp::arg("propagated") = false,
                bp::arg("overridable") = false,
                bp::arg("tosubclass") = false,
                bp::arg("toinstance") = false,
                bp::arg("translatable") = false),
                "Constructs a :py:class:`CIMQualifier`.\n\n"
                ":param str name: String containing the qualifier's name\n"
                ":param value: Qualifier's value\n"
                ":param str type: String containing the qualifier's type\n"
                ":param bool propagated: True, if the qualifier is propadated;\n"
                "\tFalse otherwise\n"
                ":param bool overridable: True, if the qualifier can be overridden;\n"
                "\tFalse otherwise\n"
                ":param bool tosubclass: True, if the qualifier shall appear in subclass;\n"
                "\tFalse otherwise"
                ":param bool toinstance: True, if the qualifier shall appear in instance;\n"
                "\tFalse otherwise"
                ":param bool translatable: True, if the qualifier can be translated;\n"
                "\tFalse otherwise"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMQualifier::cmp)
#  else
        .def("__eq__", &CIMQualifier::eq)
        .def("__gt__", &CIMQualifier::gt)
        .def("__lt__", &CIMQualifier::lt)
        .def("__ge__", &CIMQualifier::ge)
        .def("__le__", &CIMQualifier::le)
#  endif
        .def("__repr__", &CIMQualifier::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMQualifier::copy)
        .def("tomof", &CIMQualifier::tomof)
        .add_property("name",
            &CIMQualifier::getName,
            &CIMQualifier::setName,
            "Property storing qualifier's name.\n\n"
            ":returns: string containing the qualifier's name")
        .add_property("type",
            &CIMQualifier::getType,
            &CIMQualifier::setType,
            "Property storing qualifier's type.\n\n"
            ":returns: string containing the qualifier's type")
        .add_property("value",
            &CIMQualifier::m_value,
            "Property storing qualifier's value.\n\n")
        .add_property("propagated",
            &CIMQualifier::m_propagated,
            &CIMQualifier::setPropagated,
            "Property storing propagation flag of the qualifier.\n\n"
            ":returns: True, if the qualifier is propagated; False otherwise\n"
            ":rtype: bool")
        .add_property("overridable",
            &CIMQualifier::m_overridable,
            &CIMQualifier::setOverridable,
            "Property storing overriding flag of the qualifier.\n\n"
            ":returns: True, if the qualifier can be overridden; False otherwise\n"
            ":rtype: bool")
        .add_property("tosubclass",
            &CIMQualifier::m_tosubclass,
            &CIMQualifier::setToSubclass,
            "Property storing tosubclass flag.\n\n"
            ":returns: True, if the qualifier shall appear in subclass; False otherwise\n"
            ":rtype: bool")
        .add_property("toinstance",
            &CIMQualifier::m_toinstance,
            &CIMQualifier::setToInstance,
            "Property storing toinstance flag.\n\n"
            ":returns: True, if the qualifier shall appear in instance; False otherwise\n"
            ":rtype: bool")
        .add_property("translatable",
            &CIMQualifier::m_translatable,
            &CIMQualifier::setTranslatable,
            "Property storing qualifier's translation flag.\n\n"
            ":returns: True, if the qualifier can be translated; False otherwise\n"
            ":rtype: bool"));
}

bp::object CIMQualifier::create(const Pegasus::CIMQualifier &qualifier)
{
    return create(static_cast<Pegasus::CIMConstQualifier>(qualifier));
}

bp::object CIMQualifier::create(const Pegasus::CIMConstQualifier &qualifier)
{
    bp::object inst = CIMBase<CIMQualifier>::create();
    CIMQualifier &fake_this = lmi::extract<CIMQualifier&>(inst);
    fake_this.m_name = std::string(qualifier.getName().getString().getCString());
    fake_this.m_type = CIMTypeConv::asStdString(qualifier.getType());
    fake_this.m_value = CIMValue::asLMIWbemCIMValue(qualifier.getValue());
    fake_this.m_propagated = static_cast<bool>(qualifier.getPropagated());
    const Pegasus::CIMFlavor &flavor = qualifier.getFlavor();
    fake_this.m_overridable = flavor.hasFlavor(Pegasus::CIMFlavor::OVERRIDABLE);
    fake_this.m_tosubclass = flavor.hasFlavor(Pegasus::CIMFlavor::TOSUBCLASS);
    fake_this.m_toinstance = flavor.hasFlavor(Pegasus::CIMFlavor::TOINSTANCE);
    fake_this.m_translatable = flavor.hasFlavor(Pegasus::CIMFlavor::TRANSLATABLE);
    return inst;
}

Pegasus::CIMQualifier CIMQualifier::asPegasusCIMQualifier() const
{
    Pegasus::CIMFlavor flavor;
    if (m_overridable)
        flavor.addFlavor(Pegasus::CIMFlavor::OVERRIDABLE);
    if (m_tosubclass)
        flavor.addFlavor(Pegasus::CIMFlavor::TOSUBCLASS);
    if (m_toinstance)
        flavor.addFlavor(Pegasus::CIMFlavor::TOINSTANCE);
    if (m_translatable)
        flavor.addFlavor(Pegasus::CIMFlavor::TRANSLATABLE);
    return Pegasus::CIMQualifier(
        Pegasus::CIMName(m_name.c_str()),
        CIMValue::asPegasusCIMValue(m_value),
        flavor,
        m_propagated);
}

#  if PY_MAJOR_VERSION < 3
int CIMQualifier::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return 1;

    CIMQualifier &other_qualifier = lmi::extract<CIMQualifier&>(other);

    int rval;
    if ((rval = m_name.compare(other_qualifier.m_name)) != 0 ||
        (rval = m_type.compare(other_qualifier.m_type)) != 0 ||
        (rval = compare(m_value, other_qualifier.m_value)) != 0 ||
        (rval = compare(bp::object(m_propagated),
            bp::object(other_qualifier.m_propagated))) != 0 ||
        (rval = compare(bp::object(m_overridable),
            bp::object(other_qualifier.m_overridable))) != 0 ||
        (rval = compare(bp::object(m_tosubclass),
            bp::object(other_qualifier.m_tosubclass))) != 0 ||
        (rval = compare(bp::object(m_toinstance),
            bp::object(other_qualifier.m_toinstance))) != 0 ||
        (rval = compare(bp::object(m_translatable),
            bp::object(other_qualifier.m_translatable))) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMQualifier::eq(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return false;

    CIMQualifier &other_qualifier = lmi::extract<CIMQualifier&>(other);

    return m_name == other_qualifier.m_name &&
        m_type == other_qualifier.m_type &&
        m_propagated == other_qualifier.m_propagated &&
        m_overridable == other_qualifier.m_overridable &&
        m_tosubclass == other_qualifier.m_tosubclass &&
        m_toinstance == other_qualifier.m_toinstance &&
        m_translatable == other_qualifier.m_translatable &&
        compare(m_value, other_qualifier.m_value, Py_EQ);
}

bool CIMQualifier::gt(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return false;

    CIMQualifier &other_qualifier = lmi::extract<CIMQualifier&>(other);

    return m_name > other_qualifier.m_name ||
        m_type > other_qualifier.m_type ||
        m_propagated > other_qualifier.m_propagated ||
        m_overridable > other_qualifier.m_overridable ||
        m_tosubclass > other_qualifier.m_tosubclass ||
        m_toinstance > other_qualifier.m_toinstance ||
        m_translatable > other_qualifier.m_translatable ||
        compare(m_value, other_qualifier.m_value, Py_GT);
}

bool CIMQualifier::lt(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return false;

    CIMQualifier &other_qualifier = lmi::extract<CIMQualifier&>(other);

    return m_name < other_qualifier.m_name ||
        m_type < other_qualifier.m_type ||
        m_propagated < other_qualifier.m_propagated ||
        m_overridable < other_qualifier.m_overridable ||
        m_tosubclass < other_qualifier.m_tosubclass ||
        m_toinstance < other_qualifier.m_toinstance ||
        m_translatable < other_qualifier.m_translatable ||
        compare(m_value, other_qualifier.m_value, Py_LT);
}

bool CIMQualifier::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMQualifier::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif

std::string CIMQualifier::repr()
{
    std::stringstream ss;
    ss << "CIMQualifier(name='" << m_name << "', ...')";
    return ss.str();
}

bp::object CIMQualifier::copy()
{
    bp::object obj = CIMBase<CIMQualifier>::create();
    CIMQualifier &qualifier = lmi::extract<CIMQualifier&>(obj);

    qualifier.m_name = m_name;
    qualifier.m_type = m_type;
    qualifier.m_value = m_value;
    qualifier.m_propagated = m_propagated;
    qualifier.m_overridable = m_overridable;
    qualifier.m_tosubclass = m_tosubclass;
    qualifier.m_toinstance = m_toinstance;
    qualifier.m_translatable = m_translatable;

    return obj;
}

std::string CIMQualifier::tomof()
{
    std::stringstream ss;

    ss << m_name;
    if (!PyList_Check(m_value.ptr()) && !PyTuple_Check(m_value.ptr())) {
        if (isbasestring(m_value))
            ss << " (\"" << object_as_std_string(m_value) << "\")";
        else
            ss << " (" << object_as_std_string(m_value) << ')';
    } else {
        ss << " {";
        const int cnt = bp::len(m_value);
        for (int i = 0; i < cnt; ++i) {
            const bp::object &value = m_value[i];
            if (isbasestring(value))
                ss << '\'' << object_as_std_string(value) << '\'';
            else
                ss << object_as_std_string(value);

            if (i < cnt - 1)
                ss << ", ";
        }
        ss << '}';
    }

    return ss.str();
}

bp::object CIMQualifier::getName()
{
    return std_string_as_pyunicode(m_name);
}

bp::object CIMQualifier::getType()
{
    return std_string_as_pyunicode(m_type);
}

void CIMQualifier::setName(const bp::object &name)
{
    m_name = lmi::extract_or_throw<std::string>(name, "name");
}

void CIMQualifier::setType(const bp::object &type)
{
    m_type = lmi::extract_or_throw<std::string>(type, "type");
}

void CIMQualifier::setPropagated(const bp::object &propagated)
{
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");
}

void CIMQualifier::setOverridable(const bp::object &overridable)
{
    m_overridable = lmi::extract_or_throw<bool>(overridable, "overridable");
}

void CIMQualifier::setToSubclass(const bp::object &tosubclass)
{
    m_tosubclass = lmi::extract_or_throw<bool>(tosubclass, "tosubclass");
}

void CIMQualifier::setToInstance(const bp::object &toinstance)
{
    m_toinstance = lmi::extract_or_throw<bool>(toinstance, "toinstance");
}

void CIMQualifier::setTranslatable(const bp::object &translatable)
{
    m_translatable = lmi::extract_or_throw<bool>(translatable, "translatable");
}
