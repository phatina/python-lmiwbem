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

#ifndef LMIWBEM_PROPERTY_H
#define LMIWBEM_PROPERTY_H

#include <list>
#include <string>
#include <boost/python/object.hpp>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMValue.h>
#include "lmiwbem_cimbase.h"
#include "lmiwbem_refcountedptr.h"

namespace bp = boost::python;

class CIMProperty: public CIMBase<CIMProperty>
{
public:
    CIMProperty();
    CIMProperty(
        const bp::object &name,
        const bp::object &value,
        const bp::object &type,
        const bp::object &class_origin,
        const bp::object &array_size,
        const bp::object &propagated,
        const bp::object &qualifiers,
        const bp::object &is_array,
        const bp::object &reference_class);

    static void init_type();
    static bp::object create(const Pegasus::CIMConstProperty &property);
    static bp::object create(
        const bp::object &name,
        const bp::object &value);

    Pegasus::CIMProperty asPegasusCIMProperty();

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif

    std::string repr();

    bp::object copy();

    std::string getName() const { return m_name; }
    std::string getType() const { return m_type; }
    bp::object getValue();
    std::string getClassOrigin() const { return m_class_origin; }
    int getArraySize() const { return m_array_size; }
    bool getPropagated() const { return m_propagated; }
    bp::object getQualifiers();
    bool isArray() const { return m_is_array; }

    void setName(const bp::object &name);
    void setType(const bp::object &type);
    void setValue(const bp::object &value);
    void setClassOrigin(const bp::object &class_origin);
    void setArraySize(const bp::object &array_size);
    void setPropagated(const bp::object &propagated);
    void setQualifiers(const bp::object &qualifiers);
    void setReferenceClass(const bp::object &reference_class);

private:
    static std::string propertyTypeAsString(const Pegasus::CIMType type);

    std::string m_name;
    std::string m_type;
    std::string m_class_origin;
    std::string m_reference_class;
    bool m_is_array;
    bool m_propagated;
    int m_array_size;
    bp::object m_value;
    bp::object m_qualifiers;

    RefCountedPtr<Pegasus::CIMValue> m_rc_prop_value;
    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_prop_qualifiers;
};

#endif // LMIWBEM_PROPERTY_H
