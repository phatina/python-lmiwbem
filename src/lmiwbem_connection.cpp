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

#include <boost/python/class.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/tuple.hpp>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include "lmiwbem_class.h"
#include "lmiwbem_class_name.h"
#include "lmiwbem_connection.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_util.h"

namespace bp = boost::python;

std::string WBEMConnection::s_default_namespace = "root/cimv2";

WBEMConnection::WBEMConnection(const bp::object &verify_cert)
    : m_connected_tmp(false)
    , m_url()
    , m_username()
    , m_password()
    , m_default_namespace(s_default_namespace)
    , m_client()
    , m_mutex()
{
    bool verify = lmi::extract_or_throw<bool>(verify_cert, "verify_certificate");
    m_client.setVerifyCertificate(verify);
}

WBEMConnection::WBEMConnection(
    const bp::object &url,
    const bp::object &creds,
    const bp::object &default_namespace,
    const bp::object &verify_cert)
    : m_connected_tmp(false)
    , m_url()
    , m_username()
    , m_password()
    , m_default_namespace(s_default_namespace)
    , m_client()
    , m_mutex()
{
    m_url = lmi::extract_or_throw<std::string>(url, "url");

    if (creds != bp::object()) {
        bp::tuple creds_tpl = lmi::extract_or_throw<bp::tuple>(creds, "creds");
        if (bp::len(creds_tpl) != 2)
            throw_ValueError("creds must be tuple of 2 strings");
        m_username = lmi::extract_or_throw<std::string>(creds_tpl[0], "username");
        m_password = lmi::extract_or_throw<std::string>(creds_tpl[1], "password");
    }

    bool verify = lmi::extract_or_throw<bool>(verify_cert, "verify_certificate");
    m_client.setVerifyCertificate(verify);

    if (default_namespace != bp::object()) {
        m_default_namespace = lmi::extract_or_throw<std::string>(
            default_namespace, "default_namespace");
    }
}

WBEMConnection::~WBEMConnection()
{
    m_client.disconnect();
}

