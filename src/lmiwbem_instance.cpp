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
#include <boost/python/list.hpp>
#include <boost/python/str.hpp>
#include "lmiwbem_extract.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_property.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"

namespace bp = boost::python;

CIMInstance::CIMInstance()
    : m_classname()
    , m_path()
    , m_properties()
    , m_qualifiers()
    , m_property_list()
    , m_rc_inst_path()
    , m_rc_inst_properties()
    , m_rc_inst_qualifiers()
{
}

CIMInstance::CIMInstance(
    const bp::object &classname,
    const bp::object &properties,
    const bp::object &qualifiers,
    const bp::object &path,
    const bp::object &property_list)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");

    // We store properties in NocaseDict. Convert python's dict, if necessary.
    if (properties == bp::object())
        m_properties = NocaseDict::create();
    else if (lmi::extract<bp::dict>(properties).check())
        m_properties = NocaseDict::create(properties);
    else
        m_properties = lmi::get_or_throw<NocaseDict>(properties, "properties");

    // Convert plain values
    NocaseDict &prop_dict = lmi::extract<NocaseDict&>(m_properties);
    if (!prop_dict.empty() && !isinstance(prop_dict.begin()->second, CIMProperty::type())) {
        nocase_map_t::iterator it;
        for (it = prop_dict.begin(); it != prop_dict.end(); ++it) {
            it->second = CIMProperty::create(bp::object(it->first), it->second);
        }
    }

    // We store qualifiers in NocaseDict. Convert python's dict, if necessary.
    if (qualifiers == bp::object())
        m_qualifiers = NocaseDict::create();
    else if (lmi::extract<bp::dict>(qualifiers).check())
        m_qualifiers = NocaseDict::create(qualifiers);
    else
        m_qualifiers = lmi::get_or_throw<NocaseDict>(qualifiers, "qualifiers");

    m_path = lmi::get_or_throw<CIMInstanceName, bp::object>(path, "path");
    m_property_list = lmi::get_or_throw<bp::list, bp::object>(property_list, "property_list");
}

void CIMInstance::init_type()
{
    CIMBase::s_class = bp::class_<CIMInstance>("CIMInstance", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("classname"),
                bp::arg("properties") = NocaseDict::create(),
                bp::arg("qualifiers") = NocaseDict::create(),
                bp::arg("path") = bp::object(),
                bp::arg("property_list") = bp::object()),
                "Constructs a :py:class:`CIMInstance`.\n\n"
                ":param str classname: String containing a class name\n"
                ":param NocaseDict properties: Dictionary containing :py:class:`CIMProperty`\n"
                ":param NocaseDict qualifiers: Dictionary containing :py:class:`CIMQualifier`\n"
                ":param CIMInstanceName path: Object path\n"
                ":param list property_list: List containing strings of properties"))
        .def("__repr__", &CIMInstance::repr,
            ":returns: pretty string of the object")
        .add_property("classname",
            &CIMInstance::getClassname,
            &CIMInstance::setClassname,
            "Property storing class name\n\n"
            ":returns: class name\n"
            ":rtype: string")
        .add_property("path",
            &CIMInstance::getPath,
            &CIMInstance::setPath,
            "Property storing object path\n\n"
            ":returns: object path\n"
            ":rtype: :py:class:`CIMInstanceName`")
        .add_property("properties",
            &CIMInstance::getProperties,
            &CIMInstance::setProperties,
            "Property storing instance properties\n\n"
            ":returns: dictionary containing instance properties\n"
            ":rtype: :py:class:`NocaseDict`")
        .add_property("qualifiers",
            &CIMInstance::getQualifiers,
            &CIMInstance::setQualifiers,
            "Property storing instance qualifiers\n\n"
            ":returns: dictionary containing instance qualifiers\n"
            ":rtype: :py:class:`NocaseDict`")
        .add_property("property_list",
            &CIMInstance::getPropertyList,
            &CIMInstance::setPropertyList,
            "Property storing instance properties\n\n"
            ":returns: list of instance properties\n")
        ;
}

