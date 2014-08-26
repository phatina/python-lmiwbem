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
    if (isnone(properties))
        m_properties = NocaseDict::create();
    else
        m_properties = NocaseDict::create(properties);

    // Convert plain values
    NocaseDict &prop_dict = lmi::extract<NocaseDict&>(m_properties);
    nocase_map_t::iterator it;
    for (it = prop_dict.begin(); it != prop_dict.end(); ++it) {
        if (!isinstance(it->second, CIMProperty::type()))
            it->second = CIMProperty::create(bp::object(it->first), it->second);
    }

    // We store qualifiers in NocaseDict. Convert python's dict, if necessary.
    if (isnone(qualifiers))
        m_qualifiers = NocaseDict::create();
    else
        m_qualifiers = NocaseDict::create(qualifiers);

    m_path = lmi::get_or_throw<CIMInstanceName, bp::object>(path, "path");
    m_property_list = lmi::get_or_throw<bp::list, bp::object>(property_list, "property_list");
}

void CIMInstance::init_type()
{
    CIMBase<CIMInstance>::init_type(bp::class_<CIMInstance>("CIMInstance", bp::init<>())
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
                "Constructs a :py:class:`.CIMInstance`.\n\n"
                ":param str classname: String containing a class name\n"
                ":param NocaseDict properties: Dictionary containing :py:class:`.CIMProperty`\n"
                ":param NocaseDict qualifiers: Dictionary containing :py:class:`.CIMQualifier`\n"
                ":param CIMInstanceName path: Object path\n"
                ":param list property_list: List containing strings of properties"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMInstance::cmp)
#  else
        .def("__eq__", &CIMInstanceName::eq)
        .def("__gt__", &CIMInstanceName::gt)
        .def("__lt__", &CIMInstanceName::lt)
        .def("__ge__", &CIMInstanceName::ge)
        .def("__le__", &CIMInstanceName::le)
#endif // PY_MAJOR_VERSION
        .def("__repr__", &CIMInstance::repr,
            ":returns: pretty string of the object")
        .def("__getitem__", &CIMInstance::getitem)
        .def("__setitem__", &CIMInstance::setitem)
        .def("__contains__", &CIMInstance::haskey)
        .def("__len__", &CIMInstance::len)
        .def("has_key", &CIMInstance::haskey,
            "has_key(key)\n\n"
            ":param str key: key to check for presence in object's properties\n"
            ":returns: True, if the key is present in object's properties;\n"
            "\tFalse otherwise\n"
            ":rtype: bool")
        .def("keys", &CIMInstance::keys,
            "keys()\n\n"
            ":returns: list of strings of property names\n"
            ":rtype: list")
        .def("values", &CIMInstance::values,
            "values()\n\n"
            ":returns: list of property values\n"
            ":rtype: list")
        .def("items", &CIMInstance::items,
            "items()\n\n"
            ":returns: list of tuples with property name and value\n"
            ":rtype: list")
        .def("iterkeys", &CIMInstance::iterkeys,
            "iterkeys()\n\n"
            ":returns: iterator for property names iteration\n"
            ":rtype: iterator")
        .def("itervalues", &CIMInstance::itervalues,
            "itervalues()\n\n"
            ":returns: iterator for property values iteration\n"
            ":rtype: iterator")
        .def("iteritems", &CIMInstance::iteritems,
            "iteritems()\n\n"
            ":returns: iterator for property name and value iteration\n"
            ":rtype: iterator")
        .def("copy", &CIMInstance::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMInstance`")
        .def("tomof", &CIMInstance::tomof,
            "tomof()\n\n"
            ":returns: MOF representation of the object itself\n"
            ":rtype: str")
        .add_property("classname",
            &CIMInstance::getClassname,
            &CIMInstance::setClassname,
            "Property storing class name\n\n"
            ":rtype: str")
        .add_property("path",
            &CIMInstance::getPath,
            &CIMInstance::setPath,
            "Property storing object path\n\n"
            ":rtype: :py:class:`.CIMInstanceName`")
        .add_property("properties",
            &CIMInstance::getProperties,
            &CIMInstance::setProperties,
            "Property storing instance properties\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("qualifiers",
            &CIMInstance::getQualifiers,
            &CIMInstance::setQualifiers,
            "Property storing instance qualifiers\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("property_list",
            &CIMInstance::getPropertyList,
            &CIMInstance::setPropertyList,
            "Property storing instance properties\n\n"
            ":rtype: list of strings of instance properties\n"));
}

bp::object CIMInstance::create(const Pegasus::CIMInstance &instance)
{
    bp::object inst = CIMBase<CIMInstance>::create();
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

    if (!isnone(getPath())) {
        // Set CIMObjectPath
        const CIMInstanceName &path = lmi::extract<CIMInstanceName&>(getPath());
        instance.setPath(path.asPegasusCIMObjectPath());
    }

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

#  if PY_MAJOR_VERSION < 3
int CIMInstance::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return 1;

    CIMInstance &other_inst = lmi::extract<CIMInstance&>(other);

    int rval;
    if ((rval = m_classname.compare(other_inst.m_classname)) != 0 ||
        (rval = compare(getPath(), other_inst.getPath())) != 0 ||
        (rval = compare(getProperties(), other_inst.getProperties())) != 0 ||
        (rval = compare(getQualifiers(), other_inst.getQualifiers())) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMInstance::eq(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &other_inst = lmi::extract<CIMInstance&>(other);

    return m_classname == other_inst.m_classname &&
        compare(getPath(), other_inst.getPath(), Py_EQ) &&
        compare(getProperties(), other_inst.getProperties(), Py_EQ) &&
        compare(getQualifiers(), other_inst.getQualifiers(), Py_EQ);
}

bool CIMInstance::gt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &other_inst = lmi::extract<CIMInstance&>(other);

    return m_classname > other_inst.m_classname ||
        compare(getPath(), other_inst.getPath(), Py_GT) ||
        compare(getProperties(), other_inst.getProperties(), Py_GT) ||
        compare(getQualifiers(), other_inst.getQualifiers(), Py_GT);
}

bool CIMInstance::lt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &other_inst = lmi::extract<CIMInstance&>(other);

    return m_classname < other_inst.m_classname ||
        compare(getPath(), other_inst.getPath(), Py_LT) ||
        compare(getProperties(), other_inst.getProperties(), Py_LT) ||
        compare(getQualifiers(), other_inst.getQualifiers(), Py_LT);
}

bool CIMInstance::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMInstance::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

std::string CIMInstance::repr()
{
    std::stringstream ss;
    ss << "CIMInstance(classname='" << m_classname << "', ...)";
    return ss.str();
}

bp::object CIMInstance::getitem(const bp::object &key)
{
    evalProperties();
    lmi::extract<CIMProperty&> ext_property(m_properties[key]);

    if (ext_property.check())
        return static_cast<CIMProperty&>(ext_property).getValue();
    return m_properties[key];
}

void CIMInstance::setitem(
    const bp::object &key,
    const bp::object &value)
{
    evalProperties();
    lmi::extract<CIMProperty> ext_property(value);

    if (ext_property.check())
        m_properties[key] = value;
    else
        m_properties[key] = CIMProperty::create(key, value);
}

bp::object CIMInstance::keys()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    return properties.keys();
}

bp::object CIMInstance::values()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    nocase_map_t::const_iterator it;

    bp::list values;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = lmi::extract_or_throw<CIMProperty&>(
            it->second, "property");
        values.append(property.getValue());
    }

    return values;
}

bp::object CIMInstance::items()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    nocase_map_t::const_iterator it;

    bp::list items;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = lmi::extract_or_throw<CIMProperty&>(
            it->second, "property");
        items.append(
            bp::make_tuple(
                std_string_as_pyunicode(it->first),
                property.getValue()));
    }

    return items;
}

bp::object CIMInstance::iterkeys()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    return properties.iterkeys();
}

bp::object CIMInstance::itervalues()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    return properties.itervalues();
}

bp::object CIMInstance::iteritems()
{
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    return properties.iteritems();
}

bp::object CIMInstance::copy()
{
    bp::object obj = CIMBase<CIMInstance>::create();
    CIMInstance &inst = lmi::extract<CIMInstance&>(obj);
    CIMInstanceName &path = lmi::extract<CIMInstanceName&>(getPath());
    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    NocaseDict &qualifiers = lmi::extract<NocaseDict&>(getQualifiers());

    inst.m_classname = m_classname;
    if (!isnone(m_path))
        inst.m_path = path.copy();
    inst.m_properties = properties.copy();
    inst.m_qualifiers = qualifiers.copy();
    if (!isnone(m_property_list))
        inst.m_property_list = bp::list(getPropertyList());

    return obj;
}

std::string CIMInstance::tomofContent(const bp::object &value)
{
    std::stringstream ss;

    if (isnone(value)) {
        ss << "NULL";
    } else if (PyList_Check(value.ptr())) {
        ss << '{';
        const int cnt = bp::len(value);
        for (int i = 0; i < cnt; ++i) {
            ss << tomofContent(value[i]);
            if (i < cnt - 1)
                ss << ", ";
        }
        ss << '}';
    } else if (isbasestring(value)) {
        ss << '\'' << object_as_std_string(value) << '\'';
    } else {
        ss << object_as_std_string(value);
    }

    return ss.str();
}

bp::object CIMInstance::tomof()
{
    std::stringstream ss;
    ss << "instance of " << m_classname << " {\n";

    NocaseDict &properties = lmi::extract<NocaseDict&>(getProperties());
    nocase_map_t::iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = lmi::extract<CIMProperty&>(it->second);
        ss << "\t" << property.getName() << " = "
           << tomofContent(property.getValue())
           << ";\n";
    }

    ss << "};\n";

    return std_string_as_pyunicode(ss.str());
}

bp::object CIMInstance::getClassname()
{
    return std_string_as_pyunicode(m_classname);
}

bp::object CIMInstance::getPath()
{
    if (!m_rc_inst_path.empty()) {
        m_path = CIMInstanceName::create(*m_rc_inst_path.get());
        m_rc_inst_path.release();
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
        for (it = qualifiers.begin(); it != qualifiers.end(); ++it)
            m_qualifiers[bp::object(it->getName())] = CIMQualifier::create(*it);
        m_rc_inst_qualifiers.release();
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
    m_rc_inst_path.release();
}

void CIMInstance::setProperties(const bp::object &properties)
{
    evalProperties();

    m_properties = lmi::get_or_throw<NocaseDict, bp::dict>(properties, "properties");

    // Unref cached resource, it will never be used
    m_rc_inst_properties.release();
}

void CIMInstance::setQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = lmi::get_or_throw<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_inst_qualifiers.release();
}

void CIMInstance::setPropertyList(const bp::object &property_list)
{
    evalProperties();

    m_property_list = lmi::get_or_throw<bp::list>(property_list);

    // Unref cached resource, it will never be used
    m_rc_inst_properties.release();
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
        m_properties[prop_name] = CIMProperty::create(*it);
        property_list.append(prop_name);
    }

    m_property_list = property_list;
    m_rc_inst_properties.release();
}
