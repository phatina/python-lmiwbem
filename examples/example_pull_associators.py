#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and enumerates all the
# associated instances with an input instance name of LMI_Account.
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

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

iname = lmiwbem.CIMInstanceName(
    'LMI_Account',
    lmiwbem.NocaseDict({
        'CreationClassName' : 'LMI_Account',
        'Name' : account,
        'SystemCreationClassName' : 'PG_ComputerSystem',
        'SystemName' : hostname}),
    hostname,
    'root/cimv2')

# Open associated instances enumeration.
associators_all, ctx, end = conn.OpenAssociators(
    iname,
    AssocClass=None,
    ResultClass=None,
    Role=None,
    ResultRole=None,
    IncludeClassOrigin=True,
    PropertyList=None,
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instances.
while not end:
    associators, ctx, end = conn.PullInstances(
        ctx, MaxObjectCnt=max_obj_cnt)
    associators_all.extend(associators)

# Do something with associated instances.
print associators_all

# Disconnect from CIMOM.
conn.disconnect()
