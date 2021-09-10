SETLOCAL

:: If the vcpkg portion of the very first build fails for any reason,
:: use this script to try the vcpkg part again. It should succeed and the
:: build script can then be run

:: Important Variables
::
SET SRCROOTDIR=D:\midoribrowser\midori
::
:: End - Important Variables

:: VS2019 - Variables
::
SET MSVSDIR="D:\Programs\MSVS2019"
::
:: End - VS2019 - Variables

:: Internal Variables
::
SET VCPKGDIR=%SRCROOTDIR%\vcpkg
::
:: End - Internal Variables

IF exist %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ( CALL %MSVSDIR%"\VC\Auxiliary\Build\vcvars64.bat" ) ELSE ( ECHO vcvars64.bat not found. Please ensure the correct path is set inside the variable MSVSDIR. && EXIT /B )

CD %VCPKGDIR%

vcpkg install angle:x64-windows qt5-base[latest]:x64-windows  qt5-winextras:x64-windows qt5-translations:x64-windows qt5-tools:x64-windows qt5-webengine:x64-windows ecm:x64-windows openssl-windows:x64-windows gettext:x64-windows


cd %SRCROOTDIR%\windows
