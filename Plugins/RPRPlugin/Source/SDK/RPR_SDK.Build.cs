// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

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
        "RprSupport",
        "Tahoe",
    };

    static string[] DynamicLibraryNames = new string[]
    {
        "RadeonProRender",
        "RprLoadStore",
        "RprSupport",
        "Tahoe",
        "OpenImageIO_RPR",
    };

    public string ThirdPartyDirectory
    { get { return ("ThirdParty/"); } }

    public string SDKDirectory
    { get { return (ThirdPartyDirectory + "ProRenderSDK/"); } }


    public RPR_SDK(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
                "SDK/Public",
            });
        
     	PrivateIncludePaths.AddRange(
			new string[] {
                "SDK/Private",
            });


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                 "Core",
            });

        // AddRPRIncludes(PrivateIncludePaths);
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
        string moduleDir = ModuleDirectory.Replace("\\", "/") + "/";

        IncludePaths.AddRange(new string[] {
            moduleDir + SDKDirectory + "RprTools",
            moduleDir + SDKDirectory + "RadeonProRender",
            moduleDir + SDKDirectory + "RadeonProRender/inc",
            moduleDir + SDKDirectory + "RadeonProRenderInterchange/include",
        });

        Console.WriteLine("Add : " + (moduleDir + SDKDirectory + "RadeonProRender/inc"));
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

        PublicAdditionalLibraries.Add(string.Format("{0}RadeonProRenderInterchange/lib{1}/RadeonProRenderInterchange{2}{3}", ModuleDirectory + "/" + SDKDirectory, platformName, librarySuffix, libExtension));
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
