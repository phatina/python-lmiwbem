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

#ifndef   LMIWBEM_CLASS_NAME_H
#  define LMIWBEM_CLASS_NAME_H

#  include <string>
#  include <boost/python/object.hpp>
#  include "obj/lmiwbem_cimbase.h"

namespace bp = boost::python;

class CIMClassName: public CIMBase<CIMClassName>
{
public:
    CIMClassName();
    CIMClassName(
        const bp::object &classname,
        const bp::object &namespace_,
        const bp::object &hostname);

    static void init_type();
    static bp::object create(
        const std::string &classname_,
        const std::string &namespace_,
        const std::string &hostname);

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif // PY_MAJOR_VERSION

    bp::object repr();

    bp::object copy();

    std::string getClassname() const;
    std::string getNamespace() const;
    std::string getHostname()  const;
    bp::object getPyClassname() const;
    bp::object getPyNamespace() const;
    bp::object getPyHostname()  const;

    void setClassname(const std::string &classname);
    void setNamespace(const std::string &namespace_);
    void setHostname(const std::string &hostname);
    void setPyClassname(const bp::object &classname);
    void setPyNamespace(const bp::object &namespace_);
    void setPyHostname(const bp::object &hostname);

private:
    std::string m_classname;
    std::string m_namespace;
    std::string m_hostname;
};

#endif // LMIWBEM_CLASS_NAME_H
