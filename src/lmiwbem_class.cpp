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
#include <boost/python/dict.hpp>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMMethod.h>
#include "lmiwbem_class.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_method.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_property.h"
#include "lmiwbem_qualifier.h"
#include "lmiwbem_util.h"

CIMClass::CIMClass()
    : m_classname()
    , m_super_classname()
    , m_properties()
    , m_qualifiers()
    , m_methods()
    , m_rc_class_properties()
    , m_rc_class_qualifiers()
    , m_rc_class_methods()
{
}

CIMClass::CIMClass(
    const bp::object &classname,
    const bp::object &properties,
    const bp::object &qualifiers,
    const bp::object &methods,
    const bp::object &superclass)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");
    m_super_classname = lmi::extract_or_throw<std::string>(superclass, "superclass");
    m_properties = lmi::get_or_throw<NocaseDict, bp::dict>(properties, "properties");
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");
    m_methods = lmi::get_or_throw<NocaseDict, bp::dict>(methods, "methods");
}

void CIMClass::init_type()
{
    CIMBase<CIMClass>::init_type(bp::class_<CIMClass>("CIMClass", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("classname"),
                bp::arg("properties") = NocaseDict::create(),
                bp::arg("qualifiers") = NocaseDict::create(),
                bp::arg("methods") = NocaseDict::create(),
                bp::arg("superclass") = std::string()),
                "Constructs a :py:class:`CIMClass`.\n\n"
                ":param str classname: String containing class name\n"
                ":param NocaseDict properties: Dictionary containing properties\n"
                "\tof the class\n"
                ":param NocaseDict qualifiers: Dictionary containing qualifiers\n"
                "\tof the class\n"
                ":param NocaseDict methods: Dictionary containing methods of the class\n"
                ":param str superclass: String containing the name of super-class"))
        .def("__cmp__", &CIMClass::cmp)
        .def("__repr__", &CIMClass::repr,
            ":returns: pretty string of the object")
        .def("copy", &CIMClass::copy)
        .add_property("classname",
            &CIMClass::m_classname,
            &CIMClass::setClassname,
            "Property storing class name.\n\n"
            ":returns: string of the class name")
        .add_property("superclass",
            &CIMClass::m_super_classname,
            &CIMClass::setSuperClassname,
            "Property storing super-class name.\n\n"
            ":returns: string containing super-class name")
        .add_property("properties",
            &CIMClass::getProperties,
            &CIMClass::setProperties,
            "Property storing properties.\n\n"
            ":returns: dictionary containing the properties\n"
            ":rtype: :py:class:`NocaseDict`")
        .add_property("qualifiers",
            &CIMClass::getQualifiers,
            &CIMClass::setQualifiers,
            "Property storing qualifiers.\n\n"
            ":returns: dictionary containing the qualifiers\n"
            ":rtype: :py:class:`NocaseDict`")
        .add_property("methods",
            &CIMClass::getMethods,
            &CIMClass::setMethods,
            "Property storing methods.\n\n"
            ":returns: dictionary containing the methods\n"
            ":rtype: :py:class:`NocaseDict`"));
}

bp::object CIMClass::create(const Pegasus::CIMClass &cls)
{
    bp::object inst = CIMBase<CIMClass>::create();
    CIMClass &fake_this = lmi::extract<CIMClass&>(inst);

    // Store list of properties for lazy evaluation
    fake_this.m_rc_class_properties.set(std::list<Pegasus::CIMConstProperty>());
    Pegasus::Uint32 cnt = cls.getPropertyCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_class_properties.get()->push_back(cls.getProperty(i));

    // Store list of qualifiersr for lazy evaluation
    fake_this.m_rc_class_qualifiers.set(std::list<Pegasus::CIMConstQualifier>());
    cnt = cls.getQualifierCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_class_qualifiers.get()->push_back(cls.getQualifier(i));

    // Store list of methods for lazy evaluation
    fake_this.m_rc_class_methods.set(std::list<Pegasus::CIMConstMethod>());
    cnt = cls.getMethodCount();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        fake_this.m_rc_class_methods.get()->push_back(cls.getMethod(i));

    fake_this.m_classname = cls.getClassName().getString().getCString();
    fake_this.m_super_classname = cls.getSuperClassName().getString().getCString();

    return inst;
}

bp::object CIMClass::create(const Pegasus::CIMObject &object)
{
    return create(Pegasus::CIMClass(object));
}

