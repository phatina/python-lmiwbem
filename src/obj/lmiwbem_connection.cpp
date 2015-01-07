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
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/raw_function.hpp>
#include <boost/python/tuple.hpp>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#include "lmiwbem_exception.h"
#include "lmiwbem_make_method.h"
#include "obj/lmiwbem_config.h"
#include "obj/lmiwbem_connection.h"
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_class.h"
#include "obj/cim/lmiwbem_class_name.h"
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

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
    } else if (not m_conn->m_client.getURLInfo().isValid()) {
        throw_ValueError("WBEMConnection constructed with invalid url parameter");
    }

    try {
        m_conn->m_client.connect(
            m_conn->m_client.getUrl(),
            m_conn->m_username,
            m_conn->m_password,
            m_conn->m_cert_file,
            m_conn->m_key_file,
            Config::defaultTrustStore());
    } catch (...) {
        std::stringstream ss;
        if (Config::isVerbose()) {
            bool connect_locally = m_conn->m_connect_locally;
            if (connect_locally)
                ss << "connect_locally(";
            else
                ss << "connect(";

            if (Config::isVerboseMore() && !connect_locally) {
                ss << "url='" << m_conn->m_client.getURLInfo().asString() << '\'';
            }
            ss << ')';
        }
        handle_all_exceptions(ss);
    }
}

WBEMConnection::ScopedConnection::~ScopedConnection()
{
    if (!m_conn_orig_state)
        m_conn->m_client.disconnect();
}

