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

#ifndef   LMIWBEM_EXTRACT_H
#  define LMIWBEM_EXTRACT_H

#  include <string>
#  include <boost/python/dict.hpp>
#  include <boost/python/tuple.hpp>
#  include <boost/python/extract.hpp>
#  include "lmiwbem.h"
#  include "lmiwbem_exception.h"

namespace bp = boost::python;

LMIWBEM_BEGIN

template <typename T>
class extract: public bp::extract<T>
{
public:
    extract(PyObject *pyobj): bp::extract<T>(pyobj) { }
    extract(const bp::object &obj): bp::extract<T>(obj) { }
};

template <>
class extract<std::string>
{
public:
    extract(PyObject *pyobj);
    extract(const bp::object &obj);

    bool check() const { return m_good; }

    operator std::string() { return m_str; }

private:
    void convert();

    bp::object m_obj;
    std::string m_str;
    bool m_good;
};

template <typename T>
void throw_if_fail(
    const extract<T> &ext_obj,
    const std::string &member)
{
    if (ext_obj.check())
        return;
    throw_TypeError_member<T>(member);
}

template <typename T>
T extract_or_throw(
    const bp::object &obj,
    const std::string &member = "variable")
{
    extract<T> ext_obj(obj);
    throw_if_fail<T>(ext_obj, member);
    return T(ext_obj);
}

template <>
bp::dict extract_or_throw<bp::dict>(
    const bp::object &obj,
    const std::string &member);

template <>
bp::tuple extract_or_throw<bp::tuple>(
    const bp::object &obj,
    const std::string &member);

template <typename T>
bp::object get_or_throw(
    const bp::object &obj,
    const std::string &member = "variable")
{
    throw_if_fail<T>(extract<T>(obj), member);
    return obj;
}

template <typename T, typename U>
bp::object get_or_throw(
    const bp::object &obj,
    const std::string &member = "variable")
{
    extract<T> ext_objT(obj);
    extract<U> ext_objU(obj);
    if (!ext_objT.check() && !ext_objU.check())
        throw_TypeError_member<T>(member);
    return obj;
}

LMIWBEM_END

#endif // LMIWBEM_EXTRACT_H
