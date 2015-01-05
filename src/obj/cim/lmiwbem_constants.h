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

#ifndef   LMIWBEM_CONSTANTS_H
#  define LMIWBEM_CONSTANTS_H

#  include <config.h>
#  ifdef HAVE_SLP
#    include <slp.h>
#  endif // HAVE_SLP

class CIMConstants
{
public:
    static void init_type();

    enum CIMError {
        // CIM error codes
          CIM_ERR_FAILED                              = 1
        , CIM_ERR_ACCESS_DENIED                       = 2
        , CIM_ERR_INVALID_NAMESPACE                   = 3
        , CIM_ERR_INVALID_PARAMETER                   = 4
        , CIM_ERR_INVALID_CLASS                       = 5
        , CIM_ERR_NOT_FOUND                           = 6
        , CIM_ERR_NOT_SUPPORTED                       = 7
        , CIM_ERR_CLASS_HAS_CHILDREN                  = 8
        , CIM_ERR_CLASS_HAS_INSTANCES                 = 9
        , CIM_ERR_INVALID_SUPERCLASS                  = 10
        , CIM_ERR_ALREADY_EXISTS                      = 11
        , CIM_ERR_NO_SUCH_PROPERTY                    = 12
        , CIM_ERR_TYPE_MISMATCH                       = 13
        , CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED        = 14
        , CIM_ERR_INVALID_QUERY                       = 15
        , CIM_ERR_METHOD_NOT_AVAILABLE                = 16
        , CIM_ERR_METHOD_NOT_FOUND                    = 17
#  ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
        , CIM_ERR_NAMESPACE_NOT_EMPTY                 = 20
        , CIM_ERR_INVALID_ENUMERATION_CONTEXT         = 21
        , CIM_ERR_INVALID_OPERATION_TIMEOUT           = 22
        , CIM_ERR_PULL_HAS_BEEN_ABANDONED             = 23
        , CIM_ERR_PULL_CANNOT_BE_ABANDONED            = 24
        , CIM_ERR_FILTERED_ENUMERATION_NOT_SUPPORTED  = 25
        , CIM_ERR_CONTINUATION_ON_ERROR_NOT_SUPPORTED = 26
        , CIM_ERR_SERVER_LIMITS_EXCEEDED              = 27
        , CIM_ERR_SERVER_IS_SHUTTING_DOWN             = 28
#  endif // HAVE_PEGASUS_ENUMERATION_CONTEXT

        // Connection error codes
        , CON_ERR_OTHER                               = 40
        , CON_ERR_ALREADY_CONNECTED                   = 41
        , CON_ERR_NOT_CONNECTED                       = 42
        , CON_ERR_INVALID_LOCATOR                     = 43
        , CON_ERR_CANNOT_CREATE_SOCKET                = 44
        , CON_ERR_CANNOT_CONNECT                      = 45
        , CON_ERR_CONNECTION_TIMEOUT                  = 46
        , CON_ERR_SSL_EXCEPTION                       = 47

        // Non standard error codes
        , CON_ERR_BIND                                = 48

#  ifdef HAVE_SLP
        // SLP error codes
        , SLP_ERR_LANGUAGE_NOT_SUPPORTED              = SLP_LANGUAGE_NOT_SUPPORTED
        , SLP_ERR_PARSE_ERROR                         = SLP_PARSE_ERROR
        , SLP_ERR_INVALID_REGISTRATION                = SLP_INVALID_REGISTRATION
        , SLP_ERR_SCOPE_NOT_SUPPORTED                 = SLP_SCOPE_NOT_SUPPORTED
        , SLP_ERR_AUTHENTICATION_ABSENT               = SLP_AUTHENTICATION_ABSENT
        , SLP_ERR_AUTHENTICATION_FAILED               = SLP_AUTHENTICATION_FAILED
        , SLP_ERR_INVALID_UPDATE                      = SLP_INVALID_UPDATE
        , SLP_ERR_REFRESH_REJECTED                    = SLP_REFRESH_REJECTED
        , SLP_ERR_NOT_IMPLEMENTED                     = SLP_NOT_IMPLEMENTED
        , SLP_ERR_BUFFER_OVERFLOW                     = SLP_BUFFER_OVERFLOW
        , SLP_ERR_NETWORK_TIMED_OUT                   = SLP_NETWORK_TIMED_OUT
        , SLP_ERR_NETWORK_INIT_FAILED                 = SLP_NETWORK_INIT_FAILED
        , SLP_ERR_MEMORY_ALLOC_FAILED                 = SLP_MEMORY_ALLOC_FAILED
        , SLP_ERR_PARAMETER_BAD                       = SLP_PARAMETER_BAD
        , SLP_ERR_NETWORK_ERROR                       = SLP_NETWORK_ERROR
        , SLP_ERR_INTERNAL_SYSTEM_ERROR               = SLP_INTERNAL_SYSTEM_ERROR
        , SLP_ERR_HANDLE_IN_USE                       = SLP_HANDLE_IN_USE
        , SLP_ERR_TYPE_ERROR                          = SLP_TYPE_ERROR
#    if !defined(UNICAST_NOT_SUPPORTED) && defined(SLP_RETRY_UNICAST)
        , SLP_ERR_RETRY_UNICAST                       = SLP_RETRY_UNICAST
#    endif // UNICAST_NOT_SUPPORTED
#  endif // HAVE_SLP
    };
};

#endif // LMIWBEM_CONSTANTS_H
