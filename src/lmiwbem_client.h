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

#ifndef   LMIWBEM_CLIENT_H
#  define LMIWBEM_CLIENT_H

#  include <Pegasus/Client/CIMClient.h>
#  include <Pegasus/Common/CIMType.h>
#  include <Pegasus/Common/SSLContext.h>
#  include "lmiwbem_urlinfo.h"
#  include "lmiwbem_mutex.h"
#  include "util/lmiwbem_string.h"

class CIMClient;

class CIMClient: public Pegasus::CIMClient
{
public:
    // Every CIMClient operation needs to have a guard.
    //
    // Example:
    //     CIMClient client;
    //     CIMClient::ScopedCIMClientTransaction sc(client);
    //     ... further CIM operations ...
    class ScopedCIMClientTransaction
    {
    public:
        ScopedCIMClientTransaction(CIMClient &client);
        ~ScopedCIMClientTransaction();
    private:
        CIMClient &m_client;
    };

    friend class ScopedCIMClientTransaction;

public:
    CIMClient();

    void connect(
        const String &uri,
        const String &username,
        const String &password,
        const String &cert_file,
        const String &key_file,
        const String &trust_store);
    void connectLocally();
    void disconnect();
    bool isConnected();

    void setVerifyCertificate(bool verify = true);
    bool getVerifyCertificate() const;
    URLInfo getURLInfo() const;
    String hostname() const;

private:
    // We hide these from Pegasus::CIMClient
    using Pegasus::CIMClient::connect;
    using Pegasus::CIMClient::connectLocal;

    static bool matchPattern(const Pegasus::String &pattern, const Pegasus::String &str);

#  ifdef HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA
    static Pegasus::Boolean verifyCertificate(Pegasus::SSLCertificateInfo &ci, void *data);
#  else
    static Pegasus::Boolean verifyCertificate(Pegasus::SSLCertificateInfo &ci);
#  endif // HAVE_PEGASUS_VERIFICATION_CALLBACK_WITH_DATA

    URLInfo m_url_info;
    Mutex m_mutex;
    bool m_is_connected;
    bool m_verify_cert;
};

#endif // LMIWBEM_CLIENT_H
