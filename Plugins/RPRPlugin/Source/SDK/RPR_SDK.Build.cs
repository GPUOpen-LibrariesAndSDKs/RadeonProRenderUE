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
using System.Collections.Generic;
using UnrealBuildTool;

/// <summary>
/// Primary runtime module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPR_SDK : ModuleRules
{
    static string[] WindowsStaticLibraryNames = new string[]
    {
        "RadeonProRender64.lib",
        "RprLoadStore64.lib",
        "RadeonImageFilters64.lib",
        "ProRenderGLTF.lib"
    };

    static string[] WindowsDynamicLibraryNames = new string[]
    {
        "RadeonProRender64.dll",
        "RprLoadStore64.dll",
        "Tahoe64.dll",
        "Hybrid.dll",
        "RadeonImageFilters64.dll",
        "ProRenderGLTF.dll"
    };

    static string[] LinuxDynamicLibraries = new string[]
    {
        "libRadeonProRender64.so",
        "libRprLoadStore64.so",
        "libRadeonImageFilters64.so",
        "Hybrid.so",
    };

    public string ThirdPartyDirectory
    { get { return Path.GetFullPath(ModuleDirectory + @"/../../ThirdParty"); } }

    public string RPR_SDK_Directory
    { get { return Path.Combine(ThirdPartyDirectory, @"RadeonProRenderSDK"); } }


    public RPR_SDK(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableExceptions = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[] {
            "SDK/Public",
            "SDK/Private",
        });


        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "RPRPluginVersion"
        });

        PublicIncludePaths.AddRange(new string[] {
            Path.Combine(RPR_SDK_Directory, @"RadeonProRender"),
            Path.Combine(RPR_SDK_Directory, @"RadeonProRender/inc"),
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/include"));
            AddWindowsStaticLibraries(Target);
            AddWindowsDynamicLibraries(Target);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicIncludePaths.Add(Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/include"));
            AddLinuxDynamicLibraries(Target);
        }
        else
        {
            Console.WriteLine("warning: Platform '{0}' not supported!", Target.Platform);
        }
    }

    private void AddLinuxDynamicLibraries(ReadOnlyTargetRules Target)
    {
        // for Linux/Mac plugin links only with *.so files at compile time.
        string rprLibPath = Path.Combine(RPR_SDK_Directory, @"RadeonProRender/binUbuntu18");
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/Ubuntu18");

        if (!CheckDirectory(rprLibPath))
            return;

        if (!CheckDirectory(imageProcessingLibPath))
            return;

        PublicLibraryPaths.Add(rprLibPath);
        PublicLibraryPaths.Add(imageProcessingLibPath);

        for (int i = 0; i < LinuxDynamicLibraries.Length; ++i)
            PublicAdditionalLibraries.Add(Path.Combine(rprLibPath, LinuxDynamicLibraries[i]));
    }

    public void AddWindowsDynamicLibraries(ReadOnlyTargetRules Target)
    {
        string rprLibPath = Path.Combine(RPR_SDK_Directory, @"RadeonProRender/binWin64");
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/Windows");

        if (!CheckDirectory(rprLibPath))
            return;

        if (!CheckDirectory(imageProcessingLibPath))
            return;

        PublicLibraryPaths.Add(rprLibPath);
        PublicLibraryPaths.Add(imageProcessingLibPath);

        for (int i = 0; i < WindowsDynamicLibraryNames.Length; ++i)
            PublicDelayLoadDLLs.Add(WindowsDynamicLibraryNames[i]);
    }

    public void AddWindowsStaticLibraries(ReadOnlyTargetRules Target)
    {
        string rprLibPath = Path.Combine(RPR_SDK_Directory, @"RadeonProRender/libWin64");
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/Windows");

        if (!CheckDirectory(rprLibPath))
            return;

        if (!CheckDirectory(imageProcessingLibPath))
            return;

        PublicLibraryPaths.Add(rprLibPath);
        PublicLibraryPaths.Add(imageProcessingLibPath);

        for (int i = 0; i < WindowsStaticLibraryNames.Length; ++i)
            PublicAdditionalLibraries.Add(WindowsStaticLibraryNames[i]);
    }

    private bool CheckDirectory(string path)
    {
        if (!Directory.Exists(path))
        {
            Console.WriteLine("Dynamic library directory doesn't exist ! " + path);
            return false;
        }

        return true;
    }
}
