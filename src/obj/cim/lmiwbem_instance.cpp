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
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_property.h"
#include "obj/cim/lmiwbem_qualifier.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

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
    m_classname = StringConv::asString(classname, "classname");

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

    if (!isnone(path))
        m_path = Conv::get<CIMInstanceName>(path, "path");

    if (!isnone(property_list))
        m_property_list = Conv::get<bp::list>(property_list, "property_list");
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
                bp::arg("path") = None,
                bp::arg("property_list") = None),
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
    if (isUninitialized(instance)) {
        // If we got uninitialized CIMInstance, return None instead.
        return None;
    }

    bp::object py_inst = CIMBase<CIMInstance>::create();
    CIMInstance &fake_this = CIMInstance::asNative(py_inst);
    fake_this.m_classname = instance.getClassName().getString();

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

    return py_inst;
}

bp::object CIMInstance::create(const Pegasus::CIMObject &object)
{
    return create(Pegasus::CIMInstance(object));
}

Pegasus::CIMInstance CIMInstance::asPegasusCIMInstance()
{
    // Doubled parenthesis are used due to the C++ ambiguity known also as
    // The most vexing parse.
    Pegasus::CIMInstance peg_instance((Pegasus::CIMName(m_classname)));

    if (!isnone(getPyPath())) {
        // Set CIMObjectPath
        const CIMInstanceName &path = CIMInstanceName::asNative(getPyPath());
        peg_instance.setPath(path.asPegasusCIMObjectPath());
    }

    // Add all the properties
    const NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;
    for (it = cim_properties.begin(); it != cim_properties.end(); ++it) {
        CIMProperty &cim_property = CIMProperty::asNative(it->second);
        peg_instance.addProperty(cim_property.asPegasusCIMProperty());
    }

    // Add all the qualifiers
    const NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());
    for (it = cim_qualifiers.begin(); it != cim_qualifiers.end(); ++it) {
        CIMQualifier &cim_qualifier = CIMQualifier::asNative(it->second);
        peg_instance.addQualifier(cim_qualifier.asPegasusCIMQualifier());
    }

    return peg_instance;
}

#  if PY_MAJOR_VERSION < 3
int CIMInstance::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return 1;

    CIMInstance &cim_other = CIMInstance::asNative(other);

    int rval;
    if ((rval = m_classname.compare(cim_other.m_classname)) != 0 ||
        (rval = compare(getPyPath(), cim_other.getPyPath())) != 0 ||
        (rval = compare(getPyProperties(), cim_other.getPyProperties())) != 0 ||
        (rval = compare(getPyQualifiers(), cim_other.getPyQualifiers())) != 0)
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

    CIMInstance &cim_other = CIMInstance::asNative(other);

    return m_classname == cim_other.m_classname &&
        compare(getPyPath(), cim_other.getPyPath(), Py_EQ) &&
        compare(getPyProperties(), cim_other.getPyProperties(), Py_EQ) &&
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_EQ);
}

bool CIMInstance::gt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &cim_other = CIMInstance::asNative(other);

    return m_classname > cim_other.m_classname ||
        compare(getPyPath(), cim_other.getPyPath(), Py_GT) ||
        compare(getPyProperties(), cim_other.getPyProperties(), Py_GT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_GT);
}

bool CIMInstance::lt(const bp::object &other)
{
    if (!isinstance(other, CIMInstance::type()))
        return false;

    CIMInstance &cim_other = CIMInstance::asNative(other);

    return m_classname < cim_other.m_classname ||
        compare(getPyPath(), cim_other.getPyPath(), Py_LT) ||
        compare(getPyProperties(), cim_other.getPyProperties(), Py_LT) ||
        compare(getPyQualifiers(), cim_other.getPyQualifiers(), Py_LT);
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
    ss << "CIMInstance(classname=u'" << m_classname << "', ...)";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMInstance::getitem(const bp::object &key)
{
    evalProperties();

    bp::object py_item = m_properties[key];
    if (isinstance(py_item, CIMProperty::type())) {
        CIMProperty &property = CIMProperty::asNative(py_item);
        return property.getPyValue();
    }

    return m_properties[key];
}

void CIMInstance::setitem(const bp::object &key, const bp::object &value)
{
    evalProperties();

    if (isinstance(value, CIMProperty::type())) {
        m_properties[key] = value;
    } else if (m_properties.contains(key) &&
        isinstance(m_properties[key], CIMProperty::type()))
    {
        CIMProperty &cim_prop = CIMProperty::asNative(m_properties[key]);
        cim_prop.setPyValue(value);

        String type(CIMTypeConv::asString(value));
        if (!type.empty())
            cim_prop.setType(type);

        bool is_array = isarray(value);
        cim_prop.setIsArray(is_array);
        if (is_array)
            cim_prop.setArraySize(bp::len(value));
    } else {
        m_properties[key] = CIMProperty::create(key, value);
    }
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
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    return cim_properties.keys();
}

bp::object CIMInstance::values()
{
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;

    bp::list py_values;
    for (it = cim_properties.begin(); it != cim_properties.end(); ++it) {
        if (isinstance(it->second, CIMProperty::type())) {
            CIMProperty &property = CIMProperty::asNative(
                it->second, "property");
            py_values.append(property.getPyValue());
        } else {
            py_values.append(it->second);
        }
    }

    return py_values;
}

bp::object CIMInstance::items()
{
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::const_iterator it;

    bp::list py_items;
    for (it = cim_properties.begin(); it != cim_properties.end(); ++it) {
        bp::object value;
        if (isinstance(it->second, CIMProperty::type())) {
            CIMProperty &property = CIMProperty::asNative(
                it->second, "property");
            value = property.getPyValue();
        } else {
            value = it->second;
        }

        py_items.append(
            bp::make_tuple(
                StringConv::asPyUnicode(it->first), value));
    }

    return py_items;
}

bp::object CIMInstance::iterkeys()
{
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    return cim_properties.iterkeys();
}

bp::object CIMInstance::itervalues()
{
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    return cim_properties.itervalues();
}

bp::object CIMInstance::iteritems()
{
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    return cim_properties.iteritems();
}

bp::object CIMInstance::copy()
{
    bp::object py_inst = CIMBase<CIMInstance>::create();
    CIMInstance &cim_inst = CIMInstance::asNative(py_inst);
    CIMInstanceName &cim_path = CIMInstanceName::asNative(getPyPath());
    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    NocaseDict &cim_qualifiers = NocaseDict::asNative(getPyQualifiers());

    cim_inst.m_classname = m_classname;
    if (!isnone(m_path))
        cim_inst.m_path = cim_path.copy();
    cim_inst.m_properties = cim_properties.copy();
    cim_inst.m_qualifiers = cim_qualifiers.copy();
    if (!isnone(m_property_list))
        cim_inst.m_property_list = bp::list(getPyPropertyList());

    return py_inst;
}

String CIMInstance::tomofContent(const bp::object &value)
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
        ss << '\'' << ObjectConv::asString(value) << '\'';
    } else {
        ss << ObjectConv::asString(value);
    }

    return ss.str();
}

