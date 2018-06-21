// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using System.Collections.Generic;
using UnrealBuildTool;

/// <summary>
/// Main runtime plugin
/// </summary>
public class RPRPlugin : ModuleRules
{

    public RPRPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;

        PublicIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Public",
                "RPRPlugin/Public/SDK",
                "RPRPlugin/Public/Enums",

                "RPRPlugin/Public/Material",
                "RPRPlugin/Public/Material/Tools",

                "RPRPlugin/Public/Scene",
                "RPRPlugin/Public/Scene/StaticMeshComponent",

				// ... add public include paths required here ...
			}
			);

        //bFasterWithoutUnity = true;

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
                "RPRPlugin/Private/Material",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/Factory",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterArgs",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Bool",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Enum",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoM",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoMChannel1",
            }
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RPR_SDK",
                "RPRTools",
                "RPRImageManager",
                "RPRCompatibility",
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

	}
}
