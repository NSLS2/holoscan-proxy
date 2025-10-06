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
BuildRequires:  yaml-cpp-devel
BuildRequires:  redhat-rpm-config
BuildRequires:  debugedit
BuildRequires:  dwz
BuildRequires:  systemd

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

# Install systemd service file
install -D -m 0644 ../packaging/holoscan-proxy.service \
  %{buildroot}/usr/lib/systemd/system/holoscan-proxy.service

# Debug: check if binary and service file are installed
ls -l %{buildroot}/usr/bin/holoscan-proxy || echo "Binary not found in buildroot!"
ls -l %{buildroot}/usr/lib/systemd/system/holoscan-proxy.service || echo "Service file not found!"

%post
%systemd_post holoscan-proxy.service

%preun
%systemd_preun holoscan-proxy.service

%postun
%systemd_postun_with_restart holoscan-proxy.service

%files
%{_bindir}/holoscan-proxy
/usr/lib/systemd/system/holoscan-proxy.service

%changelog
