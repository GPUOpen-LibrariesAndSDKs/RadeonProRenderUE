// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

/// <summary>
/// Main runtime plugin
/// </summary>
public class RPRPlugin : ModuleRules
{
	public RPRPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Public",
                "RPRPlugin/Public/Enums",

                "RPRPlugin/Public/Material",
                "RPRPlugin/Public/Material/Tools",

                "RPRPlugin/Public/Scene",
                "RPRPlugin/Public/Scene/StaticMeshComponent",
				// ... add public include paths required here ...
			}
			);

		//bFasterWithoutUnity = true;

		string pluginRoot = ModuleDirectory + "/../..";
		pluginRoot = Path.GetFullPath(pluginRoot);
		pluginRoot = Utils.CleanDirectorySeparators(pluginRoot, '/') + "/";

		string SDKRoot = pluginRoot + "/ProRenderSDK/";

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",

                SDKRoot + "RadeonProRender",
				SDKRoot + "RadeonProRender/inc",
				SDKRoot + "RadeonProRenderInterchange/include",

                "RPRPlugin/Private/Material",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/Factory",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterArgs",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Bool",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Enum",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialConstantOrMap",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialConstantOrMapChannel1",
            }
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RPRTools",
                "RPRImageManager",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RenderCore",
				"CinematicCamera",
				"RHI",

				// ... add private dependencies that you statically link with here ...	
			});

		if (Target.bBuildEditor == true)
		{
			//Definitions.Add("MY_WITH_EDITOR=1");
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"InputCore",
					"DesktopPlatform",
					"WorkspaceMenuStructure",
					"EditorStyle",
					"Slate",
					"LevelEditor",
					"Settings",
				});
		}

		bool forceRelease = false;
		string libSuffix = ".lib";
		if (Target.Configuration == UnrealTargetConfiguration.DebugGame && !forceRelease)
			libSuffix = ".lib";

		// TODO: Modify this so it is multi platform or throw errors on non supported platforms
		PublicAdditionalLibraries.AddRange(new string[]
		{
			SDKRoot + "RadeonProRender/libWin64/RadeonProRender64.lib",
			SDKRoot + "RadeonProRender/libWin64/RprLoadStore64.lib",
			SDKRoot + "RadeonProRender/libWin64/RprSupport64.lib",
			SDKRoot + "RadeonProRender/libWin64/Tahoe64.lib",
			SDKRoot + "RadeonProRenderInterchange/libWin64/RadeonProRenderInterchange64" + libSuffix,
		});

		// TODO: This WONT work when plugin is installed in the engine plugins folder, fix that
		string gameBinDir = pluginRoot + "/../../Binaries/" + Target.Platform.ToString() + "/";
		if (!Directory.Exists(gameBinDir))
			Directory.CreateDirectory(gameBinDir);

		File.Copy(SDKRoot + "RadeonProRender/binWin64/OpenImageIO_RPR.dll", gameBinDir + "OpenImageIO_RPR.dll", true);
		File.Copy(SDKRoot + "RadeonProRender/binWin64/RadeonProRender64.dll", gameBinDir + "RadeonProRender64.dll", true);
		File.Copy(SDKRoot + "RadeonProRender/binWin64/RprLoadStore64.dll", gameBinDir + "RprLoadStore64.dll", true);
		File.Copy(SDKRoot + "RadeonProRender/binWin64/RprSupport64.dll", gameBinDir + "RprSupport64.dll", true);
		File.Copy(SDKRoot + "RadeonProRender/binWin64/Tahoe64.dll", gameBinDir + "Tahoe64.dll", true);
	}
}
