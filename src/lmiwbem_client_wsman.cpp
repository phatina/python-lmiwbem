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
#include <vector>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/CIMValue.h>
#include <boost/property_tree/exceptions.hpp>
#include <openwsman/wsman-names.h>
#include <openwsman/cpp/OpenWsmanClient.h>
#include <openwsman/cpp/WsmanEPR.h>
#include "lmiwbem_client_wsman.h"
#include "lmiwbem_client_wsman_builder.h"
#include "lmiwbem_client_wsman_request.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_urlinfo.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_string.h"

typedef std::vector<std::string> EnumerationResult;

using namespace WsmanClientNamespace;

namespace ptree = boost::property_tree;

namespace {

void handle_wsman_exceptions()
{
    try {
        // Re-throw and convert to Pegasus's exceptions.
        throw;
    } catch (WsmanResourceNotFound &e) {
        throw Pegasus::CIMException(
            Pegasus::CIM_ERR_NOT_FOUND,
            e.getDetail().c_str());
    } catch (WsmanSoapFault &e) {
        throw WsmanException(e.GetFaultReason());
    } catch (WsmanClientException &e) {
        throw WsmanException(e.getDetail());
    }
}

void handle_xml_exceptions()
{
    try {
        // Re-throw and convert to Pegasus's exceptions.
        throw;
    } catch (const ptree::ptree_error &e) {
        throw WsmanException(e.what());
    }
}

Pegasus::Array<Pegasus::CIMObject> instances_to_objects(
    const Pegasus::Array<Pegasus::CIMInstance> &instances)
{
    const Pegasus::Uint32 cnt = instances.size();
    Pegasus::Array<Pegasus::CIMObject> objects(cnt);
    for (Pegasus::Uint32 i = 0; i < cnt; ++i)
        objects[i] = instances[i];

    return objects;
}

WsmanEPR pegasusObjectPathToEPR(const Pegasus::CIMObjectPath &path)
{
    Request request(
        path.getHost(),
        path.getNameSpace().getString(),
        path.getClassName().getString());

    WsmanEPR epr(request.asString(), WSA_TO_ANONYMOUS);

    const Pegasus::Array<Pegasus::CIMKeyBinding> &keybindings = path.getKeyBindings();
    const Pegasus::Uint32 cnt = keybindings.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        const Pegasus::CIMKeyBinding &keybinding = keybindings[i];
        epr.addTextSelector(
            String(keybinding.getName().getString()),
            String(keybinding.getValue()));
    }

    return epr;
}

} // unnamed namespace

WSMANClient::WSMANClient()
    : m_client()
{
}

WSMANClient::~WSMANClient()
{
}

void WSMANClient::connect(
    const String &uri,
    const String &username,
    const String &password,
    const String &cert_file,
    const String &key_file,
    const String &trust_store)
{
    URLInfo url_info(uri);
    m_client.reset(
        new WsmanClientNamespace::OpenWsmanClient(
            url_info.hostname().c_str(),
            static_cast<const int>(url_info.port()),
            url_info.path().c_str(),
            url_info.scheme().c_str(),
            "Basic", // Authentication method
            username.c_str(),
            password.c_str()));

    m_client->SetClientCert(cert_file, key_file);
}

void WSMANClient::connectLocally()
{
    throw NotSupportedException("Local connection is not supported");
}

void WSMANClient::disconnect()
{
    m_client.reset();
}

// ------------------------------------------------------------------------
// CIM API
//
void WSMANClient::setTimeout(Pegasus::Uint32 timeoutMilliseconds)
{
}

void WSMANClient::setRequestAcceptLanguages(const Pegasus::AcceptLanguageList& langs)
{
}

Pegasus::Uint32 WSMANClient::getTimeout() const
{
    return 0;
}

Pegasus::AcceptLanguageList WSMANClient::getRequestAcceptLanguages() const
{
    return Pegasus::AcceptLanguageList();
}

Pegasus::CIMObjectPath WSMANClient::createInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMInstance &newInstance)
{
    throw NotSupportedException("CreateInstance() not supported");
}

void WSMANClient::deleteInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName)
{
    throw NotSupportedException("DeleteInstance() not supported");
}

