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

#ifndef   LMIWBEM_CONNECTION_REP_H
#  define LMIWBEM_CONNECTION_REP_H

class String;
class CIMClient;
class WBEMConnection;

class WBEMConnectionRep
{
public:
    WBEMConnectionRep();

    bool m_connected_tmp;
    bool m_connect_locally;
    String m_url;
    String m_username;
    String m_password;
    String m_cert_file;
    String m_key_file;
    String m_default_namespace;
    CIMClient m_client;
};

#endif // LMIWBEM_CONNECTION_REP_H
