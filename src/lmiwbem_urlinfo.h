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

#ifndef   WBEM_CLIENT_ADDRESS_H
#  define WBEM_CLIENT_ADDRESS_H

#  include <Pegasus/Common/CIMType.h>
#  include <Pegasus/Common/String.h>

class URLInfo
{
public:
    URLInfo();
    URLInfo(const URLInfo &copy);

    bool set(Pegasus::String url);

    Pegasus::String hostname() const { return m_hostname; }
    Pegasus::Uint32 port() const { return m_port; }

    bool isHttps() const { return m_is_https; }

    std::string asStdString() const;

    URLInfo &operator =(const URLInfo &rhs);

private:
    static const Pegasus::Uint32 DEF_HTTPS_PORT = 5989;
    static const Pegasus::Uint32 DEF_HTTP_PORT  = 5988;

    Pegasus::String m_hostname;
    Pegasus::Uint32 m_port;
    bool m_is_https;
};

#endif // WBEM_CLIENT_ADDRESS_H
