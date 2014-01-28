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

#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include "lmiwbem_class_name.h"
#include "lmiwbem_extract.h"

bp::object CIMClassName::s_class;

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
    s_class = bp::class_<CIMClassName>("CIMClassName", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("classname"),
                bp::arg("host") = std::string(),
                bp::arg("namespace") = std::string()),
                "Constructs a :py:class:`CIMClassName`.\n\n"
                ":param str classname: String containing class name\n"
                ":param str host: String containing host name\n"
                ":param str namespace: String containing namespace name"))
        .def("__repr__", &CIMClassName::repr,
            ":returns: pretty string of the object")
        .add_property("classname",
            &CIMClassName::m_classname,
            &CIMClassName::setClassname,
            "Property storing class name.\n\n"
            ":returns: string containing class name")
        .add_property("namespace",
            &CIMClassName::m_namespace,
            &CIMClassName::setNamespace,
            "Property storing namespace name.\n\n"
            ":returns: string containing namesapce name")
        .add_property("host",
            &CIMClassName::m_hostname,
            &CIMClassName::setHostname,
            "Property storing host name.\n\n"
            ":returns: string containing host name")
        ;
}

bp::object CIMClassName::create(
    const bp::object &classname,
    const bp::object &namespace_,
    const bp::object &hostname)
{
    return s_class(classname, namespace_, hostname);
}

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
