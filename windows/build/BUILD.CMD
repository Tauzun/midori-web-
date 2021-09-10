
:: Midori Browser Build Script

:: This build script requires:
:: MS Visual Studio 2017 or 2019 (and appropriate libraries + tools)
:: Cmake
:: Git
:: A working Internet connection to setup the first build

:: This build method has the extra requirements of:
:: C++ ATL for V142 x86 & x64
:: C++ MFC for V142 x86 & x64
:: C++ ATL for V142 x86 & x64 (and spectre mitigations)
:: C++ MFC for V142 x86 & x64 (and spectre mitigations)
:: they can be installed via the Visual Studio Installer.

:: The extra requirements add accessibility support to QtWebEngine.

:: https://docs.microsoft.com/en-us/cpp/windows/determining-which-dlls-to-redistribute?view=vs-2019

:: CURRENT WORKING STATUS: Working and tested using Visual Studio 2019

:: This script removes the requirement to install the Qt
:: for Windows installer. It builds all the required
:: parts of Qt including QtWebEngine from source.

:: You will need 20Gb+ free space available (minimum) to build using this script
:: A working Internet connection is required so that source code is downloaded

:: The first time this script is run, the build environment
:: will take several hours to set up.
:: Once the environment has been set up, subsequent builds
:: will complete significantly faster.

:: Intrusive changes to this script will only happen if a major
:: version bump of a dependency occurs.
:: In that case, a refresh of the build environment can be triggered
:: by deleting the vcpkg directory and then successfully completing a new build.
::

:: Set NO_PLUGINS:BOOL="0" to build the plugins
:: If you want to use -DBUILD_TESTING:BOOL="1" then you must also set NO_PLUGINS:BOOL="0"

SETLOCAL

:: VS2019 - Variables
::
SET TOOLSET=v142
SET MSVSDIR="D:\Programs\MSVS2019"
SET GENERATOR=-G "Visual Studio 16 2019" -A x64
::
:: End - VS2019 - Variables

:: Important Variables
::
SET VCRUN=%MSVSDIR%"\VC\Redist\MSVC\14.28.29325\spectre\x64\Microsoft.VC142.CRT\vcruntime140_1.dll"
SET SRCROOTDIR=D:\bluehawk\midori
::
:: End - Important Variables

IF exist %VCRUN% ( ECHO vcruntime140_1 found. ) ELSE ( ECHO vcruntime140_1.dll not found. Please ensure the correct path is set inside the variable VCRUN. && EXIT /B )

IF exist %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ( CALL %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ) ELSE ( ECHO vcvars64.bat not found. Please ensure the correct path is set inside the variable MSVSDIR. && GOTO :buildend )

:: Internal Variables
::
SET BHAWK_VER=3.2.3
SET ANALYSIS=false
SET CONFIGURATION=release
SET VCPKGDIR=%SRCROOTDIR%\vcpkg
SET QTDIR=%VCPKGDIR%\installed\x64-windows
SET INSTDIR=%SRCROOTDIR%\windeploy
SET INTL=%VCPKGDIR%\installed\x64-windows
SET OPENSSLDIR=%VCPKGDIR%\packages\openssl-windows_x64-windows

SET SCRIPTDIR=%CD%
::
:: End - Internal Variables

IF exist %SRCROOTDIR%\vcpkg\ (  GOTO :cleanup )

:setupvcpkg

MKDIR %VCPKGDIR% && CD %VCPKGDIR% && git init
git fetch https://github.com/Microsoft/vcpkg master
:: Last update to vcpkg HEAD: 30/04/2020
git merge 86f5397f76fa6b8141ea828640e94050b9c7b8e1
echo.set(VCPKG_BUILD_TYPE %CONFIGURATION%)>> %VCPKGDIR%\triplets\x64-windows.cmake
echo.set(VCPKG_BUILD_TYPE %CONFIGURATION%)>> %VCPKGDIR%\triplets\x64-windows-static.cmake
:: Fix the use of /Z7 when creating release binaries
powershell -command " & {(Get-Content %VCPKGDIR%\scripts\toolchains\windows.cmake).replace('/DNDEBUG /Z7', '/DNDEBUG /Qspectre /Qpar /Ot /MP /arch:AVX') | Set-Content %VCPKGDIR%\scripts\toolchains\windows.cmake}"
powershell -command " & {(Get-Content %VCPKGDIR%\scripts\cmake\vcpkg_configure_meson.cmake).replace('/DNDEBUG /Z7', '/DNDEBUG /Qspectre /Qpar /Ot /MP /arch:AVX') | Set-Content %VCPKGDIR%\scripts\cmake\vcpkg_configure_meson.cmake}"

