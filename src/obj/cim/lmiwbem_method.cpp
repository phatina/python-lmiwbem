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
#include <list>
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_method.h"
#include "obj/cim/lmiwbem_parameter.h"
#include "obj/cim/lmiwbem_qualifier.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

CIMMethod::CIMMethod()
    : m_name()
    , m_return_type()
    , m_class_origin()
    , m_is_propagated(false)
    , m_parameters()
    , m_qualifiers()
    , m_rc_meth_parameters()
    , m_rc_meth_qualifiers()
{
}

CIMMethod::CIMMethod(
    const bp::object &name,
    const bp::object &return_type,
    const bp::object &parameters,
    const bp::object &class_origin,
    const bp::object &propagated,
    const bp::object &qualifiers)
{
    m_name  = StringConv::asStdString(name, "name");
    m_return_type = StringConv::asStdString(return_type, "return_type");
    m_class_origin = StringConv::asStdString(class_origin, "class_origin");
    m_is_propagated = Conv::as<bool>(propagated, "propagated");

    if (isnone(parameters))
        m_parameters = NocaseDict::create();
    else
        m_parameters = NocaseDict::create(parameters);

    if (isnone(qualifiers))
        m_qualifiers = NocaseDict::create();
    else
        m_qualifiers = NocaseDict::create(qualifiers);
}

void CIMMethod::init_type()
{
    CIMBase<CIMMethod>::init_type(bp::class_<CIMMethod>("CIMMethod", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("methodname"),
                bp::arg("return_type") = std::string(),
                bp::arg("parameters") = NocaseDict::create(),
                bp::arg("class_origin") = std::string(),
                bp::arg("propagated") = false,
                bp::arg("qualifiers") = NocaseDict::create()),
                "Constructs a :py:class:`.CIMMethod`.\n\n"
                ":param str methodname: String containing the method's name\n"
                ":param str return_type: String containing the method's return type\n"
                ":param NocaseDict parameters: Dictionary containing method's parameters\n"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMMethod::cmp)
#  else
        .def("__eq__", &CIMMethod::eq)
        .def("__gt__", &CIMMethod::gt)
        .def("__lt__", &CIMMethod::lt)
        .def("__ge__", &CIMMethod::ge)
        .def("__le__", &CIMMethod::le)
#  endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMMethod::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMMethod::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMMethod`")
        .def("tomof", &CIMMethod::tomof,
            "tomof()\n\n"
            ":returns: MOF representation of the object itself\n"
            ":rtype: unicode")
        .add_property("name",
            &CIMMethod::getPyName,
            &CIMMethod::setPyName,
            "Property storing method's name.\n\n"
            ":rtype: unicode")
        .add_property("return_type",
            &CIMMethod::getPyReturnType,
            &CIMMethod::setPyReturnType,
            "Property storing method's return type.\n\n"
            ":rtype: unicode")
        .add_property("parameters",
            &CIMMethod::getPyParameters,
            &CIMMethod::setPyParameters,
            "Property storing method's parameters.\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("class_origin",
            &CIMMethod::getPyClassOrigin,
            &CIMMethod::setPyClassOrigin,
            "Property storing method's class origin.\n\n"
            ":rtype: unicode")
        .add_property("propagated",
            &CIMMethod::getPyIsPropagated,
            &CIMMethod::setPyIsPropagated,
            "Property storing flag, which indicates, if the method is propagated.\n\n"
            ":returns: True, if the method is propagated; False otherwise\n"
            ":rtype: bool")
        .add_property("qualifiers",
            &CIMMethod::getPyQualifiers,
            &CIMMethod::setPyQualifiers,
            "Property storing method's qualifiers.\n\n"
            ":rtype: :py:class:`.NocaseDict`"));
}

