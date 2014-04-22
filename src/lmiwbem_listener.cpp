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
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <boost/python/class.hpp>
#include "lmiwbem_constants.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_extract.h"
#include "lmiwbem_instance.h"
#include "lmiwbem_listener.h"
#include "lmiwbem_make_method.h"
#include "lmiwbem_util.h"

CallableWithParams::CallableWithParams(
    const bp::object &callable,
    const bp::object &args,
    const bp::object &kwds)
    : m_callable(incref(callable))
    , m_args(args)
    , m_kwds(kwds)
{
}

CallableWithParams::CallableWithParams(const CallableWithParams &copy)
    : m_callable(copy.m_callable)
    , m_args(copy.m_args)
    , m_kwds(copy.m_kwds)
{
}

void CallableWithParams::call(const bp::object &indication)
{
    // We are called from non-Python thread, which holds a GIL lock only for
    // indication processing. We can't raise any exception here. Therefore
    // we use low-level Python C-API for function call, to prevent boost::python
    // from raising an exception, which would lead to thread lock.
    bp::object args = bp::make_tuple(indication) + m_args;
    PyObject *result = PyObject_Call(m_callable.ptr(), args.ptr(), m_kwds.ptr());
    if (!result || PyErr_Occurred()) {
        // XXX: What can we do here?
        // For now, print the stack trace and clear exception flag.
        PyErr_Print();
        PyErr_Clear();
    }
}

CallableWithParams &CallableWithParams::operator=(const CallableWithParams &rhs)
{
    m_callable = incref(rhs.m_callable);
    m_args = rhs.m_args;
    m_kwds = rhs.m_kwds;
    return *this;
}

// ----------------------------------------------------------------------------

CIMIndicationConsumer::CIMIndicationConsumer(CIMIndicationListener *listener)
    : m_listener(listener)
{
}

CIMIndicationConsumer::~CIMIndicationConsumer()
{
}

void CIMIndicationConsumer::consumeIndication(
    const Pegasus::OperationContext &context,
    const Pegasus::String &url,
    const Pegasus::CIMInstance &indication)
{
    // Acquire Python's GIL state
    PyGILState_STATE gstate = PyGILState_Ensure();

    m_listener->call(
        std::string(url.subString(1).getCString()),
        CIMInstance::create(indication));

    // Release the thread.
    PyGILState_Release(gstate);
}

CIMIndicationListener::CIMIndicationListener()
    : m_listener()
    , m_consumer(this)
    , m_handlers()
{
}

void CIMIndicationListener::init_type()
{
    CIMBase<CIMIndicationListener>::init_type(
        bp::class_<CIMIndicationListener>("CIMIndicationListener", bp::init<>())
            .def("start",  &CIMIndicationListener::start,
                (bp::arg("port"),
                 bp::arg("cert_file") = bp::object(),
                 bp::arg("key_file") = bp::object()),
                 bp::arg("trust_store") = bp::object())
            .def("stop", &CIMIndicationListener::stop)
            .def("add_handler",  lmi::raw_method<CIMIndicationListener>(&CIMIndicationListener::addHandler, 1))
            .def("remove_handler", &CIMIndicationListener::removeHandler)
            .def("is_alive", &CIMIndicationListener::isAlive)
            .add_property("uses_ssl", &CIMIndicationListener::usesSSL)
            .add_property("port", &CIMIndicationListener::getPort)
            .add_property("handlers", &CIMIndicationListener::getHandlers)
        );
}

void CIMIndicationListener::start(
    const bp::object &port_,
    const bp::object &cert_file,
    const bp::object &key_file,
    const bp::object &trust_store)
{
    if (m_listener)
        return;

    Pegasus::Uint32 port = lmi::extract_or_throw<Pegasus::Uint32>(port_, "port");
    m_listener.reset(new Pegasus::CIMListener(port));
    if (!m_listener)
        throw_RuntimeError("Can't create CIMListener");

    std::string std_cert_file;
    std::string std_key_file;
    std::string std_trust_store = CIMConstants::DEF_TRUST_STORE;
    if (!isnone(cert_file)) {
        std_cert_file = lmi::extract_or_throw<std::string>(
            cert_file, "cert_file");
    }
    if (!isnone(key_file)) {
        std_key_file = lmi::extract_or_throw<std::string>(
            key_file, "key_file");
    }
    if (!isnone(trust_store)) {
        std_trust_store = lmi::extract_or_throw<std::string>(
            trust_store, "trust_store");
    }

    if (!std_cert_file.empty()) {
        // Pegasus::SSLContext will be freed by Pegasus::CIMListener
        Pegasus::SSLContext *ctx = new Pegasus::SSLContext(
            Pegasus::String(std_trust_store.c_str()),
            Pegasus::String(std_cert_file.c_str()),
            Pegasus::String(std_key_file.c_str()),
            Pegasus::String::EMPTY, // CRL path
            NULL, // verification callback
            Pegasus::String::EMPTY);

        m_listener->setSSLContext(ctx);
    }

    m_listener->addConsumer(&m_consumer);

    try {
        m_listener->start();
    } catch (...) {
        // We couldn't start CIMIndicationListener, free the instance
        // before we process all the exceptions.
        m_listener.reset();
        handle_all_exceptions();
    }
}

void CIMIndicationListener::stop()
{
    if (!m_listener)
        return;

    m_listener->stop();
    m_listener.reset();
}

bool CIMIndicationListener::usesSSL() const
{
    if (!m_listener)
        return false;

    Pegasus::SSLContext *ctx = m_listener->getSSLContext();
    return ctx ? ctx->getCertPath() != Pegasus::String::EMPTY : false;
}

bp::object CIMIndicationListener::addHandler(
    const bp::tuple &args,
    const bp::dict  &kwds)
{
    bp::object name = args[0];
    bp::object callable = args[1];
    std::string std_name = lmi::extract_or_throw<std::string>(name, "name");
    if (!iscallable(callable))
        throw_TypeError("object is not callable");

    m_handlers[std_name] = CallableWithParams(
        callable,
        args.slice(2, bp::len(args)),
        kwds);

    return bp::object();
}

void CIMIndicationListener::removeHandler(const bp::object &name)
{
    std::string std_name = lmi::extract_or_throw<std::string>(name, "name");
    handler_map_t::iterator it = m_handlers.find(std_name);
    if (it == m_handlers.end())
        throw_KeyError("No such handler registered: " + std_name);
    m_handlers.erase(it);
}

bp::object CIMIndicationListener::getHandlers() const
{
    bp::list handlers;
    handler_map_t::const_iterator it;
    for (it = m_handlers.begin(); it != m_handlers.end(); ++it)
        handlers.append(it->first);
    return handlers;
}

void CIMIndicationListener::call(
    const std::string &name,
    const bp::object &indication)
{
    handler_map_t::iterator it = m_handlers.find(name);
    if (it == m_handlers.end())
        return;

    it->second.call(indication);
}
