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
#include <sstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include "lmiwbem_client_wsman_builder.h"
#include "util/lmiwbem_string.h"

namespace ptree = boost::property_tree;

namespace {

String get_class_from_resource_uri(const String &resource_uri)
{
    size_t pos = resource_uri.rfind('/');
    if (pos != resource_uri.npos)
        return resource_uri.substr(pos + 1);
    return resource_uri;
}

String xml_omit_namespace(const String &name)
{
    size_t pos = name.find(':');
    if (pos != name.npos)
        return name.substr(pos + 1);
    return name;
}

bool ptree_node_name_equals(
    const String &xml_node,
    const String &xml_node_without_namespace)
{
    String xml_node_stripped(xml_omit_namespace(xml_node));
    return xml_node_stripped == xml_node_without_namespace;
}

bool ptree_is_xmlattr(const std::string &node_name)
{
    return node_name == "<xmlattr>";
}

ptree::ptree ptree_walk(const ptree::ptree &pt, const String &path)
{
    std::stringstream ss(path);
    String member;

    bool found = false;
    ptree::ptree::const_iterator p(pt.begin());
    ptree::ptree::const_iterator it(pt.begin());
    ptree::ptree::const_iterator end(pt.end());

    while (getline(ss, member, '.')) {
        found = false;
        for (; !found && it != end; ++it) {
            if (ptree_node_name_equals(it->first, member)) {
                p = it;
                end = it->second.end();
                it = it->second.begin();
                found = true;
            }
        }

        if (!found)
            throw ptree::ptree_bad_path("No such node", ptree::ptree::path_type(path));
    }

    return p->second;
}

} // unnamed namespace

// Throws boost::property_tree::exceptions
Pegasus::CIMInstance ObjectFactory::makeCIMInstance(const String &xml)
{
    std::stringstream ss(xml);
    ptree::ptree pt;

    // Read the XML.
    ptree::read_xml(ss, pt);

    // Walk to CIM Instance.
    ptree::ptree instance = pt.get_child("");
    ptree::ptree::const_iterator it = instance.begin();
    if (ptree_is_xmlattr(it->first))
        ++it;

    return makeCIMInstance(xml_omit_namespace(it->first), it->second);
}

// Throws boost::property_tree::exceptions
Pegasus::CIMInstance ObjectFactory::makeCIMInstance(
    const String &classname,
    const ptree::ptree &instance)
{
    Pegasus::CIMInstance peg_instance((Pegasus::CIMName(classname)));
    bool xmlattr_skipped = false;
    for (ptree::ptree::const_iterator it = instance.begin();
         it != instance.end();
         ++it)
    {
        if (!xmlattr_skipped && ptree_is_xmlattr(it->first)) {
            xmlattr_skipped = true;
            continue;
        }

        // XXX: Can WSMAN give us EPR?

        // Add property to CIMInstance.
        String prop_name(xml_omit_namespace(it->first));
        String prop_value(String(it->second.data()));
        Pegasus::CIMValue peg_value;
        Pegasus::Uint32 prop_id = peg_instance.findProperty(
            Pegasus::CIMName(prop_name));

        if (prop_id == Pegasus::PEG_NOT_FOUND) {
            peg_value.set(prop_value);
        } else {
            // Property with such name has already been set; make it an array
            // and continue pushing to it.
            Pegasus::CIMProperty existing_property(
                peg_instance.getProperty(prop_id));

            const Pegasus::CIMValue &cpeg_value = existing_property.getValue();
            Pegasus::Array<Pegasus::String> peg_array;

            if (cpeg_value.isArray()) {
                cpeg_value.get(peg_array);
            } else {
                Pegasus::String peg_str;
                cpeg_value.get(peg_str);
                peg_array.append(peg_str);
            }

            peg_array.append(prop_value);
            peg_value.set(peg_array);

            peg_instance.removeProperty(prop_id);
        }

        peg_instance.addProperty(
            Pegasus::CIMProperty(
                Pegasus::CIMName(prop_name),
                peg_value));
    }

    return peg_instance;
}

// Throws boost::property_tree::exceptions
Pegasus::CIMInstance ObjectFactory::makeCIMInstanceWithPath(
    const String &xml,
    const String &hostname,
    const String &namespace_)
{
    std::stringstream ss(xml);
    ptree::ptree pt;

    // Read the XML.
    ptree::read_xml(ss, pt);

    // Walk to the class node.
    ptree::ptree instance = ptree_walk(pt, "Item").get_child("");
    ptree::ptree::const_iterator it = instance.begin();
    if (ptree_is_xmlattr(it->first))
        ++it;
    Pegasus::CIMInstance peg_instance = makeCIMInstance(
        xml_omit_namespace(it->first), it->second);

    // Walk to EPR node.
    ptree::ptree epr = ptree_walk(pt, "Item.EndpointReference");
    peg_instance.setPath(makeCIMInstanceName(epr, hostname, namespace_));

    return peg_instance;
}

