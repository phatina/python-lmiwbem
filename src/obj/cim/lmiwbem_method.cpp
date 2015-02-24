/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014-2015, Peter Hatina <phatina@redhat.com>
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
#include <Pegasus/Common/CIMMethod.h>
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_method.h"
#include "obj/cim/lmiwbem_method_pydoc.h"
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
    : m_name()
    , m_return_type()
    , m_class_origin()
    , m_is_propagated(false)
    , m_parameters()
    , m_qualifiers()
    , m_rc_meth_parameters()
    , m_rc_meth_qualifiers()
{
    m_name  = StringConv::asString(name, "name");

    if (!isnone(return_type))
        m_return_type = StringConv::asString(return_type, "return_type");

    if (!isnone(class_origin))
        m_class_origin = StringConv::asString(class_origin, "class_origin");

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
                bp::arg("name"),
                bp::arg("return_type") = None,
                bp::arg("parameters") = NocaseDict::create(),
                bp::arg("class_origin") = None,
                bp::arg("propagated") = false,
                bp::arg("qualifiers") = NocaseDict::create()),
                docstr_CIMMethod_init))
#if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMMethod::cmp)
#else
        .def("__eq__", &CIMMethod::eq)
        .def("__gt__", &CIMMethod::gt)
        .def("__lt__", &CIMMethod::lt)
        .def("__ge__", &CIMMethod::ge)
        .def("__le__", &CIMMethod::le)
#endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMMethod::repr, docstr_CIMMethod_repr)
        .def("copy", &CIMMethod::copy, docstr_CIMMethod_copy)
        .def("tomof", &CIMMethod::tomof, docstr_CIMMethod_tomof)
        .add_property("name",
            &CIMMethod::getPyName,
            &CIMMethod::setPyName)
        .add_property("return_type",
            &CIMMethod::getPyReturnType,
            &CIMMethod::setPyReturnType)
        .add_property("parameters",
            &CIMMethod::getPyParameters,
            &CIMMethod::setPyParameters)
        .add_property("class_origin",
            &CIMMethod::getPyClassOrigin,
            &CIMMethod::setPyClassOrigin)
        .add_property("propagated",
            &CIMMethod::getPyIsPropagated,
            &CIMMethod::setPyIsPropagated)
        .add_property("qualifiers",
            &CIMMethod::getPyQualifiers,
            &CIMMethod::setPyQualifiers));
}

bp::object CIMMethod::create(const Pegasus::CIMConstMethod &method)
{
    bp::object py_inst = CIMBase<CIMMethod>::create();
    CIMMethod &fake_this = CIMMethod::asNative(py_inst);
    fake_this.m_name = method.getName().getString();
    fake_this.m_return_type = CIMTypeConv::asString(method.getType());
    fake_this.m_class_origin = method.getClassOrigin().getString();
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

    return py_inst;
}

Pegasus::CIMMethod CIMMethod::asPegasusCIMMethod()
{
    Pegasus::CIMMethod peg_method(
        Pegasus::CIMName(m_name),
        CIMTypeConv::asCIMType(m_return_type),
        Pegasus::CIMName(m_class_origin),
        m_is_propagated);

    // Add all the parameters
    NocaseDict &cim_parameters = NocaseDict::asNative(getPyParameters());
    nocase_map_t::const_iterator it;
    for (it = cim_parameters.begin(); it != cim_parameters.end(); ++it) {
        CIMParameter &cim_parameter = CIMParameter::asNative(it->second);
        peg_method.addParameter(cim_parameter.asPegasusCIMParameter());
    }

    // Add all the qualifiers
    const NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());
    for (it = cim_qualifiers.begin(); it != cim_qualifiers.end(); ++it) {
        CIMQualifier &cim_qualifier = CIMQualifier::asNative(it->second);
        peg_method.addQualifier(cim_qualifier.asPegasusCIMQualifier());
    }

    return peg_method;
}

