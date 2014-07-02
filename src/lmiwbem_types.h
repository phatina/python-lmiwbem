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

#ifndef   LMIWBEM_TYPES_H
#  define LMIWBEM_TYPES_H

#  include <boost/python/import.hpp>
#  include <boost/python/scope.hpp>
#  include <boost/python/object.hpp>
#  include "lmiwbem_cimbase.h"

namespace bp = boost::python;

#  define DECL_CIMTYPE(name) \
       class name: public CIMBase<name> \
       { \
       public: \
           static void init_type(); \
           static bp::object create() { return CIMBase<name>::create(); } \
           template <typename T> \
           static bp::object create(const T &value) { return CIMBase<name>::create(value); } \
       }

#  define DEF_CIMTYPE(name) \
       void name::init_type() \
       { \
           CIMBase<name>::init_type(bp::import("lmiwbem.lmiwbem_types").attr(#name)); \
           bp::scope().attr(#name) = CIMBase<name>::type(); \
       } \

DECL_CIMTYPE(MinutesFromUTC);
DECL_CIMTYPE(CIMType);
DECL_CIMTYPE(CIMDateTime);
DECL_CIMTYPE(CIMInt);
DECL_CIMTYPE(Uint8);
DECL_CIMTYPE(Sint8);
DECL_CIMTYPE(Uint16);
DECL_CIMTYPE(Sint16);
DECL_CIMTYPE(Uint32);
DECL_CIMTYPE(Sint32);
DECL_CIMTYPE(Uint64);
DECL_CIMTYPE(Sint64);
DECL_CIMTYPE(CIMFloat);
DECL_CIMTYPE(Real32);
DECL_CIMTYPE(Real64);

#endif // LMIWBEM_TYPES_H
