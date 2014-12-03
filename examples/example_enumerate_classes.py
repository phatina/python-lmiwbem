#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example enumerates all available classes managed by CIMOM and prints them
# out.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password

import lmiwbem


hostname = 'hostname'
username = 'username'
password = 'password'

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Enumerate instances.
classes = conn.EnumerateClasses(
    'root/cimv2',
    ClassName=None,
    DeepInheritance=True,
    LocalOnly=False,
    IncludeQualifiers=True,
    IncludeClassOrigin=True)

# Do something with instances.
print classes

# Disconnect from CIMOM.
conn.disconnect()
