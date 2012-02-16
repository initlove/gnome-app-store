#
# spec file for package gnome-app-store
#
# Copyright (c) 2011 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild


Name:           gnome-app-store
Summary:        App Store client for GNOME
Version:        0.1
Release:        1
License:        GPLv2+
Group:          System/GUI/GNOME
Url:            https://github.com/initlove/gnome-app-store
Source:         %{name}-%{version}.tar.bz2
BuildRequires:  glib2-devel >= 2.30.0
BuildRequires:  gobject-introspection-devel
BuildRequires:  dbus-1-glib-devel
BuildRequires:  libxml2-devel
BuildRequires:  libsoup-devel
BuildRequires:  librest-devel
BuildRequires:  clutter-devel
Requires:       %{name}-lang = %{version}
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
gnome app store is a app store client for GNOME. It is based on OCS
standard. The main interface is based on clutter.
Librest is used to POST/GET with the OCS server.
liboasyncworker is used to deal with async tasks.

Authors:
------
	David Liang <dliang@novell.com>

%lang_package
%prep
%setup -q

%build
libtoolize -if
autoreconf -if
./configure

make %{?jobs:-j%jobs}

%install
%makeinstall

%find_lang %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%dir %{_datadir}/pixmaps/
%dir %{_datadir}/pixmaps/gnome-app-store/
%dir %{_datadir}/gnome-app-store/
%dir %{_datadir}/gnome-app-store/ui/
%{_datadir}/pixmaps/gnome-app-store/*
%{_datadir}/gnome-app-store/ui/*
%{_bindir}/gnome-app-store

%files lang -f %{name}.lang

%changelog