WBEMConnection::ScopedTransaction::ScopedTransaction(WBEMConnection *conn)
    : m_sct(conn->m_client)
{
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
    , m_username()
    , m_password()
    , m_cert_file()
    , m_key_file()
    , m_default_namespace(Config::defaultNamespace())
    , m_client()
{
    m_connect_locally = Conv::as<bool>(connect_locally, "connect_locally");

    // We are constructing with local connection flag; disregard other
    // parameters.
    if (m_connect_locally)
        return;

    if (!isnone(url)) {
        String c_url(StringConv::asString(url, "url"));
        if (!m_client.setUrl(c_url)) {
            std::stringstream ss;
            ss << "Invalid URL: '" << c_url << '\'';
            throw_ValueError(ss.str());
        }
    }

    if (!isnone(creds))
        setCredentials(creds);

    if (!isnone(x509)) {
        bp::dict py_cert = Conv::as<bp::dict>(x509, "x509");
        bp::object py_cert_file = py_cert["cert_file"];
        bp::object py_key_file  = py_cert["key_file"];
        if (!isnone(py_cert_file)) {
            m_cert_file = StringConv::asString(
                py_cert["cert_file"], "cert_file");
        }
        if (!isnone(py_key_file)) {
            m_key_file  = StringConv::asString(
                py_cert["key_file"], "key_file");
        }
    }

    bool c_verify = Conv::as<bool>(no_verification, "no_verification");
    m_client.setVerifyCertificate(c_verify);

    if (!isnone(default_namespace)) {
        m_default_namespace = StringConv::asString(
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
    WBEMConnection::WBEMConnectionClass cls("WBEMConnection", bp::no_init);

    init_type_base(cls);
#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    init_type_pull(cls);
#endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

    CIMBase<WBEMConnection>::init_type(cls);
}

void WBEMConnection::init_type_base(WBEMConnection::WBEMConnectionClass &cls)
{
    cls.def(bp::init<
        const bp::object &,
        const bp::object &,
        const bp::object &,
        const bp::object &,
        const bp::object &,
        const bp::object &>((
            bp::arg("url") = None,
            bp::arg("creds") = None,
            bp::arg("x509") = None,
            bp::arg("default_namespace") = None,
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
        (bp::arg("url") = None,
         bp::arg("username") = None,
         bp::arg("password") = None,
         bp::arg("cert_file") = None,
         bp::arg("key_file") = None,
         bp::arg("no_verification") = None),
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
    .add_property("url",
        &WBEMConnection::getPyUrl,
        "Property returning connection URL.\n\n"
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
         bp::arg("ns") = None),
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
         bp::arg("PropertyList") = None),
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
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("DeepInheritance") = true,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
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
         bp::arg("namespace") = None),
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
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
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
        (bp::arg("namespace") = None,
         bp::arg("ClassName") = None,
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
        (bp::arg("namespace") = None,
         bp::arg("ClassName") = None,
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
         bp::arg("namespace") = None),
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
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("IncludeQualifiers") = true,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
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
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
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
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None),
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
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
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
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None),
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
        "**Example:** :ref:`example_reference_names`");
}

String WBEMConnection::repr() const
{
    std::stringstream ss;
    ss << "WBEMConnection(url=u'" << m_client.getUrl() << "', ...)";
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
    String c_url(m_client.getUrl());
    String c_cert_file(m_cert_file);
    String c_key_file(m_key_file);

    // Check, if provided parameters are None. If not, try to get a string value
    // out of them.
    if (!isnone(url))
        c_url = StringConv::asString(url, "url");
    if (!isnone(username))
        m_username = StringConv::asString(username, "username");
    if (!isnone(password))
        m_password = StringConv::asString(password, "password");
    if (!isnone(cert_file))
        c_cert_file = StringConv::asString(cert_file, "cert_file");
    if (!isnone(key_file))
        c_key_file = StringConv::asString(key_file, "key_file");

    // Check, if we have any url to connect to
    if (c_url.empty())
        throw_ValueError("url parameter missing");

    if (!isnone(no_verification)) {
        bool c_no_verify = Conv::as<bool>(no_verification, "no_verification");
        m_client.setVerifyCertificate(!c_no_verify);
    }

    try {
        m_client.connect(
            c_url,
            m_username,
            m_password,
            c_cert_file,
            c_key_file,
            Config::defaultTrustStore());
        m_connect_locally = false;
    } catch (...) {
        std::stringstream ss;
        if (Config::isVerbose()) {
            ss << "connect(";
            if (Config::isVerboseMore())
                ss << "url='" << m_client.getURLInfo().asString() << '\'';
            ss << ')';
        }
        handle_all_exceptions(ss);
    }
}

void WBEMConnection::connectLocally() try
{
    m_client.connectLocally();
    m_connect_locally = true;
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose())
        ss << "connect_locally()";
    handle_all_exceptions(ss);
}

void WBEMConnection::disconnect()
{
    m_client.disconnect();
}

bool WBEMConnection::isConnected() const
{
    return m_client.isConnected();
}

bp::object WBEMConnection::getHostname() const
{
    return StringConv::asPyUnicode(m_client.getHostname());
}

bp::object WBEMConnection::getPyUrl() const
{
    return StringConv::asPyUnicode(m_client.getUrl());
}

bool WBEMConnection::getVerifyCertificate() const
{
    return !m_client.getVerifyCertificate();
}

void WBEMConnection::setVerifyCertificate(bool verify_cert)
{
    m_client.setVerifyCertificate(!verify_cert);
}

bool WBEMConnection::getConnectLocally() const
{
    return m_connect_locally;
}

void WBEMConnection::setConnectLocally(bool connect_locally)
{
    m_connect_locally = connect_locally;
}

unsigned int WBEMConnection::getTimeout() const
{
    return m_client.getTimeout();
}

void WBEMConnection::setTimeout(unsigned int timeout)
{
    m_client.setTimeout(timeout);
}

bp::object WBEMConnection::getRequestAcceptLanguages() const
{
    Pegasus::AcceptLanguageList peg_al_list = m_client.getRequestAcceptLanguages();
    Pegasus::Uint32 cnt = peg_al_list.size();
    bp::list py_languages;

    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        py_languages.append(bp::make_tuple(
            peg_al_list.getLanguageTag(i).toString(),
            peg_al_list.getQualityValue(i)));
    }

    return py_languages;
}

