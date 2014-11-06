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
#include "lmiwbem_convert.h"
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
    m_classname = StringConv::asStdString(classname, "classname");

    // We store properties in NocaseDict. Convert python's dict, if necessary.
    if (isnone(properties))
        m_properties = NocaseDict::create();
    else
        m_properties = NocaseDict::create(properties);

    // Convert plain values
    NocaseDict &prop_dict = NocaseDict::asNative(m_properties);
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

    m_path = Conv::get<CIMInstanceName, bp::object>(path, "path");
    m_property_list = Conv::get<bp::list, bp::object>(property_list, "property_list");
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
            ":rtype: unicode")
        .add_property("classname",
            &CIMInstance::getPyClassname,
            &CIMInstance::setPyClassname,
            "Property storing class name\n\n"
            ":rtype: unicode")
        .add_property("path",
            &CIMInstance::getPyPath,
            &CIMInstance::setPyPath,
            "Property storing object path\n\n"
            ":rtype: :py:class:`.CIMInstanceName`")
        .add_property("properties",
            &CIMInstance::getPyProperties,
            &CIMInstance::setPyProperties,
            "Property storing instance properties\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("qualifiers",
            &CIMInstance::getPyQualifiers,
            &CIMInstance::setPyQualifiers,
            "Property storing instance qualifiers\n\n"
            ":rtype: :py:class:`.NocaseDict`")
        .add_property("property_list",
            &CIMInstance::getPyPropertyList,
            &CIMInstance::setPyPropertyList,
            "Property storing instance properties\n\n"
            ":rtype: list of instance properties names\n"));
}

bp::object CIMInstance::create(const Pegasus::CIMInstance &instance)
{
    bp::object inst = CIMBase<CIMInstance>::create();
    CIMInstance &fake_this = CIMInstance::asNative(inst);
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

    if (!isnone(getPyPath())) {
        // Set CIMObjectPath
        const CIMInstanceName &path = CIMInstanceName::asNative(getPyPath());
        instance.setPath(path.asPegasusCIMObjectPath());
    }

    // Add all the properties
    const NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = CIMProperty::asNative(it->second);
        instance.addProperty(property.asPegasusCIMProperty());
    }

    // Add all the qualifiers
    const NocaseDict &qualifiers = NocaseDict::asNative(getPyQualifiers());
    for (it = qualifiers.begin(); it != qualifiers.end(); ++it) {
        CIMQualifier &qualifier = CIMQualifier::asNative(it->second);
        instance.addQualifier(qualifier.asPegasusCIMQualifier());
    }

    return instance;
}

#  if PY_MAJOR_VERSION < 3
int CIMInstance::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return 1;

    CIMInstance &other_inst = CIMInstance::asNative(other);

    int rval;
    if ((rval = m_classname.compare(other_inst.m_classname)) != 0 ||
        (rval = compare(getPyPath(), other_inst.getPyPath())) != 0 ||
        (rval = compare(getPyProperties(), other_inst.getPyProperties())) != 0 ||
        (rval = compare(getPyQualifiers(), other_inst.getPyQualifiers())) != 0)
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

    CIMInstance &other_inst = CIMInstance::asNative(other);

    return m_classname == other_inst.m_classname &&
        compare(getPyPath(), other_inst.getPyPath(), Py_EQ) &&
        compare(getPyProperties(), other_inst.getPyProperties(), Py_EQ) &&
        compare(getPyQualifiers(), other_inst.getPyQualifiers(), Py_EQ);
}

bool CIMInstance::gt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &other_inst = CIMInstance::asNative(other);

    return m_classname > other_inst.m_classname ||
        compare(getPyPath(), other_inst.getPyPath(), Py_GT) ||
        compare(getPyProperties(), other_inst.getPyProperties(), Py_GT) ||
        compare(getPyQualifiers(), other_inst.getPyQualifiers(), Py_GT);
}

bool CIMInstance::lt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &other_inst = CIMInstance::asNative(other);

    return m_classname < other_inst.m_classname ||
        compare(getPyPath(), other_inst.getPyPath(), Py_LT) ||
        compare(getPyProperties(), other_inst.getPyProperties(), Py_LT) ||
        compare(getPyQualifiers(), other_inst.getPyQualifiers(), Py_LT);
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

bp::object CIMInstance::repr()
{
    std::stringstream ss;
    ss << "CIMInstance(classname='" << m_classname << "', ...)";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMInstance::getitem(const bp::object &key)
{
    evalProperties();

    bp::object item = m_properties[key];
    if (isinstance(item, CIMProperty::type())) {
        CIMProperty &property = CIMProperty::asNative(item);
        return property.getPyValue();
    }

    return m_properties[key];
}

void CIMInstance::setitem(const bp::object &key, const bp::object &value)
{
    evalProperties();

    if (isinstance(value, CIMProperty::type()))
        m_properties[key] = value;
    else
        m_properties[key] = CIMProperty::create(key, value);
}

bp::object CIMInstance::len()
{
    return bp::object(bp::len(getPyProperties()));
}

bp::object CIMInstance::haskey(const bp::object &key)
{
    return getPyProperties().contains(key);
}

bp::object CIMInstance::keys()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    return properties.keys();
}

bp::object CIMInstance::values()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;

    bp::list values;
    for (it = properties.begin(); it != properties.end(); ++it) {
        if (isinstance(it->second, CIMProperty::type())) {
            CIMProperty &property = CIMProperty::asNative(
                it->second, "property");
            values.append(property.getPyValue());
        } else {
            values.append(it->second);
        }
    }

    return values;
}

