Name:           holoscan-proxy
Version:        0.0.0
Release:        1%{?dist}
Summary:        Holoscan Proxy - C++ app using ZeroMQ and YAML

License:        BSD-3
URL:            https://github.com/NSLS2/holoscan-proxy.git
Source0:        holoscan-proxy-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  make
BuildRequires:  cppzmq-devel
BuildRequires:  yaml-cpp-devel

Requires:       cppzmq
Requires:       yaml-cpp
Requires:       libstdc++

%description
Holoscan Proxy is a C++ application using cppzmq and yaml-cpp for messaging and config.

%prep
%setup -q

%build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=%{_prefix}
make

%install
cd build
make install DESTDIR=%{buildroot}

%files
%license LICENSE
%doc README.md
/usr/bin/holoscan-proxy

%changelog
* Tue Sep 30 2025 Seher Karakuzu <karakuzu.seher@gmail.com> - %{version}-%{release}
- Initial RPM release
