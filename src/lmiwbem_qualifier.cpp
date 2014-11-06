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
#include "lmiwbem_convert.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"
#include "lmiwbem_value.h"

CIMQualifier::CIMQualifier()
    : m_name()
    , m_type()
    , m_value()
    , m_is_propagated(false)
    , m_is_overridable(false)
    , m_is_tosubclass(false)
    , m_is_toinstance(false)
    , m_is_translatable(false)
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
    m_name = StringConv::asStdString(name, "name");
    m_type = StringConv::asStdString(type, "type");
    m_value = value;
    m_is_propagated = Conv::as<bool>(propagated, "propagated");
    m_is_overridable = Conv::as<bool>(overridable, "overridable");
    m_is_tosubclass = Conv::as<bool>(tosubclass, "tosubclass");
    m_is_toinstance = Conv::as<bool>(toinstance, "toinstance");
    m_is_translatable = Conv::as<bool>(translatable, "translatable");
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
                "Constructs a :py:class:`.CIMQualifier`.\n\n"
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
#  endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMQualifier::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMQualifier::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMQualifier`")
        .def("tomof", &CIMQualifier::tomof,
            "tomof()\n\n"
            ":returns: MOF representation of the object itself\n"
            ":rtype: unicode")
        .add_property("name",
            &CIMQualifier::getPyName,
            &CIMQualifier::setPyName,
            "Property storing qualifier's name.\n\n"
            ":rtype: unicode")
        .add_property("type",
            &CIMQualifier::getPyType,
            &CIMQualifier::setPyType,
            "Property storing qualifier's type.\n\n"
            ":rtype: unicode")
        .add_property("value",
            &CIMQualifier::getPyValue,
            &CIMQualifier::setPyValue,
            "Property storing qualifier's value.\n\n"
            ":returns: qualifier's value")
        .add_property("propagated",
            &CIMQualifier::getPyIsPropagated,
            &CIMQualifier::setPyIsPropagated,
            "Property storing propagation flag of the qualifier.\n\n"
            ":rtype: bool")
        .add_property("overridable",
            &CIMQualifier::getPyIsOverridable,
            &CIMQualifier::setPyIsOverridable,
            "Property storing overriding flag of the qualifier.\n\n"
            ":rtype: bool")
        .add_property("tosubclass",
            &CIMQualifier::getPyIsToSubclass,
            &CIMQualifier::setPyIsToSubclass,
            "Property storing tosubclass flag.\n\n"
            ":rtype: bool")
        .add_property("toinstance",
            &CIMQualifier::getPyIsToInstance,
            &CIMQualifier::setPyIsToInstance,
            "Property storing toinstance flag.\n\n"
            ":rtype: bool")
        .add_property("translatable",
            &CIMQualifier::getPyIsTranslatable,
            &CIMQualifier::setPyIsTranslatable,
            "Property storing qualifier's translation flag.\n\n"
            ":rtype: bool"));
}

bp::object CIMQualifier::create(const Pegasus::CIMQualifier &qualifier)
{
    return create(static_cast<Pegasus::CIMConstQualifier>(qualifier));
}

bp::object CIMQualifier::create(const Pegasus::CIMConstQualifier &qualifier)
{
    bp::object inst = CIMBase<CIMQualifier>::create();
    CIMQualifier &fake_this = CIMQualifier::asNative(inst);
    fake_this.m_name = std::string(qualifier.getName().getString().getCString());
    fake_this.m_type = CIMTypeConv::asStdString(qualifier.getType());
    fake_this.m_value = CIMValue::asLMIWbemCIMValue(qualifier.getValue());
    fake_this.m_is_propagated = static_cast<bool>(qualifier.getPropagated());
    const Pegasus::CIMFlavor &flavor = qualifier.getFlavor();
    fake_this.m_is_overridable = flavor.hasFlavor(Pegasus::CIMFlavor::OVERRIDABLE);
    fake_this.m_is_tosubclass = flavor.hasFlavor(Pegasus::CIMFlavor::TOSUBCLASS);
    fake_this.m_is_toinstance = flavor.hasFlavor(Pegasus::CIMFlavor::TOINSTANCE);
    fake_this.m_is_translatable = flavor.hasFlavor(Pegasus::CIMFlavor::TRANSLATABLE);
    return inst;
}

