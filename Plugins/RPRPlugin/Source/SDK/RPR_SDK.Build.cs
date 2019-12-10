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
    static string[] StaticLibraryNames = new string[]
    {
        "RadeonProRender",
        "RprLoadStore",
        "Tahoe",
    };

    static string[] DynamicLibraryNames = new string[]
    {
        "RadeonProRender",
        "RprLoadStore",
        "Tahoe",
    };

    public string ThirdPartyDirectory
    { get { return ("../../ThirdParty/"); } }

    public string SDKDirectory
    { get { return (ThirdPartyDirectory + "RadeonProRenderSDK/"); } }


    public RPR_SDK(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableExceptions = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
       
     	PrivateIncludePaths.AddRange(
			new string[] {
                "SDK/Public",
                "SDK/Private",
            });


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                 "Core",
                 "RPRPluginVersion"
            });
        
        AddRPRIncludes(PublicIncludePaths);

        AddRPRStaticLibraries(Target);
        AddDynamicLibraries(Target);
    }

    public void AddDynamicLibraries(ReadOnlyTargetRules Target)
    {
        string libExtension = GetDynamicLibraryExtensionByPlatform(Target.Platform);

        string librarySuffix;
        string platformName;
        if (!GetPlatformDatas(Target.Platform, out platformName, out librarySuffix))
        {
            return;
        }
        
        string libPath = ModuleDirectory + "/" + SDKDirectory + "RadeonProRender/bin" + platformName;
        if (!Directory.Exists(libPath))
        {
            Console.WriteLine("Dynamic library directory doesn't exist ! " + libPath);
            return;
        }

        PublicLibraryPaths.Add(libPath);

        for (int i = 0; i < DynamicLibraryNames.Length; ++i)
        {
            string filename = DynamicLibraryNames[i] + librarySuffix + libExtension;
            string srcPath = filename;

            PublicDelayLoadDLLs.Add(srcPath);
        }
    }

    public void AddRPRIncludes(List<string> IncludePaths)
    {
        IncludePaths.AddRange(new string[] {
            ModuleDirectory + "/" + SDKDirectory + "RadeonProRender",
            ModuleDirectory + "/" + SDKDirectory + "RadeonProRender/inc",
        });
    }

    public void AddRPRStaticLibraries(ReadOnlyTargetRules Target)
    {
        string libExtension = GetStaticLibraryExtensionByPlatform(Target.Platform);

        string librarySuffix;
        string platformName;
        if (!GetPlatformDatas(Target.Platform, out platformName, out librarySuffix))
        {
            return;
        }
        
        string librariesDirectoryPath = ModuleDirectory + "/" + SDKDirectory + "RadeonProRender/lib" + platformName + "/";

        for (int i = 0; i < StaticLibraryNames.Length; ++i)
        {
            PublicAdditionalLibraries.Add(librariesDirectoryPath + StaticLibraryNames[i] + librarySuffix + libExtension);
        }
    }

    public static string GetStaticLibraryExtensionByPlatform(UnrealTargetPlatform Platform)
    {
        switch (Platform)
        {
            case UnrealTargetPlatform.Win64:
            case UnrealTargetPlatform.Win32:
                return (".lib");

            case UnrealTargetPlatform.Linux:
            case UnrealTargetPlatform.Mac:
                return (".a");

            default:
                Console.WriteLine("Platform '{0}' not supported", Platform);
                return (string.Empty);
        }
    }

    public static string GetDynamicLibraryExtensionByPlatform(UnrealTargetPlatform Platform)
    {
        switch (Platform)
        {
            case UnrealTargetPlatform.Win64:
            case UnrealTargetPlatform.Win32:
                return (".dll");

            case UnrealTargetPlatform.Linux:
            case UnrealTargetPlatform.Mac:
                return (".so");

            default:
                Console.WriteLine("Platform '{0}' not supported", Platform);
                return (string.Empty);
        }
    }

    public static bool GetPlatformDatas(UnrealTargetPlatform Platform, out string platformName, out string librarySuffix)
    {
        switch (Platform)
        {
            case UnrealTargetPlatform.Win64:
                platformName = "Win64";
                librarySuffix = "64";
                return (true);

            default:
                Console.WriteLine("warning: Platform '{0}' not supported!", Platform);
                platformName = string.Empty;
                librarySuffix = string.Empty;
                return (false);
        }
    }
}
