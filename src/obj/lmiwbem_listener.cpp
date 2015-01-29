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
#include <sstream>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Listener/CIMListener.h>
#include <boost/python/class.hpp>
#include "lmiwbem_config.h"
#include "lmiwbem_exception.h"
#include "lmiwbem_gil.h"
#include "lmiwbem_make_method.h"
#include "obj/lmiwbem_listener.h"
#include "obj/lmiwbem_listener_pydoc.h"
#include "obj/cim/lmiwbem_instance.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

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

void CallableWithParams::call(const bp::object &indication) const
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
        String(url).substr(1),
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
    , m_trust_store(Config::defaultTrustStore())
    , m_terminating(false)
{
    m_listen_address = StringConv::asString(
        listen_address, "listen_address");
    m_port = Conv::as<Pegasus::Uint32>(port, "port");

    if (!isnone(certfile))
        m_certfile = StringConv::asString(certfile, "certfile");
    if (!isnone(keyfile))
        m_keyfile = StringConv::asString(keyfile, "keyfile");
    if (!isnone(trust_store))
        m_trust_store = StringConv::asString(trust_store, "trust_store");
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
                    bp::arg("certfile") = None,
                    bp::arg("keyfile") = None,
                    bp::arg("trust_store") = None),
                    docstr_CIMIndicationListener_init))
            .def("__repr__", &CIMIndicationListener::repr,
                 docstr_CIMIndicationListener_repr)
            .def("start",  &CIMIndicationListener::start,
                (bp::arg("retries") = 1),
                docstr_CIMIndicationListener_start)
            .def("stop", &CIMIndicationListener::stop,
                docstr_CIMIndicationListener_stop)
            .def("add_handler",
                lmi::raw_method<CIMIndicationListener>(&CIMIndicationListener::addPyHandler, 1),
                docstr_CIMIndicationListener_add_handler)
            .def("remove_handler", &CIMIndicationListener::removePyHandler,
                docstr_CIMIndicationListener_remove_handler)
            .add_property("is_alive", &CIMIndicationListener::getIsAlive,
                docstr_CIMIndicationListener_is_alive)
            .add_property("uses_ssl",
                &CIMIndicationListener::getUsesSSL,
                docstr_CIMIndicationListener_uses_ssl)
            .add_property("listen_address",
                &CIMIndicationListener::getPyListenAddress)
            .add_property("port", &CIMIndicationListener::getPyPort)
            .add_property("handlers", &CIMIndicationListener::getPyHandlers,
                docstr_CIMIndicationListener_handlers));
}

bp::object CIMIndicationListener::repr()
{
    std::stringstream ss;
    ss << "CIMIndicationListener(listen_address=u'" << m_listen_address
       << "', port=" << m_port << ")";
    return StringConv::asPyUnicode(ss.str());
}

void CIMIndicationListener::start(const bp::object &retries)
{
    if (m_listener)
        return;

    m_terminating = false;
    // Get retries count for port binding. By default, we try only once.
    const int c_retries = Conv::as<int>(retries, "retries");
    if (c_retries < 0)
        throw_ValueError("retries must be positive number");

    // Try to create a listener for retries-times.
    for (int i = 0; !m_listener && i < c_retries; ++i) {
        m_listener.reset(new Pegasus::CIMListener(
#ifdef HAVE_PEGASUS_LISTENER_WITH_BIND_ADDRESS
            m_listen_address),
#endif
            m_port + static_cast<Pegasus::Uint32>(i)));

        if (!m_listener)
            throw_RuntimeError("Can't create CIMListener");

        try {
            if (!m_certfile.empty()) {
                // Pegasus::SSLContext will be freed by Pegasus::CIMListener
                Pegasus::SSLContext *peg_ctx = new Pegasus::SSLContext(
                    m_trust_store,
                    m_certfile,
                    m_keyfile,
                    String(), // CRL path
                    NULL, // verification callback
                    String());

                m_listener->setSSLContext(peg_ctx);
            }

            m_listener->addConsumer(&m_consumer);

            m_listener->start();

            // Update the actual port we managed to bind to.
            m_port += i;
        } catch (...) {
            // We couldn't start CIMIndicationListener, free the instance
            // before we process all the exceptions.
            m_listener.reset();

            // We raise the exception, when we run out of retries or TCP port
            // is out of range.
            if (i == c_retries - 1 || m_port + i >= 65536) {
                std::stringstream ss;
                if (Config::isVerbose()) {
                    ss << "CIMIndicationListener.start(";
                    if (Config::isVerboseMore())
                        ss << "port=" << m_port << ", port_retries=+" << c_retries;
                    ss << ')';
                }
                handle_all_exceptions(ss);
            }
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

bool CIMIndicationListener::getIsAlive() const
{
    return m_listener && m_listener->isAlive();
}

bool CIMIndicationListener::getUsesSSL() const
{
    if (!m_listener)
        return false;

    Pegasus::SSLContext *peg_ctx = m_listener->getSSLContext();
    return peg_ctx ? peg_ctx->getCertPath() != Pegasus::String::EMPTY : false;
}

String CIMIndicationListener::getListenAddress() const
{
    return m_listen_address;
}

int CIMIndicationListener::getPort() const
{
    return m_listener ? m_listener->getPortNumber() : -1;
}

bp::object CIMIndicationListener::getPyListenAddress() const
{
    return StringConv::asPyUnicode(m_listen_address);
}

bp::object CIMIndicationListener::getPyPort() const
{
    return bp::object(m_port);
}

bp::object CIMIndicationListener::addPyHandler(
    const bp::tuple &args,
    const bp::dict  &kwds)
{
    bp::object py_name = args[0];
    bp::object py_callable = args[1];
    String c_name = StringConv::asString(py_name, "name");

    if (iscallable(py_callable)) {
        m_handlers[c_name].push_back(
            CallableWithParams(
                py_callable,
                args.slice(2, bp::len(args)),
                kwds));
    } else if (islist(py_callable) || istuple(py_callable)) {
        const int cnt = bp::len(py_callable);
        for (int i = 0; i < cnt; ++i) {
            bp::object py_handler(py_callable[i]);

            if (!iscallable(py_handler))
                throw_TypeError("list/tuple with handlers contains non-callable");

            m_handlers[c_name].push_back(
                CallableWithParams(
                    py_handler,
                    args.slice(2, bp::len(args)),
                    kwds));
        }
    } else {
        throw_TypeError("object is not callable or list/tuple with callables");
    }

    return None;
}

void CIMIndicationListener::removePyHandler(const bp::object &name)
{
    String c_name = StringConv::asString(name, "name");
    handler_map_t::iterator it = m_handlers.find(c_name);
    if (it == m_handlers.end())
        throw_KeyError("No such handler registered: " + c_name);
    m_handlers.erase(it);
}

bp::object CIMIndicationListener::getPyHandlers() const
{
    bp::list py_handlers;
    handler_map_t::const_iterator it;
    for (it = m_handlers.begin(); it != m_handlers.end(); ++it)
        py_handlers.append(it->first);
    return py_handlers;
}

void CIMIndicationListener::call(
    const String &name,
    const bp::object &indication) const
{

    const handler_map_t::const_iterator it = m_handlers.find(name);
    if (it == m_handlers.end())
        return;

    std::list<CallableWithParams>::const_iterator it_cb;
    for (it_cb = it->second.begin(); it_cb != it->second.end(); ++it_cb)
        it_cb->call(indication);
}
