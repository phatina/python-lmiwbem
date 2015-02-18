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
#include "lmiwbem_client_cimxml.h"
#include "lmiwbem_client_wsman.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_make_method.h"
#include "lmiwbem_urlinfo.h"
#include "obj/lmiwbem_config.h"
#include "obj/lmiwbem_connection.h"
#include "obj/lmiwbem_connection_pydoc.h"
#include "obj/lmiwbem_nocasedict.h"
#include "obj/cim/lmiwbem_class.h"
#include "obj/cim/lmiwbem_class_name.h"
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

namespace bp = boost::python;

WBEMConnectionBase::WBEMConnectionBase()
    : m_client()
    , m_type(CLIENT_CIMXML)
{
}

CIMClient *WBEMConnectionBase::client() const
{
    if (!m_client) {
        switch (m_type) {
        case CLIENT_WSMAN:
#ifdef HAVE_OPENWSMAN
            m_client.reset(new WSMANClient());
            break;
#else
            // YES, this is OK. Fall through and always use CIM-XML.
#endif // HAVE_OPENWSMAN
        case CLIENT_CIMXML:
            m_client.reset(new CIMXMLClient());
            break;
        }
    }

    return m_client.get();
}

WBEMConnectionBase::CIMClientType WBEMConnectionBase::clientGetType() const
{
    return m_type;
}

void WBEMConnectionBase::clientSetType(CIMClientType type)
{
    if (m_type == type)
        return;

    m_type = type;
    m_client.reset();
}

WBEMConnection::ScopedConnection::ScopedConnection(WBEMConnection *conn)
    : m_conn(conn)
    , m_conn_orig_state(m_conn->client()->isConnected())
{
    if (m_conn_orig_state) {
        // We are already connected, nothing to do here.
        return;
    } else if (m_conn->m_connect_locally) {
        connectLocally();
    } else if (m_conn->client()->getURLInfo().isValid()) {
        connect();
    } else {
        throw_ValueError("WBEMConnection constructed with invalid url parameter");
    }
}

void WBEMConnection::ScopedConnection::connect() try
{
    m_conn->client()->connect(
        m_conn->client()->getUrl(),
        m_conn->m_username,
        m_conn->m_password,
        m_conn->m_cert_file,
        m_conn->m_key_file,
        Config::defaultTrustStore());
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "connect(";
        if (Config::isVerboseMore())
            ss << "url='" << m_conn->client()->getURLInfo().url() << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
}

void WBEMConnection::ScopedConnection::connectLocally() try
{
    m_conn->client()->connectLocally();
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose())
        ss << "connect_locally()";
    handle_all_exceptions(ss);
}

void WBEMConnection::ScopedConnection::disconnect()
{
    m_conn->client()->disconnect();
}

WBEMConnection::ScopedConnection::~ScopedConnection()
{
    if (!m_conn_orig_state)
        disconnect();
}

// -----------------------------------------------------------------------------

WBEMConnection::ScopedTransaction::ScopedTransaction(WBEMConnection *conn)
    : m_sct(conn->client())
{
}

