#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example issues GetClass method and prints its Class name, Properties,
# Qualifiers and Methods.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   cls

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"
cls      = "ClassName"

def delim():
    print "-" * 80

def print_class(cls):
    delim()
    print "Classname:", cls.classname
    delim()
    print "Properties:", cls.properties.keys()
    delim()
    print "Qualifiers:", cls.qualifiers.keys()
    delim()
    print "Methods:", cls.methods.keys()
    delim()

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Get class
cim_cls = conn.GetClass(
    cls,
    "root/cimv2",
    LocalOnly=False,
    IncludeQualifiers=True,
    IncludeClassOrigin=True,
    PropertyList=None)

# Do something with the instance
print_class(cim_cls)

# Disconnect from CIMOM
conn.disconnect()