void WSMANClient::modifyInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMInstance &modifiedInstance,
    Pegasus::Boolean includeQualifiers,
    const Pegasus::CIMPropertyList &propertyList)
{
    throw NotSupportedException("ModifyInstance() not supported");
}

Pegasus::Array<Pegasus::CIMClass> WSMANClient::enumerateClasses(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin)
{
    UNUSED(nameSpace);
    UNUSED(deepInheritance);
    UNUSED(localOnly);
    UNUSED(includeQualifiers);
    UNUSED(includeClassOrigin);

    throw NotSupportedException("EnumerateClasses() not supported");
}

Pegasus::Array<Pegasus::CIMName> WSMANClient::enumerateClassNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance)
{
    UNUSED(nameSpace);
    UNUSED(className);
    UNUSED(deepInheritance);

    throw NotSupportedException("EnumerateClassNames() not supported");
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::enumerateInstances(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className,
    Pegasus::Boolean deepInheritance,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    UNUSED(localOnly);
    UNUSED(includeQualifiers);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);

    Request request(
        getHostname(),
        nameSpace.getString(),
        className.getString());

    WsmanOptions options(FLAG_ENUMERATION_ENUM_OBJ_AND_EPR);
    if (deepInheritance)
        options.addFlag(FLAG_EXCLUDESUBCLASSPROPERTIES);
    else
        options.addFlag(FLAG_INCLUDESUBCLASSPROPERTIES);

    return enumerateInstancesWithPath(request, options);
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::enumerateInstanceNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName &className)
{
    Request request(
        getHostname(),
        nameSpace.getString(),
        className.getString());

    WsmanOptions options(FLAG_ENUMERATION_ENUM_EPR);

    return enumerateInstanceNames(request, options);
}

Pegasus::Array<Pegasus::CIMObject> WSMANClient::execQuery(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::String &queryLanguage,
    const Pegasus::String &query)
{
    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(queryLanguage),
        String(query));

    if (!request.isValid()) {
        throw Pegasus::CIMException(
            Pegasus::CIM_ERR_INVALID_QUERY,
            "Invalid query");
    }

    WsmanOptions options(FLAG_ENUMERATION_ENUM_OBJ_AND_EPR);
    WsmanFilter filter(
        request.getQueryDialect(),
        request.getQuery());

    return instances_to_objects(
        enumerateInstancesWithPath(request, options, filter));
}

Pegasus::CIMClass WSMANClient::getClass(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMName& className,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    UNUSED(nameSpace);
    UNUSED(className);
    UNUSED(localOnly);
    UNUSED(includeQualifiers);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);

    return Pegasus::CIMClass();
}

Pegasus::CIMInstance WSMANClient::getInstance(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName,
    Pegasus::Boolean localOnly,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(instanceName.getClassName().getString()));

    const Pegasus::Array<Pegasus::CIMKeyBinding> &keybindings = instanceName.getKeyBindings();
    const Pegasus::Uint32 cnt = keybindings.size();
    WsmanOptions options;
    options.setNamespace(request.getNamespace());
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        options.addSelector(
            String(keybindings[i].getName().getString()),
            String(keybindings[i].getValue()));
    }

    String instance;
    try {
        instance = m_client->Get(request, options);
    } catch (...) {
        handle_wsman_exceptions();
    }

    try {
        Pegasus::CIMInstance peg_instance(ObjectFactory::makeCIMInstance(
            instance));
        peg_instance.setPath(instanceName);
        return peg_instance;
    } catch (...) {
        handle_xml_exceptions();
    }

    return Pegasus::CIMInstance();
}