void WBEMConnection::setRequestAcceptLanguages(const bp::object &languages)
{
    bp::list py_lang_list(Conv::get<bp::list>(
        languages, "request_accept_languages"));
    const int cnt = bp::len(py_lang_list);
    Pegasus::AcceptLanguageList peg_al_list;

    for (int i = 0; i < cnt; ++i) {
        bp::tuple py_lang_elem(Conv::get<bp::tuple>(
            py_lang_list[i], "request_accept_languages[i]"));
        String c_lang(StringConv::asString(py_lang_elem[0]));
        Pegasus::Real32 q = Conv::as<Pegasus::Real32>(py_lang_elem[1]);
        peg_al_list.insert(Pegasus::LanguageTag(c_lang), q);
    }

    m_client.setRequestAcceptLanguages(peg_al_list);
}

bp::object WBEMConnection::getDefaultNamespace() const
{
    return StringConv::asPyUnicode(m_default_namespace);
}

void WBEMConnection::setDefaultNamespace(const bp::object &ns)
{
    m_default_namespace = StringConv::asString(ns, "default_namespace");
}

bp::object WBEMConnection::getCredentials() const
{
    return bp::make_tuple(
        StringConv::asPyUnicode(m_username),
        StringConv::asPyUnicode(m_password));
}

void WBEMConnection::setCredentials(const bp::object &creds)
{
    bp::tuple py_creds_tpl(Conv::get<bp::tuple>(creds, "creds"));

    if (bp::len(py_creds_tpl) != 2)
        throw_ValueError("creds must be tuple of 2 strings");

    m_username = StringConv::asString(py_creds_tpl[0], "username");
    m_password = StringConv::asString(py_creds_tpl[1], "password");
}

