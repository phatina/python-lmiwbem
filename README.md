ABOUT
=====

LMIWBEM is a Python library that performs CIM operations using CIM-XML
protocol.  The library tries to mimic PyWBEM, but does things in different way:

- TOG-Pegasus client's library is used for communication
- lazy evaluation of CIM objects is used
- some minor API was added for performance reasons

Reasons why the library was created:

- *Performance* - Using TOG-Pegasus client's library, Boost::Python and C++ STL,
  CIM operations are executed faster (e.g. ~60% faster Instance enumeration
  compared to PyWBEM).

  **NOTE:** performance gain depends on CIM operation; there
  are CIM operations where the percentage is even higher.
- *Memory* - Using PyWBEM, the Python's interpreter can eat up to several GB of
  memory. In LMIWBEM, C++ allocator for CIM objects is used and unnecessary
  memory blocks are properly freed and returned to OS. Lazy evaluation
  (construction) of nested objects helps to perform CIM operations faster and
  Python does not use additional space for such objects unless it's necessary.


LATEST VERSION
==============

Latest sources of the software can be found at: [lmiwbem][]


INSTALLATION
============

LMIWBEM has several dependencies:

- C++ compiler with support of C++98 standard
- tog-pegasus-libs (2.12.0)
- boost-python (1.50.0)
- python-devel
- build tools (autoconf, automake, make, ...)

LMIWBEM supports 2 build systems:
- Autotools (*must precede distutils*)
- Python's distutils.


AUTOTOOLS
---------

To install the module using `configure` script, follow these steps:

    $ ./autogen.sh # if you got sources from git repository
    $ mkdir build && cd build
    $ ../configure
    $ make
    # make install


DISTUTILS
---------

To install the module using Python's distutils, follow these steps:

    $ ./autogen.sh # if you got sources from git repository
    $ mkdir build && cd build
    $ ../configure
    # python setup.py install

**NOTE:** It is necessary to run `configure` script, which generates
Python's setup.py for build and installation.


OPTIONS
-------

These options apply for `configure` and `setup.py` will be generated
based on the options selected:

    --with-default-namespace=NAMESPACE; default: root/cimv2
    --with-default-trust-store=DIR;     default: /etc/pki/ca-trust/source/anchors/
    --with-listener=[yes/no];           default: yes
    --with-slp=[yes/no];                default: yes

By default, LMIWBEM is configured to work with Python 2.6.x and 2.7.x. It is
possible to build the project with Python 3.x.x compatibility by running
`configure` script with:

    --with-python3=yes

If the module is built using distutils, Python version is determined at
runtime. Setup with Python 3.x.x:

    $ python3 setup.py install


USAGE
=====

Currently supported intrinsic methods:

- Associators()
- AssociatorNames()
- CreateInstance()
- DeleteInstance()
- EnumerateClassNames()
- EnumerateClasses()
- EnumerateInstanceNames()
- EnumerateInstances()
- ExecQuery()
- GetClass()
- GetInstance()
- InvokeMethod()
- ModifyInstance()
- References()
- ReferenceNames()
- OpenAssociators()
- OpenAssociatorNames()
- OpenEnumerateInstances()
- OpenEnumerateInstanceNames()
- OpenExecQuery()
- OpenReferences()
- OpenReferenceNames()

**NOTE:** See the directory `examples`.

There is major difference when compared to PyWBEM: using LMIWBEM, it is
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

**NOTE:** It is not necessary to call `.disconnect()` when using such
structure.


PERFORMANCE
===========

[LMI Meta-Command][] uses "real" world constructions in LMIShell and
was used for benchmarking purposes.


SETUP
-----

- Managed system:
  - openlmi-account-0.4.2-2.fc21
  - openlmi-logicalfile-0.4.2-2.fc21
  - openlmi-hardware-0.4.2-2.fc21
  - openlmi-networking-0.2.2-3.fc21
  - openlmi-service-0.4.2-2.fc21
  - openlmi-storage-0.7.1-2.fc21

- Client system:
  - openlmi-scripts-account-0.0.1-7.fc20
  - openlmi-scripts-logicalfile-0.0.3-7.fc20
  - openlmi-scripts-hardware-0.0.3-7.fc20
  - openlmi-scripts-networking-0.0.2-7.fc20
  - openlmi-scripts-service-0.1.2-7.fc20
  - openlmi-scripts-storage-0.0.4-7.fc20
  - lmishell with pywbem/lmiwbem


RESULTS
-------

|             Command                |       PyWBEM        |      LMIWBEM        |
|------------------------------------|---------------------|---------------------|
| `lmi file list <dir>`              | 21662.043ms (349MB) |  6837.136ms (273MB) |
| `lmi group list`                   |   411.938ms (241MB) |   236.245ms (267MB) |
| `lmi hwinfo`                       |  1699.851ms (246MB) |   631.720ms (267MB) |
| `lmi net device list`              |   697.675ms (236MB) |   348.451ms (267MB) |
| `lmi net device show`              |  3016.917ms (246MB) |  1147.636ms (267MB) |
| `lmi net setting list`             |   417.707ms (235MB) |   248.474ms (267MB) |
| `lmi service list`                 | 60996.685ms (269MB) | 58527.529ms (267MB) |
| `lmi storage show /dev/vda`        |  1790.241ms (244MB) |   843.254ms (267MB) |
| `lmi storage tree /dev/vda`        |  3069.120ms (246MB) |   973.947ms (267MB) |
| `lmi storage fs list`              |   854.803ms (251MB) |   487.202ms (267MB) |
| `lmi storage mount list`           |  1503.447ms (248MB) |  1063.852ms (267MB) |
| `lmi storage partition list`       |   523.848ms (243MB) |   278.820ms (267MB) |
| `lmi storage partition-table list` |  1861.788ms (246MB) |   935.674ms (267MB) |
| `lmi user list`                    |  2510.069ms (232MB) |  2175.117ms (267MB) |

**NOTE:** LMIWBEM uses Boost::Python, which brings a little overhead with the
module itself.  This can be seen in memory results.


BUG REPORTS
===========

Report bugs to [phatina@redhat.com](mailto:phatina@redhat.com) or [lmiwbem issues][].

[lmiwbem]: https://github.com/phatina/lmiwbem "lmiwbem repository on github"
[lmiwbem issues]: https://github.com/phatina/lmiwbem/issues "Report a bug"
[LMI Meta-Command]: https://git.fedorahosted.org/git/openlmi-tools.git
