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
#include <boost/python/scope.hpp>
#include "obj/cim/lmiwbem_constants.h"
#include "util/lmiwbem_util.h"

namespace bp = boost::python;

void CIMConstants::init_type()
{
    bp::scope().attr("CIM_ERR_FAILED") = CIM_ERR_FAILED;
    bp::scope().attr("CIM_ERR_ACCESS_DENIED") =  CIM_ERR_ACCESS_DENIED;
    bp::scope().attr("CIM_ERR_INVALID_NAMESPACE") =  CIM_ERR_INVALID_NAMESPACE;
    bp::scope().attr("CIM_ERR_INVALID_PARAMETER") =  CIM_ERR_INVALID_PARAMETER;
    bp::scope().attr("CIM_ERR_INVALID_CLASS") =  CIM_ERR_INVALID_CLASS;
    bp::scope().attr("CIM_ERR_NOT_FOUND") =  CIM_ERR_NOT_FOUND;
    bp::scope().attr("CIM_ERR_NOT_SUPPORTED") =  CIM_ERR_NOT_SUPPORTED;
    bp::scope().attr("CIM_ERR_CLASS_HAS_CHILDREN") =  CIM_ERR_CLASS_HAS_CHILDREN;
    bp::scope().attr("CIM_ERR_CLASS_HAS_INSTANCES") =  CIM_ERR_CLASS_HAS_INSTANCES;
    bp::scope().attr("CIM_ERR_INVALID_SUPERCLASS") =  CIM_ERR_INVALID_SUPERCLASS;
    bp::scope().attr("CIM_ERR_ALREADY_EXISTS") =  CIM_ERR_ALREADY_EXISTS;
    bp::scope().attr("CIM_ERR_NO_SUCH_PROPERTY") =  CIM_ERR_NO_SUCH_PROPERTY;
    bp::scope().attr("CIM_ERR_TYPE_MISMATCH") =  CIM_ERR_TYPE_MISMATCH;
    bp::scope().attr("CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED") =  CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED;
    bp::scope().attr("CIM_ERR_INVALID_QUERY") =  CIM_ERR_INVALID_QUERY;
    bp::scope().attr("CIM_ERR_METHOD_NOT_AVAILABLE") =  CIM_ERR_METHOD_NOT_AVAILABLE;
    bp::scope().attr("CIM_ERR_METHOD_NOT_FOUND") =  CIM_ERR_METHOD_NOT_FOUND;

#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    bp::scope().attr("CIM_ERR_NAMESPACE_NOT_EMPTY") = CIM_ERR_NAMESPACE_NOT_EMPTY;
    bp::scope().attr("CIM_ERR_INVALID_ENUMERATION_CONTEXT") = CIM_ERR_INVALID_ENUMERATION_CONTEXT;
    bp::scope().attr("CIM_ERR_INVALID_OPERATION_TIMEOUT") = CIM_ERR_INVALID_OPERATION_TIMEOUT;
    bp::scope().attr("CIM_ERR_PULL_HAS_BEEN_ABANDONED") = CIM_ERR_PULL_HAS_BEEN_ABANDONED;
    bp::scope().attr("CIM_ERR_PULL_CANNOT_BE_ABANDONED") = CIM_ERR_PULL_CANNOT_BE_ABANDONED;
    bp::scope().attr("CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED") = CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED;
    bp::scope().attr("CIM_ERR_CONTINUATION_ON_ERROR_NOT_SUPPORTED") = CIM_ERR_CONTINUATION_ON_ERROR_NOT_SUPPORTED;
    bp::scope().attr("CIM_ERR_SERVER_LIMITS_EXCEEDED") = CIM_ERR_SERVER_LIMITS_EXCEEDED;
    bp::scope().attr("CIM_ERR_SERVER_IS_SHUTTING_DOWN") = CIM_ERR_SERVER_IS_SHUTTING_DOWN;
#endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

    bp::scope().attr("CON_ERR_OTHER") = CON_ERR_OTHER;
    bp::scope().attr("CON_ERR_ALREADY_CONNECTED") = CON_ERR_ALREADY_CONNECTED;
    bp::scope().attr("CON_ERR_NOT_CONNECTED") = CON_ERR_NOT_CONNECTED;
    bp::scope().attr("CON_ERR_INVALID_LOCATOR") = CON_ERR_INVALID_LOCATOR;
    bp::scope().attr("CON_ERR_CANNOT_CREATE_SOCKET") = CON_ERR_CANNOT_CREATE_SOCKET;
    bp::scope().attr("CON_ERR_CANNOT_CONNECT") = CON_ERR_CANNOT_CONNECT;
    bp::scope().attr("CON_ERR_CONNECTION_TIMEOUT") = CON_ERR_CONNECTION_TIMEOUT;
    bp::scope().attr("CON_ERR_SSL_EXCEPTION") = CON_ERR_SSL_EXCEPTION;
    bp::scope().attr("CON_ERR_BIND") = CON_ERR_BIND;

#ifdef HAVE_SLP
    bp::scope().attr("SLP_ERR_LANGUAGE_NOT_SUPPORTED") = SLP_ERR_LANGUAGE_NOT_SUPPORTED;
    bp::scope().attr("SLP_ERR_PARSE_ERROR") = SLP_ERR_PARSE_ERROR;
    bp::scope().attr("SLP_ERR_INVALID_REGISTRATION") = SLP_ERR_INVALID_REGISTRATION;
    bp::scope().attr("SLP_ERR_SCOPE_NOT_SUPPORTED") = SLP_ERR_SCOPE_NOT_SUPPORTED;
    bp::scope().attr("SLP_ERR_AUTHENTICATION_ABSENT") = SLP_ERR_AUTHENTICATION_ABSENT;
    bp::scope().attr("SLP_ERR_AUTHENTICATION_FAILED") = SLP_ERR_AUTHENTICATION_FAILED;
    bp::scope().attr("SLP_ERR_INVALID_UPDATE") = SLP_ERR_INVALID_UPDATE;
    bp::scope().attr("SLP_ERR_REFRESH_REJECTED") = SLP_ERR_REFRESH_REJECTED;
    bp::scope().attr("SLP_ERR_NOT_IMPLEMENTED") = SLP_ERR_NOT_IMPLEMENTED;
    bp::scope().attr("SLP_ERR_BUFFER_OVERFLOW") = SLP_ERR_BUFFER_OVERFLOW;
    bp::scope().attr("SLP_ERR_NETWORK_TIMED_OUT") = SLP_ERR_NETWORK_TIMED_OUT;
    bp::scope().attr("SLP_ERR_NETWORK_INIT_FAILED") = SLP_ERR_NETWORK_INIT_FAILED;
    bp::scope().attr("SLP_ERR_MEMORY_ALLOC_FAILED") = SLP_ERR_MEMORY_ALLOC_FAILED;
    bp::scope().attr("SLP_ERR_PARAMETER_BAD") = SLP_ERR_PARAMETER_BAD;
    bp::scope().attr("SLP_ERR_NETWORK_ERROR") = SLP_ERR_NETWORK_ERROR;
    bp::scope().attr("SLP_ERR_INTERNAL_SYSTEM_ERROR") = SLP_ERR_INTERNAL_SYSTEM_ERROR;
    bp::scope().attr("SLP_ERR_HANDLE_IN_USE") = SLP_ERR_HANDLE_IN_USE;
    bp::scope().attr("SLP_ERR_TYPE_ERROR") = SLP_ERR_TYPE_ERROR;
#  if !defined(UNICAST_NOT_SUPPORTED) && defined(SLP_RETRY_UNICAST)
    bp::scope().attr("SLP_ERR_RETRY_UNICAST") = SLP_ERR_RETRY_UNICAST;
#  endif // UNICAST_NOT_SUPPORTED
#endif // HAVE_SLP
}
