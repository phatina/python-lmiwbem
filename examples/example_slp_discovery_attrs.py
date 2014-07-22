#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example demonstrates, how to discover attributes using SLP.

import lmiwbem

def delim():
    print "-" * 80

def print_attrs(attrs):
    """
    Prints a SLP attrs result.

    :param dict attrs: SLP attrs result
    """
    delim()
    for k, v in attrs.iteritems():
        print "%s: %s" % (k, v)
    delim()

# Discover attributes.
attrs = lmiwbem.slp_discover_attrs("service:test.openslp://192.168.100.1:12345")

# Do something with the attributes.
print_attrs(attrs)
