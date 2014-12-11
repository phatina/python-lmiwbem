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

#ifndef   LMIWBEM_ENUM_CTX_H
#  define LMIWBEM_ENUM_CTX_H

#  include <boost/shared_ptr.hpp>
#  include <boost/python/object.hpp>
#  include <Pegasus/Client/CIMEnumerationContext.h>
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

namespace bp = boost::python;

class CIMEnumerationContext: public CIMBase<CIMEnumerationContext>
{
public:
    CIMEnumerationContext();

    static void init_type();
    static bp::object create(
        Pegasus::CIMEnumerationContext *ctx_ptr,
        const bool with_paths = true,
        const String &ns = String());
    static bp::object create(
        const boost::shared_ptr<Pegasus::CIMEnumerationContext> &ctx_ptr,
        const bool with_paths = true,
        const String &ns = String());

    bp::object repr();

    Pegasus::CIMEnumerationContext &getPegasusContext();

    // These methods are present due to non-uniform Pegasus::CIMClient pull
    // methods (::pullInstancesWithPaths() vs. ::pullInstnaces()).
    String getNamespace() const;
    bool getIsWithPaths() const;
    void setNamespace(const String &ns);
    void setIsWithPaths(const bool is_with_paths);

    void clear();

private:
    boost::shared_ptr<Pegasus::CIMEnumerationContext> m_enum_ctx_ptr;
    bool m_is_with_paths;
    String m_namespace;
};

#endif // LMIWBEM_ENUM_CTX_H
