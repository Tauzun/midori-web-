
:: Gitlab CI Windows Native build

:: Pass -DNO_PLUGINS:BOOL="1" to cmake if you do not want to build the plugins

SETLOCAL

:: Important Variables
::
SET SRCROOTDIR=C:\GitLab-Runner\builds\midori-web\midori-desktop
::
:: End - Important Variables

:: VS2019 - Variables
::
SET TOOLSET=v142
SET MSVSDIR="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools"
SET GENERATOR=-G "Visual Studio 16 2019" -A x64
::
:: End - VS2019 - Variables

:: Internal Variables
::
SET MIDORI_VER=10.0.1
SET ANALYSIS=false
SET CONFIGURATION=release
SET VCPKGDIR=%SRCROOTDIR%\vcpkg
SET INSTDIR=%SRCROOTDIR%\windeploy
SET INTL=%VCPKGDIR%\installed\x64-windows
SET OPENSSLDIR=%SRCROOTDIR%\qt\Tools\OpenSSL\Win_x64
:: The correct folder directory (eg 14.26.28801) can be found via 'DIR %MSVSDIR%\VC\Tools\MSVC\'
SET VCRUN=%MSVSDIR%\VC\Tools\MSVC\14.26.28801\bin\Hostx64\x64\vcruntime140_1.dll
:: Use a mirror here instead of https://download.qt.io/online/qtsdkrepository/windows_x86/desktop
SET BASEURLQT=https://ftp1.nluug.nl/languages/qt/online/qtsdkrepository/windows_x86/desktop
SET QTURL=%BASEURLQT%/qt5_5152
SET EXTRASTRING=qt.qt5.5152.win64_msvc2019_64/5.15.2-0-202011130602
SET QTDIR=%SRCROOTDIR%\qt\5.15.2\msvc2019_64
SET CMAKEBIN=%SRCROOTDIR%\cmake-3.19.2-win64-x64\bin\cmake.exe

::
:: End - Internal Variables

IF exist %VCRUN% ( ECHO vcruntime140_1 found. ) ELSE ( ECHO vcruntime140_1.dll not found. Please ensure the correct path is set inside the variable VCRUN. && DIR %MSVSDIR%\VC\Tools\MSVC\ && powershell -command "throw 'vcruntime140_1.dll not found.'" && GOTO :buildend )

IF exist %SRCROOTDIR%\vcpkg\ (  GOTO :fetchqt )

:setupvcpkg

MKDIR %VCPKGDIR% && CD %VCPKGDIR% && git init
git fetch https://github.com/Microsoft/vcpkg master
:: Last update to vcpkg HEAD: 30/04/2020
git merge 86f5397f76fa6b8141ea828640e94050b9c7b8e1
echo.set(VCPKG_BUILD_TYPE %CONFIGURATION%)>> %VCPKGDIR%\triplets\x64-windows.cmake
echo.set(VCPKG_BUILD_TYPE %CONFIGURATION%)>> %VCPKGDIR%\triplets\x64-windows-static.cmake
:: Fix the use of /Z7 when creating release binaries
powershell -command " & {(Get-Content %VCPKGDIR%\scripts\toolchains\windows.cmake).replace('/DNDEBUG /Z7', '/DNDEBUG /Qspectre /Qpar /Ot /MP /std:c++14 /arch:AVX') | Set-Content %VCPKGDIR%\scripts\toolchains\windows.cmake}"
powershell -command " & {(Get-Content %VCPKGDIR%\scripts\cmake\vcpkg_configure_meson.cmake).replace('/DNDEBUG /Z7', '/DNDEBUG /Qspectre /Qpar /Ot /MP /std:c++14 /arch:AVX') | Set-Content %VCPKGDIR%\scripts\cmake\vcpkg_configure_meson.cmake}"
::
CALL .\bootstrap-vcpkg.bat -disableMetrics
vcpkg install ecm:x64-windows gettext:x64-windows
::

:fetchqt

