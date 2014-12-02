#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider, it issues OpenExecQuery
# metod and enumerates specified instances of LMI_Account.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   account

import lmiwbem

hostname = 'hostname'
username = 'username'
password = 'password'
account  = 'account'
max_obj_cnt = 16

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Open a query in CQL language.
instances_all, ctx, end = conn.OpenExecQuery(
    'DMTF:CQL',
    'select * from LMI_Account where Name = \'%s\'' % account,
    'root/cimv2')

# Pull the rest of instances.
while not end:
    instances, ctx, end = conn.PullInstances(
        ctx, MaxObjectCnt=max_obj_cnt)
    instances_all.extend(instances)

# Do something instances.
print instances_all

# Disconnect from CIMOM.
conn.disconnect()
