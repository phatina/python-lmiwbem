LMIWBEM Usage Examples
======================

This section describes common LMIWBEM use-cases.

.. _example_connection:

Connecting to CIMOM
-------------------

This section describes and demonstrates, how to create a connection object,
through which all the CIM operations are performed.

There are 3 means, how to create and use the connection object:

- Providing username and password
- Providing X509 certificate
- Using Unix socket

Connection using username/password
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Major difference, when compared to PyWBEM, is, that it is possible to create
and maintain a connection with the CIMOM. See the next example:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection()
   conn.connect("hostname", "username", "password")

   # CIM operations

   conn.disconnect()

It is possible to create a connection object in PyWBEM's way. The connection to
CIMOM is opened for each CIM operation and when done, the connection is closed.
See the next example:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection("hostname", ("username", "password"))

   # CIM operations

**NOTE:** When :py:meth:`.WBEMConnection.connect` is not called, temporary
connection for each CIM operations will be performed.

Connection using X509 certificate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Apart from username/password authentication, LMIWBEM is capable of using X509
certificate and private key for authentication purposes. See the next example:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection()
   conn.connect(
       "hostname",
       cert_file="path/to/certificate.pem",
       key_file="path/to/private_key.pem")

   # CIM operations

   conn.disconnect()

Or PyWBEM's way:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection(
       "hostname",
       x509={
           "cert_file" : "path/to/certificate.pem",
           "key_file"  : "path/to/private_key.pem"})

   # CIM operations

**NOTE:** When :py:meth:`.WBEMConnection.connect` is not called, temporary
connection for each CIM operations will be performed.

.. _example_connection_socket:

Connection using Unix Socket
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

LMIWBEM is capable of creating a connection via Unix socket. See next 3
following examples:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection()
   conn.connectLocally()

   # CIM operations

   conn.disconnect()

Or:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection(connect_locally=True)
   conn.connect() # No need to call connectLocally()

   # CIM operations

   conn.disconnect()

Even:

.. code-block:: python

   import lmiwbem

   conn = lmiwbem.WBEMConnection(connect_locally=True)

   # CIM operations

**NOTE:** When :py:meth:`.WBEMConnection.connect` is not called, temporary
connection for each CIM operations will be performed.

Supported CIM operations
------------------------

Following CIM operations are supported in :py:class:`.WBEMConnection`:

- :py:meth:`.WBEMConnection.AssociatorNames` (Full example :ref:`example_associator_names`)
- :py:meth:`.WBEMConnection.Associators` (Full example :ref:`example_associators`)
- :py:meth:`.WBEMConnection.CreateInstance` (Full example :ref:`example_create_instance`)
- :py:meth:`.WBEMConnection.DeleteInstance` (Full example :ref:`example_delete_instance`)
- :py:meth:`.WBEMConnection.EnumerateClassNames` (Full example :ref:`example_enumerate_class_names`)
- :py:meth:`.WBEMConnection.EnumerateClasses` (Full example :ref:`example_enumerate_classes`)
- :py:meth:`.WBEMConnection.EnumerateInstanceNames` (Full example :ref:`example_enumerate_instance_names`)
- :py:meth:`.WBEMConnection.EnumerateInstances` (Full example :ref:`example_enumerate_instances`)
- :py:meth:`.WBEMConnection.ExecQuery` (Full example :ref:`example_exec_query`)
- :py:meth:`.WBEMConnection.GetClass` (Full example :ref:`example_get_class`)
- :py:meth:`.WBEMConnection.GetInstance` (Full example :ref:`example_get_instance`)
- :py:meth:`.WBEMConnection.InvokeMethod` (Full example :ref:`example_invoke_method`)
- :py:meth:`.WBEMConnection.ModifyInstance` (Full example :ref:`example_modify_instance`)
- :py:meth:`.WBEMConnection.ReferenceNames` (Full example :ref:`example_reference_names`)
- :py:meth:`.WBEMConnection.References` (Full example :ref:`example_references`)

Indication Listener
-------------------

This section briefly describes, how to create :py:class:`.CIMIndicationListener`
and start listening for incoming indications.

.. code-block:: python

   import lmiwbem

   def handler(indication, *args, **kwargs):
       """
       :param CIMInstance indication: exported instance
       :param *args: positional arguments
       :param **kwargs: keyword arguments
       """
       # Do something with exported indication
       pass

    listener = lmiwbem.CIMIndicationListener()
    listener.add_handler("indication_name", handler, *args, **kwargs)
    listener.start()

    #
    # Do something useful here
    #

    listener.stop()

**Break-down:**

1. Define a indication handler:

    .. code-block:: python

       def handler(indication, *args, **kwargs):
           ...

    The handler needs to take at least 1 argument (`indication` in the example)
    and it can also accept any other positional and keyword arguments. Values
    of these optional arguments will be defined when registering the indication
    handler in a indication listener.

2. Create a indication listener and register the indication handler:

    .. code-block:: python

       listener = lmiwbem.CIMIndicationListener()
       listener.add_handler("indication_name", handler, *args, **kwargs)

    :py:meth:`.CIMIndicationListener.add_handler` takes at least 2 arguments,
    where the first one defines the indication name and the second is callable
    handler.  It is possible to pass any optional (user defined) positional and
    keywords arguments here – they will passed to indication handler when the
    indication arrives.

    **NOTE:** When creating indication subscription, **it is substantial to prefix
    indication name with "CIMListener"**. If the prefix is missing, indication will
    not be processed by :py:class:`.CIMIndicationListener`. The next listing
    demonstrates `CIMIndicationHandler` instance with proper `Destination`
    property:

    .. code-block:: python

       instance of CIM_IndicationHandlerCIMXML {
           ...

           Destination = 'https://destination:port/CIMListener/indication_name';
                                                   ^^^^^^^^^^^
                                                THIS IS IMPORTANT

           ...
       };

3. Start the indication listener:

    .. code-block:: python

       listener.start()

4. Some user-defined code

    .. code-block:: python

       #
       # Do something useful here
       #

5. Stop the indication listener:

    .. code-block:: python

       listener.stop()
