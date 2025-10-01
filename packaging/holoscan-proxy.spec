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
BuildRequires:  redhat-rpm-config
BuildRequires:  debugedit
BuildRequires:  dwz

#Requires:       cppzmq
Requires:       yaml-cpp
Requires:       libstdc++

#%debug_package

%description
Holoscan Proxy is a C++ application using cppzmq and yaml-cpp for messaging and config.

#%prep
#%autosetup

#%build
#mkdir -p build
#cd build
#cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%{_prefix}
#make

#%install
#rm -rf %{buildroot}
#cd build 
#make install DESTDIR=%{buildroot}

## Debug : check if binary is installed correctly
#ls -l %{buildroot}/usr/bin/holoscan-proxy || echo "Binary not found in buildroot!"

#%files
#%{_bindir}/holoscan-proxy
##/usr/local/bin/holoscan-proxy
##/usr/bin/holoscan-proxy

#%changelog

%prep
%autosetup -n %{name}-%{version}

%build
# Use EL8 macros so -g and --build-id are injected (RelWithDebInfo keeps symbols)
%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%{_prefix}
%cmake_build

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%files
%license LICENSE*
%doc README* CHANGELOG* docs/*
%{_bindir}/holoscan-proxy

%changelog
