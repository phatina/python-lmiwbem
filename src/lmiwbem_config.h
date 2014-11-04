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

#ifndef   LMIWBEM_CONFIG_H
#  define LMIWBEM_CINFIG_H

#  include <string>

class Config
{
public:
    static void init_type();

    static std::string defaultNamespace();
    static std::string defaultTrustStore();

    static int exceptionVerbosity();
    static bool isVerbose();
    static bool isVerboseCall();
    static bool isVerboseMore();

private:
    enum {
        EXC_VERB_NONE,
        EXC_VERB_CALL,
        EXC_VERB_MORE
    };

    static const std::string DEF_NAMESPACE;
    static const std::string DEF_TRUST_STORE;
    static const int DEF_EXC_VERBOSITY;
};

#endif // LMIWBEM_CONFIG_H
