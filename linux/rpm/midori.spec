Name:           midori
Version:        staging
Release:        1%{?dist}
Summary:        Code name Midori Browser: A web browser Lightweight fast, secure free software & open source

# Files in src/lib/opensearch and src/lib/3rdparty are GPLv2+
# Files in src/lib/data/html/vantajs are MIT
# Files in src/plugins/MouseGestures/3rdparty are BSD (2 clause)

License:        GPLv3+, BSD and MIT
URL:            astian.org
Source0:        https://gitlab.astian.org/astian/midori/%{name}/-/archive/%{version}/%{name}-%{version}.tar.bz2

# handled by qt5-srpm-macros, which defines %%qt5_qtwebengine_arches
%{?qt5_qtwebengine_arches:ExclusiveArch: %{qt5_qtwebengine_arches}}

BuildRequires:  cmake
BuildRequires:  desktop-file-utils
BuildRequires:  extra-cmake-modules
BuildRequires:  gcc-c++
BuildRequires:  kf5-rpm-macros
BuildRequires:  libappstream-glib
BuildRequires:  xcb-util-devel
BuildRequires:  openssl-devel
BuildRequires:  qt5-linguist >= 5.14.2
BuildRequires:  qt5-qtbase-devel >= 5.14.2
BuildRequires:  qt5-qtwebengine-devel >= 5.14.2
BuildRequires:  qt5-qtx11extras-devel >= 5.14.2

# require the correct minimum versions of Qt, symbol versioning does not work
%{?_qt5_version:Requires: qt5-qtbase%{?_isa} >= %(echo %{_qt5_version} | cut -d. -f-2)}
%global qtwebengine_version %(pkg-config --modversion Qt5WebEngine 2>/dev/null || echo 5.9)
Requires:       qt5-qtwebengine%{?_isa} >= %(echo %{qtwebengine_version} | cut -d. -f-2)

# directory ownership
Requires:       hicolor-icon-theme

Obsoletes:      qupzilla < 2.3
Provides:       qupzilla = %{version}-%{release}

# forked version that uses D-Bus instead of lock files (see also #1551678)
Provides:       bundled(qtsingleapplication-qt5)

%global __provides_exclude_from ^%{_kf5_qtplugindir}/midori/.*$

%package kde
Summary: KDE Frameworks Integration plugin for %{name}
BuildRequires:  kf5-kwallet-devel
BuildRequires:  kf5-kio-devel
BuildRequires:  kf5-kcrash-devel
BuildRequires:  kf5-kcoreaddons-devel
BuildRequires:  kf5-purpose-devel
BuildRequires: make
Requires: %{name}%{?_isa} = %{version}-%{release}

Obsoletes:      qupzilla-kwallet < 2.3
Provides:       qupzilla-kwallet = %{version}-%{release}

Obsoletes:      falkon-kwallet < 3.1
Provides:       midori-kwallet = %{version}-%{release}

%description kde
Plugin for Blue Hawk adding support for:
- storing passwords securely in KWallet,
- additional URL protocols using KIO (e.g., man:, info:, gopher:, etc.),
- a "Share page" menu using the KDE Purpose Framework,
- intercepting crashes with KCrash, bringing up the DrKonqi crash handler.

%description
Blue Hawk is a Qt based web browser which uses the QtWebEngine
fork of the Chromium open source project.
Originally forked from Falkon, Blue Hawk is experimental
and attempts to improve or fix existing features found
in Falkon and add new and improved features; mostly to
improve user experience.
- Ad/nuisance block support built in and enabled by default,
- sensible default settings,
- theming support,
- improved speed dial, and
- additional preference options and plugins.

%prep
%setup -q

# FIXME: The following note from the creator of this spec
# file indicates that translations are not being
# packaged correctly. Fix it.
# Side note: midori_helloqml & midori_hellopython will have
# been built because some options being passed
# to cmake were missing

# delete falkon_hellopython and other Python plugins' and falkon_helloqml
# translations, we do not package hellopython etc. yet
# note: if you remove this, you also have to add BuildRequires: kf5-ki18n-devel
rm -rf po

%build
mkdir %{_target_platform}
pushd %{_target_platform}
# Add -DNO_PLUGINS:BOOL="1" here for faster builds without any plugins
# Testing -DCMAKE_INSTALL_PREFIX=%{buildroot}
%cmake_kf5 -DCMAKE_CONFIGURATION_TYPES:STRING="Release" -DBUILD_TESTING:BOOL="0" -DBUILD_SHARED_LIBS:BOOL="1" ..
popd

pushd %{_target_platform}
make %{?_smp_mflags} -C %{_target_platform}
popd

%install
pushd %{_target_platform}
make install DESTDIR=%{buildroot} -C %{_target_platform}
popd

# translations (find_lang_kf5 does not support --all-name, so adapt it)
find %{buildroot}/%{_datadir}/locale/ -name "*.qm" -type f | sed 's:%{buildroot}/::;s:%{_datadir}/locale/\([a-zA-Z_\@]*\)/LC_MESSAGES/\([^/]*\.qm\):%lang(\1) %{_datadir}/locale/\1/LC_MESSAGES/\2:' >%{name}.lang

desktop-file-install \
    --add-mime-type="x-scheme-handler/http;x-scheme-handler/https;" \
    --dir=%{buildroot}%{_datadir}/applications \
    %{buildroot}/%{_datadir}/applications/org.astian.midori.desktop

# remove useless plugins - these don't get built if cmake options are passed
rm -fv %{buildroot}%{_kf5_qtplugindir}/%{name}/TestPlugin.so
rm -rfv %{buildroot}%{_kf5_qtplugindir}/%{name}/qml/helloqml

%check
# It's unlikely the below line will work
# appstream-util validate-relax --nonet %{buildroot}%{_kf5_metainfodir}/org.astian.midori.appdata.xml

%ldconfig_scriptlets

# %files -f %{name}.lang
%files
%doc README.md
%license COPYING
%{_kf5_bindir}/midori
%{_kf5_libdir}/libMidoriPrivate.so.*
%dir %{_kf5_qtplugindir}/midori/
%{_kf5_qtplugindir}/midori/AutoScroll.so
%{_kf5_qtplugindir}/midori/DisableJS.so
%{_kf5_qtplugindir}/midori/FlashCookieManager.so
%{_kf5_qtplugindir}/midori/GreaseMonkey.so
%{_kf5_qtplugindir}/midori/MouseGestures.so
%{_kf5_qtplugindir}/midori/PIM.so
%{_kf5_qtplugindir}/midori/StatusBarIcons.so
%{_kf5_qtplugindir}/midori/TabManager.so
%{_kf5_qtplugindir}/midori/VerticalTabs.so
# %dir %{_kf5_qtplugindir}/midori/qml/
%{_kf5_metainfodir}/org.astian.midori.appdata.xml
%{_kf5_datadir}/applications/org.astian.midori.desktop
%{_kf5_datadir}/pixmaps/midori.png
%{_kf5_datadir}/icons/hicolor/*/*/*
%{_kf5_datadir}/midori/

%files kde
%{_kf5_qtplugindir}/midori/KDEFrameworksIntegration.so


%changelog
* Mon Feb 1 2021 TW3 <2378845-TW3@users.noreply.gitlab.com> - 3.2.4-1
- midori package spec created using https://src.fedoraproject.org/rpms/falkon/blob/master/f/falkon.spec

* Mon Apr 30 2012 Christoph Wickert <cwickert@fedoraproject.org> - 1.2.0-1
- Initial package