bp::object CIMInstance::tomof()
{
    std::stringstream ss;
    ss << "instance of " << m_classname << " {\n";

    NocaseDict &cim_properties = NocaseDict::asNative(getPyProperties());
    nocase_map_t::iterator it;
    for (it = cim_properties.begin(); it != cim_properties.end(); ++it) {
        CIMProperty &cim_property = CIMProperty::asNative(it->second);
        ss << "\t" << cim_property.getName() << " = "
           << tomofContent(cim_property.getPyValue())
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

String CIMInstance::getClassname() const
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
        std::list<Pegasus::CIMConstQualifier> &cim_qualifiers = *m_rc_inst_qualifiers.get();
        std::list<Pegasus::CIMConstQualifier>::const_iterator it;
        for (it = cim_qualifiers.begin(); it != cim_qualifiers.end(); ++it)
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

void CIMInstance::setClassname(const String &classname)
{
    m_classname = classname;
}

void CIMInstance::setPyClassname(const bp::object &classname)
{
    m_classname = StringConv::asString(classname, "classname");
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
    bp::list py_property_list;
    std::list<Pegasus::CIMConstProperty>::const_iterator it;
    std::list<Pegasus::CIMConstProperty> &properties = *m_rc_inst_properties.get();
    for (it = properties.begin(); it != properties.end(); ++it) {
        bp::object py_prop_name(it->getName());
        if (it->getValue().getType() == Pegasus::CIMTYPE_REFERENCE) {
            // We got a property with CIMObjectPath value. Let's set its
            // hostname which could be left out by Pegasus.
            // FIXME: refactor using getHostname()
            const CIMInstanceName &this_iname = getPath();
            Pegasus::CIMProperty peg_property = it->clone();
            Pegasus::CIMValue peg_value = peg_property.getValue();
            Pegasus::CIMObjectPath peg_iname;
            peg_value.get(peg_iname);
            peg_iname.setHost(this_iname.getHostname());
            peg_value.set(peg_iname);
            peg_property.setValue(peg_value);

            m_properties[py_prop_name] = CIMProperty::create(peg_property);
        } else {
            m_properties[py_prop_name] = CIMProperty::create(*it);
        }
        py_property_list.append(py_prop_name);
    }

    m_property_list = py_property_list;
    m_rc_inst_properties.release();
}

void CIMInstance::updatePegasusCIMInstanceNamespace(
    Pegasus::CIMInstance &instance,
    const String &ns)
{
    Pegasus::CIMObjectPath peg_path(instance.getPath());
    if (!peg_path.getNameSpace().isNull()) {
        // The namespace is already set. We have nothing to do.
        return;
    }

    CIMInstanceName::updatePegasusCIMObjectPathNamespace(peg_path, ns);
    instance.setPath(peg_path);
}

void CIMInstance::updatePegasusCIMInstanceHostname(
    Pegasus::CIMInstance &instance,
    const String &hostname)
{
    Pegasus::CIMObjectPath peg_path(instance.getPath());
    if (peg_path.getHost() != Pegasus::String::EMPTY) {
        // The hostname is already set. We have nothing to do.
        return;
    }

    CIMInstanceName::updatePegasusCIMObjectPathHostname(peg_path, hostname);
    instance.setPath(peg_path);
}

bool CIMInstance::isUninitialized(const Pegasus::CIMInstance &instance)
{
    return instance.isUninitialized();
}