Pegasus::CIMQualifier CIMQualifier::asPegasusCIMQualifier() const
{
    Pegasus::CIMFlavor flavor;
    if (m_is_overridable)
        flavor.addFlavor(Pegasus::CIMFlavor::OVERRIDABLE);
    if (m_is_tosubclass)
        flavor.addFlavor(Pegasus::CIMFlavor::TOSUBCLASS);
    if (m_is_toinstance)
        flavor.addFlavor(Pegasus::CIMFlavor::TOINSTANCE);
    if (m_is_translatable)
        flavor.addFlavor(Pegasus::CIMFlavor::TRANSLATABLE);
    return Pegasus::CIMQualifier(
        Pegasus::CIMName(m_name.c_str()),
        CIMValue::asPegasusCIMValue(m_value),
        flavor,
        m_is_propagated);
}

#  if PY_MAJOR_VERSION < 3
int CIMQualifier::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return 1;

    CIMQualifier &other_qualifier = CIMQualifier::asNative(other);

    int rval;
    if ((rval = m_name.compare(other_qualifier.m_name)) != 0 ||
        (rval = m_type.compare(other_qualifier.m_type)) != 0 ||
        (rval = compare(m_value, other_qualifier.m_value)) != 0 ||
        (rval = compare(bp::object(m_is_propagated),
            bp::object(other_qualifier.m_is_propagated))) != 0 ||
        (rval = compare(bp::object(m_is_overridable),
            bp::object(other_qualifier.m_is_overridable))) != 0 ||
        (rval = compare(bp::object(m_is_tosubclass),
            bp::object(other_qualifier.m_is_tosubclass))) != 0 ||
        (rval = compare(bp::object(m_is_toinstance),
            bp::object(other_qualifier.m_is_toinstance))) != 0 ||
        (rval = compare(bp::object(m_is_translatable),
            bp::object(other_qualifier.m_is_translatable))) != 0)
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

    CIMQualifier &other_qualifier = CIMQualifier::asNative(other);

    return m_name == other_qualifier.m_name &&
        m_type == other_qualifier.m_type &&
        m_is_propagated == other_qualifier.m_is_propagated &&
        m_is_overridable == other_qualifier.m_is_overridable &&
        m_is_tosubclass == other_qualifier.m_is_tosubclass &&
        m_is_toinstance == other_qualifier.m_is_toinstance &&
        m_is_translatable == other_qualifier.m_is_translatable &&
        compare(m_value, other_qualifier.m_value, Py_EQ);
}

bool CIMQualifier::gt(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return false;

    CIMQualifier &other_qualifier = CIMQualifier::asNative(other);

    return m_name > other_qualifier.m_name ||
        m_type > other_qualifier.m_type ||
        m_is_propagated > other_qualifier.m_is_propagated ||
        m_is_overridable > other_qualifier.m_is_overridable ||
        m_is_tosubclass > other_qualifier.m_is_tosubclass ||
        m_is_toinstance > other_qualifier.m_is_toinstance ||
        m_is_translatable > other_qualifier.m_is_translatable ||
        compare(m_value, other_qualifier.m_value, Py_GT);
}

