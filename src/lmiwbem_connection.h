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
    class list;
    class object;
BOOST_PYTHON_END

namespace bp = boost::python;

class WBEMConnection
{
public:
    WBEMConnection(const bp::object &verify_cert);

    WBEMConnection(
        const bp::object &url,
        const bp::object &creds,
        const bp::object &default_namespace,
        const bp::object &verify_cert);
    ~WBEMConnection();

    static void init_type();

    void connect(
        const bp::object &url,
        const bp::object &username,
        const bp::object &password,
        const bp::object &verify_cert);
    void disconnect();
    bool isConnected() const { return m_client.isConnected(); }

    void connectTmp();
    void disconnectTmp();

    bool getVerifyCertificate() { return m_client.getVerifyCertificate(); }
    void setVerifyCertificate(bool verify_cert) { m_client.setVerifyCertificate(verify_cert); }

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

    bp::object getClass(
        const bp::object &cls,
        const bp::object &ns,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::object getAssociators(
        const bp::object &object_path,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object property_list);

    bp::object getAssociatorNames(
        const bp::object &object_path,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role);

    bp::object getReferences(
        const bp::object &object_path,
        const bp::object &result_class,
        const bp::object &role,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::object getReferenceNames(
        const bp::object &object_path,
        const bp::object &result_class,
        const bp::object &role);

protected:
    static std::string s_default_namespace;

    bool m_connected_tmp;
    std::string m_url;
    std::string m_username;
    std::string m_password;
    std::string m_default_namespace;
    CIMClient m_client;
    Mutex m_mutex;
};

#endif // LMIWBEM_CONNECTION_H
