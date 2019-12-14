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
/// Module for all the RPRMaterial editor part (editor-only).
/// </summary>
public class RPRMaterialEditor : ModuleRules
{
	public RPRMaterialEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "Runtime/Slate/Private"
            }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRMaterialEditor/Public",

                "RPRMaterialEditor/Public/Assets",
                "RPRMaterialEditor/Public/Assets/Factories",
                "RPRMaterialEditor/Public/Assets/AssetTypeActions",

                "RPRMaterialEditor/Public/RPRMaterialEditor",
                "RPRMaterialEditor/Public/RPRMaterialEditor/PropertiesLayout",
                "RPRMaterialEditor/Public/RPRMaterialEditor/RPRMaterialParamCopiers",
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRMaterialEditor/Private",

                "RPRMaterialEditor/Private/Tools/PropertyHelper",

                "RPRMaterialEditor/Private/RPRMaterialEditor",
                "RPRMaterialEditor/Public/RPRMaterialEditor/PropertiesLayout",

                "RPRMaterialEditor/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPR_SDK",
                "RPRCore",
                "RPRPlugin",
                //"RPRMaterialImporter",
                "RPREditorTools",
                "RPRTools",
				
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
                "Slate",
                "LevelEditor",
                "Settings",
                "PropertyEditor",
                "MaterialEditor",
                "AssetTools",
                "AppFramework"
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
}
