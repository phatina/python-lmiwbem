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

#include <config.h>
#include <boost/python/class.hpp>
#include "obj/cim/lmiwbem_enum_ctx.h"

class CIMEnumerationContext::CIMEnumerationContextRep
{
public:
    CIMEnumerationContextRep();

    boost::shared_ptr<Pegasus::CIMEnumerationContext> m_enum_ctx_ptr;
    bool m_is_with_paths;
    String m_namespace;
};

CIMEnumerationContext::CIMEnumerationContextRep::CIMEnumerationContextRep()
    : m_enum_ctx_ptr()
    , m_is_with_paths(true)
    , m_namespace()
{
}

CIMEnumerationContext::CIMEnumerationContext()
    : m_rep(new CIMEnumerationContextRep)
{
}

void CIMEnumerationContext::init_type()
{
    CIMBase<CIMEnumerationContext>::init_type(
        bp::class_<CIMEnumerationContext, boost::noncopyable>("CIMEnumerationContext", bp::init<>())
        .def("__repr__", &CIMEnumerationContext::repr)
        .def("clear", &CIMEnumerationContext::clear));
}

bp::object CIMEnumerationContext::create(
    Pegasus::CIMEnumerationContext *ctx_ptr,
    const bool with_paths,
    const String &ns)
{
    bp::object py_inst(CIMBase<CIMEnumerationContext>::create());
    CIMEnumerationContext &fake_this = CIMEnumerationContext::asNative(py_inst);
    fake_this.m_rep->m_enum_ctx_ptr.reset(ctx_ptr);
    fake_this.m_rep->m_is_with_paths = with_paths;
    fake_this.m_rep->m_namespace = ns;
    return py_inst;
}

bp::object CIMEnumerationContext::create(
    const boost::shared_ptr<Pegasus::CIMEnumerationContext> &ctx_ptr,
    const bool with_paths,
    const String &ns)
{
    bp::object py_inst(CIMBase<CIMEnumerationContext>::create());
    CIMEnumerationContext &fake_this = CIMEnumerationContext::asNative(py_inst);
    fake_this.m_rep->m_enum_ctx_ptr = ctx_ptr;
    fake_this.m_rep->m_is_with_paths = with_paths;
    fake_this.m_rep->m_namespace = ns;
    return py_inst;
}

bp::object CIMEnumerationContext::repr()
{
    return StringConv::asPyUnicode(
        String("CIMEnumerationContext()"));
}

Pegasus::CIMEnumerationContext &CIMEnumerationContext::getPegasusContext()
{
    if (!m_rep->m_enum_ctx_ptr) {
        std::cout << "We don't have a context!\n";

        // Just in case, we don't dereference a NULL pointer.
        m_rep->m_enum_ctx_ptr.reset(new Pegasus::CIMEnumerationContext);
    }

    return *m_rep->m_enum_ctx_ptr;
}

String CIMEnumerationContext::getNamespace() const
{
    return m_rep->m_namespace;
}

bool CIMEnumerationContext::getIsWithPaths() const
{
    return m_rep->m_is_with_paths;
}

void CIMEnumerationContext::setNamespace(const String &ns)
{
    m_rep->m_namespace = ns;
}

void CIMEnumerationContext::setIsWithPaths(const bool is_with_paths)
{
    m_rep->m_is_with_paths = is_with_paths;
}

void CIMEnumerationContext::clear()
{
    if (!m_rep->m_enum_ctx_ptr)
        return;
    m_rep->m_enum_ctx_ptr->clear();
}