#if PY_MAJOR_VERSION < 3
int CIMMethod::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return 1;

    CIMMethod &cim_other = CIMMethod::asNative(other);

    int rval;
    if ((rval = m_name.compare(cim_other.m_name)) != 0 ||
        (rval = m_return_type.compare(cim_other.m_return_type)) != 0 ||
        (rval = m_class_origin.compare(cim_other.m_class_origin)) != 0 ||
        (rval = compare(bp::object(m_is_propagated),
            bp::object(cim_other.m_is_propagated))) != 0 ||
        (rval = compare(getPyParameters(), cim_other.getPyParameters())) != 0 ||
        (rval = compare(getPyQualifiers(), cim_other.getPyQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}
#else
bool CIMMethod::eq(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &cim_other = CIMMethod::asNative(other);

    return m_name == cim_other.m_name &&
        m_return_type == cim_other.m_return_type &&
        m_class_origin == cim_other.m_class_origin &&
        m_is_propagated == cim_other.m_is_propagated &&
        compare(getPyParameters(), cim_other.getPyParameters(), Py_EQ) &&
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_EQ);
}

bool CIMMethod::gt(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &cim_other = CIMMethod::asNative(other);

    return m_name > cim_other.m_name ||
        m_return_type > cim_other.m_return_type ||
        m_class_origin > cim_other.m_class_origin ||
        m_is_propagated > cim_other.m_is_propagated ||
        compare(getPyParameters(), cim_other.getPyParameters(), Py_GT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_GT);
}

bool CIMMethod::lt(const bp::object &other)
{
    if (!isinstance(other, CIMMethod::type()))
        return false;

    CIMMethod &cim_other = CIMMethod::asNative(other);

    return m_name < cim_other.m_name ||
        m_return_type < cim_other.m_return_type ||
        m_class_origin < cim_other.m_class_origin ||
        m_is_propagated < cim_other.m_is_propagated ||
        compare(getPyParameters(), cim_other.getPyParameters(), Py_LT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_LT);
}

bool CIMMethod::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMMethod::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#endif // PY_MAJOR_VERSION

bp::object CIMMethod::repr()
{
    std::stringstream ss;
    ss << "CIMMethod(name=u'" << m_name << "', return_type=u'" << m_return_type << "', ...)";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMMethod::tomof()
{
    std::stringstream ss;

    if (!m_return_type.empty())
        ss << m_return_type << ' ';
    ss << m_name << '(';

    const NocaseDict &cim_parameters = NocaseDict::asNative(getPyParameters());
    nocase_map_t::const_iterator it;
    for (it = cim_parameters.begin(); it != cim_parameters.end(); ++it) {
        CIMParameter &parameter = CIMParameter::asNative(it->second);
        ss << parameter.tomof();
        nocase_map_t::const_iterator tmp_it = it;
        if (tmp_it != cim_parameters.end() && ++tmp_it != cim_parameters.end())
            ss << ", ";
    }
    ss << ");";

    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMMethod::copy()
{
    bp::object py_inst = CIMBase<CIMMethod>::create();
    CIMMethod &cim_method = CIMMethod::asNative(py_inst);
    NocaseDict &cim_parameters = NocaseDict::asNative(getPyParameters());
    NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());

    cim_method.m_name = m_name;
    cim_method.m_return_type = m_return_type;
    cim_method.m_class_origin = m_class_origin;
    cim_method.m_is_propagated = m_is_propagated;
    cim_method.m_parameters = cim_parameters.copy();
    cim_method.m_qualifiers = cim_qualifiers.copy();

    return py_inst;
}

String CIMMethod::getName() const
{
    return m_name;
}

String CIMMethod::getReturnType() const
{
    return m_return_type;
}

String CIMMethod::getClassOrigin() const
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

void CIMMethod::setName(const String &name)
{
    m_name = name;
}

void CIMMethod::setReturnType(const String &return_type)
{
    m_return_type = return_type;
}

void CIMMethod::setClassOrigin(const String &class_origin)
{
    m_class_origin = class_origin;
}

void CIMMethod::setIsPropagated(bool is_propagated)
{
    m_is_propagated = is_propagated;
}

void CIMMethod::setPyName(const bp::object &name)
{
    m_name = StringConv::asString(name, "name");
}

void CIMMethod::setPyReturnType(const bp::object &rtype)
{
    m_return_type = StringConv::asString(rtype, "return_type");
}

void CIMMethod::setPyClassOrigin(const bp::object &class_origin)
{
    m_class_origin = StringConv::asString(class_origin, "class_origin");
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
