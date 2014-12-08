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
#include <boost/algorithm/string/replace.hpp>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include <Pegasus/Common/CIMValue.h>
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

CIMInstanceName::CIMInstanceName()
    : m_classname()
    , m_namespace()
    , m_hostname()
    , m_keybindings()
{
}

CIMInstanceName::CIMInstanceName(
    const bp::object &cls,
    const bp::object &keybindings,
    const bp::object &host,
    const bp::object &ns)
    : m_classname()
    , m_namespace()
    , m_hostname()
    , m_keybindings()
{
    m_classname = StringConv::asStdString(cls, "classname");
    m_namespace = StringConv::asStdString(ns, "namespace");
    if (!isnone(host)) {
        // Host may be missing, other members (classname, namespace not)
        m_hostname  = StringConv::asStdString(host, "host");
    }
    if (isnone(keybindings))
        m_keybindings = NocaseDict::create();
    else
        m_keybindings = NocaseDict::create(keybindings);
}

void CIMInstanceName::init_type()
{
    CIMBase<CIMInstanceName>::init_type(
        bp::class_<CIMInstanceName>("CIMInstanceName", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("classname"),
                bp::arg("keybindings") = NocaseDict::create(),
                bp::arg("host") = std::string(),
                bp::arg("namespace") = std::string()),
                "Constructs :py:class:`CIMInstance`.\n\n"
                ":param str classname: Class name of the object path\n"
                ":param NocaseDict keybindings: Dictionary containing keybindings\n"
                "\t of the object path\n"
                ":param str host: String containing hostname of the object path\n"
                ":param str namespace: String containing namespace of the object path"))
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &CIMInstanceName::cmp)
#  else
        .def("__eq__", &CIMInstanceName::eq)
        .def("__gt__", &CIMInstanceName::gt)
        .def("__lt__", &CIMInstanceName::lt)
        .def("__ge__", &CIMInstanceName::ge)
        .def("__le__", &CIMInstanceName::le)
#  endif // PY_MAJOR_VERSION
        .def("__str__", &CIMInstanceName::str,
            ":returns: serialized object\n"
            ":rtype: unicode")
        .def("__repr__", &CIMInstanceName::repr,
            ":returns: pretty string of the object")
        .def("__getitem__", &CIMInstanceName::getitem)
        .def("__delitem__", &CIMInstanceName::delitem)
        .def("__setitem__", &CIMInstanceName::setitem)
        .def("__contains__", &CIMInstanceName::haskey)
        .def("__len__", &CIMInstanceName::len)
        .def("has_key", &CIMInstanceName::haskey,
            "has_key(key)\n\n"
            ":param str key: key to check for presence in object's keybindings\n"
            ":returns: True, if the key is present in object's keybindings;\n"
            "\tFalse otherwise\n"
            ":rtype: bool")
        .def("keys", &CIMInstanceName::keys,
            "keys()\n\n"
            ":returns: list of strings of keybinding names\n"
            ":rtype: list")
        .def("values", &CIMInstanceName::values,
            "values()\n\n"
            ":returns: list of keybinding values\n"
            ":rtype: list")
        .def("items", &CIMInstanceName::items,
            "items()\n\n"
            ":returns: list of tuples with keybinding name and value\n"
            ":rtype: list")
        .def("iterkeys", &CIMInstanceName::iterkeys,
            "iterkeys()\n\n"
            ":returns: iterator for keybinding names iteration\n"
            ":rtype: iterator")
        .def("itervalues", &CIMInstanceName::itervalues,
            "itervalues()\n\n"
            ":returns: iterator for keybinding values iteration\n"
            ":rtype: iterator")
        .def("iteritems", &CIMInstanceName::iteritems,
            "iteritems()\n\n"
            ":returns: iterator for keybinding name and value iteration\n"
            ":rtype: iterator")
        .def("copy", &CIMInstanceName::copy,
            "copy()\n\n"
            ":returns: copy of the object itself\n"
            ":rtype: :py:class:`.CIMInstanceName`")
        .add_property("classname",
            &CIMInstanceName::getPyClassname,
            &CIMInstanceName::setPyClassname,
            "Property storing class name.\n\n"
            ":rtype: unicode")
        .add_property("namespace",
            &CIMInstanceName::getPyNamespace,
            &CIMInstanceName::setPyNamespace,
            "Property storing namespace.\n\n"
            ":rtype: unicode")
        .add_property("host",
            &CIMInstanceName::getPyHostname,
            &CIMInstanceName::setPyHostname,
            "Property storing hostname.\n\n"
            ":rtype: unicode")
        .add_property("keybindings",
            &CIMInstanceName::getPyKeybindings,
            &CIMInstanceName::setPyKeybindings,
            "Property storing keybindings.\n\n"
            ":rtype: :py:class:`.NocaseDict`"));
}

