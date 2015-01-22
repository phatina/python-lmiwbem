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
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/SSLContext.h>
#include "lmiwbem_client_cimxml.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_gil.h"
#include "obj/cim/lmiwbem_constants.h"

#include <cctype>

CIMXMLClient::CIMXMLClient()
    : CIMClient()
{
}

CIMXMLClient::~CIMXMLClient()
{
}

void CIMXMLClient::connect(
    const String &uri,
    const String &username,
    const String &password,
    const String &cert_file,
    const String &key_file,
    const String &trust_store)
{
    if (!uri.empty()) {
        // Override URL info by.
        m_url_info.set(uri);
    }

    if (!m_url_info.isValid()) {
        throw Pegasus::InvalidLocatorException(
            String("Invalid locator: ") + uri);
    }

    bool is_creds_valid = m_url_info.isCredsValid();

    if (m_url_info.isLocal()) {
        connectLocally();
    } else if (!m_url_info.isHttps()) {
        m_client.connect(
            m_url_info.hostname(),
            m_url_info.port(),
            is_creds_valid ? m_url_info.username() : username,
            is_creds_valid ? m_url_info.password() : password);
    } else {
        Pegasus::SSLContext ctx(
            trust_store,
            cert_file,
            key_file,
            m_verify_cert ? verifyCertificate : NULL,
#ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
            this,
#endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
            String()
        );
        m_client.connect(
            m_url_info.hostname(),
            m_url_info.port(),
            ctx,
            is_creds_valid ? m_url_info.username() : username,
            is_creds_valid ? m_url_info.password() : password);
    }
    m_is_connected = true;
}

void CIMXMLClient::connectLocally()
{
    m_client.connectLocal();
    m_is_connected = true;
    m_url_info.set("localhost");
}

void CIMXMLClient::disconnect()
{
    m_client.disconnect();
    m_is_connected = false;
}

void CIMXMLClient::setTimeout(Pegasus::Uint32 timeoutMilliseconds)
{
    m_client.setTimeout(timeoutMilliseconds);
}

void CIMXMLClient::setRequestAcceptLanguages(
    const Pegasus::AcceptLanguageList& langs)
{
    m_client.setRequestAcceptLanguages(langs);
}

Pegasus::Uint32 CIMXMLClient::getTimeout() const
{
    return m_client.getTimeout();
}

Pegasus::AcceptLanguageList CIMXMLClient::getRequestAcceptLanguages() const
{
    return m_client.getRequestAcceptLanguages();
}

Pegasus::CIMObjectPath CIMXMLClient::createInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMInstance &newInstance)
{
    return m_client.createInstance(nameSpace, newInstance);
}

void CIMXMLClient::deleteInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName)
{
    m_client.deleteInstance(nameSpace, instanceName);
}

void CIMXMLClient::modifyInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMInstance &modifiedInstance,
    Pegasus::Boolean includeQualifiers,
    const Pegasus::CIMPropertyList &propertyList)
{
    m_client.modifyInstance(
        nameSpace, modifiedInstance, includeQualifiers, propertyList);
}

Pegasus::Array<Pegasus::CIMClass> CIMXMLClient::enumerateClasses(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin)
{
    return m_client.enumerateClasses(
        nameSpace, className, deepInheritance, localOnly, includeQualifiers,
        includeClassOrigin);
}

Pegasus::Array<Pegasus::CIMName> CIMXMLClient::enumerateClassNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance)
{
    return m_client.enumerateClassNames(
        nameSpace, className, deepInheritance);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::enumerateInstances(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    return m_client.enumerateInstances(
        nameSpace, className, deepInheritance, localOnly, includeQualifiers,
        includeClassOrigin, propertyList);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::enumerateInstanceNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className)
{
    return m_client.enumerateInstanceNames(nameSpace, className);
}

Pegasus::Array<Pegasus::CIMObject> CIMXMLClient::execQuery(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::String &queryLanguage,
    const Pegasus::String &query)
{
    return m_client.execQuery(nameSpace, queryLanguage, query);
}

Pegasus::CIMClass CIMXMLClient::getClass(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName& className,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    return m_client.getClass(
        nameSpace, className, localOnly, includeQualifiers, includeClassOrigin,
        propertyList);
}

Pegasus::CIMInstance CIMXMLClient::getInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    return m_client.getInstance(
        nameSpace, instanceName, localOnly, includeQualifiers,
        includeClassOrigin, propertyList);
}

Pegasus::CIMValue CIMXMLClient::invokeMethod(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName,
    const Pegasus::CIMName &methodName,
    const Pegasus::Array<Pegasus::CIMParamValue> &inParameters,
    Pegasus::Array<Pegasus::CIMParamValue> &outParameters)
{
    return m_client.invokeMethod(
        nameSpace, instanceName, methodName, inParameters, outParameters);
}

Pegasus::Array<Pegasus::CIMObject> CIMXMLClient::associators(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &assocClass,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &resultRole,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    return m_client.associators(
        nameSpace, objectName, assocClass, resultClass, role, resultRole,
        includeQualifiers, includeClassOrigin, propertyList);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::associatorNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &assocClass,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &resultRole)
{
    return m_client.associatorNames(
        nameSpace, objectName, assocClass, resultClass, role, resultRole);
}

Pegasus::Array<Pegasus::CIMObject> CIMXMLClient::references(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    return m_client.references(
        nameSpace, objectName, resultClass, role, includeQualifiers,
        includeClassOrigin, propertyList);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::referenceNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role)
{
    return m_client.referenceNames(nameSpace, objectName, resultClass, role);
}

