Unclassified
============

.. autofunction:: lmiwbem.lmiwbem_core.is_subclass

.. autofunction:: lmiwbem.lmiwbem_core.slp_discover

.. autofunction:: lmiwbem.lmiwbem_core.slp_discover_attrs

.. autoattribute:: lmiwbem.lmiwbem_core.DEFAULT_NAMESPACE

   This variable is used, when no namespace parameter is provided to
   :py:class:`.WBEMConnection`.

.. autoattribute:: lmiwbem.lmiwbem_core.config

   .. autoattribute:: lmiwbem.lmiwbem_core.config.DEFAULT_TRUST_STORE

      This variable is used, when SSL connection is applied.

   .. autoattribute:: lmiwbem.lmiwbem_core.config.EXCEPTION_VERBOSITY

      This attribute defines the exceptions verbosity. There are 3 applicable levels:

      .. autoattribute:: lmiwbem.lmiwbem_core.EXC_VERB_NONE

         No additional information is added to exceptions' args.

      .. autoattribute:: lmiwbem.lmiwbem_core.EXC_VERB_CALL

         Call prototype is added to exceptions' args.

      .. autoattribute:: lmiwbem.lmiwbem_core.EXC_VERB_MORE

         Call prototype and other useful information is added to exceptions' args.
