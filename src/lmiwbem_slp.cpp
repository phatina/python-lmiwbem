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
#include <list>
#include <string>
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/str.hpp>
#include <boost/python/tuple.hpp>
#include "lmiwbem_exception.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_slp.h"
#include "lmiwbem_util.h"

void SLP::init_type()
{
    bp::def("slp_discover", SLP::discover,
        (bp::arg("srvtype") = "",
         bp::arg("scopelist") = "",
         bp::arg("filter") = "",
         bp::arg("async") = false),
        "Peforms SLP discovery.\n\n"
        ":param str srvtype: service type\n"
        ":param str scopelist: comma separated list of scope names\n"
        ":param str filter: query formulated of attribute pattern matching"
        "\texpressions in the form of an LDAPv3 search filter\n"
        ":returns: list of :py:class:`.SLPResult`\n"
        ":raises: :py:exc:`.SLPError`");
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

        bp::list *srvs = static_cast<bp::list *>(cookie);
        srvs->append(SLPResult::create(url));

        SLPFree(url);
    }

    return SLP_TRUE;
}


bp::object SLP::discover(
    const bp::object &srvtype,
    const bp::object &scopelist,
    const bp::object &filter,
    const bp::object &async)
{
    std::string std_srvtype = lmi::extract_or_throw<std::string>(
        srvtype, "srvtype");
    std::string std_scopelist = lmi::extract_or_throw<std::string>(
        scopelist, "scopelist");
    std::string std_filter = lmi::extract_or_throw<std::string>(
        filter, "filter");
    bool std_async = lmi::extract_or_throw<bool>(async, "async");

    SLPError  err;
    SLPHandle hslp;

    // Open SLP handle.
    if ((err = SLPOpen(NULL, std_async ? SLP_TRUE : SLP_FALSE, &hslp)) != SLP_OK)
        throw_SLPError("Can't open SLP handle", static_cast<int>(err));

    // Discover the services.
    bp::list srvs;
    if ((err = SLPFindSrvs(hslp, std_srvtype.c_str(), std_scopelist.c_str(),
        std_filter.c_str(), SLP::urlCallback, static_cast<void*>(&srvs))) != SLP_OK)
    {
        throw_SLPError("SLP discovery failed", static_cast<int>(err));
    }

    // Close SLP handle.
    SLPClose(hslp);

    return srvs;
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
{
    m_srvtype = lmi::extract_or_throw<std::string>(srvtype, "srvtype");
    m_host = lmi::extract_or_throw<std::string>(host, "host");
    m_port = lmi::extract_or_throw<int>(port, "port");
    m_family = lmi::extract_or_throw<std::string>(family, "family");
    m_srvpart = lmi::extract_or_throw<std::string>(srvpart, "srvpart");
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
                bp::arg("family") = std::string(),
                bp::arg("srvpart") = std::string()),
                "Constructs a :py:class:`.CIMClass`.\n\n"
                ":param str srvtype: service type\n"
                ":param str host: host name\n"
                ":param int port: service port\n"
                ":param str family: network address family\n"
                ":param str srvpart: remainder of a URL (see SLP)"))
        .def("__repr__", &SLPResult::repr)
        .add_property("srvtype",
            &SLPResult::getSrvType,
            &SLPResult::setSrvType,
            "Property storing service type.\n\n"
            ":rtype: str")
        .add_property("host",
            &SLPResult::getHost,
            &SLPResult::setHost,
            "Property storing host name of the service.\n\n"
            ":rtype: str")
        .add_property("port",
            &SLPResult::getPort,
            &SLPResult::setPort,
            "Property storing port of the service.\n\n"
            ":rtype: int")
        .add_property("family",
            &SLPResult::getFamily,
            &SLPResult::setFamily,
            "Property storing network address of the service.\n\n"
            ":rtype: str")
        .add_property("srvpart",
            &SLPResult::getSrvPart,
            &SLPResult::setSrvPart,
            "Property storing remainder of the service URL.\n\n"
            ":rtype: str"));
}

bp::object SLPResult::create(const SLPSrvURL *url)
{
    bp::object inst = CIMBase<SLPResult>::create();
    SLPResult &fake_this = lmi::extract<SLPResult&>(inst);

    fake_this.m_srvtype = std::string(url->s_pcSrvType);
    fake_this.m_host = std::string(url->s_pcHost);
    fake_this.m_port = url->s_iPort;
    fake_this.m_family = std::string(url->s_pcNetFamily);
    fake_this.m_srvpart = std::string(url->s_pcSrvPart);

    return inst;
}

std::string SLPResult::repr()
{
    std::stringstream ss;
    ss << "SLPResult(srvtype='" << m_srvtype
       << "', host='" << m_host << "', port='"
       << m_port << "', ...)";
    return ss.str();
}

void SLPResult::setSrvType(const bp::object &srvtype)
{
    m_srvtype = lmi::extract_or_throw<std::string>(srvtype, "srvtype");
}

void SLPResult::setHost(const bp::object &host)
{
    m_host = lmi::extract_or_throw<std::string>(host, "host");
}

void SLPResult::setFamily(const bp::object &family)
{
    m_family = lmi::extract_or_throw<std::string>(family, "family");
}

void SLPResult::setSrvPart(const bp::object &srvpart)
{
    m_srvpart = lmi::extract_or_throw<std::string>(srvpart, "srvpart");
}

void SLPResult::setPort(const bp::object &port)
{
    m_port = lmi::extract_or_throw<int>(port, "port");
}
