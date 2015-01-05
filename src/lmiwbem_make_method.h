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

#ifndef   LMIWBEM_MAKE_METHOD
#  define LMIWBEM_MAKE_METHOD

#  include <boost/python/tuple.hpp>
#  include <boost/python/dict.hpp>
#  include <boost/python/object/py_function.hpp>
#  include <boost/python/raw_function.hpp>
#  include <boost/mpl/vector/vector10.hpp>
#  include <boost/limits.hpp>
#  include "util/lmiwbem_convert.h"

namespace bp = boost::python;

namespace lmi {

namespace detail {

template <typename C, typename M>
class raw_method_dispatcher
{
public:
    raw_method_dispatcher(M method): m_method(method) {}

    PyObject* operator()(PyObject* args, PyObject* kwds)
    {
        bp::detail::borrowed_reference_t *rargs = bp::detail::borrowed_reference(args);
        bp::object args_obj(rargs);

        // Extract a C++ reference to object, which is about to call raw method
        C &fake_this = Conv::as<C&>(bp::object(args_obj[0]));

        // Return boost::python object containing the raw method call
        return bp::incref(
            bp::object(
                (fake_this.*m_method)(
                    bp::tuple(args_obj.slice(1, bp::len(args_obj))),
                    kwds ? bp::dict(bp::detail::borrowed_reference(kwds)) : bp::dict())
                ).ptr()
            );
    }

private:
    M m_method;
};

} // namespace detail

template <typename C, typename M>
bp::object raw_method(M method, std::size_t min_args = 0)
{
    return bp::detail::make_raw_function(
        bp::objects::py_function(
            detail::raw_method_dispatcher<C, M>(method),
            boost::mpl::vector1<PyObject*>(),
            min_args,
            (std::numeric_limits<unsigned>::max)())
    );
}

} // namespace lmi

#endif // LMIWBEM_MAKE_METHOD
