// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

public class RPRImageManager : ModuleRules
{
	public RPRImageManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"RPRImagerManager/Public",
                "RPRImagerManager/Public/Cache",
            }
			);

		//bFasterWithoutUnity = true;

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
            });

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RPRTools"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
                "Engine"
				// ... add private dependencies that you statically link with here ...	
			});

		if (Target.bBuildEditor == true)
		{
			//Definitions.Add("MY_WITH_EDITOR=1");
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd"
				});
        }

        RPRPlugin.AddRPRIncludes(ModuleDirectory, PrivateIncludePaths);
        RPRPlugin.AddRPRStaticLibraries(ModuleDirectory, PublicAdditionalLibraries, Target);
	}
}