WBEMConnection::WBEMConnection(
    const bp::object &url,
    const bp::object &creds,
    const bp::object &x509,
    const bp::object &default_namespace,
    const bp::object &no_verification,
    const bp::object &connect_locally)
    : WBEMConnectionBase()
    , CIMBase<WBEMConnection>()
    , m_connected_tmp(false)
    , m_connect_locally(false)
    , m_username()
    , m_password()
    , m_cert_file()
    , m_key_file()
    , m_default_namespace(Config::defaultNamespace())
{
    setConnectLocally(Conv::as<bool>(connect_locally, "connect_locally"));

    // We are constructing with local connection flag; disregard other
    // parameters.
    if (getConnectLocally()) {
        clientSetType(CLIENT_CIMXML);
        return;
    }

    if (!isnone(url)) {
        URLInfo url_info(StringConv::asString(url, "url"));

        if (!url_info.isValid()) {
            std::stringstream ss;
            ss << url_info.error() << '\'';
            throw_ValueError(ss.str());
        }

        // Decide, which backend will be used.
        if (url_info.isWSMAN())
            clientSetType(CLIENT_WSMAN);
        else
            clientSetType(CLIENT_CIMXML);

        client()->setUrlInfo(url_info);
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
    client()->setVerifyCertificate(c_verify);

    if (!isnone(default_namespace)) {
        m_default_namespace = StringConv::asString(
            default_namespace, "default_namespace");
    }

    // Set timeout to 1 minute.
    client()->setTimeout(60000);
}

WBEMConnection::~WBEMConnection()
{
    client()->disconnect();
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
            docstr_WBEMConnection_init))
    .def("__repr__", &WBEMConnection::repr)
    .def("connect", &WBEMConnection::connect,
        (bp::arg("url") = None,
         bp::arg("username") = None,
         bp::arg("password") = None,
         bp::arg("cert_file") = None,
         bp::arg("key_file") = None,
         bp::arg("no_verification") = None),
        docstr_WBEMConnection_connect)
    .def("connectLocally", &WBEMConnection::connectLocally,
        docstr_WBEMConnection_connectLocally)
    .def("disconnect",
        &WBEMConnection::disconnect,
        docstr_WBEMConnection_disconnect)
    .add_property("is_connected",
        &WBEMConnection::isConnected,
        docstr_WBEMConnection_is_connected)
    .add_property("hostname",
        &WBEMConnection::getHostname,
        docstr_WBEMConnection_hostname)
    .add_property("url",
        &WBEMConnection::getPyUrl)
    .add_property("no_verification",
        &WBEMConnection::getVerifyCertificate,
        &WBEMConnection::setVerifyCertificate)
    .add_property("connect_locally",
        &WBEMConnection::getConnectLocally,
        &WBEMConnection::setConnectLocally)
    .add_property("timeout",
        &WBEMConnection::getTimeout,
        &WBEMConnection::setTimeout,
        docstr_WBEMConnection_timeout)
    .add_property("request_accept_languages",
        &WBEMConnection::getRequestAcceptLanguages,
        &WBEMConnection::setRequestAcceptLanguages,
        docstr_WBEMConnection_request_accept_languages)
    .add_property("default_namespace",
        &WBEMConnection::getDefaultNamespace,
        &WBEMConnection::setDefaultNamespace,
        docstr_WBEMConnection_default_namespace)
    .add_property("creds",
        &WBEMConnection::getCredentials,
        &WBEMConnection::setCredentials)
    .def("CreateInstance", &WBEMConnection::createInstance,
        (bp::arg("NewInstance"),
         bp::arg("ns") = None),
        docstr_WBEMConnection_CreateInstance)
    .def("DeleteInstance", &WBEMConnection::deleteInstance,
        (bp::arg("InstanceName")),
        docstr_WBEMConnection_DeleteInstance)
    .def("ModifyInstance", &WBEMConnection::modifyInstance,
        (bp::arg("ModifiedInstance"),
         bp::arg("IncludeQualifiers") = true,
         bp::arg("PropertyList") = None),
        docstr_WBEMConnection_ModifyInstance)
    .def("EnumerateInstances", &WBEMConnection::enumerateInstances,
        (bp::arg("ClassName"),
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("DeepInheritance") = true,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
        docstr_WBEMConnection_EnumerateInstances)
    .def("EnumerateInstanceNames", &WBEMConnection::enumerateInstanceNames,
        (bp::arg("ClassName"),
         bp::arg("namespace") = None),
        docstr_WBEMConnection_EnumerateInstanceNames)
    .def("GetInstance", &WBEMConnection::getInstance,
        (bp::arg("InstanceName"),
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
        docstr_WBEMConnection_GetInstance)
    .def("EnumerateClasses", &WBEMConnection::enumerateClasses,
        (bp::arg("namespace") = None,
         bp::arg("ClassName") = None,
         bp::arg("DeepInheritance") = false,
         bp::arg("LocalOnly") = true,
         bp::arg("IncludeQualifiers") = true,
         bp::arg("IncludeClassOrigin") = false),
        docstr_WBEMConnection_EnumerateClasses)
    .def("EnumerateClassNames", &WBEMConnection::enumerateClassNames,
        (bp::arg("namespace") = None,
         bp::arg("ClassName") = None,
         bp::arg("DeepInheritance") = false),
        docstr_WBEMConnection_EnumerateClassNames)
    .def("ExecQuery", &WBEMConnection::execQuery,
        (bp::arg("QueryLanguage"),
         bp::arg("Query"),
         bp::arg("namespace") = None),
        docstr_WBEMConnection_ExecQuery)
    .def("InvokeMethod",
        lmi::raw_method<WBEMConnection>(&WBEMConnection::invokeMethod, 1),
        docstr_WBEMConnection_InvokeMethod)
    .def("GetClass", &WBEMConnection::getClass,
        (bp::arg("ClassName"),
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("IncludeQualifiers") = true,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None),
        docstr_WBEMConnection_GetClass)
    .def("Associators", &WBEMConnection::getAssociators,
        (bp::arg("ObjectName"),
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None,
         bp::arg("namespace") = None),
        docstr_WBEMConnection_Associators)
    .def("AssociatorNames", &WBEMConnection::getAssociatorNames,
        (bp::arg("ObjectName"),
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None,
         bp::arg("namespace") = None),
        docstr_WBEMConnection_AssociatorNames)
    .def("References", &WBEMConnection::getReferences,
        (bp::arg("ObjectName"),
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None,
         bp::arg("namespace") = None),
        docstr_WBEMConnection_References)
    .def("ReferenceNames", &WBEMConnection::getReferenceNames,
        (bp::arg("ObjectName"),
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("namespace") = None),
        docstr_WBEMConnection_ReferenceNames);
}

String WBEMConnection::repr() const
{
    std::stringstream ss;
    ss << "WBEMConnection(url=u'" << client()->getUrl() << "', ...)";
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
    String c_url(client()->getUrl());
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
        client()->setVerifyCertificate(!c_no_verify);
    }

    try {
        client()->connect(
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
                ss << "url='" << client()->getURLInfo().url() << '\'';
            ss << ')';
        }
        handle_all_exceptions(ss);
    }
}

