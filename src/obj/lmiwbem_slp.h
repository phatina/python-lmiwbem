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

#ifndef   LMIWBEM_SLP_H
#  define LMIWBEM_SLP_H

#  include <slp.h>
#  include "lmiwbem.h"
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

BOOST_PYTHON_BEGIN
class object;
BOOST_PYTHON_END

namespace bp = boost::python;

class ScopedSLPHandle
{
public:
    ScopedSLPHandle(
        const bool is_async = false,
        const String pc_lang = String());
    ~ScopedSLPHandle();

    SLPHandle handle();
    SLPError error() const;
    bool good() const;

    bool operator!() const;
    operator SLPHandle() const;

private:
    SLPHandle m_handle;
    SLPError m_error;
};

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

    static SLPBoolean attrCallback(
        SLPHandle hslp,
        const char *attrlist,
        SLPError errcode,
        void *cookie);

    static bp::object discover(
        const bp::object &srvtype,
        const bp::object &scopelist,
        const bp::object &filter,
        const bp::object &async);

    static bp::object discoverAttrs(
        const bp::object &srvurl,
        const bp::object &scopelist,
        const bp::object &attrids,
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

    String repr();

    String getSrvType() const;
    String getHost() const;
    String getFamily() const;
    String getSrvPart() const;
    int getPort() const;
    bp::object getPySrvType() const;
    bp::object getPyHost() const;
    bp::object getPyFamily() const;
    bp::object getPySrvPart() const;
    bp::object getPyPort() const;

    void setSrvType(const String &srvtype);
    void setHost(const String &host);
    void setFamily(const String &family);
    void setSrvPart(const String &srvpart);
    void setPort(int port);
    void setPySrvType(const bp::object &srvtype);
    void setPyHost(const bp::object &host);
    void setPyFamily(const bp::object &family);
    void setPySrvPart(const bp::object &srvpart);
    void setPyPort(const bp::object &port);

private:
    String m_srvtype;
    String m_host;
    String m_family;
    String m_srvpart;
    int m_port;
};

#endif // LMIWBEM_SLP_H
