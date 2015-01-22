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

#include "lmiwbem_client.h"

CIMClient::ScopedCIMClientTransaction::ScopedCIMClientTransaction(
    CIMClient *client)
    : m_client(client)
{
    m_client->m_mutex.lock();
}

CIMClient::ScopedCIMClientTransaction::~ScopedCIMClientTransaction()
{
    m_client->m_mutex.unlock();
}

CIMClient::CIMClient()
    : m_url_info()
    , m_is_connected(false)
    , m_verify_cert(true)
{
}

CIMClient::~CIMClient()
{
}

bool CIMClient::isConnected() const
{
    return m_is_connected;
}

void CIMClient::setVerifyCertificate(bool verify)
{
    m_verify_cert = verify;
}

void CIMClient::setUrlInfo(const URLInfo &url_info)
{
    m_url_info = url_info;
}

bool CIMClient::setUrl(const String &url)
{
    return m_url_info.set(url);
}

bool CIMClient::getVerifyCertificate() const
{
    return m_verify_cert;
}

URLInfo CIMClient::getURLInfo() const
{
    return m_url_info;
}

String CIMClient::getHostname() const
{
    return m_url_info.hostname();
}

String CIMClient::getUrl() const
{
    return m_url_info.url();
}

