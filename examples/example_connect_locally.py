#!/usr/bin/python
#
# Author: Peter Hatina <phatina@redhat.com>
#
# This example demonstrates how to perform Unix socket connection to CIMOM.

import lmiwbem


conn = lmiwbem.WBEMConnection()
conn.connectLocally()

# Do something useful.

conn.disconnect()

# -------------------------------------

conn = lmiwbem.WBEMConnection(connect_locally=True)
conn.connect() # No need to call connectLocally().

# Do something useful.

conn.disconnect()
