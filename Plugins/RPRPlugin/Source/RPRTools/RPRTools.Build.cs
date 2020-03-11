/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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

        PrivateIncludePaths.AddRange(
			new string[] {
				"RPRTools/Public",
                "RPRTools/Private",
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty/gli"),
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty/glm"),
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty"),
            }
			);

        PublicIncludePaths.AddRange(
            new string[] {
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty/gli"),
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty/glm"),
                System.IO.Path.Combine(ModuleDirectory, @"../../ThirdParty"),
            }
            );

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "RPR_SDK"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
                "Engine",
                "RenderCore",
				// ... add private dependencies that you statically link with here ...
			});
    }
}
