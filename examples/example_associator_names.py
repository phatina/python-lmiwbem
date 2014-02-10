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

hostname = "hostname"
username = "username"
password = "password"
account  = "account"

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

iname = lmiwbem.CIMInstanceName(
    "LMI_Account",
    lmiwbem.NocaseDict({
        "CreationClassName" : "LMI_Account",
        "Name" : account,
        "SystemCreationClassName" : "PG_ComputerSystem",
        "SystemName" : hostname}),
    hostname,
    "root/cimv2")

# Get associated instance names with iname
associated_inames = conn.AssociatorNames(
    iname,
    AssocClass=None,
    ResultClass=None,
    Role=None,
    ResultRole=None)

# Do something with associated instance names
print associated_inames

# Disconnect from CIMOM
conn.disconnect()
