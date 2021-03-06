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

#ifndef   LMIWBEM_CLIENT_H
#  define LMIWBEM_CLIENT_H

#  include <config.h>
#  include <Pegasus/Common/Config.h> // types
#  include <Pegasus/Common/AcceptLanguageList.h>
#  include <Pegasus/Common/CIMClass.h>
#  include <Pegasus/Common/CIMInstance.h>
#  include <Pegasus/Common/CIMObject.h>
#  include <Pegasus/Common/CIMObjectPath.h>
#  include <Pegasus/Common/CIMParamValue.h>
#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
#    include <Pegasus/Client/CIMEnumerationContext.h>
#    include <Pegasus/Common/UintArgs.h>
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT
#  include "lmiwbem_mutex.h"
#  include "lmiwbem_urlinfo.h"
#  include "util/lmiwbem_string.h"

// Base class for CIMXML and WSMAN clients.
class CIMClient
{
public:
    // Every CIMClient operation needs to have a guard.
    //
    // Example:
    //     CIMClient client;
    //     CIMClient::ScopedCIMClientTransaction sc(client);
    //     ... further CIM operations ...
    class ScopedCIMClientTransaction
    {
    public:
        ScopedCIMClientTransaction(CIMClient *client);
        ~ScopedCIMClientTransaction();
    private:
        CIMClient *m_client;
    };

    friend class ScopedCIMClientTransaction;

public:
    CIMClient();
    virtual ~CIMClient() = 0;

    // ------------------------------------------------------------------------
    // Connection API
    //
    virtual void connect(
        const String &uri,
        const String &username,
        const String &password,
        const String &cert_file,
        const String &key_file,
        const String &trust_store) = 0;
    virtual void connectLocally() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const;

    void setVerifyCertificate(bool verify = true);
    void setUrlInfo(const URLInfo &url_info);
    bool setUrl(const String &url);
    bool getVerifyCertificate() const;
    URLInfo getURLInfo() const;
    String getHostname() const;
    String getUrl() const;

    // ------------------------------------------------------------------------
    // CIM API
    //
    virtual void setTimeout(Pegasus::Uint32 timeoutMilliseconds) = 0;
    virtual void setRequestAcceptLanguages(const Pegasus::AcceptLanguageList& langs) = 0;
    virtual Pegasus::Uint32 getTimeout() const = 0;
    virtual Pegasus::AcceptLanguageList getRequestAcceptLanguages() const = 0;

    virtual Pegasus::CIMObjectPath createInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMInstance &newInstance) = 0;
    virtual void deleteInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName) = 0;
    virtual void modifyInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMInstance &modifiedInstance,
        Pegasus::Boolean includeQualifiers = true,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;

    // Enumeration
    virtual Pegasus::Array<Pegasus::CIMClass> enumerateClasses(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className = Pegasus::CIMName(),
        Pegasus::Boolean deepInheritance = false,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = true,
        Pegasus::Boolean includeClassOrigin = false) = 0;
    virtual Pegasus::Array<Pegasus::CIMName> enumerateClassNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className = Pegasus::CIMName(),
        Pegasus::Boolean deepInheritance = false) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> enumerateInstances(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className,
        Pegasus::Boolean deepInheritance = true,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> enumerateInstanceNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className) = 0;
    virtual Pegasus::Array<Pegasus::CIMObject> execQuery(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::String &queryLanguage,
        const Pegasus::String &query) = 0;

    // Getters
    virtual Pegasus::CIMClass getClass(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName& className,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = true,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;
    virtual Pegasus::CIMInstance getInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;

    // Method call
    virtual Pegasus::CIMValue invokeMethod(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName,
        const Pegasus::CIMName &methodName,
        const Pegasus::Array<Pegasus::CIMParamValue> &inParameters,
        Pegasus::Array<Pegasus::CIMParamValue> &outParameters) = 0;

    // Associations
    virtual Pegasus::Array<Pegasus::CIMObject> associators(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &assocClass = Pegasus::CIMName(),
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &resultRole = Pegasus::String::EMPTY,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> associatorNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &assocClass = Pegasus::CIMName(),
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &resultRole = Pegasus::String::EMPTY) = 0;
    virtual Pegasus::Array<Pegasus::CIMObject> references(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList()) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> referenceNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY) = 0;

#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    // Pull operations
    virtual Pegasus::Array<Pegasus::CIMInstance> openEnumerateInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className,
        Pegasus::Boolean deepInheritance = true,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList(),
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> openEnumerateInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className,
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> openReferenceInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList(),
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> openReferenceInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> openAssociatorInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &assocClass = Pegasus::CIMName(),
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &resultRole = Pegasus::String::EMPTY,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList(),
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> openAssociatorInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &assocClass = Pegasus::CIMName(),
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &resultRole = Pegasus::String::EMPTY,
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> openQueryInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::String &queryLanguage,
        const Pegasus::String &query,
        Pegasus::CIMClass &queryResultClass,
        Pegasus::Boolean returnQueryResultClass = false,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> pullInstancesWithPath(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount) = 0;
    virtual Pegasus::Array<Pegasus::CIMObjectPath> pullInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount) = 0;
    virtual Pegasus::Array<Pegasus::CIMInstance> pullInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount) = 0;
    virtual void closeEnumeration(
        Pegasus::CIMEnumerationContext &enumerationContext) = 0;
    virtual Pegasus::Uint64Arg enumerationCount(
        Pegasus::CIMEnumerationContext &enumerationContext) = 0;
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

protected:
    URLInfo m_url_info;
    Mutex m_mutex;
    bool m_is_connected;
    bool m_verify_cert;
};

#endif // LMIWBEM_CLIENT_H
