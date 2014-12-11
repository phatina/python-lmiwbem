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

#include <config.h>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include "lmiwbem_config.h"
#include "obj/lmiwbem_connection.h"
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
         bp::arg("namespace") = bp::object(),
         bp::arg("DeepInheritance") = true,
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenEnumerateInstances(ClassName, namespace=None, "
        "DeepInheritance=True, IncludeQualifiers=False, IncludeClassOrigin=False, "
        "PropertyList=None, QueryLang=None, Query=None, "
        "OperationTimeout=None, ContinueOnError=False, MaxObjectCnt=0)\n\n"
        "Opens an enumeration sequence of :py:class:`.CIMInstance`.\n\n"
        ":param str ClassName: String containing class name of instances to be "
        "retrieved.\n"
        ":param str namespace: String containing namespace, from which the "
        "instances should be retrieved.\n"
        ":param bool DeepInheritance: Boolean indicating whether the "
        "enumeration should include all levels of derivation.\n"
        ":param bool IncludeClassOrigin: Boolean indicating whether the "
        "`CLASS ORIGIN` attribute is to be included in elements of the "
        "returned instance.\n"
        ":param list PropertyList: List of properties available in returned "
        "instances.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return instances subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved :py:class:`.CIMInstance` "
        "objects, enumeration context and boolean which defines if all the instances"
        "have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenEnumerateInstanceNames", &WBEMConnection::openEnumerateInstanceNames,
        (bp::arg("ClassName"),
         bp::arg("namespace") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenEnumerateInstanceNames(ClassName, namespace=None, "
        "QueryLanguage=None, Query=None, OperationTimeout=None, "
        "ContinueOnError=False, MaxObjectCnt=0)\n\n"
        "Opens an enumeration sequence of :py:class:`.CIMInstanceName`.\n\n"
        ":param str ClassName: String containing class name of elements to be "
        "retrieved.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstanceName` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenAssociators", &WBEMConnection::openAssociators,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = bp::object(),
         bp::arg("AssocClass") = bp::object(),
         bp::arg("ResultClass") = bp::object(),
         bp::arg("Role") = bp::object(),
         bp::arg("ResultRole") = bp::object(),
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenAssociators(ObjectName, namespace=None, AssocClass=None, "
        "ResultClass=None, Role=None, ResultRole=None, "
        "IncludeClassOrigin=false, PropertyList=None, QueryLanguage=None, "
        "Query=None, OperationTimeout=None, ContinueOnError=False, "
        "MaxObjectCnt=0)\n\n"
        "Opens an enumeration for associated :py:class:`.CIMInstance` objects "
        "with an input ObjectName.\n\n"
        ":param CIMInstanceName ObjectName: Object path that is the basis "
        "for the enumeration.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param str AssocClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str ResultClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str Role: Defines a filter on the roles of associated "
        "instances to be returned.\n"
        ":param str ResultRole: Defines a filter on the result roles of "
        "associated instances.\n"
        ":param bool IncludeClassOrigin: Boolean indicating whether the "
        "`CLASS ORIGIN` attribute is to be included in elements of the "
        "returned instance.\n"
        ":param list PropertyList: List of properties available in returned "
        "instances.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstance` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenAssociatorNames", &WBEMConnection::openAssociatorNames,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = bp::object(),
         bp::arg("AssocClass") = bp::object(),
         bp::arg("ResultClass") = bp::object(),
         bp::arg("Role") = bp::object(),
         bp::arg("ResultRole") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenAssociatorNames(ObjectName, namespace=None, AssocClass=None, "
        "ResultClass=None, Role=None, ResultRole=None, QueryLanguage=None, "
        "Query=None, OperationTimeout=None, ContinueOnError=False, "
        "MaxObjectCnt=0)\n\n"
        "Opens an enumeration for associated :py:class:`.CIMInstanceName` "
        "objects with an input ObjectName.\n\n"
        ":param CIMInstanceName ObjectName: Object path that is the basis "
        "for the enumeration.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param str AssocClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str ResultClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str Role: Defines a filter on the roles of associated "
        "instances to be returned.\n"
        ":param str ResultRole: Defines a filter on the result roles of "
        "associated instances to be returned.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstanceName` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenReferences", &WBEMConnection::openReferences,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = bp::object(),
         bp::arg("ResultClass") = bp::object(),
         bp::arg("Role") = bp::object(),
         bp::arg("IncludeClassOrigin") = false,
         bp::arg("PropertyList") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenReferences(ObjectName, ns=None, ResultClass=None, Role=None, "
        "IncludeClassOrigin=False, PropertyList=None, QueryLanguage=None, "
        "Query=None, OperationTimeout=None, ContinueOnError=False, "
        "MaxObjectCnt=0)\n\n"
        "Opens an enumeration for association :py:class:`.CIMInstance` "
        "objects with an input instance name.\n\n"
        ":param CIMInstanceName ObjectName: Object path that is the basis "
        "for the enumeration.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param str ResultClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str Role: Defines a filter on the roles of references "
        "instances to be returned.\n"
        ":param bool IncludeClassOrigin: Indicates whether the `CLASS ORIGIN` "
        "attribute is to be included in elements of the returned instance.\n"
        ":param list PropertyList: List of properties available in returned "
        "instances.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstance` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenReferenceNames", &WBEMConnection::openReferenceNames,
        (bp::arg("ObjectName"),
         bp::arg("namespace") = bp::object(),
         bp::arg("ResultClass") = bp::object(),
         bp::arg("Role") = bp::object(),
         bp::arg("QueryLanguage") = bp::object(),
         bp::arg("Query") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenReferenceNames(ObjectName, namespace=None, ResultClass=None, "
        "Role=None, QueryLanguage=None, Query=None, OperationTimeout=None, "
        "ContinueOnError=False, MaxObjectCnt=0)\n\n"
        "Opens an enumeration for association :py:class:`.CIMInstanceName` "
        "objects with an input instance name.\n\n"
        ":param CIMInstanceName ObjectName: Object path that is the basis "
        "for the enumeration.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param str ResultClass: Defines a filter on the reference instances "
        "set to be returned.\n"
        ":param str Role: Defines a filter on the roles of references "
        "instances to be returned.\n"
        ":param str QueryLanguage: Query language to be used with the "
        "Query.\n"
        ":param str Query: Filter query to be applied to the enumeration.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstanceName` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("OpenExecQuery", &WBEMConnection::openExecQuery,
        (bp::arg("QueryLanguage"),
         bp::arg("Query"),
         bp::arg("namespace") = bp::object(),
         bp::arg("OperationTimeout") = bp::object(),
         bp::arg("ContinueOnError") = false,
         bp::arg("MaxObjectCnt") = 0),
        "OpenExecQuery(QueryLanguage, Query, namespace=None, "
        "OperationTimeout=None, ContinueOnError=False, MaxObjectCnt=0)\n\n"
        "Opens an enumeration for Query.\n\n"
        ":param str QueryLanguage: Defines the query language in which the "
        "query parameter is expressed.\n"
        ":param str Query: Specified the query to be executed.\n"
        ":param str namespace: String containing namespace, from which the "
        "elements should be retrieved.\n"
        ":param int OperationTimeout: Defines the interoperation timeout in "
        "seconds between the response and subsequent request operations of "
        "an enumeration sequence.\n"
        ":param bool ContinueOnError: Defines whether the server should "
        "continue to return elements subsequent to a :py:exc:`.CIMError` "
        "occurring in the server.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstance` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("PullInstances", &WBEMConnection::pullInstances,
        (bp::arg("Context"),
         bp::arg("MaxObjectCnt") = 0),
        "PullInstances(Context, MaxObjectCnt=0)\n\n"
        "Retrieves a list of :py:class:`.CIMInstance` for an open enumeration "
        "sequence opened by an openQueryInstances operation and represented "
        "by an enumeration context returned by the original open or "
        "previous :py:meth:`.WBEMConnection.PullInstancesWithPath` "
        "operation.\n\n"
        ":param CIMEnumerationContext Context: Identifier for the "
        "enumeration sequence.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing a list of retrieved "
        ":py:class:`.CIMInstance` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("PullInstanceNames", &WBEMConnection::pullInstanceNames,
        (bp::arg("Context"),
         bp::arg("MaxObjectCnt") = 0),
        "PullInstanceNames(Context, MaxObjectCnt=0)\n\n"
        "Retrieves a list of :py:class:`.CIMInstanceName` for an open "
        "enumeration sequence opened by an openQueryInstances operation and "
        "represented by an enumeration context returned by the original open "
        "or previous :py:meth:`.WBEMConnection.PullInstanceNames` "
        "operation.\n\n"
        ":param CIMEnumerationContext Context: Identifier for the "
        "enumeration sequence.\n"
        ":param int MaxObjectCnt: Defines the maximum number of elements "
        "that this Open operation can return.\n"
        ":returns: Tuple containing list of retrieved "
        ":py:class:`.CIMInstanceName` objects, enumeration context and boolean "
        "which defines if all the instances have been retrieved.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`")
    .def("CloseEnumeration", &WBEMConnection::closeEnumeration,
        (bp::arg("Context")),
        "CloseEnumeration(Context)\n\n"
        "Closes an enumeration sequence.\n\n"
        ":param CIMEnumerationContext Context: Enumeration context to close.\n"
        ":raises: :py:exc:`.CIMError`, :py:exc:`.ConnectionError`");
}

bp::object WBEMConnection::openEnumerateInstances(
    const bp::object &cls,
    const bp::object &ns,
    const bp::object &deep_inheritance,
    const bp::object &include_class_origin,
    const bp::object &property_list,
    const bp::object &query_lang,
    const bp::object &query,
    const bp::object &operation_timeout,
    const bp::object &continue_on_error,
    const bp::object &max_object_cnt) try
{
    Pegasus::CIMName peg_class(StringConv::asPegasusString(cls, "ClassName"));
    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!isnone(ns))
        peg_ns = StringConv::asPegasusString(ns, "namespace");

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_instances = m_client.openEnumerateInstances(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_class,
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
            ss << "classname=" << StringConv::asString(cls) << ", "
               << "namespace=" << c_ns;
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
    Pegasus::CIMName peg_class(StringConv::asPegasusString(cls, "ClassName"));
    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!isnone(ns))
        peg_ns = StringConv::asPegasusString(ns, "namespace");

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_instance_names = m_client.openEnumerateInstancePaths(
        *ctx_ptr,
        peg_end_of_sequence,
        peg_ns,
        peg_class,
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
            ss << "classname=" << StringConv::asString(cls) << ", "
               << "namespace=" << c_ns;
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
    if (!isnone(assoc_class)) {
        peg_assoc_class = StringConv::asPegasusString(
            assoc_class, "AssocClass");
    }

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class)) {
        peg_result_class = StringConv::asPegasusString(
            result_class, "ResultClass");
    }

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asPegasusString(role, "Role");

    String c_result_role;
    if (!isnone(result_role)) {
        c_result_role = StringConv::asPegasusString(
            result_role, "ResultRole");
    }

    Pegasus::Boolean peg_include_class_origin = \
        Conv::as<Pegasus::Boolean>(include_class_origin, "IncludeClassOrigin");
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_associators = m_client.openAssociatorInstances(
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
    if (!isnone(assoc_class)) {
        peg_assoc_class = StringConv::asPegasusString(
            assoc_class, "AssocClass");
    }

    Pegasus::CIMName peg_result_class;
    if (!isnone(result_class)) {
        peg_result_class = StringConv::asPegasusString(
            result_class, "ResultClass");
    }

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asPegasusString(role, "Role");

    String c_result_role;
    if (!isnone(result_role)) {
        c_result_role = StringConv::asPegasusString(
            result_role, "ResultRole");
    }

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_associator_names = m_client.openAssociatorInstancePaths(
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
    if (!isnone(result_class)) {
        peg_result_class = StringConv::asPegasusString(
            result_class, "ResultClass");
    }

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asPegasusString(role, "Role");

    Pegasus::Boolean peg_include_class_origin = \
        Conv::as<Pegasus::Boolean>(include_class_origin, "IncludeClassOrigin");
    Pegasus::CIMPropertyList peg_property_list(
        ListConv::asPegasusPropertyList(property_list,
        "PropertyList"));

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_references = m_client.openReferenceInstances(
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
    if (!isnone(result_class)) {
        peg_result_class = StringConv::asPegasusString(
            result_class, "ResultClass");
    }

    String c_role;
    if (!isnone(role))
        c_role = StringConv::asPegasusString(role, "Role");

    String c_query_lang;
    String c_query;
    if (!isnone(query_lang))
        c_query_lang = StringConv::asPegasusString(query_lang);
    if (!isnone(query))
        c_query = StringConv::asPegasusString(query);

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
    peg_reference_names = m_client.openReferenceInstancePaths(
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
        StringConv::asPegasusString(query_lang, "QueryLanguage"));
    String c_query(
        StringConv::asPegasusString(query, "Query"));
    Pegasus::CIMClass c_query_cls;

    Pegasus::CIMNamespaceName peg_ns(m_default_namespace);
    if (!isnone(ns))
        peg_ns = StringConv::asPegasusString(ns, "namespace");

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
    peg_instances = m_client.openQueryInstances(
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
            m_client.hostname()),
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
            ss << "lang='"  << StringConv::asString(query_lang) << "', "
               << "query='" << StringConv::asString(query) << '\'';
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
        peg_instances = m_client.pullInstancesWithPath(
            ctx_.getPegasusContext(),
            peg_end_of_sequence,
            peg_max_object_cnt);
    } else {
        peg_instances = m_client.pullInstances(
            ctx_.getPegasusContext(),
            peg_end_of_sequence,
            peg_max_object_cnt);
    }
    ScopedTransactionEnd();

    return bp::make_tuple(
        ListConv::asPyCIMInstanceList(
            peg_instances,
            ctx_.getNamespace(),
            m_client.hostname()),
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
    peg_instance_names = m_client.pullInstancePaths(
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
    m_client.closeEnumeration(ctx_.getPegasusContext());
    ScopedTransactionEnd();
} catch (...) {
    std::stringstream ss;
    if (Config::isVerbose()) {
        ss << "CloseEnumeration()";
    }
    handle_all_exceptions(ss);
}
