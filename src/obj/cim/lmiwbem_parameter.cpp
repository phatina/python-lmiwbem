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
#include "lmiwbem_exception.h"
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_parameter.h"
#include "obj/cim/lmiwbem_qualifier.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

CIMParameter::CIMParameter()
    : m_name()
    , m_type()
    , m_reference_class()
    , m_is_array(false)
    , m_array_size(-1)
    , m_qualifiers()
    , m_rc_param_qualifiers()
{
}

CIMParameter::CIMParameter(
    const bp::object &name,
    const bp::object &type,
    const bp::object &reference_class,
    const bp::object &is_array,
    const bp::object &array_size,
    const bp::object &qualifiers)
{
    m_name = StringConv::asString(name, "name");
    m_type = StringConv::asString(type, "type");
    m_reference_class = StringConv::asString(reference_class, "reference_class");
    m_is_array = Conv::as<bool>(is_array, "is_array");
    m_array_size = Conv::as<int>(array_size, "array_size");
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(
        qualifiers, "qualifiers");
}

void CIMParameter::init_type()
{
    CIMBase<CIMParameter>::init_type(
        bp::class_<CIMParameter>("CIMParameter", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("name"),
                bp::arg("type"),
                bp::arg("reference_class") = String(),
                bp::arg("is_array") = false,
                bp::arg("array_size") = -1,
                bp::arg("qualifiers") = NocaseDict::create()),
                "Parameter of a CIM method.\n\n"
                ":param str name: String containing the parameter name\n"
                ":param str type: String containing the parameter type\n"
                ":param str reference_class: String containing the reference\n"
                "\tclass for the property\n"
                ":param bool is_array: Flag, which indicates, if the parameter\n"
                "\tis array\n"
                ":param int array_size: Array size\n"
                ":param NocaseDict qualifiers: Dictionary of :py:class:`.CIMQualifier`"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMParameter::cmp)
#  else
        .def("__eq__", &CIMParameter::eq)
        .def("__gt__", &CIMParameter::gt)
        .def("__lt__", &CIMParameter::lt)
        .def("__ge__", &CIMParameter::ge)
        .def("__le__", &CIMParameter::le)
#  endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMParameter::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMParameter::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMParameter`")
        .def("tomof", &CIMParameter::tomof,
            "tomof()\n\n"
            ":returns: MOF representation of the object itself\n"
            ":rtype: unicode")
        .add_property("name",
            &CIMParameter::getPyName,
            &CIMParameter::setPyName,
            "Property storing name of the parameter.\n\n"
            ":returns: string containing the parameter's name")
        .add_property("type",
            &CIMParameter::getPyType,
            &CIMParameter::setPyType,
            "Property storing type of the parameter.\n\n"
            ":rtype: unicode")
        .add_property("reference_class",
            &CIMParameter::getPyReferenceClass,
            &CIMParameter::setPyReferenceClass,
            "Property storing reference class of the parameter.\n\n"
            ":rtype: unicode")
        .add_property("is_array",
            &CIMParameter::getPyIsArray,
            &CIMParameter::setPyIsArray,
            "Property storing flag, which indicates, if the parameter is array.\n\n"
            ":returns: True, if the parameter is array; False otherwise\n"
            ":rtype: bool")
        .add_property("array_size",
            &CIMParameter::getPyArraySize,
            &CIMParameter::setPyArraySize,
            "Property storing array size of the parameter.\n\n"
            ":rtype: int")
        .add_property("qualifiers",
            &CIMParameter::getPyQualifiers,
            &CIMParameter::setPyQualifiers,
            "Property storing qualifiers of the parameter.\n\n"
            ":rtype: :py:class:`.NocaseDict`"));
}

bp::object CIMParameter::create(const Pegasus::CIMParameter &parameter)
{
    return create(static_cast<Pegasus::CIMConstParameter>(parameter));
}

bp::object CIMParameter::create(const Pegasus::CIMConstParameter &parameter)
{
    bp::object py_inst = CIMBase<CIMParameter>::create();
    CIMParameter &fake_this = CIMParameter::asNative(py_inst);
    fake_this.m_name = parameter.getName().getString();
    fake_this.m_type = CIMTypeConv::asString(parameter.getType());
    fake_this.m_reference_class = parameter.getReferenceClassName().getString();
    fake_this.m_is_array = parameter.isArray();
    fake_this.m_array_size = static_cast<int>(parameter.getArraySize());

    // Store list of qualifiers for lazy evaluation
    fake_this.m_rc_param_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    const Pegasus::Uint32 cnt = parameter.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_param_qualifiers.get()->push_back(parameter.getQualifier(i));

    return py_inst;
}

Pegasus::CIMParameter CIMParameter::asPegasusCIMParameter() try
{
    Pegasus::CIMParameter cim_parameter(
        Pegasus::CIMName(m_name),
        CIMTypeConv::asCIMType(m_type),
        m_is_array,
        static_cast<Pegasus::Uint32>(m_array_size),
        Pegasus::CIMName(m_reference_class));

    // Add all the qualifiers
    const NocaseDict &qualifiers = NocaseDict::asNative(getPyQualifiers());
    nocase_map_t::const_iterator it;
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = CIMQualifier::asNative(it->second);
        cim_parameter.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    return cim_parameter;
} catch (const Pegasus::TypeMismatchException &e) {
    String msg(m_name);
    msg += ": ";
    msg += e.getMessage();
    throw Pegasus::TypeMismatchException(msg);
    return Pegasus::CIMParameter();
}

