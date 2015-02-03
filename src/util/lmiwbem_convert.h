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

#ifndef   LMIWBEM_CONVERT_H
#  define LMIWBEM_CONVERT_H

#  include <map>
#  include <boost/python/extract.hpp>
#  include <boost/python/list.hpp>
#  include <boost/python/to_python_converter.hpp>
#  include <Pegasus/Common/Array.h>
#  include <Pegasus/Common/CIMInstance.h>
#  include <Pegasus/Common/CIMObject.h>
#  include <Pegasus/Common/CIMObjectPath.h>
#  include <Pegasus/Common/CIMType.h>
#  include "lmiwbem.h"
#  include "lmiwbem_exception.h"
#  include "obj/cim/lmiwbem_constants.h"
#  include "util/lmiwbem_string.h"

BOOST_PYTHON_BEGIN
class object;
BOOST_PYTHON_END

PEGASUS_BEGIN
class Char16;
class CIMDateTime;
class CIMName;
class CIMPropertyList;
class CIMValue;
class String;
PEGASUS_END

namespace bp = boost::python;

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

DECL_TO_CONVERTER(StringToPythonString, String);
DECL_TO_CONVERTER(PegasusStringToPythonString, Pegasus::String);
DECL_TO_CONVERTER(PegasusCIMNameToPythonString, Pegasus::CIMName);
DECL_TO_CONVERTER(PegasusCIMDateteTimeToPythonDateTime, Pegasus::CIMDateTime);
DECL_TO_CONVERTER(PegasusChar16ToPythonUint16, Pegasus::Char16);
DECL_TO_CONVERTER(PegasusCIMObjectPathToPythonCIMInstanceName, Pegasus::CIMObjectPath);
DECL_TO_CONVERTER(PegasusCIMObjectToPythonCIMObject, Pegasus::CIMObject);
DECL_TO_CONVERTER(CIMConstantsCIMErrorToPythonInt, CIMConstants::CIMError);

namespace Conv {

namespace detail {

template <typename T>
class extract: public bp::extract<T>
{
public:
    extract(PyObject *pyobj): bp::extract<T>(pyobj) { }
    extract(const bp::object &obj): bp::extract<T>(obj) { }
};

template <>
class extract<String>
{
public:
    extract(const bp::object &obj);
    bool check() const { return m_good; }
    operator String() { return m_str; }
    String operator ()() { return m_str; }

private:
    bool m_good;
    String m_str;
};

template <typename T>
class get
{
public:
    get(const bp::object &obj): m_ext(obj) { }
    bool check() const { return m_ext.check(); }
    operator T() { return T(m_ext); }
private:
    extract<T> m_ext;
};

template <typename T>
void throw_if_fail(
    const extract<T> &ext_obj,
    const String &obj_name)
{
    if (ext_obj.check())
        return;
    throw_TypeError_member<T>(obj_name);
}

} // namespace detail

template<typename T>
T as(
    const bp::object &obj,
    const String &obj_name = String("variable"))
{
    detail::extract<T> ext_obj(obj);
    //if (!ext_obj.check())
    //    throw_TypeError_member<T>(obj_name);
    detail::throw_if_fail<T>(ext_obj, obj_name);
    return ext_obj();
}

template <typename T>
bp::object get(
    const bp::object &obj,
    const String &obj_name = String("variable"))
{
    detail::get<T> get_obj(obj);
    if (!get_obj.check())
        throw_TypeError_member<T>(obj_name);
    return obj;
}

template <typename T, typename U>
bp::object get(
    const bp::object &obj,
    const String &obj_name = String("variable"))
{
    detail::get<T> get_objT(obj);
    detail::get<U> get_objU(obj);
    if (!get_objT.check() && !get_objU.check())
        throw_TypeError_member<T>(obj_name);
    return obj;
}

} // namespace Conv

// -----------------------------------------------------------------------------
// Converters
// -----------------------------------------------------------------------------

class CIMTypeConv
{
public:
    static String asString(const bp::object &obj);
    static String asString(Pegasus::CIMType type);
    static Pegasus::CIMType asCIMType(const String &type);

private:
    CIMTypeConv();

