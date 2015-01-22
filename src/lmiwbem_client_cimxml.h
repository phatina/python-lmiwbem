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

#ifndef   LMIWBEM_CLIENT_CIMXML_H
#  define LMIWBEM_CLIENT_CIMXML_H

#  include <Pegasus/Client/CIMClient.h>
#  include "lmiwbem_client.h"
#  include "util/lmiwbem_string.h"

class CIMXMLClient;

class CIMXMLClient: public CIMClient
{
public:
    CIMXMLClient();
    virtual ~CIMXMLClient();

    // Connection API
    virtual void connect(
        const String &uri,
        const String &username,
        const String &password,
        const String &cert_file,
        const String &key_file,
        const String &trust_store);
    virtual void connectLocally();
    virtual void disconnect();

    // ------------------------------------------------------------------------
    // CIM API
    //
    virtual void setTimeout(Pegasus::Uint32 timeoutMilliseconds);
    virtual void setRequestAcceptLanguages(const Pegasus::AcceptLanguageList& langs);
    virtual Pegasus::Uint32 getTimeout() const;
    virtual Pegasus::AcceptLanguageList getRequestAcceptLanguages() const;

    virtual Pegasus::CIMObjectPath createInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMInstance &newInstance);
    virtual void deleteInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName);
    virtual void modifyInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMInstance &modifiedInstance,
        Pegasus::Boolean includeQualifiers = true,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());

    // Enumeration
    virtual Pegasus::Array<Pegasus::CIMClass> enumerateClasses(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className = Pegasus::CIMName(),
        Pegasus::Boolean deepInheritance = false,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = true,
        Pegasus::Boolean includeClassOrigin = false);
    virtual Pegasus::Array<Pegasus::CIMName> enumerateClassNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className = Pegasus::CIMName(),
        Pegasus::Boolean deepInheritance = false);
    virtual Pegasus::Array<Pegasus::CIMInstance> enumerateInstances(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className,
        Pegasus::Boolean deepInheritance = true,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());
    virtual Pegasus::Array<Pegasus::CIMObjectPath> enumerateInstanceNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className);
    virtual Pegasus::Array<Pegasus::CIMObject> execQuery(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::String &queryLanguage,
        const Pegasus::String &query);

    // Getters
    virtual Pegasus::CIMClass getClass(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName& className,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = true,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());
    virtual Pegasus::CIMInstance getInstance(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName,
        Pegasus::Boolean localOnly = true,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());

    // Method call
    virtual Pegasus::CIMValue invokeMethod(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &instanceName,
        const Pegasus::CIMName &methodName,
        const Pegasus::Array<Pegasus::CIMParamValue> &inParameters,
        Pegasus::Array<Pegasus::CIMParamValue> &outParameters);

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
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());
    virtual Pegasus::Array<Pegasus::CIMObjectPath> associatorNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &assocClass = Pegasus::CIMName(),
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        const Pegasus::String &resultRole = Pegasus::String::EMPTY);
    virtual Pegasus::Array<Pegasus::CIMObject> references(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY,
        Pegasus::Boolean includeQualifiers = false,
        Pegasus::Boolean includeClassOrigin = false,
        const Pegasus::CIMPropertyList &propertyList = Pegasus::CIMPropertyList());
    virtual Pegasus::Array<Pegasus::CIMObjectPath> referenceNames(
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMObjectPath &objectName,
        const Pegasus::CIMName &resultClass = Pegasus::CIMName(),
        const Pegasus::String &role = Pegasus::String::EMPTY);

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
        Pegasus::Uint32 maxObjectCount = 0);
    virtual Pegasus::Array<Pegasus::CIMObjectPath> openEnumerateInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        const Pegasus::CIMNamespaceName &nameSpace,
        const Pegasus::CIMName &className,
        const Pegasus::String &filterQueryLanguage = Pegasus::String::EMPTY,
        const Pegasus::String &filterQuery = Pegasus::String::EMPTY,
        const Pegasus::Uint32Arg &operationTimeout = Pegasus::Uint32Arg(),
        Pegasus::Boolean continueOnError = false,
        Pegasus::Uint32 maxObjectCount = 0);
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
        Pegasus::Uint32 maxObjectCount = 0);
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
        Pegasus::Uint32 maxObjectCount = 0);
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
        Pegasus::Uint32 maxObjectCount = 0);
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
        Pegasus::Uint32 maxObjectCount = 0);
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
        Pegasus::Uint32 maxObjectCount = 0);
    virtual Pegasus::Array<Pegasus::CIMInstance> pullInstancesWithPath(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount);
    virtual Pegasus::Array<Pegasus::CIMObjectPath> pullInstancePaths(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount);
    virtual Pegasus::Array<Pegasus::CIMInstance> pullInstances(
        Pegasus::CIMEnumerationContext &enumerationContext,
        Pegasus::Boolean &endOfSequence,
        Pegasus::Uint32 maxObjectCount);
    virtual void closeEnumeration(
        Pegasus::CIMEnumerationContext &enumerationContext);
    virtual Pegasus::Uint64Arg enumerationCount(
        Pegasus::CIMEnumerationContext &enumerationContext);
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

private:
    static bool matchPattern(const Pegasus::String &pattern, const Pegasus::String &str);

#  ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
    static Pegasus::Boolean verifyCertificate(Pegasus::SSLCertificateInfo &ci, void *data);
#  else
    static Pegasus::Boolean verifyCertificate(Pegasus::SSLCertificateInfo &ci);
#  endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA

    Pegasus::CIMClient m_client;
};

#endif // LMIWBEM_CLIENT_CIMXML_H