bp::object CIMMethod::create(const Pegasus::CIMConstMethod &method)
{
    bp::object inst = CIMBase<CIMMethod>::create();
    CIMMethod &fake_this = CIMMethod::asNative(inst);
    fake_this.m_name = method.getName().getString().getCString();
    fake_this.m_return_type = CIMTypeConv::asStdString(method.getType());
    fake_this.m_class_origin = method.getClassOrigin().getString().getCString();
    fake_this.m_is_propagated = method.getPropagated();

    // Store list of parameters for lazy evaluation
    fake_this.m_rc_meth_parameters.set(std::list<Pegasus::CIMConstParameter>());
    Pegasus::Uint32 cnt = method.getParameterCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_meth_parameters.get()->push_back(method.getParameter(i));

    // Store list of qualifiers for lazy evaluation
    fake_this.m_rc_meth_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    cnt = method.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_meth_qualifiers.get()->push_back(method.getQualifier(i));

    return inst;
}

Pegasus::CIMMethod CIMMethod::asPegasusCIMMethod()
{
    Pegasus::CIMMethod method(
        Pegasus::CIMName(m_name.c_str()),
        CIMTypeConv::asCIMType(m_return_type),
        Pegasus::CIMName(m_class_origin.c_str()),
        m_is_propagated);

    // Add all the parameters
    NocaseDict &parameters = NocaseDict::asNative(getPyParameters());
    nocase_map_t::const_iterator it;
    for (it = parameters.begin(); it != parameters.end(); ++it) {
        CIMParameter &parameter = CIMParameter::asNative(it->second);
        method.addParameter(parameter.asPegasusCIMParameter());
    }

    // Add all the qualifiers
    const NocaseDict &qualifiers = NocaseDict::asNative(getPyQualifiers());
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = CIMQualifier::asNative(it->second);
        method.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    return method;
}

