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

#ifndef WBEM_CLIENT_ADDRESS_H
#define WBEM_CLIENT_ADDRESS_H

#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/String.h>

class Address
{
public:
    Address(Pegasus::String url);

    Pegasus::String hostname() const { return m_hostname; }
    Pegasus::Uint32 port() const { return m_port; }

    bool isHttps() const { return m_is_https; }
    bool isValid() const { return m_is_valid; }

private:
    Pegasus::String m_hostname;
    Pegasus::Uint32 m_port;
    bool m_is_https;
    bool m_is_valid;
};

#endif // WBEM_CLIENT_ADDRESS_H
