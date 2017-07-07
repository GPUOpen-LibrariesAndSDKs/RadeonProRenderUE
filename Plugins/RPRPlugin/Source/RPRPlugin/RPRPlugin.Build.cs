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
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
				// ... add other private include paths required here ...
				"RPRPlugin/ThirdParty/RadeonProRender/inc"
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

				"WorkspaceMenuStructure",
				"EditorStyle",
				"Slate",
				"LevelEditor",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
			

                //Console.WriteLine ( Path.GetFullPath ( Path.Combine ( ModuleDirectory, "RPRPlugin/ThirdParty/RadeonProRender/libWin64/" ) ) );
		PublicAdditionalLibraries.Add (
                  Path.Combine ( Path.GetFullPath ( Path.Combine ( ModuleDirectory,
                                                                   "ThirdParty/RadeonProRender/libWin64/" ) ),
                                 "RadeonProRender64.lib" )
                  );
		PublicAdditionalLibraries.Add ( Path.Combine ( Path.GetFullPath ( Path.Combine ( ModuleDirectory,
                  "ThirdParty/RadeonProRender/libWin64/" ) ), "RprLoadStore64.lib" ) );
		PublicAdditionalLibraries.Add ( Path.Combine ( Path.GetFullPath ( Path.Combine ( ModuleDirectory,
                  "ThirdParty/RadeonProRender/libWin64/" ) ), "RprSupport64.lib" ) );
		PublicAdditionalLibraries.Add ( Path.Combine ( Path.GetFullPath ( Path.Combine ( ModuleDirectory,
                  "ThirdParty/RadeonProRender/libWin64/" ) ), "Tahoe64.lib" ) );
                  
                  
                //RuntimeDependencies.Add ( "RadeonProRender64.dll" );
                //RuntimeDependencies.Add ( "OpenImageIO_RPR.dll" );
                //RuntimeDependencies.Add ( "RprLoadStore64.dll" );
                //RuntimeDependencies.Add ( "RprSupport64.dll" );
                //RuntimeDependencies.Add ( "Tahoe64.dll" );
			
	}
}
