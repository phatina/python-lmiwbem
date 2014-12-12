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
#include <algorithm>
#include <string>
#include <utility>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include <boost/python/str.hpp>
#include <boost/python/tuple.hpp>
#include "obj/lmiwbem_nocasedict.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

bool NocaseDictComparator::operator ()(const String &a, const String &b) const
{
    String low_a(a);
    String low_b(b);
    std::transform(low_a.begin(), low_a.end(), low_a.begin(), ::tolower);
    std::transform(low_b.begin(), low_b.end(), low_b.begin(), ::tolower);
    return low_a < low_b;
}

NocaseDict::NocaseDict()
    : m_dict()
{
}

NocaseDict::NocaseDict(const bp::object &d)
    : m_dict()
{
    update(d);
}

void NocaseDict::init_type()
{
    CIMBase<NocaseDict>::init_type(bp::class_<NocaseDict>("NocaseDict", bp::init<>())
        .def(bp::init<const bp::object&>())
        .def("__getitem__", &NocaseDict::getitem)
        .def("__setitem__", &NocaseDict::setitem)
        .def("__delitem__", &NocaseDict::delitem)
        .def("__contains__", &NocaseDict::haskey)
#  if PY_MAJOR_VERSION < 3
        .def("__cmp__", &NocaseDict::cmp)
#  else
        .def("__eq__", &NocaseDict::eq)
        .def("__gt__", &NocaseDict::gt)
        .def("__lt__", &NocaseDict::lt)
        .def("__ge__", &NocaseDict::ge)
        .def("__le__", &NocaseDict::le)
#  endif // PY_MAJOR_VERSION
        .def("__len__", &NocaseDict::len)
        .def("__repr__", &NocaseDict::repr)
        .def("keys", &NocaseDict::keys, "keys()")
        .def("values", &NocaseDict::values, "values()")
        .def("items", &NocaseDict::items, "items()")
        .def("iterkeys", &NocaseDict::iterkeys, "iterkeys()")
        .def("itervalues", &NocaseDict::itervalues, "itervalues()")
        .def("iteritems", &NocaseDict::iteritems, "iteritems()")
        .def("has_key", &NocaseDict::haskey, "has_key()")
        .def("update", &NocaseDict::update, "update()")
        .def("clear", &NocaseDict::clear, "clear()")
        .def("get", &NocaseDict::get,
            (bp::arg("key"),
             bp::arg("def") = None),
            "get(key, default_value=None)")
        .def("pop", &NocaseDict::pop,
            (bp::arg("key"),
             bp::arg("def") = None),
            "pop(key, default_value)")
        .def("copy", &NocaseDict::copy, "copy()"));
}

bp::object NocaseDict::create(const bp::object &d)
{
    bp::object inst = create();
    NocaseDict& nocase_dict = NocaseDict::asNative(inst);
    nocase_dict.update(d);
    return inst;
}

void NocaseDict::delitem(const bp::object &key)
{
    String c_key = StringConv::asString(key, "key");

    nocase_map_t::iterator found = m_dict.find(c_key);
    if (found == m_dict.end())
        throw_KeyError("Key not found");

    m_dict.erase(found);
}

void NocaseDict::setitem(const bp::object &key, const bp::object &value)
{
    String c_key = StringConv::asString(key, "key");
    m_dict[c_key] = value;
}

bp::object NocaseDict::getitem(const bp::object &key)
{
    String c_key = StringConv::asString(key, "key");

    nocase_map_t::const_iterator found = m_dict.find(c_key);
    if (found == m_dict.end())
        throw_KeyError("Key not found");

    return found->second;
}

String NocaseDict::repr()
{
    std::stringstream ss;
    ss << "NocaseDict({";
    nocase_map_t::const_iterator it;
    for (it = m_dict.begin(); it != m_dict.end(); ++it) {
        String c_value = ObjectConv::asString(it->second);
        ss << "u'" << it->first << "': ";
        if (isunicode(it->second))
            ss << 'u';
        ss << '\'' << c_value << '\'';
        if (it != --m_dict.end())
            ss << ", ";
    }
    ss << "})";

    return ss.str();
}

bp::list NocaseDict::keys()
{
    bp::list py_keys;
    nocase_map_t::const_iterator it;
    for (it = m_dict.begin(); it != m_dict.end(); ++it)
        py_keys.append(StringConv::asPyUnicode(it->first));
    return py_keys;
}

bp::list NocaseDict::values()
{
    bp::list py_values;
    nocase_map_t::const_iterator it;
    for (it = m_dict.begin(); it != m_dict.end(); ++it)
        py_values.append(it->second);
    return py_values;
}

bp::list NocaseDict::items()
{
    bp::list py_items;
    nocase_map_t::const_iterator it;
    for (it = m_dict.begin(); it != m_dict.end(); ++it) {
        py_items.append(
            bp::make_tuple(
                StringConv::asPyUnicode(it->first),
                it->second));
    }

    return py_items;
}

bp::object NocaseDict::iterkeys()
{
    return NocaseDictKeyIterator::create(m_dict);
}

bp::object NocaseDict::itervalues()
{
    return NocaseDictValueIterator::create(m_dict);
}

bp::object NocaseDict::iteritems()
{
    return NocaseDictItemIterator::create(m_dict);
}

bp::object NocaseDict::haskey(const bp::object &key) const
{
    String c_key = StringConv::asString(key, "key");
    return bp::object(m_dict.find(c_key) != m_dict.end());
}

bp::object NocaseDict::len() const
{
    return bp::object(m_dict.size());
}

