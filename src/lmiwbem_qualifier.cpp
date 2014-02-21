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
    CIMBase::s_class = bp::class_<CIMQualifier>("CIMQualifier", bp::init<>())
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
        .def("__repr__", &CIMQualifier::repr,
            ":returns: pretty string of the object")
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
            ":rtype: bool")
        ;
}

bp::object CIMQualifier::create(const Pegasus::CIMQualifier &qualifier)
{
    return create(static_cast<Pegasus::CIMConstQualifier>(qualifier));
}

bp::object CIMQualifier::create(const Pegasus::CIMConstQualifier &qualifier)
{
    bp::object inst = CIMBase::s_class();
    CIMQualifier &fake_this = lmi::extract<CIMQualifier&>(inst);
    fake_this.m_name = std::string(qualifier.getName().getString().getCString());
    fake_this.m_type = CIMTypeConv::asStdString(qualifier.getType());
    fake_this.m_value = CIMValue::create(qualifier.getValue());
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

std::string CIMQualifier::repr()
{
    std::stringstream ss;
    ss << "CIMQualifier(name='" << m_name << "', ...')";
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
