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
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/tuple.hpp>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include "lmiwbem_make_method.h"
#include "lmiwbem_class.h"
#include "lmiwbem_class_name.h"
#include "lmiwbem_constants.h"
#include "lmiwbem_connection.h"
#include "lmiwbem_convert.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_gil.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_instance_name.h"
#include "lmiwbem_nocasedict.h"
#include "lmiwbem_util.h"
#include "lmiwbem_value.h"

/* NOTE: These macros need to be used around every CIM operation.
 * ScopedConnectionBegin creates a temporary connection, if necessary and also
 * releases Python's global interpret lock. ScopedConnectionEnd is defined due
 * to semantics; to close the scope.
 */
#define ScopedConnectionBegin() { \
    ScopedConnection sc(this); \
    ScopedGILRelease sr;
#define ScopedConnectionEnd() }

namespace bp = boost::python;

WBEMConnection::ScopedConnection::ScopedConnection(WBEMConnection *conn)
    : m_conn(conn)
    , m_conn_orig_state(m_conn->m_client.isConnected())
{
    if (m_conn_orig_state) {
        // We are already connected, nothing to do here.
        return;
    } else if (m_conn->m_connect_locally) {
        m_conn->m_client.connectLocally();
        return;
    } else if (m_conn->m_url.empty()) {
        throw_ValueError("WBEMConnection constructed without url parameter");
    }

    try {
        m_conn->m_client.connect(
            Pegasus::String(m_conn->m_url.c_str()),
            Pegasus::String(m_conn->m_username.c_str()),
            Pegasus::String(m_conn->m_password.c_str()),
            Pegasus::String(m_conn->m_cert_file.c_str()),
            Pegasus::String(m_conn->m_key_file.c_str()),
            Pegasus::String(CIMConstants::defaultTrustStore().c_str()));
    } catch (...) {
        handle_all_exceptions();
    }
}

WBEMConnection::ScopedConnection::~ScopedConnection()
{
    if (!m_conn_orig_state)
        m_conn->m_client.disconnect();
}

WBEMConnection::WBEMConnection(
    const bp::object &url,
    const bp::object &creds,
    const bp::object &x509,
    const bp::object &default_namespace,
    const bp::object &no_verification,
    const bp::object &connect_locally)
    : m_connected_tmp(false)
    , m_connect_locally(false)
    , m_url()
    , m_username()
    , m_password()
    , m_cert_file()
    , m_key_file()
    , m_default_namespace(CIMConstants::defaultNamespace())
    , m_client()
{
    m_connect_locally = lmi::extract_or_throw<bool>(
        connect_locally, "connect_locally");

    // We are constructing with local connection flag; disregard other
    // parameters.
    if (m_connect_locally)
        return;

    if (!isnone(url))
        m_url = lmi::extract_or_throw<std::string>(url, "url");

    if (!isnone(creds))
        setCredentials(creds);

    if (!isnone(x509)) {
        bp::dict cert = lmi::extract_or_throw<bp::dict>(x509, "x509");
        bp::object cert_file = cert["cert_file"];
        bp::object key_file  = cert["key_file"];
        if (!isnone(cert_file)) {
            m_cert_file = lmi::extract_or_throw<std::string>(
                cert["cert_file"], "cert_file");
        }
        if (!isnone(key_file)) {
            m_key_file  = lmi::extract_or_throw<std::string>(
                cert["key_file"], "key_file");
        }
    }

    bool verify = lmi::extract_or_throw<bool>(no_verification,
        "no_verification");
    m_client.setVerifyCertificate(verify);

    if (!isnone(default_namespace)) {
        m_default_namespace = lmi::extract_or_throw<std::string>(
            default_namespace, "default_namespace");
    }

    // Set timeout to 1 minute.
    m_client.setTimeout(60000);
}

WBEMConnection::~WBEMConnection()
{
    m_client.disconnect();
}

