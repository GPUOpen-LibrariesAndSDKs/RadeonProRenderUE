@echo off
SET LOCAL

:FindUE4InstallationDirectory
rem Find UE4 installation directory
set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\4.22"
set VALUE_NAME=InstalledDirectory

echo Search in Windows Register for : %KEY_NAME%

set UE4Path=""

FOR /F "usebackq skip=2 tokens=1,2*" %%A IN (`reg query %KEY_NAME% /v %VALUE_NAME%`) DO (

	echo %%C
    echo Installation found at : %%C
    call :GenerateVisualStudioProject "%%C"
)
goto :LabelExit

:GenerateVisualStudioProject
call %1\Engine\Binaries\DotNET\UnrealBuildTool.exe -2019 -projectfiles -project="%CD%\RPR.uproject" -game

:LabelExit
ENDLOCAL

pause