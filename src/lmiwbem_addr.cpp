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

#include <cerrno>
#include "lmiwbem_addr.h"

Address::Address(Pegasus::String url)
    : m_hostname("unknown")
    , m_port(5989)
    , m_is_https(true)
    , m_is_valid(true)
{
    if (url.find("http://") != Pegasus::PEG_NOT_FOUND) {
        url.remove(0, 7);
        m_is_https = false;
    } else if (url.find("https://") != Pegasus::PEG_NOT_FOUND) {
        url.remove(0, 8);
    }

    Pegasus::Uint32 pos = url.reverseFind(':');
    if (pos != Pegasus::PEG_NOT_FOUND) {
        m_hostname = url.subString(0, pos);
        long int port = strtol(url.subString(pos + 1,
            url.size() - pos - 1).getCString(), NULL, 10);
        if (errno == ERANGE || port < 0 || port > 65535)
            m_is_valid = false;
        m_port = static_cast<Pegasus::Uint32>(port);
    } else {
        m_hostname = url;
    }
}
