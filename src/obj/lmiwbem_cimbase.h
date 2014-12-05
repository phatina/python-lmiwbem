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

#ifndef   LMIWBEM_CIMBASE
#  define LMIWBEM_CIMBASE

#  include <boost/python/object.hpp>
#  include "util/lmiwbem_convert.h"

namespace bp = boost::python;

template <typename T>
class CIMBase
{
public:
    static void init_type(const bp::object &class_) { s_class = class_; }
    static const bp::object &type() { return s_class; }

    static bp::object create() { return s_class(); }

    template <typename U>
    static bp::object create(const U &param) { return s_class(param); }

    template <typename U, typename V>
    static bp::object create(
        const U &param1,
        const V &param2)
    {
        return s_class(param1, param2);
    }

    static T &asNative(const bp::object &obj)
    {
        return Conv::as<T&>(obj);
    }

    static T &asNative(
        const bp::object &obj,
        const std::string &member)
    {
        return Conv::as<T&>(obj, member);
    }

private:
    static bp::object s_class;
};

template <typename T>
bp::object CIMBase<T>::s_class;

#endif // LMIWBEM_CIMBASE
