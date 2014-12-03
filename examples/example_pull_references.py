#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and enumerates all the
# association instances with an input instance name of LMI_Account.
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
max_obj_cnt=16

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

iname = lmiwbem.CIMInstanceName(
    'LMI_Account',
    lmiwbem.NocaseDict({
        'CreationClassName': 'LMI_Account',
        'Name': account,
        'SystemCreationClassName': 'PG_ComputerSystem',
        'SystemName': hostname}),
    hostname,
    'root/cimv2')

# Open association instances enumeration.
references_all, ctx, end = conn.OpenReferences(
    iname,
    ResultClass=None,
    Role=None,
    IncludeClassOrigin=True,
    PropertyList=None,
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instances.
while not end:
    references, ctx, end = conn.PullInstances(
        ctx, MaxObjectCnt=max_obj_cnt)
    references_all.extend(references)

# Do something with associated instances.
print references_all

# Disconnect from CIMOM.
conn.disconnect()
