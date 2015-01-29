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

#include <config.h>
#include <list>
#include <string>
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/object.hpp>
#include <boost/python/str.hpp>
#include <boost/python/tuple.hpp>
#include "lmiwbem_exception.h"
#include "obj/lmiwbem_slp.h"
#include "obj/lmiwbem_slp_pydoc.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

ScopedSLPHandle::ScopedSLPHandle(
    const bool is_async,
    const String pc_lang)
{
    m_error = SLPOpen(
        pc_lang.c_str(),
        is_async ? SLP_TRUE : SLP_FALSE,
        &m_handle);
}

ScopedSLPHandle::~ScopedSLPHandle()
{
    if (!good())
        return;
    SLPClose(m_handle);
}

SLPHandle ScopedSLPHandle::handle()
{
    return m_handle;
}

SLPError ScopedSLPHandle::error() const
{
    return m_error;
}

bool ScopedSLPHandle::good() const
{
    return m_error == SLP_OK;
}

bool ScopedSLPHandle::operator!() const
{
    return !good();
}

ScopedSLPHandle::operator SLPHandle() const
{
    return m_handle;
}

void SLP::init_type()
{
    bp::def("slp_discover", SLP::discover,
        (bp::arg("srvtype") = None,
         bp::arg("scopelist") = None,
         bp::arg("filter") = None,
         bp::arg("async") = false),
        docstr_slp_discover);
    bp::def("slp_discover_attrs", SLP::discoverAttrs,
        (bp::arg("srvurl"),
         bp::arg("scopelist") = None,
         bp::arg("attrids") = None,
         bp::arg("async") = false),
         docstr_slp_discover_attrs);
}

SLPBoolean SLP::urlCallback(
    SLPHandle hslp,
    const char *srvurl,
    unsigned short lifetime,
    SLPError errcode,
    void *cookie)
{
    if ((errcode == SLP_OK || errcode == SLP_LAST_CALL) && srvurl != NULL) {
        SLPSrvURL *url;
        SLPParseSrvURL(srvurl, &url);

        bp::list *py_srvs_ptr = static_cast<bp::list *>(cookie);
        py_srvs_ptr->append(SLPResult::create(url));

        SLPFree(url);
    }

    return SLP_TRUE;
}

SLPBoolean SLP::attrCallback(
    SLPHandle hslp,
    const char *attrlist,
    SLPError errcode,
    void *cookie)
{
    if (errcode == SLP_OK) {
        bp::dict &py_attrs = *static_cast<bp::dict *>(cookie);

        std::stringstream ss(attrlist);
        String item;

        while (std::getline(ss, item, ',')) {
            std::size_t pos = item.find("=", 0, 1);

            // Basic check of the attribute's format.
            if (item[0] != '(' ||
                item[item.length() - 1] != ')' ||
                pos == String::npos)
            {
                return SLP_FALSE;
            }

            // Cut the key and value of the attribute.
            String key = item.substr(1, pos - 1);
            String val = item.substr(pos + 1, item.length() - pos - 2);

            py_attrs[key] = val;
        }
    }

    return SLP_TRUE;
}

bp::object SLP::discover(
    const bp::object &srvtype,
    const bp::object &scopelist,
    const bp::object &filter,
    const bp::object &async)
{
    String c_srvtype;
    String c_scopelist;
    String c_filter;

    if (!isnone(srvtype))
        c_srvtype = StringConv::asString(srvtype, "srvtype");

    if (!isnone(scopelist))
        c_scopelist = StringConv::asString(scopelist, "scopelist");

    if (!isnone(filter))
        c_filter = StringConv::asString(filter, "filter");

    bool c_async = Conv::as<bool>(async, "async");

    // Open SLP handle.
    ScopedSLPHandle hslp(c_async);
    if (!hslp)
        throw_SLPError("Can't open SLP handle", static_cast<int>(hslp.error()));

    // Discover the services.
    SLPError err;
    bp::list py_srvs;
    if ((err = SLPFindSrvs(
            hslp,
            c_srvtype.c_str(),
            c_scopelist.c_str(),
            c_filter.c_str(),
            SLP::urlCallback,
            static_cast<void*>(&py_srvs))) != SLP_OK)
    {
        throw_SLPError("SLP discovery failed", static_cast<int>(err));
    }

    return py_srvs;
}

bp::object SLP::discoverAttrs(
    const bp::object &srvurl,
    const bp::object &scopelist,
    const bp::object &attrids,
    const bp::object &async)
{
    String c_srvurl(StringConv::asString(srvurl, "srvurl"));
    String c_scopelist;
    String c_attrids;

    if (!isnone(scopelist))
        c_scopelist = StringConv::asString(scopelist, "scopelist");

    if (!isnone(attrids))
        c_attrids = StringConv::asString(attrids, "attrids");

    bool c_async = Conv::as<bool>(async, "async");

    // Open SLP handle.
    ScopedSLPHandle hslp(c_async);
    if (!hslp)
        throw_SLPError("Can't open SLP handle", static_cast<int>(hslp.error()));

    // Discover the attrs.
    SLPError err;
    bp::dict py_attrs;
    if ((err = SLPFindAttrs(
            hslp,
            c_srvurl.c_str(),
            c_scopelist.c_str(),
            c_attrids.c_str(),
            SLP::attrCallback,
            static_cast<void*>(&py_attrs))) != SLP_OK)
    {
        throw_SLPError("SLP attrs discovery failed", static_cast<int>(err));
    }

    return py_attrs;
}

