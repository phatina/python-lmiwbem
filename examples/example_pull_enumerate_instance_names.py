#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider. It issues
# OpenEnumerateInstanceNames method, retrieves all the LMI_Account's instance
# names from the remote machine and prints them out.
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

# Open an instance name enumeration.
inames_all, ctx, end = conn.OpenEnumerateInstanceNames(
    cls,
    'root/cimv2',
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instance names.
while not end:
    inames, ctx, end = conn.PullInstanceNames(ctx, MaxObjectCnt=max_obj_cnt)
    inames_all.extend(inames)

# Do something with instance names.
print inames_all

# Disconnect from CIMOM.
conn.disconnect()
