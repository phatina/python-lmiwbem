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

#include <string>
#include <boost/python/object.hpp>
#include "lmiwbem_extract.h"

namespace bp = boost::python;

namespace lmi {

extract<std::string>::extract(PyObject *pyobj)
    : m_obj(bp::handle<>(pyobj))
    , m_str()
    , m_good(true)
{
    convert();
}

extract<std::string>::extract(const bp::object &obj)
    : m_obj(obj)
    , m_str()
    , m_good(true)
{
    convert();
}

void extract<std::string>::convert()
{
    PyObject *pyobj = m_obj.ptr();
    if (PyUnicode_Check(pyobj)) {
        m_str = std::string(
            PyString_AsString(
                PyUnicode_EncodeUTF8(
                    PyUnicode_AS_UNICODE(pyobj),
                    PyUnicode_GetSize(pyobj),
                    NULL
                )
            )
        );
    } else if (PyString_Check(pyobj)) {
        m_str = std::string(PyString_AS_STRING(pyobj));
    } else {
        m_good = false;
    }
}

template <>
bp::dict extract_or_throw<bp::dict>(
    const bp::object &obj,
    const std::string &member)
{
    extract<bp::dict> ext_obj(obj);
    throw_if_fail<bp::dict>(ext_obj, member);
    return ext_obj;
}

template <>
bp::tuple extract_or_throw<bp::tuple>(
    const bp::object &obj,
    const std::string &member)
{
    extract<bp::tuple> ext_obj(obj);
    throw_if_fail<bp::tuple>(ext_obj, member);
    return ext_obj;
}

} // namespace lmi
