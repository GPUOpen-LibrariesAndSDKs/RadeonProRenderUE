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
            PublicIncludePaths.Add(Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/radeonimagefilters-1.4.4-778df0-Windows-rel/include"));
            AddWindowsStaticLibraries(Target);
            AddWindowsDynamicLibraries(Target);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicIncludePaths.Add(Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/radeonimagefilters-1.4.4-778df0-Ubuntu18-rel/include"));
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
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/radeonimagefilters-1.4.4-778df0-Ubuntu18-rel/bin");

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
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/radeonimagefilters-1.4.4-778df0-Windows-rel/bin");

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
        string imageProcessingLibPath = Path.Combine(ThirdPartyDirectory, @"RadeonProImageProcessingSDK/radeonimagefilters-1.4.4-778df0-Windows-rel/bin");

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
