Name:           holoscan-proxy
Version:        0.0.0
Release:        1%{?dist}
Summary:        Holoscan Proxy - C++ app using ZeroMQ and YAML

License:        BSD-3
URL:            https://github.com/yourusername/holoscan-proxy
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
* $(date +"%a %b %d %Y") Your Name <you@example.com> - %{version}-%{release}
- Initial RPM release
