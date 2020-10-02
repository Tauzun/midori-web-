!macro customInstall
  WriteRegStr SHCTX "SOFTWARE\RegisteredApplications" "Midori" "Software\Clients\StartMenuInternet\Midori\Capabilities"

  WriteRegStr SHCTX "SOFTWARE\Classes\Midori" "" "Midori HTML Document"
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\Application" "AppUserModelId" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\Application" "ApplicationIcon" "$INSTDIR\Midori.exe,0"
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\Application" "ApplicationName" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\Application" "ApplicationCompany" "Midori"      
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\Application" "ApplicationDescription" "A privacy-focused, extensible and beautiful web browser"      
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\DefaultIcon" "DefaultIcon" "$INSTDIR\Midori.exe,0"
  WriteRegStr SHCTX "SOFTWARE\Classes\Midori\shell\open\command" "" '"$INSTDIR\Midori.exe" "%1"'

  WriteRegStr SHCTX "SOFTWARE\Classes\.htm\OpenWithProgIds" "Midori" ""
  WriteRegStr SHCTX "SOFTWARE\Classes\.html\OpenWithProgIds" "Midori" ""

  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori" "" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\DefaultIcon" "" "$INSTDIR\Midori.exe,0"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities" "ApplicationDescription" "A privacy-focused, extensible and beautiful web browser"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities" "ApplicationName" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities" "ApplicationIcon" "$INSTDIR\Midori.exe,0"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities\FileAssociations" ".htm" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities\FileAssociations" ".html" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities\URLAssociations" "http" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities\URLAssociations" "https" "Midori"
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\Capabilities\StartMenu" "StartMenuInternet" "Midori"
  
  WriteRegDWORD SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\InstallInfo" "IconsVisible" 1
  
  WriteRegStr SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori\shell\open\command" "" "$INSTDIR\Midori.exe"
!macroend
!macro customUnInstall
  DeleteRegKey SHCTX "SOFTWARE\Classes\Midori"
  DeleteRegKey SHCTX "SOFTWARE\Clients\StartMenuInternet\Midori"
  DeleteRegValue SHCTX "SOFTWARE\RegisteredApplications" "Midori"
!macroend