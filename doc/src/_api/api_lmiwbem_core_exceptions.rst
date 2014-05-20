Exceptions
==========

.. autoexception:: lmiwbem.lmiwbem_core.CIMError

   Raised when a CIM operations fails. Following constants may be used to
   identify error type:

   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_FAILED
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_ACCESS_DENIED
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_INVALID_NAMESPACE
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_INVALID_PARAMETER
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_INVALID_CLASS
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_NOT_FOUND
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_NOT_SUPPORTED
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_CLASS_HAS_CHILDREN
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_CLASS_HAS_INSTANCES
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_INVALID_SUPERCLASS
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_ALREADY_EXISTS
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_NO_SUCH_PROPERTY
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_TYPE_MISMATCH
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_INVALID_QUERY
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_METHOD_NOT_AVAILABLE
   .. autoattribute:: lmiwbem.lmiwbem_core.CIM_ERR_METHOD_NOT_FOUND


.. autoexception:: lmiwbem.lmiwbem_core.ConnectionError

   Raised when a connection to CIMOM fails. Following constants may be used to
   identify error type:

   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_OTHER
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_ALREADY_CONNECTED
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_NOT_CONNECTED
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_INVALID_LOCATOR
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_CANNOT_CREATE_SOCKET
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_CANNOT_CONNECT
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_CONNECTION_TIMEOUT
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_SSL_EXCEPTION
   .. autoattribute:: lmiwbem.lmiwbem_core.CON_ERR_BIND

