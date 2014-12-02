#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider. It issues
# OpenEnumerateInstances method, retrieves all the LMI_Account's instances
# from the remote machine and prints them out.
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
cls = 'LMI_Account'
max_obj_cnt = 16

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Open an instance enumeration.
accounts_all, ctx, end = conn.OpenEnumerateInstances(
    cls,
    'root/cimv2',
    DeepInheritance=True,
    IncludeClassOrigin=True,
    PropertyList=None,
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instances.
while not end:
    accounts, ctx, end = conn.PullInstances(ctx, MaxObjectCnt=max_obj_cnt)
    accounts_all.extend(accounts)

# Do something with instances.
print accounts_all

# Disconnect from CIMOM.
conn.disconnect()
