#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example demonstrates, how to use Indication Listener.
#

from time import sleep
import lmiwbem


def handler(indication, arg1, arg2, **kwargs):
    # Do something with user specific data
    print arg1, arg2, kwargs
    # Do something with the indication CIMInstance
    for k, v in indication.properties.iteritems():
        print '\t%s: %s' % (k, v.value)


# Create indication listener.
listener = lmiwbem.CIMIndicationListener()

# Add handler for indication with name 'storage'.
listener.add_handler('storage', handler, 'arg1', 'arg2', arg3=10, arg4=True)

# Start indication listener.
listener.run()

# Run forever...
try:
    while True:
        sleep(8)
except KeyboardInterrupt, e:
    pass

listener.stop()
