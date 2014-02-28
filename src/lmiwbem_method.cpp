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

#include <list>
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include "lmiwbem_extract.h"
#include "lmiwbem_method.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_parameter.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"

CIMMethod::CIMMethod()
    : m_name()
    , m_return_type()
    , m_class_origin()
    , m_propagated(false)
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
    m_name  = lmi::extract_or_throw<std::string>(name, "name");
    m_return_type = lmi::extract_or_throw<std::string>(return_type, "return_type");
    m_class_origin = lmi::extract_or_throw<std::string>(class_origin, "class_origin");
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");

    lmi::extract<bp::dict> ext_parameters(parameters);
    if (ext_parameters.check())
        m_parameters = NocaseDict::create(parameters);
    else
        m_parameters = lmi::get_or_throw<NocaseDict>(parameters, "parameters");

    lmi::extract<bp::dict> ext_qualifiers(qualifiers);
    if (ext_qualifiers.check())
        m_qualifiers = NocaseDict::create(qualifiers);
    else
        m_qualifiers = lmi::get_or_throw<NocaseDict>(qualifiers, "qualifiers");
}

void CIMMethod::init_type()
{
    CIMBase::init_type(bp::class_<CIMMethod>("CIMMethod", bp::init<>())
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
                "Constructs a :py:class:`CIMMethod`.\n\n"
                ":param str methodname: String containing the method's name\n"
                ":param str return_type: String containing the method's return type\n"
                ":param NocaseDict parameters: Dictionary containing method's parameters\n"))
        .def("__cmp__", &CIMMethod::cmp)
        .def("__repr__", &CIMMethod::repr,
            ":returns: pretty string of the object")
        .def("tomof", &CIMMethod::tomof)
        .def("copy", &CIMMethod::copy)
        .add_property("name",
            &CIMMethod::getName,
            &CIMMethod::setName,
            "Property storing method's name.\n\n"
            ":returns: string containing the method's name")
        .add_property("return_type",
            &CIMMethod::getReturnType,
            &CIMMethod::setReturnType,
            "Property storing method's return type.\n\n"
            ":returns: string containing the method's return type\n")
        .add_property("parameters",
            &CIMMethod::getParameters,
            &CIMMethod::setParameters,
            "Property storing method's parameters.\n\n"
            ":returns: dictionary containing the method's parameters\n"
            ":rtype: :py:class:`NocaseDict`")
        .add_property("class_origin",
            &CIMMethod::getClassOrigin,
            &CIMMethod::setClassOrigin,
            "Property storing method's class origin.\n\n"
            ":returns: string containing the method's class origin")
        .add_property("propagated",
            &CIMMethod::getPropagated,
            &CIMMethod::setPropagated,
            "Property storing flag, which indicates, if the method is propagated.\n\n"
            ":returns: True, if the method is propagated; False otherwise\n"
            ":rtype: bool")
        .add_property("qualifiers",
            &CIMMethod::getQualifiers,
            &CIMMethod::setQualifiers,
            "Property storing method's qualifiers.\n\n"
            ":returns: dictionary containing the method's qualifiers\n"
            ":rtype: :py:class:`NocaseDict`"));
}

bp::object CIMMethod::create(const Pegasus::CIMConstMethod &method)
{
    bp::object inst = CIMBase::create();
    CIMMethod &fake_this = lmi::extract<CIMMethod&>(inst);
    fake_this.m_name = method.getName().getString().getCString();
    fake_this.m_return_type = CIMTypeConv::asStdString(method.getType());
    fake_this.m_class_origin = method.getClassOrigin().getString().getCString();
    fake_this.m_propagated = method.getPropagated();

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
        m_propagated);

    // Add all the parameters
    NocaseDict &parameters = lmi::extract<NocaseDict&>(getParameters());
    nocase_map_t::const_iterator it;
    for (it = parameters.begin(); it != parameters.end(); ++it) {
        CIMParameter &parameter = lmi::extract<CIMParameter&>(it->second);
        method.addParameter(parameter.asPegasusCIMParameter());
    }

    // Add all the qualifiers
    const NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = lmi::extract<CIMQualifier&>(it->second);
        method.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    return method;
}

