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
#include <stdexcept>
#include <sstream>
#include <boost/python/class.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/object.hpp>
#include <boost/python/scope.hpp>
#include "obj/lmiwbem_config.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

boost::shared_ptr<Config> Config::s_inst_ptr;

namespace {

const char *KEY_DEF_NAMESPACE    = "DEFAULT_NAMESPACE";
const char *KEY_DEF_TRUST_STORE  = "DEFAULT_TRUST_STORE";
const char *KEY_EXC_VERBOSITY    = "EXCEPTION_VERBOSITY";
const char *KEY_EXC_VERB_NONE    = "EXC_VERB_NONE";
const char *KEY_EXC_VERB_CALL    = "EXC_VERB_CALL";
const char *KEY_EXC_VERB_MORE    = "EXC_VERB_MORE";
const char *KEY_SUPPORTS_PULL_OP = "SUPPORTS_PULL_OPERATIONS";
const char *KEY_SUPPORTS_WSMAN   = "SUPPORTS_WSMAN";

} // Unnamed namespace

Config::Config()
    : m_def_namespace(DEFAULT_NAMESPACE)
    , m_def_trust_store(DEFAULT_TRUST_STORE)
    , m_exc_verbosity(EXC_VERB_NONE)
{
}

Config *Config::instance()
{
    if (!s_inst_ptr)
        s_inst_ptr.reset(new Config);
    return s_inst_ptr.get();
}

String Config::getDefaultNamespace()
{
    return instance()->m_def_namespace;
}

String Config::getDefaultTrustStore()
{
    return instance()->m_def_trust_store;
}

void Config::setDefaultNamespace(const String &def_namespace)
{
    instance()->m_def_namespace = def_namespace;
}

void Config::setDefaultTrustStore(const String &def_trust_store)
{
    instance()->m_def_trust_store = def_trust_store;
}

int Config::getExceptionVerbosity()
{
    return instance()->m_exc_verbosity;
}

bool Config::isVerbose()
{
    return instance()->isVerboseCall() || instance()->isVerboseMore();
}

bool Config::isVerboseCall()
{
    return instance()->m_exc_verbosity == EXC_VERB_CALL;
}

bool Config::isVerboseMore()
{
    return instance()->m_exc_verbosity == EXC_VERB_MORE;
}

void Config::setExceptionVerbosity(const int verbosity)
{
    switch (verbosity) {
    case EXC_VERB_NONE:
    case EXC_VERB_CALL:
    case EXC_VERB_MORE:
        break;
    default:
        throw std::out_of_range("EXCEPTION_VERBOSITY contains unexpected value");
    }

    instance()->m_exc_verbosity = verbosity;
}


// -----------------------------------------------------------------------------

ConfigProxy::ConfigProxy()
{
}

void ConfigProxy::init_type()
{
    CIMBase<ConfigProxy>::init_type(
        bp::class_<ConfigProxy>("_ConfigProxy", bp::init<>())
        .def("__repr__", &ConfigProxy::repr)
        .add_property(KEY_DEF_NAMESPACE,
            &ConfigProxy::getPyDefaultNamespace,
            &ConfigProxy::setPyDefaultNamespace)
        .add_property(KEY_DEF_TRUST_STORE,
            &ConfigProxy::getPyDefaultTrustStore,
            &ConfigProxy::setPyDefaultTrustStore)
        .add_property(KEY_EXC_VERBOSITY,
            &ConfigProxy::getPyExcVerbosity,
            &ConfigProxy::setPyExceptionVerbosity)
        .add_property(KEY_SUPPORTS_PULL_OP,
            &ConfigProxy::getPySupportsPullOp)
        .add_property(KEY_SUPPORTS_WSMAN,
            &ConfigProxy::getPySupportsWSMAN));

    bp::scope().attr(KEY_EXC_VERB_NONE) = static_cast<int>(Config::EXC_VERB_NONE);
    bp::scope().attr(KEY_EXC_VERB_CALL) = static_cast<int>(Config::EXC_VERB_CALL);
    bp::scope().attr(KEY_EXC_VERB_MORE) = static_cast<int>(Config::EXC_VERB_MORE);

    bp::object inst = ConfigProxy::create();
    bp::scope().attr("config") = inst;
    bp::scope().attr(KEY_DEF_NAMESPACE) = inst.attr(KEY_DEF_NAMESPACE);
}

bp::object ConfigProxy::repr() const
{
    std::stringstream ss;
    ss << "Config(ns='" << Config::instance()->getDefaultNamespace()
       << "', trust_store='" << Config::instance()->getDefaultTrustStore()
       << "', verbosity='";
    switch (Config::instance()->getExceptionVerbosity()) {
    case Config::EXC_VERB_NONE:
        ss << "EXC_VERB_NONE";
        break;
    case Config::EXC_VERB_CALL:
        ss << "EXC_VERB_CALL";
        break;
    case Config::EXC_VERB_MORE:
        ss << "EXC_VERB_MORE";
        break;
    }
    ss << "')";

    return StringConv::asPyUnicode(ss.str());
}


bp::object ConfigProxy::getPyDefaultNamespace() const
{
    return StringConv::asPyUnicode(Config::instance()->getDefaultNamespace());
}

bp::object ConfigProxy::getPyDefaultTrustStore() const
{
    return StringConv::asPyUnicode(Config::instance()->getDefaultTrustStore());
}

bp::object ConfigProxy::getPyExcVerbosity() const
{
    return bp::object(Config::instance()->getExceptionVerbosity());
}

bp::object ConfigProxy::getPySupportsPullOp() const
{
#ifdef HAVE_PEGASUS_ENUMERATION_CONTEXT
    return bp::object(true);
#else
    return bp::object(false);
#endif
}

bp::object ConfigProxy::getPySupportsWSMAN() const
{
#ifdef HAVE_OPENWSMAN
    return bp::object(true);
#else
    return bp::object(false);
#endif
}

void ConfigProxy::setPyDefaultNamespace(const bp::object &def_namespace)
{
    Config::instance()->setDefaultNamespace(
        StringConv::asString(
            def_namespace, KEY_DEF_NAMESPACE));
}

void ConfigProxy::setPyDefaultTrustStore(const bp::object &def_trust_store)
{
    Config::instance()->setDefaultTrustStore(
        StringConv::asString(
            def_trust_store, KEY_DEF_TRUST_STORE));
}

void ConfigProxy::setPyExceptionVerbosity(const bp::object &exc_verbosity)
{
    Config::instance()->setExceptionVerbosity(
        Conv::as<int>(exc_verbosity, KEY_EXC_VERBOSITY));
}
