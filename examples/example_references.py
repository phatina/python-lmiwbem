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

# Get association instances with iname.
references = conn.References(
    iname,
    ResultClass=None,
    Role=None,
    IncludeQualifiers=True,
    IncludeClassOrigin=True,
    PropertyList=None)

# Do something with associated instances.
print references

# Disconnect from CIMOM.
conn.disconnect()