#  if PY_MAJOR_VERSION < 3
int CIMMethod::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return 1;

    CIMMethod &other_method = CIMMethod::asNative(other);

    int rval;
    if ((rval = m_name.compare(other_method.m_name)) != 0 ||
        (rval = m_return_type.compare(other_method.m_return_type)) != 0 ||
        (rval = m_class_origin.compare(other_method.m_class_origin)) != 0 ||
        (rval = compare(bp::object(m_is_propagated),
            bp::object(other_method.m_is_propagated))) != 0 ||
        (rval = compare(getPyParameters(), other_method.getPyParameters())) != 0 ||
        (rval = compare(getPyQualifiers(), other_method.getPyQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMMethod::eq(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &other_method = CIMMethod::asNative(other);

    return m_name == other_method.m_name &&
        m_return_type == other_method.m_return_type &&
        m_class_origin == other_method.m_class_origin &&
        m_is_propagated == other_method.m_is_propagated &&
        compare(getPyParameters(), other_method.getPyParameters(), Py_EQ) &&
        compare(getPyQualifiers(), other_method.getPyQualifiers(), Py_EQ);
}

bool CIMMethod::gt(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &other_method = CIMMethod::asNative(other);

    return m_name > other_method.m_name ||
        m_return_type > other_method.m_return_type ||
        m_class_origin > other_method.m_class_origin ||
        m_is_propagated > other_method.m_is_propagated ||
        compare(getPyParameters(), other_method.getPyParameters(), Py_GT) ||
        compare(getPyQualifiers(), other_method.getPyQualifiers(), Py_GT);
}

bool CIMMethod::lt(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &other_method = CIMMethod::asNative(other);

    return m_name < other_method.m_name ||
        m_return_type < other_method.m_return_type ||
        m_class_origin < other_method.m_class_origin ||
        m_is_propagated < other_method.m_is_propagated ||
        compare(getPyParameters(), other_method.getPyParameters(), Py_LT) ||
        compare(getPyQualifiers(), other_method.getPyQualifiers(), Py_LT);
}

bool CIMMethod::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMMethod::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

bp::object CIMMethod::repr()
{
    std::stringstream ss;
    ss << "CIMMethod(name='" << m_name << "', return_type='" << m_return_type << "', ...)";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMMethod::tomof()
{
    std::stringstream ss;

    if (!m_return_type.empty())
        ss << m_return_type << ' ';
    ss << m_name << '(';

    const NocaseDict &parameters = NocaseDict::asNative(getPyParameters());
    nocase_map_t::const_iterator it;
    for (it = parameters.begin(); it != parameters.end(); ++it) {
        CIMParameter &parameter = CIMParameter::asNative(it->second);
        ss << parameter.tomof();
        nocase_map_t::const_iterator tmp_it = it;
        if (tmp_it != parameters.end() && ++tmp_it != parameters.end())
            ss << ", ";
    }
    ss << ");";

    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMMethod::copy()
{
    bp::object obj = CIMBase<CIMMethod>::create();
    CIMMethod &method = CIMMethod::asNative(obj);
    NocaseDict &parameters = NocaseDict::asNative(getPyParameters());
    NocaseDict &qualifiers = NocaseDict::asNative(getPyQualifiers());

    method.m_name = m_name;
    method.m_return_type = m_return_type;
    method.m_class_origin = m_class_origin;
    method.m_is_propagated = m_is_propagated;
    method.m_parameters = parameters.copy();
    method.m_qualifiers = qualifiers.copy();

    return obj;
}

std::string CIMMethod::getName() const
{
    return m_name;
}

std::string CIMMethod::getReturnType() const
{
    return m_return_type;
}

std::string CIMMethod::getClassOrigin() const
{
    return m_class_origin;
}

bool CIMMethod::getIsPropagated() const
{
    return m_is_propagated;
}


bp::object CIMMethod::getPyName() const
{
    return StringConv::asPyUnicode(m_name);
}

bp::object CIMMethod::getPyReturnType() const
{
    return StringConv::asPyUnicode(m_return_type);
}

bp::object CIMMethod::getPyClassOrigin() const
{
    return StringConv::asPyUnicode(m_class_origin);
}

bp::object CIMMethod::getPyIsPropagated() const
{
    return bp::object(m_is_propagated);
}

bp::object CIMMethod::getPyParameters()
{
    if (!m_rc_meth_parameters.empty()) {
        m_parameters = NocaseDict::create();
        std::list<Pegasus::CIMConstParameter>::const_iterator it;
        for (it = m_rc_meth_parameters.get()->begin();
             it != m_rc_meth_parameters.get()->end(); ++it)
        {
            m_parameters[bp::object(it->getName())] = CIMParameter::create(*it);
        }

        m_rc_meth_parameters.release();
    }
    return m_parameters;
}

bp::object CIMMethod::getPyQualifiers()
{
    if (!m_rc_meth_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = m_rc_meth_qualifiers.get()->begin();
             it != m_rc_meth_qualifiers.get()->end(); ++it)
        {
            m_qualifiers[bp::object(it->getName())] = CIMQualifier::create(*it);
        }

        m_rc_meth_qualifiers.release();
    }

    return m_qualifiers;
}

void CIMMethod::setName(const std::string &name)
{
    m_name = name;
}

void CIMMethod::setReturnType(const std::string &return_type)
{
    m_return_type = return_type;
}

void CIMMethod::setClassOrigin(const std::string &class_origin)
{
    m_class_origin = class_origin;
}

void CIMMethod::setIsPropagated(bool is_propagated)
{
    m_is_propagated = is_propagated;
}

void CIMMethod::setPyName(const bp::object &name)
{
    m_name = StringConv::asStdString(name, "name");
}

void CIMMethod::setPyReturnType(const bp::object &rtype)
{
    m_return_type = StringConv::asStdString(rtype, "return_type");
}

void CIMMethod::setPyClassOrigin(const bp::object &class_origin)
{
    m_class_origin = StringConv::asStdString(class_origin, "class_origin");
}

void CIMMethod::setPyIsPropagated(const bp::object &propagated)
{
    m_is_propagated = Conv::as<bool>(propagated, "propagated");
}

void CIMMethod::setPyParameters(const bp::object &parameters)
{
    m_parameters = Conv::get<NocaseDict, bp::dict>(parameters, "parameters");

    // Unref cached resource, it will never be used
    m_rc_meth_parameters.release();
}

void CIMMethod::setPyQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_meth_qualifiers.release();
}
