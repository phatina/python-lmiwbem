#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example enumerates all the class names managed by CIMOM and prints them
# out.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Enumerate Instances
class_names = conn.EnumerateClassNames(
    "root/cimv2",
    ClassName=None,
    DeepInheritance=True)

# Do something with instances
print class_names

# Disconnect from CIMOM
conn.disconnect()