int CIMMethod::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return 1;

    CIMMethod &other_method = lmi::extract<CIMMethod&>(other);

    int rval;
    if ((rval = m_name.compare(other_method.m_name)) != 0 ||
        (rval = m_return_type.compare(other_method.m_return_type)) != 0 ||
        (rval = m_class_origin.compare(other_method.m_class_origin)) != 0 ||
        (rval = compare(bp::object(m_propagated),
            bp::object(other_method.m_propagated))) != 0 ||
        (rval = compare(getParameters(), other_method.getParameters())) != 0 ||
        (rval = compare(getQualifiers(), other_method.getQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}

std::string CIMMethod::repr()
{
    std::stringstream ss;
    ss << "CIMMethod(name='" << m_name << "', return_type='" << m_return_type << "', ...)";
    return ss.str();
}

std::string CIMMethod::tomof()
{
    std::stringstream ss;

    if (!m_return_type.empty())
        ss << m_return_type << ' ';
    ss << m_name << '(';

    const NocaseDict &parameters = lmi::extract<NocaseDict&>(getParameters());
    nocase_map_t::const_iterator it;
    for (it = parameters.begin(); it != parameters.end(); ++it) {
        CIMParameter &parameter = lmi::extract_or_throw<CIMParameter&>(it->second);
        ss << parameter.tomof();
        nocase_map_t::const_iterator tmp_it = it;
        if (tmp_it != parameters.end() && ++tmp_it != parameters.end())
            ss << ", ";
    }
    ss << ");";

    return ss.str();
}

bp::object CIMMethod::copy()
{
    bp::object obj = CIMBase::create();
    CIMMethod &method = lmi::extract<CIMMethod&>(obj);
    NocaseDict &parameters = lmi::extract<NocaseDict&>(getParameters());
    NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());

    method.m_name = m_name;
    method.m_return_type = m_return_type;
    method.m_class_origin = m_class_origin;
    method.m_propagated = m_propagated;
    method.m_parameters = parameters.copy();
    method.m_qualifiers = qualifiers.copy();

    return obj;
}

bp::object CIMMethod::getParameters()
{
    if (!m_rc_meth_parameters.empty()) {
        m_parameters = NocaseDict::create();
        std::list<Pegasus::CIMConstParameter>::const_iterator it;
        for (it = m_rc_meth_parameters.get()->begin(); it != m_rc_meth_parameters.get()->end(); ++it) {
            bp::object name = std_string_as_pyunicode(
                std::string(it->getName().getString().getCString()));
            m_parameters[name] = CIMParameter::create(*it);
        }

        m_rc_meth_parameters.unref();
    }
    return m_parameters;
}

bp::object CIMMethod::getQualifiers()
{
    if (!m_rc_meth_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = m_rc_meth_qualifiers.get()->begin();
            it != m_rc_meth_qualifiers.get()->end(); ++it)
        {
            bp::object name = std_string_as_pyunicode(
                std::string(it->getName().getString().getCString()));
            m_qualifiers[name] = CIMQualifier::create(*it);
        }

        m_rc_meth_qualifiers.unref();
    }

    return m_qualifiers;
}

void CIMMethod::setName(const bp::object &name)
{
    m_name = lmi::extract_or_throw<std::string>(name, "name");
}

void CIMMethod::setReturnType(const bp::object &rtype)
{
    m_return_type = lmi::extract_or_throw<std::string>(rtype, "return_type");
}

void CIMMethod::setParameters(const bp::object &parameters)
{
    m_parameters = lmi::get_or_throw<NocaseDict, bp::dict>(parameters, "parameters");

    // Unref cached resource, it will never be used
    m_rc_meth_parameters.unref();
}

void CIMMethod::setClassOrigin(const bp::object &class_origin)
{
    m_class_origin = lmi::extract_or_throw<std::string>(class_origin, "class_origin");
}

void CIMMethod::setPropagated(const bp::object &propagated)
{
    m_propagated = lmi::extract_or_throw<bool>(propagated, "propagated");
}

void CIMMethod::setQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_meth_qualifiers.unref();
}
