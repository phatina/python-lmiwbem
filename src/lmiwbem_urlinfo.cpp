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
#include <cctype>
#include <cerrno>
#include <algorithm>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "lmiwbem_urlinfo.h"

namespace {

const unsigned int BUFLEN = 1024;

String get_fqdn()
{
    struct addrinfo hints, *info, *p;
    int gai_result;
    char name[BUFLEN + 1];
    char port[BUFLEN];

    if (gethostname(name, BUFLEN) < 0)
        return String("localhost");

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // either IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    snprintf(port, BUFLEN, "%d", URLInfo::PORT_CIMXML_HTTPS);
    if ((gai_result = getaddrinfo(name, port, &hints, &info)) == 0) {
        for (p = info; p != NULL; p = p->ai_next) {
            if (p->ai_canonname != NULL) {
                snprintf(name, BUFLEN, "%s", p->ai_canonname);
                break;
            }
        }
        freeaddrinfo(info);
    }
    return String(name);
}

bool is_path_delimiter(int i)
{
    // A bit simplified version how to determine a URL's path. This should work
    // for most of occasions. If an bug occurs, we would need to implement
    // delimiter wrt:
    //
    //    path = path-abempty    ; begins with "/" or is empty
    //         | path-absolute   ; begins with "/" but not "//"
    //         | path-noscheme   ; begins with a non-colon segment
    //         | path-rootless   ; begins with a segment
    //         | path-empty      ; zero characters
    //
    // See RFC 3986; https://tools.ietf.org/html/rfc3986
    return i == '/';
}

} // unnamed namespace

URLInfo::URLInfo()
    : m_url("https://unknown")
    , m_hostname("unknown")
    , m_username()
    , m_password()
    , m_path()
    , m_error()
    , m_port(URLInfo::PORT_NOT_SET)
    , m_is_https(true)
    , m_is_local(false)
    , m_is_valid(false)
    , m_is_creds_valid(false)
{
}

URLInfo::URLInfo(const String &url)
    : m_url("https://unknown")
    , m_hostname("unknown")
    , m_username()
    , m_password()
    , m_path()
    , m_error()
    , m_port(URLInfo::PORT_NOT_SET)
    , m_is_https(true)
    , m_is_local(false)
    , m_is_valid(false)
    , m_is_creds_valid(false)
{
    set(url);
}

URLInfo::URLInfo(const URLInfo &copy)
    : m_url(copy.m_url)
    , m_hostname(copy.m_hostname)
    , m_username(copy.m_username)
    , m_password(copy.m_password)
    , m_path(copy.m_path)
    , m_error(copy.m_error)
    , m_port(copy.m_port)
    , m_is_https(copy.m_is_https)
    , m_is_local(copy.m_is_local)
    , m_is_valid(copy.m_is_valid)
    , m_is_creds_valid(copy.m_is_creds_valid)
{
}

bool URLInfo::set(const String &url)
{
    m_url = url;

    // Handle local connection.
    if (isLocalhost(m_url)) {
        m_is_https = false;
        m_is_local = true;
        m_hostname = get_fqdn();
        m_port = 0;
        return m_is_valid = true;
    }

    // Handle remote connection
    size_t spos;
    m_port = PORT_NOT_SET;
    if (m_url.substr(0, 7) == "http://") {
        spos = 7;
        m_is_https = false;
    } else if (m_url.substr(0, 8) == "https://") {
        spos = 8;
        m_is_https = true;
    } else {
        return setInvalid("Invalid scheme: " + url);
    }

    // Parse username:password from URL.
    size_t pos = m_url.find('@', spos);
    if (pos != String::npos) {
        String creds = m_url.substr(spos, pos - spos);
        size_t cpos = creds.find(':');
        if (cpos != String::npos) {
            m_username = creds.substr(0, cpos);
            m_password = creds.substr(cpos + 1);
            m_is_creds_valid = true;
        } else {
            m_is_creds_valid = false;
        }

        // Remove username:password information from the URL.
        m_url.erase(spos, creds.length() + 1);
    }

    // Parse port from URL.
    pos = m_url.find(':', spos);
    if (pos != String::npos) {
        // Store a hostname.
        m_hostname = m_url.substr(spos, pos - spos);

        // Search for the end of port.
        String::iterator begin = m_url.begin() + pos + 1;
        String::iterator end = std::find_if(
            begin, m_url.end(), is_path_delimiter);
        size_t len = end - begin;
        if (len == 0 || len > 5)
            return setInvalid("Port number out of range: " + url);

        // Store a port.
        char *pend;
        long int port = strtol(m_url.substr(pos + 1, len).c_str(), &pend, 10);
        if (*pend != '\0' || errno == ERANGE || port < 0 || port > 65535)
            return setInvalid("Port number out of range: " + url);
        m_port = static_cast<uint32_t>(port);

        spos = pos + len + 1;
    } else {
        pos = m_url.find('/', spos);
        m_hostname = m_url.substr(spos, pos - spos);
        spos += pos - spos;
    }

    // If we haven't reached the end of URL, the rest is path.
    if (spos != String::npos)
        m_path = m_url.substr(spos, String::npos);

    // Now that we have read all the URL string, we can assign a default port
    // value to m_port, if it's not set.
    if (m_port == PORT_NOT_SET) {
        if (m_path == "/wsman")
            m_port = m_is_https ? PORT_WSMAN_HTTPS : PORT_WSMAN_HTTP;
        else
            m_port = m_is_https ? PORT_CIMXML_HTTPS : PORT_CIMXML_HTTP;
    }

    return m_is_valid = !m_hostname.empty();
}

