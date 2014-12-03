#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and enumerates all the
# association instance names with an input instance name of LMI_Account.
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

# Get association CIMInstanceNames with iname.
reference_inames_all, ctx, end = conn.OpenReferenceNames(
    iname,
    ResultClass=None,
    Role=None,
    MaxObjectCnt=max_obj_cnt)

# Pull the rest of instance names.
while not end:
    reference_inames, ctx, end = conn.PullInstanceNames(
        ctx, MaxObjectCnt=max_obj_cnt)
    reference_inames_all.extend(reference_inames)

# Do something with instance names.
print reference_inames_all

# Disconnect from CIMOM.
conn.disconnect()
