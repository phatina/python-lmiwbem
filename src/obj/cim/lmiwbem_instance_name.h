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

#ifndef   LMIWBEM_INSTANCE_NAME_H
#  define LMIWBEM_INSTANCE_NAME_H

#  include <boost/python/object.hpp>
#  include "lmiwbem.h"
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

PEGASUS_BEGIN
class CIMObjectPath;
PEGASUS_END

namespace bp = boost::python;

class CIMInstanceName: public CIMBase<CIMInstanceName>
{
public:
    CIMInstanceName();
    CIMInstanceName(
        const bp::object &cls,
        const bp::object &keybindings,
        const bp::object &host,
        const bp::object &ns);

    static void init_type();
    static bp::object create(
        const Pegasus::CIMObjectPath &obj_path,
        const String &ns = String(),
        const String &hostname = String());

    Pegasus::CIMObjectPath asPegasusCIMObjectPath() const;

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif // PY_MAJOR_VERSION

    String asString() const;
    bp::object str() const;
    bp::object repr() const;

    bp::object getitem(const bp::object &key);
    void delitem(const bp::object &key);
    void setitem(const bp::object &key, const bp::object &value);
    bp::object len() const;
    bp::object haskey(const bp::object &key) const;
    bp::object keys();
    bp::object values();
    bp::object items();
    bp::object iterkeys();
    bp::object itervalues();
    bp::object iteritems();

    bp::object copy();

    String getClassname() const;
    String getNamespace() const;
    String getHostname()  const;
    bp::object getPyClassname() const;
    bp::object getPyNamespace() const;
    bp::object getPyHostname() const;
    bp::object getPyKeybindings() const;

    void setClassname(const String &classname);
    void setNamespace(const String &namespace_);
    void setHostname(const String &hostname);
    void setPyClassname(const bp::object &classname);
    void setPyNamespace(const bp::object &namespace_);
    void setPyHostname(const bp::object &hostname);
    void setPyKeybindings(const bp::object &keybindings);

    static void updatePegasusCIMObjectPathNamespace(
        Pegasus::CIMObjectPath &path,
        const String &ns);
    static void updatePegasusCIMObjectPathHostname(
        Pegasus::CIMObjectPath &path,
        const String &hostname);
    static bool isUninitialized(const Pegasus::CIMObjectPath &path);

private:
    static bp::object keybindingToValue(const Pegasus::CIMKeyBinding &keybinding);

    String m_classname;
    String m_namespace;
    String m_hostname;
    bp::object m_keybindings;
};

#endif // LMIWBEM_INSTANCE_NAME_H
