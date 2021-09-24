
# Midori Browser

Midori Browser is a Web browser cross platform (64 bit.) It uses the [QtWebEngine](https://wiki.qt.io/QtWebEngine) rendering component and is a fork of the [Falkon] Web browser but with much changes.

![screenshot](/web/content/assets/images/screens/cap1.png)

## Downloads

Midori Browser releases are available from the [homepage](https://astian.org/midori-browser).

## Building

<!--- [![pipeline status](https://gitlab.com/TW3/b/badges/staging/pipeline.svg)](https://gitlab.com/midori-web/midori-desktop/-/commits/dev) --->

In quick summary, the build dependencies are:

- [Cmake](https://cmake.org) (_version 3.17 minimum_)
- [Microsoft Visual Studio](https://visualstudio.microsoft.com) (_version 2019_) and the following individual components:
  - **MsBuild**
  - **MSVC** (_v142 VS2019 C++ **:** x64/x86 build tools v14.23 or newer_)
  - **MSVC** (_v142 VS2019 C++ **:** x64/x86 Spectre-mitigated libs v14.23 or newer_)
  - **C++ Core features**
  - **Windows 10 SDK** (_latest - current 10.0.18362.0_)
  - **Windows Universal C runtime**
  - **C++ ATL** for latest V142 build tools (x86 & x64)
  - **C++ ATL** for latest V142 build tools with spectre mitigations (x86 & x64)
  - **C++ MFC** for latest V142 build tools (x86 & x64)
  - **C++ MFC** for latest V142 build tools with spectre mitigations (x86 & x64)
  - **[Git](https://git-scm.com) for Windows** (_required only if git is not already installed_)

Please read the build instructions inside the docs folder to understand the steps required to build the project.

## Contributing

Please send any and all patches as merge requests via [Gitlab](https://about.gitlab.com/).
All forms of help are very welcome! Please don't hesitate to get in touch and join in.

#### Reporting bugs

<!--- Please report any issues you find on the [issue tracker](https://gitlab.com/midori-web/midori-desktop/issues). Thanks! -->

Unfortunately, the issue tracker is only being used by project members currently (to prevent abuse.)

You can check to see if a bug you have found has also been observed in Falkon by checking the [Falkon bug tracker](https://bugs.kde.org/describecomponents.cgi?product=Falkon).

#### Contact

Please conduct all project related communications using [Gitlab](https://gitlab.com). We would **love** to hear from you!

![logo](/logo.png)
