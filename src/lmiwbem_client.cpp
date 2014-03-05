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

#include <Pegasus/Common/SSLContext.h>
#include "lmiwbem_client.h"
#include "lmiwbem_addr.h"
#include "lmiwbem_exception.h"

#include <cctype>

CIMClient::CIMClient()
    : Pegasus::CIMClient()
    , m_hostname()
    , m_is_connected(false)
    , m_verify_cert(true)
{
}

void CIMClient::connect(
    const Pegasus::String &uri,
    const Pegasus::String &username,
    const Pegasus::String &password,
    const Pegasus::String &trust_store)
{
    Address addr(uri);
    if (!addr.isValid()) {
        throw_CIMError("Invalid host address");
        return;
    }

    m_hostname = std::string(addr.hostname().getCString());
    if (!addr.isHttps()) {
        Pegasus::CIMClient::connect(addr.hostname(), addr.port(), username, password);
    } else {
        Pegasus::SSLContext ctx(trust_store,
            m_verify_cert ? verifyCertificate : NULL
#ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
            , this);
#else
            );
#endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
        Pegasus::CIMClient::connect(addr.hostname(), addr.port(), ctx, username, password);
    }
    m_is_connected = true;
}

void CIMClient::connectLocally()
{
    Pegasus::CIMClient::connectLocal();
    m_is_connected = true;
}

void CIMClient::disconnect()
{
    Pegasus::CIMClient::disconnect();
    m_hostname.clear();
    m_is_connected = false;
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
    Pegasus::String hostname(fake_this->m_hostname.c_str());

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