void WBEMConnection::init_type()
{
    CIMBase<WBEMConnection>::init_type(
        bp::class_<WBEMConnection, boost::noncopyable>("WBEMConnection", bp::no_init)
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("url") = bp::object(),
                bp::arg("creds") = bp::object(),
                bp::arg("x509") = bp::object(),
                bp::arg("default_namespace") = bp::object(),
                bp::arg("no_verification") = false,
                bp::arg("connect_locally") = false),
                "Constructs :py:class:`.WBEMConnection` object.\n\n"
                ":param str url: String containing URL of CIMOM instance\n"
                ":param tuple creds: tuple containing two string, where the first\n"
                "\tone stands for username, second for password\n"
                ":param dict x509: dictionary containing keys 'cert_file' and 'key_file'\n"
                "\tThe value of 'cert_file' must be string specifying a filename of\n"
                "\tcertificate and the value of 'key_file' must be string specifying\n"
                "\ta filename of private key belonging to the certificate.\n"
                ":param bool no_verification: set to True, if CIMOM's X509 certificate\n"
                "\t shall not be verified; False otherwise. Default value is False.\n"
                ":param bool connect_locally: if True, Unix socket will be\n"
                "\tused. Default value is False.\n"))
        .def("__repr__", &WBEMConnection::repr)
        .def("connect", &WBEMConnection::connect,
            (bp::arg("url") = bp::object(),
             bp::arg("username") = bp::object(),
             bp::arg("password") = bp::object(),
             bp::arg("cert_file") = bp::object(),
             bp::arg("key_file") = bp::object(),
             bp::arg("no_verification") = bp::object()),
            "connect(url=None, username=None, password=None, cert_file=None, "
            "key_file=None, no_verification=None)\n\n"
            "Connects to CIMOM.\n\n"
            ":param str url: String containing url of remote CIMOM.\n"
            ":param str username: String containing username for authentication.\n"
            ":param str password: String containing password for authentication.\n"
            ":param bool no_verification: set to True, if CIMOM's X509 certificate\n"
            "\t shall not be verified; False otherwise. Default value is False.\n"
            ":raises: :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_connection`")
        .def("connectLocally", &WBEMConnection::connectLocally,
            "connectLocally()\n\n"
            "Connect to CIMOM using local Unix socket.\n\n"
            ":raises: :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_connection_socket`")
        .def("disconnect",
            &WBEMConnection::disconnect,
            "disconnect()\n\n"
            "Disconnects from CIMOM.")
        .add_property("is_connected",
            &WBEMConnection::isConnected,
            "Property, which return True, if the client is connected to certain CIMOM.\n\n"
            ":rtype: bool")
        .add_property("hostname",
            &WBEMConnection::getHostname,
            "Property returning CIMOM hostname.\n\n"
            ":rtype: str")
        .add_property("no_verification",
            &WBEMConnection::getVerifyCertificate,
            &WBEMConnection::setVerifyCertificate,
            "Property storing X509 certificate verification flag.\n\n"
            ":rtype: bool")
        .add_property("connect_locally",
            &WBEMConnection::getConnectLocally,
            &WBEMConnection::setConnectLocally,
            "Property storing flag means of connection. If set to True, local Unix socket\n"
            "will be used; HTTP(S) otherwise.\n\n"
            ":rtype: bool")
        .add_property("timeout",
            &WBEMConnection::getTimeout,
            &WBEMConnection::setTimeout,
            "Property storing CIM operations timeout in milliseconds. Default value is 60000ms.\n\n"
            ":rtype: int")
        .add_property("request_accept_languages",
            &WBEMConnection::getRequestAcceptLanguages,
            &WBEMConnection::setRequestAcceptLanguages,
            "Property storing accept languages currently configured for this client.\n\n"
            ":rtype: list of tuples (lang, q)")
        .add_property("default_namespace",
            &WBEMConnection::getDefaultNamespace,
            &WBEMConnection::setDefaultNamespace,
            "Property storing default CIM namespace used for CIM operations.\n\n"
            ":rtype: str")
        .add_property("creds",
            &WBEMConnection::getCredentials,
            &WBEMConnection::setCredentials,
            "Property storing user credentials.\n\n"
            ":rtype: tuple containing username and password")
        .def("CreateInstance", &WBEMConnection::createInstance,
            (bp::arg("NewInstance"),
             bp::arg("ns") = bp::object()),
            "CreateInstance(NewInstance, ns=None)\n\n"
            "Creates a new CIM instance and returns its instance name.\n\n"
            ":param CIMInstance NewInstance: new local :py:class:`.CIMInstance`\n"
            ":param str ns: Namespace in which the :py:class:`.CIMInstance`\n"
            "\twill be created. If `ns` is ``None``, namespace will be used from\n"
            "\t``NewInstance.path.namespace``.\n"
            ":returns: instance name of new CIM instance\n"
            ":rtype: :py:class:`.CIMInstanceName`\n\n"
            "**Example:** :ref:`example_create_instance`")
        .def("DeleteInstance", &WBEMConnection::deleteInstance,
            (bp::arg("InstanceName")),
            "DeleteInstance(InstanceName)\n\n"
            "Deletes a CIM instance identified by :py:class:`.CIMInstanceName`.\n\n"
            ":param CIMInstanceName InstanceName: object path of CIM instance\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_delete_instance`")
        .def("ModifyInstance", &WBEMConnection::modifyInstance,
            (bp::arg("ModifiedInstance"),
             bp::arg("IncludeQualifiers") = true,
             bp::arg("PropertyList") = bp::object()),
            "ModifyInstance(ModifiedInstance, IncludeQualifiers=True, PropertyList=None)\n\n"
            "Modifies properties of a existing instance.\n\n"
            ":param CIMInstance ModifiedInstance: modified instance\n"
            ":param bool IncludeQualifiers: Indicates, if the qualifiers are modified as\n"
            "\tspecified in ModifiedInstance. If the parameter is false, qualifiers in\n"
            "\tModifiedInstance are ignored and no qualifiers are explicitly modified.\n"
            ":param list PropertyList: if present and not None, the members of the list\n"
            "\tdefine one or more property names. The properties specified in PropertyList\n"
            "\tare designated to be modified. Properties of the ModifiedInstance that are\n"
            "\tmissing from PropertyList are not designated to be modified. If PropertyList\n"
            "\tis an empty array, no properties are designated to be modified. If\n"
            "\tPropertyList is None, the properties of ModifiedInstance with values different\n"
            "\tfrom the current values in the instance are designated to be modified.\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_modify_instance`")
        .def("EnumerateInstances", &WBEMConnection::enumerateInstances,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("DeepInheritance") = true,
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "EnumerateInstances(ClassName, namespace=None, LocalOnly=True, "
            "DeepInheritance=True, IncludeQualifiers=False, IncludeClassOrigin=False, "
            "PropertyList=None)\n\n"
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
            "\tLocalOnly is set to True and A is not local to the requested class, it is not\n"
            "\tincluded in the response. If the PropertyList input parameter is an empty\n"
            "\tlist, no properties are included in the response. If the PropertyList input\n"
            "\tparameter is None, no additional filtering is defined. Default value is None.\n"
            ":returns: List of :py:class:`.CIMInstance` objects\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_enumerate_instances`")
        .def("EnumerateInstanceNames", &WBEMConnection::enumerateInstanceNames,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object()),
            "EnumerateInstanceNames(ClassName, namespace=None)\n\n"
            "Enumerates instance names of a given class name.\n\n"
            ":param str ClassName: String containing class name of instance\n"
            "\tnames to be retrieved.\n"
            ":param str namespace: String containing namespace, from which the\n"
            "\tinstance names will be retrieved.\n"
            ":returns: List of :py:class:`.CIMInstanceName` objects\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_enumerate_instance_names`")
        .def("GetInstance", &WBEMConnection::getInstance,
            (bp::arg("InstanceName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "GetInstance(InstanceName, namespace=None, LocalOnly=True, "
            "IncludeQualifiers=False, IncludeClassOrigin=False, PropertyList=None)\n\n"
            "Fetches a :py:class:`.CIMInstance` from CIMOM identified by\n"
            ":py:class:`.CIMInstanceName`.\n\n"
            ":param CIMInstanceName InstanceName: :py:class:`.CIMInstanceName`,\n"
            "\twhich identifies a :py:class:`.CIMInstance`\n"
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
            "\tLocalOnly is set to True and A is not local to the requested class, it is not\n"
            "\tincluded in the response. If the PropertyList input parameter is an empty\n"
            "\tlist, no properties are included in the response. If the PropertyList input\n"
            "\tparameter is None, no additional filtering is defined. Default value is None.\n"
            ":returns: :py:class:`.CIMInstance` object\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_get_instance`")
        .def("EnumerateClasses", &WBEMConnection::enumerateClasses,
            (bp::arg("namespace") = bp::object(),
             bp::arg("ClassName") = bp::object(),
             bp::arg("DeepInheritance") = false,
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = true,
             bp::arg("IncludeClassOrigin") = false),
            "EnumerateClasses(namespace=None, ClassName=None, DeepInheritance=False, "
            "LocalOnly=True, IncludeQualifiers=True, IncludeClassOrigin=False)\n\n"
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
            ":returns: list of :py:class:`.CIMClass` objects\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_enumerate_classes`")
        .def("EnumerateClassNames", &WBEMConnection::enumerateClassNames,
            (bp::arg("namespace") = bp::object(),
             bp::arg("ClassName") = bp::object(),
             bp::arg("DeepInheritance") = false),
            "EnumerateClassNames(namespace=None, ClassName=None, DeepInheritance=False)\n\n"
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
            ":returns: list of strings of class names\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_enumerate_class_names`")
        .def("ExecQuery", &WBEMConnection::execQuery,
            (bp::arg("QueryLanguage"),
             bp::arg("Query"),
             bp::arg("namespace") = bp::object()),
            "ExecQuery(QueryLanguage, Query, namespace=None)\n\n"
            "Executes a query and returns a list of :py:class:`.CIMInstance` objects.\n\n"
            ":param str query_lang: query language\n"
            ":param str query: query to execute\n"
            ":param str namespace: target namespace for the query\n"
            ":returns: list of :py:class:`.CIMInstance` objects\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_exec_query`")
        .def("InvokeMethod", lmi::raw_method<WBEMConnection>(&WBEMConnection::invokeMethod, 1),
            "InvokeMethod(MethodName, ObjectName, **params)\n\n"
            "Executes a method within a given instance.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object within which the method\n"
            "\twill be called\n"
            ":param str MethodName: string containing a method name\n"
            ":param CIMInstanceName ObjectName: object path\n"
            ":param dictionary params: parameters passed to the method call\n"
            ":returns: tuple containing method's return value and output parameters\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_invoke_method`")
        .def("GetClass", &WBEMConnection::getClass,
            (bp::arg("ClassName"),
             bp::arg("namespace") = bp::object(),
             bp::arg("LocalOnly") = true,
             bp::arg("IncludeQualifiers") = true,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "GetClass(ClassName, namespace=None, LocalOnly=True, IncludeQualifiers=True, "
            "IncludeClassOrigin=False, PropertyList=None)\n\n"
            "Returns a :py:class:`.CIMClass` representing the named class.\n\n"
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
            ":returns: :py:class:`.CIMClass` object\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_get_class`")
        .def("Associators", &WBEMConnection::getAssociators,
            (bp::arg("ObjectName"),
             bp::arg("AssocClass") = bp::object(),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("ResultRole") = bp::object(),
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "Associators(ObjectName, AssocClass=None, ResultClass=None, Role=None, "
            "ResultRole=None, IncludeQualifiers=False, IncludeClassOrigin=False, "
            "PropertyList=None)\n\n"
            "Returns a list of associated :py:class:`.CIMInstance` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the associated\n"
            "\tinstances will be enumerated.\n"
            ":param str AssocClass: valid CIM association class name. It acts as a filter on\n"
            "\tthe returned set of names by mandating that each returned name identify an\n"
            "\tobject that shall be associated to the source object through an instance of\n"
            "\tthis class or one of its subclasses.\n"
            ":param str ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of names by mandating that each returned name identify an object\n"
            "\tthat shall be either an instance of this class (or one of its subclasses) or\n"
            "\tbe this class (or one of its subclasses).\n"
            ":param str Role: valid property name. It acts as a filter on the returned set\n"
            "\tof names by mandating that each returned name identify an object that shall be\n"
            "\tassociated to the source object through an association in which the source\n"
            "\tobject plays the specified role. That is, the name of the property in the\n"
            "\tassociation class that refers to the source object shall match the value of\n"
            "\tthis parameter.\n"
            ":param str ResultRole: valid property name. It acts as a filter on the returned\n"
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
            ":returns: list of associated :py:class:`.CIMInstance` objects with an input\n"
            "\tinstance\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_associators`")
        .def("AssociatorNames", &WBEMConnection::getAssociatorNames,
            (bp::arg("ObjectName"),
             bp::arg("AssocClass") = bp::object(),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("ResultRole") = bp::object()),
            "AssociatorNames(ObjectName, AssocClass=None, ResultClass=None, Role=None, "
            "ResultRole=None)\n\n"
            "Returns a list of associated :py:class:`.CIMInstanceName` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the associated\n"
            "\tinstance names will be enumerated.\n"
            ":param str AssocClass: valid CIM association class name. It acts as a filter on\n"
            "\tthe returned set of names by mandating that each returned name identify an\n"
            "\tobject that shall be associated to the source object through an instance of\n"
            "\tthis class or one of its subclasses.\n"
            ":param str ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of names by mandating that each returned name identify an object\n"
            "\tthat shall be either an instance of this class (or one of its subclasses) or\n"
            "\tbe this class (or one of its subclasses).\n"
            ":param str Role: valid property name. It acts as a filter on the returned set\n"
            "\tof names by mandating that each returned name identify an object that shall be\n"
            "\tassociated to the source object through an association in which the source\n"
            "\tobject plays the specified role. That is, the name of the property in the\n"
            "\tassociation class that refers to the source object shall match the value of\n"
            "\tthis parameter.\n"
            ":param str ResultRole: valid property name. It acts as a filter on the returned\n"
            "\tset of names by mandating that each returned name identify an object that\n"
            "\tshall be associated to the source object through an association in which the\n"
            "\tnamed returned object plays the specified role. That is, the name of the\n"
            "\tproperty in the association class that refers to the returned object shall\n"
            "\tmatch the value of this parameter.\n"
            ":returns: list of associated :py:class:`.CIMInstanceName` objects with\n"
            "\tan input instance\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_associator_names`")
        .def("References", &WBEMConnection::getReferences,
            (bp::arg("ObjectName"),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object(),
             bp::arg("IncludeQualifiers") = false,
             bp::arg("IncludeClassOrigin") = false,
             bp::arg("PropertyList") = bp::object()),
            "References(ObjectName, ResultClass=None, Role=None, IncludeQualifiers=False, "
            "IncludeClassOrigin=False, PropertyList=None)\n\n"
            "Returns a list of association :py:class:`.CIMInstance` objects with an input\n"
            "instance name.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the association\n"
            "\tinstances will be enumerated.\n"
            ":param str ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of objects by mandating that each returned object shall be an\n"
            "\tinstance of this class (or one of its subclasses) or this class (or one of its\n"
            "\tsubclasses). Default value is None.\n"
            ":param str Role: valid property name. It acts as a filter on the returned set\n"
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
            ":returns: list of association :py:class:`.CIMInstance` objects with an input\n"
            "\tinstance\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_references`")
        .def("ReferenceNames", &WBEMConnection::getReferenceNames,
            (bp::arg("ObjectName"),
             bp::arg("ResultClass") = bp::object(),
             bp::arg("Role") = bp::object()),
            "ReferenceNames(ObjectName, ResultClass=None, Role=None)\n\n"
            "Returns a list of association :py:class:`.CIMInstanceName` objects with an\n"
            "input instance.\n\n"
            ":param CIMInstanceName ObjectName: specifies CIM object for which the association\n"
            "\tinstance names will be enumerated.\n"
            ":param str ResultClass: valid CIM class name. It acts as a filter on the\n"
            "\treturned set of object names by mandating that each returned Object Name identify\n"
            "\tan instance of this class (or one of its subclasses) or this class (or one of its\n"
            "\tsubclasses).\n"
            ":param str Role: valid property name. It acts as a filter on the returned set\n"
            "\tof object names by mandating that each returned object name shall identify an\n"
            "\tobject that refers to the target instance through a property with a name that\n"
            "\tmatches the value of this parameter.\n"
            ":returns: list of association :py:class:`.CIMInstanceName` objects with an input\n"
            "\tinstance\n"
            ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`\n\n"
            "**Example:** :ref:`example_reference_names`"));
}

std::string WBEMConnection::repr() const
{
    std::stringstream ss;
    ss << "WBEMConnection(url='" << m_url << "', ...)";
    return ss.str();
}

void WBEMConnection::connect(
    const bp::object &url,
    const bp::object &username,
    const bp::object &password,
    const bp::object &cert_file,
    const bp::object &key_file,
    const bp::object &no_verification)
{
    // If we have local connection flag set, disregard other parameters and
    // perform socket connection.
    if (m_connect_locally) {
        connectLocally();
        return;
    }

    // All three parameters can be omitted, WBEMConnection was has been
    // with such parameters; WBEMConnection(url, (username, password)).
    std::string std_url(m_url);
    std::string std_cert_file(m_cert_file);
    std::string std_key_file(m_key_file);

    // Check, if provided parameters are None. If not, try to get a string value
    // out of them.
    if (!isnone(url))
        std_url = lmi::extract_or_throw<std::string>(url, "url");
    if (!isnone(username))
        m_username = lmi::extract_or_throw<std::string>(username, "username");
    if (!isnone(password))
        m_password = lmi::extract_or_throw<std::string>(password, "password");
    if (!isnone(cert_file))
        std_cert_file = lmi::extract_or_throw<std::string>(cert_file, "cert_file");
    if (!isnone(key_file))
        std_key_file = lmi::extract_or_throw<std::string>(key_file, "key_file");

    // Check, if we have any url to connect to
    if (std_url.empty())
        throw_ValueError("url parameter missing");

    if (!isnone(no_verification)) {
        bool no_verify = lmi::extract_or_throw<bool>(no_verification, "no_verification");
        m_client.setVerifyCertificate(!no_verify);
    }

    try {
        m_client.connect(
            Pegasus::String(std_url.c_str()),
            Pegasus::String(m_username.c_str()),
            Pegasus::String(m_password.c_str()),
            Pegasus::String(std_cert_file.c_str()),
            Pegasus::String(std_key_file.c_str()),
            Pegasus::String(CIMConstants::defaultTrustStore().c_str()));
        m_connect_locally = false;
    } catch (...) {
        handle_all_exceptions();
    }
}

void WBEMConnection::connectLocally() try
{
    m_client.connectLocally();
    m_connect_locally = true;
} catch (...) {
    handle_all_exceptions();
}

void WBEMConnection::disconnect()
{
    m_client.disconnect();
}

bp::object WBEMConnection::getRequestAcceptLanguages() const
{
    Pegasus::AcceptLanguageList al_list = m_client.getRequestAcceptLanguages();
    Pegasus::Uint32 cnt = al_list.size();
    bp::list languages;
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        languages.append(bp::make_tuple(
            al_list.getLanguageTag(i).toString(),
            al_list.getQualityValue(i)));
    }

    return languages;
}

void WBEMConnection::setRequestAcceptLanguages(const bp::object &languages)
{
    bp::list lang_list(lmi::get_or_throw<bp::list>(
        languages, "request_accept_languages"));
    const int cnt = bp::len(lang_list);
    Pegasus::AcceptLanguageList al_list;
    for (int i = 0; i < cnt; ++i) {
        bp::tuple lang_elem = lmi::extract_or_throw<bp::tuple>(
            lang_list[i], "request_accept_languages[i]");
        std::string lang = lmi::extract_or_throw<std::string>(lang_elem[0]);
        Pegasus::Real32 q = lmi::extract_or_throw<Pegasus::Real32>(lang_elem[1]);
        al_list.insert(Pegasus::LanguageTag(lang.c_str()), q);
    }

    m_client.setRequestAcceptLanguages(al_list);
}

void WBEMConnection::setDefaultNamespace(const bp::object &ns)
{
    m_default_namespace = lmi::extract_or_throw<std::string>(ns, "default_namespace");
}

bp::object WBEMConnection::getCredentials() const
{
    return bp::make_tuple(m_username, m_password);
}

void WBEMConnection::setCredentials(const bp::object &creds)
{
    bp::tuple creds_tpl(lmi::get_or_throw<bp::tuple>(creds, "creds"));

    if (bp::len(creds_tpl) != 2)
        throw_ValueError("creds must be tuple of 2 strings");

    m_username = lmi::extract_or_throw<std::string>(creds_tpl[0], "username");
    m_password = lmi::extract_or_throw<std::string>(creds_tpl[1], "password");
}

bp::object WBEMConnection::createInstance(
    const bp::object &instance,
    const bp::object &ns) try
{
    CIMInstance &inst = lmi::extract_or_throw<CIMInstance&>(
        instance, "NewInstance");

    std::string std_ns(m_default_namespace);

    if (!isnone(inst.getPyPath())) {
        // First, try to get creation namespace from CIMInstanceName.
        CIMInstanceName &inst_name = lmi::extract<CIMInstanceName&>(inst.getPyPath());
        std_ns = inst_name.getNamespace();
    }
    if (!isnone(ns)) {
        // Override CIMInstanceName's namespace.
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");
    }

    Pegasus::CIMObjectPath new_inst_name;
    Pegasus::CIMNamespaceName new_inst_name_ns(std_ns.c_str());
    Pegasus::CIMInstance cim_inst = inst.asPegasusCIMInstance();

    ScopedConnectionBegin();
    new_inst_name = m_client.createInstance(
        new_inst_name_ns,
        cim_inst);
    ScopedConnectionEnd();

    // CIMClient::createInstance() does not set namespace and hostname
    // in newly created CIMInstanceName. We need to do that manually.
    new_inst_name.setNameSpace(
        Pegasus::CIMNamespaceName(std_ns.c_str()));
    new_inst_name.setHost(m_client.hostname().c_str());

    return CIMInstanceName::create(new_inst_name);
} catch (...) {
    handle_all_exceptions();
    return None;
}

void WBEMConnection::deleteInstance(const bp::object &object_path) try
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName&>(
        object_path, "InstanceName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());

    ScopedConnectionBegin()
    m_client.deleteInstance(
        cim_ns,
        cim_path);
    ScopedConnectionEnd();
} catch (...) {
    handle_all_exceptions();
}

void WBEMConnection::modifyInstance(
    const bp::object &instance,
    const bool include_qualifiers,
    const bp::object &property_list) try
{
    CIMInstance &inst = lmi::extract_or_throw<CIMInstance&>(
        instance, "ModifiedInstance");
    CIMInstanceName &inst_name = lmi::extract<CIMInstanceName&>(
        inst.getPyPath());

    Pegasus::CIMNamespaceName cim_ns(inst_name.getNamespace().c_str());
    Pegasus::CIMInstance cim_inst = inst.asPegasusCIMInstance();
    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    ScopedConnectionBegin();
    m_client.modifyInstance(
        cim_ns,
        cim_inst,
        include_qualifiers,
        cim_property_list);
    ScopedConnectionEnd();
} catch (...) {
    handle_all_exceptions();
}

bp::object WBEMConnection::enumerateInstances(
    const bp::object &cls,
    const bp::object &ns,
    const bool local_only,
    const bool deep_inheritance,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list) try
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::Array<Pegasus::CIMInstance> cim_instances;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_name(std_cls.c_str());
    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    ScopedConnectionBegin();
    cim_instances = m_client.enumerateInstances(
        cim_ns,
        cim_name,
        deep_inheritance,
        local_only,
        include_qualifiers,
        include_class_origin,
        cim_property_list);
    ScopedConnectionEnd();

    bp::list instances;
    const Pegasus::Uint32 cnt = cim_instances.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        Pegasus::CIMInstance &cim_instance = cim_instances[i];

        // XXX: Update Pegasus::CIMInstance namespace by setting a new
        // Pegasus::CIMObjectPath to the instance with updated namespace.
        // This needs to be done, because CIMClient doesn't set namespace
        // property in Pegasus::CIMObjectPath.
        CIMInstance::updatePegasusCIMInstanceNamespace(cim_instance, std_ns);

        bp::object instance = CIMInstance::create(cim_instance);
        instances.append(instance);
    }

    return instances;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::enumerateInstanceNames(
    const bp::object &cls,
    const bp::object &ns) try
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::Array<Pegasus::CIMObjectPath> cim_instance_names;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_name(std_cls.c_str());

    ScopedConnectionBegin();
    cim_instance_names = m_client.enumerateInstanceNames(
        cim_ns,
        cim_name);
    ScopedConnectionEnd();

    bp::list instance_names;
    const Pegasus::Uint32 cnt = cim_instance_names.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        bp::object instance_name = CIMInstanceName::create(
            cim_instance_names[i], std_ns, m_client.hostname());
        instance_names.append(instance_name);
    }

    return instance_names;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::invokeMethod(
    const bp::tuple &args,
    const bp::dict  &kwds) try
{
    if (bp::len(args) != 2)
        throw_TypeError("InvokeMethod() takes at least 2 arguments");

    std::string std_method = lmi::extract_or_throw<std::string>(args[0], "MethodName");

    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName&>(
        args[1], "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    Pegasus::CIMValue cim_rval;
    Pegasus::Array<Pegasus::CIMParamValue> cim_out_params;
    Pegasus::Array<Pegasus::CIMParamValue> cim_in_params;

    // Create Pegasus::Array from **kwargs
    bp::list keys = kwds.keys();
    const int keys_cnt = bp::len(keys);
    for (int i = 0; i < keys_cnt; ++i) {
        std::string param_name = lmi::extract_or_throw<std::string>(keys[i]);
        cim_in_params.append(
            Pegasus::CIMParamValue(
                Pegasus::String(param_name.c_str()),
                CIMValue::asPegasusCIMValue(kwds[keys[i]]),
                true /* isTyped */));
    }

    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_name(std_method.c_str());

    ScopedConnectionBegin();
    cim_rval = m_client.invokeMethod(
        cim_ns,
        cim_path,
        cim_name,
        cim_in_params,
        cim_out_params);
    ScopedConnectionEnd();

    // Create a NocaseDict of method's return parameters
    bp::object rparams = NocaseDict::create();
    const Pegasus::Uint32 cnt = cim_out_params.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        rparams[bp::object(cim_out_params[i].getParameterName())] =
            CIMValue::asLMIWbemCIMValue(cim_out_params[i].getValue());
    }

    return bp::make_tuple(
        CIMValue::asLMIWbemCIMValue(cim_rval),
        rparams);
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getInstance(
    const bp::object &instance_name,
    const bp::object &ns,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list) try
{
    CIMInstanceName &cim_instance_name = lmi::extract_or_throw<CIMInstanceName&>(
        instance_name, "InstanceName");
    std::string std_ns(m_default_namespace);
    if (!cim_instance_name.getNamespace().empty())
        std_ns = cim_instance_name.getNamespace();
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMInstance cim_instance;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMObjectPath cim_object_path = cim_instance_name.asPegasusCIMObjectPath();
    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list, "PropertyList"));

    ScopedConnectionBegin();
    cim_instance = m_client.getInstance(
        cim_ns,
        cim_object_path,
        local_only,
        include_qualifiers,
        include_class_origin,
        cim_property_list);
    ScopedConnectionEnd();

    // CIMClient::getInstance() does not set the CIMObjectPath member in
    // CIMInstance. We need to do that manually.
    cim_instance.setPath(cim_object_path);

    return CIMInstance::create(cim_instance);
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::enumerateClasses(
    const bp::object &ns,
    const bp::object &cls,
    const bool deep_inheritance,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin) try
{
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMName classname;
    if (!isnone(cls)) {
        std::string std_cls(lmi::extract_or_throw<std::string>(cls, "ClassName"));
        classname = Pegasus::CIMName(std_cls.c_str());
    }

    Pegasus::Array<Pegasus::CIMClass> cim_classes;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());

    ScopedConnectionBegin();
    cim_classes = m_client.enumerateClasses(
        cim_ns,
        classname,
        deep_inheritance,
        local_only,
        include_qualifiers,
        include_class_origin);
    ScopedConnectionEnd();

    bp::list classes;
    const Pegasus::Uint32 cnt = cim_classes.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        classes.append(CIMClass::create(cim_classes[i]));

    return classes;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::enumerateClassNames(
    const bp::object &ns,
    const bp::object &cls,
    const bool deep_inheritance) try
{
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMName classname;
    if (!isnone(cls)) {
        std::string std_cls(lmi::extract_or_throw<std::string>(cls, "cls"));
        classname = Pegasus::CIMName(std_cls.c_str());
    }

    Pegasus::Array<Pegasus::CIMName> cim_classnames;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());

    ScopedConnectionBegin();
    cim_classnames = m_client.enumerateClassNames(
        cim_ns,
        classname,
        deep_inheritance);
    ScopedConnectionEnd();

    // We do not create lmiwbem.CIMClassName objects here; we try to mimic pywbem.
    bp::list classnames;
    const Pegasus::Uint32 cnt = cim_classnames.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        classnames.append(bp::object(cim_classnames[i]));

    return classnames;
} catch (...) {
    handle_all_exceptions();
    return None;
}


bp::object WBEMConnection::execQuery(
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &ns) try
{
    std::string std_query_lang = lmi::extract_or_throw<std::string>(query_lang, "QueryLanguage");
    std::string std_query = lmi::extract_or_throw<std::string>(query, "Query");
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::Array<Pegasus::CIMObject> cim_instances;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::String cim_query_lang(std_query_lang.c_str());
    Pegasus::String cim_query(std_query.c_str());

    ScopedConnectionBegin();
    cim_instances = m_client.execQuery(
        cim_ns,
        cim_query_lang,
        cim_query);
    ScopedConnectionEnd();

    bp::list instances;
    const Pegasus::Uint32 cnt = cim_instances.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        instances.append(CIMInstance::create(cim_instances[i]));

    return instances;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getClass(
    const bp::object &cls,
    const bp::object &ns,
    const bool local_only,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list) try
{
    std::string std_cls(lmi::extract_or_throw<std::string>(cls, "ClassName"));
    std::string std_ns(m_default_namespace);
    if (!isnone(ns))
        std_ns = lmi::extract_or_throw<std::string>(ns, "namespace");

    Pegasus::CIMClass cim_class;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_name(std_cls.c_str());
    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    ScopedConnectionBegin()
    cim_class = m_client.getClass(
        cim_ns,
        cim_name,
        local_only,
        include_qualifiers,
        include_class_origin,
        cim_property_list);
    ScopedConnectionEnd();

    return CIMClass::create(cim_class);
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getAssociators(
    const bp::object &object_path,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object property_list) try
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_assoc_class;
    std::string std_result_class;
    std::string std_role;
    std::string std_result_role;
    if (!isnone(assoc_class))
        std_assoc_class = lmi::extract_or_throw<std::string>(assoc_class, "AssocClass");
    if (!isnone(result_class))
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (!isnone(role))
        std_role = lmi::extract_or_throw<std::string>(role, "Role");
    if (!isnone(result_role))
        std_result_role = lmi::extract_or_throw<std::string>(result_role, "ResultRole");

    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> cim_associators;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
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

    ScopedConnectionBegin();
    cim_associators = m_client.associators(
        cim_ns,
        cim_path,
        cim_assoc_class,
        cim_result_class,
        cim_role,
        cim_result_role,
        include_qualifiers,
        include_class_origin,
        cim_property_list);
    ScopedConnectionEnd();

    bp::list associators;
    const Pegasus::Uint32 cnt = cim_associators.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        associators.append(CIMInstance::create(cim_associators[i]));

    return associators;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getAssociatorNames(
    const bp::object &object_path,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role) try
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_assoc_class;
    std::string std_result_class;
    std::string std_role;
    std::string std_result_role;
    if (!isnone(assoc_class))
        std_assoc_class = lmi::extract_or_throw<std::string>(assoc_class, "AssocClass");
    if (!isnone(result_class))
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (!isnone(role))
        std_role = lmi::extract_or_throw<std::string>(role, "Role");
    if (!isnone(result_role))
        std_result_role = lmi::extract_or_throw<std::string>(result_role, "ResultRole");

    Pegasus::Array<Pegasus::CIMObjectPath> cim_associator_names;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
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

    ScopedConnectionBegin();
    cim_associator_names = m_client.associatorNames(
        cim_ns,
        cim_path,
        cim_assoc_class,
        cim_result_class,
        cim_role,
        cim_result_role);
    ScopedConnectionEnd();

    bp::list associator_names;
    const Pegasus::Uint32 cnt = cim_associator_names.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        associator_names.append(CIMInstanceName::create(cim_associator_names[i]));

    return associator_names;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getReferences(
    const bp::object &object_path,
    const bp::object &result_class,
    const bp::object &role,
    const bool include_qualifiers,
    const bool include_class_origin,
    const bp::object &property_list) try
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_result_class;
    std::string std_role;
    if (!isnone(result_class))
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (!isnone(role))
        std_role = lmi::extract_or_throw<std::string>(role, "Role");

    Pegasus::CIMPropertyList cim_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> cim_references;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_result_class;
    Pegasus::String  cim_role = Pegasus::String::EMPTY;
    if (!std_result_class.empty())
        cim_result_class = Pegasus::CIMName(std_result_class.c_str());
    if (!std_role.empty())
        cim_role = Pegasus::String(std_role.c_str());

    ScopedConnectionBegin();
    cim_references = m_client.references(
        cim_ns,
        cim_path,
        cim_result_class,
        cim_role,
        include_qualifiers,
        include_class_origin,
        cim_property_list);
    ScopedConnectionEnd();

    bp::list references;
    const Pegasus::Uint32 cnt = cim_references.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        references.append(CIMInstance::create(cim_references[i]));

    return references;
} catch (...) {
    handle_all_exceptions();
    return None;
}

bp::object WBEMConnection::getReferenceNames(
    const bp::object &object_path,
    const bp::object &result_class,
    const bp::object &role) try
{
    const CIMInstanceName &inst_name = lmi::extract_or_throw<CIMInstanceName>(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath cim_path = inst_name.asPegasusCIMObjectPath();

    std::string std_ns(m_default_namespace);
    if (!cim_path.getNameSpace().isNull())
        std_ns = cim_path.getNameSpace().getString().getCString();

    std::string std_result_class;
    std::string std_role;
    if (!isnone(result_class))
        std_result_class = lmi::extract_or_throw<std::string>(result_class, "ResultClass");
    if (!isnone(role))
        std_role = lmi::extract_or_throw<std::string>(role, "Role");

    Pegasus::Array<Pegasus::CIMObjectPath> cim_reference_names;
    Pegasus::CIMNamespaceName cim_ns(std_ns.c_str());
    Pegasus::CIMName cim_result_class;
    Pegasus::String  cim_role = Pegasus::String::EMPTY;
    if (!std_result_class.empty())
        cim_result_class = Pegasus::CIMName(std_result_class.c_str());
    if (!std_role.empty())
        cim_role = Pegasus::String(std_role.c_str());

    ScopedConnectionBegin();
    cim_reference_names = m_client.referenceNames(
        cim_ns,
        cim_path,
        cim_result_class,
        cim_role);
    ScopedConnectionEnd();

    bp::list reference_names;
    const Pegasus::Uint32 cnt = cim_reference_names.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        reference_names.append(CIMInstanceName::create(cim_reference_names[i]));

    return reference_names;
} catch (...) {
    handle_all_exceptions();
    return None;
}
