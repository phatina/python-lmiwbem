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

#ifndef   LMIWBEM_CLIENT_WSMAN_REQUEST_H
#  define LMIWBEM_CLIENT_WSMAN_REQUEST_H

#  include <map>
#  include "util/lmiwbem_string.h"

class Request
{
public:
    Request();
    Request(
        const String &hostname,
        const String &namespace_,
        const String &classname,
        const bool use_wildcard = false);
    Request(
        const String &hostname,
        const String &namespace_,
        const String &query_language,
        const String &query);

    void setHostname(const String &hostname);
    void setNamespace(const String &namespace_);
    void setClassname(const String &classname);

    String getHostname() const;
    String getNamespace() const;
    String getClassname() const;
    String getQueryDialect() const;
    String getQueryLanguage() const;
    String getQuery() const;

    bool isWildcard() const;
    bool isValid() const;

    String asString() const;
    operator String() const;

private:
    String getScheme(const bool for_query = false) const;
    static std::map<String, String> makeSchemes();
    static std::map<String, String> s_schemes;

    String m_hostname;
    String m_namespace;
    String m_classname;
    String m_query_language;
    String m_query;
    bool m_use_wildcard;
};

#endif // LMIWBEM_CLIENT_WSMAN_REQUEST_H