IF exist %SRCROOTDIR%\qt\ (  GOTO :begin )

powershell -command "Invoke-WebRequest -Uri https://www.7-zip.org/a/7za920.zip -OutFile 7za920.zip"
powershell -command "Expand-Archive 7za920.zip -DestinationPath C:\GitLab-Runner\builds\midori-web\midori-desktop\7z\ -Force"
powershell -command "Invoke-WebRequest -Uri https://github.com/Kitware/CMake/releases/download/v3.19.2/cmake-3.19.2-win64-x64.zip -OutFile C:\cmake.zip"
%SRCROOTDIR%\7z\7za x C:\cmake.zip -aoa -o%SRCROOTDIR%\

powershell -command "Invoke-WebRequest -Uri %QTURL%/qt.qt5.5152.qtwebengine.win64_msvc2019_64/5.15.2-0-202011130602qtwebengine-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtwebengine.7z"
%SRCROOTDIR%\7z\7za x C:\qtwebengine.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %BASEURLQT%/tools_openssl_x64/qt.tools.openssl.win_x64/1.1.1-4openssl_1.1.1d_prebuild_x64.7z -OutFile C:\openssl.7z"
%SRCROOTDIR%\7z\7za x C:\openssl.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtactiveqt-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtactiveqt.7z"
%SRCROOTDIR%\7z\7za x C:\qtactiveqt.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtbase-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtbase.7z"
%SRCROOTDIR%\7z\7za x C:\qtbase.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtdeclarative-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtdeclarative.7z"
%SRCROOTDIR%\7z\7za x C:\qtdeclarative.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtgraphicaleffects-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtgraphicaleffects.7z"
%SRCROOTDIR%\7z\7za x C:\qtgraphicaleffects.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtimageformats-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtimageformats.7z"
%SRCROOTDIR%\7z\7za x C:\qtimageformats.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtlocation-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtlocation.7z"
%SRCROOTDIR%\7z\7za x C:\qtlocation.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtmultimedia-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtmultimedia.7z"
%SRCROOTDIR%\7z\7za x C:\qtmultimedia.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtquickcontrols-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtquickcontrols.7z"
%SRCROOTDIR%\7z\7za x C:\qtquickcontrols.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtquickcontrols2-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtquickcontrols2.7z"
%SRCROOTDIR%\7z\7za x C:\qtquickcontrols2.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtserialport-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtserialport.7z"
%SRCROOTDIR%\7z\7za x C:\qtserialport.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtsvg-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtsvg.7z"
%SRCROOTDIR%\7z\7za x C:\qtsvg.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qttools-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qttools.7z"
%SRCROOTDIR%\7z\7za x C:\qttools.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qttranslations-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qttranslations.7z"
%SRCROOTDIR%\7z\7za x C:\qttranslations.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtwinextras-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtwinextras.7z"
%SRCROOTDIR%\7z\7za x C:\qtwinextras.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%qtwebchannel-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z -OutFile C:\qtwebchannel.7z"
%SRCROOTDIR%\7z\7za x C:\qtwebchannel.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%d3dcompiler_47-x64.7z -OutFile C:\d3dcompiler.7z"
%SRCROOTDIR%\7z\7za x C:\d3dcompiler.7z -aoa -o%SRCROOTDIR%\qt
powershell -command "Invoke-WebRequest -Uri %QTURL%/%EXTRASTRING%opengl32sw-64-mesa_12_0_rc2.7z -OutFile C:\opengl32sw.7z"
%SRCROOTDIR%\7z\7za x C:\opengl32sw.7z -aoa -o%SRCROOTDIR%\qt

:begin

IF exist %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ( CALL %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ) ELSE ( ECHO vcvars64.bat not found. Please ensure the correct path is set inside the variable MSVSDIR. && EXIT /B )

cd %SRCROOTDIR% && mkdir %SRCROOTDIR%\build && cd %SRCROOTDIR%\build

