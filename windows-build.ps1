
# --------------------------------------------------------------------------------
# Windows Build Script
# --------------------------------------------------------------------------------

# IMPORTANT! : Windows 10 & Windows Server 2019 are the only supported build environments currently
# IMPORTANT! : The copy of the source code must be at the tip of a drive (e.g C:\b\)
# IMPORTANT! : MS Visual Studio 2019 must already be installed
# IMPORTANT! : The default built takes 5hrs to complete the very first time (on a good machine.)
# Set -codecs to 0 to download instead of build the Qt binaries.
# IMPORTANT! : There must be a working Internet connection, but only the very first time this
# script is run (the required build dependencies are downloaded during that time)

# NOTE : You shouldn't need to modify this script at all. If you do need to, please consider sending a patch :)

# --------------------------------------------------------------------------------
# Commandline switches
# --------------------------------------------------------------------------------

# --------------------------------------------------------------------------------
# -codecs 1 # Setting this to 1 will build QtWebengine using vcpkg.
# -codecs 0 # Setting this to 0 will use precompiled QtWebengine downloaded from the Qt project.
# --------------------------------------------------------------------------------
# -buildandrun 1 # Setting this to 1 will run the application once a successful build has been completed.
# -buildandrun 0 # Setting this to 0 will prevent the application running once a successful build has been completed.
# --------------------------------------------------------------------------------
# -plugins 1 # Setting this to 1 will ensure that all of the default plugins are built along with the application.
# -plugins 0 # Setting this to 0 will ensure that the plugins are not built, this speeds up build times.
# --------------------------------------------------------------------------------
# -debugbuild 1 # Setting this to 1 will ensure that all of the possible debug options are applied. Useful for testing.
# -debugbuild 0 # Setting this to 0 will ensure that the debug options are not passed to the compiler and a release binary is built.
# --------------------------------------------------------------------------------
# -codeanalysis 1 # Setting this to 1 will enable the compiler's ability to analyse the code it is compiling. Very verbose output will appear in the console.
# -codeanalysis 0 # Setting this to 0 will ensure that code analysis is not run.
# --------------------------------------------------------------------------------
# -labbuild 1 # Setting this to 1 will enable specific options only applicable for Gitlab CI\CD builds.
# -labbuild 0 # Setting this to 0 will disable specific options only applicable for Gitlab CI\CD builds.
# --------------------------------------------------------------------------------

Param(
    [int]$codecs = 1,
    [int]$buildandrun = 1,
    [int]$plugins = 0,
    [int]$debugbuild = 0,
    [int]$codeanalysis = 0,
    [int]$labbuild = 0
)

# --------------------------------------------------------------------------------
# Build Begin
# --------------------------------------------------------------------------------

$TIMESTAMPBEGIN="{0:HH:mm:ss} {0:dd/MM/yyyy}" -f (Get-Date)
Write-Host "`nApplication build script started at $TIMESTAMPBEGIN`n"

# --------------------------------------------------------------------------------
# Process any commandline switches
# --------------------------------------------------------------------------------

Write-Host "Setting some variables..."

if ($labbuild -eq 1) {
	$buildandrun = 0
	$codecs = 0
	$plugins = 1

	Write-Host "Gitlab build was selected..."

}

if ($debugbuild -eq 1) {
	$TESTBUILD=1
	$CONFIGURATION="debug"
	Write-Host "debug build was selected..."
} else {
	$TESTBUILD=0
	$CONFIGURATION="release"
}

if ($codeanalysis -eq 1) {
	$ANALYSIS="true"
	Write-Host "Code analysis was selected..."
} else {
	$ANALYSIS="false"
}

if ($plugins -eq 1) {
	$NOPLUGINS=0
	Write-Host "Plug-ins will be built..."
} else {
	if ($debugbuild -eq 1) {
		$NOPLUGINS=0 # Building the plugins is currently required for debug builds to succeed
		Write-Host "Plugins will be built..."
	} else {
		$NOPLUGINS=1
		Write-Host "No plugins will be built..."
	}
}

# --------------------------------------------------------------------------------
# Variables
# --------------------------------------------------------------------------------

$VSINSTBINARY=[Environment]::GetEnvironmentVariable("ProgramFiles(x86)")+"\Microsoft Visual Studio\Installer\vs_installer.exe"
$SCRIPTDIR=Split-Path $MyInvocation.MyCommand.Path -Parent
$SRCROOTDIR="$SCRIPTDIR"
$VCPKGDIR="$SRCROOTDIR\vcpkg"
$GENERATOR="Visual Studio 16 2019"
$TOOLSET="v142"

$INSTDIR="$SRCROOTDIR\windeploy"
$INTLDIR="$VCPKGDIR\packages\gettext_x64-windows"
$ICONVDIR="$VCPKGDIR\packages\libiconv_x64-windows"

$BASEURLQT="https://ftp1.nluug.nl/languages/qt/online/qtsdkrepository/windows_x86/desktop"
$QTURL="$BASEURLQT/qt5_5152"
$QTEXTRASTRING="qt.qt5.5152.win64_msvc2019_64/5.15.2-0-202011130602"

# --------------------------------------------------------------------------------

if ($codecs -eq 1) {

	if ($SRCROOTDIR.Length -gt 7) {
		Throw "ERROR - The path to the location of the source code is too long.`nEither use the '-codecs 0' command line switch for this build script, or`nmove the source code to a shorter path, such as C:\b\`nThe path must be short because building QtWebEngine using a long path will fail."
	}

	$DRIVELETTER=$SRCROOTDIR.SubString(0,1)
	$DISKFREESPACE = Get-PSDrive $DRIVELETTER | Select-Object Free

	if ([Math]::Round($DISKFREESPACE.Free / 1GB) -lt 35) {
		Throw "ERROR - The amount of free disk space on the $DRIVELETTER drive is too low. The minimum required is 35GB. Please free up some space and try again."
	}

	$QTDIR="$VCPKGDIR\installed\x64-windows"
	$QTTOOLSDIR="$QTDIR\tools\qt5\bin"
	Write-Host "vcpkg's Qt binaries will be used..."
} else {
	$QTDIR="$SRCROOTDIR\qt\5.15.2\msvc2019_64"
	$QTTOOLSDIR="$QTDIR\bin"
	Write-Host "Binaries compiled by the Qt project will be used..."
}

