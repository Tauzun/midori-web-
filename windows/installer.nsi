; Midori Windows Installer NSIS Script
; Copyright (C) 2010-2017  David Rosca <nowrep@gmail.com>
;               2012-2017  S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
;
; For compiling this script you need following plugins:
; FindProcDLL_plug-in, KillProcDLL_plug-in and 'AllAssociation.nsh' needs
; Registry_plug-in, Application_Association_Registration_plug-in
; Unicode version of them can be downloaded from:
; http://sourceforge.net/projects/findkillprocuni/files/bin/
; http://nsis.sourceforge.net/Application_Association_Registration_plug-in
; http://nsis.sourceforge.net/Registry_plug-in

!ifndef CUSTOM
  !define VERSION 3.0.0
  !define ARCH x86
  !define MSVC_VER 140
  !define OPENSSL_BIN_DIR .
  !define MSVC_REDIST_DIR .
  !define MIDORI_SRC_DIR ..\..\
  !define MIDORI_BIN_DIR .
  !define ICU_BIN_DIR .
  !define QT_DIR .
  !define QT_BIN_DIR .
  !define QT_PLUGINS_DIR .
  !define QTWEBENGINE_DICTIONARIES_DIR qtwebengine_dictionaries
  !undef PORTABLE
!endif

; WinVer.nsh was added in the same release that RequestExecutionLevel so check
; if ___WINVER__NSH___ is defined to determine if RequestExecutionLevel is
; available.
!include /NONFATAL WinVer.nsh
!include x64.nsh

Var installAsPortable
RequestExecutionLevel admin

!addplugindir "wininstall\"

!include "StdUtils.nsh"
!include "FileFunc.nsh"
!include "wininstall\AllAssociation.nsh"
SetCompressor /SOLID /FINAL lzma

!define PRODUCT_NAME "Midori"
!define /date PRODUCT_VERSION "${VERSION}"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\midori.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_CAPABILITIES_KEY "Software\${PRODUCT_NAME}\Capabilities"

!include "MUI.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "wininstall\install.ico"
!define MUI_UNICON "wininstall\uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "wininstall\welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "wininstall\welcome.bmp"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ${MIDORI_BIN_DIR}\COPYRIGHT.txt
;;;
Page custom InstallationModePage InstallationModeLeave
;;;
!define MUI_PAGE_CUSTOMFUNCTION_PRE "SkipComponentsIfPortableInstalltion"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
;;;
Page custom installationInfoPage installationInfoLeave
;;;
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION "RunMidoriAsUser"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME} Installer ${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}\"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

!include "wininstall\strings.nsh"

Section "${TITLE_SecMain}" SecMain
  SectionIn RO
  FindProcDLL::FindProc "midori.exe"
  IntCmp $R0 1 0 notRunning
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${MSG_RunningInstance}" /SD IDOK IDCANCEL AbortInstallation
    KillProcDLL::KillProc "midori.exe"
    Sleep 100
    Goto notRunning
AbortInstallation:
  Abort "${MSG_InstallationCanceled}"

