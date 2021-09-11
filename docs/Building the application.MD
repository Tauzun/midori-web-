# Building Midori Web Browser

## **Build documentation**
### Building the application from source code.

On Windows 10 using Visual Studio 2019.

### Download the source code

Midori Web Browser source code is available via [Gitlab](https://tw3.gitlab.io/b).
The easiest way to obtain a copy is to use git.

With a working Internet connection and git installed, open up a PowerShell window using the start menu, change directory to the root of the drive (eg: `cd \`) and then do the following:

`git clone https://gitlab.com/TW3/b`

A copy of the source code will be copied into a folder named **b**.

### Setting up the build environment

Before you attempt to build the application from a copy of the source code, you need to ensure that the following software is installed.

The build script used to build the application checks lots of requirements and should prompt you to install any of the missing requirements the first time you try to build.

#### Microsoft Visual Studio

Microsoft's compiler, build environment and IDE.

- [Microsoft Visual Studio](https://visualstudio.microsoft.com)

_Microsoft Visual Studio 2019 is currently the only recommended version._

You can use the file **vsconfig/bhawk.vsconfig** and the Visual Studio installer to import and install the workload required to build the project. Please see https://docs.microsoft.com/en-us/visualstudio/install/import-export-installation-configurations?view=vs-2019#import-a-configuration for more details.

#### Git

Source code management for software projects.

- [Git for Windows](https://git-scm.com) (current version)

_Download the latest version from the git-scm.com website or install the Git for Windows component via Microsoft Visual Studio 2019's installer_

#### Cmake

Scripting language and build system specifically targeted towards C++ software projects.

- [Cmake for Windows](https://cmake.org) (_latest version or version 3.17 minimum_)

_cmake 3.17 or newer is required because the application build process uses a new cmake feature called unity builds_

### Choosing how to build

Before the first build is started, you need to decide if you want to build the application the long way (which will allow you to use and to test html video to it's full capabilities, provided that you have the appropriate licenses.) Or instead, use precompiled binaries downloaded from the Internet (which do not support some html5 video codecs.)

- Using pre-compiled binaries is faster to set up than compiling everything using vcpkg. The required hard disk space is approximately 4GB. Time to set up takes approximately 30 minutes but no more than an hour.
- Compiling everything using vcpkg takes a very long time (5hrs+) but only the very first time the build is started. The required hard disk space is a minimum to build everything is 35GB.

_Please ensure that you understand the time requirement to set up each method and that your disk has enough free space before attempting to build. The build script will simply not allow you to build if you do not have enough free space available._

The long setup method is the default method and the recommended way to build the application on Windows. Running the very first build at night time when the machine can just be left to compile everything is highly recommended. The reason the software takes so long is QtWebEngine. QtWebEngine is a fork of the Chromium open source project. Chromium is a huge undertaking and it's source code is vast.

### Starting the first build

Once the prerequisites of the build environment have been installed, the file **windows-build.ps1** can be used to build the project.

**The very first build will take a while to complete**

If you want to build the long way:

- Right click on the file **windows-build.ps1** and click on **Run with PowerShell**

If you want to build the short way:

- Open a PowerShell window, move into the directory containing the source code and do: `./windows-build.ps1 -codecs 0`

If at any point the build fails, please review the console output, correct the error and re-run the script.

_Windows 10 (64 bit) is currently the only supported operating system which can be used as a build platform._

### Once the build succeeds

Once a build has been successfully completed, the folder **Midori Web Browser** contains the resulting application and associated files.
The application is portable. This means that the resulting **Midori Web Browser** folder can be copied and run from anywhere you like.

By default, the application will automatically run upon successful build completion. If you do not want the application to start in this way, run the build script using the `-buildandrun` switch and set it to **0**.

To start another build, maybe after you have made some changes to the source code, re-run the build script **windows-build.ps1** once more. Stale build directories are purged, so you don't need to clean up after any builds.

After the first build, builds (when not building the plugins) take approximately 2 minutes on a fast machine and do not require an Internet connection to complete.

### Command line switches

The following are all of the command line switches that the file **windows-build.ps1** will accept:

- `-codecs 1` **Setting this to 1 will build QtWebengine using vcpkg.**
- `-codecs 0` **Setting this to 0 will use precompiled QtWebengine downloaded from the Qt project.**

- `-buildandrun` 1 **Setting this to 1 will run the application once a successful build has been completed.**
- `-buildandrun 0` **Setting this to 0 will prevent the application running once a successful build has been completed.**

- `-plugins 1` **Setting this to 1 will ensure that all of the default plugins are built along with the application.**
- `-plugins 0` **Setting this to 0 will ensure that the plugins are not built, this speeds up build times.**

- `-debugbuild 1` **Setting this to 1 will ensure that all of the possible debug options are applied. Useful for testing.**
- `-debugbuild 0` **Setting this to 0 will ensure that the debug options are not passed to the compiler and a release binary is built.**

- `-codeanalysis 1` **Setting this to 1 will enable the compiler's ability to analyse the code it is compiling. Very verbose output will appear in the console.**
- `-codeanalysis 0` **Setting this to 0 will ensure that code analysis is not run.**

- `-labbuild 1` **Setting this to 1 will enable specific options only applicable for Gitlab CI\CD builds.** _Look at the file yml\windows-build.yml to see how this option is used. This option ensures that other incompatible options are not set and that the plugins are built too._
- `-labbuild 0` **Setting this to 0 will disable specific options only applicable for Gitlab CI\CD builds.**

The defaults are:

- `-codecs 1`
- `-buildandrun 1`
- `-plugins 0`
- `-debugbuild 0`
- `-codeanalysis 0`
- `-labbuild 0`

Changing how the application is built is possible.
For example running:

`./windows-build.ps1 -codecs 0 -buildandrun 0 -plugins 1 -codeanalysis 1`

Will try to build the application using Qt's compiled binaries, will not run the application once the build completes, the plugins will be built and the build output should contain code analysis information.

### Sending patches

If this software works for you and you think that you can change it for the better; please consider offering any improvements you have made via a pull request on [Gitlab](https://about.gitlab.com/).

Thanks!
