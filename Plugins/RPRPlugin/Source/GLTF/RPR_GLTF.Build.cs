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
using System;
using System.IO;

public class RPR_GLTF : ModuleRules
{
    public string ThirdPartyDirectory
    { get { return (Path.Combine(ModuleDirectory, "../../ThirdParty")); }}

    public string GLTFDirectory
    { get { return (Path.Combine(ThirdPartyDirectory, "gltf")); }}

    public string ThirdPartyBinDirectory
    { get { return (Path.Combine(GLTFDirectory, "Binaries")); }}

    public string ThirdPartyIncludesDirectory
    { get { return (Path.Combine(GLTFDirectory, "Includes")); }}


    public RPR_GLTF(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;

        DefineIncludesAndDependencies(Target);
        DefineIncludesAndDependenciesForThirdParty(Target);
    }

    void DefineIncludesAndDependencies(ReadOnlyTargetRules Target)
    {
        PrivateIncludePaths.AddRange(
            new string[] {
                "GLTF/Public",
                "GLTF/Public/gltf",
                "GLTF/Public/gltf/Extensions",
            }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
                "GLTF/Private",
                "GLTF/Private/gltf",
                "GLTF/Private/gltf/Extensions",
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPRPluginVersion",
                "RPRTools",
                "RPR_SDK",
                "RPR_Json",
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
				"RHI",
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }

    void DefineIncludesAndDependenciesForThirdParty(ReadOnlyTargetRules Target)
    {
        string gltfBinDir = Path.Combine(ThirdPartyBinDirectory, Target.Platform.ToString());
        
        string gltfStaLib = Path.Combine(gltfBinDir, "ProRenderGLTF" + RPR_SDK.GetStaticLibraryExtensionByPlatform(Target.Platform));
        PublicAdditionalLibraries.Add(gltfStaLib);

        string gltfDynLib = "ProRenderGLTF" + RPR_SDK.GetDynamicLibraryExtensionByPlatform(Target.Platform);
        PublicLibraryPaths.Add(gltfBinDir);
        PublicDelayLoadDLLs.Add(gltfDynLib);

        PrivateIncludePaths.Add(ThirdPartyIncludesDirectory);
    }

}
