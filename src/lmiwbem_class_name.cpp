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
#include "lmiwbem_class_name.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_util.h"

CIMClassName::CIMClassName()
    : m_classname()
    , m_namespace()
    , m_hostname()
{
}

CIMClassName::CIMClassName(
    const bp::object &classname,
    const bp::object &namespace_,
    const bp::object &hostname)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");
    m_namespace = lmi::extract_or_throw<std::string>(namespace_, "namespace");
    m_hostname  = lmi::extract_or_throw<std::string>(hostname, "hostname");
}

void CIMClassName::init_type()
{
    CIMBase<CIMClassName>::init_type(
        bp::class_<CIMClassName>("CIMClassName", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("classname"),
                bp::arg("host") = std::string(),
                bp::arg("namespace") = std::string()),
                "Constructs a :py:class:`.CIMClassName`.\n\n"
                ":param str classname: String containing class name\n"
                ":param str host: String containing host name\n"
                ":param str namespace: String containing namespace name"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMClassName::cmp)
#  else
        .def("__eq__", &CIMClassName::eq)
        .def("__gt__", &CIMClassName::gt)
        .def("__lt__", &CIMClassName::lt)
        .def("__ge__", &CIMClassName::ge)
        .def("__le__", &CIMClassName::le)
#  endif
        .def("__repr__", &CIMClassName::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMClassName::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMClassName`")
        .add_property("classname",
            &CIMClassName::getClassname,
            &CIMClassName::setClassname,
            "Property storing class name.\n\n"
            ":rtype: str")
        .add_property("namespace",
            &CIMClassName::getNamespace,
            &CIMClassName::setNamespace,
            "Property storing namespace name.\n\n"
            ":rtype: str")
        .add_property("host",
            &CIMClassName::getHostname,
            &CIMClassName::setHostname,
            "Property storing host name.\n\n"
            ":rtype: str"));
}

bp::object CIMClassName::create(
    const std::string &classname_,
    const std::string &namespace_,
    const std::string &hostname)
{
    bp::object inst = CIMBase<CIMClassName>::create();
    CIMClassName &classname = lmi::extract<CIMClassName&>(inst);

    classname.m_classname = classname_;
    classname.m_namespace = namespace_;
    classname.m_hostname  = hostname;

    return inst;
}

#  if PY_MAJOR_VERSION < 3
int CIMClassName::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMClassName::type()))
        return 1;

    CIMClassName &other_classname = lmi::extract<CIMClassName&>(other);

    int rval;
    if ((rval = m_classname.compare(other_classname.m_classname)) != 0 ||
        (rval = m_namespace.compare(other_classname.m_namespace)) != 0 ||
        (rval = m_hostname.compare(other_classname.m_hostname)) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMClassName::eq(const bp::object &other)
{
    if (!isinstance(other, CIMClassName::type()))
        return false;

    CIMClassName &other_classname = lmi::extract<CIMClassName&>(other);

    return m_classname == other_classname.m_classname &&
        m_namespace == other_classname.m_namespace &&
        m_hostname  == other_classname.m_hostname;
}

bool CIMClassName::gt(const bp::object &other)
{
    if (!isinstance(other, CIMClassName::type()))
        return false;

    CIMClassName &other_classname = lmi::extract<CIMClassName&>(other);

    return m_classname > other_classname.m_classname ||
        m_namespace > other_classname.m_namespace ||
        m_hostname  > other_classname.m_hostname;
}

bool CIMClassName::lt(const bp::object &other)
{
    if (!isinstance(other, CIMClassName::type()))
        return false;

    CIMClassName &other_classname = lmi::extract<CIMClassName&>(other);

    return m_classname < other_classname.m_classname ||
        m_namespace < other_classname.m_namespace ||
        m_hostname  < other_classname.m_hostname;
}

bool CIMClassName::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMClassName::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif

std::string CIMClassName::repr()
{
    std::stringstream ss;
    ss << "CIMClassName(classname='" << m_classname << '\'';
    if (!m_hostname.empty())
        ss << ", host='" << m_hostname << '\'';
    if (!m_namespace.empty())
        ss << ", namespace='" << m_namespace << '\'';
    ss << ')';
    return ss.str();
}

bp::object CIMClassName::copy()
{
    bp::object obj = CIMBase<CIMClassName>::create();
    CIMClassName &classname = lmi::extract<CIMClassName&>(obj);

    classname.m_classname = m_classname;
    classname.m_namespace = m_namespace;
    classname.m_hostname  = m_hostname;

    return obj;
}

void CIMClassName::setClassname(const bp::object &classname)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");
}

void CIMClassName::setNamespace(const bp::object &namespace_)
{
    m_namespace = lmi::extract_or_throw<std::string>(namespace_, "namespace");
}

void CIMClassName::setHostname(const bp::object &hostname)
{
    m_hostname = lmi::extract_or_throw<std::string>(hostname, "hostname");
}
