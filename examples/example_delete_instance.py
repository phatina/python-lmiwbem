#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and deletes a user
# account from the given system.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   del_user

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"
del_user = "del_user"

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Create LMI_Account instance name
del_user_iname = lmiwbem.CIMInstanceName(
    "LMI_Account",
    lmiwbem.NocaseDict({
        "CreationClassName" : "LMI_Account",
        "Name" : del_user,
        "SystemCreationClassName" : "PG_ComputerSystem",
        "SystemName" : hostname}),
    hostname,
    "root/cimv2")

# Delete the instance
conn.DeleteInstance(del_user_iname)

# Disconnect from CIMOM
conn.disconnect()
