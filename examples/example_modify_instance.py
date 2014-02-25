#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and changes a Login Shell
# of the given account on managed system.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   account
#   shell

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"
account  = "account"
shell    = "login_shell"

# Connect to CIMOM
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# Create LMI_Account instance name
account_iname = lmiwbem.CIMInstanceName(
    "LMI_Account",
    lmiwbem.NocaseDict({
        "CreationClassName" : "LMI_Account",
        "Name" : account,
        "SystemCreationClassName" : "PG_ComputerSystem",
        "SystemName" : hostname}),
    hostname,
    "root/cimv2")

# Get LMI_Account instance
account = conn.GetInstance(
    account_iname,
    LocalOnly=False,
    IncludeQualifiers=False,
    IncludeClassOrigin=False)

# Modify the instance
account.properties["LoginShell"] = lmiwbem.CIMProperty("LoginShell", shell)
conn.ModifyInstance(account)

# Disconnect from CIMOM
conn.disconnect()