notRunning:
  SetOverwrite on

  SetOutPath "$INSTDIR"
  File "${MIDORI_BIN_DIR}\COPYRIGHT.txt"
  File "${MIDORI_BIN_DIR}\midori.exe"
  File "${MIDORI_BIN_DIR}\midoriprivate.dll"
  File "${MIDORI_BIN_DIR}\qt.conf"
  File "${OPENSSL_BIN_DIR}\libeay32.dll"
  File "${OPENSSL_BIN_DIR}\ssleay32.dll"
  File /nonfatal "${OPENSSL_BIN_DIR}\msvcr*.dll"
  File "${MSVC_REDIST_DIR}\*"
  File "${ICU_BIN_DIR}\icudt54.dll"
  File "${ICU_BIN_DIR}\icuin54.dll"
  File "${ICU_BIN_DIR}\icuuc54.dll"
  File "${QT_BIN_DIR}\libEGL.dll"
  File "${QT_BIN_DIR}\libGLESv2.dll"
  File "${QT_BIN_DIR}\opengl32sw.dll"
  File "${QT_BIN_DIR}\D3Dcompiler_47.dll"
  File "${QT_BIN_DIR}\Qt5Core.dll"
  File "${QT_BIN_DIR}\Qt5Gui.dll"
  File "${QT_BIN_DIR}\Qt5Network.dll"
  File "${QT_BIN_DIR}\Qt5Positioning.dll"
  File "${QT_BIN_DIR}\Qt5PrintSupport.dll"
  File "${QT_BIN_DIR}\Qt5Qml.dll"
  File "${QT_BIN_DIR}\Qt5Quick.dll"
  File "${QT_BIN_DIR}\Qt5QuickWidgets.dll"
  File "${QT_BIN_DIR}\Qt5Sql.dll"
  File "${QT_BIN_DIR}\Qt5Svg.dll"
  File "${QT_BIN_DIR}\Qt5WinExtras.dll"
  File "${QT_BIN_DIR}\Qt5WebEngine.dll"
  File "${QT_BIN_DIR}\Qt5WebEngineCore.dll"
  File "${QT_BIN_DIR}\Qt5WebEngineWidgets.dll"
  File "${QT_BIN_DIR}\Qt5WebChannel.dll"
  File "${QT_BIN_DIR}\Qt5Widgets.dll"
  File "${QT_BIN_DIR}\QtWebEngineProcess.exe"

  SetOutPath "$INSTDIR\iconengines"
  File "${QT_PLUGINS_DIR}\iconengines\qsvgicon.dll"

  SetOutPath "$INSTDIR\imageformats"
  File "${QT_PLUGINS_DIR}\imageformats\*.dll"

  SetOutPath "$INSTDIR\platforms"
  File "${QT_PLUGINS_DIR}\platforms\qwindows.dll"

  SetOutPath "$INSTDIR\printsupport"
  File "${QT_PLUGINS_DIR}\printsupport\windowsprintersupport.dll"

  SetOutPath "$INSTDIR\qml\QtQuick.2"
  File "${QT_DIR}\qml\QtQuick.2\*"

  SetOutPath "$INSTDIR\qml\QtWebEngine"
  File "${QT_DIR}\qml\QtWebEngine\*"

  SetOutPath "$INSTDIR\resources"
  File "${QT_DIR}\resources\*"

  SetOutPath "$INSTDIR\sqldrivers"
  File "${QT_PLUGINS_DIR}\sqldrivers\qsqlite.dll"

  SetOutPath "$INSTDIR\styles"
  File "${QT_PLUGINS_DIR}\styles\*.dll"

  call RegisterCapabilities

  StrCmp $installAsPortable "YES" 0 skipPortableMode
  FileOpen $0 $INSTDIR\midori.conf w
  FileWrite $0 "[Config]$\r$\n"
  FileWrite $0 "Portable=true$\r$\n"

  StrCmp $0 "" 0 closeHandle
  MessageBox MB_OK|MB_ICONEXCLAMATION "${MSG_PortableWriteError}"
  goto skipPortableMode

closeHandle:
  FileClose $0

skipPortableMode:
SectionEnd

SectionGroup "${TITLE_SecThemes}" SecThemes

  Section Default SecDefault
  SectionIn RO
  SetOutPath "$INSTDIR\themes\windows"
  File "${MIDORI_SRC_DIR}\themes\windows\*"
  SetOutPath "$INSTDIR\themes\windows\images"
  File "${MIDORI_SRC_DIR}\themes\windows\images\*"
  SectionEnd

  Section Chrome SecChrome
  SetOutPath "$INSTDIR\themes\chrome"
  File "${MIDORI_SRC_DIR}\themes\chrome\*"
  SetOutPath "$INSTDIR\themes\chrome\images"
  File "${MIDORI_SRC_DIR}\themes\chrome\images\*"
  SectionEnd

  Section Mac SecMac
  SetOutPath "$INSTDIR\themes\mac"
  File "${MIDORI_SRC_DIR}\themes\mac\*"
  SetOutPath "$INSTDIR\themes\mac\images"
  File "${MIDORI_SRC_DIR}\themes\mac\images\*"
  SectionEnd
SectionGroupEnd

Section "${TITLE_SecTranslations}" SecTranslations
  SetOutPath "$INSTDIR\locale"
  File /r "${MIDORI_BIN_DIR}\locale\*"

  SetOutPath "$INSTDIR\translations\qtwebengine_locales"
  File "${QT_DIR}\translations\qtwebengine_locales\*"

  SetOutPath "$INSTDIR\qtwebengine_dictionaries\doc"
  ; In some packages underline '_' is used and in some other packages dash '-' is used so we use wildcard
  File "${QTWEBENGINE_DICTIONARIES_DIR}\doc\README*en*US.txt"

  SetOutPath "$INSTDIR\qtwebengine_dictionaries"
  ; in some packages *.bdic files use dash '-' instead of underline '_' followed by a version number. e.g. en-US-3-0.bdic
  File "${QTWEBENGINE_DICTIONARIES_DIR}\en*US*.bdic"
