@echo off
SETLOCAL

rem Find UE4 installation directory
set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%1"
set VALUE_NAME=InstalledDirectory

echo Search in Windows Register for : %KEY_NAME%

set UE4Path=""

FOR /F "usebackq skip=2 tokens=1,2*" %%A IN (`reg query %KEY_NAME% /v %VALUE_NAME%`) DO (
	
	echo %%C
    echo Installation found at : %%C
    CALL :LabelStartBuild "%%C" %1
)

goto :EOF

rem Build!
:LabelStartBuild
echo.
echo == Start Build %2... ==
echo.
echo Use UnrealEngine UE4_PATH environment variable : %1
CALL %1\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="%CD%\..\Plugins\RPRPlugin\RPRPlugin.uplugin" -nop4 -utf8output -pak -distribution -compile -package="%CD%\PluginStaging\UE4_%2\RPRPlugin" -Rocket -TargetPlatforms=Win64 ^> output_log.txt
echo.
echo == Build completed ==
echo.
GOTO :EOF
REM :LabelStartBuild

:LabelExit
ENDLOCAL

pause