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

#ifndef   WBEM_CLIENT_ADDRESS_H
#  define WBEM_CLIENT_ADDRESS_H

extern "C" {
// We can't include <cstdint>, because GNU stdlib requires compiler standard at
// least C++ 2011.
#  include <stdint.h>
}
#  include "util/lmiwbem_string.h"

class URLInfo
{
public:
    URLInfo();
    URLInfo(const URLInfo &copy);

    bool set(String url);

    String url() const;
    String hostname() const;
    String username() const;
    String password() const;
    uint32_t port() const;

    bool isHttps() const;
    bool isLocal() const;
    bool isValid() const;
    bool isCredsValid() const;

    String asString() const;

    URLInfo &operator =(const URLInfo &rhs);

    static const uint32_t DEF_HTTPS_PORT = 5989;
    static const uint32_t DEF_HTTP_PORT  = 5988;

private:
    String m_url;
    String m_hostname;
    String m_username;
    String m_password;
    uint32_t m_port;
    bool m_is_https;
    bool m_is_local;
    bool m_is_valid;
    bool m_is_creds_valid;
};

#endif // WBEM_CLIENT_ADDRESS_H
