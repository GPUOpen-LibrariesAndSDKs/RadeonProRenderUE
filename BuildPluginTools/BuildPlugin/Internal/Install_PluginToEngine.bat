@echo off

:FindUE4InstallationDirectory
rem Find UE4 installation directory
set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%1"
set VALUE_NAME=InstalledDirectory

echo Search in Windows Register for : %KEY_NAME%

set UE4Path=""

FOR /F "usebackq skip=2 tokens=1,2*" %%A IN (`reg query %KEY_NAME% /v %VALUE_NAME%`) DO (
	
	echo %%C
    echo Installation found at : %%C
    CALL :StartInstallation "%%C" %1
)
goto :EOF

:StartInstallation <UE4Path>
set "UE4Path=%1"
set "Mode=%2"
set "PluginPath=%CD%\..\..\Plugins\RPRPlugin"
set "EnginePluginPath=%UE4Path%\Engine\Plugins\Experimental"
set "EngineRPRPluginDirectory=%EnginePluginPath%\RPRPlugin"

echo Delete existing files...
rmdir /S /Q %EngineRPRPluginDirectory%

echo Copy Binaries...
xcopy /syik %PluginPath%\Binaries\*.dll %EngineRPRPluginDirectory%\Binaries\
xcopy /syik %PluginPath%\Binaries\*.pdb %EngineRPRPluginDirectory%\Binaries\
xcopy /syik %PluginPath%\Binaries\*.modules %EngineRPRPluginDirectory%\Binaries\

echo Copy Config...
xcopy /syik %PluginPath%\Config %EngineRPRPluginDirectory%\Config\

echo Copy Content...
xcopy /syik %PluginPath%\Content %EngineRPRPluginDirectory%\Content\

echo Copy Resources...
xcopy /syik %PluginPath%\Resources %EngineRPRPluginDirectory%\Resources\

echo Copy ThirdParties
xcopy /syik %PluginPath%\ThirdParty\*.dll %EngineRPRPluginDirectory%\ThirdParty\

echo Copy Plugin descriptor
xcopy /yk %PluginPath%\RPRPlugin.uplugin %EngineRPRPluginDirectory%\RPRPlugin.uplugin*

echo Copies completed.

echo Explorer to %EngineRPRPluginDirectory%
call explorer "%EngineRPRPluginDirectory%"