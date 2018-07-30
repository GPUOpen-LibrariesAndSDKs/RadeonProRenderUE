@echo off
SETLOCAL

if not "%2"=="" (
	if "%2" == "-debug" (
		goto :SetDebugMode
	)
)
goto :SetShippingMode


:SetDebugMode
echo Make Debug build
set Mode=Debug
set UATArgs=-clientconfig=DebugGame
goto :FindUE4InstallationDirectory

:SetShippingMode
echo Make Shipping build
set Mode=Shipping
set UATArgs=-distribution
goto :FindUE4InstallationDirectory


:FindUE4InstallationDirectory
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
goto :LabelExit

rem Build!
:LabelStartBuild
echo.
echo == Start Build %2... ==
echo.
echo Use UnrealEngine UE4_PATH environment variable : %1
set OutputPath=%CD%\Stage\UE4_%2\%Mode%\RPRPlugin
CALL %1\Engine\Build\BatchFiles\RunUAT.bat BuildPlugin -Plugin="%CD%\..\..\Plugins\RPRPlugin\RPRPlugin.uplugin" -nop4 -utf8output -pak -compile -package="%OutputPath%" -Rocket -TargetPlatforms=Win64 %UATArgs%
if not %errorlevel% == 0 goto :LabelExit
echo.
echo == Build completed ==
echo.


:Clean
echo == Clean ==
del /f /s /q "%OutputPath%\Intermediate" 1>nul
rmdir /s /q "%OutputPath%\Intermediate"
goto :PostBuild

:PostBuild
echo == Post build ==
if %Mode%=="Shipping" (
	rem echo Delete PDB files...
	rem del "%OutputPath%\Binaries\Win64\*.pdb"
	echo Delete library files...
	del "%OutputPath%\RPRPlugin\Binaries\Win64\*.lib"
	echo Delete sources...
	del /f /s /q "%OutputPath%\Sources" 1>nul
	rmdir /s /q "%OutputPath%\Sources"
	echo Deletes completed.
)
goto :LabelExit

:LabelExit
ENDLOCAL

pause