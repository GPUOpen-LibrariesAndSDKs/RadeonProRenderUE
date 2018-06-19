#!/bin/sh

ue4version=418

echo Build...
echo Use UnrealEngine UE4_PATH environment variable : $UE4_Path
"$UE4_Path\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin="$PWD\..\Plugins\RPRPlugin\RPRPlugin.uplugin" -nop4 -utf8output -pak -distribution -compile -package="$PWD\PluginStaging\UE4_$ue4version\RPRPlugin" -Rocket
echo Build completed.