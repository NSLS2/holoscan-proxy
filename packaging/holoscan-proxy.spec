Name:           holoscan-proxy
Version:        0.0.0
Release:        1%{?dist}
Summary:        Holoscan Proxy - C++ app using ZeroMQ and YAML

License:        BSD-3
URL:            https://github.com/NSLS2/holoscan-proxy.git
Source0:        holoscan-proxy-%{version}.tar.gz

# Disable debug packages
# %global debug_package %{nil}

BuildRequires:  cmake
BuildRequires:  gcc-c++
BuildRequires:  make
#BuildRequires:  cppzmq-devel
BuildRequires:  yaml-cpp-devel

#Requires:       cppzmq
Requires:       yaml-cpp
Requires:       libstdc++

%description
Holoscan Proxy is a C++ application using cppzmq and yaml-cpp for messaging and config.

%prep
%setup -q

%build
mkdir -p build
cd build
export CC=/opt/rh/gcc-toolset-12/root/usr/bin/gcc
export CXX=/opt/rh/gcc-toolset-12/root/usr/bin/g++
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo 
make

%install
#rm -rf %{buildroot}
#mkdir -p %{buildroot}/usr/local/bin
#cp build/holoscan-proxy %{buildroot}/usr/local/bin/
rm -rf %{buildroot}
cd build 
make install DESTDIR=%{builtroot}

%files
#/usr/local/bin/holoscan-proxy
/usr/bin/holoscan-proxy

%changelog
