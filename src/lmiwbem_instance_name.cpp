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
#include <boost/algorithm/string/replace.hpp>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include "lmiwbem_instance_name.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_util.h"

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
{
    m_classname = lmi::extract_or_throw<std::string>(cls, "classname");
    m_namespace = lmi::extract_or_throw<std::string>(ns, "namespace");
    m_hostname  = lmi::extract_or_throw<std::string>(host, "host");
    if (keybindings.is_none())
        m_keybindings = NocaseDict::create();
    else if (lmi::extract<bp::dict>(keybindings).check())
        m_keybindings = NocaseDict::create(keybindings);
    else
        m_keybindings = lmi::get_or_throw<NocaseDict>(keybindings, "keybindings");
}

void CIMInstanceName::init_type()
{
    CIMBase::init_type(bp::class_<CIMInstanceName>("CIMInstanceName", bp::init<>())
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
        .def("__cmp__", &CIMInstanceName::cmp)
        .def("__str__", &CIMInstanceName::str,
            ":returns: serialized object\n"
            ":rtype: str")
        .def("__repr__", &CIMInstanceName::repr,
            ":returns: pretty string of the object")
        .def("__getitem__", &CIMInstanceName::getitem)
        .def("__delitem__", &CIMInstanceName::delitem)
        .def("__setitem__", &CIMInstanceName::setitem)
        .def("__contains__", &CIMInstanceName::haskey)
        .def("__len__", &CIMInstanceName::len)
        .def("has_key", &CIMInstanceName::haskey)
        .def("keys", &CIMInstanceName::keys)
        .def("values", &CIMInstanceName::values)
        .def("items", &CIMInstanceName::items)
        .def("iterkeys", &CIMInstanceName::iterkeys)
        .def("itervalues", &CIMInstanceName::itervalues)
        .def("iteritems", &CIMInstanceName::iteritems)
        .def("copy", &CIMInstanceName::copy)
        .add_property("classname",
            &CIMInstanceName::getClassname,
            &CIMInstanceName::setClassname,
            "Property storing class name.\n\n"
            ":returns: string containing the class name")
        .add_property("namespace",
            &CIMInstanceName::getNamespace,
            &CIMInstanceName::setNamespace,
            "Property storing namespace.\n\n"
            ":returns: string containing the namespace name")
        .add_property("host",
            &CIMInstanceName::getHostname,
            &CIMInstanceName::setHostname,
            "Property storing hostname.\n\n"
            ":returns: string containing the hostname")
        .add_property("keybindings",
            &CIMInstanceName::getKeybindings,
            &CIMInstanceName::setKeybindings,
            "Property storing keybindings.\n\n"
            ":returns: dictionary containing keybindings\n"
            ":rtype: :py:class:`NocaseDict`"));
}

bp::object CIMInstanceName::create(
    const Pegasus::CIMObjectPath &obj_path,
    const std::string &ns,
    const std::string &hostname)
{
    bp::object inst = CIMBase::create();
    CIMInstanceName& fake_this = lmi::extract<CIMInstanceName&>(inst);

    fake_this.m_classname = obj_path.getClassName().getString().getCString();
    fake_this.m_namespace = obj_path.getNameSpace().isNull() ? ns :
        std::string(obj_path.getNameSpace().getString().getCString());
    fake_this.m_hostname = obj_path.getHost() == Pegasus::String::EMPTY
        ? hostname : std::string(obj_path.getHost().getCString());
    fake_this.m_keybindings = NocaseDict::create();

    const Pegasus::Array<Pegasus::CIMKeyBinding> &keybindings = obj_path.getKeyBindings();
    const Pegasus::Uint32 cnt = keybindings.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        const Pegasus::CIMKeyBinding &keybinding = keybindings[i];
        bp::object name(
            std_string_as_pyunicode(
                std::string(keybinding.getName().getString().getCString()))
        );
        bp::object value(
            std_string_as_pyunicode(std::string(keybinding.getValue().getCString()))
        );

        fake_this.m_keybindings[name] = value;
    }

    return inst;
}