if ($labbuild -eq 1) {

	$CMAKEBIN="$SRCROOTDIR\cmake-3.19.6-win64-x64\bin\cmake.exe"
	$OPENSSLDIR="$SRCROOTDIR\qt\Tools\OpenSSL\Win_x64"
	$DLDIR="C:"

} else {

	if (!(Test-Path "$VCPKGDIR")) {
		# Check for Windows 10
		$CURWINVER=(get-itemproperty -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion" -Name ProductName).ProductName

		if (!($CURWINVER -Match 'Windows 10' -or $CURWINVER -Match 'Windows Server 2019')) {
			Throw "ERROR - Sorry, Windows 10 & Windows Server 2019 are the only supported build environments currently."
		}

	}

	$CURCMAKEVER=(cmake /V)

	if (!($CURCMAKEVER -Match 'version')) {
		Throw "ERROR - cmake was not found. Please install https://cmake.org/ and try again."
	}

	$CMAKEBIN="cmake"
	$OPENSSLDIR="$VCPKGDIR\packages\openssl_x64-windows"
	$DLDIR="$SRCROOTDIR\downloads"

}

# --------------------------------------------------------------------------------
# Check for powershell version 5 or newer
# --------------------------------------------------------------------------------

if ((Get-Host).Version.Major -lt 5) {
	Throw "ERROR - Please upgrade your installed copy of powershell to version 5 or newer."
}

# --------------------------------------------------------------------------------
# Check for a working Internet connection
# --------------------------------------------------------------------------------

if (!(Test-Path "$VCPKGDIR")) {
	if (!(Test-Connection -ComputerName status.gitlab.com -Quiet)) {
		# This should theoretically never be triggered
		Throw "ERROR - There is no Internet connection. Please try running the script again once a working connection has been brought online."
	}
}

# --------------------------------------------------------------------------------
# Visual Studio 2019 prep and path detection
# --------------------------------------------------------------------------------

if (!(Test-Path "$VCPKGDIR") -Or ($labbuild -eq 1)) {
	Write-Host "Ensuring NuGet is available... `n"
	Install-PackageProvider -Name NuGet -MinimumVersion 2.8.5.201 -Scope CurrentUser -Force
	Write-Host "Adding VSSetup powershell module for the current user so that we can query Visual Studio...`n"
	Install-Module VSSetup -Scope CurrentUser -Force
	if ($labbuild -eq 0) {
		Write-Host "Changing powershell script execution policy for the current user to 'RemoteSigned' so that VSSetup won't prompt everytime...`n"
		Set-ExecutionPolicy RemoteSigned -Scope CurrentUser -Force
	}
	#Get-VSSetupInstance # This command simply provides information on it's own but that in itself can confirm that the scriptlet is installed and works, if required
}

Write-Host "Looking for Visual Studio..."
$latestVsInstalled = Get-VSSetupInstance -All | Sort-Object -Property InstallationVersion -Descending | Select-Object -First 1

if ($latestVsInstalled.InstallationVersion -eq $null) {
	Throw "ERROR - MS Visual Studio was not found. Please install MS Visual Studio 2019 from https://visualstudio.microsoft.com/downloads/"
} else {
	$latestVsPath = Get-VSSetupInstance -All | Sort-Object -Property InstallationPath -Descending | Select-Object -First 1
	Write-Host "Found MS Visual Studio: $($latestVsInstalled.InstallationVersion) in $($latestVsPath.InstallationPath)"
	$MSVSDIR="$($latestVsPath.InstallationPath)"
}

if ($labbuild -eq 1) {
	$MSBUILDBIN="$MSVSDIR\MSBuild\Current\Bin\msbuild.exe"
} else {
	$MSBUILDBIN="$MSVSDIR\MSBuild\Current\Bin\amd64\MSBuild.exe"
}

if ($labbuild -eq 0) {

	if (!(Test-Path "$VCPKGDIR")) {

		if (Test-Path "$VSINSTBINARY") {
			Write-Host "Found vs_installer.exe `n"
			Write-Host "Please install the workload presented in the Visual Studio installer (if required) `n"
			# This can't just be done silently because it needs privilege escalation (to be run as admin) to do so
			Start-Process -FilePath $VSINSTBINARY -ArgumentList "modify", "--installPath", "$MSVSDIR", "--config", "$SRCROOTDIR\vsconfig\midori.vsconfig" -Wait -PassThru
			if (!(Test-Path "$MSVSDIR\VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt", $MSBUILDBIN)) {
				# This check that the required components have been installed could be made to a little more comprehensive...
				Throw "ERROR - Please re-run this script and install the workload presented by the Visual Studio installer"
				Read-Host -Prompt "Press any key to exit"
			}

		} else {
			Throw "ERROR - Cannot find vs_installer.exe - Please manually set the location of vs_installer.exe in this script file using the VSINSTBINARY variable"
			Read-Host -Prompt "Press any key to exit"
		}

	}

}

Write-Host "Looking for MS Visual C runtime..."

$VCRUNVERNUM=Get-Content "$MSVSDIR\VC\Auxiliary\Build\Microsoft.VCRedistVersion.default.txt" -First 1

if ($VCRUNVERNUM -eq $null) {
	Throw "ERROR - Microsoft.VCToolsVersion.default.txt was not found.."
} else {
	Write-Host "Found MS Visual C runtime $VCRUNVERNUM"
}

if ($labbuild -eq 1) {
	$VCRUN="$MSVSDIR\VC\Tools\MSVC\$VCRUNVERNUM\bin\Hostx64\x64\vcruntime140_1.dll"
} else {
	$VCRUN="$MSVSDIR\VC\Redist\MSVC\$VCRUNVERNUM\spectre\x64\Microsoft.VC142.CRT\vcruntime140_1.dll"
}

if (!(Test-Path "$VCRUN")) {
		Throw "ERROR - Cannot find $VCRUN - Please check that the VCRUN variable is looking in the correct place."
		if ($labbuild -eq 0) {
			Read-Host -Prompt "Press any key to exit"
		}
} else {
	Write-Host "Found vcruntime140_1.dll"
}

Write-Host "Looking for vcvars64.bat"

if (Test-Path "$MSVSDIR\VC\Auxiliary\Build\vcvars64.bat") {

	Write-Host "Found vcvars64.bat"

	function Invoke-Environment { # https://github.com/majkinetor/posh/blob/master/MM_Admin/Invoke-Environment.ps1 # gnu gpl v2
    	param
    	(
        	[Parameter(Mandatory=$true)]
        	[string] $Command
    	)
    	$Command = "`"" + $Command + "`""
    	cmd /c "$Command > nul 2>&1 && set" | . { process {
        	if ($_ -match '^([^=]+)=(.*)') {
            	[System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        	}
    	}}

	}
	Invoke-Environment "$MSVSDIR\VC\Auxiliary\Build\vcvars64.bat"
} else {
	Throw "ERROR - Cannot find vcvars64.bat"
	if ($labbuild -eq 0) {
		Read-Host -Prompt "Press any key to exit"
	}
}

# --------------------------------------------------------------------------------
# vcpkg
# --------------------------------------------------------------------------------

if (!(Test-Path "$VCPKGDIR")) {

	$CURGITVER=(git --version)

	if (!($CURGITVER -Match 'version')) {
		Throw "ERROR - git was not found. Please install https://git-scm.com/ and try again."
	}

	Write-Host "Downloading vcpkg for the first time...`n"
	MKDIR $VCPKGDIR | Out-Null
	CD $VCPKGDIR
	& git init
	& git fetch https://github.com/Microsoft/vcpkg master
	# Last update to vcpkg HEAD: 07/03/2021
	& git merge c47216ac7904b09187c119c7e4d010dcf993e3d5
	Write-Host "Fixing up vcpkg... `n"
	# Build only release binaries instead of debug and release
	Add-Content $VCPKGDIR\triplets\x64-windows.cmake "set(VCPKG_BUILD_TYPE $CONFIGURATION)"
	Add-Content $VCPKGDIR\triplets\x64-windows-static.cmake "set(VCPKG_BUILD_TYPE $CONFIGURATION)"
	# Fix the use of /Z7 when creating release binaries
	(Get-Content $VCPKGDIR\scripts\toolchains\windows.cmake).replace('/DNDEBUG /Z7', '/DNDEBUG /Qspectre /Qpar /Ot /MP /arch:AVX') | Set-Content $VCPKGDIR\scripts\toolchains\windows.cmake
	if ($codecs -eq 1) {
		XCOPY /Y /E /I $SRCROOTDIR\ports\qt5-webengine $VCPKGDIR\ports\qt5-webengine
	}
	Write-Host "Running vcpkg's bootstrap... `n"
	& $VCPKGDIR\bootstrap-vcpkg.bat -disableMetrics

	if ($codecs -eq 1) {
		Write-Host "Building Qt5 and other required dependencies via vcpkg - Even on a good machine, this will take over 4hrs to complete. `n"
		& $VCPKGDIR\vcpkg.exe install angle:x64-windows qt5-base:x64-windows qt5-winextras:x64-windows qt5-translations:x64-windows qt5-tools:x64-windows qt5-webengine:x64-windows openssl-windows:x64-windows ecm:x64-windows gettext:x64-windows --binarysource=clear
	} else {
		if ($labbuild -eq 1) {
			Write-Host "Using vcpkg to obtain ecm and gettext... `n"
			& $VCPKGDIR\vcpkg.exe install ecm:x64-windows gettext:x64-windows --binarysource=clear
		} else {
			Write-Host "Using vcpkg to obtain openssl, ecm and gettext... `n"
			& $VCPKGDIR\vcpkg.exe install openssl-windows:x64-windows ecm:x64-windows gettext:x64-windows --binarysource=clear
		}
	}

} else {

	if ($codecs -eq 1) {

		if (!(Test-Path "$INTLDIR\bin\intl-8.dll", "$VCPKGDIR\packages\qt5-webengine_x64-windows\bin\Qt5WebEngine.dll", "$VCPKGDIR\packages\ecm_x64-windows\share\ECM\cmake\ECMConfig.cmake", "$OPENSSLDIR\bin\libssl-1_1-x64.dll")) {
			Write-Host "Trying again to use to vcpkg to obtain Qt and other required dependencies. `n"
			& $VCPKGDIR\vcpkg.exe install angle:x64-windows qt5-base:x64-windows qt5-winextras:x64-windows qt5-translations:x64-windows qt5-tools:x64-windows qt5-webengine:x64-windows openssl-windows:x64-windows ecm:x64-windows gettext:x64-windows --binarysource=clear
		}

	} else {

		if ($labbuild -eq 0) {

			if (!(Test-Path "$INTLDIR\bin\intl-8.dll", "$VCPKGDIR\packages\ecm_x64-windows\share\ECM\cmake\ECMConfig.cmake", "$OPENSSLDIR\bin\libssl-1_1-x64.dll")) {
				Write-Host "Trying again to use vcpkg to obtain openssl, ecm and gettext... `n"
				& $VCPKGDIR\vcpkg.exe install openssl-windows:x64-windows ecm:x64-windows gettext:x64-windows --binarysource=clear
			}

		}

	}

}

if ($codecs -eq 0) {

# --------------------------------------------------------------------------------
# Check and create directories
# --------------------------------------------------------------------------------

	Write-Host "Checking for and creating directories..."

	if (!(Test-Path "$SRCROOTDIR\7z\")) {
		MKDIR "$SRCROOTDIR\7z\" | Out-Null
		Write-Host "Created $SRCROOTDIR\7z\"
	}

	if (!(Test-Path "$SRCROOTDIR\qt")) {
		MKDIR "$SRCROOTDIR\qt" | Out-Null
		Write-Host "Created $SRCROOTDIR\qt\"
	}

	if (!(Test-Path "$DLDIR")) {
		MKDIR "$DLDIR" | Out-Null
		Write-Host "Created $DLDIR"
	}

	Write-Host "Checking for and downloading build dependencies..."

# --------------------------------------------------------------------------------
# 7-zip
# --------------------------------------------------------------------------------

	if (!(Test-Path "$SRCROOTDIR\7z\7za.exe")) {

		$SEVENZIPARCHIVE="7za.zip"

		if (!(Test-Path "$DLDIR\$SEVENZIPARCHIVE")) {
			Write-Host "Downloading 7-zip..."
			Invoke-WebRequest -Uri "https://www.7-zip.org/a/7za920.zip" -OutFile "$DLDIR\$SEVENZIPARCHIVE"
			Write-Host "Downloaded 7-zip..."
		} else {
			Write-Host "Found existing 7-zip archive..."
		}

		if (Test-Path "$DLDIR\$SEVENZIPARCHIVE") {
			$SEVENZIPHASH=(Get-FileHash -Path $DLDIR\$SEVENZIPARCHIVE -Algorithm SHA256).Hash
			if ($SEVENZIPHASH -eq "2A3AFE19C180F8373FA02FF00254D5394FEC0349F5804E0AD2F6067854FF28AC") {
				Write-Host "The checksum for the file $SEVENZIPARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$SEVENZIPARCHIVE is incorrect. Should be $SEVENZIPHASH" | Write-Warning
			}
			Write-Host "Extracting 7-zip..."
			Expand-Archive "$DLDIR\$SEVENZIPARCHIVE" -DestinationPath "$SRCROOTDIR\7z\" -Force | Out-Null
			Write-Host "Extracted 7-zip..."
		} else {
				Throw "ERROR - $DLDIR\$SEVENZIPARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing 7-zip binary..."
	}

	if ($labbuild -eq 1) {

# --------------------------------------------------------------------------------
# cmake
# --------------------------------------------------------------------------------

		if (!(Test-Path "$SRCROOTDIR\cmake-3.19.6-win64-x64\bin\cmake.exe")) {

			$CMAKEARCHIVE="cmake.zip"

			if (!(Test-Path "$DLDIR\$CMAKEARCHIVE")) {
				Write-Host "Downloading cmake..."
				Invoke-WebRequest -Uri "https://cmake.org/files/v3.19/cmake-3.19.6-win64-x64.zip" -OutFile "$DLDIR\$CMAKEARCHIVE"
				Write-Host "Downloaded cmake..."
			} else {
				Write-Host "Found existing cmake archive..."
			}

			if (Test-Path "$DLDIR\$CMAKEARCHIVE") {
				$CMAKEHASH=(Get-FileHash -Path $DLDIR\$CMAKEARCHIVE -Algorithm SHA256).Hash
				if ($CMAKEHASH -eq "6883A07F95AE01360D24F1341622F71B3E6DDC6251381752CD4A4D9D4D704C67") {
					Write-Host "The checksum for the file $CMAKEARCHIVE is correct..."
				} else {
					"The checksum for $DLDIR\$CMAKEARCHIVE is incorrect. Should be $CMAKEHASH" | Write-Warning
				}
				Write-Host "Extracting cmake..."
				& $SRCROOTDIR\7z\7za x $DLDIR\$CMAKEARCHIVE -aoa -o"${SRCROOTDIR}\" | Out-Null
				Write-Host "Extracted cmake..."
			} else {
				Throw "ERROR - $DLDIR\$CMAKEARCHIVE was not found.."
			}

		} else {
			Write-Host "Found existing cmake binary..."
		}

# --------------------------------------------------------------------------------
# openssl
# --------------------------------------------------------------------------------

		if (!(Test-Path "$OPENSSLDIR\bin\libcrypto-1_1-x64.dll")) {

			$OPENSSLARCHIVE="openssl.7z"

			if (!(Test-Path "$DLDIR\$OPENSSLARCHIVE")) {
				Write-Host "Downloading openssl..."
				Invoke-WebRequest -Uri "$BASEURLQT/tools_openssl_x64/qt.tools.openssl.win_x64/1.1.1-10openssl_1.1.1j_prebuild_x64.7z" -OutFile "$DLDIR\$OPENSSLARCHIVE"
				Write-Host "Downloaded openssl..."
			} else {
				Write-Host "Found existing openssl archive..."
			}

			if (Test-Path "$DLDIR\$OPENSSLARCHIVE") {
				$OPENSSLHASH=(Get-FileHash -Path $DLDIR\$OPENSSLARCHIVE -Algorithm SHA256).Hash
				if ($OPENSSLHASH -eq "B11EBB54254A9B17B056091557A98D53B731E5EFD51B57363D52548404C7ED9E") {
					Write-Host "The checksum for the file $OPENSSLARCHIVE is correct..."
				} else {
					"The checksum for $DLDIR\$OPENSSLARCHIVE is incorrect. Should be $OPENSSLHASH" | Write-Warning
				}
				Write-Host "Extracting openssl..."
				& $SRCROOTDIR\7z\7za x $DLDIR\$OPENSSLARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
				Write-Host "Extracted openssl..."
			} else {
				Throw "ERROR - $DLDIR\$OPENSSLARCHIVE was not found.."
			}

		} else {
			Write-Host "Found existing openssl..."
		}

	}

# --------------------------------------------------------------------------------
# Qt
# --------------------------------------------------------------------------------

	Write-Host "Checking for Qt and downloading archives if required..."

# --------------------------------------------------------------------------------
# QtWebEngine
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5WebEngine.dll")) {

		$QTWEBENGINEARCHIVE="qtwebengine.7z"

		if (!(Test-Path "$DLDIR\$QTWEBENGINEARCHIVE")) {
			Write-Host "Downloading QtWebEngine..."
			Invoke-WebRequest -Uri "$QTURL/qt.qt5.5152.qtwebengine.win64_msvc2019_64/5.15.2-0-202011130602qtwebengine-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTWEBENGINEARCHIVE"
			Write-Host "Downloaded QtWebEngine archive..."
		} else {
			Write-Host "Found existing QtWebEngine archive..."
		}

		if (Test-Path "$DLDIR\$QTWEBENGINEARCHIVE") {
			$QTWEBENGINEHASH=(Get-FileHash -Path $DLDIR\$QTWEBENGINEARCHIVE -Algorithm SHA256).Hash
			if ($QTWEBENGINEHASH -eq "E5219AC8ABE8EA757B9BB07C7CAFA59A762AF71A8975FC36BF53848222F56B3B") {
				Write-Host "The checksum for the file $QTWEBENGINEARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTWEBENGINEARCHIVE is incorrect. Should be $QTWEBENGINEHASH" | Write-Warning
			}
			Write-Host "Extracting QtWebEngine..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTWEBENGINEARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtWebEngine..."
		} else {
			Throw "ERROR - $DLDIR\$QTWEBENGINEARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtWebEngine..."
	}

# --------------------------------------------------------------------------------
# QtActiveQt
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\idc.exe")) {

		$QTACTIVEARCHIVE="qtactiveqt.7z"

		if (!(Test-Path "$DLDIR\$QTACTIVEARCHIVE")) {
			Write-Host "Downloading QtActiveQt..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtactiveqt-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTACTIVEARCHIVE"
			Write-Host "Downloaded QtActiveQt archive..."
		} else {
			Write-Host "Found existing QtActiveQt archive..."
		}

		if (Test-Path "$DLDIR\$QTACTIVEARCHIVE") {
			$QTACTIVEHASH=(Get-FileHash -Path $DLDIR\$QTACTIVEARCHIVE -Algorithm SHA256).Hash
			if ($QTACTIVEHASH -eq "2F3449FEF00A5920FA3095812B9096C27A63C7DDBC7617BA9F258E592130E980") {
				Write-Host "The checksum for the file $QTACTIVEARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTACTIVEARCHIVE is incorrect. Should be $QTACTIVEHASH" | Write-Warning
			}
			Write-Host "Extracting QtActiveQt..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTACTIVEARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtActiveQt..."
		} else {
			Throw "ERROR - $DLDIR\$QTACTIVEARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtActiveQt..."
	}

# --------------------------------------------------------------------------------
# QtBase
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5Core.dll")) {

		$QTBASEARCHIVE="qtbase.7z"

		if (!(Test-Path "$DLDIR\$QTBASEARCHIVE")) {
			Write-Host "Downloading QtBase..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtbase-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTBASEARCHIVE"
			Write-Host "Downloaded QtBase archive..."
		} else {
			Write-Host "Found existing QtBase archive..."
		}

		if (Test-Path "$DLDIR\$QTBASEARCHIVE") {
			$QTBASEHASH=(Get-FileHash -Path $DLDIR\$QTBASEARCHIVE -Algorithm SHA256).Hash
			if ($QTBASEHASH -eq "E563DE40230295841C2EECE26E347DE709EDC4BF035515CC239CAE3C994C9AF6") {
				Write-Host "The checksum for the file $QTBASEARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTBASEARCHIVE is incorrect. Should be $QTBASEHASH" | Write-Warning
			}
			Write-Host "Extracting QtBase..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTBASEARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtBase..."
		} else {
			Throw "ERROR - $DLDIR\$QTBASEARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtBase..."
	}

# --------------------------------------------------------------------------------
# QtDeclarative
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5Quick.dll")) {

		$QTDECLARCHIVE="qtdeclarative.7z"

		if (!(Test-Path "$DLDIR\$QTDECLARCHIVE")) {
			Write-Host "Downloading QtDeclarative..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtdeclarative-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTDECLARCHIVE"
			Write-Host "Downloaded QtDeclarative archive..."
		} else {
			Write-Host "Found existing QtDeclarative archive..."
		}

		if (Test-Path "$DLDIR\$QTDECLARCHIVE") {
			$QTDECLHASH=(Get-FileHash -Path $DLDIR\$QTDECLARCHIVE -Algorithm SHA256).Hash
			if ($QTDECLHASH -eq "14F761D84B8591C094245C4D66037A813C50E7FFFB1D77CA25BDDCF1271ACF41") {
				Write-Host "The checksum for the file $QTDECLARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTDECLARCHIVE is incorrect. Should be $QTDECLHASH" | Write-Warning
			}
			Write-Host "Extracting QtDeclarative..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTDECLARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtDeclarative..."
		} else {
			Throw "ERROR - $DLDIR\$QTDECLARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtDeclarative..."
	}

# --------------------------------------------------------------------------------
# QtGraphicalEffects
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\qml\QtGraphicalEffects\qtgraphicaleffectsplugin.dll")) {

		$QTGRAPHARCHIVE="qtgraphicaleffects.7z"

		if (!(Test-Path "$DLDIR\$QTGRAPHARCHIVE")) {
			Write-Host "Downloading QtGraphicalEffects..."		
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtgraphicaleffects-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTGRAPHARCHIVE"
			Write-Host "Downloaded QtGraphicalEffects archive..."
		} else {
			Write-Host "Found existing QtGraphicalEffects archive..."
		}

		if (Test-Path "$DLDIR\$QTGRAPHARCHIVE") {
			$QTGRAPHHASH=(Get-FileHash -Path $DLDIR\$QTGRAPHARCHIVE -Algorithm SHA256).Hash
			if ($QTGRAPHHASH -eq "0B403E7798DAF9417D8B920E5E35D1715E302532A5D1B8A167A7D6D53B613353") {
				Write-Host "The checksum for the file $QTGRAPHARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTGRAPHARCHIVE is incorrect. Should be $QTGRAPHHASH" | Write-Warning
			}
			Write-Host "Extracting QtGraphicalEffects..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTGRAPHARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtGraphicalEffects..."
		} else {
			Throw "ERROR - $DLDIR\$QTGRAPHARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtGraphicalEffects..."
	}

# --------------------------------------------------------------------------------
# QtImageFormats
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\plugins\imageformats\qicns.dll")) {

		$QTIMGFARCHIVE="qtimageformats.7z"

		if (!(Test-Path "$DLDIR\$QTIMGFARCHIVE")) {
			Write-Host "Downloading QtImageFormats..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtimageformats-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTIMGFARCHIVE"
			Write-Host "Downloaded QtImageFormats archive..."
		} else {
			Write-Host "Found existing QtImageFormats archive..."
		}

		if (Test-Path "$DLDIR\$QTIMGFARCHIVE") {
			$QTIMGFHASH=(Get-FileHash -Path $DLDIR\$QTIMGFARCHIVE -Algorithm SHA256).Hash
			if ($QTIMGFHASH -eq "3CC93E55C0E8509290DB49DE36D879DEE1E84CB74DDF177FB611F25562CD5F2C") {
				Write-Host "The checksum for the file $QTIMGFARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTIMGFARCHIVE is incorrect. Should be $QTIMGFHASH" | Write-Warning
			}
			Write-Host "Extracting QtImageFormats..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTIMGFARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtImageFormats..."
		} else {
			Throw "ERROR - $DLDIR\$QTIMGFARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtImageFormats..."
	}

# --------------------------------------------------------------------------------
# QtLocation
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5Location.dll")) {

		$QTLOCARCHIVE="qtlocation.7z"

		if (!(Test-Path "$DLDIR\$QTLOCARCHIVE")) {
			Write-Host "Downloading QtLocation..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtlocation-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTLOCARCHIVE"
			Write-Host "Downloaded QtLocation archive..."
		} else {
			Write-Host "Found existing QtLocation archive..."
		}

		if (Test-Path "$DLDIR\$QTLOCARCHIVE") {
			$QTLOCHASH=(Get-FileHash -Path $DLDIR\$QTLOCARCHIVE -Algorithm SHA256).Hash
			if ($QTLOCHASH -eq "68B630D524170FEC504C02E8051A6969A62D284B442642CA728C088B95D5258A") {
				Write-Host "The checksum for the file $QTLOCARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTLOCARCHIVE is incorrect. Should be $QTLOCHASH" | Write-Warning
			}
			Write-Host "Extracting QtLocation..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTLOCARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtLocation..."
		} else {
			Throw "ERROR - $DLDIR\$QTLOCARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtLocation..."
	}

# --------------------------------------------------------------------------------
# QtMultimedia
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5Multimedia.dll")) {

		$QTMULTIARCHIVE="qtmultimedia.7z"

		if (!(Test-Path "$DLDIR\$QTMULTIARCHIVE")) {
			Write-Host "Downloading QtMultimedia..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtmultimedia-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTMULTIARCHIVE"
			Write-Host "Downloaded QtMultimedia archive..."
		} else {
			Write-Host "Found existing QtMultimedia archive..."
		}

		if (Test-Path "$DLDIR\$QTMULTIARCHIVE") {
			$QTMULTIHASH=(Get-FileHash -Path $DLDIR\$QTMULTIARCHIVE -Algorithm SHA256).Hash
			if ($QTMULTIHASH -eq "F07053981AFE77D4BFBA8841470A685B1ABD4545C18168D14656FBB4DB1AEFFE") {
				Write-Host "The checksum for the file $QTMULTIARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTMULTIARCHIVE is incorrect. Should be $QTMULTIHASH" | Write-Warning
			}
			Write-Host "Extracting QtMultimedia..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTMULTIARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtMultimedia..."
		} else {
			Throw "ERROR - $DLDIR\$QTMULTIARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtMultimedia..."
	}

# --------------------------------------------------------------------------------
# QtQuickControls
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\qml\QtQuick\Controls\Button.qml")) {

		$QTQUICARCHIVE="qtquickcontrols.7z"

		if (!(Test-Path "$DLDIR\$QTQUICARCHIVE")) {
			Write-Host "Downloading QtQuickControls..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtquickcontrols-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTQUICARCHIVE"
			Write-Host "Downloaded QtQuickControls archive..."
		} else {
			Write-Host "Found existing QtQuickControls archive..."
		}

		if (Test-Path "$DLDIR\$QTQUICARCHIVE") {
			$QTQUICHASH=(Get-FileHash -Path $DLDIR\$QTQUICARCHIVE -Algorithm SHA256).Hash
			if ($QTQUICHASH -eq "2DF51FE56AF5A187A3D58C97A09CA753DDD2CEC155B5C19DF84A0E25C85DD134") {
				Write-Host "The checksum for the file $QTQUICARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTQUICARCHIVE is incorrect. Should be $QTQUICHASH" | Write-Warning
			}
			Write-Host "Extracting QtQuickControls..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTQUICARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtQuickControls..."
		} else {
			Throw "ERROR - $DLDIR\$QTQUICARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtQuickControls..."
	}

# --------------------------------------------------------------------------------
# QtQuickControls2
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5QuickControls2.dll")) {

		$QTQUICKCSARCHIVE="qtquickcontrols2.7z"

		if (!(Test-Path "$DLDIR\$QTQUICKCSARCHIVE")) {
			Write-Host "Downloading QtQuickControls2..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtquickcontrols2-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTQUICKCSARCHIVE"
			Write-Host "Downloaded QtQuickControls2 archive..."
		} else {
			Write-Host "Found existing QtQuickControls2 archive..."
		}

		if (Test-Path "$DLDIR\$QTQUICKCSARCHIVE") {
			$QTQUICKCSHASH=(Get-FileHash -Path $DLDIR\$QTQUICKCSARCHIVE -Algorithm SHA256).Hash
			if ($QTQUICKCSHASH -eq "AF9A2AB0E149E01D2DF1FAF37AE9047FBC3FD56EEB6F40AEE8A90EF3E18612E9") {
				Write-Host "The checksum for the file $QTQUICKCSARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTQUICKCSARCHIVE is incorrect. Should be $QTQUICKCSHASH" | Write-Warning
			}
			Write-Host "Extracting QtQuickControls2..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTQUICKCSARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtQuickControls2..."
		} else {
			Throw "ERROR - $DLDIR\$QTQUICKCSARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtQuickControls2..."
	}

# --------------------------------------------------------------------------------
# QtSerialPort
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5SerialPort.dll")) {

		$QTSERIALPORTARCHIVE="qtserialport.7z"

		if (!(Test-Path "$DLDIR\$QTSERIALPORTARCHIVE")) {
			Write-Host "Downloading QtSerialPort..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtserialport-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTSERIALPORTARCHIVE"
			Write-Host "Downloaded QtSerialPort archive..."
		} else {
			Write-Host "Found existing QtSerialPort archive..."
		}

		if (Test-Path "$DLDIR\$QTSERIALPORTARCHIVE") {
			$QTSERIALPORTHASH=(Get-FileHash -Path $DLDIR\$QTSERIALPORTARCHIVE -Algorithm SHA256).Hash
			if ($QTSERIALPORTHASH -eq "09EB0840E82F9C1D3EAFE9D0811DDC51032889F651C29FBAEECD4A02A1D13BAA") {
				Write-Host "The checksum for the file $QTSERIALPORTARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTSERIALPORTARCHIVE is incorrect. Should be $QTSERIALPORTHASH" | Write-Warning
			}
			Write-Host "Extracting QtSerialPort..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTSERIALPORTARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtSerialPort..."
		} else {
			Throw "ERROR - $DLDIR\$QTSERIALPORTARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtSerialPort..."
	}

# --------------------------------------------------------------------------------
# QtSvg
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5Svg.dll")) {

		$QTSVGARCHIVE="qtsvg.7z"

		if (!(Test-Path "$DLDIR\$QTSVGARCHIVE")) {
			Write-Host "Downloading QtSvg..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtsvg-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTSVGARCHIVE"
			Write-Host "Downloaded QtSvg archive..."
		} else {
			Write-Host "Found existing QtSvg archive..."
		}

		if (Test-Path "$DLDIR\$QTSVGARCHIVE") {
			$QTSVGHASH=(Get-FileHash -Path $DLDIR\$QTSVGARCHIVE -Algorithm SHA256).Hash
			if ($QTSVGHASH -eq "22DAA63531F2EED8AEEC02E047B73641739FA9D0288A5D2DB4DA25BAF3A005A7") {
				Write-Host "The checksum for the file $QTSVGARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTSVGARCHIVE is incorrect. Should be $QTSVGHASH" | Write-Warning
			}
			Write-Host "Extracting QtSvg..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTSVGARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtSvg..."
		} else {
			Throw "ERROR - $DLDIR\$QTSVGARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtSvg..."
	}

# --------------------------------------------------------------------------------
# QtTools
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\windeployqt.exe")) {

		$QTTOOLARCHIVE="qttools.7z"

		if (!(Test-Path "$DLDIR\$QTTOOLARCHIVE")) {
			Write-Host "Downloading QtTools..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qttools-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTTOOLARCHIVE"
			Write-Host "Downloaded QtTools archive..."
		} else {
			Write-Host "Found existing QtTools archive..."
		}

		if (Test-Path "$DLDIR\$QTTOOLARCHIVE") {
			$QTTOOLHASH=(Get-FileHash -Path $DLDIR\$QTTOOLARCHIVE -Algorithm SHA256).Hash
			if ($QTTOOLHASH -eq "BABB583BBBAC0E946AD25A2A687B4A3349779D750B1F3DE1C4DADBF1C343C3EA") {
				Write-Host "The checksum for the file $QTTOOLARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTTOOLARCHIVE is incorrect. Should be $QTTOOLHASH" | Write-Warning
			}
			Write-Host "Extracting QtTools..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTTOOLARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtTools..."
		} else {
			Throw "ERROR - $DLDIR\$QTTOOLARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtTools..."
	}

# --------------------------------------------------------------------------------
# QtTranslations
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\translations\qt_en.qm")) {

		$QTTRANARCHIVE="qttranslations.7z"

		if (!(Test-Path "$DLDIR\$QTTRANARCHIVE")) {
			Write-Host "Downloading QtTranslations..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qttranslations-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTTRANARCHIVE"
			Write-Host "Downloaded QtTranslations archive..."
		} else {
			Write-Host "Found existing QtTranslations archive..."
		}

		if (Test-Path "$DLDIR\$QTTRANARCHIVE") {
			$QTTRANHASH=(Get-FileHash -Path $DLDIR\$QTTRANARCHIVE -Algorithm SHA256).Hash
			if ($QTTRANHASH -eq "F039471D3AFB4CBD245D98BE0051C59C74BB53E894EE46C2B2C70DEDDF5B6E69") {
				Write-Host "The checksum for the file $QTTRANARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTTRANARCHIVE is incorrect. Should be $QTTRANHASH" | Write-Warning
			}
			Write-Host "Extracting QtTranslations..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTTRANARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtTranslations..."
		} else {
			Throw "ERROR - $DLDIR\$QTTRANARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtTranslations..."
	}

# --------------------------------------------------------------------------------
# QtWebChannel
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5WebChannel.dll")) {

		$QTWEBCHAARCHIVE="qtwebchannel.7z"

		if (!(Test-Path "$DLDIR\$QTWEBCHAARCHIVE")) {
			Write-Host "Downloading QtWebChannel..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtwebchannel-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTWEBCHAARCHIVE"
			Write-Host "Downloaded QtWebChannel archive..."
		} else {
			Write-Host "Found existing QtWebChannel archive..."
		}

		if (Test-Path "$DLDIR\$QTWEBCHAARCHIVE") {
			$QTWEBCHAHASH=(Get-FileHash -Path $DLDIR\$QTWEBCHAARCHIVE -Algorithm SHA256).Hash
			if ($QTWEBCHAHASH -eq "7C1636053BDB66564AD6D1CC1160BA5D2B8A0D7F36C30C807F316AA30549CAA3") {
				Write-Host "The checksum for the file $QTWEBCHAARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTWEBCHAARCHIVE is incorrect. Should be $QTWEBCHAHASH" | Write-Warning
			}
			Write-Host "Extracting QtWebChannel..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTWEBCHAARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtWebChannel..."
		} else {
			Throw "ERROR - $DLDIR\$QTWEBCHAARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtWebChannel..."
	}

# --------------------------------------------------------------------------------
# QtWinExtras
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\Qt5WinExtras.dll")) {

		$QTWINEXTRARCHIVE="qtwinextras.7z"

		if (!(Test-Path "$DLDIR\$QTWINEXTRARCHIVE")) {
			Write-Host "Downloading QtWinExtras..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}qtwinextras-Windows-Windows_10-MSVC2019-Windows-Windows_10-X86_64.7z" -OutFile "$DLDIR\$QTWINEXTRARCHIVE"
			Write-Host "Downloaded QtWinExtras archive..."
		} else {
			Write-Host "Found existing QtWinExtras archive..."
		}

		if (Test-Path "$DLDIR\$QTWINEXTRARCHIVE") {
			$QTWINEXTRHASH=(Get-FileHash -Path $DLDIR\$QTWINEXTRARCHIVE -Algorithm SHA256).Hash
			if ($QTWINEXTRHASH -eq "F705EB5D232761C9989434B5FA7A5BAA01DD5EF67489A8D6DED3EF8C4FAB27F1") {
				Write-Host "The checksum for the file $QTWINEXTRARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$QTWINEXTRARCHIVE is incorrect. Should be $QTWINEXTRHASH" | Write-Warning
			}
			Write-Host "Extracting QtWinExtras..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$QTWINEXTRARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted QtWinExtras..."
		} else {
			Throw "ERROR - $DLDIR\$QTWINEXTRARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing QtWinExtras..."
	}

# --------------------------------------------------------------------------------
# d3dcompiler
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\d3dcompiler_47.dll")) {

		$DCOMPARCHIVE="d3dcompiler.7z"

		if (!(Test-Path "$DLDIR\$DCOMPARCHIVE")) {
			Write-Host "Downloading d3dcompiler..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}d3dcompiler_47-x64.7z" -OutFile "$DLDIR\$DCOMPARCHIVE"
			Write-Host "Downloaded d3dcompiler archive..."
		} else {
			Write-Host "Found existing d3dcompiler archive..."
		}

		if (Test-Path "$DLDIR\$DCOMPARCHIVE") {
			$DCOMPHASH=(Get-FileHash -Path $DLDIR\$DCOMPARCHIVE -Algorithm SHA256).Hash
			if ($DCOMPHASH -eq "0DC63CA9BB91CB204D479356EDB89B30E3599F1C0BCE469B1DD5A339134F25E2") {
				Write-Host "The checksum for the file $DCOMPARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$DCOMPARCHIVE is incorrect. Should be $DCOMPHASH" | Write-Warning
			}
			Write-Host "Extracting d3dcompiler..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$DCOMPARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted d3dcompiler..."
		} else {
			Throw "ERROR - $DLDIR\$DCOMPARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing d3dcompiler..."
	}

# --------------------------------------------------------------------------------
# opengl32sw
# --------------------------------------------------------------------------------

	if (!(Test-Path "$QTDIR\bin\opengl32sw.dll")) {

		$OGLSWARCHIVE="opengl32sw.7z"

		if (!(Test-Path "$DLDIR\$OGLSWARCHIVE")) {
			Write-Host "Downloading opengl32sw..."
			Invoke-WebRequest -Uri "$QTURL/${QTEXTRASTRING}opengl32sw-64-mesa_12_0_rc2.7z" -OutFile "$DLDIR\$OGLSWARCHIVE"
			Write-Host "Downloaded opengl32sw archive..."
		} else {
			Write-Host "Found existing opengl32sw archive..."
		}

		if (Test-Path "$DLDIR\$OGLSWARCHIVE") {
			$OGLSWHASH=(Get-FileHash -Path $DLDIR\$OGLSWARCHIVE -Algorithm SHA256).Hash
			if ($OGLSWHASH -eq "4E43BC46665F31E12E528F01105310B07D3B8C0FF914668E3B77969D672FCC14") {
				Write-Host "The checksum for the file $OGLSWARCHIVE is correct..."
			} else {
				"The checksum for $DLDIR\$OGLSWARCHIVE is incorrect. Should be $OGLSWHASH" | Write-Warning
			}
			Write-Host "Extracting opengl32sw..."
			& $SRCROOTDIR\7z\7za x $DLDIR\$OGLSWARCHIVE -aoa -o"${SRCROOTDIR}\qt" | Out-Null
			Write-Host "Extracted opengl32sw..."
		} else {
			Throw "ERROR - $DLDIR\$OGLSWARCHIVE was not found.."
		}

	} else {
		Write-Host "Found existing opengl32sw..."
	}

}

# --------------------------------------------------------------------------------
# Clean stale build directories
# --------------------------------------------------------------------------------

Write-Host "Removing any stale build directories..."

if (Test-Path "$INSTDIR") {
	remove-item -path "$INSTDIR" -Force -Recurse
}

if (Test-Path "$SRCROOTDIR\Midori Browser") {
	remove-item -path "$SRCROOTDIR\Midori Browser" -Force -Recurse
}

if (Test-Path "$SRCROOTDIR\build") {
	remove-item -path "$SRCROOTDIR\build" -Force -Recurse
}

Write-Host "Creating the build directory..."

MKDIR $SRCROOTDIR\build | Out-Null
CD $SRCROOTDIR\build

# --------------------------------------------------------------------------------
# Run cmake
# --------------------------------------------------------------------------------

Write-Host "`nRunning cmake...`n"

if ($labbuild -eq 1) {
	# Not building with spectre mitigations currently
	& $CMAKEBIN ".." -G $GENERATOR -A x64 `
	-Duse_mp:BOOL="1" `
	-Duse_o2:BOOL="1" `
	-Duse_ot:BOOL="1" `
	-Duse_gy:BOOL="1" `
	-Duse_oi:BOOL="1" `
	-Duse_avx2:BOOL="1" `
	-Duse_qpar:BOOL="1" `
	-Duse_qspectre:BOOL="0" `
	-Duse_control_flow_guard:BOOL="1" `
	-DBUILD_TESTING:BOOL="$TESTBUILD" `
	-DNO_PLUGINS:BOOL="$NOPLUGINS" `
	-DBUILD_SHARED_LIBS:BOOL="1" `
	-DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" `
	-DCMAKE_TOOLCHAIN_FILE="$VCPKGDIR\scripts\buildsystems\vcpkg.cmake" `
	-DQt5_DIR:PATH="$QTDIR\lib\cmake\Qt5" `
	-DQt5Network_DIR:PATH="$QTDIR\lib\cmake\Qt5Network" `
	-DQt5WinExtras_DIR:PATH="$QTDIR\lib\cmake\Qt5WinExtras" `
	-DQMAKE_EXECUTABLE:FILEPATH="$QTDIR\bin\qmake.exe" `
	-DCMAKE_INCLUDE_PATH:PATH="$QTDIR\include" `
	-DOPENSSL_ROOT_DIR:PATH="$OPENSSLDIR"

} else {

	if ($codecs -eq 1) {

		& $CMAKEBIN ".." -G $GENERATOR -A x64 `
		-Duse_mp:BOOL="1" `
		-Duse_o2:BOOL="1" `
		-Duse_ot:BOOL="1" `
		-Duse_gy:BOOL="1" `
		-Duse_oi:BOOL="1" `
		-Duse_avx2:BOOL="1" `
		-Duse_qpar:BOOL="1" `
		-Duse_qspectre:BOOL="1" `
		-Duse_control_flow_guard:BOOL="1" `
		-DBUILD_TESTING:BOOL="$TESTBUILD" `
		-DNO_PLUGINS:BOOL="$NOPLUGINS" `
		-DBUILD_SHARED_LIBS:BOOL="1" `
		-DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" `
		-DCMAKE_TOOLCHAIN_FILE="$VCPKGDIR\scripts\buildsystems\vcpkg.cmake"

	} else {

		& $CMAKEBIN ".." -G $GENERATOR -A x64 `
		-Duse_mp:BOOL="1" `
		-Duse_o2:BOOL="1" `
		-Duse_ot:BOOL="1" `
		-Duse_gy:BOOL="1" `
		-Duse_oi:BOOL="1" `
		-Duse_avx2:BOOL="1" `
		-Duse_qpar:BOOL="1" `
		-Duse_qspectre:BOOL="1" `
		-Duse_control_flow_guard:BOOL="1" `
		-DBUILD_TESTING:BOOL="$TESTBUILD" `
		-DNO_PLUGINS:BOOL="$NOPLUGINS" `
		-DBUILD_SHARED_LIBS:BOOL="1" `
		-DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" `
		-DQt5_DIR:PATH="$QTDIR\lib\cmake\Qt5" `
		-DQt5Network_DIR:PATH="$QTDIR\lib\cmake\Qt5Network" `
		-DQt5WinExtras_DIR:PATH="$QTDIR\lib\cmake\Qt5WinExtras" `
		-DECM_DIR:PATH="$VCPKGDIR\packages\ecm_x64-windows\share\ECM\cmake" `
		-DIntl_INCLUDE_DIR:PATH="$VCPKGDIR\packages\gettext_x64-windows\share\intl" `
		-DQMAKE_EXECUTABLE:FILEPATH="$QTDIR\bin\qmake.exe" `
		-DCMAKE_INCLUDE_PATH:PATH="$QTDIR\include" `
		-DOPENSSL_ROOT_DIR:PATH="$VCPKGDIR\packages\openssl_x64-windows"

	}

}

# --------------------------------------------------------------------------------
# MSBUILD
# --------------------------------------------------------------------------------

if (Test-Path "$SRCROOTDIR\build\Midori.sln") {
	Write-Host "`nAttempting to build the application...`n"
	& $MSBUILDBIN /m ".\Midori.sln" /p:CharacterSet=Unicode /p:configuration=$CONFIGURATION /p:platform=x64 /p:PlatformToolset=$TOOLSET /p:RunCodeAnalysis=$ANALYSIS /p:PreferredToolArchitecture=x64 /p:UseEnv=true
} else {
	CD $SRCROOTDIR
	Throw "ERROR - cmake failed to configure the project. Please correct the errors presented in the console output and try again."
	Read-Host -Prompt "Press any key to exit"
}

# --------------------------------------------------------------------------------
# Copy files to the installation directory
# --------------------------------------------------------------------------------

Write-Host "`nCopying required files into the application's directory...`n"

MKDIR $INSTDIR | Out-Null

if ($debugbuild -eq 1) {
	COPY "$SRCROOTDIR\build\bin\$CONFIGURATION\*.*" $INSTDIR
	XCOPY /Y /E /I $SRCROOTDIR\build\bin\plugins\$CONFIGURATION $INSTDIR\plug-ins
} else {
	COPY $SRCROOTDIR\build\bin\$CONFIGURATION\MidoriPrivate.dll $INSTDIR
	COPY $SRCROOTDIR\build\bin\$CONFIGURATION\midori.exe $INSTDIR
}
COPY $SRCROOTDIR\COPYING $INSTDIR
XCOPY /Y /E /I $SRCROOTDIR\themes $INSTDIR\themes
MKDIR $INSTDIR\plug-ins | Out-Null

if ($plugins -eq 1) {
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\AutoScroll.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\DisableJS.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\FlashCookieManager.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\GreaseMonkey.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\MouseGestures.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\PIM.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\StatusBarIcons.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\TabManager.dll $INSTDIR\plug-ins\
	COPY $SRCROOTDIR\build\bin\plugins\$CONFIGURATION\VerticalTabs.dll $INSTDIR\plug-ins\
}

MKDIR $INSTDIR\userscripts | Out-Null
# If any default userscripts are ever shipped with the application, use the line below to copy them
#XCOPY /Y /E /I $SRCROOTDIR\userscripts $INSTDIR\userscripts
Rename-Item $INSTDIR\COPYING LICENSE.TXT

# --------------------------------------------------------------------------------

'[Config]' | Out-File -FilePath $INSTDIR\midori.conf -encoding utf8
'Portable=true' | Out-File -FilePath $INSTDIR\midori.conf -Append -encoding utf8

'[Paths]' | Out-File -FilePath $INSTDIR\qt.conf -encoding utf8
'Plugins = "."' | Out-File -FilePath $INSTDIR\qt.conf -Append -encoding utf8

# --------------------------------------------------------------------------------

COPY $INTLDIR\bin\intl-8.dll $INSTDIR

COPY $ICONVDIR\bin\charset-1.dll $INSTDIR
COPY $ICONVDIR\bin\iconv-2.dll $INSTDIR

COPY $OPENSSLDIR\bin\libcrypto-1_1-x64.dll $INSTDIR
COPY $OPENSSLDIR\bin\libssl-1_1-x64.dll $INSTDIR

if ($codecs -eq 1) {
	COPY $QTDIR\bin\*.dll $INSTDIR
}

# --------------------------------------------------------------------------------

# widevinecdm.dll
# widevinecdm.dll.sig

# --------------------------------------------------------------------------------

COPY $VCRUN $INSTDIR

# --------------------------------------------------------------------------------
# windeployqt - https://doc.qt.io/qt-5/windows-deployment.html https://doc.qt.io/Qt-5/qtwebengine-deploying.html
# --------------------------------------------------------------------------------

Write-Host "`nRunning windeployqt to ensure that the application is properly configured...`n"

if ($debugbuild -eq 1) {

& $QTTOOLSDIR\windeployqt.exe `
-sql `
-svg `
-winextras `
-webengine `
-printsupport `
-quickwidgets `
-webenginecore `
-webenginewidgets `
--libdir $INSTDIR `
--plugindir $INSTDIR `
--debug `
--pdb `
--no-opengl-sw `
--no-compiler-runtime `
$INSTDIR

} else {

& $QTTOOLSDIR\windeployqt.exe `
-sql `
-svg `
-winextras `
-webengine `
-printsupport `
-quickwidgets `
-webenginecore `
-webenginewidgets `
--libdir $INSTDIR `
--plugindir $INSTDIR `
--release `
--no-opengl-sw `
--no-compiler-runtime `
$INSTDIR

}
# --------------------------------------------------------------------------------
# Spelling
# --------------------------------------------------------------------------------

Write-Host "`nAdding spelling dictionaries...`n"
# For archive size reasons, we obviously can't add all available dictionaries
if (!(Test-Path "$SRCROOTDIR\spell")) {
	MKDIR "$SRCROOTDIR\spell" | Out-Null
}

# --------------------------------------------------------------------------------

$ENGBARCHIVE="en_GB.zip"

if (!(Test-Path "$SRCROOTDIR\spell\$ENGBARCHIVE")) {
	Write-Host "Downloading dictionary en_GB..."
	Invoke-WebRequest -Uri "https://tw3.gitlab.io/qtwebenginedictionaries/dicts/$ENGBARCHIVE" -OutFile $SRCROOTDIR\spell\$ENGBARCHIVE
	Write-Host "Downloaded dictionary en_GB..."
	$ENGBHASH=(Get-FileHash -Path $SRCROOTDIR\spell\$ENGBARCHIVE -Algorithm SHA256).Hash
	if ($ENGBHASH -eq "A21ED295618DF42E5558B6760EC7888AECFD5757ACA4170F061448831D3FD516") {
		Write-Host "The checksum for the file $ENGBARCHIVE is correct..."
	} else {
		"The checksum for $SRCROOTDIR\spell\$ENGBARCHIVE is incorrect. Should be $ENGBHASH" | Write-Warning
	}
} else {
	Write-Host "Found existing en_GB dictionary archive..."
}

# --------------------------------------------------------------------------------

$ENUSARCHIVE="en_US.zip"

if (!(Test-Path "$SRCROOTDIR\spell\$ENUSARCHIVE")) {
	Write-Host "Downloading dictionary en_US..."
	Invoke-WebRequest -Uri "https://tw3.gitlab.io/qtwebenginedictionaries/dicts/$ENUSARCHIVE" -OutFile $SRCROOTDIR\spell\$ENUSARCHIVE
	Write-Host "Downloaded dictionary en_US..."
	$ENUSHASH=(Get-FileHash -Path $SRCROOTDIR\spell\$ENUSARCHIVE -Algorithm SHA256).Hash
	if ($ENUSHASH -eq "66A71991FDDABA9A8D2D34968D56C2FD82B46FF7191DB2FA40893C851C9E4588") {
		Write-Host "The checksum for the file $ENUSARCHIVE is correct..."
	} else {
		"The checksum for $SRCROOTDIR\spell\$ENUSARCHIVE is incorrect. Should be $ENUSHASH" | Write-Warning
	}
} else {
	Write-Host "Found existing en_US dictionary archive..."
}

# --------------------------------------------------------------------------------

$ESESARCHIVE="es_ES.zip"

if (!(Test-Path "$SRCROOTDIR\spell\$ESESARCHIVE")) {
	Write-Host "Downloading dictionary es_ES..."
	Invoke-WebRequest -Uri "https://tw3.gitlab.io/qtwebenginedictionaries/dicts/$ESESARCHIVE" -OutFile $SRCROOTDIR\spell\$ESESARCHIVE
	Write-Host "Downloaded dictionary es_ES..."
	$ESESHASH=(Get-FileHash -Path $SRCROOTDIR\spell\$ESESARCHIVE -Algorithm SHA256).Hash
	if ($ESESHASH -eq "499520076A13C4FD4A91AE56509DCEE294E858368018BF1294AD609ED26D9BB9") {
		Write-Host "The checksum for the file $ESESARCHIVE is correct..."
	} else {
		"The checksum for $SRCROOTDIR\spell\$ESESARCHIVE is incorrect. Should be $ESESHASH" | Write-Warning
	}
} else {
	Write-Host "Found existing es_ES dictionary archive..."
}

# --------------------------------------------------------------------------------

$PTPTARCHIVE="pt_PT.zip"

if (!(Test-Path "$SRCROOTDIR\spell\$PTPTARCHIVE")) {
	Write-Host "Downloading dictionary pt_PT..."
	Invoke-WebRequest -Uri "https://tw3.gitlab.io/qtwebenginedictionaries/dicts/$PTPTARCHIVE" -OutFile $SRCROOTDIR\spell\$PTPTARCHIVE
	Write-Host "Downloaded dictionary pt_PT..."
	$PTPTHASH=(Get-FileHash -Path $SRCROOTDIR\spell\$PTPTARCHIVE -Algorithm SHA256).Hash
	if ($PTPTHASH -eq "E418EA0B1A2F74CDA46A5CE00351AE147CAAA50DEBE6AE864307CF1BE1926EA2") {
		Write-Host "The checksum for the file $PTPTARCHIVE is correct..."
	} else {
		"The checksum for $SRCROOTDIR\spell\$PTPTARCHIVE is incorrect. Should be $PTPTHASH" | Write-Warning
	}
} else {
	Write-Host "Found existing pt_PT dictionary archive..."
}

# --------------------------------------------------------------------------------

$RURUARCHIVE="ru_RU.zip"

if (!(Test-Path "$SRCROOTDIR\spell\$RURUARCHIVE")) {
	Write-Host "Downloading dictionary ru_RU..."
	Invoke-WebRequest -Uri "https://tw3.gitlab.io/qtwebenginedictionaries/dicts/$RURUARCHIVE" -OutFile $SRCROOTDIR\spell\$RURUARCHIVE
	Write-Host "Downloaded dictionary ru_RU..."
	$RURUHASH=(Get-FileHash -Path $SRCROOTDIR\spell\$RURUARCHIVE -Algorithm SHA256).Hash
	if ($RURUHASH -eq "C902DBE82B9279F2511A4BF2F76E90FCAF48B763BCFAD28563BFBCD888CBF2F2") {
		Write-Host "The checksum for the file $RURUARCHIVE is correct..."
	} else {
		"The checksum for $SRCROOTDIR\spell\$RURUARCHIVE is incorrect. Should be $RURUHASH" | Write-Warning
	}
} else {
	Write-Host "Found existing ru_RU dictionary archive..."
}

# --------------------------------------------------------------------------------
# Create the destination directory and extract the archives
# --------------------------------------------------------------------------------

$DICTSDIR="$INSTDIR\qtwebengine_dictionaries"
MKDIR $DICTSDIR | Out-Null

# --------------------------------------------------------------------------------

Expand-Archive $SRCROOTDIR\spell\$ENGBARCHIVE -DestinationPath $DICTSDIR -Force  | Out-Null

if (!(Test-Path "$DICTSDIR\en_GB.bdic")) {
	"ERROR: Spelling dictionary en_GB was not extracted because it does not exist!`n" | Write-Warning
} else {
	Write-Host "Extracted $ENGBARCHIVE sucessfully..."
}

# --------------------------------------------------------------------------------

Expand-Archive $SRCROOTDIR\spell\$ENUSARCHIVE -DestinationPath $DICTSDIR -Force | Out-Null

if (!(Test-Path "$DICTSDIR\en_US.bdic")) {
	"ERROR: Spelling dictionary en_US was not extracted because it does not exist!`n" | Write-Warning
} else {
	Write-Host "Extracted $ENUSARCHIVE sucessfully..."
}

# --------------------------------------------------------------------------------

Expand-Archive $SRCROOTDIR\spell\$ESESARCHIVE -DestinationPath $DICTSDIR -Force | Out-Null

if (!(Test-Path "$DICTSDIR\es_ES.bdic")) {
	"ERROR: Spelling dictionary es_ES was not extracted because it does not exist!`n" | Write-Warning
} else {
	Write-Host "Extracted $ESESARCHIVE sucessfully..."
}

# --------------------------------------------------------------------------------

Expand-Archive $SRCROOTDIR\spell\$PTPTARCHIVE -DestinationPath $DICTSDIR -Force | Out-Null

if (!(Test-Path "$DICTSDIR\pt_PT.bdic")) {
	"ERROR: Spelling dictionary pt_PT was not extracted because it does not exist!`n" | Write-Warning
} else {
	Write-Host "Extracted $PTPTARCHIVE sucessfully..."
}

# --------------------------------------------------------------------------------

Expand-Archive $SRCROOTDIR\spell\$RURUARCHIVE -DestinationPath $DICTSDIR -Force | Out-Null

if (!(Test-Path "$DICTSDIR\ru_RU.bdic")) {
	"ERROR: Spelling dictionary ru_RU was not extracted because it does not exist!`n" | Write-Warning
} else {
	Write-Host "Extracted $RURUARCHIVE sucessfully..."
}

# --------------------------------------------------------------------------------

Write-Host "Spelling dictionaries added.`n"

# --------------------------------------------------------------------------------
# Finish up
# --------------------------------------------------------------------------------

Rename-Item $INSTDIR "Midori Browser"
#Rename-Item $INSTDIR\midori.exe "Midori Browser.exe" # Not currently doing this
if (Test-Path "$SRCROOTDIR\Midori Browser\midori.exe") {
	$TIMESTAMPEND="{0:HH:mm:ss} {0:dd/MM/yyyy}" -f (Get-Date)
	Write-Host "--------------------------------------------------------------------------------`n"
	Write-Host "- Build success!`n"
	Write-Host "- The folder `"$SRCROOTDIR\Midori Browser`" contains the output from this build.`n"
	Write-Host "- Build started at $TIMESTAMPBEGIN and completed at $TIMESTAMPEND`n"
	Write-Host "--------------------------------------------------------------------------------`n"
	if ($buildandrun -eq 1) {
		& "$SRCROOTDIR\Midori Browser\midori.exe"
	}
	if ($labbuild -eq 1) {
		Compress-Archive -Path "$SRCROOTDIR\Midori Browser" -DestinationPath "$SRCROOTDIR\midori.zip"
	}
} else {
	Write-Host "--------------------------------------------------------------------------------`n"
	Write-Host "- Build failed.. Please review the console output.`n"
	Write-Host "--------------------------------------------------------------------------------`n"
}

# --------------------------------------------------------------------------------

if ($labbuild -eq 0) {
	CD $SRCROOTDIR
	Read-Host -Prompt "Press any key to exit"
}
