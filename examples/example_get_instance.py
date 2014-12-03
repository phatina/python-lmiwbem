#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example is based upon openlmi-account provider. It issues GetInstance
# method, retrieves a LMI_Instance from the remote machine and prints out
# its Class name, Path, Properties and Qualifiers.
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
cls      = 'LMI_Account'
account  = 'account'


def delim():
    print '-' * 80


def print_instance(inst):
    delim()
    print 'Classname:', inst.classname
    delim()
    print 'Path:', inst.path
    delim()
    print 'Properties:', inst.properties.keys()
    delim()
    print 'Qualifiers:', inst.qualifiers.keys()
    delim()


# Connect to CIMOM.
conn = lmiwbem.WBEMConnection()
conn.connect(hostname, username, password)

# First, create CIMInstanceName.
tester_iname = lmiwbem.CIMInstanceName(
    cls,
    lmiwbem.NocaseDict({
      'CreationClassName': cls,
      'Name': account,
      'SystemCreationClassName': 'PG_ComputerSystem',
      'SystemName': hostname}),
    'virt-rawhide',
    'root/cimv2')

# Get Instance.
account = conn.GetInstance(
    tester_iname,
    LocalOnly=False,
    IncludeQualifiers=True,
    IncludeClassOrigin=True)

# Do something with the instance.
print_instance(account)

# Disconnect from CIMOM.
conn.disconnect()
