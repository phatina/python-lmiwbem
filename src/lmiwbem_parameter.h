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

#ifndef LMIWBEM_PARAMETER_H
#define LMIWBEM_PARAMETER_H

#include <list>
#include <string>
#include <boost/python/object.hpp>
#include <Pegasus/Common/CIMParameter.h>
#include "lmiwbem_refcountedptr.h"

namespace bp = boost::python;

class CIMParameter
{
public:
    CIMParameter();
    CIMParameter(
        const bp::object &name,
        const bp::object &type,
        const bp::object &reference_class,
        const bp::object &is_array,
        const bp::object &array_size,
        const bp::object &qualifiers);

    static void init_type();
    static bp::object create(const Pegasus::CIMParameter &parameter);
    static bp::object create(const Pegasus::CIMConstParameter &parameter);

    std::string repr();

    bp::object getQualifiers();

    void setName(const bp::object &name);
    void setType(const bp::object &type);
    void setReferenceClass(const bp::object &reference_class);
    void setIsArray(const bp::object &is_array);
    void setArraySize(const bp::object &array_size);
    void setQualifiers(const bp::object &qualifiers);
    void setValue(const bp::object &value);

private:
    static bp::object s_class;

    std::string m_name;
    std::string m_type;
    std::string m_reference_class;
    bool m_is_array;
    int  m_array_size;
    bp::object m_qualifiers;

    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_param_qualifiers;
};

#endif // LMIWBEM_PARAMETER_H
