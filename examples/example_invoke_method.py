#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider, it issues InvokeMethod
# metod within specified LMI_AccountManagementService and creates a new user
# account on the given system.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   new_user

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"
new_user = "new_user"

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Create LMI_AccountManagementService instance name
lams_iname = lmiwbem.CIMInstanceName(
    "LMI_AccountManagementService",
    lmiwbem.NocaseDict({
        "CreationClassName" : "LMI_AccountManagementService",
        "Name" : "OpenLMI Linux Users Account Management Service",
        "SystemCreationClassName" : "PG_ComputerSystem",
        "SystemName" : hostname}),
    hostname,
    "root/cimv2")

# Create PG_ComputerSystem instance name
cs_iname = lmiwbem.CIMInstanceName(
    "PG_ComputerSystem",
    lmiwbem.NocaseDict({
        "CreationClassName" : "PG_ComputerSystem",
        "Name" : hostname}),
    hostname,
    "root/cimv2")

# Invoke the CreateAccount method
rval = conn.InvokeMethod(
    "CreateAccount",
    lams_iname,
    Name=new_user,
    System=cs_iname,
)

# Do something with the result
print rval

# Disconnect from CIMOM
conn.disconnect()
