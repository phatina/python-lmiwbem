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

#ifndef   LMIWBEM_CONNECTION_H
#  define LMIWBEM_CONNECTION_H

#  include <boost/python/class.hpp>
#  include "lmiwbem.h"
#  include "lmiwbem_cimbase.h"
#  include "lmiwbem_client.h"
#  include "lmiwbem_gil.h"
#  include "util/lmiwbem_string.h"

BOOST_PYTHON_BEGIN
    class dict;
    class object;
    class tuple;
BOOST_PYTHON_END

namespace bp = boost::python;

class WBEMConnection: public CIMBase<WBEMConnection>
{
private:
        /* NOTE: These macros need to be used around every CIM operation.
         * ScopedTransactionBegin creates a temporary connection, if necessary and also
         * it ensures that CIMClient can enter a critical section.
         * ScopedTransactionEnd is defined due to semantics; to close the scope.
         */
#  define ScopedTransactionBegin() { \
       ScopedTransaction _st(this);  \
       ScopedConnection  _sc(this);
#  define ScopedTransactionEnd() }

    class ScopedConnection
    {
    public:
        ScopedConnection(WBEMConnection *conn);
        ~ScopedConnection();

    private:
        WBEMConnection *m_conn;
        bool m_conn_orig_state;
    };

    class ScopedTransaction
    {
    public:
        ScopedTransaction(WBEMConnection *conn);

    private:
        CIMClient::ScopedCIMClientTransaction m_sct;
    };

    friend class ScopedConnection;
    friend class ScopedTransaction;

    typedef bp::class_<WBEMConnection, boost::noncopyable> WBEMConnectionClass;

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

    String repr() const;

    void connect(
        const bp::object &url,
        const bp::object &username,
        const bp::object &password,
        const bp::object &cert_file,
        const bp::object &key_file,
        const bp::object &no_verification);
    void connectLocally();
    void disconnect();
    bool isConnected() const;
    bp::object getHostname() const;

    bool getVerifyCertificate() const;
    void setVerifyCertificate(bool verify_cert);
    bool getConnectLocally() const;
    void setConnectLocally(bool connect_locally);
    unsigned int getTimeout() const;
    void setTimeout(unsigned int timeout);
    bp::object getRequestAcceptLanguages() const;
    void setRequestAcceptLanguages(const bp::object &langs);
    bp::object getDefaultNamespace() const;
    void setDefaultNamespace(const bp::object &ns);
    bp::object getCredentials() const;
    void setCredentials(const bp::object &creds);

    bp::object createInstance(
        const bp::object &instance,
        const bp::object &ns);

    void deleteInstance(const bp::object &object_path);

    void modifyInstance(
        const bp::object &instance,
        const bool include_qualifiers,
        const bp::object &property_list);

    bp::object enumerateInstances(
        const bp::object &cls,
        const bp::object &ns,
        const bool local_only,
        const bool deep_inheritance,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::object enumerateInstanceNames(
        const bp::object &cls,
        const bp::object &ns);

    bp::object getInstance(
        const bp::object &instance_name,
        const bp::object &ns,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin,
        const bp::object &property_list);

    bp::object enumerateClasses(
        const bp::object &ns,
        const bp::object &cls,
        const bool deep_inheritance,
        const bool local_only,
        const bool include_qualifiers,
        const bool include_class_origin);

    bp::object enumerateClassNames(
        const bp::object &ns,
        const bp::object &cls,
        const bool deep_inheritance);

    bp::object execQuery(
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &ns);

    bp::object invokeMethod(
        const bp::tuple &args,
        const bp::dict  &kwds);

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

#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    bp::object openEnumerateInstances(
        const bp::object &cls,
        const bp::object &ns,
        const bp::object &deep_inheritance,
        const bp::object &include_class_origin,
        const bp::object &property_list,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openEnumerateInstanceNames(
        const bp::object &cls,
        const bp::object &ns,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openAssociators(
        const bp::object &object_path,
        const bp::object &ns,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role,
        const bp::object &include_class_origin,
        const bp::object &property_list,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openAssociatorNames(
        const bp::object &object_path,
        const bp::object &ns,
        const bp::object &assoc_class,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &result_role,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openReferences(
        const bp::object &object_path,
        const bp::object &ns,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &include_class_origin,
        const bp::object &property_list,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openReferenceNames(
        const bp::object &object_path,
        const bp::object &ns,
        const bp::object &result_class,
        const bp::object &role,
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object openExecQuery(
        const bp::object &query_lang,
        const bp::object &query,
        const bp::object &ns,
        const bp::object &operation_timeout,
        const bp::object &continue_on_error,
        const bp::object &max_object_cnt);

    bp::object pullInstances(
        const bp::object &ctx,
        const bp::object &max_object_cnt);

    bp::object pullInstanceNames(
        bp::object &ctx,
        bp::object &max_object_cnt);

    void closeEnumeration(const bp::object &ctx);
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

protected:
    static void init_type_base(WBEMConnectionClass &cls);
#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    static void init_type_pull(WBEMConnectionClass &cls);
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

    bool m_connected_tmp;
    bool m_connect_locally;
    String m_url;
    String m_username;
    String m_password;
    String m_cert_file;
    String m_key_file;
    String m_default_namespace;
    CIMClient m_client;
};

#endif // LMIWBEM_CONNECTION_H
