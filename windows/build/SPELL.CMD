
:: Blue Hawk spelling dictionary script
:: For archive size reasons, we obviously can't add all available dictionaries

IF exist %SRCROOTDIR%\spell\ ( echo Spelling dictionary archive folder exists. ) ELSE (
	MKDIR %SRCROOTDIR%\spell
)

IF exist %SRCROOTDIR%\spell\en_GB.zip ( echo Spelling dictionary en_GB.zip exists. ) ELSE (
	powershell -command "Invoke-WebRequest -Uri 'https://tw3.gitlab.io/qtwebenginedictionaries/dicts/en_GB.zip' -OutFile %SRCROOTDIR%\spell\en_GB.zip"
)
IF exist %SRCROOTDIR%\spell\en_US.zip ( echo Spelling dictionary en_US.zip exists. ) ELSE (
	powershell -command "Invoke-WebRequest -Uri 'https://tw3.gitlab.io/qtwebenginedictionaries/dicts/en_US.zip' -OutFile %SRCROOTDIR%\spell\en_US.zip"
)
IF exist %SRCROOTDIR%\spell\es_ES.zip ( echo Spelling dictionary es_ES.zip exists. ) ELSE (
	powershell -command "Invoke-WebRequest -Uri 'https://tw3.gitlab.io/qtwebenginedictionaries/dicts/es_ES.zip' -OutFile %SRCROOTDIR%\spell\es_ES.zip"
)
IF exist %SRCROOTDIR%\spell\pt_PT.zip ( echo Spelling dictionary pt_PT.zip exists. ) ELSE (
	powershell -command "Invoke-WebRequest -Uri 'https://tw3.gitlab.io/qtwebenginedictionaries/dicts/pt_PT.zip' -OutFile %SRCROOTDIR%\spell\pt_PT.zip"
)
IF exist %SRCROOTDIR%\spell\ru_RU.zip ( echo Spelling dictionary ru_RU.zip exists. ) ELSE (
	powershell -command "Invoke-WebRequest -Uri 'https://tw3.gitlab.io/qtwebenginedictionaries/dicts/ru_RU.zip' -OutFile %SRCROOTDIR%\spell\ru_RU.zip"
)

SET DICTSDIR=%INSTDIR%\qtwebengine_dictionaries\
MKDIR %DICTSDIR%
:: Extract the dictionary archives
IF exist %SRCROOTDIR%\spell\en_GB.zip (
	powershell -command "Expand-Archive %SRCROOTDIR%\spell\en_GB.zip -DestinationPath %DICTSDIR% -Force"
) ELSE (
	echo [31mSpelling dictionary en_GB.zip did not download. Please check your net connection.[0m && EXIT /B
)
IF exist %SRCROOTDIR%\spell\en_US.zip (
	powershell -command "Expand-Archive %SRCROOTDIR%\spell\en_US.zip -DestinationPath %DICTSDIR% -Force"
) ELSE (
	echo [31mSpelling dictionary en_US.zip did not download. Please check your net connection.[0m && EXIT /B
)
IF exist %SRCROOTDIR%\spell\es_ES.zip (
	powershell -command "Expand-Archive %SRCROOTDIR%\spell\es_ES.zip -DestinationPath %DICTSDIR% -Force"
) ELSE (
	echo [31mSpelling dictionary es_ES.zip did not download. Please check your net connection.[0m && EXIT /B
)
IF exist %SRCROOTDIR%\spell\pt_PT.zip (
	powershell -command "Expand-Archive %SRCROOTDIR%\spell\pt_PT.zip -DestinationPath %DICTSDIR% -Force"
) ELSE (
	echo [31mSpelling dictionary pt_PT.zip did not download. Please check your net connection.[0m && EXIT /B
)
IF exist %SRCROOTDIR%\spell\ru_RU.zip (
	powershell -command "Expand-Archive %SRCROOTDIR%\spell\ru_RU.zip -DestinationPath %DICTSDIR% -Force"
) ELSE (
	echo [31mSpelling dictionary ru_RU.zip did not download. Please check your net connection.[0m && EXIT /B
)
