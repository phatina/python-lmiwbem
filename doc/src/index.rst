LMIWBEM Documentation
=====================

*LMIWBEM is a Python library that performs CIM operations using CIM-XML
protocol. The library tries to mimic PyWBEM, but does things in different way:*

- TOG-Pegasus client's library is used for communication
- lazy evaluation of CIM objects is used
- some minor API was added for performance reasons

**Benefits of the library:**

- **Performance** – Using *TOG-Pegasus* client's library, *Boost::Python* and C++ STL,
  CIM operations are executed faster (e.g. ~60% faster Instance enumeration
  compared to PyWBEM).

  **NOTE:** performance gain depends on CIM operation; there are CIM operations
  where the percentage is even higher.

- **Memory** – Using PyWBEM, the Python's interpreter can eat up several GB of
  memory. In LMIWBEM, C++ allocator for CIM objects is used and unnecessary
  memory blocks are properly freed and returned to OS. *Lazy evaluation*
  (construction) of nested objects helps to perform CIM operations faster and
  Python does not use additional space for such objects unless it's necessary.


**Content:**

.. toctree::
   :maxdepth: 3

   usage
   api
   examples

Indices and tables
------------------

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

