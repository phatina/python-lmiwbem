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

#ifndef   LMIWBEM_INSTANCE_H
#  define LMIWBEM_INSTANCE_H

#  include <list>
#  include <string>
#  include <boost/python/object.hpp>
#  include <Pegasus/Common/CIMInstance.h>
#  include "lmiwbem.h"
#  include "lmiwbem_cimbase.h"
#  include "lmiwbem_refcountedptr.h"

namespace bp = boost::python;

class CIMInstance: public CIMBase<CIMInstance>
{
public:
    CIMInstance();
    CIMInstance(
        const bp::object &classname,
        const bp::object &properties,
        const bp::object &qualifiers,
        const bp::object &path,
        const bp::object &property_list);

    static void init_type();
    static bp::object create(const Pegasus::CIMInstance &instance);
    static bp::object create(const Pegasus::CIMObject &object);

    Pegasus::CIMInstance asPegasusCIMInstance();

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
    bp::object tomof();

    bp::object getitem(const bp::object &key);
    void setitem(
        const bp::object &key,
        const bp::object &value);
    ssize_t len() { return bp::len(getProperties()); }
    bool haskey(const bp::object &key) { return getProperties().contains(key); }
    bp::object keys();
    bp::object values();
    bp::object items();
    bp::object iterkeys();
    bp::object itervalues();
    bp::object iteritems();

    bp::object copy();

    bp::object getClassname();
    bp::object getPath();
    bp::object getProperties();
    bp::object getQualifiers();
    bp::object getPropertyList();

    void setClassname(const bp::object &classname);
    void setPath(const bp::object &path);
    void setProperties(const bp::object &properties);
    void setQualifiers(const bp::object &qualifiers);
    void setPropertyList(const bp::object &property_list);

    static void updatePegasusCIMInstanceNamespace(
        Pegasus::CIMInstance &instance,
        const std::string &ns);

private:
    void evalProperties();

    static std::string tomofContent(const bp::object &value);

    std::string m_classname;
    bp::object m_path;
    bp::object m_properties;
    bp::object m_qualifiers;
    bp::object m_property_list;

    RefCountedPtr<Pegasus::CIMObjectPath> m_rc_inst_path;
    RefCountedPtr<std::list<Pegasus::CIMConstProperty> > m_rc_inst_properties;
    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_inst_qualifiers;
};

#endif // LMIWBEM_INSTANCE_H