XCOPY /Y /E /I %SRCROOTDIR%\ports\qt5-webengine %VCPKGDIR%\ports\qt5-webengine

CALL .\bootstrap-vcpkg.bat -disableMetrics
vcpkg install angle:x64-windows qt5-base[latest]:x64-windows qt5-winextras:x64-windows qt5-translations:x64-windows qt5-tools:x64-windows qt5-webengine:x64-windows ecm:x64-windows openssl-windows:x64-windows gettext:x64-windows
::

:cleanup

IF exist %INSTDIR%\ ( RMDIR /Q /S "%INSTDIR%\" ) ELSE ( ECHO stale windeploy directory not found )
IF exist %SRCROOTDIR%\build\ ( RMDIR /Q /S "%SRCROOTDIR%\build\" ) ELSE ( ECHO stale build directory not found )
IF exist %SRCROOTDIR%\"Blue Hawk"\ ( RMDIR /Q /S %SRCROOTDIR%\"Blue Hawk"\ ) ELSE ( ECHO stale Blue Hawk directory not found )

:build

CD %SRCROOTDIR%
COPY CMakeLists.txt CMakeLists.bak
powershell -Command "(Get-Content 'CMakeLists.txt') | Where-Object {$_ -notmatch 'Midori VERSION'} | Set-Content  'CMakeLists.txt'"
powershell -Command "(Get-Content 'CMakeLists.txt') | Where-Object {$_ -notmatch 'cmake_minimum_required'} | Set-Content  'CMakeLists.txt'"
powershell -Command "@('project(Midori VERSION %BHAWK_VER%)') + (Get-Content 'CMakeLists.txt') | set-content 'CMakeLists.txt'"
powershell -Command "@('cmake_minimum_required(VERSION 3.1)') + (Get-Content 'CMakeLists.txt') | set-content 'CMakeLists.txt'"
MKDIR build
CD build

cmake .. %GENERATOR% ^
-Duse_mp:BOOL="1" ^
-Duse_o2:BOOL="1" ^
-Duse_ot:BOOL="1" ^
-Duse_gy:BOOL="1" ^
-Duse_oi:BOOL="1" ^
-Duse_avx2:BOOL="1" ^
-Duse_qpar:BOOL="1" ^
-Duse_qspectre:BOOL="1" ^
-Duse_control_flow_guard:BOOL="1" ^
-DCMAKE_CONFIGURATION_TYPES:STRING="Release" ^
-DBUILD_TESTING:BOOL="0" ^
-DNO_PLUGINS:BOOL="1" ^
-DBUILD_SHARED_LIBS:BOOL="1" ^
-DQMAKE_EXECUTABLE:FILEPATH="%QTDIR%\tools\qt5\bin\qmake.exe" ^
-DECM_DIR:PATH="%VCPKGDIR%\packages\ecm_x64-windows\share\ECM\cmake" ^
-DCMAKE_INCLUDE_PATH:PATH="%QTDIR%\include" ^
-DQt5_DIR:PATH="%QTDIR%\share\cmake\Qt5" ^
-Dunofficial-gettext_DIR:PATH="%INTL%\share\unofficial-gettext" ^
-Dunofficial-iconv_DIR:PATH="%INTL%\share\unofficial-iconv" ^
-DOPENSSL_ROOT_DIR:PATH="%VCPKGDIR%\packages\openssl-windows_x64-windows"

IF exist %SRCROOTDIR%\build\Midori.sln (

msbuild /m ".\Midori.sln" /p:CharacterSet=Unicode /p:configuration=%CONFIGURATION% /p:platform=x64 /p:PlatformToolset=%TOOLSET% /p:RunCodeAnalysis=%ANALYSIS% /p:PreferredToolArchitecture=x64 /p:UseEnv=true

) ELSE ( GOTO :buildend )

CD %SRCROOTDIR%

IF exist %SRCROOTDIR%\userscripts\ ( echo User script folder exists. ) ELSE (
	MKDIR %SRCROOTDIR%\userscripts
)
::IF exist %SRCROOTDIR%\userscripts\DarkPages.js ( echo User script DarkPages.js exists. ) ELSE (
::		powershell -command "Invoke-WebRequest -Uri 'https://glcdn.githack.com/TW3/darkpages/raw/master/src/DarkPages.js' -OutFile %SRCROOTDIR%\userscripts\DarkPages.js"
::)

MKDIR windeploy
:: windeployqt https://doc.qt.io/qt-5/windows-deployment.html https://doc.qt.io/Qt-5/qtwebengine-deploying.html
COPY %SRCROOTDIR%\build\bin\Release\MidoriPrivate.dll %INSTDIR%
COPY %SRCROOTDIR%\build\bin\Release\midori.exe %INSTDIR%
COPY %SRCROOTDIR%\COPYING %INSTDIR%
XCOPY /Y /E /I %SRCROOTDIR%\themes %INSTDIR%\themes
XCOPY /Y /E /I %SRCROOTDIR%\build\bin\plugins\Release %INSTDIR%\plug-ins
XCOPY /Y /E /I %SRCROOTDIR%\userscripts %INSTDIR%\userscripts
REN %INSTDIR%\COPYING LICENSE.TXT
ECHO [Config] > %INSTDIR%\midori.conf
ECHO Portable=true >> %INSTDIR%\midori.conf
:: Something is preventing the plugins to be found so qt.conf is a temp fix
ECHO [Paths] > %INSTDIR%\qt.conf
ECHO Plugins = "." >> %INSTDIR%\qt.conf

:: --------------------------------------------------------------------------------

COPY %VCPKGDIR%\packages\zlib_x64-windows\bin\zlib1.dll %INSTDIR%
COPY %VCPKGDIR%\packages\pcre2_x64-windows\bin\pcre2-16.dll %INSTDIR%
COPY %VCPKGDIR%\packages\libpng_x64-windows\bin\libpng16.dll %INSTDIR%
COPY %VCPKGDIR%\packages\harfbuzz_x64-windows\bin\harfbuzz.dll %INSTDIR%
COPY %VCPKGDIR%\packages\freetype_x64-windows\bin\freetype.dll %INSTDIR%
COPY %VCPKGDIR%\packages\bzip2_x64-windows\bin\bz2.dll %INSTDIR%
COPY %VCPKGDIR%\packages\libiconv_x64-windows\bin\libiconv.dll %INSTDIR%
COPY %VCPKGDIR%\packages\libiconv_x64-windows\bin\libcharset.dll %INSTDIR%
COPY %INTL%\bin\libintl.dll %INSTDIR%
COPY %OPENSSLDIR%\bin\libcrypto-1_1-x64.dll %INSTDIR%
COPY %OPENSSLDIR%\bin\libssl-1_1-x64.dll %INSTDIR%
:: widevinecdm.dll
:: widevinecdm.dll.sig

:: --------------------------------------------------------------------------------

COPY %VCRUN% %INSTDIR%

SET PATH=%PATH%;%QTDIR%\bin;%QTDIR%\tools\qt5\bin

%QTDIR%\tools\qt5-tools\bin\windeployqt ^
-sql ^
-svg ^
-winextras ^
-webengine ^
-printsupport ^
-quickwidgets ^
-webenginecore ^
-webenginewidgets ^
--libdir %INSTDIR% ^
--plugindir %INSTDIR% ^
--release ^
--no-opengl-sw ^
--no-compiler-runtime ^
%INSTDIR%

:buildend

DEL %SRCROOTDIR%\CMakeLists.txt
REN %SRCROOTDIR%\CMakeLists.bak CMakeLists.txt
CD %SCRIPTDIR%

IF exist "%INSTDIR%\MidoriPrivate.dll" (
	IF exist "%INSTDIR%\QtWebEngineProcess.exe" (
		CALL %SRCROOTDIR%\windows\build\SPELL.CMD
		::RENAME %INSTDIR%\midori.exe "Blue Hawk.exe"
		RENAME %INSTDIR% "Blue Hawk"
		ECHO -
		ECHO - [32mBuild completed successfully![0m
	) ELSE (
		ECHO -
		ECHO - [31mBuild failure[0m - Please review the console output
	)
) ELSE (
	ECHO -
	ECHO - [31mBuild failure[0m - Please review the console output
)