Pegasus::CIMValue WSMANClient::invokeMethod(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &instanceName,
    const Pegasus::CIMName &methodName,
    const Pegasus::Array<Pegasus::CIMParamValue> &inParameters,
    Pegasus::Array<Pegasus::CIMParamValue> &outParameters)
{
    Request request(
        getHostname(),
        String(instanceName.getNameSpace().getString()),
        String(instanceName.getClassName().getString()));

    const Pegasus::Array<Pegasus::CIMKeyBinding> &keybindings = instanceName.getKeyBindings();
    Pegasus::Uint32 cnt = keybindings.size();
    WsmanOptions options;
    options.setNamespace(request.getNamespace());
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        options.addSelector(
            String(keybindings[i].getName().getString()),
            String(keybindings[i].getValue()));
    }

    cnt = inParameters.size();
    for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
        const Pegasus::CIMParamValue &param = inParameters[i];
        try {
            options.addProperty(
                String(param.getParameterName()),
                CIMValue::asString(param.getValue()));
        } catch (const Pegasus::TypeMismatchException &e) {
            // We got a unsupported value type
            continue;
        }
    }

    String method_name(methodName.getString());
    String rval;
    try {
        rval = m_client->Invoke(
            request,
            method_name,
            options);
    } catch (...) {
        handle_wsman_exceptions();
    }

    try {
        return ObjectFactory::makeMethodReturnValue(rval, method_name, outParameters);
    } catch (...) {
        handle_xml_exceptions();
    }

    return Pegasus::CIMValue();
}

Pegasus::Array<Pegasus::CIMObject> WSMANClient::associators(
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
    UNUSED(includeQualifiers);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);

    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(objectName.getClassName().getString()),
        true);

    WsmanOptions options(FLAG_ENUMERATION_ENUM_OBJ_AND_EPR);

    WsmanFilter filter(
        pegasusObjectPathToEPR(objectName),
        WSMAN_ASSOCIATOR,
        String(assocClass.getString()),
        String(resultClass.getString()),
        String(role),
        String(resultRole));

    return instances_to_objects(
        enumerateInstancesWithPath(request, options, filter));
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::associatorNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &assocClass,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    const Pegasus::String &resultRole)
{
    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(objectName.getClassName().getString()),
        true);

    WsmanOptions options(FLAG_ENUMERATION_ENUM_EPR);

    WsmanFilter filter(
        pegasusObjectPathToEPR(objectName),
        WSMAN_ASSOCIATOR,
        String(assocClass.getString()),
        String(resultClass.getString()),
        String(role),
        String(resultRole));

    return enumerateInstanceNames(request, options, filter);
}

Pegasus::Array<Pegasus::CIMObject> WSMANClient::references(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role,
    Pegasus::Boolean includeQualifiers,
    Pegasus::Boolean includeClassOrigin,
    const Pegasus::CIMPropertyList &propertyList)
{
    UNUSED(includeQualifiers);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);

    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(objectName.getClassName().getString()),
        true);

    WsmanOptions options(FLAG_ENUMERATION_ENUM_OBJ_AND_EPR);

    WsmanFilter filter(
        pegasusObjectPathToEPR(objectName),
        WSMAN_ASSOCIATED,
        String(), // AssocClass
        String(resultClass.getString()),
        String(role),
        String()); // ResultRole

    return instances_to_objects(
        enumerateInstancesWithPath(request, options, filter));
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::referenceNames(
    const Pegasus::CIMNamespaceName &nameSpace,
    const Pegasus::CIMObjectPath &objectName,
    const Pegasus::CIMName &resultClass,
    const Pegasus::String &role)
{
    Request request(
        getHostname(),
        String(nameSpace.getString()),
        String(objectName.getClassName().getString()),
        true);

    WsmanOptions options(FLAG_ENUMERATION_ENUM_EPR);

    WsmanFilter filter(
        pegasusObjectPathToEPR(objectName),
        WSMAN_ASSOCIATED,
        String(), // AssocClass
        String(resultClass.getString()),
        String(role),
        String()); // ResultRole

    return enumerateInstanceNames(request, options, filter);
}