bp::object CIMInstanceName::create(const Pegasus::String &obj_path)
{
    return create(Pegasus::CIMObjectPath(obj_path));
}

bp::object CIMInstanceName::create(
    const Pegasus::CIMObjectPath &obj_path,
    const std::string &ns,
    const std::string &hostname)
{
    if (isUninitialized(obj_path)) {
        // If we got uninitialized CIMObjectPath, return None instead.
        return None;
    }

    bp::object inst = CIMBase<CIMInstanceName>::create();
    CIMInstanceName& fake_this = CIMInstanceName::asNative(inst);

    fake_this.m_classname = obj_path.getClassName().getString().getCString();
    fake_this.m_namespace = obj_path.getNameSpace().isNull() ? ns :
        std::string(obj_path.getNameSpace().getString().getCString());
    fake_this.m_hostname = obj_path.getHost() == Pegasus::String::EMPTY
        ? hostname : std::string(obj_path.getHost().getCString());
    fake_this.m_keybindings = NocaseDict::create();

    const Pegasus::Array<Pegasus::CIMKeyBinding> &keybindings = obj_path.getKeyBindings();
    const Pegasus::Uint32 cnt = keybindings.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        Pegasus::CIMKeyBinding keybinding = keybindings[i];

        if (keybinding.getType() == Pegasus::CIMKeyBinding::REFERENCE) {
            // We got a keybinding with CIMObjectPath value. Let's set its
            // hostname which could be left out by Pegasus.
            Pegasus::CIMObjectPath path(keybinding.getValue());
            if (path.getHost() == Pegasus::String::EMPTY) {
                path.setHost(fake_this.m_hostname.c_str());
                keybinding.setValue(path.toString());
            }
        }

        bp::object value = keybindingToValue(keybinding);

        fake_this.m_keybindings[bp::object(keybinding.getName())] = value;
    }

    return inst;
}

Pegasus::CIMObjectPath CIMInstanceName::asPegasusCIMObjectPath() const
{
    Pegasus::Array<Pegasus::CIMKeyBinding> arr_keybindings;

    if (!isnone(m_keybindings)) {
        NocaseDict &keybindings = NocaseDict::asNative(
            m_keybindings, "self.keybindings");

        // Create an array of keybindings. Allowed keybindings' types:
        // bool, numeric, str or CIMInstanceName
        nocase_map_t::const_iterator it;
        for (it = keybindings.begin(); it != keybindings.end(); ++it) {
            if (isbool(it->second)) {
                // Create bool CIMKeyBinding
                bool bval = Conv::as<bool>(it->second);
                Pegasus::CIMValue value = Pegasus::CIMValue(bval);
                arr_keybindings.append(
                    Pegasus::CIMKeyBinding(
                        Pegasus::CIMName(it->first.c_str()),
                        value));
                continue;
            }

            if (islong(it->second) ||
#  if PY_MAJOR_VERSION < 3
                isint(it->second) ||
#  endif // PY_MAJOR_VERSION
                isfloat(it->second))
            {
                // Create numeric CIMKeyBinding. All the lmiwbem.lmiwbem_types.{Uint8, Sint8, ...}
                // are derived from long or float, so we get here.
                arr_keybindings.append(
                    Pegasus::CIMKeyBinding(
                        Pegasus::CIMName(it->first.c_str()),
                        Pegasus::String(ObjectConv::asStdString(it->second).c_str()),
                        Pegasus::CIMKeyBinding::NUMERIC));
                continue;
            }

            if (isbasestring(it->second)) {
                // Create str CIMKeyBinding
                std::string std_string(
                    StringConv::asStdString(it->second));
                Pegasus::CIMValue value = Pegasus::CIMValue(Pegasus::String(std_string.c_str()));
                arr_keybindings.append(
                    Pegasus::CIMKeyBinding(
                        Pegasus::CIMName(it->first.c_str()),
                        value));
                continue;
            }

            if (isinstance(it->second, CIMInstanceName::type())) {
                // Create CIMInstanceName CIMKeyBinding
                CIMInstanceName &instance_name = CIMInstanceName::asNative(it->second);
                arr_keybindings.append(
                    Pegasus::CIMKeyBinding(
                        Pegasus::CIMName(it->first.c_str()),
                        instance_name.asPegasusCIMObjectPath()));
                continue;
            }

            throw_TypeError("Invalid keybinding type");
        }
    }

    return Pegasus::CIMObjectPath(
        Pegasus::String(m_hostname.c_str()),
        Pegasus::CIMNamespaceName(m_namespace.c_str()),
        Pegasus::CIMName(m_classname.c_str()),
        arr_keybindings);
}

