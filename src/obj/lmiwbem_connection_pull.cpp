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
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include "obj/lmiwbem_config.h"
#include "obj/lmiwbem_connection.h"
#include "obj/lmiwbem_connection_pydoc.h"
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_enum_ctx.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

namespace {

boost::shared_ptr<Pegasus::CIMEnumerationContext> make_enumeration_ctx()
{
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        new Pegasus::CIMEnumerationContext);
    if (!ctx_ptr)
        throw_MemoryError("Can't create CIMEnumerationContext");
    return ctx_ptr;
}

} // unnamed namespace

void WBEMConnection::init_type_pull(WBEMConnection::WBEMConnectionClass &cls)
{
    cls.def("OpenEnumerateInstances", &WBEMConnection::openEnumerateInstances,
        (bp::arg("ClassName"),
         bp::arg("namespace") = None,
         bp::arg("LocalOnly") = true,
         bp::arg("DeepInheritance") = true,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenEnumerateInstances)
    .def("OpenEnumerateInstanceNames", &WBEMConnection::openEnumerateInstanceNames,
        (bp::arg("ClassName"),
         bp::arg("namespace") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenEnumerateInstanceNames)
    .def("OpenAssociators", &WBEMConnection::openAssociators,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = None,
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenAssociators)
    .def("OpenAssociatorNames", &WBEMConnection::openAssociatorNames,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = None,
         bp::arg("AssocClass") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("ResultRole") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenAssociatorNames)
    .def("OpenReferences", &WBEMConnection::openReferences,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("IncludeQualifiers") = false,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenReferences)
    .def("OpenReferenceNames", &WBEMConnection::openReferenceNames,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = None,
         bp::arg("ResultClass") = None,
         bp::arg("Role") = None,
         bp::arg("QueryLanguage") = None,
         bp::arg("Query") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenReferenceNames)
    .def("OpenExecQuery", &WBEMConnection::openExecQuery,
        (bp::arg("QueryLanguage"),
         bp::arg("Query"),
         bp::arg("namespace") = None,
         bp::arg("OperationTimeout") = None,
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_OpenExecQuery)
    .def("PullInstances", &WBEMConnection::pullInstances,
        (bp::arg("Context"),
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_PullInstances)
    .def("PullInstanceNames", &WBEMConnection::pullInstanceNames,
        (bp::arg("Context"),
         bp::arg("MaxObjectCnt") = 0),
        docstr_WBEMConnection_PullInstanceNames)
    .def("CloseEnumeration", &WBEMConnection::closeEnumeration,
        (bp::arg("Context")),
        docstr_WBEMConnection_CloseEnumeration);
}

bp::object WBEMConnection::openEnumerateInstances(
    const bp::object &cls,
    const bp::object &ns,
    const bp::object &local_only,          // Unused
    const bp::object &deep_inheritance,
    const bp::object &include_qualifiers,  // Unused
    const bp::object &include_class_origin,
    const bp::object &property_list,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    String c_cls(StringConv::asString(cls, "ClassName"));
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    Pegasus::CIMName peg_cls(c_cls);
    Pegasus::CIMNamespaceName peg_ns(c_ns);

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_deep_inheritance = Conv::as<Pegasus::Boolean>(
        deep_inheritance, "DeepInheritance");
    Pegasus::Boolean peg_include_class_origin = Conv::as<Pegasus::Boolean>(
        include_class_origin, "IncludeClassOrigin");
    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));
    Pegasus::Uint32Arg peg_operation_timeout;
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Array<Pegasus::CIMInstance> peg_instances;
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_instances = client()->openEnumerateInstances(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_cls,
        peg_deep_inheritance,
        peg_include_class_origin,
        peg_property_list,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(peg_instances),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenEnumerateInstances(";
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

bp::object WBEMConnection::openEnumerateInstanceNames(
    const bp::object &cls,
    const bp::object &ns,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    String c_cls(StringConv::asString(cls, "ClassName"));
    String c_ns(m_default_namespace);
    if (!isnone(ns))
        c_ns = StringConv::asString(ns, "namespace");
    Pegasus::CIMName peg_cls(c_cls);
    Pegasus::CIMNamespaceName peg_ns(c_ns);

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Array<Pegasus::CIMObjectPath> peg_instance_names;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_instance_names = client()->openEnumerateInstancePaths(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_cls,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceNameList(peg_instance_names),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenEnumerateInstanceNames(";
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

bp::object WBEMConnection::openAssociators(
    const bp::object &object_path,
    const bp::object &ns,
    const bp::object &assoc_class,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &result_role,
    const bp::object &include_qualifiers,
    const bp::object &include_class_origin,
    const bp::object &property_list,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    const CIMInstanceName &inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = inst_name.asPegasusCIMObjectPath();

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        peg_ns = peg_path.getNameSpace().getString();

    Pegasus::CIMName peg_assoc_class;
    if (!isnone(assoc_class))
        peg_assoc_class = StringConv::asString(assoc_class, "AssocClass");

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class))
        peg_result_class = StringConv::asString(result_class, "ResultClass");

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    String c_result_role;
    if (!isnone(result_role))
        c_result_role = StringConv::asString(result_role, "ResultRole");

    Pegasus::Boolean peg_include_class_origin = \
        Conv::as<Pegasus::Boolean>(include_class_origin, "IncludeClassOrigin");
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Array<Pegasus::CIMInstance> peg_associators;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_associators = client()->openAssociatorInstances(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_path,
        peg_assoc_class,
        peg_result_class,
        c_role,
        c_result_role,
        peg_include_class_origin,
        peg_property_list,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(peg_associators),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenAssociators(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::openAssociatorNames(
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
    const bp::object &max_object_cnt) try
{
    const CIMInstanceName &inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = inst_name.asPegasusCIMObjectPath();

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        peg_ns = peg_path.getNameSpace().getString();

    Pegasus::CIMName peg_assoc_class;
    if (!isnone(assoc_class))
        peg_assoc_class = StringConv::asString(assoc_class, "AssocClass");

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class))
        peg_result_class = StringConv::asString(result_class, "ResultClass");

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    String c_result_role;
    if (!isnone(result_role))
        c_result_role = StringConv::asString(result_role, "ResultRole");

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Array<Pegasus::CIMObjectPath> peg_associator_names;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_associator_names = client()->openAssociatorInstancePaths(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_path,
        peg_assoc_class,
        peg_result_class,
        c_role,
        c_result_role,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceNameList(peg_associator_names),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenAssociatorNames(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::openReferences(
    const bp::object &object_path,
    const bp::object &ns,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &include_qualifiers,
    const bp::object &include_class_origin,
    const bp::object &property_list,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    const CIMInstanceName &inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = inst_name.asPegasusCIMObjectPath();

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        peg_ns = peg_path.getNameSpace().getString();

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class))
        peg_result_class = StringConv::asString(result_class, "ResultClass");

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    Pegasus::Boolean peg_include_class_origin = \
        Conv::as<Pegasus::Boolean>(include_class_origin, "IncludeClassOrigin");
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Array<Pegasus::CIMInstance> peg_references;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_references = client()->openReferenceInstances(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_path,
        peg_result_class,
        c_role,
        peg_include_class_origin,
        peg_property_list,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(peg_references),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenReferences(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::openReferenceNames(
    const bp::object &object_path,
    const bp::object &ns,
    const bp::object &result_class,
    const bp::object &role,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    const CIMInstanceName &inst_name = CIMInstanceName::asNative(
        object_path, "ObjectName");
    Pegasus::CIMObjectPath peg_path = inst_name.asPegasusCIMObjectPath();

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!peg_path.getNameSpace().isNull())
        peg_ns = peg_path.getNameSpace().getString();

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class))
        peg_result_class = StringConv::asString(result_class, "ResultClass");

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asString(role, "Role");

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asString(query);

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Array<Pegasus::CIMObjectPath> peg_reference_names;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_reference_names = client()->openReferenceInstancePaths(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_path,
        peg_result_class,
        c_role,
        c_query_lang,
        c_query,
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceNameList(peg_reference_names),
        CIMEnumerationContext::create(ctx_ptr),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenReferenceNames(";
        if (Config::isVerboseMore())
            ss << '\'' << ObjectConv::asString(object_path) << '\'';
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::openExecQuery(
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &ns,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    String c_query_lang(
        StringConv::asString(query_lang, "QueryLanguage"));
    String c_query(
        StringConv::asString(query, "Query"));
    Pegasus::CIMClass c_query_cls;

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!isnone(ns))
        peg_ns = StringConv::asString(ns, "namespace");

    Pegasus::Uint32Arg peg_operation_timeout;
    if (!isnone(operation_timeout)) {
        peg_operation_timeout.setValue(Conv::as<Pegasus::Uint32>(
            operation_timeout, "OperationTimeout"));
    }

    Pegasus::Boolean peg_continue_on_error = Conv::as<Pegasus::Boolean>(
        continue_on_error, "ContinueOnError");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Array<Pegasus::CIMInstance> peg_instances;
    boost::shared_ptr<Pegasus::CIMEnumerationContext> ctx_ptr(
        make_enumeration_ctx());
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_instances = client()->openQueryInstances(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        c_query_lang,
        c_query,
        c_query_cls, // Unused: queryResultClass
        false,         // Unused: returnQueryResultClass
        peg_operation_timeout,
        peg_continue_on_error,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(
            peg_instances,
            peg_ns.getString(),
            client()->getHostname()),
        CIMEnumerationContext::create(
            ctx_ptr,
            false, /* with_paths */
            peg_ns.getString()),
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "OpenExecQuery(";
        if (Config::isVerboseMore()) {
            ss << "lang=u'"  << StringConv::asString(query_lang) << "', "
               << "query=u'" << StringConv::asString(query) << '\'';
        }
        ss << ')';
    }
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::pullInstances(
    const bp::object &ctx,
    const bp::object &max_object_cnt) try
{
    CIMEnumerationContext &ctx_ = CIMEnumerationContext::asNative(ctx, "Context");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCount");

    Pegasus::Array<Pegasus::CIMInstance> peg_instances;
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    if (ctx_.getIsWithPaths()) {
        peg_instances = client()->pullInstancesWithPath(
            ctx_.getPegasusContext(),
            peg_end_of_sequence,
            peg_max_object_cnt);
    } else {
        peg_instances = client()->pullInstances(
            ctx_.getPegasusContext(),
            peg_end_of_sequence,
            peg_max_object_cnt);
    }
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(
            peg_instances,
            ctx_.getNamespace(),
            client()->getHostname()),
        ctx,
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose())
        ss << "PullInstances()";
    handle_all_exceptions(ss);
    return None;
}

bp::object WBEMConnection::pullInstanceNames(
    bp::object &ctx,
    bp::object &max_object_cnt) try
{
    CIMEnumerationContext &ctx_ = CIMEnumerationContext::asNative(ctx, "Context");
    Pegasus::Uint32 peg_max_object_cnt = Conv::as<Pegasus::Uint32>(
        max_object_cnt, "MaxObjectCnt");

    Pegasus::Array<Pegasus::CIMObjectPath> peg_instance_names;
    Pegasus::Boolean peg_end_of_sequence;

    ScopedTransactionBegin();
    peg_instance_names = client()->pullInstancePaths(
        ctx_.getPegasusContext(),
        peg_end_of_sequence,
        peg_max_object_cnt);
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceNameList(peg_instance_names),
        ctx,
        bp::object(peg_end_of_sequence));
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose())
        ss << "PullInstanceNames()";
    handle_all_exceptions(ss);
    return None;
}

void WBEMConnection::closeEnumeration(const bp::object &ctx) try
{
    CIMEnumerationContext &ctx_ = CIMEnumerationContext::asNative(ctx, "Context");
    ScopedTransactionBegin();
    client()->closeEnumeration(ctx_.getPegasusContext());
    ScopedTransactionEnd();
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "CloseEnumeration()";
    }
    handle_all_exceptions(ss);
}
