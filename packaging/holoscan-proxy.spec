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
#BuildRequires:  cppzmq-devel
BuildRequires:  yaml-cpp-devel
BuildRequires:  redhat-rpm-config
BuildRequires:  debugedit
BuildRequires:  dwz

#Requires:       cppzmq
Requires:       yaml-cpp
Requires:       libstdc++

%description
Holoscan Proxy is a C++ application using cppzmq and yaml-cpp for messaging and config.

%prep
%autosetup -n %{name}-%{version}

%build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%{_prefix}
make

%install
rm -rf %{buildroot}
cd build 
make install DESTDIR=%{buildroot}

# Debug : check if binary is installed correctly
ls -l %{buildroot}/usr/bin/holoscan-proxy || echo "Binary not found in buildroot!"

%files
%{_bindir}/holoscan-proxy
##/usr/local/bin/holoscan-proxy
##/usr/bin/holoscan-proxy

%changelog
