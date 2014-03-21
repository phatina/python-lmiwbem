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

#ifndef LMIWBEM_EXCEPTION_H
#define LMIWBEM_EXCEPTION_H

#include <string>
#include <boost/python/errors.hpp>
#include <boost/python/object.hpp>
#include <Pegasus/Common/Exception.h>
#include "lmiwbem_traits.h"

namespace bp = boost::python;

void throw_Exception(const Pegasus::Exception &e);
void throw_CIMError(const Pegasus::CIMException &e);
void throw_CIMError(const std::string &message, int code = 0);
void throw_ConnectionError(const std::string &message, int code = 0);

void throw_ValueError(const std::string &message);
void throw_KeyError(const std::string &message);
void throw_StopIteration(const std::string &message);
void throw_TypeError(const std::string &message);
void throw_RuntimeError(const std::string &message);

template <typename T>
void throw_TypeError_member(const std::string &member)
{
    std::string msg("Wrong type");
    std::string type = lmi::type_name<T>::get();
    if (!type.empty())
        msg = member + std::string(" must be ") + type + std::string(" type");
    throw_TypeError(msg);
}

void handle_all_exceptions();

#endif // LMIWBEM_EXCEPTION_H
