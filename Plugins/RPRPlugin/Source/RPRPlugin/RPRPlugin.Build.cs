/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

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

        PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Public",

                "RPRPlugin/Public/Scene",
                "RPRPlugin/Public/Scene/StaticMeshComponent",

				// ... add public include paths required here ...
			}
			);

        //bFasterWithoutUnity = true;

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
            }
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RPR_SDK",
                "RPRTools",
                "RPRCore",
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

        PublicIncludePathModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
            );


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
                    "RPR_GLTF"
				});
        }

	}
}
