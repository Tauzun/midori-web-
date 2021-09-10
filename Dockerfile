
# Define base image
FROM fedora:33
# Install dependencies required to build the application
# https://src.fedoraproject.org/rpms/falkon/blob/master/f/falkon.spec
# https://apps.fedoraproject.org/packages/
RUN dnf -q -y --refresh --noautoremove upgrade-minimal && dnf -q -y --nodocs --noautoremove --skip-broken install glibc-devel make extra-cmake-modules desktop-file-utils xcb-util-devel qt5*-devel qt5-linguist hicolor-icon-theme cmake gcc-c++ openssl-devel intltool gettext-libs gettext-common-devel pkgconfig libgnome-keyring-devel kf5-kwallet-devel kf5-kio-devel kf5-kcrash-devel kf5-kcoreaddons-devel kf5-purpose-devel kf5-karchive-devel
# Or with the optional python plugin support (currently broken due to: fatal: 'stddef.h' file not found)
#RUN dnf -q -y --refresh --noautoremove upgrade-minimal && dnf -q -y --nodocs --noautoremove --skip-broken install glibc-devel make extra-cmake-modules desktop-file-utils xcb-util-devel qt5*-devel qt5-linguist hicolor-icon-theme cmake gcc-c++ openssl-devel intltool gettext-libs gettext-common-devel pkgconfig libgnome-keyring-devel kf5-kwallet-devel kf5-kio-devel kf5-kcrash-devel kf5-kcoreaddons-devel kf5-purpose-devel kf5-karchive-devel python3-pyside2-devel python3-shiboken2-devel shiboken2 python-devel
# https://fedoralovespython.org/ ? But need to install the python-devel package..
