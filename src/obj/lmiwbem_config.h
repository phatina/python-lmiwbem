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

#ifndef   LMIWBEM_CONFIG_H
#  define LMIWBEM_CONFIG_H

#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

namespace bp = boost::python;

BOOST_PYTHON_BEGIN
class object;
BOOST_PYTHON_END

class Config
{
public:
    enum {
        EXC_VERB_NONE,
        EXC_VERB_CALL,
        EXC_VERB_MORE
    };

    static Config *instance();

    static String getDefaultNamespace();
    static String getDefaultTrustStore();
    static void setDefaultNamespace(const String &def_namespace);
    static void setDefaultTrustStore(const String &def_trust_store);

    static int getExceptionVerbosity();
    static bool isVerbose();
    static bool isVerboseCall();
    static bool isVerboseMore();
    static void setExceptionVerbosity(const int verbosity);

private:
    Config();

    static boost::shared_ptr<Config> s_inst_ptr;

    String m_def_namespace;
    String m_def_trust_store;
    int m_exc_verbosity;
};

class ConfigProxy: public CIMBase<ConfigProxy>
{
public:
    ConfigProxy();

    static void init_type();

    bp::object repr() const;

    bp::object getPyDefaultNamespace() const;
    bp::object getPyDefaultTrustStore() const;
    bp::object getPyExcVerbosity() const;
    bp::object getPySupportsPullOp() const;
    bp::object getPySupportsWSMAN() const;

    void setPyDefaultNamespace(const bp::object &def_namespace);
    void setPyDefaultTrustStore(const bp::object &def_trust_store);
    void setPyExceptionVerbosity(const bp::object &exc_verbosity);

protected:
    static Config *instance();
};

#endif // LMIWBEM_CONFIG_H
