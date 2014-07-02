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

#ifndef LMIWBEM_SLP_H
#define LMIWBEM_SLP_H

#include <slp.h>
#include <string>
#include <boost/python/object.hpp>
#include "lmiwbem_cimbase.h"

namespace bp = boost::python;

class SLP
{
public:
    static void init_type();

    static SLPBoolean urlCallback(
        SLPHandle hslp,
        const char *srvurl,
        unsigned short lifetime,
        SLPError errcode,
        void *cookie);

    static bp::object discover(
        const bp::object &srvtype,
        const bp::object &scopelist,
        const bp::object &filter,
        const bp::object &async);
};

class SLPResult: public CIMBase<SLPResult>
{
public:
    SLPResult();
    SLPResult(
        const bp::object &srvtype,
        const bp::object &host,
        const bp::object &port,
        const bp::object &family,
        const bp::object &srvpart);

    static void init_type();

    static bp::object create(const SLPSrvURL *url);

    std::string repr();

    std::string getSrvType() const { return m_srvtype; }
    std::string getHost() const { return m_host; }
    std::string getFamily() const { return m_family; }
    std::string getSrvPart() const { return m_srvpart; }
    int getPort() const { return m_port; }

    void setSrvType(const bp::object &srvtype);
    void setHost(const bp::object &host);
    void setFamily(const bp::object &family);
    void setSrvPart(const bp::object &srvpart);
    void setPort(const bp::object &port);

private:
    std::string m_srvtype;
    std::string m_host;
    std::string m_family;
    std::string m_srvpart;
    int m_port;
};

#endif // LMIWBEM_SLP_H
