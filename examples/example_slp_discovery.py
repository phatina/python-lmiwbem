#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example demonstrates, how to discover services using SLP.

import lmiwbem


def delim():
    print '-' * 80


def print_slp_result(result):
    '''
    Prints a SLP result.

    :param lmiwbem.SLPResult result: SLP result to print
    '''
    delim()
    print 'Type:', result.srvtype
    print 'Host:', result.host
    print 'Port:', result.port
    delim()
    print ''


# Discover WBEM services.
services = lmiwbem.slp_discover('service:wbem')

# Do something with SLP results.
for result in services:
    print_slp_result(result)