%CMAKEBIN% .. %GENERATOR% ^
-Duse_mp:BOOL="1" ^
-Duse_o2:BOOL="1" ^
-Duse_ot:BOOL="1" ^
-Duse_gy:BOOL="1" ^
-Duse_oi:BOOL="1" ^
-Duse_avx2:BOOL="1" ^
-Duse_qpar:BOOL="1" ^
-Duse_qspectre:BOOL="0" ^
-Duse_control_flow_guard:BOOL="1" ^
-DQt5_DIR:PATH="%QTDIR%\lib\cmake\Qt5" ^
-DQt5Network_DIR:PATH="%QTDIR%\lib\cmake\Qt5Network" ^
-DQt5WinExtras_DIR:PATH="%QTDIR%\lib\cmake\Qt5WinExtras" ^
-DCMAKE_CONFIGURATION_TYPES:STRING="Release" ^
-DBUILD_TESTING:BOOL="0" ^
-DNO_PLUGINS:BOOL="0" ^
-DBUILD_SHARED_LIBS:BOOL="1" ^
-DQMAKE_EXECUTABLE:FILEPATH="%QTDIR%\bin\qmake.exe" ^
-DECM_DIR:PATH="%VCPKGDIR%\packages\ecm_x64-windows\share\ECM\cmake" ^
-DCMAKE_INCLUDE_PATH:PATH="%QTDIR%\include" ^
-Dunofficial-gettext_DIR:PATH="%INTL%\share\unofficial-gettext" ^
-Dunofficial-iconv_DIR:PATH="%INTL%\share\unofficial-iconv" ^
-DOPENSSL_ROOT_DIR:PATH="%OPENSSLDIR%"

IF exist %SRCROOTDIR%\build\Midori.sln (

%MSVSDIR%\MSBuild\Current\Bin\msbuild /m ".\Midori.sln" /p:CharacterSet=Unicode /p:configuration=%CONFIGURATION% /p:platform=x64 /p:PlatformToolset=%TOOLSET% /p:RunCodeAnalysis=%ANALYSIS% /p:PreferredToolArchitecture=x64 /p:UseEnv=true

) ELSE ( GOTO :buildend )

CD %SRCROOTDIR%

IF exist %SRCROOTDIR%\userscripts\ ( echo User script folder exists. ) ELSE (
	MKDIR %SRCROOTDIR%\userscripts
)

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

COPY %INTL%\bin\libintl.dll %INSTDIR%

COPY %INTL%\bin\libiconv.dll %INSTDIR%
COPY %INTL%\bin\libcharset.dll %INSTDIR%

COPY %OPENSSLDIR%\bin\libcrypto-1_1-x64.dll %INSTDIR%
COPY %OPENSSLDIR%\bin\libssl-1_1-x64.dll %INSTDIR%

:: --------------------------------------------------------------------------------

COPY %VCRUN% %INSTDIR%

SET PATH=%PATH%;%QTDIR%

%QTDIR%\bin\windeployqt ^
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

CALL %SRCROOTDIR%\windows\build\SPELL.CMD

::RENAME %INSTDIR%\midori.exe "Midori Browser.exe"
RENAME %INSTDIR% "Midori Browser"

:buildend

DEL %SRCROOTDIR%\CMakeLists.txt
REN %SRCROOTDIR%\CMakeLists.bak CMakeLists.txt

ECHO -
IF exist "%SRCROOTDIR%\Midori Browser\MidoriPrivate.dll" (
	IF exist "%SRCROOTDIR%\Midori Browser\QtWebEngineProcess.exe" (
		powershell -command "Compress-Archive -Path 'C:\GitLab-Runner\builds\tw3\midori\Midori Browser' -DestinationPath 'C:\GitLab-Runner\builds\tw3\midori\midori.zip'"
		ECHO - [32mBuild completed successfully![0m ) ELSE (
		ECHO - [31mBuild failure[0m - Please review the console output
	)
) ELSE (
	ECHO - [31mBuild failure[0m - Please review the console output
)
