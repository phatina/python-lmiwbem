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

#ifndef   LMIWBEM_UTIL_H
#  define LMIWBEM_UTIL_H

#  include <cassert>
#  include "lmiwbem.h"

BOOST_PYTHON_BEGIN
class object;
BOOST_PYTHON_END

namespace bp = boost::python;

#  if defined(__GNUC__)
#    define GCC_VERSION (__GNUC__ * 10000 + \
                         __GNUC_MINOR__ * 100 + \
                         __GNUC_PATCHLEVEL__)
#  endif

// In case __builtin_unreachable is not available, CODE like
// assert( false && "Some explanation of situation") can be used as
// LMIWBEM_UNREACHABLE( assert(false  && "explanation"));
#  if GCC_VERSION >= 40500 //supported only for gcc 4.5 and above
#    define LMIWBEM_UNREACHABLE(CODE) __builtin_unreachable()
#  else
#    define LMIWBEM_UNREACHABLE(CODE) CODE
#  endif

// Declare a Python's equivalent of None
extern const bp::object None;

bp::object this_module();
bp::object incref(const bp::object &obj);

bool isnone(const bp::object &obj);
bool isinstance(const bp::object &inst, const bp::object &cls);
bool isunicode(const bp::object &obj);
bool isbasestring(const bp::object &obj);
bool isbool(const bp::object &obj);
#  if PY_MAJOR_VERSION < 3
bool isstring(const bp::object &obj);
bool isint(const bp::object &obj);
#  endif // PY_MAJOR_VERSION
bool islong(const bp::object &obj);
bool isfloat(const bp::object &obj);
bool isdict(const bp::object &obj);
bool islist(const bp::object &obj);
bool istuple(const bp::object &obj);
bool iscallable(const bp::object &obj);

bool cim_issubclass(
    const bp::object &ch,
    const bp::object &ns,
    const bp::object &superclass,
    const bp::object &subclass);
bool is_error(const bp::object &value);

#  if PY_MAJOR_VERSION < 3
int compare(const bp::object &o1, const bp::object &o2);
#  else
bool compare(const bp::object &o1, const bp::object &o2, int cmp_type);
#  endif // PY_MAJOR_VERSION

#endif // LMIWBEM_UTIL_H
