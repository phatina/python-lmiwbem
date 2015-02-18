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

#ifndef   LMIWBEM_CLIENT_WSMAN_BUILDER_H
#  define LMIWBEM_CLIENT_WSMAN_BUILDER_H

#  include <boost/property_tree/ptree.hpp>
#  include <Pegasus/Common/Array.h>
#  include "lmiwbem.h"
#  include "util/lmiwbem_string.h"

PEGASUS_BEGIN
class CIMInstance;
class CIMParamValue;
class CIMObjectPath;
class CIMValue;
PEGASUS_END

namespace ptree = boost::property_tree;

class String;

class ObjectFactory
{
public:
    static Pegasus::CIMInstance makeCIMInstance(
        const String &classname,
        const ptree::ptree &instance);
    static Pegasus::CIMInstance makeCIMInstance(
        const String &xml);

    static Pegasus::CIMInstance makeCIMInstanceWithPath(
        const String &xml,
        const String &hostname,
        const String &namespace_);

    static Pegasus::CIMObjectPath makeCIMInstanceName(
        const String &xml,
        const String &hostname,
        const String &namespace_);
    static Pegasus::CIMObjectPath makeCIMInstanceName(
        const ptree::ptree &epr,
        const String &hostname,
        const String &namespace_);

    static Pegasus::CIMValue makeMethodReturnValue(
        const String &xml,
        const String &method_name,
        Pegasus::Array<Pegasus::CIMParamValue> &out_parameters);

private:
    ObjectFactory();
    ObjectFactory(const ObjectFactory &copy);
};

#endif // LMIWBEM_CLIENT_WSMAN_BUILDER_H