bp::object WBEMConnection::createInstance(
    const bp::object &instance,
    const bp::object &ns) try
{
    CIMInstance &cim_inst = CIMInstance::asNative(instance, "NewInstance");

    String c_ns(m_default_namespace);

    if (!isnone(cim_inst.getPyPath())) {
        // First, try to get creation namespace from CIMInstanceName.
        CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(cim_inst.getPyPath());
        c_ns = cim_inst_name.getNamespace();
    }
    if (!isnone(ns)) {
        // Override CIMInstanceName's namespace.
        c_ns = StringConv::asString(ns, "namespace");
    }

    Pegasus::CIMObjectPath peg_new_inst_name;
    Pegasus::CIMNamespaceName peg_new_inst_name_ns(c_ns);
    Pegasus::CIMInstance peg_inst = cim_inst.asPegasusCIMInstance();

    ScopedTransactionBegin();
    peg_new_inst_name = m_client.createInstance(
        peg_new_inst_name_ns,
        peg_inst);
    ScopedTransactionEnd();

    // CIMClient::createInstance() does not set namespace and hostname
    // in newly created CIMInstanceName. We need to do that manually.
    peg_new_inst_name.setNameSpace(Pegasus::CIMNamespaceName(c_ns));
    peg_new_inst_name.setHost(m_client.getHostname());

    return CIMInstanceName::create(peg_new_inst_name);
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "CreateInstance(";
        if (Config::isVerboseMore()) {
            CIMInstance &inst = CIMInstance::asNative(instance);
            ss << '\'' << inst.getPath().asString() << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

void WBEMConnection::deleteInstance(const bp::object &object_path) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "InstanceName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();
    Pegasus::CIMNamespaceName peg_ns(c_ns);

    ScopedTransactionBegin()
    m_client.deleteInstance(
        peg_ns,
        peg_path);
    ScopedTransactionEnd();
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "DeleteInstance(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
}

void WBEMConnection::modifyInstance(
    const bp::object &instance,
    const bool include_qualifiers,
    const bp::object &property_list) try
{
    CIMInstance &cim_inst = CIMInstance::asNative(
        instance, "ModifiedInstance");
    CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        cim_inst.getPyPath());

    Pegasus::CIMNamespaceName peg_ns(cim_inst_name.getNamespace());
    Pegasus::CIMInstance peg_inst = cim_inst.asPegasusCIMInstance();
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(
            property_list, "PropertyList"));

    ScopedTransactionBegin();
    m_client.modifyInstance(
        peg_ns,
        peg_inst,
        include_qualifiers,
        peg_property_list);
    ScopedTransactionEnd();
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "ModifyInstance(";
        if (Config::isVerboseMore()) {
            CIMInstance &cim_inst = CIMInstance::asNative(instance);
            ss << '\'' << cim_inst.getPath().asString() << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    String c_cls(StringConv::asString(cls, "ClassName"));
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::Array<Pegasus::CIMInstance> peg_instances;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_name(c_cls);
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(
            property_list, "PropertyList"));

    ScopedTransactionBegin();
    peg_instances = m_client.enumerateInstances(
        peg_ns,
        peg_name,
        deep_inheritance,
        local_only,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_instances, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "EnumerateInstances(";
        if (Config::isVerboseMore()) {
            String c_ns(m_default_namespace);
            if (!isnone(ns))
                c_ns = StringConv::asString(ns);
            ss << "classname=u" << StringConv::asString(cls) << ", "
               << "namespace=u" << c_ns;
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::enumerateInstanceNames(
    const bp::object &cls,
    const bp::object &ns) try
{
    String c_cls(StringConv::asString(cls, "ClassName"));
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::Array<Pegasus::CIMObjectPath> peg_instance_names;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_name(c_cls);

    ScopedTransactionBegin();
    peg_instance_names = m_client.enumerateInstanceNames(
        peg_ns,
        peg_name);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_instance_names, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "EnumerateInstanceNames(";
        if (Config::isVerboseMore()) {
            String c_ns(m_default_namespace);
            if (!isnone(ns))
                c_ns = StringConv::asString(ns);
            ss << "classname=u'" << StringConv::asString(cls) << "', "
               << "namespace=u'" << c_ns << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::invokeMethod(
    const bp::tuple &args,
    const bp::dict  &kwds) try
{
    if (bp::len(args) != 2)
        throw_TypeError("InvokeMethod() takes at least 2 arguments");

    String c_method = StringConv::asString(args[0], "MethodName");

    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        args[1], "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();

    Pegasus::CIMValue peg_rval;
    Pegasus::Array<Pegasus::CIMParamValue> peg_out_params;
    Pegasus::Array<Pegasus::CIMParamValue> peg_in_params;

    // Create Pegasus::Array from **kwargs
    bp::list py_keys = kwds.keys();
    const int keys_cnt = bp::len(py_keys);
    for (int i = 0; i < keys_cnt; ++i) {
        String c_param_name = StringConv::asString(py_keys[i]);
        peg_in_params.append(
            Pegasus::CIMParamValue(
                c_param_name,
                CIMValue::asPegasusCIMValue(kwds[py_keys[i]]),
                true /* isTyped */));
    }

    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_name(c_method);

    ScopedTransactionBegin();
    peg_rval = m_client.invokeMethod(
        peg_ns,
        peg_path,
        peg_name,
        peg_in_params,
        peg_out_params);
    ScopedTransactionEnd();

    // Create a NocaseDict of method's return parameters
    bp::object py_rparams = NocaseDict::create();
    const Pegasus::Uint32 cnt = peg_out_params.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        py_rparams[bp::object(peg_out_params[i].getParameterName())] =
            CIMValue::asLMIWbemCIMValue(peg_out_params[i].getValue());
    }

    return bp::make_tuple(
        CIMValue::asLMIWbemCIMValue(peg_rval),
        py_rparams);
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "InvokeMethod(";
        if (Config::isVerboseMore()) {
            String c_method_name = StringConv::asString(args[0]);
            const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
                args[1]);
            ss << '\'' << c_method_name << "', '" << cim_inst_name.asString() << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    CIMInstanceName &cim_instance_name = CIMInstanceName::asNative(
        instance_name, "InstanceName");
    String c_ns(m_default_namespace);
    if (!cim_instance_name.getNamespace().empty())
        c_ns = cim_instance_name.getNamespace();
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::CIMInstance peg_instance;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMObjectPath peg_object_path = cim_instance_name.asPegasusCIMObjectPath();
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list, "PropertyList"));

    ScopedTransactionBegin();
    peg_instance = m_client.getInstance(
        peg_ns,
        peg_object_path,
        local_only,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    // CIMClient::getInstance() does not set the CIMObjectPath member in
    // CIMInstance. We need to do that manually.
    peg_instance.setPath(peg_object_path);

    return CIMInstance::create(peg_instance);
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "GetInstance(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(instance_name) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::CIMName peg_classname;
    if (!isnone(cls)) {
        String c_cls(StringConv::asString(cls, "ClassName"));
        peg_classname = Pegasus::CIMName(c_cls);
    }

    Pegasus::Array<Pegasus::CIMClass> peg_classes;
    Pegasus::CIMNamespaceName peg_ns(c_ns);

    ScopedTransactionBegin();
    peg_classes = m_client.enumerateClasses(
        peg_ns,
        peg_classname,
        deep_inheritance,
        local_only,
        include_qualifiers,
        include_class_origin);
    ScopedTransactionEnd();

    return ListConv::asPyCIMClassList(peg_classes);
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "EnumerateClasses(";
        if (Config::isVerboseMore()) {
            String c_ns(m_default_namespace);
            if (!isnone(ns))
                c_ns = StringConv::asString(ns);
            ss << "namespace=u'" << c_ns << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::enumerateClassNames(
    const bp::object &ns,
    const bp::object &cls,
    const bool deep_inheritance) try
{
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::CIMName peg_classname;
    if (!isnone(cls)) {
        String c_cls(StringConv::asString(cls, "ClassName"));
        peg_classname = Pegasus::CIMName(c_cls);
    }

    Pegasus::Array<Pegasus::CIMName> peg_classnames;
    Pegasus::CIMNamespaceName peg_ns(c_ns);

    ScopedTransactionBegin();
    peg_classnames = m_client.enumerateClassNames(
        peg_ns,
        peg_classname,
        deep_inheritance);
    ScopedTransactionEnd();

    // We do not create lmiwbem.CIMClassName objects here; we try to mimic pywbem.
    bp::list py_class_names;
    const Pegasus::Uint32 cnt = peg_classnames.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        py_class_names.append(bp::object(peg_classnames[i]));

    return py_class_names;;
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "EnumerateClasseNames(";
        if (Config::isVerboseMore()) {
            String c_ns(m_default_namespace);
            if (!isnone(ns))
                c_ns = StringConv::asString(ns);
            ss << "namespace=u'" << c_ns << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}


bp::object WBEMConnection::execQuery(
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &ns) try
{
    String c_query_lang = StringConv::asString(query_lang, "QueryLanguage");
    String c_query = StringConv::asString(query, "Query");
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::Array<Pegasus::CIMObject> peg_instances;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::String peg_query_lang(c_query_lang);
    Pegasus::String peg_query(c_query);

    ScopedTransactionBegin();
    peg_instances = m_client.execQuery(
        peg_ns,
        peg_query_lang,
        peg_query);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_instances, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "ExecQuery(";
        if (Config::isVerboseMore()) {
            ss << "lang=u'"  << StringConv::asString(query_lang) << "', "
               << "query=u'" << StringConv::asString(query) << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    String c_cls(StringConv::asString(cls, "ClassName"));
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");

    Pegasus::CIMClass peg_class;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_name(c_cls);
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(
            property_list, "PropertyList"));

    ScopedTransactionBegin()
    peg_class = m_client.getClass(
        peg_ns,
        peg_name,
        local_only,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return CIMClass::create(peg_class);
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "GetClass(";
        if (Config::isVerboseMore()) {
            String c_ns(m_default_namespace);
            if (!isnone(ns))
                c_ns = StringConv::asString(ns);
            ss << "class=u'" << StringConv::asString(cls) << "', "
               << "namespace=u'" << c_ns << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    const bp::object &property_list) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();

    String c_assoc_class;
    String c_result_class;
    String c_role;
    String c_result_role;
    if (!isnone(assoc_class))
        c_assoc_class = StringConv::asString(assoc_class, "AssocClass");
    if (!isnone(result_class))
        c_result_class = StringConv::asString(result_class, "ResultClass");
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");
    if (!isnone(result_role))
        c_result_role = StringConv::asString(result_role, "ResultRole");

    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> peg_associators;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_assoc_class;
    Pegasus::CIMName peg_result_class;

    if (!c_assoc_class.empty())
        peg_assoc_class = Pegasus::CIMName(c_assoc_class);
    if (!c_result_class.empty())
        peg_result_class = Pegasus::CIMName(c_result_class);

    ScopedTransactionBegin();
    peg_associators = m_client.associators(
        peg_ns,
        peg_path,
        peg_assoc_class,
        peg_result_class,
        c_role,
        c_result_role,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_associators, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "Associators(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::getAssociatorNames(
    const bp::object &object_path,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();

    String c_assoc_class;
    String c_result_class;
    String c_role;
    String c_result_role;
    if (!isnone(assoc_class))
        c_assoc_class = StringConv::asString(assoc_class, "AssocClass");
    if (!isnone(result_class))
        c_result_class = StringConv::asString(result_class, "ResultClass");
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");
    if (!isnone(result_role))
        c_result_role = StringConv::asString(result_role, "ResultRole");

    Pegasus::Array<Pegasus::CIMObjectPath> peg_associator_names;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_assoc_class;
    Pegasus::CIMName peg_result_class;

    if (!c_assoc_class.empty())
        peg_assoc_class = Pegasus::CIMName(c_assoc_class);
    if (!c_result_class.empty())
        peg_result_class = Pegasus::CIMName(c_result_class);

    ScopedTransactionBegin();
    peg_associator_names = m_client.associatorNames(
        peg_ns,
        peg_path,
        peg_assoc_class,
        peg_result_class,
        c_role,
        c_result_role);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_associator_names, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "AssociatorNames(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
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
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();

    String c_result_class;
    String c_role;
    if (!isnone(result_class))
        c_result_class = StringConv::asString(result_class, "ResultClass");
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    Pegasus::Array<Pegasus::CIMObject> peg_references;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_result_class;
    if (!c_result_class.empty())
        peg_result_class = Pegasus::CIMName(c_result_class);

    ScopedTransactionBegin();
    peg_references = m_client.references(
        peg_ns,
        peg_path,
        peg_result_class,
        c_role,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_references, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "References(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::getReferenceNames(
    const bp::object &object_path,
    const bp::object &result_class,
    const bp::object &role) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        c_ns = peg_path.getNameSpace().getString();

    String c_result_class;
    String c_role;
    if (!isnone(result_class))
        c_result_class = StringConv::asString(result_class, "ResultClass");
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    Pegasus::Array<Pegasus::CIMObjectPath> peg_reference_names;
    Pegasus::CIMNamespaceName peg_ns(c_ns);
    Pegasus::CIMName peg_result_class;
    if (!c_result_class.empty())
        peg_result_class = Pegasus::CIMName(c_result_class);

    ScopedTransactionBegin();
    peg_reference_names = m_client.referenceNames(
        peg_ns,
        peg_path,
        peg_result_class,
        c_role);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_reference_names, c_ns, m_client.getHostname());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "ReferenceNames(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}
