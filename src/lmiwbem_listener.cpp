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

#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <boost/python/class.hpp>
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

CIMIndicationListener::CIMIndicationListener(Pegasus::Uint32 port)
    : m_port(port)
    , m_listener()
    , m_consumer(this)
    , m_handlers()
{
}

void CIMIndicationListener::init_type()
{
    CIMBase::init_type(
        bp::class_<CIMIndicationListener>("CIMIndicationListener",
            bp::init<Pegasus::Uint32>(
                (bp::arg("port") = CIMIndicationListener::DEF_LISTENER_PORT)))
            .def("start",  &CIMIndicationListener::start)
            .def("stop", &CIMIndicationListener::stop)
            .def("add_handler",  lmi::raw_method<CIMIndicationListener>(&CIMIndicationListener::addHandler, 1))
            .def("remove_handler", &CIMIndicationListener::removeHandler)
            .def("handlers", &CIMIndicationListener::handlers)
            .def("is_alive", &CIMIndicationListener::isAlive)
            .add_property("port", &CIMIndicationListener::getPort)
        );
}

void CIMIndicationListener::start()
{
    if (m_listener)
        return;

    m_listener.reset(new Pegasus::CIMListener(m_port));
    if (!m_listener)
        throw_RuntimeError("Can't create CIMListener");

    m_listener->addConsumer(&m_consumer);

    try {
        m_listener->start();
    } catch (...) {
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

bp::object CIMIndicationListener::handlers() const
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