Pegasus::CIMClass CIMClass::asPegasusCIMClass()
{
    Pegasus::CIMClass cls(
        Pegasus::CIMName(m_classname.c_str()),
        Pegasus::CIMName(m_super_classname.c_str()));

    // Add all the properties
    const NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    nocase_map_t::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = lmi::extract<CIMProperty&>(it->second);
        cls.addProperty(property.asPegasusCIMProperty());
    }

    // Add all the qualifiers
    const NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = lmi::extract<CIMQualifier&>(it->second);
        cls.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    // Add all the methods
    const NocaseDict &methods = lmi::extract<NocaseDict&>(getMethods());
    for (it = methods.begin(); it != methods.end(); ++it) {
        CIMMethod &method = lmi::extract<CIMMethod&>(it->second);
        cls.addMethod(method.asPegasusCIMMethod());
    }

    return cls;
}

int CIMClass::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMClass::type()))
        return 1;

    CIMClass &other_class = lmi::extract<CIMClass&>(other);

    int rval;
    if ((rval = m_classname.compare(other_class.m_classname)) != 0 ||
        (rval = m_super_classname.compare(other_class.m_super_classname)) != 0 ||
        (rval = compare(getProperties(), other_class.getProperties())) != 0 ||
        (rval = compare(getQualifiers(), other_class.getQualifiers())) != 0 ||
        (rval = compare(getMethods(), other_class.getMethods())) != 0)
    {
        return rval;
    }

    return 0;
}

std::string CIMClass::repr()
{
    std::stringstream ss;
    ss << "CIMClass(classname='" << m_classname << "', ...)";
    return ss.str();
}

bp::object CIMClass::copy()
{
    bp::object obj = CIMBase<CIMClass>::create();
    CIMClass &cls = lmi::extract<CIMClass&>(obj);
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());
    NocaseDict &methods    = lmi::extract<NocaseDict&>(getMethods());

    cls.m_classname = m_classname;
    cls.m_super_classname = m_super_classname;
    cls.m_properties = properties.copy();
    cls.m_qualifiers = qualifiers.copy();
    cls.m_methods    = methods.copy();

    return obj;
}

bp::object CIMClass::getProperties()
{
    if (!m_rc_class_properties.empty()) {
        m_properties = NocaseDict::create();
        std::list<Pegasus::CIMConstProperty>::const_iterator it;
        std::list<Pegasus::CIMConstProperty> &properties = *m_rc_class_properties.get();

        for (it = properties.begin(); it != properties.end(); ++it)
            m_properties[bp::object(it->getName())] = CIMProperty::create(*it);

        m_rc_class_properties.release();
    }

    return m_properties;
}

bp::object CIMClass::getQualifiers()
{
    if (!m_rc_class_qualifiers.empty()) {
        m_qualifiers = NocaseDict::create();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        std::list<Pegasus::CIMConstQualifier> &qualifiers = *m_rc_class_qualifiers.get();

        for (it = qualifiers.begin(); it != qualifiers.end(); ++it)
            m_qualifiers[bp::object(it->getName())] = CIMQualifier::create(*it);

        m_rc_class_qualifiers.release();
    }

    return m_qualifiers;
}

bp::object CIMClass::getMethods()
{
    if (!m_rc_class_methods.empty()) {
        m_methods = NocaseDict::create();
        std::list<Pegasus::CIMConstMethod>::const_iterator it;
        std::list<Pegasus::CIMConstMethod> &methods = *m_rc_class_methods.get();

        for (it = methods.begin(); it != methods.end(); ++it)
            m_methods[bp::object(it->getName())] = CIMMethod::create(*it);

        m_rc_class_methods.release();
    }

    return m_methods;
}

void CIMClass::setClassname(const bp::object &classname)
{
    m_classname = lmi::extract_or_throw<std::string>(classname, "classname");
}

void CIMClass::setSuperClassname(const bp::object &superclassname)
{
    m_super_classname = lmi::extract_or_throw<std::string>(superclassname, "superclass");
}

void CIMClass::setProperties(const bp::object &properties)
{
    m_properties = lmi::get_or_throw<NocaseDict, bp::dict>(properties, "properties");

    // Unref cached resource, it will never be used
    m_rc_class_properties.release();
}

void CIMClass::setQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_class_qualifiers.release();
}

void CIMClass::setMethods(const bp::object &methods)
{
    m_methods = lmi::get_or_throw<NocaseDict, bp::dict>(methods, "methods");

    // Unref cached resource, it will never be used
    m_rc_class_methods.release();
}
