@echo off

set ue4version=419

echo Build...
echo Use UnrealEngine UE4_PATH environment variable : %UE4_Path%
"%UE4_Path%\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="%CD%\..\Plugins\RPRPlugin\RPRPlugin.uplugin" -nop4 -utf8output -pak -distribution -compile -package="%CD%\PluginStaging_ALL\UE4_%ue4version%\RPRPlugin" -Rocket
echo Build completed.

pause