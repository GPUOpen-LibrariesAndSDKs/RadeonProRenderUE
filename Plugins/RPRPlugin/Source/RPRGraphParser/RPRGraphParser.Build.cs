// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using UnrealBuildTool;

/// <summary>
/// Primary runtime module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPRGraphParser : ModuleRules
{
	public RPRGraphParser(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
                "RPRGraphParser/Public",
            }
			);

        PrivateIncludePaths.AddRange(
			new string[] {
                "RPRGraphParser/Private",				
            }
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "RPRPlugin"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
                "Engine",

				// ... add private dependencies that you statically link with here ...	
			});
    }
}