#  if PY_MAJOR_VERSION < 3
int CIMInstanceName::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMInstanceName::type()))
        return 1;

    CIMInstanceName &other_inst_name = CIMInstanceName::asNative(other);

    int rval;
    if ((rval = m_classname.compare(other_inst_name.m_classname)) != 0 ||
        (rval = m_namespace.compare(other_inst_name.m_namespace)) != 0 ||
        (rval = m_hostname.compare(other_inst_name.m_hostname)) != 0 ||
        (rval = compare(m_keybindings, other_inst_name.m_keybindings)) != 0)
    {
        return rval;
    }

    return 0;
}
#  else
bool CIMInstanceName::eq(const bp::object &other)
{
    if (!isinstance(other, CIMInstanceName::type()))
        return false;

    CIMInstanceName &other_inst_name = CIMInstanceName::asNative(other);

    return m_classname == other_inst_name.m_classname &&
        m_namespace == other_inst_name.m_namespace &&
        m_hostname  == other_inst_name.m_hostname  &&
        compare(m_keybindings, other_inst_name.m_keybindings, Py_EQ);
}

bool CIMInstanceName::gt(const bp::object &other)
{
    if (!isinstance(other, CIMInstanceName::type()))
        return false;

    CIMInstanceName &other_inst_name = CIMInstanceName::asNative(other);

    return m_classname > other_inst_name.m_classname ||
        m_namespace > other_inst_name.m_namespace ||
        m_hostname  > other_inst_name.m_hostname  ||
        compare(m_keybindings, other_inst_name.m_keybindings, Py_GT);
}

bool CIMInstanceName::lt(const bp::object &other)
{
    if (!isinstance(other, CIMInstanceName::type()))
        return false;

    CIMInstanceName &other_inst_name = CIMInstanceName::asNative(other);

    return m_classname < other_inst_name.m_classname ||
        m_namespace < other_inst_name.m_namespace ||
        m_hostname  < other_inst_name.m_hostname  ||
        compare(m_keybindings, other_inst_name.m_keybindings, Py_LT);
}

bool CIMInstanceName::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool CIMInstanceName::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

bp::object CIMInstanceName::copy()
{
    bp::object obj = CIMBase<CIMInstanceName>::create();
    CIMInstanceName &inst_name = CIMInstanceName::asNative(obj);
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    inst_name.m_classname = m_classname;
    inst_name.m_namespace = m_namespace;
    inst_name.m_hostname = m_hostname;
    inst_name.m_keybindings = keybindings.copy();
    return obj;
}

std::string CIMInstanceName::asStdString() const
{
    std::stringstream ss;

    if (!m_hostname.empty())
        ss << "//" << m_hostname << '/';
    if (!m_namespace.empty())
        ss << m_namespace << ':';
    ss << m_classname << '.';

    const NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    nocase_map_t::const_iterator it;
    for (it = keybindings.begin(); it != keybindings.end(); ++it) {
        ss << it->first << '=';
        if (isinstance(it->second, CIMInstanceName::type())) {
            std::string iname_str = ObjectConv::asStdString(it->second);
            boost::replace_all(iname_str, "\\", "\\\\");
            boost::replace_all(iname_str, "\"", "\\\"");
            ss << '"' << iname_str << '"';
        } else if (isbasestring(it->second)) {
            ss << '"' << ObjectConv::asStdString(it->second) << '"';
        } else {
            ss << ObjectConv::asStdString(it->second);
        }

        nocase_map_t::const_iterator tmp_it = it;
        if (tmp_it != keybindings.end() && ++tmp_it != keybindings.end())
            ss << ',';
    }

    return ss.str();
}

bp::object CIMInstanceName::str() const
{

    return StringConv::asPyUnicode(asStdString());
}

bp::object CIMInstanceName::repr() const
{
    std::stringstream ss;
    ss << "CIMInstanceName(classname='" << m_classname << "', keybindings="
       << ObjectConv::asStdString(m_keybindings);
    if (!m_hostname.empty())
        ss << ", host='" << m_hostname << '\'';
    ss <<  ", namespace='"
       << m_namespace << "')";
    return StringConv::asPyUnicode(ss.str());
}

bp::object CIMInstanceName::getitem(const bp::object &key)
{
    return m_keybindings[key];
}

void CIMInstanceName::delitem(const bp::object &key)
{
    bp::delitem(m_keybindings, key);
}

