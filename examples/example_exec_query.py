#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider, it issues ExecQuery
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

# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Execute a query in CQL language.
instances = conn.ExecQuery(
    'DMTF:CQL',
    'select * from LMI_Account where Name = \'%s\'' % account,
    'root/cimv2')

# Do something instances.
print instances

# Disconnect from CIMOM.
conn.disconnect()
