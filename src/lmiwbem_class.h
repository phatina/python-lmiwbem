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

#ifndef   LMIWBEM_CLASS_H
#  define LMIWBEM_CLASS_H

#  include <list>
#  include <string>
#  include <boost/python/object.hpp>
#  include <Pegasus/Common/CIMClass.h>
#  include "lmiwbem_cimbase.h"
#  include "lmiwbem_refcountedptr.h"

namespace bp = boost::python;

class CIMClass: public CIMBase<CIMClass>
{
public:
    CIMClass();
    CIMClass(
        const bp::object &classname,
        const bp::object &properties,
        const bp::object &qualifiers,
        const bp::object &methods,
        const bp::object &superclass);

    static void init_type();
    static bp::object create(const Pegasus::CIMClass &cls);
    static bp::object create(const Pegasus::CIMObject &object);

    Pegasus::CIMClass asPegasusCIMClass();

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif // PY_MAJOR_VERSION

    std::string repr();

    bp::object copy();

    std::string getClassname() const { return m_classname; }
    std::string getSuperClassname() const { return m_super_classname; }
    bp::object getProperties();
    bp::object getQualifiers();
    bp::object getMethods();

    void setClassname(const bp::object &classname);
    void setSuperClassname(const bp::object &superclassname);
    void setProperties(const bp::object &properties);
    void setQualifiers(const bp::object &qualifiers);
    void setMethods(const bp::object &methods);

private:
    std::string m_classname;
    std::string m_super_classname;
    bp::object m_properties;
    bp::object m_qualifiers;
    bp::object m_methods;

    RefCountedPtr<std::list<Pegasus::CIMConstProperty> >  m_rc_class_properties;
    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_class_qualifiers;
    RefCountedPtr<std::list<Pegasus::CIMConstMethod> > m_rc_class_methods;
};

#endif // LMIWBEM_CLASS_H
