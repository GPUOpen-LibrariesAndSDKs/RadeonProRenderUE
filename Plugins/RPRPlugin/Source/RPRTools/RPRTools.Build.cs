// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

/// <summary>
/// Primary runtime module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPRTools : ModuleRules
{
	public RPRTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"RPRTools/Public",
                "RPRTools/Public/Helpers",
                "RPRTools/Public/Typedefs",
                "RPRTools/Public/Enums",
            }
			);

        PrivateIncludePaths.AddRange(
			new string[] {
                "RPRTools/Private",
				
				
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

				// ... add private dependencies that you statically link with here ...	
			});

        RPRPlugin.AddRPRIncludes(ModuleDirectory, PrivateIncludePaths);
        RPRPlugin.AddRPRStaticLibraries(ModuleDirectory, PublicAdditionalLibraries, Target);
    }
}