void NocaseDict::update(const bp::object &d)
{
    if (isinstance(d, type())) {
        NocaseDict &cim_nocasedict = NocaseDict::asNative(d);
        // Update from NocaseDict
        nocase_map_t::iterator it;
        for (it = cim_nocasedict.m_dict.begin();
            it != cim_nocasedict.m_dict.end(); ++it)
        {
            std::pair<nocase_map_t::iterator, bool> ret;
            ret = m_dict.insert(std::make_pair(it->first, it->second));
            if (!ret.second) {
                // Update existing key
                ret.first->second = it->second;
            }
        }
    } else if (isdict(d)) {
        // Update from boost::python::dict
        bp::dict py_dict(Conv::as<bp::dict>(d));
        const bp::list &py_keys = py_dict.keys();
        const ssize_t len = bp::len(py_keys);
        for (int i = 0; i < len; ++i) {
            bp::object py_key(py_keys[i]);
            String c_key = StringConv::asString(py_key, "key");
            m_dict[c_key] = py_dict[py_key];
        }
    } else {
        throw_TypeError("NocaseDict can be updated from NocaseDict or dict");
    }
}

bp::object NocaseDict::get(const bp::object &key, const bp::object &def)
{
    String c_key = StringConv::asString(key, "key");

    nocase_map_t::const_iterator found = m_dict.find(c_key);
    if (found == m_dict.end())
        return def;

    return found->second;
}

bp::object NocaseDict::pop(const bp::object &key, const bp::object &def)
{
    String c_key = StringConv::asString(key, "key");

    nocase_map_t::iterator found = m_dict.find(c_key);
    if (found == m_dict.end())
            return def;

    bp::object py_rval = found->second;
    m_dict.erase(found);

    return py_rval;
}

bp::object NocaseDict::copy()
{
    bp::object py_inst = CIMBase<NocaseDict>::create();
    NocaseDict &fake_this = NocaseDict::asNative(py_inst);
    fake_this.m_dict = nocase_map_t(m_dict);
    return py_inst;
}

#  if PY_MAJOR_VERSION < 3
int NocaseDict::cmp(const bp::object &other)
{
    if (!isinstance(other, type()))
        return -1;

    const nocase_map_t &c_other_dict = NocaseDict::asNative(other).m_dict;
    nocase_map_t::const_iterator it;
    for (it = m_dict.begin(); it != m_dict.end(); ++it) {
        const nocase_map_t::const_iterator found = c_other_dict.find(it->first);
        if (found == c_other_dict.end())
            return -1;

        if (it->second < found->second)
            return -1;
        else if (it->second > found->second)
            return 1;
    }

    return m_dict.size() - c_other_dict.size();
}
#  else
bool NocaseDict::eq(const bp::object &other)
{
    if (!isinstance(other, type()))
        return false;

    const nocase_map_t &c_other_dict = NocaseDict::asNative(other).m_dict;
    return m_dict.size() == c_other_dict.size() &&
        std::equal(m_dict.begin(), m_dict.end(), c_other_dict.begin());
}

bool NocaseDict::gt(const bp::object &other)
{
    if (!isinstance(other, type()))
        return false;

    const nocase_map_t &c_other_dict = NocaseDict::asNative(other).m_dict;
    return m_dict > c_other_dict;
}

bool NocaseDict::lt(const bp::object &other)
{
    if (!isinstance(other, type()))
        return false;

    const nocase_map_t &c_other_dict = NocaseDict::asNative(other).m_dict;
    return m_dict < c_other_dict;
}

bool NocaseDict::ge(const bp::object &other)
{
    return gt(other) || eq(other);
}

bool NocaseDict::le(const bp::object &other)
{
    return lt(other) || eq(other);
}
#  endif // PY_MAJOR_VERSION

// ----------------------------------------------------------------------------

void NocaseDictKeyIterator::init_type()
{
    NocaseDictIterator::init_type<NocaseDictKeyIterator>("NocaseDictKeyIterator");
}

bp::object NocaseDictKeyIterator::create(const nocase_map_t &dict)
{
    return NocaseDictIterator::create<NocaseDictKeyIterator>(dict);
}

bp::object NocaseDictKeyIterator::iter()
{
    return create(m_dict);
}

bp::object NocaseDictKeyIterator::next()
{
    if (m_iter == m_dict.end())
        throw_StopIteration("Stop iteration");

    bp::object py_key(StringConv::asPyUnicode(m_iter->first));

    ++m_iter;

    return py_key;
}

// ----------------------------------------------------------------------------

void NocaseDictValueIterator::init_type()
{
    NocaseDictIterator::init_type<NocaseDictValueIterator>("NocaseDictValueIterator");
}

bp::object NocaseDictValueIterator::create(const nocase_map_t &dict)
{
    return NocaseDictIterator::create<NocaseDictValueIterator>(dict);
}

bp::object NocaseDictValueIterator::iter()
{
    return create(m_dict);
}

bp::object NocaseDictValueIterator::next()
{
    if (m_iter == m_dict.end())
        throw_StopIteration("Stop iteration");

    bp::object py_value(m_iter->second);

    ++m_iter;

    return py_value;
}

// ----------------------------------------------------------------------------

void NocaseDictItemIterator::init_type()
{
    NocaseDictIterator::init_type<NocaseDictItemIterator>("NocaseDictItemIterator");
}

bp::object NocaseDictItemIterator::create(const nocase_map_t &dict)
{
    return NocaseDictIterator::create<NocaseDictItemIterator>(dict);
}

bp::object NocaseDictItemIterator::iter()
{
    return create(m_dict);
}

bp::object NocaseDictItemIterator::next()
{
    if (m_iter == m_dict.end())
        throw_StopIteration("Stop iteration");

    bp::object py_pair = bp::make_tuple(
        bp::str(m_iter->first),
        m_iter->second);

    ++m_iter;

    return py_pair;
}