#  if PY_MAJOR_VERSION < 3
int CIMParameter::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMParameter::type()))
        return 1;

    CIMParameter &cim_other = CIMParameter::asNative(other);

    int rval;
    if ((rval = m_name.compare(cim_other.m_name)) != 0 ||
        (rval = m_type.compare(cim_other.m_type)) != 0 ||
        (rval = m_reference_class.compare(cim_other.m_reference_class)) != 0 ||
        (rval = compare(bp::object(m_is_array),
            bp::object(cim_other.m_is_array))) != 0 ||
        (rval = compare(bp::object(m_array_size),
            bp::object(cim_other.m_array_size))) != 0 ||
        (rval = compare(getPyQualifiers(), cim_other.getPyQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMParameter::eq(const bp::object &other)
{
    if (!isinstance(other, CIMParameter::type()))
        return false;

    CIMParameter &cim_other = CIMParameter::asNative(other);

    return m_name == cim_other.m_name &&
        m_type == cim_other.m_type &&
        m_reference_class == cim_other.m_reference_class &&
        m_is_array == cim_other.m_is_array &&
        m_array_size == cim_other.m_array_size &&
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_EQ);
}

bool CIMParameter::gt(const bp::object &other)
{
    if (!isinstance(other, CIMParameter::type()))
        return false;

    CIMParameter &cim_other = CIMParameter::asNative(other);

    return m_name > cim_other.m_name ||
        m_type > cim_other.m_type ||
        m_reference_class > cim_other.m_reference_class ||
        m_is_array > cim_other.m_is_array ||
        m_array_size > cim_other.m_array_size ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_GT);
}

bool CIMParameter::lt(const bp::object &other)
{
    if (!isinstance(other, CIMParameter::type()))
        return false;

    CIMParameter &cim_other = CIMParameter::asNative(other);

    return m_name < cim_other.m_name ||
        m_type < cim_other.m_type ||
        m_reference_class < cim_other.m_reference_class ||
        m_is_array < cim_other.m_is_array ||
        m_array_size < cim_other.m_array_size ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_LT);
}

bool CIMParameter::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMParameter::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

bp::object CIMParameter::repr()
{
    std::stringstream ss;
    ss << "CIMParameter(name=u'" << m_name << "', type=u'" << m_type
       << "', is_array=" << (m_is_array ? "True" : "False") << ')';
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMParameter::tomof()
{
    std::stringstream ss;
    ss << m_type << ' ' << m_name;
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMParameter::copy()
{
    bp::object py_inst = CIMBase<CIMParameter>::create();
    CIMParameter &cim_parameter = CIMParameter::asNative(py_inst);
    NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());

    cim_parameter.m_name = m_name;
    cim_parameter.m_type = m_type;
    cim_parameter.m_reference_class = m_reference_class;
    cim_parameter.m_is_array = m_is_array;
    cim_parameter.m_array_size = m_array_size;
    cim_parameter.m_qualifiers = cim_qualifiers.copy();

    return py_inst;
}

String CIMParameter::getName() const
{
    return m_name;
}

String CIMParameter::getType() const
{
    return m_type;
}

String CIMParameter::getReferenceClass() const
{
    return m_reference_class;
}

bool CIMParameter::getIsArray() const
{
    return m_is_array;
}

int CIMParameter::getArraySize() const
{
    return m_array_size;
}

bp::object CIMParameter::getPyName() const
{
    return StringConv::asPyUnicode(m_name);
}

bp::object CIMParameter::getPyType() const
{
    return StringConv::asPyUnicode(m_type);
}

bp::object CIMParameter::getPyReferenceClass() const
{
    return StringConv::asPyUnicode(m_reference_class);
}

bp::object CIMParameter::getPyIsArray() const
{
    return bp::object(m_is_array);
}

bp::object CIMParameter::getPyArraySize() const
{
    return bp::object(m_array_size);
}

bp::object CIMParameter::getPyQualifiers()
{
    if (!m_rc_param_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = m_rc_param_qualifiers.get()->begin();
             it != m_rc_param_qualifiers.get()->end(); ++it)
        {
            m_qualifiers[bp::object(it->getName())] = CIMQualifier::create(*it);
        }

        m_rc_param_qualifiers.release();
    }

    return m_qualifiers;
}

void CIMParameter::setName(const String &name)
{
    m_name = name;
}

void CIMParameter::setType(const String &type)
{
    m_type = type;
}

void CIMParameter::setReferenceClass(const String &reference_class)
{
    m_reference_class = reference_class;
}

void CIMParameter::setIsArray(bool is_array)
{
    m_is_array = is_array;
}

void CIMParameter::setArraySize(int array_size)
{
    if (array_size < 0)
        throw_ValueError("array_size must be positive number or 0");

    m_array_size = array_size;
}

void CIMParameter::setPyName(const bp::object &name)
{
    m_name = StringConv::asString(name, "name");
}

void CIMParameter::setPyType(const bp::object &type)
{
    m_type = StringConv::asString(type, "type");
}

void CIMParameter::setPyReferenceClass(const bp::object &reference_class)
{
    m_reference_class = StringConv::asString(reference_class, "reference_class");
}

void CIMParameter::setPyIsArray(const bp::object &is_array)
{
    m_is_array = Conv::as<bool>(is_array, "is_array");
}

void CIMParameter::setPyArraySize(const bp::object &array_size)
{
    m_array_size = Conv::as<int>(array_size, "array_size");
}

void CIMParameter::setPyQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_param_qualifiers.release();
}
