#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and enumerates all the
# associated instance names with an input instance name of LMI_Account.
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
        'CreationClassName' : 'LMI_Account',
        'Name' : account,
        'SystemCreationClassName' : 'PG_ComputerSystem',
        'SystemName' : hostname}),
    hostname,
    'root/cimv2')

# Open associated instance names enumeration.
associator_names_all, ctx, end = conn.OpenAssociatorNames(
    iname,
    AssocClass=None,
    ResultClass=None,
    Role=None,
    ResultRole=None,
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instance names.
while not end:
    associator_names, ctx, end = conn.PullInstanceNames(
        ctx, MaxObjectCnt=max_obj_cnt)
    associator_names_all.extend(associator_names)

# Do something with associated instance names.
print associator_names_all

# Disconnect from CIMOM.
conn.disconnect()
