#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example issues EnumerateInstances method, retrieves all the
# instance names from the remote machine and prints them out.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   cls

import lmiwbem


hostname = 'hostname'
username = 'username'
password = 'password'
cls = 'class'

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Enumerate Instances.
accounts = conn.EnumerateInstances(
    cls,
    'root/cimv2',
    LocalOnly=False,
    DeepInheritance=True,
    IncludeQualifiers=True,
    IncludeClassOrigin=True,
    PropertyList=None)

# Do something with instances.
print accounts

# Disconnect from CIMOM.
conn.disconnect()
