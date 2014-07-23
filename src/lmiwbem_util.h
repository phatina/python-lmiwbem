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

#ifndef   LMIWBEM_UTIL_H
#  define LMIWBEM_UTIL_H

#  include <map>
#  include <string>
#  include <boost/shared_ptr.hpp>
#  include <Pegasus/Common/Char16.h>
#  include <Pegasus/Common/CIMDateTime.h>
#  include <Pegasus/Common/CIMName.h>
#  include <Pegasus/Common/CIMObjectPath.h>
#  include <Pegasus/Common/CIMPropertyList.h>
#  include <Pegasus/Common/CIMValue.h>
#  include <Pegasus/Common/String.h>
#  include "lmiwbem.h"

BOOST_PYTHON_BEGIN
class object;
BOOST_PYTHON_END

namespace bp = boost::python;

#  include <boost/python/to_python_converter.hpp>

#  define DECL_TO_CONVERTER(name, type) \
       struct name \
       { \
           static PyObject *convert(const type &val); \
           static void register_converter(); \
       }

#  define DEFINE_TO_CONVERTER(name, type) \
       void name::register_converter() \
       { \
           boost::python::to_python_converter<type, name>(); \
       } \
       PyObject *name::convert(const type &value)

DECL_TO_CONVERTER(PegasusStringToPythonString, Pegasus::String);
DECL_TO_CONVERTER(PegasusCIMNameToPythonString, Pegasus::CIMName);
DECL_TO_CONVERTER(PegasusCIMDateteTimeToPythonDateTime, Pegasus::CIMDateTime);
DECL_TO_CONVERTER(PegasusChar16ToPythonUint16, Pegasus::Char16);
DECL_TO_CONVERTER(PegasusCIMObjectPathToPythonCIMInstanceName, Pegasus::CIMObjectPath);
DECL_TO_CONVERTER(PegasusCIMObjectToPythonCIMObject, Pegasus::CIMObject);

class CIMTypeConv
{
public:
    static std::string asStdString(Pegasus::CIMType type)
    {
        return CIMTypeConv::CIMTypeHolder::instance()->get(type);
    }

    static Pegasus::CIMType asCIMType(const std::string &type)
    {
        return CIMTypeConv::CIMTypeHolder::instance()->get(type);
    }

private:
    CIMTypeConv();

    class CIMTypeHolder
    {
    public:
        static CIMTypeHolder *instance();

        std::string get(Pegasus::CIMType type);
        Pegasus::CIMType get(const std::string &type);

    private:
        CIMTypeHolder();
        CIMTypeHolder(const CIMTypeHolder &copy) { }

        static boost::shared_ptr<CIMTypeHolder> s_instance;

        std::map<Pegasus::CIMType, std::string> m_type_string;
        std::map<std::string, Pegasus::CIMType> m_string_type;
    };

    static CIMTypeHolder s_type_holder;
};

class ListConv
{
public:
    static Pegasus::CIMPropertyList asPegasusPropertyList(
        const bp::object &property_list,
        const std::string &message);

private:
    ListConv();
};

std::string object_as_std_string(const bp::object &obj);

std::string pystring_as_std_string(const bp::object &obj);
std::string pystring_as_std_string(const bp::object &obj, bool &good);
bp::object  std_string_as_pyunicode(const std::string &str);
bp::object  std_string_as_pybool(const std::string &str);
#  if PY_MAJOR_VERSION < 3
bp::object  std_string_as_pyint(const std::string &str);
#  endif // PY_MAJOR_VERSION
bp::object  std_string_as_pyfloat(const std::string &str);
bp::object  std_string_as_pylong(const std::string &str);

bp::object this_module();
bp::object incref(const bp::object &obj);

bool isnone(const bp::object &obj);
bool isinstance(const bp::object &inst, const bp::object &cls);
bool isunicode(const bp::object &obj);
bool isbasestring(const bp::object &obj);
bool isbool(const bp::object &obj);
#  if PY_MAJOR_VERSION < 3
bool isstring(const bp::object &obj);
bool isint(const bp::object &obj);
#  endif // PY_MAJOR_VERSION
bool islong(const bp::object &obj);
bool isfloat(const bp::object &obj);
bool isdict(const bp::object &obj);
bool iscallable(const bp::object &obj);

bool cim_issubclass(
    const bp::object &ch,
    const bp::object &ns,
    const bp::object &superclass,
    const bp::object &subclass);
bool is_error(const bp::object &value);

#  if PY_MAJOR_VERSION < 3
int compare(const bp::object &o1, const bp::object &o2);
#  else
bool compare(const bp::object &o1, const bp::object &o2, int cmp_type);
#  endif // PY_MAJOR_VERSION

#endif // LMIWBEM_UTIL_H
