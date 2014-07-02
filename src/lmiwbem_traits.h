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

#ifndef   LMIWBEM_TRAITS_H
#  define LMIWBEM_TRAITS_H

#  include "lmiwbem.h"

BOOST_PYTHON_BEGIN
class dict;
class list;
class tuple;
BOOST_PYTHON_END

class CIMInstance;
class CIMInstanceName;
class NocaseDict;
class WBEMConnection;

namespace bp = boost::python;

LMIWBEM_BEGIN

#  define DEF_TYPE_NAME_TYPE(type, value) \
       template <> \
       struct type_name<type> \
       { \
           static std::string get() { return std::string(#value); } \
       }

#  define DEF_TYPE_NAME(type) DEF_TYPE_NAME_TYPE(type, type)

template <typename T>
struct type_name
{
    static std::string get() { return std::string(); }
};

DEF_TYPE_NAME(bool);
DEF_TYPE_NAME(int);
DEF_TYPE_NAME(CIMInstance);
DEF_TYPE_NAME(CIMInstanceName);
DEF_TYPE_NAME(WBEMConnection);
DEF_TYPE_NAME_TYPE(CIMInstance&, CIMInstance);
DEF_TYPE_NAME_TYPE(CIMInstanceName&, CIMInstanceName);
DEF_TYPE_NAME_TYPE(WBEMConnection&, WBEMConnection);
DEF_TYPE_NAME(NocaseDict);
DEF_TYPE_NAME_TYPE(bp::dict, dict);
DEF_TYPE_NAME_TYPE(bp::list, list);
DEF_TYPE_NAME_TYPE(bp::tuple, tuple);
DEF_TYPE_NAME_TYPE(std::string, string);

LMIWBEM_END

#endif // LMIWBEM_TRAITS_H
