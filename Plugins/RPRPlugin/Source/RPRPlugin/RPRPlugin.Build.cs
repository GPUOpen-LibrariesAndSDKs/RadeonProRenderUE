// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

public class RPRPlugin : ModuleRules
{
	public RPRPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Public"
				// ... add public include paths required here ...
			}
			);


		string pluginRoot = ModuleDirectory + "/../..";
		pluginRoot = Path.GetFullPath(pluginRoot);
		pluginRoot = Utils.CleanDirectorySeparators(pluginRoot, '/') + "/";

		string SDKRoot = pluginRoot + "/ProRenderSDK/";

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
				SDKRoot + "RadeonProRender",
				SDKRoot + "RadeonProRender/inc",
				SDKRoot + "RadeonProRenderInterchange/include"
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
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
				"Settings",

				// Editor only stuff
				"UnrealEd",
				"InputCore",
				"DesktopPlatform",
				"WorkspaceMenuStructure",
				"EditorStyle",
				"Slate",
				"LevelEditor",
				// ... add private dependencies that you statically link with here ...	
			});

		bool forceRelease = false;
		string libSuffix = ".lib";
		if (Target.Configuration == UnrealTargetConfiguration.DebugGame && !forceRelease)
			libSuffix = "D.lib";

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