void WBEMConnection::connectLocally() try
{
    client()->connectLocally();
    m_connect_locally = true;
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose())
        ss << "connect_locally()";
    handle_all_exceptions(ss);
}

void WBEMConnection::disconnect()
{
    client()->disconnect();
}

bool WBEMConnection::isConnected() const
{
    return client()->isConnected();
}

bp::object WBEMConnection::getHostname() const
{
    return StringConv::asPyUnicode(client()->getHostname());
}

bp::object WBEMConnection::getPyUrl() const
{
    return StringConv::asPyUnicode(client()->getUrl());
}

bool WBEMConnection::getVerifyCertificate() const
{
    return !client()->getVerifyCertificate();
}

void WBEMConnection::setVerifyCertificate(bool verify_cert)
{
    client()->setVerifyCertificate(!verify_cert);
}

bool WBEMConnection::getConnectLocally() const
{
    return m_connect_locally;
}

void WBEMConnection::setConnectLocally(bool connect_locally)
{
    if (clientGetType() == CLIENT_WSMAN) {
        if (m_connect_locally)
            m_connect_locally = false;
        return;
    }

    m_connect_locally = connect_locally;
}

unsigned int WBEMConnection::getTimeout() const
{
    return client()->getTimeout();
}

void WBEMConnection::setTimeout(unsigned int timeout)
{
    client()->setTimeout(timeout);
}

bp::object WBEMConnection::getRequestAcceptLanguages() const
{
    Pegasus::AcceptLanguageList peg_al_list = client()->getRequestAcceptLanguages();
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

    client()->setRequestAcceptLanguages(peg_al_list);
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
    peg_new_inst_name = client()->createInstance(
        peg_new_inst_name_ns,
        peg_inst);
    ScopedTransactionEnd();

    // CIMClient::createInstance() does not set namespace and hostname
    // in newly created CIMInstanceName. We need to do that manually.
    peg_new_inst_name.setNameSpace(Pegasus::CIMNamespaceName(c_ns));
    peg_new_inst_name.setHost(client()->getHostname());

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
    client()->deleteInstance(
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
    client()->modifyInstance(
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
    peg_instances = client()->enumerateInstances(
        peg_ns,
        peg_name,
        deep_inheritance,
        local_only,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_instances, c_ns, client()->getHostname());
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
    peg_instance_names = client()->enumerateInstanceNames(
        peg_ns,
        peg_name);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_instance_names, c_ns, client()->getHostname());
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
    peg_rval = client()->invokeMethod(
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
    peg_instance = client()->getInstance(
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
    peg_classes = client()->enumerateClasses(
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
    peg_classnames = client()->enumerateClassNames(
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
    peg_instances = client()->execQuery(
        peg_ns,
        peg_query_lang,
        peg_query);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_instances, c_ns, client()->getHostname());
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
    peg_class = client()->getClass(
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
    const bp::object &property_list,
    const bp::object &ns) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    else if (!peg_path.getNameSpace().isNull())
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
    peg_associators = client()->associators(
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
        peg_associators, c_ns, client()->getHostname());
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
    const bp::object &result_role,
    const bp::object &ns) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    else if (!peg_path.getNameSpace().isNull())
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
    peg_associator_names = client()->associatorNames(
        peg_ns,
        peg_path,
        peg_assoc_class,
        peg_result_class,
        c_role,
        c_result_role);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_associator_names, c_ns, client()->getHostname());
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
    const bp::object &property_list,
    const bp::object &ns) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    else if (!peg_path.getNameSpace().isNull())
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
    peg_references = client()->references(
        peg_ns,
        peg_path,
        peg_result_class,
        c_role,
        include_qualifiers,
        include_class_origin,
        peg_property_list);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceList(
        peg_references, c_ns, client()->getHostname());
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
    const bp::object &role,
    const bp::object &ns) try
{
    const CIMInstanceName &cim_inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = cim_inst_name.asPegasusCIMObjectPath();

    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    else if (!peg_path.getNameSpace().isNull())
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
    peg_reference_names = client()->referenceNames(
        peg_ns,
        peg_path,
        peg_result_class,
        c_role);
    ScopedTransactionEnd();

    return ListConv::asPyCIMInstanceNameList(
        peg_reference_names, c_ns, client()->getHostname());
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
