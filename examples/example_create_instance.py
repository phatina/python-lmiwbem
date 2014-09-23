#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider and adds a user account
# to the given group on managed system.
#
# To make this example work, modify following variables:
#   hostname
#   username
#   password
#   account
#   group

import lmiwbem

hostname = "hostname"
username = "username"
password = "password"
account  = "account"
group    = "group"

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

# Get account LMI_Identity association
identity_iname = conn.AssociatorNames(
    account_iname,
    ResultClass="LMI_Identity")[0]

# Create LMI_Group instance name
group_iname = lmiwbem.CIMInstanceName(
    "LMI_Group",
    lmiwbem.NocaseDict({
        "CreationClassName" : "LMI_Group",
        "Name" : group}),
    hostname,
    "root/cimv2")

if 1:
    # Prepare LMI_MemberOfGroup instance
    member_of_group = lmiwbem.CIMInstance(
        "LMI_MemberOfGroup",
        lmiwbem.NocaseDict({
            "Member" : identity_iname,
            "Collection" : group_iname}),
        # No qualifiers
        None,
        lmiwbem.CIMInstanceName(
            "LMI_MemberOfGroup",
            # No keybindings; Member and Collection are not key properties in
            # LMI_MemberOfGroup
            None,
            hostname,
            "root/cimv2"))

    # Create new instance of LMI_MemberOfGroup
    new_iname = conn.CreateInstance(member_of_group)
else:
    # CIMInstance can be constructed without CIMInstanceName.
    member_of_group = lmiwbem.CIMInstance(
        "LMI_MemberOfGroup",
        lmiwbem.NocaseDict({
            "Member" : identity_iname,
            "Collection" : group_iname}),
        # No qualifiers
        None,
        # No path
        None)

    # Create new instance of LMI_MemberOfGroup
    new_iname = conn.CreateInstance(member_of_group, "root/cimv2")

# Do something with new CIMInstanceName
print new_iname

# Disconnect from CIMOM
conn.disconnect()
