@echo off

rem Find UE4 installation directory
set KEY_NAME="HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%1"
set VALUE_NAME=InstalledDirectory

echo Search in Windows Register for : %KEY_NAME%

FOR /F "usebackq skip=2 tokens=1,2*" %%A IN (`reg query %KEY_NAME% /v %VALUE_NAME%`) DO (
	
    set UE4Path=%%C

)

if defined UE4Path (
    echo Installation found at : %UE4Path%
	goto lb_start_build
) else (
    echo Unreal Engine version %1 installation cannot be found!
	goto lb_exit
)

rem Build!
:lb_start_build
echo
echo == Start Build... ==
echo
echo Use UnrealEngine UE4_PATH environment variable : %UE4Path%
"%UE4Path%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CD%\..\Plugins\RPRPlugin\RPRPlugin.uplugin" -nop4 -utf8output -pak -distribution -compile -package="%CD%\PluginStaging\UE4_%1\RPRPlugin" -Rocket -TargetPlatforms=Win64
echo
echo == Build completed ==
echo
goto lb_exit

:lb_exit
pause