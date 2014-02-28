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

#ifndef LMIWBEM_QUALIFIER_H
#define LMIWBEM_QUALIFIER_H

#include <string>
#include <boost/python/object.hpp>
#include <Pegasus/Common/CIMQualifier.h>
#include "lmiwbem_cimbase.h"
#include "lmiwbem.h"

namespace bp = boost::python;

class CIMQualifier: public CIMBase<CIMQualifier>
{
public:
    CIMQualifier();
    CIMQualifier(
        const bp::object &name,
        const bp::object &value,
        const bp::object &type,
        const bp::object &propagated,
        const bp::object &overridable,
        const bp::object &tosubclass,
        const bp::object &toinstance,
        const bp::object &translatable);

    static void init_type();
    static bp::object create(const Pegasus::CIMQualifier &qualifier);
    static bp::object create(const Pegasus::CIMConstQualifier &qualifier);

    Pegasus::CIMQualifier asPegasusCIMQualifier() const;

    int cmp(const bp::object &other);

    std::string repr();
    std::string tomof();

    bp::object copy();

    bp::object getName();
    bp::object getType();

    void setName(const bp::object &name);
    void setType(const bp::object &name);

    void setPropagated(const bp::object &propagated);
    void setOverridable(const bp::object &overridable);
    void setToSubclass(const bp::object &tosubclass);
    void setToInstance(const bp::object &toinstance);
    void setTranslatable(const bp::object &translatable);

private:
    std::string m_name;
    std::string m_type;
    bp::object m_value;
    bool m_propagated;
    bool m_overridable;
    bool m_tosubclass;
    bool m_toinstance;
    bool m_translatable;
};

#endif // LMIWBEM_QUALIFIER_H
