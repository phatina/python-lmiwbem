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

#include <config.h>
#include <boost/python/errors.hpp>
#include <boost/python/scope.hpp>
#include "lmiwbem_config.h"
#include "lmiwbem_convert.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_util.h"

namespace {

const char *KEY_DEF_NAMESPACE   = "DEFAULT_NAMESPACE";
const char *KEY_DEF_TRUST_STORE = "DEFAULT_TRUST_STORE";
const char *KEY_EXC_VERBOSITY   = "EXCEPTION_VERBOSITY";
const char *KEY_EXC_VERB_NONE   = "EXC_VERB_NONE";
const char *KEY_EXC_VERB_CALL   = "EXC_VERB_CALL";
const char *KEY_EXC_VERB_MORE   = "EXC_VERB_MORE";

} // Unnamed namespace

const std::string Config::DEF_NAMESPACE     = DEFAULT_NAMESPACE;
const std::string Config::DEF_TRUST_STORE   = DEFAULT_TRUST_STORE;
const int         Config::DEF_EXC_VERBOSITY = EXC_VERB_NONE;

void Config::init_type()
{
    bp::scope().attr(KEY_DEF_NAMESPACE) = StringConv::asPyUnicode(DEF_NAMESPACE);
    bp::scope().attr(KEY_DEF_TRUST_STORE) = StringConv::asPyUnicode(DEF_TRUST_STORE);
    bp::scope().attr(KEY_EXC_VERBOSITY) = DEF_EXC_VERBOSITY;

    bp::scope().attr(KEY_EXC_VERB_NONE) = static_cast<int>(EXC_VERB_NONE);
    bp::scope().attr(KEY_EXC_VERB_CALL) = static_cast<int>(EXC_VERB_CALL);
    bp::scope().attr(KEY_EXC_VERB_MORE) = static_cast<int>(EXC_VERB_MORE);
}

std::string Config::defaultNamespace() try
{
    bp::object def_namespace(this_module().attr(KEY_DEF_NAMESPACE));
    return lmi::extract_or_throw<std::string>(def_namespace, KEY_DEF_NAMESPACE);
} catch (const bp::error_already_set &e) {
    this_module().attr(KEY_DEF_NAMESPACE) = StringConv::asPyUnicode(DEF_NAMESPACE);
    return DEF_NAMESPACE;
}

std::string Config::defaultTrustStore() try
{
    bp::object def_trust_store(this_module().attr(KEY_DEF_TRUST_STORE));
    return lmi::extract_or_throw<std::string>(def_trust_store, KEY_DEF_TRUST_STORE);
} catch (const bp::error_already_set &e) {
    this_module().attr(KEY_DEF_TRUST_STORE) = StringConv::asPyUnicode(DEF_TRUST_STORE);
    return DEF_TRUST_STORE;
}

int Config::exceptionVerbosity() try
{
    bp::object exc_verbosity(this_module().attr(KEY_EXC_VERBOSITY));
    int verbosity = lmi::extract_or_throw<int>(exc_verbosity, KEY_EXC_VERBOSITY);

    switch (verbosity) {
    case EXC_VERB_NONE:
    case EXC_VERB_CALL:
    case EXC_VERB_MORE:
        break;
    default:
        throw_ValueError("EXCEPTION_VERBOSITY contains unexpected value");
        return DEF_EXC_VERBOSITY;
    }

    return verbosity;
} catch (const bp::error_already_set &e) {
    this_module().attr(KEY_EXC_VERBOSITY) = DEF_EXC_VERBOSITY;
    return DEF_EXC_VERBOSITY;
}

bool Config::isVerbose()
{
    return isVerboseCall() || isVerboseMore();
}

bool Config::isVerboseCall()
{
    return exceptionVerbosity() == static_cast<int>(EXC_VERB_CALL);
}

bool Config::isVerboseMore()
{
    return exceptionVerbosity() == static_cast<int>(EXC_VERB_MORE);
}