Pegasus::CIMObjectPath CIMInstanceName::asPegasusCIMObjectPath() const
{
    Pegasus::Array<Pegasus::CIMKeyBinding> arr_keybindings;
    NocaseDict &keybindings = lmi::extract_or_throw<NocaseDict&>(
        m_keybindings, "self.keybindings");

    // Create an array of keybindings. Allowed keybindings' types:
    // bool, numeric, str or CIMInstanceName
    nocase_map_t::const_iterator it;
    for (it = keybindings.begin(); it != keybindings.end(); ++it) {
        if (isbool(it->second)) {
            // Create bool CIMKeyBinding
            bool bval = lmi::extract<bool>(it->second);
            Pegasus::CIMValue value = Pegasus::CIMValue(bval);
            arr_keybindings.append(
                Pegasus::CIMKeyBinding(
                    Pegasus::CIMName(it->first.c_str()),
                    value));
            continue;
        }

        if (islong(it->second) || isfloat(it->second) || isint(it->second)) {
            // Create numeric CIMKeyBinding. All the lmiwbem.lmiwbem_types.{Uint8, Sint8, ...}
            // are derived from long or float, so we get here.
            arr_keybindings.append(
                Pegasus::CIMKeyBinding(
                    Pegasus::CIMName(it->first.c_str()),
                    Pegasus::String(object_as_std_string(it->second).c_str()),
                    Pegasus::CIMKeyBinding::NUMERIC));
            continue;
        }

        lmi::extract<std::string> ext_string(it->second);
        if (ext_string.check()) {
            // Create str CIMKeyBinding
            std::string std_string(ext_string);
            Pegasus::CIMValue value = Pegasus::CIMValue(Pegasus::String(std_string.c_str()));
            arr_keybindings.append(
                Pegasus::CIMKeyBinding(
                    Pegasus::CIMName(it->first.c_str()),
                    value));
            continue;
        }

        lmi::extract<CIMInstanceName&> ext_instance_name(it->second);
        if (ext_instance_name.check()) {
            // Create CIMInstanceName CIMKeyBinding
            CIMInstanceName &instance_name = ext_instance_name;
            arr_keybindings.append(
                Pegasus::CIMKeyBinding(
                    Pegasus::CIMName(it->first.c_str()),
                    instance_name.asPegasusCIMObjectPath()));
            continue;
        }

        throw_TypeError("Invalid keybinding type");
    }

    return Pegasus::CIMObjectPath(
        Pegasus::String(m_hostname.c_str()),
        Pegasus::CIMNamespaceName(m_namespace.c_str()),
        Pegasus::CIMName(m_classname.c_str()),
        arr_keybindings);
}

int CIMInstanceName::cmp(const bp::object &other)
{
    if (!isinstance(other, CIMInstanceName::type()))
        return 1;

    CIMInstanceName &other_inst_name = lmi::extract<CIMInstanceName&>(other);

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

bp::object CIMInstanceName::copy()
{
    bp::object obj = CIMBase::create();
    CIMInstanceName &inst_name = lmi::extract<CIMInstanceName&>(obj);
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    inst_name.m_classname = m_classname;
    inst_name.m_namespace = m_namespace;
    inst_name.m_hostname = m_hostname;
    inst_name.m_keybindings = keybindings.copy();
    return obj;
}

std::string CIMInstanceName::str()
{
    std::stringstream ss;

    if (!m_hostname.empty())
        ss << "//" << m_hostname << '/';
    if (!m_namespace.empty())
        ss << m_namespace << ':';
    ss << m_classname << '.';

    const NocaseDict &keybindings = lmi::extract<const NocaseDict&>(m_keybindings);
    nocase_map_t::const_iterator it;
    for (it = keybindings.begin(); it != keybindings.end(); ++it) {
        ss << it->first << '=';
        if (isinstance(it->second, CIMInstanceName::type())) {
            std::string iname_str = object_as_std_string(it->second);
            boost::replace_all(iname_str, "\\", "\\\\");
            boost::replace_all(iname_str, "\"", "\\\"");
            ss << '"' << iname_str << '"';
        } else if (isbasestring(it->second)) {
            ss << '"' << object_as_std_string(it->second) << '"';
        } else {
            ss << object_as_std_string(it->second);
        }

        nocase_map_t::const_iterator tmp_it = it;
        if (tmp_it != keybindings.end() && ++tmp_it != keybindings.end())
            ss << ',';
    }

    return ss.str();
}

std::string CIMInstanceName::repr()
{
    std::stringstream ss;
    ss << "CIMInstanceName(classname='" << m_classname << "', keybindings="
       << object_as_std_string(m_keybindings);
    if (!m_hostname.empty())
        ss << ", host='" << m_hostname << '\'';
    ss <<  ", namespace='"
       << m_namespace << "')";
    return ss.str();
}

bp::object CIMInstanceName::keys()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.keys();
}

bp::object CIMInstanceName::values()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.values();
}

bp::object CIMInstanceName::items()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.items();
}

bp::object CIMInstanceName::iterkeys()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.iterkeys();
}

bp::object CIMInstanceName::itervalues()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.itervalues();
}

bp::object CIMInstanceName::iteritems()
{
    NocaseDict &keybindings = lmi::extract<NocaseDict&>(m_keybindings);
    return keybindings.iteritems();
}

void CIMInstanceName::setClassname(const bp::object &classname)
{
    m_classname = lmi::extract_or_throw<std::string>(classname);
}

void CIMInstanceName::setNamespace(const bp::object &namespace_)
{
    m_namespace = lmi::extract_or_throw<std::string>(namespace_);
}

void CIMInstanceName::setHostname(const bp::object &hostname)
{
    m_hostname = lmi::extract_or_throw<std::string>(hostname);
}

void CIMInstanceName::setKeybindings(const bp::object &keybindings)
{
    m_keybindings = lmi::get_or_throw<NocaseDict, bp::dict>(keybindings);
}
