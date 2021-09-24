
# How to prep and build the rpm for this project (tested using Fedora 33.)

# Env prep hints from: https://rpm-packaging-guide.github.io/#prerequisites

# First install the rpm build package deps
su -c 'dnf install gcc rpm-build rpm-devel rpmlint make python bash coreutils diffutils patch rpmdevtools'

# Move into your home directory (because you need write access)
cd $HOME

# Use the rpm build tree creation script to create a build structure in your home directory
rpmdev-setuptree

# Move into the SOURCES directory
cd $HOME/rpmbuild/SOURCES

# Download the midori source archive
wget https://gitlab.com/TW3/b/-/archive/staging/midori-staging.tar.bz2

# Copy the file midori.spec (from the linux/rpm directory inside the midori source tree) into the directory $HOME/rpmbuild/SPECS

# Move into the SPECS dircetory
cd $HOME/rpmbuild/SPECS

# Build the rpm - be sure that you have all of the build deps installed. Check the file Dockerfile in the root of the midori source archive for the dnf command
rpmbuild -ba midori.spec

# Install the rpms once they have been built (hawk-kde is the kde integration plugin and is not required)
su -c 'dnf install /home/harry/rpmbuild/RPMS/x86_64/midori-staging-1.fc33.x86_64.rpm /home/harry/rpmbuild/RPMS/x86_64/midori-kde-staging-1.fc33.x86_64.rpm'

# To remove
su -c 'dnf erase midori midori-kde
