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
#include <algorithm>
#include <cctype>
#include <sstream>
#include "lmiwbem_client_wsman_request.h"

std::map<String, String> Request::s_schemes = Request::makeSchemes();

Request::Request()
    : m_hostname()
    , m_namespace()
    , m_classname()
    , m_query_language()
    , m_query()
    , m_use_wildcard(false)
{
}

Request::Request(
    const String &hostname,
    const String &namespace_,
    const String &classname,
    const bool use_wildcard)
    : m_hostname(hostname)
    , m_namespace(namespace_)
    , m_classname(classname)
    , m_query_language()
    , m_query()
    , m_use_wildcard(use_wildcard)
{
}

Request::Request(
    const String &hostname,
    const String &namespace_,
    const String &query_language,
    const String &query)
    : m_hostname(hostname)
    , m_namespace(namespace_)
    , m_classname()
    , m_query_language(query_language)
    , m_query(query)
    , m_use_wildcard(true)
{
    String query_lower(query);
    std::transform(
        query_lower.begin(),
        query_lower.end(),
        query_lower.begin(),
        ::tolower);

    size_t begin = query_lower.find("from");
    if (begin == query_lower.npos)
        return;

    size_t end = query_lower.find("where", begin);

    std::stringstream ss(query.substr(begin + 4, end - begin - 4));

    int cnt;
    String cls;
    for (cnt = 0; cnt < 2 && getline(ss, cls, ','); ++cnt) {
        // Trim leading and trailing spaces.
        cls.erase(0, cls.find_first_not_of(' '));
        cls.erase(cls.find_last_not_of(' ') + 1);
    }

    if (cnt > 1 || find_if(cls.begin(), cls.end(), ::isspace) != cls.end()) {
        // We can't handle more classes in select; class name contains illegal
        // characters.
        return;
    }

    m_classname = cls;
}


void Request::setHostname(const String &hostname)
{
    m_hostname = hostname;
}

void Request::setNamespace(const String &namespace_)
{
    m_namespace = namespace_;
}

void Request::setClassname(const String &classname)
{
    m_classname = classname;
}

String Request::getHostname() const
{
    return m_hostname;
}

String Request::getNamespace() const
{
    return m_namespace;
}

String Request::getClassname() const
{
    return m_classname;
}

String Request::getQueryDialect() const
{
    std::stringstream ss;
    ss << getScheme(true) << '/' << m_query_language;
    return ss.str();
}

String Request::getQueryLanguage() const
{
    return m_query_language;
}

String Request::getQuery() const
{
    return m_query;
}

bool Request::isWildcard() const
{
    return m_use_wildcard;
}

bool Request::isValid() const
{
    return !m_namespace.empty() && !m_classname.empty();
}

Request::operator String() const
{
    return asString();
}

String Request::asString() const
{
    std::stringstream ss;

    // Scheme
    ss << getScheme() << '/';

    // Namespace
    ss << m_namespace << '/';

    // Class name
    if (m_use_wildcard)
        ss << '*';
    else
        ss << m_classname;

    return String(ss.str());
}

std::map<String, String> Request::makeSchemes()
{
    std::map<String, String> schemes;
    schemes["CIM"] = "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2";
    schemes["PRS"] = "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2";
    schemes["Win32"] = "http://schemas.microsoft.com/wbem/wsman/1/wmi";
    schemes["OpenWBEM"] = "http://schema.openwbem.org/wbem/wscim/1/cim-schema/2";
    schemes["Linux"] = "http://sblim.sf.net/wbem/wscim/1/cim-schema/2";
    schemes["OMC"] = "http://schema.omc-project.org/wbem/wscim/1/cim-schema/2";
    schemes["PG"] = "http://schema.openpegasus.org/wbem/wscim/1/cim-schema/2";
    schemes["AMT"] = "http://intel.com/wbem/wscim/1/amt-schema/1";
    schemes["IPS"] = "http://intel.com/wbem/wscim/1/ips-schema/1";
    schemes["Sun"] = "http://schemas.sun.com/wbem/wscim/1/cim-schema/2";
    schemes["Msvm"] = "http://schemas.microsoft.com/wbem/wsman/1/wmi";
    // XXX: Dell's iDrac uses this schema
    // schemes["DCIM"] = "http://schemas.dmtf.org/wbem/wscim/1/cim-schema/2";
    schemes["DCIM"] = "http://schemas.dell.com/wbem/wscim/1/cim-schema/2";
    // Query dialects...
    schemes["WQL"] = "http://schemas.microsoft.com/wbem/wsman/1";
    return schemes;
}

String Request::getScheme(const bool for_query) const
{
    std::map<String, String>::const_iterator it;

    if (for_query) {
        it = s_schemes.find(m_query_language);
        if (it != s_schemes.end())
            return it->second;
    } else {
        for (it = s_schemes.begin(); it != s_schemes.end(); ++it) {
            if (m_classname.find(it->first) != m_classname.npos)
                return it->second;
        }
    }

    return String();
}
