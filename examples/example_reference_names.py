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
import sys

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

# Get association CIMInstanceNames with iname
reference_inames = conn.ReferenceNames(
    iname,
    ResultClass=None,
    Role=None)

# Do something with instance names
print reference_inames

# Disconnect from CIMOM
conn.disconnect()
