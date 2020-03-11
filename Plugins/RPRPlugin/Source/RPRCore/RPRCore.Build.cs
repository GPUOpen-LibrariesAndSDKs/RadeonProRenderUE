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

namespace UnrealBuildTool.Rules
{
	public class RPRCore : ModuleRules
	{
		public RPRCore(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
            bEnableExceptions = true;

            PrivateIncludePaths.AddRange(
				new string[] {
                    "RPRCore/Public/Material/Tools/MaterialCacheMaker",

                    "RPRCore/Private/Material",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/Factory",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Bool",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Enum",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoM",
                    "RPRCore/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoMChannel1",

                }
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
                    "RPRPluginVersion",
                    "RPR_SDK",
                    "RPRTools",
				}
				);

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "RenderCore",
                    "CoreUObject",
                    "Engine",
                }
                );
		}
	}
}
