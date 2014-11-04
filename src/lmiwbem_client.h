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

#  include <string>
#  include <Pegasus/Client/CIMClient.h>
#  include <Pegasus/Common/CIMType.h>
#  include <Pegasus/Common/SSLContext.h>
#  include "lmiwbem_addr.h"
#  include "lmiwbem_mutex.h"

class CIMClient: public Pegasus::CIMClient
{
public:
    CIMClient();

    void connect(
        const Pegasus::String &uri,
        const Pegasus::String &username,
        const Pegasus::String &password,
        const Pegasus::String &cert_file,
        const Pegasus::String &key_file,
        const Pegasus::String &trust_store);
    void connectLocally();
    void disconnect();
    bool isConnected();

    void setVerifyCertificate(bool verify = true) { m_verify_cert = verify; }
    bool getVerifyCertificate() const { return m_verify_cert; }
    Address getAddressInfo() const { return m_addr_info; }

    std::string hostname() const
    {
        return std::string(m_addr_info.hostname().getCString());
    }

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

    Address m_addr_info;
    Mutex m_mutex;
    bool m_is_connected;
    bool m_verify_cert;
};

#endif // LMIWBEM_CLIENT_H
