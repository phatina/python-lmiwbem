#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
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
accounts = conn.EnumerateInstances(
    "LMI_Account",
    "root/cimv2",
    LocalOnly=False,
    DeepInheritance=True,
    IncludeQualifiers=True,
    IncludeClassOrigin=True,
    PropertyList=None)

# Do something with instances
print accounts

# Disconnect from CIMOM
conn.disconnect()
