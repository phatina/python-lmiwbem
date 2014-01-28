Name:           lmiwbem
Version:        0.1.0
Release:        1%{?dist}
Summary:        Python WBEM Client
License:        LGPLv2+
URL:            https://github.com/phatina/lmiwbem
Source0:        https://github.com/phatina/lmiwbem/releases/download/%{name}-%{version}/%{name}-%{version}.tar.gz

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  python2-devel
BuildRequires:  boost-devel > 1.50.0
BuildRequires:  boost-python >= 1.50.0
BuildRequires:  tog-pegasus-devel >= 2.12.0
BuildRequires:  tog-pegasus-libs >= 2.12.0
Requires:       boost-python > 1.50.0

%description
%{name} is a Python library, which performs CIM operations using CIM-XML
protocol. The library tries to mimic PyWBEM.

%prep
%setup -q -n %{name}-%{version}

%build
%configure
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install
find %{buildroot} -name '*.la' | xargs rm -f

%files
%doc COPYING README.md
%{python_sitearch}/lmiwbem/

%changelog
* Wed Jan 29 2014 Peter Hatina <phatina@redhat.com> - 0.1.0-1
- initial import