void CIMInstanceName::setitem(const bp::object &key, const bp::object &value)
{
    m_keybindings[key] = value;
}

bp::object CIMInstanceName::len() const
{
    return bp::object(bp::len(m_keybindings));
}

bp::object CIMInstanceName::haskey(const bp::object &key) const
{
    return m_keybindings.contains(key);
}

bp::object CIMInstanceName::keys()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.keys();
}

bp::object CIMInstanceName::values()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.values();
}

bp::object CIMInstanceName::items()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.items();
}

bp::object CIMInstanceName::iterkeys()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.iterkeys();
}

bp::object CIMInstanceName::itervalues()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.itervalues();
}

bp::object CIMInstanceName::iteritems()
{
    NocaseDict &keybindings = NocaseDict::asNative(m_keybindings);
    return keybindings.iteritems();
}

std::string CIMInstanceName::getClassname() const
{
    return m_classname;
}

std::string CIMInstanceName::getNamespace() const
{
    return m_namespace;
}

std::string CIMInstanceName::getHostname()  const
{
    return m_hostname;
}

bp::object CIMInstanceName::getPyClassname() const
{
    return StringConv::asPyUnicode(m_classname);
}

bp::object CIMInstanceName::getPyNamespace() const
{
    return StringConv::asPyUnicode(m_namespace);
}

bp::object CIMInstanceName::getPyHostname() const
{
    return StringConv::asPyUnicode(m_hostname);
}

bp::object CIMInstanceName::getPyKeybindings() const
{
    return m_keybindings;
}

void CIMInstanceName::setClassname(const std::string &classname)
{
    m_classname = classname;
}

void CIMInstanceName::setNamespace(const std::string &namespace_)
{
    m_namespace = namespace_;
}

void CIMInstanceName::setHostname(const std::string &hostname)
{
    m_hostname = hostname;
}

void CIMInstanceName::setPyClassname(const bp::object &classname)
{
    m_classname = StringConv::asStdString(classname);
}

void CIMInstanceName::setPyNamespace(const bp::object &namespace_)
{
    m_namespace = StringConv::asStdString(namespace_);
}

void CIMInstanceName::setPyHostname(const bp::object &hostname)
{
    m_hostname = StringConv::asStdString(hostname);
}

void CIMInstanceName::setPyKeybindings(const bp::object &keybindings)
{
    m_keybindings = Conv::get<NocaseDict, bp::dict>(keybindings);
}

bp::object CIMInstanceName::keybindingToValue(const Pegasus::CIMKeyBinding &keybinding)
{
    bp::object value;

    const Pegasus::String cim_value = keybinding.getValue();
    switch (keybinding.getType()) {
    case Pegasus::CIMKeyBinding::BOOLEAN:
        return StringConv::asPyBool(cim_value);
    case Pegasus::CIMKeyBinding::STRING:
        return StringConv::asPyUnicode(cim_value);
    case Pegasus::CIMKeyBinding::NUMERIC: {
        bp::object num;
#  if PY_MAJOR_VERSION < 3
        if (!isnone(num = StringConv::asPyInt(cim_value))  ||
            !isnone(num = StringConv::asPyLong(cim_value)) ||
#  else
        if (!isnone(num = StringConv::asPyLong(cim_value)) ||
#  endif // PY_MAJOR_VERSION
            !isnone(num = StringConv::asPyFloat(cim_value)))
        {
            return num;
        }

        throw_TypeError("Wrong keybinding numeric type");
        return None;
    }
    case Pegasus::CIMKeyBinding::REFERENCE:
        return CIMInstanceName::create(cim_value);
    }

    return value;
}

void CIMInstanceName::updatePegasusCIMObjectPathNamespace(
    Pegasus::CIMObjectPath &path,
    const std::string &ns)
{
    if (!path.getNameSpace().isNull()) {
        // The namespace is already set. We have nothing to do.
        return;
    }

    path.setNameSpace(Pegasus::CIMNamespaceName(ns.c_str()));
}

void CIMInstanceName::updatePegasusCIMObjectPathHostname(
    Pegasus::CIMObjectPath &path,
    const std::string &hostname)
{
    if (path.getHost() != Pegasus::String::EMPTY) {
        // The hostname is already set. We have nothing to do.
        return;
    }

    path.setHost(Pegasus::String(hostname.c_str()));
}

bool CIMInstanceName::isUninitialized(const Pegasus::CIMObjectPath &path)
{
    // Let's consider object path to be Null, when its class name is
    // undefined or Null.
    return path.getClassName().isNull();
}