SectionEnd

Section "${TITLE_SecPlugins}" SecPlugins
  SetOutPath "$INSTDIR\plugins"
  File "${MIDORI_BIN_DIR}\plugins\*.dll"
SectionEnd


SectionGroup "${TITLE_SecSetASDefault}" SecSetASDefault
    Section "${TITLE_SecExtensions}" SecExtensions
      StrCmp $installAsPortable "NO" 0 skipSetExtentions
      SetOutPath "$INSTDIR"
      ${RegisterAssociation} ".htm" "$INSTDIR\midori.exe" "MidoriHTML" "Midori HTML Document" "$INSTDIR\midori.exe,1" "file"
      ${RegisterAssociation} ".html" "$INSTDIR\midori.exe" "MidoriHTML" "Midori HTML Document" "$INSTDIR\midori.exe,1" "file"
      ${UpdateSystemIcons}
      skipSetExtentions:
    SectionEnd

    Section "${TITLE_SecProtocols}" SecProtocols
      StrCmp $installAsPortable "NO" 0 skipSecProtocols
      ${RegisterAssociation} "http" "$INSTDIR\midori.exe" "MidoriURL" "Midori URL" "$INSTDIR\midori.exe,0" "protocol"
      ${RegisterAssociation} "https" "$INSTDIR\midori.exe" "MidoriURL" "Midori URL" "$INSTDIR\midori.exe,0" "protocol"
      ${RegisterAssociation} "ftp" "$INSTDIR\midori.exe" "MidoriURL" "Midori URL" "$INSTDIR\midori.exe,0" "protocol"
      ${UpdateSystemIcons}
      skipSecProtocols:
    SectionEnd
SectionGroupEnd

Section -StartMenu
  StrCmp $installAsPortable "NO" 0 skipStartMenu
  SetOutPath "$INSTDIR"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\Midori"
  CreateShortCut "$SMPROGRAMS\Midori\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\Midori\Midori.lnk" "$INSTDIR\midori.exe"
  CreateShortCut "$SMPROGRAMS\Midori\License.lnk" "$INSTDIR\COPYRIGHT.txt"
  skipStartMenu:
SectionEnd

Section "${TITLE_SecDesktop}" SecDesktop
  StrCmp $installAsPortable "NO" 0 skipDesktopIcon
  SetOutPath "$INSTDIR"
  CreateShortCut "$DESKTOP\Midori.lnk" "$INSTDIR\midori.exe" ""
  skipDesktopIcon:
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "${DESC_SecMain}"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecTranslations} "${DESC_SecTranslations}"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecPlugins} "${DESC_SecPlugins}"

  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "${DESC_SecDesktop}"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecExtensions} "${DESC_SecExtensions}"

  !insertmacro MUI_DESCRIPTION_TEXT ${SecThemes} "${DESC_SecThemes}"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecSetASDefault} "${DESC_SecSetASDefault}"
  !insertmacro MUI_DESCRIPTION_TEXT ${SecProtocols} "${DESC_SecProtocols}"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section -Uninstaller
  StrCmp $installAsPortable "NO" 0 skipUninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\midori.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\midori.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "TW3"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "https://gitlab.com/TW3/midori"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstallSource" "$EXEDIR"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "https://tw3.gitlab.io/midori/"
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
  skipUninstaller:
SectionEnd

Section Uninstall
  FindProcDLL::FindProc "midori.exe"
  IntCmp $R0 1 0 notRunning
  MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "${MSG_RunningInstance}" /SD IDOK IDCANCEL AbortInstallation
    KillProcDLL::KillProc "midori.exe"
    Sleep 100
    Goto notRunning
AbortInstallation:
  Abort "${MSG_InstallationCanceled}"

