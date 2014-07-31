Name:           lmiwbem
Version:        0.2.0
Release:        1%{?dist}
Summary:        Python WBEM Client
License:        LGPLv2+
URL:            https://github.com/phatina/lmiwbem
Source0:        https://github.com/phatina/lmiwbem/releases/download/%{name}-%{version}/%{name}-%{version}.tar.gz

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  python2-devel
BuildRequires:  python-sphinx
BuildRequires:  python-sphinx-theme-openlmi
BuildRequires:  boost-devel >= 1.50.0
BuildRequires:  boost-python >= 1.50.0
BuildRequires:  tog-pegasus-devel >= 2.12.0
BuildRequires:  tog-pegasus-libs >= 2.12.0
Requires:       boost-python >= 1.50.0
Requires:       python
Requires:       tog-pegasus-libs >= 2.12.0

%description
%{name} is a Python library, which performs CIM operations using CIM-XML
protocol. The library tries to mimic PyWBEM.

%package doc
Summary:        Documentation for %{name}
Group:          Documentation

%description doc
%{summary}

%prep
%setup -q -n %{name}-%{version}

%build
%configure --with-doc=yes
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install
find %{buildroot} -name '*.la' | xargs rm -f

%files
%doc COPYING README.md
%{python_sitearch}/lmiwbem/

%files doc
%dir %{_docdir}/%{name}-%{version}
%{_docdir}/%{name}-%{version}/html

%changelog
* Thu Jul 31 2014 Peter Hatina <phatina@redhat.com> - 0.2.0-2
- fix (Build)Requires

* Wed May 21 2014 Peter Hatina <phatina@redhat.com> - 0.2.0-1
- upgrade to 0.2.0
- introduce lmiwbem-doc package

* Wed Jan 29 2014 Peter Hatina <phatina@redhat.com> - 0.1.0-1
- initial import