#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
Pegasus::Array<Pegasus::CIMInstance> WSMANClient::openEnumerateInstances(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(className);
    UNUSED(deepInheritance);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenEnumerateInstances() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::openEnumerateInstancePaths(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(className);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenEnumerateInstanceNames() not supported");

    return Pegasus::Array<Pegasus::CIMObjectPath>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::openReferenceInstances(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(objectName);
    UNUSED(resultClass);
    UNUSED(role);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenReferences() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::openReferenceInstancePaths(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(objectName);
    UNUSED(resultClass);
    UNUSED(role);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenReferenceNames() not supported");

    return Pegasus::Array<Pegasus::CIMObjectPath>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::openAssociatorInstances(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(objectName);
    UNUSED(assocClass);
    UNUSED(resultClass);
    UNUSED(role);
    UNUSED(resultRole);
    UNUSED(includeClassOrigin);
    UNUSED(propertyList);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenAssociators() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::openAssociatorInstancePaths(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(objectName);
    UNUSED(assocClass);
    UNUSED(resultClass);
    UNUSED(role);
    UNUSED(resultRole);
    UNUSED(filterQueryLanguage);
    UNUSED(filterQuery);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenAssociatorNames() not supported");

    return Pegasus::Array<Pegasus::CIMObjectPath>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::openQueryInstances(
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
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(nameSpace);
    UNUSED(queryLanguage);
    UNUSED(query);
    UNUSED(queryResultClass);
    UNUSED(returnQueryResultClass);
    UNUSED(operationTimeout);
    UNUSED(continueOnError);
    UNUSED(maxObjectCount);

    throw NotSupportedException("OpenExecQuery() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::pullInstancesWithPath(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(maxObjectCount);

    throw NotSupportedException("PullInstances() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::pullInstancePaths(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(maxObjectCount);

    throw NotSupportedException("PullInstanceNames() not supported");

    return Pegasus::Array<Pegasus::CIMObjectPath>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::pullInstances(
    Pegasus::CIMEnumerationContext &enumerationContext,
    Pegasus::Boolean &endOfSequence,
    Pegasus::Uint32 maxObjectCount)
{
    UNUSED(enumerationContext);
    UNUSED(endOfSequence);
    UNUSED(maxObjectCount);

    throw NotSupportedException("PullInstances() not supported");

    return Pegasus::Array<Pegasus::CIMInstance>();
}

void WSMANClient::closeEnumeration(
    Pegasus::CIMEnumerationContext &enumerationContext)
{
    throw NotSupportedException("CloseEnumeration() not supported");
}

Pegasus::Uint64Arg WSMANClient::enumerationCount(
    Pegasus::CIMEnumerationContext &enumerationContext)
{
    throw NotSupportedException("EnumerationCount() not supported");

    return Pegasus::Uint64Arg();
}
#endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::enumerateInstanceNames(
    const Request &request,
    const WsmanClientNamespace::WsmanOptions &options)
{
    return enumerateInstanceNames(request, options, WsmanFilter());
}

Pegasus::Array<Pegasus::CIMObjectPath> WSMANClient::enumerateInstanceNames(
    const Request &request,
    const WsmanClientNamespace::WsmanOptions &options,
    const WsmanClientNamespace::WsmanFilter &filter)
{
    EnumerationResult enum_result;

    try {
        m_client->Enumerate(request, enum_result, options, filter);
    } catch (...) {
        handle_wsman_exceptions();
    }

    try {
        return ListConv::asPegasusCIMInstanceNameList(
            enum_result,
            request.getHostname(),
            request.getNamespace());
    } catch (...) {
        handle_xml_exceptions();
    }

    return Pegasus::Array<Pegasus::CIMObjectPath>();
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::enumerateInstancesWithPath(
    const Request &request,
    const WsmanClientNamespace::WsmanOptions &options)
{
    return enumerateInstancesWithPath(request, options, WsmanFilter());
}

Pegasus::Array<Pegasus::CIMInstance> WSMANClient::enumerateInstancesWithPath(
    const Request &request,
    const WsmanClientNamespace::WsmanOptions &options,
    const WsmanClientNamespace::WsmanFilter &filter)
{
    EnumerationResult enum_result;

    try {
        m_client->Enumerate(request, enum_result, options, filter);
    } catch (...) {
        handle_wsman_exceptions();
    }

    try {
        return ListConv::asPegasusCIMInstancesWithPathList(
            enum_result,
            request.getHostname(),
            request.getNamespace());
    } catch (...) {
        handle_xml_exceptions();
    }

    return Pegasus::Array<Pegasus::CIMInstance>();
}