notRunning:
  SetShellVarContext all
  Delete "$DESKTOP\Midori.lnk"

  Delete "$INSTDIR\midori.exe"
  Delete "$INSTDIR\midoriprivate.dll"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\COPYRIGHT.txt"
  Delete "$INSTDIR\qt.conf"
  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\ssleay32.dll"
  Delete "$INSTDIR\libEGL.dll"
  Delete "$INSTDIR\libGLESv2.dll"
  Delete "$INSTDIR\opengl32sw.dll"
  Delete "$INSTDIR\D3Dcompiler_47.dll"
  Delete "$INSTDIR\QtWebEngineProcess.exe"

  ; Wildcard delete to compact script of uninstall section
  Delete "$INSTDIR\icu*.dll"
  Delete "$INSTDIR\Qt5*.dll"
  Delete "$INSTDIR\msvc*.dll"
  Delete "$INSTDIR\vc*.dll"
  Delete "$INSTDIR\concrt*.dll"

  ; Recursively delete folders in root of $INSTDIR
  RMDir /r "$INSTDIR\iconengines"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\platforms"
  RMDir /r "$INSTDIR\printsupport"
  RMDir /r "$INSTDIR\qml"
  RMDir /r "$INSTDIR\resources"
  RMDir /r "$INSTDIR\translations"
  RMDir /r "$INSTDIR\sqldrivers"
  RMDir /r "$INSTDIR\styles"
  RMDir /r "$INSTDIR\qtwebengine_dictionaries"
  RMDir /r "$INSTDIR\themes"
  RMDir /r "$INSTDIR\locale"
  RMDir /r "$INSTDIR\plugins"

  ; Remove $INSTDIR if it is empty
  RMDir "$INSTDIR"

  ; Remove start menu programs folder
  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

  DeleteRegKey HKLM "Software\${PRODUCT_NAME}"
  DeleteRegValue HKLM "SOFTWARE\RegisteredApplications" "${PRODUCT_NAME}"

  ${UnRegisterAssociation} ".htm" "MidoriHTML" "$INSTDIR\midori.exe" "file"
  ${UnRegisterAssociation} ".html" "MidoriHTML" "$INSTDIR\midori.exe" "file"
  ${UnRegisterAssociation} "http" "MidoriURL" "$INSTDIR\midori.exe" "protocol"
  ${UnRegisterAssociation} "https" "MidoriURL" "$INSTDIR\midori.exe" "protocol"
  ${UnRegisterAssociation} "ftp" "MidoriURL" "$INSTDIR\midori.exe" "protocol"
  ${UpdateSystemIcons}
SectionEnd

BrandingText "${PRODUCT_NAME} ${PRODUCT_VERSION} Installer"

Function .onInit
        ;Prevent running installer of 64-bit Midori on 32-bit Windows
        ${If} ${RunningX64}
          ${If} ${ARCH} == "x64"
            StrCpy $InstDir "$PROGRAMFILES64\${PRODUCT_NAME}\"
          ${Else}
            StrCpy $InstDir "$PROGRAMFILES32\${PRODUCT_NAME}\"
          ${Endif}
        ${Else}
          ${If} ${ARCH} == "x64"
            MessageBox MB_OK|MB_ICONEXCLAMATION "This installation requiers Windows x64!"
            Quit
          ${Else}
            StrCpy $InstDir "$PROGRAMFILES\${PRODUCT_NAME}\"
          ${Endif}
        ${EndIf}

        ;Extract InstallOptions files
        ;$PLUGINSDIR will automatically be removed when the installer closes
        InitPluginsDir
        File /oname=$PLUGINSDIR\portable-mode.ini "portable-mode.ini"
        File /oname=$PLUGINSDIR\portable-info.ini "portable-info.ini"


        ;Prevent Multiple Instances
        System::Call 'kernel32::CreateMutexA(i 0, i 0, t "MidoriInstaller-4ECB4694-2C39-4f93-9122-A986344C4E7B") i .r1 ?e'
        Pop $R0
        StrCmp $R0 0 skip
          ;Return when running silent instalation
          IfSilent doAbort 0
            MessageBox MB_OK|MB_ICONEXCLAMATION "Midori installer is already running!" /SD IDOK
        doAbort:
            Abort
    skip:
        call notifyUserIfHotfixNeeded
FunctionEnd