    class CIMTypeHolder
    {
    public:
        static CIMTypeHolder *instance();

        String get(Pegasus::CIMType type);
        Pegasus::CIMType get(const String &type);

    private:
        CIMTypeHolder();
        CIMTypeHolder(const CIMTypeHolder &copy) { }

        static boost::shared_ptr<CIMTypeHolder> s_instance;

        std::map<Pegasus::CIMType, String> m_type_string;
        std::map<String, Pegasus::CIMType> m_string_type;
    };

    static CIMTypeHolder s_type_holder;
};

class ListConv
{
private:
    ListConv();

    // ------------------------------------------------------------------------
    // To Python Functor classes
    // ------------------------------------------------------------------------
    class PyFunctor
    {
    public:
        PyFunctor();
        PyFunctor(
            const String &ns,
            const String &hostname);

    protected:
        String m_ns;
        String m_hostname;
    };

    class PyFunctorCIMInstance: public PyFunctor
    {
    public:
        PyFunctorCIMInstance(
            const String &ns,
            const String &hostname);

        bp::object operator()(Pegasus::CIMInstance instance) const;
        bp::object operator()(const Pegasus::CIMObject object) const;
    };

    class PyFunctorCIMInstanceName: public PyFunctor
    {
    public:
        PyFunctorCIMInstanceName(
            const String &ns,
            const String &hostname);

        bp::object operator()(
            const Pegasus::CIMObjectPath &instance_name) const;
    };

    class PyFunctorCIMClass: public PyFunctor
    {
    public:
        bp::object operator()(const Pegasus::CIMClass &class_) const;
    };

    template <typename T, typename Functor>
    static bp::object asPyListCore(
        const Pegasus::Array<T> &arr,
        const Functor &f)
    {
        bp::list py_list;
        const Pegasus::Uint32 cnt = arr.size();
        for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
            py_list.append(f(arr[i]));
        }
        return py_list;
    }

public:
    static Pegasus::CIMPropertyList asPegasusPropertyList(
        const bp::object &property_list,
        const String &message);

    static bp::object asPyCIMInstanceList(
        const Pegasus::Array<Pegasus::CIMInstance> &arr,
        const String &ns = String(),
        const String &hostname = String());

    static bp::object asPyCIMInstanceList(
        const Pegasus::Array<Pegasus::CIMObject> &arr,
        const String &ns = String(),
        const String &hostname = String());

    static bp::object asPyCIMInstanceNameList(
        const Pegasus::Array<Pegasus::CIMObjectPath> &arr,
        const String &ns = String(),
        const String &hostname = String());

    static bp::object asPyCIMClassList(
        const Pegasus::Array<Pegasus::CIMClass> &arr);
};

class ObjectConv
{
public:
    static String asString(const bp::object &obj);
    static bp::object asPyUnicode(const bp::object &obj);

private:
    ObjectConv();
};

class StringConv
{
public:
    static String asString(const bp::object &obj);
    static String asString(const bp::object &obj, const String &member);
    static Pegasus::String asPegasusString(const bp::object &obj);
    static Pegasus::String asPegasusString(const bp::object &obj, const String &member);
    static bp::object  asPyUnicode(const char *str);
    static bp::object  asPyUnicode(const String &str);
    static bp::object  asPyUnicode(const Pegasus::String &str);
    static bp::object  asPyBool(const char *str);
    static bp::object  asPyBool(const String &str);
    static bp::object  asPyBool(const Pegasus::String &str);
#  if PY_MAJOR_VERSION < 3
    static bp::object  asPyInt(const char *str);
    static bp::object  asPyInt(const String &str);
    static bp::object  asPyInt(const Pegasus::String &str);
#  endif // PY_MAJOR_VERSION
    static bp::object  asPyFloat(const char *str);
    static bp::object  asPyFloat(const String &str);
    static bp::object  asPyFloat(const Pegasus::String &str);
    static bp::object  asPyLong(const char *str);
    static bp::object  asPyLong(const String &str);
    static bp::object  asPyLong(const Pegasus::String &str);

private:
    StringConv();
};

#endif // LMIWBEM_CONVERT_H
