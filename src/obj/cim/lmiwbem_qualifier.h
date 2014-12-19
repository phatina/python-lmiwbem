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

#ifndef   LMIWBEM_QUALIFIER_H
#  define LMIWBEM_QUALIFIER_H

#  include <boost/shared_ptr.hpp>
#  include <boost/python/object.hpp>
#  include <Pegasus/Common/CIMQualifier.h>
#  include "lmiwbem.h"
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

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
    bp::object tomof();

    bp::object copy();

    String getName() const;
    String getType() const;
    bool getIsPropagated()   const;
    bool getIsOverridable()  const;
    bool getIsToSubClass()   const;
    bool getIsToInstance()   const;
    bool getIsTranslatable() const;
    bp::object getPyName()  const;
    bp::object getPyType()  const;
    bp::object getPyValue() const;
    bp::object getPyIsPropagated()   const;
    bp::object getPyIsOverridable()  const;
    bp::object getPyIsToSubclass()   const;
    bp::object getPyIsToInstance()   const;
    bp::object getPyIsTranslatable() const;

    void setName(const String &name);
    void setType(const String &type);
    void setIsPropagated(bool is_propagated);
    void setIsOverridable(bool is_overridable);
    void setIsToSubclass(bool is_tosubclass);
    void setIsToInstance(bool is_toinstance);
    void setIsTranslatable(bool is_translatable);
    void setPyName(const bp::object &name);
    void setPyType(const bp::object &type);
    void setPyValue(const bp::object &value);
    void setPyIsPropagated(const bp::object &is_propagated);
    void setPyIsOverridable(const bp::object &is_overridable);
    void setPyIsToSubclass(const bp::object &is_tosubclass);
    void setPyIsToInstance(const bp::object &is_toinstance);
    void setPyIsTranslatable(const bp::object &is_translatable);

private:
    class CIMQualifierRep;

    boost::shared_ptr<CIMQualifierRep> m_rep;
};

#endif // LMIWBEM_QUALIFIER_H
