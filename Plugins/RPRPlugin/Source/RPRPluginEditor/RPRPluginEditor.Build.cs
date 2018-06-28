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

using UnrealBuildTool;

/// <summary>
/// The editor version of the RPR Plugin module. Gathers the other editor module and use them.
/// </summary>
public class RPRPluginEditor : ModuleRules
{
	public RPRPluginEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRPluginEditor/Public",
                "RPRPlugin/Public",
				// ... add public include paths required here ...

                "RPRPlugin/Public/UVMappingEditor",
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRPluginEditor/Private",

                "RPRPluginEditor/Private/ShapePreviews",
                "RPRPluginEditor/Private/RPRViewport",

                "RPRPluginEditor/Private/Material/",
                "RPRPluginEditor/Private/Material/NodeParamTypes",
                
                "RPRPluginEditor/Private/Outliners",
                "RPRPluginEditor/Private/Outliners/ObjectsOutliner",
                "RPRPluginEditor/Private/Outliners/SceneOutliner",

                "RPRPlugin/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPR_SDK",
                "RPRPlugin",
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
                "UnrealEd",
                "InputCore",
                "EditorStyle",
                "LevelEditor",
                "Settings",
                "StaticMeshEditor",
                "PropertyEditor",
                "AdvancedPreviewScene",
                "RenderCore",
                "RHI",
                "RawMesh",
                "RPREditorTools",
                "UVVisualizer",
                "RPRSectionsManager",
                "Outliners"
				// ... add private dependencies that you statically link with here ...	
			}
        );

    }
}