void WBEMConnection::init_type()
{
    bp::class_<WBEMConnection, boost::noncopyable>(
        "WBEMConnection",
        bp::init<
            const bp::object &>((
                bp::arg("verify_certificate") = true))
        )
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("url"),
                bp::arg("creds") = bp::object(),
                bp::arg("default_namespace") = bp::object(),
                bp::arg("verify_certificate") = true),
                "Constructs :py:class:`WBEMConnection` object.\n\n"
                ":param str url: String containing URL of CIMOM instance\n"
                ":param tuple creds: tuple containing two string, where the first\n"
                "\tone stands for username, second for password\n"
                ":param bool verify_certificate: set to True, if CIMOM's X509 certificate\n"
                "\t shall be verified; False otherwise. Default value is True.\n"))
        .def("connect", &WBEMConnection::connect,
            (bp::arg("url") = bp::object(),
             bp::arg("username") = bp::object(),
             bp::arg("password") = bp::object(),
             bp::arg("verify_certificate") = bp::object()
            ),
            "Connects to CIMOM.\n\n"
            ":param str url: String containing url of remote CIMOM.\n"
            ":param str username: String containing username for authentication.\n"
            ":param str password: String containing password for authentication.\n"
            ":param bool verify_certificate: set to True, if CIMOM's X509 certificate\n"
            "\t shall be verified; False otherwise. Default value is True.\n"
            ":raises: :py:exc:ConnectionError:"
        )
        .def("disconnect",
            &WBEMConnection::disconnect,
            "Disconnects from CIMOM.")
        .add_property("is_connected",
            &WBEMConnection::isConnected,
            "Property, which return True, if the client is connected to certain CIMOM.\n\n"
            ":rtype: bool")
        .add_property("verify_certificate",
            &WBEMConnection::getVerifyCertificate,
            &WBEMConnection::setVerifyCertificate,
            "Property storing X509 certificate verification flag.\n\n"
            ":returns: True, if the X509 certificate shall be verified; False otherwise.\n"
            ":rtype: bool")
        .def("EnumerateInstances", &WBEMConnection::enumerateInstances,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("DeepInheritance") = true,
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
           "Enumerates instances of a given class name.\n\n"
           ":param str ClassName: String containing class name of instances to be\n"
           "\tretrieved.\n"
           ":param str namespace: String containing namespace, from which the\n"
           "\tinstances should be retrieved.\n"
           ":param bool LocalOnly: Indicates, if any CIM elements (properties,\n"
           "\tmethods, and qualifiers) except those added or" "overridden in the class as\n"
           "\tspecified in the classname input parameter shall not be included in the\n"
           "\treturned class.\n"
           ":param bool IncludeQualifiers: Indicates, if all qualifiers for each class\n"
           "\t(including qualifiers on the class and on any returned properties, methods,\n"
           "\tor method parameters) shall be included as ``<QUALIFIER>`` elements in the\n"
           "\tresponse.\n"
           ":param bool IncludeClassOrigin: Indicates, if the ``CLASSORIGIN`` attribute\n"
           "\tshall be present on all appropriate elements in each returned class.\n"
           ":param list PropertyList: if present and not None, the members of the list\n"
           "\tdefine one or more property names. The returned class shall not include\n"
           "\telements for properties missing from this list. Note that if LocalOnly is\n"
           "\tspecified as True, it acts as an additional filter on the set of properties\n"
           "\treturned. For example, if property A is included in the PropertyList but\n"
           "\tLocalOnly is set t True and A is not local to the requested class, it is not\n"
           "\tincluded in the response. If the PropertyList input parameter is an empty\n"
           "\tlist, no properties are included in the response. If the PropertyList input\n"
           "\tparameter is None, no additional filtering is defined. Default value is None.\n"
           ":returns: List of :py:class:`CIMInstance` objects.\n"
           ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("EnumerateInstanceNames", &WBEMConnection::enumerateInstanceNames,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object()),
            "Enumerates instance names of a given class name.\n\n"
            ":param str ClassName: String containing class name of instance\n"
            "\tnames to be retrieved.\n"
            ":param str namespace: String containing namespace, from which the\n"
            "\tinstance names will be retrieved.\n"
            ":returns: List of :py:class:`CIMInstanceName` objects."
            ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("GetInstance", &WBEMConnection::getInstance,
            (bp::arg("InstanceName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "Fetches a :py:class:`CIMInstance` from CIMOM identified by\n"
            ":py:class:`CIMInstanceName`.\n\n"
            ":param CIMInstanceName InstanceName: :py:class:`CIMInstanceName`,\n"
            "\twhich identifies a :py:class:`CIMInstance`\n"
            ":param str namespace: string containing namespace, from which the\n"
            "\tinstance will be retrieved\n"
            ":param bool LocalOnly: indicates, if any CIM elements (properties,\n"
            "\tmethods, and qualifiers) except those added or" "overridden in the class as\n"
            "\tspecified in the classname input parameter shall not be included in the\n"
            "\treturned class.\n"
            ":param bool IncludeQualifiers: indicates, if all qualifiers for each class\n"
            "\t(including qualifiers on the class and on any returned properties, methods,\n"
            "\tor method parameters) shall be included as ``<QUALIFIER>`` elements in the\n"
            "\tresponse.\n"
            ":param bool IncludeClassOrigin: indicates, if the ``CLASSORIGIN`` attribute\n"
            "\tshall be present on all appropriate elements in each returned class.\n"
            ":param list PropertyList: if present and not None, the members of the list\n"
            "\tdefine one or more property names. The returned class shall not include\n"
            "\telements for properties missing from this list. Note that if LocalOnly is\n"
            "\tspecified as True, it acts as an additional filter on the set of properties\n"
            "\treturned. For example, if property A is included in the PropertyList but\n"
            "\tLocalOnly is set t True and A is not local to the requested class, it is not\n"
            "\tincluded in the response. If the PropertyList input parameter is an empty\n"
            "\tlist, no properties are included in the response. If the PropertyList input\n"
            "\tparameter is None, no additional filtering is defined. Default value is None.\n"
            ":returns: :py:class:`CIMInstance` object."
            ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("EnumerateClasses", &WBEMConnection::enumerateClasses,
            (bp::arg("namespace") = bp::object(),
             bp::arg("ClassName") = bp::object(),
             bp::arg("DeepInheritance") = false,
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = true,
             bp::arg("IncludeClassOrigin") = false),
            "Enumerates classes managed by the CIMOM.\n\n"
            ":param str namespace: string containing namespace, from which the\n"
            "\tclasses  will be retrieved\n"
            ":param str ClassName: string containing a class name, which defines the\n"
            "\tclass that is the basis for the enumeration\n"
            ":param bool DeepInheritance: indicates, if all subclasses of the specified\n"
            "\tclass should be returned. If the ClassName input parameter is absent, this\n"
            "\timplies that all classes in the target namespace should be returned. If\n"
            "\tDeepInheritance is false, only immediate child subclasses are returned. If the\n"
            "\tClassName input parameter is None, this implies that all top-level classes\n"
            "\t(that is, classes with no superclass) in the target namespace should be\n"
            "\treturned. This definition of DeepInheritance applies only to the\n"
            "\tEnumerateClasses and EnumerateClassName operations.\n"
            ":param bool LocalOnly: indicates, if any CIM elements (properties, methods,\n"
            "\tand qualifiers) except those added or overridden in the class as specified in\n"
            "\tthe classname input parameter shall not be included in the returned class. If\n"
            "\tit is false, this parameter defines no additional filtering.\n"
            ":param bool IncludeQualifiers: indicates, if all qualifiers for each class\n"
            "\t(including qualifiers on the class and on any returned properties, methods, or\n"
            "\tmethod parameters) shall be included as <QUALIFIER> XML elements in the\n"
            "\tresponse. If it is false, no <QUALIFIER> XML elements are present.\n"
            ":param bool IncludeClassOrigin: indicates, if the CLASSORIGIN attribute shall\n"
            "\tbe present on all appropriate elements in each returned class. If it is false,\n"
            "\tno CLASSORIGIN attributes are present.\n"
            ":returns: list of :py:class:`CIMClass` objects"
            ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("EnumerateClassNames", &WBEMConnection::enumerateClassNames,
            (bp::arg("namespace") = bp::object(),
             bp::arg("ClassName") = bp::object(),
             bp::arg("DeepInheritance") = false),
            "Enumerates class names managed by the CIMOM.\n\n"
            ":param str namespace: string containing namespace, from which the\n"
            "\tclasses  will be retrieved\n"
            ":param str ClassName: string containing a class name, which defines the\n"
            "\tclass that is the basis for the enumeration\n"
            ":param bool DeepInheritance: indicates, if all subclasses of the specified\n"
            "\tclass should be returned. If the ClassName input parameter is absent, this\n"
            "\timplies that all classes in the target namespace should be returned. If\n"
            "\tDeepInheritance is false, only immediate child subclasses are returned. If the\n"
            "\tClassName input parameter is None, this implies that all top-level classes\n"
            "\t(that is, classes with no superclass) in the target namespace should be\n"
            "\treturned. This definition of DeepInheritance applies only to the\n"
            "\tEnumerateClasses and EnumerateClassName operations.\n"
            ":returns: list of strings of class names"
            ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("GetClass", &WBEMConnection::getClass,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = true,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "Returns a :py:class:`CIMClass` representing the named class.\n\n"
            ":param str ClassName: defines the name of the class to be retrieved.\n"
            ":param str namespace: string containing namespace, from which the\n"
            "\tclass will be retrieved\n"
            ":param bool LocalOnly: indicates, if any CIM elements (properties, methods,\n"
            "\tand qualifiers), except those added or overridden in the class as specified in\n"
            "\tthe classname input parameter, shall not be included in the returned class. If\n"
            "\tit is false, no additional filtering is defined.\n"
            ":param bool IncludeQualifiers: indicates, if all qualifiers for that class\n"
            "\t(including qualifiers on the class and on any returned properties, methods, or\n"
            "\tmethod parameters) shall be included as <QUALIFIER> XML elements in the\n"
            "\tresponse. If it is false, no <QUALIFIER> XML elements are present in the\n"
            "\treturned class.\n"
            ":param bool IncludeClassOrigin: indicates, if all qualifiers for that class\n"
            "\t(including qualifiers on the class and on any returned properties, methods, or\n"
            "\tmethod parameters) shall be included as <QUALIFIER> XML elements in the\n"
            "\tresponse. If it is false, no <QUALIFIER> XML elements are present in the\n"
            "\treturned class.\n"
            ":param list PropertyList: if not None, the members of the array define one or\n"
            "\tmore property names. The returned class shall not include any properties\n"
            "\tmissing from this list. Note that if LocalOnly is specified as true, it acts\n"
            "\tas an additional filter on the set of properties returned. For example, if\n"
            "\tproperty A is included in PropertyList but LocalOnly is set to true and A is\n"
            "\tnot local to the requested class, it is not included in the response. If the\n"
            "\tPropertyList input parameter is an empty array, no properties are included in\n"
            "\tthe response. If the PropertyList input parameter is None, no additional\n"
            "\tfiltering is defined.\n"
            ":returns: :py:class:`CIMClass` object."
            ":raises: :py:exc:`CIMError`, :py:exc:`ConnectionError`.")
        .def("Associators", &WBEMConnection::getAssociators,
            (bp::arg("ObjectName"),
             bp::arg("AssocClass") = bp::object(),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("ResultRole") = bp::object(),
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "Returns a list of associated :py:class:`CIMInstance` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the associated\n"
            "\tinstances will be enumerated.\n"
            ":param string AssocClass: valid CIM association class name. It acts as a filter on\n"
            "\tthe returned set of names by mandating that each returned name identify an\n"
            "\tobject that shall be associated to the source object through an instance of\n"
            "\tthis class or one of its subclasses.\n"
            ":param string ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of names by mandating that each returned name identify an object\n"
            "\tthat shall be either an instance of this class (or one of its subclasses) or\n"
            "\tbe this class (or one of its subclasses).\n"
            ":param string Role: valid property name. It acts as a filter on the returned set\n"
            "\tof names by mandating that each returned name identify an object that shall be\n"
            "\tassociated to the source object through an association in which the source\n"
            "\tobject plays the specified role. That is, the name of the property in the\n"
            "\tassociation class that refers to the source object shall match the value of\n"
            "\tthis parameter.\n"
            ":param string ResultRole: valid property name. It acts as a filter on the returned\n"
            "\tset of names by mandating that each returned name identify an object that\n"
            "\tshall be associated to the source object through an association in which the\n"
            "\tnamed returned object plays the specified role. That is, the name of the\n"
            "\tproperty in the association class that refers to the returned object shall\n"
            "\tmatch the value of this parameter.\n"
            ":param bool IncludeQualifiers: indicates, if all qualifiers for each object\n"
            "\t(including qualifiers on the object and on any returned properties) shall be\n"
            "\tincluded as ``<QUALIFIER>`` elements in the response. Default value is False.\n"
            ":param bool IncludeClassOrigin: indicates, if the ``CLASSORIGIN`` attribute shall\n"
            "\tbe present on all appropriate elements in each returned object. Default value\n"
            "\tis False.\n"
            ":param list PropertyList: if not None, the members of the array define one or more\n"
            "\tproperty names. Each returned object shall not include elements for any\n"
            "\tproperties missing from this list. If PropertyList is an empty list, no\n"
            "\tproperties are included in each returned object. If it is None, no additional\n"
            "\tfiltering is defined. Default value is None.\n"
            ":returns: list of associated :py:class:`CIMInstance` objects with an input\n"
            "\tinstance\n"
            ":raises: :py:exc:`pywbem.CIMError`, :py:exc:`pywbem.ConnectionError`.")
        .def("AssociatorNames", &WBEMConnection::getAssociatorNames,
            (bp::arg("ObjectName"),
             bp::arg("AssocClass") = bp::object(),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("ResultRole") = bp::object()),
            "Returns a list of associated :py:class:`CIMInstanceName` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the associated\n"
            "\tinstance names will be enumerated.\n"
            ":param string AssocClass: valid CIM association class name. It acts as a filter on\n"
            "\tthe returned set of names by mandating that each returned name identify an\n"
            "\tobject that shall be associated to the source object through an instance of\n"
            "\tthis class or one of its subclasses.\n"
            ":param string ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of names by mandating that each returned name identify an object\n"
            "\tthat shall be either an instance of this class (or one of its subclasses) or\n"
            "\tbe this class (or one of its subclasses).\n"
            ":param string Role: valid property name. It acts as a filter on the returned set\n"
            "\tof names by mandating that each returned name identify an object that shall be\n"
            "\tassociated to the source object through an association in which the source\n"
            "\tobject plays the specified role. That is, the name of the property in the\n"
            "\tassociation class that refers to the source object shall match the value of\n"
            "\tthis parameter.\n"
            ":param string ResultRole: valid property name. It acts as a filter on the returned\n"
            "\tset of names by mandating that each returned name identify an object that\n"
            "\tshall be associated to the source object through an association in which the\n"
            "\tnamed returned object plays the specified role. That is, the name of the\n"
            "\tproperty in the association class that refers to the returned object shall\n"
            "\tmatch the value of this parameter.\n"
            ":returns: list of associated :py:class:`CIMInstanceName` objects with\n"
            "\tan input instance\n"
            ":raises: :py:exc:`pywbem.CIMError`, :py:exc:`pywbem.ConnectionError`.")
        .def("References", &WBEMConnection::getReferences,
            (bp::arg("ObjectName"),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "Returns a list of association :py:class:`CIMInstance` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the association\n"
            "\tinstances will be enumerated.\n"
            ":param string ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of objects by mandating that each returned object shall be an\n"
            "\tinstance of this class (or one of its subclasses) or this class (or one of its\n"
            "\tsubclasses). Default value is None.\n"
            ":param string Role: valid property name. It acts as a filter on the returned set\n"
            "\tof objects by mandating that each returned object shall refer to the target\n"
            "\tobject through a property with a name that matches the value of this\n"
            "\tparameter. Default value is None.\n"
            ":param bool IncludeQualifiers: bool flag indicating, if all qualifiers for each\n"
            "\tobject (including qualifiers on the object and on any returned properties)\n"
            "\tshall be included as ``<QUALIFIER>`` elements in the response. Default value\n"
            "\tis False.\n"
            ":param bool IncludeClassOrigin: bool flag indicating, if the ``CLASSORIGIN``\n"
            "\tattribute shall be present on all appropriate elements in each returned\n"
            "\tobject. Default value is False.\n"
            ":param list PropertyList: if not None, the members of the list define one or more\n"
            "\tproperty names. Each returned object shall not include elements for any\n"
            "\tproperties missing from this list. If PropertyList is an empty list, no\n"
            "\tproperties are included in each returned object. If PropertyList is None, no\n"
            "\tadditional filtering is defined. Default value is None.\n"
            ":returns: list of association :py:class:`CIMInstance` objects with an input\n"
            "\tinstance\n"
            ":raises: :py:exc:`pywbem.CIMError`, :py:exc:`pywbem.ConnectionError`.")
    ;
}

void WBEMConnection::connect(
    const bp::object &url,
    const bp::object &username,
    const bp::object &password,
    const bp::object &verify_cert)
{
    // All three parameters can be omitted, WBEMConnection was has been
    // with such parameters; WBEMConnection(url, (username, password)).
    std::string std_url(m_url);
    std::string std_username(m_username);
    std::string std_password(m_password);

    // Check, if provided parameters are None. If not, try to get a string value
    // out of them.
    if (url != bp::object())
        std_url = lmi::extract_or_throw<std::string>(url, "url");
    if (username != bp::object())
        std_username = lmi::extract_or_throw<std::string>(username, "username");
    if (password != bp::object())
        std_password = lmi::extract_or_throw<std::string>(password, "password");

    // Check, if we have any url to connect to
    if (std_url.empty())
        throw_ValueError("url parameter missing");

    if (verify_cert != bp::object()) {
        bool verify = lmi::extract_or_throw<bool>(verify_cert, "verify_certificate");
        m_client.setVerifyCertificate(verify);
    }

    try {
        m_client.connect(
            Pegasus::String(std_url.c_str()),
            Pegasus::String(std_username.c_str()),
            Pegasus::String(std_password.c_str()));
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }
}

void WBEMConnection::disconnect()
{
    m_client.disconnect();
}

void WBEMConnection::connectTmp()
{
    if (m_client.isConnected() || m_connected_tmp)
        return;

    if (m_url.empty())
        throw_ValueError("WBEMConnection constructed without url parameter");

    m_client.connect(
        Pegasus::String(m_url.c_str()),
        Pegasus::String(m_username.c_str()),
        Pegasus::String(m_password.c_str()));
    m_connected_tmp = true;
}

void WBEMConnection::disconnectTmp()
{
    if (!m_connected_tmp)
        return;

    m_client.disconnect();
    m_connected_tmp = false;
}

bp::list WBEMConnection::enumerateInstances(
    const bp::object &cls,
    const bp::object &ns,
    const bool local_only,
    const bool deep_inheritance,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list)
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::Array<Pegasus::CIMInstance> cim_instances;
    try {
        Pegasus::CIMPropertyList cim_property_list(
            ListConv::asPegasusPropertyList(property_list,
            "PropertyList"));

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_instances = m_client.enumerateInstances(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            Pegasus::CIMName(std_cls.c_str()),
            deep_inheritance,
            local_only,
            include_qualifiers,
            include_class_origin,
            cim_property_list);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list instances;
    const Pegasus::Uint32 cnt = cim_instances.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        bp::object instance = CIMInstance::create(cim_instances[i]);
        instances.append(instance);
    }

    return instances;
}

bp::list WBEMConnection::enumerateInstanceNames(
    const bp::object &cls,
    const bp::object &ns)
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::Array<Pegasus::CIMObjectPath> cim_instance_names;
    try {
        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_instance_names = m_client.enumerateInstanceNames(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            Pegasus::CIMName(std_cls.c_str()));
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list instance_names;
    const Pegasus::Uint32 cnt = cim_instance_names.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        bp::object instance_name = CIMInstanceName::create(
            cim_instance_names[i], std_ns, m_client.hostname());
        instance_names.append(instance_name);
    }

    return instance_names;
}

bp::object WBEMConnection::getInstance(
    const bp::object &instance_name,
    const bp::object &ns,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list)
{
    CIMInstanceName &cim_instance_name = lmi::extract_or_throw<CIMInstanceName&>(
        instance_name, "InstanceName");
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMInstance cim_instance;
    try {
        Pegasus::CIMPropertyList cim_property_list(
            ListConv::asPegasusPropertyList(property_list, "PropertyList"));
        Pegasus::CIMObjectPath cim_object_path = cim_instance_name.asCIMObjectPath();

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_instance = m_client.getInstance(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            cim_object_path,
            local_only,
            include_qualifiers,
            include_class_origin,
            cim_property_list);
        disconnectTmp();

        // CIMClient::getInstance() does not set the CIMObjectPath member in
        // CIMInstance. We need to do that manually.
        cim_instance.setPath(cim_object_path);
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    return CIMInstance::create(cim_instance);
}

bp::list WBEMConnection::enumerateClasses(
    const bp::object &ns,
    const bp::object &cls,
    const bool deep_inheritance,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin)
{
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMName classname;
    if (cls != bp::object()) {
        std::string std_cls(lmi::extract_or_throw<std::string>(cls, "ClassName"));
        classname = Pegasus::CIMName(std_cls.c_str());
    }

    Pegasus::Array<Pegasus::CIMClass> cim_classes;
    try {
        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_classes = m_client.enumerateClasses(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            classname,
            deep_inheritance,
            local_only,
            include_qualifiers,
            include_class_origin);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list classes;
    const Pegasus::Uint32 cnt = cim_classes.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        classes.append(CIMClass::create(cim_classes[i]));

    return classes;
}

bp::list WBEMConnection::enumerateClassNames(
    const bp::object &ns,
    const bp::object &cls,
    const bool deep_inheritance)
{
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMName classname;
    if (cls != bp::object()) {
        std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
        classname = Pegasus::CIMName(std_cls.c_str());
    }

    Pegasus::Array<Pegasus::CIMName> cim_classnames;
    try {
        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_classnames = m_client.enumerateClassNames(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            classname,
            deep_inheritance);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    // We do not create lmiwbem.CIMClassName objects here; we try to mimic pywbem.
    bp::list classnames;
    const Pegasus::Uint32 cnt = cim_classnames.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        classnames.append(bp::object(cim_classnames[i]));

    return classnames;
}

bp::object WBEMConnection::getClass(
    const bp::object &cls,
    const bp::object &ns,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list)
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "ClassName"));
    std::string std_ns(s_default_namespace);
    if (ns != bp::object())
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMClass cim_class;
    try {
        Pegasus::CIMPropertyList cim_property_list(
            ListConv::asPegasusPropertyList(property_list,
            "PropertyList"));

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_class = m_client.getClass(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            Pegasus::CIMName(std_cls.c_str()),
            local_only,
            include_qualifiers,
            include_class_origin,
            cim_property_list);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    return CIMClass::create(cim_class);
}

bp::object WBEMConnection::getAssociators(
    const bp::object &object_path,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object property_list)
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asCIMObjectPath();

    std::string std_ns(s_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_assoc_class;
    std::string std_result_class;
    std::string std_role;
    std::string std_result_role;
    if (assoc_class != bp::object())
        std_assoc_class = lmi::extract_or_throw<std::string>(assoc_class, "AssocClass");
    if (result_class != bp::object())
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (role != bp::object())
        std_role = lmi::extract_or_throw<std::string>(role, "Role");
    if (result_role != bp::object())
        std_result_role = lmi::extract_or_throw<std::string>(result_role, "ResultRole");

    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> cim_associators;
    try {
        Pegasus::CIMName cim_assoc_class;
        Pegasus::CIMName cim_result_class;
        Pegasus::String  cim_role = Pegasus::String::EMPTY;
        Pegasus::String  cim_result_role = Pegasus::String::EMPTY;

        if (!std_assoc_class.empty())
            cim_assoc_class = Pegasus::CIMName(std_assoc_class.c_str());
        if (!std_result_class.empty())
            cim_result_class = Pegasus::CIMName(std_result_class.c_str());
        if (!std_role.empty())
            cim_role = std_role.c_str();
        if (!std_result_role.empty())
            cim_result_role = std_result_role.c_str();

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_associators = m_client.associators(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            cim_path,
            cim_assoc_class,
            cim_result_class,
            cim_role,
            cim_result_role,
            include_qualifiers,
            include_class_origin,
            cim_property_list);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list associators;
    const Pegasus::Uint32 cnt = cim_associators.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        associators.append(CIMInstance::create(cim_associators[i]));

    return associators;
}

bp::object WBEMConnection::getAssociatorNames(
    const bp::object &object_path,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role)
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asCIMObjectPath();

    std::string std_ns(s_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_assoc_class;
    std::string std_result_class;
    std::string std_role;
    std::string std_result_role;
    if (assoc_class != bp::object())
        std_assoc_class = lmi::extract_or_throw<std::string>(assoc_class, "AssocClass");
    if (result_class != bp::object())
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (role != bp::object())
        std_role = lmi::extract_or_throw<std::string>(role, "Role");
    if (result_role != bp::object())
        std_result_role = lmi::extract_or_throw<std::string>(result_role, "ResultRole");

    Pegasus::Array<Pegasus::CIMObjectPath> cim_associator_names;
    try {
        Pegasus::CIMName cim_assoc_class;
        Pegasus::CIMName cim_result_class;
        Pegasus::String  cim_role = Pegasus::String::EMPTY;
        Pegasus::String  cim_result_role = Pegasus::String::EMPTY;

        if (!std_assoc_class.empty())
            cim_assoc_class = Pegasus::CIMName(std_assoc_class.c_str());
        if (!std_result_class.empty())
            cim_result_class = Pegasus::CIMName(std_result_class.c_str());
        if (!std_role.empty())
            cim_role = std_role.c_str();
        if (!std_result_role.empty())
            cim_result_role = std_result_role.c_str();

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_associator_names = m_client.associatorNames(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            cim_path,
            cim_assoc_class,
            cim_result_class,
            cim_role,
            cim_result_role);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list associator_names;
    const Pegasus::Uint32 cnt = cim_associator_names.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        associator_names.append(CIMInstanceName::create(cim_associator_names[i]));

    return associator_names;
}

bp::object WBEMConnection::getReferences(
    const bp::object &object_path,
    const bp::object &result_class,
    const bp::object &role,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list)
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asCIMObjectPath();

    std::string std_ns(s_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_result_class;
    std::string std_role;
    if (result_class != bp::object())
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (role != bp::object())
        std_role = lmi::extract_or_throw<std::string>(role, "Role");

    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> cim_references;
    try {
        Pegasus::CIMName cim_result_class;
        Pegasus::String  cim_role = Pegasus::String::EMPTY;
        if (!std_result_class.empty())
            cim_result_class = Pegasus::CIMName(std_result_class.c_str());
        if (!std_role.empty())
            cim_role = Pegasus::String(std_role.c_str());

        ScopedMutex sm(m_mutex);
        connectTmp();
        cim_references = m_client.references(
            Pegasus::CIMNamespaceName(std_ns.c_str()),
            cim_path,
            cim_result_class,
            cim_role,
            include_qualifiers,
            include_class_origin,
            cim_property_list);
        disconnectTmp();
    } catch (const Pegasus::CannotConnectException &e) {
        throw_Exception(e);
    } catch (const Pegasus::Exception &e) {
        throw_Exception(e);
    }

    bp::list references;
    const Pegasus::Uint32 cnt = cim_references.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        references.append(CIMInstance::create(cim_references[i]));

    return references;
}
