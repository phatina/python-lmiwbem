ABOUT
=====

LMIWBEM is a Python library, which performs CIM operations using CIM-XML
protocol. The library tries to mimic PyWBEM, but does things in different way:

- TOG-Pegasus client's library is used for communication
- lazy evaluation of CIM objects is used
- some minor API was added for performance reasons

Reasons, why the library was made:

- *Performance* - Using TOG-Pegasus client's library, Boost::Python and C++ STL,
  CIM operations are executed faster (e.g. ~60% faster Instance enumeration
  compared to PyWBEM).

  **NOTE:** performance gain depends on CIM operation; there
  are CIM operations, where the percentage is even higher.
- *Memory* - Using PyWBEM, the Python's interpreter can eat up several GB of
  memory. In LMIWBEM, C++ allocator for CIM objects is used and unnecessary
  memory blocks are properly freed and returned to OS. Lazy evaluation
  (construction) of nested objects helps to perform CIM operations faster and
  Python does not use additional space for such objects, when not needed.

LASTEST VERSION
===============

Latest sources of the software can be found at: [lmiwbem][]

INSTALLATION
============

LMIWBEM has several dependencies:

- C++ compiler, which supports C++98 standard
- tog-pegasus-libs (2.12.0)
- boost-python (1.50.0)
- python-devel
- build tools (autoconf, automake, make, ...)

To install the module, follow these steps:

    $ mkdir build && cd build
    $ ../configure
    $ make
    # make install

If you got the sources from git repository, first you have to run:

    $ ./autogen.sh

USAGE
=====

Currently supported intrinsic methods:

- EnumerateClassNames()
- EnumerateClasses()
- EnumerateInstanceNames()
- EnumerateInstances()
- GetClass()
- GetInstance()

**NOTE:** See the directory `examples`.

There is major difference, when compared to PyWBEM: using LMIWBEM, it is
possible to create and maintain a connection with a CIMOM:

``` python
import lmiwbem

conn = lmiwbem.WBEMConnection()
conn.connect("hostname", "username", "password")
...
conn.disconnect()
```

It is possible to create connections in PyWBEM's way:

``` python
import lmiwbem

conn = lmiwbem.WBEMConnection("hostname", ("username", "password"))
...
```

**NOTE:** It is not necessary to call `.disconnect()`, when using such
structure.

BUG REPORTS
===========

Report bugs to [phatina@redhat.com](mailto:phatina@redhat.com) or
[lmiwbem issues][].

[lmiwbem]: https://github.com/phatina/lmiwbem "lmiwbem repository on github"
[lmiwbem issues]: https://github.com/phatina/lmiwbem/issues "Report a bug"
