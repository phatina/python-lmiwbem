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
#include "lmiwbem_gil.h"
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
    ScopedMutex sm(m_listener->m_mutex);
    if (m_listener->m_terminating) {
        return;
    }

    /* We call python from inside a thread created by CIMOM. We need to acquire
     * the GIL. */
    ScopedGILAcquire sg;
    bp::object inst = CIMInstance::create(indication);
    m_listener->call(
        std::string(url.subString(1).getCString()),
        inst);
}

// ----------------------------------------------------------------------------

CIMIndicationListener::CIMIndicationListener(
    const bp::object &listen_address,
    const bp::object &port,
    const bp::object &certfile,
    const bp::object &keyfile,
    const bp::object &trust_store)
    : m_listener()
    , m_consumer(this)
    , m_handlers()
    , m_port(0)
    , m_listen_address()
    , m_certfile()
    , m_keyfile()
    , m_trust_store(CIMConstants::defaultTrustStore())
    , m_terminating(false)
{
    m_listen_address = lmi::extract_or_throw<std::string>(
        listen_address, "listen_address");
    m_port = lmi::extract_or_throw<Pegasus::Uint32>(port, "port");

    if (!isnone(certfile))
        m_certfile = lmi::extract_or_throw<std::string>(certfile, "certfile");
    if (!isnone(keyfile))
        m_keyfile = lmi::extract_or_throw<std::string>(keyfile, "keyfile");
    if (!isnone(trust_store))
        m_trust_store = lmi::extract_or_throw<std::string>(trust_store, "trust_store");
}

void CIMIndicationListener::init_type()
{
    CIMBase<CIMIndicationListener>::init_type(
        bp::class_<CIMIndicationListener>("CIMIndicationListener", bp::no_init)
            .def(bp::init<
                const bp::object &,
                const bp::object &,
                const bp::object &,
                const bp::object &,
                const bp::object &>((
                    bp::arg("listen_address"),
                    bp::arg("port"),
                    bp::arg("certfile") = bp::object(),
                    bp::arg("keyfile") = bp::object(),
                    bp::arg("trust_store") = bp::object()),
                    "Constructs a :py:class:`.CIMIndicationListener` object.\n\n"
                    ":param str listen_address: bind address\n"
                    ":param int port: listening port\n"
                    ":param str certfile: path to X509 certificate\n"
                    ":param str keyfile: path to X509 private key; may be None,\n"
                    "\tif cert_file also contains private key\n"
                    ":param str trust_store: path to trust store"))
            .def("start",  &CIMIndicationListener::start,
                (bp::arg("retries") = 1),
                 "start(retries=1)\n\n"
                 "Starts indication listener.\n\n"
                 ":param int retries: number of bind retries.\n")
            .def("stop", &CIMIndicationListener::stop,
                "stop()\n\n"
                "Stops indication listener.")
            .def("add_handler",
                lmi::raw_method<CIMIndicationListener>(&CIMIndicationListener::addHandler, 1),
                "add_handler(name, handler, *args, **kwargs)\n\n"
                "Adds callback for specific indication.\n\n"
                ":param str name: indication name\n"
                ":param handler: callable for indication\n"
                ":param args: positional arguments passed to handler\n"
                ":param kwargs: keyword arguments passed to handler")
            .def("remove_handler", &CIMIndicationListener::removeHandler,
                "remove_handler(name)\n\n"
                "Removes a specified handler from indication listener.\n\n"
                ":param str name: indication name\n"
                ":raises: :py:exc:`KeyError`")
            .add_property("is_alive", &CIMIndicationListener::isAlive,
                "Property storing flag, which indicates, if the indication\n"
                "listener is running.\n\n"
                ":rtype: bool")
            .add_property("uses_ssl", &CIMIndicationListener::usesSSL,
                "Property storing flag, which indicates, if the indication\n"
                "listener uses secure connection.\n\n"
                ":rtype: bool")
            .add_property("listen_address", &CIMIndicationListener::getListenAddress,
                "Property storing bind address.\n\n"
                ":rtype: str")
            .add_property("port", &CIMIndicationListener::getPort,
                "Property storing listening port.\n\n"
                ":rtype: int")
            .add_property("handlers", &CIMIndicationListener::getHandlers,
                "Property storing list of strings of handlers.\n\n"
                ":rtype: list"));
}

void CIMIndicationListener::start(const bp::object &retries)
{
    if (m_listener)
        return;

    m_terminating = false;
    // Extract retries count for port binding. By default, we try only once.
    const int std_retries = lmi::extract_or_throw<int>(retries, "retries");
    if (std_retries < 0)
        throw_ValueError("retries must be positive number");

    // Try to create a listener for retries-times.
    for (int i = 0; !m_listener && i < std_retries; ++i) {
        m_listener.reset(new Pegasus::CIMListener(
#ifdef HAVE_PEGASUS_LISTENER_WITH_BIND_ADDRESS
            Pegasus::String(m_listen_address.c_str()),
#endif
            m_port));

        if (!m_listener)
            throw_RuntimeError("Can't create CIMListener");

        try {
            if (!m_certfile.empty()) {
                // Pegasus::SSLContext will be freed by Pegasus::CIMListener
                Pegasus::SSLContext *ctx = new Pegasus::SSLContext(
                    Pegasus::String(m_trust_store.c_str()),
                    Pegasus::String(m_certfile.c_str()),
                    Pegasus::String(m_keyfile.c_str()),
                    Pegasus::String::EMPTY, // CRL path
                    NULL, // verification callback
                    Pegasus::String::EMPTY);

                m_listener->setSSLContext(ctx);
            }

            m_listener->addConsumer(&m_consumer);

            m_listener->start();
        } catch (...) {
            // We couldn't start CIMIndicationListener, free the instance
            // before we process all the exceptions.
            m_listener.reset();

            // We raise the exception, when we run out of retries or TCP port
            // is out of range.
            if (i == std_retries - 1 || ++m_port >= 65536)
                handle_all_exceptions();
        }
    }
}

void CIMIndicationListener::stop()
{
    if (!m_listener)
        return;

    /* While terminating, new callbacks can still be invoked by indication
     * dispatcher. We need to wait for them to finish (m_listener->stop() does
     * that). Thus we need to release GIL to let these callbacks acquire it. */
    ScopedGILRelease sr;
    {   /* Indicate to newly spawned listener callbacks that we are
         * terminating. This will prevent them from acquiring GIL and calling
         * indication handlers. */
        ScopedMutex sm(m_mutex);
        m_terminating = true;
    }

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