// Throws boost::property_tree::exceptions
Pegasus::CIMObjectPath ObjectFactory::makeCIMInstanceName(
    const ptree::ptree &epr,
    const String &hostname,
    const String &namespace_)
{
    // Walk the EPR tree to SelectorSet.
    ptree::ptree reference_params = ptree_walk(epr, "ReferenceParameters");
    ptree::ptree resource_uri = ptree_walk(reference_params, "ResourceURI");
    ptree::ptree selector_set = ptree_walk(reference_params, "SelectorSet");

    // Get class name from ResourceURI.
    String classname(get_class_from_resource_uri(resource_uri.data()));

    // Fill the keybindings.
    Pegasus::Array<Pegasus::CIMKeyBinding> peg_keybindings;
    ptree::ptree::const_iterator it;
    for (it = selector_set.begin(); it != selector_set.end(); ++it) {
        String name = it->second.get_child("<xmlattr>.Name").data();
        String value = it->second.data();

        // XXX: Can WSMAN give us EPR?

        peg_keybindings.append(
            Pegasus::CIMKeyBinding(
                Pegasus::CIMName(name),
                value,
                Pegasus::CIMKeyBinding::STRING));
    }

    return Pegasus::CIMObjectPath(
        hostname,
        Pegasus::CIMNamespaceName(namespace_),
        Pegasus::CIMName(classname),
        peg_keybindings);
}

// Throws boost::property_tree::exceptions
Pegasus::CIMObjectPath ObjectFactory::makeCIMInstanceName(
    const String &xml,
    const String &hostname,
    const String &namespace_)
{
    std::stringstream ss(xml);

    // Read the XML.
    ptree::ptree pt;
    ptree::read_xml(ss, pt);

    // Walk the XML tree to EndpointReference.
    ptree::ptree epr = ptree_walk(pt, "EndpointReference");

    return makeCIMInstanceName(epr, hostname, namespace_);
}

// Throws boost::property_tree::exceptions
Pegasus::CIMValue ObjectFactory::makeMethodReturnValue(
    const String &xml,
    const String &method_name,
    Pegasus::Array<Pegasus::CIMParamValue> &out_parameters)
{
    std::stringstream ss(xml);

    // Read the XML.
    ptree::ptree pt;
    ptree::read_xml(ss, pt);

    // Walk to method_name_OUTPUT
    ptree::ptree method = ptree_walk(
        pt,
        method_name + String("_OUTPUT"));

    bool got_xmlattr = false;
    bool got_return_value = false;
    String return_value;
    ptree::ptree::const_iterator it;
    for (it = method.begin(); it != method.end(); ++it) {
        if (!got_xmlattr && ptree_is_xmlattr(it->first)) {
            got_xmlattr = true;
            continue;
        } else if (!got_return_value && ptree_node_name_equals(it->first, "ReturnValue")) {
            got_return_value = true;
            return_value = it->second.data();
        } else {
            out_parameters.append(
                Pegasus::CIMParamValue(
                    String(it->first),
                    Pegasus::CIMValue(
                        String(it->second.data()))));
        }
    }

    return Pegasus::CIMValue(return_value);
}

#ifdef DEBUG
#  include <iostream>
#  include <fstream>
#  include <sstream>

Pegasus::CIMInstance readCIMInstance(const String &filename)
{
    std::ifstream fin(filename.c_str());
    std::stringstream ss;
    ss << fin.rdbuf();

    return Pegasus::CIMInstance(
        ObjectFactory::makeCIMInstanceWithPath(
            ss.str(),
            "hostname",
            "root/cimv2"));
}

void printCIMInstance(const Pegasus::CIMInstance &instance)
{
    std::cout << instance.getClassName().getString() << ':' << std::endl;

    for (Pegasus::Uint32 i = 0; i < instance.getPropertyCount(); ++i) {
        const Pegasus::CIMConstProperty &p = instance.getProperty(i);
        std::cout << "    " << p.getName().getString() << " : ";
        if (p.isArray()) {
            Pegasus::Array<Pegasus::String> arr;
            p.getValue().get(arr);
            for (Pegasus::Uint32 j = 0; j < arr.size(); ++j)
                std::cout << arr[j] << ' ';
            std::cout << std::endl;
        } else {
            Pegasus::String v;
            p.getValue().get(v);
            std::cout << v << std::endl;
        }
    }
}


int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "XML file missing!" << std::endl;
        return 1;
    }

    printCIMInstance(
        readCIMInstance(argv[1]));

    return 0;
}

#endif // DEBUG