Function RegisterCapabilities
    StrCmp $installAsPortable "NO" 0 skipRegisterCapabilities
    !ifdef ___WINVER__NSH___
        ${If} ${AtLeastWinVista}
            ; even if we don't associate Midori as default for ".htm" and ".html"
            ; we need to write these ProgIds for future use!
            ;(e.g.: user uses "Default Programs" on Win7 or Vista to set Midori as default.)
            ${CreateProgId} "MidoriHTML" "$INSTDIR\midori.exe" "Midori HTML Document" "$INSTDIR\midori.exe,1"
            ${CreateProgId} "MidoriURL" "$INSTDIR\midori.exe" "Midori URL" "$INSTDIR\midori.exe,0"

            ; note: these lines just introduce capabilities of Midori to OS and don't change defaults!
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}" "ApplicationDescription" "${PRODUCT_DESC}"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}" "ApplicationIcon" "$INSTDIR\midori.exe,0"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}" "ApplicationName" "${PRODUCT_NAME}"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\FileAssociations" ".htm" "MidoriHTML"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\FileAssociations" ".html" "MidoriHTML"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\URLAssociations" "http" "MidoriURL"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\URLAssociations" "https" "MidoriURL"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\URLAssociations" "ftp" "MidoriURL"
            WriteRegStr HKLM "${PRODUCT_CAPABILITIES_KEY}\Startmenu" "StartMenuInternet" "$INSTDIR\midori.exe"
            WriteRegStr HKLM "SOFTWARE\RegisteredApplications" "${PRODUCT_NAME}" "${PRODUCT_CAPABILITIES_KEY}"
        ${EndIf}
    !endif
skipRegisterCapabilities:
FunctionEnd

Function RunMidoriAsUser
    ${StdUtils.ExecShellAsUser} $0 "$INSTDIR\midori.exe" "open" ""
FunctionEnd

Function un.onInit
    ReadRegStr $R0 ${PRODUCT_UNINST_ROOT_KEY}  "${PRODUCT_UNINST_KEY}" "InstallLocation"
    IfErrors +2 0
        StrCpy $INSTDIR "$R0"

    IfFileExists "$INSTDIR\midori.exe" found
        MessageBox MB_OK|MB_ICONSTOP "${MSG_InvalidInstallPath}"
        Abort
    found:
FunctionEnd

Function InstallationModePage
    !insertmacro MUI_HEADER_TEXT "${TITLE_InstallationMode}" "${DESC_InstallationMode}"

    WriteINIStr "$PLUGINSDIR\portable-mode.ini" "Field 1" "Text" "${TITLE_StandardInstallation}"
    WriteINIStr "$PLUGINSDIR\portable-mode.ini" "Field 2" "Text" "${TITLE_PortableInstallation}"

    InstallOptions::dialog $PLUGINSDIR\portable-mode.ini
FunctionEnd

Function InstallationModeLeave
  ReadINIStr $0 "$PLUGINSDIR\portable-mode.ini" "Field 2" "State"

  StrCmp $0 1 0 notchecked
  StrCpy $installAsPortable "YES"
  StrCpy $InstDir "$DESKTOP\${PRODUCT_NAME} Portable\"

  goto skip

 notchecked:
  StrCpy $installAsPortable "NO"

 skip:
FunctionEnd

Function installationInfoPage
    !insertmacro MUI_HEADER_TEXT "${TITLE_InstallInfo}" "${DESC_InstallInfo}"

    StrCmp $installAsPortable "NO" 0 infoPortable
    WriteINIStr "$PLUGINSDIR\portable-info.ini" "Field 1" "Text" "${DESC_StandardInstallation}"

    Goto showInfo

infoPortable:
    WriteINIStr "$PLUGINSDIR\portable-info.ini" "Field 1" "Text" "${DESC_PortableInstallation}"

showInfo:
    InstallOptions::dialog $PLUGINSDIR\portable-info.ini
FunctionEnd

Function installationInfoLeave
FunctionEnd

Function SkipComponentsIfPortableInstalltion
StrCmp $installAsPortable "YES" 0 +2
    Abort
FunctionEnd

Function notifyUserIfHotfixNeeded
    ; check if Windows 10 family. Taken from: http://nsis.sourceforge.net/Get_Windows_version
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentMajorVersionNumber
    StrCmp $R0 '' 0 skiphotfixwarn

    ClearErrors
    nsExec::ExecToStack 'cmd /Q /C "%SYSTEMROOT%\System32\wbem\wmic.exe qfe get hotfixid | %SYSTEMROOT%\System32\findstr.exe "^KB2999226""'
    Pop $0 ; return value (it always 0 even if an error occured)
    Pop $1 ; command output KB2999226

    StrCmp $1 "" 0 skiphotfixwarn
      MessageBox MB_YESNO|MB_ICONQUESTION "${MSG_HotfixNeeded}" IDNO +2
        ExecShell open "https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows"

    skiphotfixwarn:
FunctionEnd