#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::openEnumerateInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openEnumerateInstances(
        enumerationContext, endOfSequence, nameSpace, className,
        deepInheritance, includeClassOrigin, propertyList, filterQueryLanguage,
        filterQuery, operationTimeout, continueOnError, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::openEnumerateInstancePaths(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openEnumerateInstancePaths(
        enumerationContext, endOfSequence, nameSpace, className,
        filterQueryLanguage, filterQuery, operationTimeout, continueOnError,
        maxObjectCount);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::openReferenceInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openReferenceInstances(
        enumerationContext, endOfSequence, nameSpace, objectName, resultClass,
        role, includeClassOrigin, propertyList, filterQueryLanguage,
        filterQuery, operationTimeout, continueOnError, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::openReferenceInstancePaths(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openReferenceInstancePaths(
        enumerationContext, endOfSequence, nameSpace, objectName, resultClass,
        role, filterQueryLanguage, filterQuery, operationTimeout,
        continueOnError, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::openAssociatorInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &assocClass,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &resultRole,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openAssociatorInstances(
        enumerationContext, endOfSequence, nameSpace, objectName, assocClass,
        resultClass, role, resultRole, includeClassOrigin, propertyList,
        filterQueryLanguage, filterQuery, operationTimeout, continueOnError,
        maxObjectCount);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::openAssociatorInstancePaths(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &assocClass,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &resultRole,
    const Pegasus::String &filterQueryLanguage,
    const Pegasus::String &filterQuery,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openAssociatorInstancePaths(
        enumerationContext, endOfSequence, nameSpace, objectName, assocClass,
        resultClass, role, resultRole, filterQueryLanguage, filterQuery,
        operationTimeout, continueOnError, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::openQueryInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::String &queryLanguage,
    const Pegasus::String &query,
    Pegasus::CIMClass &queryResultClass,
    Pegasus::Boolean returnQueryResultClass,
    const Pegasus::Uint32Arg &operationTimeout,
    Pegasus::Boolean continueOnError,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.openQueryInstances(
        enumerationContext, endOfSequence, nameSpace, queryLanguage, query,
        queryResultClass, returnQueryResultClass, operationTimeout,
        continueOnError, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::pullInstancesWithPath(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.pullInstancesWithPath(
        enumerationContext, endOfSequence, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMObjectPath> CIMXMLClient::pullInstancePaths(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.pullInstancePaths(
        enumerationContext, endOfSequence, maxObjectCount);
}

Pegasus::Array<Pegasus::CIMInstance> CIMXMLClient::pullInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    return m_client.pullInstances(
        enumerationContext, endOfSequence, maxObjectCount);
}

void CIMXMLClient::closeEnumeration(
    Pegasus::CIMEnumerationContext &enumerationContext)
{
    m_client.closeEnumeration(enumerationContext);
}

Pegasus::Uint64Arg CIMXMLClient::enumerationCount(
    Pegasus::CIMEnumerationContext &enumerationContext)
{
    return m_client.enumerationCount(enumerationContext);
}
#endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

#ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
bool CIMXMLClient::matchPattern(const Pegasus::String &pattern, const Pegasus::String &str)
{
    Pegasus::String l_str(str);
    Pegasus::String l_pattern(pattern);
    l_str.toLower();
    l_pattern.toLower();

    Pegasus::Uint32 p = 0;
    Pegasus::Uint32 n = 0;

    const Pegasus::Uint32 str_len = l_str.size();
    const Pegasus::Uint32 pattern_len = l_pattern.size();
    for (; p < pattern_len && n < str_len; ++p, ++n) {
        // Got wildcard symbol; skip all of them, if there is more than one
        // asterisk in a row. When done, skip one domain name in the string.
        if (l_pattern[p] == '*') {
            for (; l_pattern[p] == '*' && p < pattern_len; ++p)
                ;
            for (; l_str[n] != '.' && n < str_len; ++n)
                ;
        }
        if (l_pattern[p] != l_str[n])
            return false;
    }

    return n == str_len;
}

Pegasus::Boolean CIMXMLClient::verifyCertificate(Pegasus::SSLCertificateInfo &ci, void *data)
{
    if (!ci.getResponseCode()) {
        // Pre-verify of the certificate failed, immediate return
        return false;
    }

    CIMXMLClient *fake_this = reinterpret_cast<CIMXMLClient*>(data);
    Pegasus::String hostname(fake_this->m_url_info.hostname());

    // Verify against DNS names
    Pegasus::Array<Pegasus::String> dnsNames = ci.getSubjectAltNames().getDnsNames();
    for (Pegasus::Uint32 i = 0; i < dnsNames.size(); ++i) {
        if (matchPattern(dnsNames[i], hostname))
            return true;
    }

    // Verify against IP addresses
    Pegasus::Array<Pegasus::String> ipAddresses = ci.getSubjectAltNames().getIpAddresses();
    for (Pegasus::Uint32 i = 0; i < ipAddresses.size(); ++i) {
        if (matchPattern(ipAddresses[i], hostname))
            return true;
    }

    // Verify against Common Name
    return matchPattern(ci.getSubjectCommonName(), hostname);
}
#else
Pegasus::Boolean CIMXMLClient::verifyCertificate(Pegasus::SSLCertificateInfo &ci)
{
    return ci.getResponseCode();
}
#endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
