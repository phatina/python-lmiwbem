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

    static const int CIM_ERR_FAILED                       = 1;
    static const int CIM_ERR_ACCESS_DENIED                = 2;
    static const int CIM_ERR_INVALID_NAMESPACE            = 3;
    static const int CIM_ERR_INVALID_PARAMETER            = 4;
    static const int CIM_ERR_INVALID_CLASS                = 5;
    static const int CIM_ERR_NOT_FOUND                    = 6;
    static const int CIM_ERR_NOT_SUPPORTED                = 7;
    static const int CIM_ERR_CLASS_HAS_CHILDREN           = 8;
    static const int CIM_ERR_CLASS_HAS_INSTANCES          = 9;
    static const int CIM_ERR_INVALID_SUPERCLASS           = 10;
    static const int CIM_ERR_ALREADY_EXISTS               = 11;
    static const int CIM_ERR_NO_SUCH_PROPERTY             = 12;
    static const int CIM_ERR_TYPE_MISMATCH                = 13;
    static const int CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED = 14;
    static const int CIM_ERR_INVALID_QUERY                = 15;
    static const int CIM_ERR_METHOD_NOT_AVAILABLE         = 16;
    static const int CIM_ERR_METHOD_NOT_FOUND             = 17;

    static const int CON_ERR_OTHER                        = 18;
    static const int CON_ERR_ALREADY_CONNECTED            = 19;
    static const int CON_ERR_NOT_CONNECTED                = 20;
    static const int CON_ERR_INVALID_LOCATOR              = 21;
    static const int CON_ERR_CANNOT_CREATE_SOCKET         = 22;
    static const int CON_ERR_CANNOT_CONNECT               = 23;
    static const int CON_ERR_CONNECTION_TIMEOUT           = 24;
    static const int CON_ERR_SSL_EXCEPTION                = 25;

    // Non standard error codes
    static const int CON_ERR_BIND                         = 26;

#  ifdef HAVE_SLP
    // SLP error codes
    static const int SLP_ERR_LANGUAGE_NOT_SUPPORTED       = SLP_LANGUAGE_NOT_SUPPORTED;
    static const int SLP_ERR_PARSE_ERROR                  = SLP_PARSE_ERROR;
    static const int SLP_ERR_INVALID_REGISTRATION         = SLP_INVALID_REGISTRATION;
    static const int SLP_ERR_SCOPE_NOT_SUPPORTED          = SLP_SCOPE_NOT_SUPPORTED;
    static const int SLP_ERR_AUTHENTICATION_ABSENT        = SLP_AUTHENTICATION_ABSENT;
    static const int SLP_ERR_AUTHENTICATION_FAILED        = SLP_AUTHENTICATION_FAILED;
    static const int SLP_ERR_INVALID_UPDATE               = SLP_INVALID_UPDATE;
    static const int SLP_ERR_REFRESH_REJECTED             = SLP_REFRESH_REJECTED;
    static const int SLP_ERR_NOT_IMPLEMENTED              = SLP_NOT_IMPLEMENTED;
    static const int SLP_ERR_BUFFER_OVERFLOW              = SLP_BUFFER_OVERFLOW;
    static const int SLP_ERR_NETWORK_TIMED_OUT            = SLP_NETWORK_TIMED_OUT;
    static const int SLP_ERR_NETWORK_INIT_FAILED          = SLP_NETWORK_INIT_FAILED;
    static const int SLP_ERR_MEMORY_ALLOC_FAILED          = SLP_MEMORY_ALLOC_FAILED;
    static const int SLP_ERR_PARAMETER_BAD                = SLP_PARAMETER_BAD;
    static const int SLP_ERR_NETWORK_ERROR                = SLP_NETWORK_ERROR;
    static const int SLP_ERR_INTERNAL_SYSTEM_ERROR        = SLP_INTERNAL_SYSTEM_ERROR;
    static const int SLP_ERR_HANDLE_IN_USE                = SLP_HANDLE_IN_USE;
    static const int SLP_ERR_TYPE_ERROR                   = SLP_TYPE_ERROR;
#    ifndef UNICAST_NOT_SUPPORTED
    static const int SLP_ERR_RETRY_UNICAST                = SLP_RETRY_UNICAST;
#    endif // UNICAST_NOT_SUPPORTED
#  endif // HAVE_SLP

    static std::string defaultNamespace();
    static std::string defaultTrustStore();

private:
    static const std::string DEF_NAMESPACE;
    static const std::string DEF_TRUST_STORE;
};

#endif // LMIWBEM_CONSTANTS_H
