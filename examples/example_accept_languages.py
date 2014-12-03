#!/usr/bin/python
#
# Author: alexanderlaw <a.lahin@ntcit-rosa.ru>
#
# This example demonstrates how to get/set the accept languages.

import lmiwbem


conn = lmiwbem.WBEMConnection()

# Get Accept languages.
print conn.request_accept_languages

# Set Accept languages.
conn.request_accept_languages = [('ru', 1.0), ('en', 0.5)]
