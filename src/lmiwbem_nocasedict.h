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

#ifndef LMIWBEM_NOCASEDICT_H
#define LMIWBEM_NOCASEDICT_H

#include <map>
#include <boost/python/class.hpp>
#include <boost/python/object.hpp>
#include "lmiwbem.h"
#include "lmiwbem_cimbase.h"
#include "lmiwbem_extract.h"

namespace bp = boost::python;

BOOST_PYTHON_BEGIN
class list;
class dict;
BOOST_PYTHON_END

class NocaseDictComparator;

typedef std::map <std::string, bp::object, NocaseDictComparator> nocase_map_t;

class NocaseDictComparator
{
public:
    bool operator ()(const std::string &a, const std::string &b) const;
};

class NocaseDict: public CIMBase<NocaseDict>
{
public:
    NocaseDict();
    NocaseDict(const bp::object &d);

    static void init_type();
    static bp::object create() { return CIMBase::create(); }
    static bp::object create(const bp::object &d);

    nocase_map_t::iterator begin() { return m_dict.begin(); }
    nocase_map_t::iterator end() { return m_dict.end(); }
    nocase_map_t::const_iterator begin() const { return m_dict.begin(); }
    nocase_map_t::const_iterator end() const { return m_dict.end(); }

    bool empty() { return m_dict.empty(); }

    void delitem(const bp::object &key);
    void setitem(const bp::object &key, const bp::object &value);
    bp::object getitem(const bp::object &key);

    std::string repr();

    bp::list keys();
    bp::list values();
    bp::list items();
    bp::object iterkeys();
    bp::object itervalues();
    bp::object iteritems();

    bool haskey(const bp::object &key);
    std::size_t len() { return m_dict.size(); }

    void update(const bp::object &d);
    void clear() { m_dict.clear(); }

    bp::object get(
        const bp::object &key,
        const bp::object &def = bp::object());
    bp::object pop(
        const bp::object &key,
        const bp::object &def = bp::object());

    bp::object copy();
    bool equals(const bp::object &other);
    int compare(const bp::object &other);

private:
    nocase_map_t m_dict;
};

class NocaseDictIterator
{
public:
    virtual bp::object iter() = 0;
    virtual bp::object next() = 0;

protected:
    template <typename T>
    static void init_type(const char *classname)
    {
        T::CIMBase::init_type(bp::class_<T>(
            classname, bp::init<>())
            .def("__iter__", &T::iter)
            .def("next", &T::next));
    }

    template <typename T>
    static bp::object create(const nocase_map_t &dict)
    {
        bp::object inst = T::CIMBase::create();
        T &fake_this = lmi::extract<T&>(inst);
        fake_this.m_dict = dict;
        fake_this.m_iter = fake_this.m_dict.begin();
        return inst;
    }

    nocase_map_t m_dict;
    nocase_map_t::const_iterator m_iter;
};

class NocaseDictKeyIterator:
    public CIMBase<NocaseDictKeyIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

class NocaseDictValueIterator:
    public CIMBase<NocaseDictValueIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

class NocaseDictItemIterator:
    public CIMBase<NocaseDictItemIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

#endif // LMIWBEM_NOCASEDICT_H
