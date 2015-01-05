/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014-2015, Peter Hatina <phatina@redhat.com>
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

#ifndef   LMIWBEM_PARAMETER_H
#  define LMIWBEM_PARAMETER_H

#  include <list>
#  include <boost/python/object.hpp>
#  include "lmiwbem.h"
#  include "lmiwbem_refcountedptr.h"
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

BOOST_PYTHON_BEGIN
class CIMConstParameter;
BOOST_PYTHON_END

namespace bp = boost::python;

class CIMParameter: public CIMBase<CIMParameter>
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

    Pegasus::CIMParameter asPegasusCIMParameter();

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
    String getReferenceClass() const;
    bool getIsArray() const;
    int  getArraySize() const;
    bp::object getPyName() const;
    bp::object getPyType() const;
    bp::object getPyReferenceClass() const;
    bp::object getPyIsArray() const;
    bp::object getPyArraySize() const;
    bp::object getPyQualifiers();

    void setName(const String &name);
    void setType(const String &type);
    void setReferenceClass(const String &reference_class);
    void setIsArray(bool is_array);
    void setArraySize(int array_size);
    void setPyName(const bp::object &name);
    void setPyType(const bp::object &type);
    void setPyReferenceClass(const bp::object &reference_class);
    void setPyIsArray(const bp::object &is_array);
    void setPyArraySize(const bp::object &array_size);
    void setPyQualifiers(const bp::object &qualifiers);

private:
    String m_name;
    String m_type;
    String m_reference_class;
    bool m_is_array;
    int  m_array_size;
    bp::object m_qualifiers;

    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_param_qualifiers;
};

#endif // LMIWBEM_PARAMETER_H
