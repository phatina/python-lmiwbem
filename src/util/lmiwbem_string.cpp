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
#include "util/lmiwbem_string.h"

String::String()
    : std::string()
{
}

String::String(const char *str)
    : std::string(str)
{
}

String::String(const char *str, size_t pos, size_t len)
    : std::string(str, pos, len)
{
}

String::String(const std::string &str)
    : std::string(str)
{
}

String::String(const Pegasus::String &str)
    : std::string(str.getCString())
{
}

String::String(size_t n, char c)
    : std::string(n, c)
{
}

std::string String::asStdString() const
{
    return *this;
}

Pegasus::String String::asPegasusString() const
{
    return Pegasus::String(c_str());
}

String::operator Pegasus::String() const
{
    return asPegasusString();
}

std::ostream &operator<<(std::ostream &o, const String &str)
{
    o.write(str.data(), str.size());
    return o;
}

String &String::operator=(const char *rhs)
{
    std::string::operator=(rhs);
    return *this;
}

String &String::operator=(const String &rhs)
{
    std::string::operator=(rhs);
    return *this;
}

String &String::operator=(const std::string &rhs)
{
    std::string::operator=(rhs);
    return *this;
}

String &String::operator=(const Pegasus::String &rhs)
{
    std::string::operator=(rhs.getCString());
    return *this;
}

String &String::operator+=(const Pegasus::String &rhs)
{
    std::string::operator+=(rhs.getCString());
    return *this;
}

String String::operator+(const String &rhs)
{
    String result(*this);
    result += rhs;
    return result;
}
