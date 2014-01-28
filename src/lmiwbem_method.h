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

#ifndef LMIWBEM_METHOD_H
#define LMIWBEM_METHOD_H

#include <list>
#include <boost/python/object.hpp>
#include <Pegasus/Common/CIMMethod.h>
#include "lmiwbem_refcountedptr.h"

namespace bp = boost::python;

class CIMMethod
{
public:
    CIMMethod();
    CIMMethod(
        const bp::object &name,
        const bp::object &return_type,
        const bp::object &parameters,
        const bp::object &class_origin,
        const bp::object &propagated,
        const bp::object &qualifiers);

    static void init_type();
    static bp::object create(const Pegasus::CIMConstMethod &method);

    std::string repr();

    bp::object getParameters();
    bp::object getQualifiers();

    void setName(const bp::object &name);
    void setReturnType(const bp::object &rtype);
    void setParameters(const bp::object &parameters);
    void setClassOrigin(const bp::object &class_origin);
    void setPropagated(const bp::object &propagated);
    void setQualifiers(const bp::object &qualifiers);

private:
    static bp::object s_class;

    std::string m_name;
    std::string m_return_type;
    std::string m_class_origin;
    bool m_propagated;
    bp::object m_parameters;
    bp::object m_qualifiers;

    RefCountedPtr<std::list<Pegasus::CIMConstParameter> > m_rc_meth_parameters;
    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_meth_qualifiers;
};

#endif // LMIWBEM_METHOD_H
