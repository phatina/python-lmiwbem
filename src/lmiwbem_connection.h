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

#ifndef LMIWBEM_CONNECTION_H
#define LMIWBEM_CONNECTION_H

#include <string>
#include "lmiwbem.h"
#include "lmiwbem_client.h"
#include "lmiwbem_mutex.h"

BOOST_PYTHON_BEGIN
    class dict;
    class list;
    class object;
    class tuple;
BOOST_PYTHON_END

namespace bp = boost::python;

class WBEMConnection
{
public:
    WBEMConnection(
        const bp::object &url,
        const bp::object &creds,
        const bp::object &x509,
        const bp::object &default_namespace,
        const bp::object &no_verification,
        const bp::object &connect_locally);
    ~WBEMConnection();

    static void init_type();

    void connect(
        const bp::object &url,
        const bp::object &username,
        const bp::object &password,
        const bp::object &cert_file,
        const bp::object &key_file,
        const bp::object &no_verification);
    void connectLocally();
    void disconnect();
    bool isConnected() const { return m_client.isConnected(); }
    std::string getHostname() const { return m_client.hostname(); }

    void connectTmp();
    void disconnectTmp();

    bool getVerifyCertificate() { return !m_client.getVerifyCertificate(); }
    void setVerifyCertificate(bool verify_cert) { m_client.setVerifyCertificate(!verify_cert); }
    bool getConnectLocally() const { return m_connect_locally; }
    void setConnectLocally(bool connect_locally) { m_connect_locally = connect_locally; }
    unsigned int getTimeout() const { return m_client.getTimeout(); }
    void setTimeout(unsigned int timeout) { m_client.setTimeout(timeout); }
    std::string getDefaultNamespace() const { return m_default_namespace; }
    void setDefaultNamespace(const bp::object &ns);
    bp::object getCredentials() const;

    bp::object createInstance(const bp::object &instance);

    void deleteInstance(const bp::object &object_path);

    void modifyInstance(
        const bp::object &instance,
        const bool include_qualifiers,
        const bp::object &property_list);

    bp::list enumerateInstances(
        const bp::object &cls,
        const bp::object &ns,
        const bool local_only,
        const bool deep_inheritance,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::list enumerateInstanceNames(
        const bp::object &cls,
        const bp::object &ns);

    bp::object getInstance(
        const bp::object &instance_name,
        const bp::object &ns,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::list enumerateClasses(
        const bp::object &ns,
        const bp::object &cls,
        const bool deep_inheritance,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin);

    bp::list enumerateClassNames(
        const bp::object &ns,
        const bp::object &cls,
        const bool deep_inheritance);

    bp::list execQuery(
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &ns);

    bp::tuple invokeMethod(
        const bp::tuple &args,
        const bp::dict  &kwds);

    bp::object getClass(
        const bp::object &cls,
        const bp::object &ns,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::list getAssociators(
        const bp::object &object_path,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object property_list);

    bp::list getAssociatorNames(
        const bp::object &object_path,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role);

    bp::list getReferences(
        const bp::object &object_path,
        const bp::object &result_class,
        const bp::object &role,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::list getReferenceNames(
        const bp::object &object_path,
        const bp::object &result_class,
        const bp::object &role);

protected:
    bool m_connected_tmp;
    bool m_connect_locally;
    std::string m_url;
    std::string m_username;
    std::string m_password;
    std::string m_cert_file;
    std::string m_key_file;
    std::string m_default_namespace;
    CIMClient m_client;
    Mutex m_mutex;
};

#endif // LMIWBEM_CONNECTION_H
