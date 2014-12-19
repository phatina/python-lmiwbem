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

#include "lmiwbem_client.h"
#include "lmiwbem_config.h"
#include "obj/lmiwbem_connection_rep.h"
#include "util/lmiwbem_string.h"

WBEMConnectionRep::WBEMConnectionRep()
    : m_connected_tmp(false)
    , m_connect_locally(false)
    , m_url()
    , m_username()
    , m_password()
    , m_cert_file()
    , m_key_file()
    , m_default_namespace(Config::defaultNamespace())
    , m_client()
{
}