bool CIMQualifier::lt(const bp::object &other)
{
    if (!isinstance(other, CIMQualifier::type()))
        return false;

    CIMQualifier &other_qualifier = CIMQualifier::asNative(other);

    return m_name < other_qualifier.m_name ||
        m_type < other_qualifier.m_type ||
        m_is_propagated < other_qualifier.m_is_propagated ||
        m_is_overridable < other_qualifier.m_is_overridable ||
        m_is_tosubclass < other_qualifier.m_is_tosubclass ||
        m_is_toinstance < other_qualifier.m_is_toinstance ||
        m_is_translatable < other_qualifier.m_is_translatable ||
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
#  endif // PY_MAJOR_VERSION

bp::object CIMQualifier::repr()
{
    std::stringstream ss;
    ss << "CIMQualifier(name='" << m_name << "', ...')";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMQualifier::tomof()
{
    std::stringstream ss;

    ss << m_name;
    if (!PyList_Check(m_value.ptr()) && !PyTuple_Check(m_value.ptr())) {
        if (isbasestring(m_value))
            ss << " (\"" << ObjectConv::asStdString(m_value) << "\")";
        else
            ss << " (" << ObjectConv::asStdString(m_value) << ')';
    } else {
        ss << " {";
        const int cnt = bp::len(m_value);
        for (int i = 0; i < cnt; ++i) {
            const bp::object &value = m_value[i];
            if (isbasestring(value))
                ss << '\'' << ObjectConv::asStdString(value) << '\'';
            else
                ss << ObjectConv::asStdString(value);

            if (i < cnt - 1)
                ss << ", ";
        }
        ss << '}';
    }

    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMQualifier::copy()
{
    bp::object obj = CIMBase<CIMQualifier>::create();
    CIMQualifier &qualifier = CIMQualifier::asNative(obj);

    qualifier.m_name = m_name;
    qualifier.m_type = m_type;
    qualifier.m_value = m_value;
    qualifier.m_is_propagated = m_is_propagated;
    qualifier.m_is_overridable = m_is_overridable;
    qualifier.m_is_tosubclass = m_is_tosubclass;
    qualifier.m_is_toinstance = m_is_toinstance;
    qualifier.m_is_translatable = m_is_translatable;

    return obj;
}

std::string CIMQualifier::getName() const
{
    return m_name;
}

std::string CIMQualifier::getType() const
{
    return m_type;
}

bool CIMQualifier::getIsPropagated() const
{
    return m_is_propagated;
}

bool CIMQualifier::getIsOverridable() const
{
    return m_is_overridable;
}

bool CIMQualifier::getIsToSubClass() const
{
    return m_is_tosubclass;
}

bool CIMQualifier::getIsToInstance() const
{
    return m_is_toinstance;
}

bool CIMQualifier::getIsTranslatable() const
{
    return m_is_translatable;
}

bp::object CIMQualifier::getPyName() const
{
    return StringConv::asPyUnicode(m_name);
}

bp::object CIMQualifier::getPyType() const
{
    return StringConv::asPyUnicode(m_type);
}

bp::object CIMQualifier::getPyValue() const
{
    return m_value;
}

bp::object CIMQualifier::getPyIsPropagated() const
{
    return bp::object(m_is_propagated);
}

bp::object CIMQualifier::getPyIsOverridable() const
{
    return bp::object(m_is_overridable);
}

bp::object CIMQualifier::getPyIsToSubclass() const
{
    return bp::object(m_is_tosubclass);
}

bp::object CIMQualifier::getPyIsToInstance() const
{
    return bp::object(m_is_toinstance);
}

bp::object CIMQualifier::getPyIsTranslatable() const
{
    return bp::object(m_is_translatable);
}

void CIMQualifier::setName(const std::string &name)
{
    m_name = name;
}

void CIMQualifier::setType(const std::string &type)
{
    m_type = type;
}

void CIMQualifier::setIsPropagated(bool is_propagated)
{
    m_is_propagated = is_propagated;
}

void CIMQualifier::setIsOverridable(bool is_overridable)
{
    m_is_overridable = is_overridable;
}

void CIMQualifier::setIsToSubclass(bool is_tosubclass)
{
    m_is_tosubclass = is_tosubclass;
}

void CIMQualifier::setIsToInstance(bool is_toinstance)
{
    m_is_toinstance = is_toinstance;
}

void CIMQualifier::setIsTranslatable(bool is_translatable)
{
    m_is_translatable = is_translatable;
}

void CIMQualifier::setPyName(const bp::object &name)
{
    m_name = StringConv::asStdString(name, "name");
}

void CIMQualifier::setPyType(const bp::object &type)
{
    m_type = StringConv::asStdString(type, "type");
}

void CIMQualifier::setPyValue(const bp::object &value)
{
    m_value = value;
}

void CIMQualifier::setPyIsPropagated(const bp::object &propagated)
{
    m_is_propagated = Conv::as<bool>(propagated, "propagated");
}

void CIMQualifier::setPyIsOverridable(const bp::object &overridable)
{
    m_is_overridable = Conv::as<bool>(overridable, "overridable");
}

void CIMQualifier::setPyIsToSubclass(const bp::object &tosubclass)
{
    m_is_tosubclass = Conv::as<bool>(tosubclass, "tosubclass");
}

void CIMQualifier::setPyIsToInstance(const bp::object &toinstance)
{
    m_is_toinstance = Conv::as<bool>(toinstance, "toinstance");
}

void CIMQualifier::setPyIsTranslatable(const bp::object &translatable)
{
    m_is_translatable = Conv::as<bool>(translatable, "translatable");
}