String URLInfo::url() const
{
    std::stringstream ss;

    if (m_is_https)
        ss << "https://";
    else
        ss << "http://";

    ss << m_hostname << ':' << m_port << m_path;

    return String(ss.str());
}

String URLInfo::hostname() const
{
    return m_hostname;
}

String URLInfo::username() const
{
    return m_username;
}

String URLInfo::password() const
{
    return m_password;
}

String URLInfo::path() const
{
    return m_path;
}

uint32_t URLInfo::port() const
{
    return m_port;
}

String URLInfo::error() const
{
    return m_error;
}

bool URLInfo::isHttps() const
{
    return m_is_https;
}

bool URLInfo::isLocal() const
{
    return m_is_local;
}

bool URLInfo::isValid() const
{
    return m_is_valid;
}

bool URLInfo::isCredsValid() const
{
    return m_is_creds_valid;
}

bool URLInfo::isWSMAN() const
{
    String path(m_path.length());
    std::transform(m_path.begin(), m_path.end(), path.begin(), ::tolower);

    return path == String("/wsman");
}

URLInfo &URLInfo::operator=(const URLInfo &rhs)
{
    m_url = rhs.m_url;
    m_hostname = rhs.m_hostname;
    m_username = rhs.m_username;
    m_password = rhs.m_password;
    m_path = rhs.m_path;
    m_error = rhs.m_error;
    m_port = rhs.m_port;
    m_is_https = rhs.m_is_https;
    m_is_local = rhs.m_is_local;
    m_is_valid = rhs.m_is_valid;
    m_is_creds_valid = rhs.m_is_creds_valid;
    return *this;
}

bool URLInfo::isLocalhost(const String &url)
{
    return url.substr(0, 7) == "file://" ||
        url == "localhost" ||
        url == "localhost.localdomain" ||
        url == "localhost4" ||
        url == "localhost4.localdomain4" ||
        url == "localhost6" ||
        url == "localhost6.localdomain6" ||
        url == "127.0.0.1" || url == "::1";
}

bool URLInfo::setInvalid(const String &error)
{
    m_error = error;
    return m_is_valid = false;
}

#ifdef DEBUG
#  include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Wrong argument count!" << std::endl << std::endl
                  << "Usage: " << argv[0] << " [URL]" << std::endl;
        return 1;
    }

    URLInfo info(argv[1]);

    if (!info.isValid()) {
        std::cerr << info.error() << std::endl;
        return 2;
    }

    std::cout << "hostname: " << info.hostname() << std::endl;
    std::cout << "port    : " << info.port() << std::endl;
    std::cout << "path    : " << info.path() << std::endl;

    if (info.isCredsValid()) {
        std::cout << "username: " << info.username() << std::endl;
        std::cout << "password: " << info.password() << std::endl;
    }

    std::cout << std::endl << info.url() << std::endl;

    return 0;
}

#endif
