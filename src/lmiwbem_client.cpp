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

#include <config.h>
#include <Pegasus/Common/SSLContext.h>
#include "lmiwbem_client.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_gil.h"
#include "obj/cim/lmiwbem_constants.h"

#include <cctype>

CIMClient::ScopedCIMClientTransaction::ScopedCIMClientTransaction(
    CIMClient &client)
    : m_client(client)
{
    m_client.m_mutex.lock();
}

CIMClient::ScopedCIMClientTransaction::~ScopedCIMClientTransaction()
{
    m_client.m_mutex.unlock();
}

CIMClient::CIMClient()
    : Pegasus::CIMClient()
    , m_url_info()
    , m_is_connected(false)
    , m_verify_cert(true)
{
}

void CIMClient::connect(
    const String &uri,
    const String &username,
    const String &password,
    const String &cert_file,
    const String &key_file,
    const String &trust_store)
{
    if (!m_url_info.isValid()) {
        throw_ConnectionError(
            "Invalid locator",
            CIMConstants::CON_ERR_INVALID_LOCATOR);
    }

    if (m_url_info.isLocal()) {
        connectLocally();
    } else if (!m_url_info.isHttps()) {
        Pegasus::CIMClient::connect(
            m_url_info.hostname(),
            m_url_info.port(),
            username,
            password);
    } else {
        Pegasus::SSLContext ctx(
            trust_store,
            cert_file,
            key_file,
            m_verify_cert ? verifyCertificate : NULL,
#ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
            this,
#endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
            String()
        );
        Pegasus::CIMClient::connect(
            m_url_info.hostname(),
            m_url_info.port(),
            ctx,
            username,
            password);
    }
    m_is_connected = true;
}

void CIMClient::connectLocally()
{
    Pegasus::CIMClient::connectLocal();
    m_is_connected = true;
    m_url_info.set("localhost");
}

void CIMClient::disconnect()
{
    Pegasus::CIMClient::disconnect();
    m_is_connected = false;
}

bool CIMClient::isConnected() const
{
    return m_is_connected;
}

void CIMClient::setVerifyCertificate(bool verify)
{
    m_verify_cert = verify;
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

#ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
bool CIMClient::matchPattern(const Pegasus::String &pattern, const Pegasus::String &str)
{
    Pegasus::String l_str(str);
    Pegasus::String l_pattern(pattern);
    l_str.toLower();
    l_pattern.toLower();

    Pegasus::Uint32 p = 0;
    Pegasus::Uint32 n = 0;

    const Pegasus::Uint32 str_len = l_str.size();
    const Pegasus::Uint32 pattern_len = l_pattern.size();
    for (; p < pattern_len && n < str_len; ++p, ++n) {
        // Got wildcard symbol; skip all of them, if there is more than one
        // asterisk in a row. When done, skip one domain name in the string.
        if (l_pattern[p] == '*') {
            for (; l_pattern[p] == '*' && p < pattern_len; ++p)
                ;
            for (; l_str[n] != '.' && n < str_len; ++n)
                ;
        }
        if (l_pattern[p] != l_str[n])
            return false;
    }

    return n == str_len;
}

Pegasus::Boolean CIMClient::verifyCertificate(Pegasus::SSLCertificateInfo &ci, void *data)
{
    if (!ci.getResponseCode()) {
        // Pre-verify of the certificate failed, immediate return
        return false;
    }

    CIMClient *fake_this = reinterpret_cast<CIMClient*>(data);
    Pegasus::String hostname(fake_this->m_url_info.hostname());

    // Verify against DNS names
    Pegasus::Array<Pegasus::String> dnsNames = ci.getSubjectAltNames().getDnsNames();
    for (Pegasus::Uint32 i = 0; i < dnsNames.size(); ++i) {
        if (matchPattern(dnsNames[i], hostname))
            return true;
    }

    // Verify against IP addresses
    Pegasus::Array<Pegasus::String> ipAddresses = ci.getSubjectAltNames().getIpAddresses();
    for (Pegasus::Uint32 i = 0; i < ipAddresses.size(); ++i) {
        if (matchPattern(ipAddresses[i], hostname))
            return true;
    }

    // Verify against Common Name
    return matchPattern(ci.getSubjectCommonName(), hostname);
}
#else
Pegasus::Boolean CIMClient::verifyCertificate(Pegasus::SSLCertificateInfo &ci)
{
    return ci.getResponseCode();
}
#endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
