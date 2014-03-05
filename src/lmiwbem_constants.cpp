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

#include <boost/python/scope.hpp>
#include "lmiwbem_constants.h"

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
}