bp::object CIMInstance::items()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;

    bp::list items;
    for (it = properties.begin(); it != properties.end(); ++it) {
        bp::object value;
        if (isinstance(it->second, CIMProperty::type())) {
            CIMProperty &property = CIMProperty::asNative(
                it->second, "property");
            value = property.getPyValue();
        } else {
            value = it->second;
        }

        items.append(
            bp::make_tuple(
                StringConv::asPyUnicode(it->first), value));
    }

    return items;
}

bp::object CIMInstance::iterkeys()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    return properties.iterkeys();
}

bp::object CIMInstance::itervalues()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    return properties.itervalues();
}

bp::object CIMInstance::iteritems()
{
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    return properties.iteritems();
}

bp::object CIMInstance::copy()
{
    bp::object obj = CIMBase<CIMInstance>::create();
    CIMInstance &inst = CIMInstance::asNative(obj);
    CIMInstanceName &path = CIMInstanceName::asNative(getPyPath());
    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    NocaseDict &qualifiers = NocaseDict::asNative(getPyQualifiers());

    inst.m_classname = m_classname;
    if (!isnone(m_path))
        inst.m_path = path.copy();
    inst.m_properties = properties.copy();
    inst.m_qualifiers = qualifiers.copy();
    if (!isnone(m_property_list))
        inst.m_property_list = bp::list(getPyPropertyList());

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
        ss << '\'' << ObjectConv::asStdString(value) << '\'';
    } else {
        ss << ObjectConv::asStdString(value);
    }

    return ss.str();
}

bp::object CIMInstance::tomof()
{
    std::stringstream ss;
    ss << "instance of " << m_classname << " {\n";

    NocaseDict &properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::iterator it;
    for (it = properties.begin(); it != properties.end(); ++it) {
        CIMProperty &property = CIMProperty::asNative(it->second);
        ss << "\t" << property.getName() << " = "
           << tomofContent(property.getPyValue())
           << ";\n";
    }

    ss << "};\n";

    return StringConv::asPyUnicode(ss.str());
}

CIMInstanceName CIMInstance::getPath()
{
    return CIMInstanceName::asNative(getPyPath());
}

const CIMInstanceName &CIMInstance::getPath() const
{
    CIMInstance *fake_this = const_cast<CIMInstance*>(this);
    return CIMInstanceName::asNative(fake_this->getPyPath());
}

std::string CIMInstance::getClassname() const
{
    return m_classname;
}

bp::object CIMInstance::getPyClassname() const
{
    return StringConv::asPyUnicode(m_classname);
}

bp::object CIMInstance::getPyPath()
{
    if (!m_rc_inst_path.empty()) {
        m_path = CIMInstanceName::create(*m_rc_inst_path.get());
        m_rc_inst_path.release();
    }

    return m_path;
}

bp::object CIMInstance::getPyProperties()
{
    evalProperties();
    return m_properties;
}

bp::object CIMInstance::getPyQualifiers()
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

bp::object CIMInstance::getPyPropertyList()
{
    evalProperties();
    return m_property_list;
}

void CIMInstance::setClassname(const std::string &classname)
{
    m_classname = classname;
}

void CIMInstance::setPyClassname(const bp::object &classname)
{
    m_classname = StringConv::asStdString(classname, "classname");
}

void CIMInstance::setPyPath(const bp::object &path)
{
    m_path = Conv::get<CIMInstanceName>(path, "path");

    // Unref cached resource, it will never be used
    m_rc_inst_path.release();
}

void CIMInstance::setPyProperties(const bp::object &properties)
{
    evalProperties();

    m_properties = Conv::get<NocaseDict, bp::dict>(properties, "properties");

    // Unref cached resource, it will never be used
    m_rc_inst_properties.release();
}

void CIMInstance::setPyQualifiers(const bp::object &qualifiers)
{
    m_qualifiers = Conv::get<NocaseDict, bp::dict>(qualifiers, "qualifiers");

    // Unref cached resource, it will never be used
    m_rc_inst_qualifiers.release();
}

void CIMInstance::setPyPropertyList(const bp::object &property_list)
{
    evalProperties();

    m_property_list = Conv::get<bp::list>(property_list);

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
        if (it->getValue().getType() == Pegasus::CIMTYPE_REFERENCE) {
            // We got a property with CIMObjectPath value. Let's set its
            // hostname which could be left out by Pegasus.
            // FIXME: refactor using getHostname()
            const CIMInstanceName &this_iname = getPath();
            Pegasus::CIMProperty property = it->clone();
            Pegasus::CIMValue value = property.getValue();
            Pegasus::CIMObjectPath iname;
            value.get(iname);
            iname.setHost(this_iname.getHostname().c_str());
            value.set(iname);
            property.setValue(value);

            m_properties[prop_name] = CIMProperty::create(property);
        } else {
            m_properties[prop_name] = CIMProperty::create(*it);
        }
        property_list.append(prop_name);
    }

    m_property_list = property_list;
    m_rc_inst_properties.release();
}

void CIMInstance::updatePegasusCIMInstanceNamespace(
    Pegasus::CIMInstance &instance,
    const std::string &ns)
{
    Pegasus::CIMObjectPath path = instance.getPath();
    if (!path.getNameSpace().isNull()) {
        // The namespace is already set. We have nothing to do.
        return;
    }

    CIMInstanceName::updatePegasusCIMObjectPathNamespace(path, ns);
    instance.setPath(path);
}