bp::object CIMInstance::create(const Pegasus::CIMInstance &instance)
{
    bp::object inst = CIMBase::s_class();
    CIMInstance &fake_this = lmi::extract<CIMInstance&>(inst);
    fake_this.m_classname = instance.getClassName().getString().getCString();

    // Store path for lazy evaluation
    fake_this.m_rc_inst_path.set(instance.getPath());

    // Store list of properties for lazy evaluation
    fake_this.m_rc_inst_properties.set(std::list<Pegasus::CIMConstProperty>());
    Pegasus::Uint32 cnt = instance.getPropertyCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_inst_properties.get()->push_back(instance.getProperty(i));

    // Store list of qualifiers for lazy evaluation
    fake_this.m_rc_inst_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    cnt = instance.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_inst_qualifiers.get()->push_back(instance.getQualifier(i));

    return inst;
}

bp::object CIMInstance::create(const Pegasus::CIMObject &object)
{
    return create(Pegasus::CIMInstance(object));
}

Pegasus::CIMInstance CIMInstance::asPegasusCIMInstance()
{
    Pegasus::CIMInstance instance(Pegasus::CIMName(m_classname.c_str()));
    const CIMInstanceName &path = lmi::extract<CIMInstanceName&>(getPath());

    // Set CIMObjectPath
    instance.setPath(path.asPegasusCIMObjectPath());

    // Add all the properties
    const NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    nocase_map_t::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = lmi::extract<CIMProperty&>(it->second);
        instance.addProperty(property.asPegasusCIMProperty());
    }

    // Add all the qualifiers
    const NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = lmi::extract<CIMQualifier&>(it->second);
        instance.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    return instance;
}

std::string CIMInstance::repr()
{
    std::stringstream ss;
    ss << "CIMInstance(classname='" << m_classname << "', ...)";
    return ss.str();
}

bp::object CIMInstance::getClassname()
{
    return std_string_as_pyunicode(m_classname);
}

bp::object CIMInstance::getPath()
{
    if (!m_rc_inst_path.empty()) {
        m_path = CIMInstanceName::create(*m_rc_inst_path.get());
        m_rc_inst_path.unref();
    }

    return m_path;
}

bp::object CIMInstance::getProperties()
{
    evalProperties();
    return m_properties;
}

bp::object CIMInstance::getQualifiers()
{
    if (!m_rc_inst_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier> &qualifiers = *m_rc_inst_qualifiers.get();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
            bp::object qualif_name(it->getName());
            bp::setitem(m_qualifiers, qualif_name, CIMQualifier::create(*it));
        }
        m_rc_inst_qualifiers.unref();
    }

    return m_qualifiers;
}

bp::object CIMInstance::getPropertyList()
{
    evalProperties();
    return m_property_list;
}

void CIMInstance::setClassname(const bp::object &classname)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");
}

void CIMInstance::setPath(const bp::object &path)
{
    m_path = lmi::get_or_throw<CIMInstanceName>(path);

    // Unref cached resource, it will never be used
    m_rc_inst_path.unref();
}

void CIMInstance::setProperties(const bp::object &properties)
{
    if (!m_rc_inst_properties.empty())
        evalProperties();

    m_properties = lmi::get_or_throw<NocaseDict, bp::dict>(properties, "properties");

    // Unref cached resource, it will never be used
    m_rc_inst_properties.unref();
}

void CIMInstance::setQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_inst_qualifiers.unref();
}

void CIMInstance::setPropertyList(const bp::object &property_list)
{
    if (!m_rc_inst_properties.empty())
        evalProperties();

    m_property_list = lmi::get_or_throw<bp::list>(property_list);

    // Unref cached resource, it will never be used
    m_rc_inst_properties.unref();
}

void CIMInstance::evalProperties()
{
    if (m_rc_inst_properties.empty())
        return;

    m_properties = NocaseDict::create();
    bp::list property_list;
    std::list<Pegasus::CIMConstProperty>::const_iterator it;
    std::list<Pegasus::CIMConstProperty> &properties = *m_rc_inst_properties.get();
    for (it = properties.begin(); it != properties.end(); ++it) {
        bp::object prop_name(it->getName());
        bp::setitem(m_properties, prop_name, CIMProperty::create(*it));
        property_list.append(prop_name);
    }

    m_property_list = property_list;
    m_rc_inst_properties.unref();
}