SLPResult::SLPResult()
    : m_srvtype()
    , m_host()
    , m_family()
    , m_srvpart()
    , m_port(0)
{
}

SLPResult::SLPResult(
    const bp::object &srvtype,
    const bp::object &host,
    const bp::object &port,
    const bp::object &family,
    const bp::object &srvpart)
    : m_srvtype()
    , m_host()
    , m_family()
    , m_srvpart()
    , m_port(0)
{
    m_srvtype = StringConv::asString(srvtype, "srvtype");
    m_host = StringConv::asString(host, "host");
    m_port = Conv::as<int>(port, "port");

    if (!isnone(family))
        m_family = StringConv::asString(family, "family");

    if (!isnone(srvpart))
        m_srvpart = StringConv::asString(srvpart, "srvpart");
}

void SLPResult::init_type()
{
    CIMBase<SLPResult>::init_type(
        bp::class_<SLPResult>("SLPResult", bp::init<>())
        .def(bp::init<
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &,
            const bp::object &>((
                bp::arg("srvtype"),
                bp::arg("host"),
                bp::arg("port") = 0,
                bp::arg("family") = None,
                bp::arg("srvpart") = None),
                docstr_SLPResult_init))
        .def("__repr__", &SLPResult::repr)
        .add_property("srvtype",
            &SLPResult::getPySrvType,
            &SLPResult::setPySrvType)
        .add_property("host",
            &SLPResult::getPyHost,
            &SLPResult::setPyHost)
        .add_property("port",
            &SLPResult::getPyPort,
            &SLPResult::setPyPort)
        .add_property("family",
            &SLPResult::getPyFamily,
            &SLPResult::setFamily)
        .add_property("srvpart",
            &SLPResult::getPySrvPart,
            &SLPResult::setPySrvPart));
}

bp::object SLPResult::create(const SLPSrvURL *url)
{
    bp::object py_inst = CIMBase<SLPResult>::create();
    SLPResult &fake_this = SLPResult::asNative(py_inst);

    fake_this.m_srvtype = String(url->s_pcSrvType);
    fake_this.m_host = String(url->s_pcHost);
    fake_this.m_port = url->s_iPort;
    fake_this.m_family = String(url->s_pcNetFamily);
    fake_this.m_srvpart = String(url->s_pcSrvPart);

    return py_inst;
}

String SLPResult::repr()
{
    std::stringstream ss;
    ss << "SLPResult(srvtype=u'" << m_srvtype
       << "', host=u'" << m_host << "', port='"
       << m_port << "', ...)";
    return ss.str();
}

String SLPResult::getSrvType() const
{
    return m_srvtype;
}

String SLPResult::getHost() const
{
    return m_host;
}

String SLPResult::getFamily() const
{
    return m_family;
}

String SLPResult::getSrvPart() const
{
    return m_srvpart;
}

int SLPResult::getPort() const
{
    return m_port;
}

bp::object SLPResult::getPySrvType() const
{
    return StringConv::asPyUnicode(m_srvtype);
}

bp::object SLPResult::getPyHost() const
{
    return StringConv::asPyUnicode(m_host);
}

bp::object SLPResult::getPyFamily() const
{
    return StringConv::asPyUnicode(m_family);
}

bp::object SLPResult::getPySrvPart() const
{
    return StringConv::asPyUnicode(m_srvpart);
}

bp::object SLPResult::getPyPort() const
{
    return bp::object(m_port);
}

void SLPResult::setSrvType(const String &srvtype)
{
    m_srvtype = srvtype;
}

void SLPResult::setHost(const String &host)
{
    m_host = host;
}

void SLPResult::setFamily(const String &family)
{
    m_family = family;
}

void SLPResult::setSrvPart(const String &srvpart)
{
    m_srvpart = srvpart;
}

void SLPResult::setPort(int port)
{
    m_port = port;
}

void SLPResult::setPySrvType(const bp::object &srvtype)
{
    m_srvtype = StringConv::asString(srvtype, "srvtype");
}

void SLPResult::setPyHost(const bp::object &host)
{
    m_host = StringConv::asString(host, "host");
}

void SLPResult::setPyFamily(const bp::object &family)
{
    m_family = StringConv::asString(family, "family");
}

void SLPResult::setPySrvPart(const bp::object &srvpart)
{
    m_srvpart = StringConv::asString(srvpart, "srvpart");
}

void SLPResult::setPyPort(const bp::object &port)
{
    m_port = Conv::as<int>(port, "port");
}
