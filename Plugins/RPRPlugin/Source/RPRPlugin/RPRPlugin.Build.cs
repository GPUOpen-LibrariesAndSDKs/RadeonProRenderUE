// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using System; // Console.WriteLine("");
using System.IO;
using System.Collections.Generic;
using UnrealBuildTool;

/// <summary>
/// Main runtime plugin
/// </summary>
public class RPRPlugin : ModuleRules
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

    static string GetPluginRoot(string ModuleDirectory)
    {
        string pluginRoot = ModuleDirectory + "/../..";
        pluginRoot = Path.GetFullPath(pluginRoot);
        pluginRoot = Utils.CleanDirectorySeparators(pluginRoot, '/') + "/";
        return (pluginRoot);
    }

    static string GetSDKRoot(string ModuleDirectory)
    {
        string SDKRoot = GetPluginRoot(ModuleDirectory) + "ProRenderSDK/";
        return (SDKRoot);
    }

    public RPRPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableExceptions = true;

        PublicIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Public",
                "RPRPlugin/Public/Enums",

                "RPRPlugin/Public/Material",
                "RPRPlugin/Public/Material/Tools",

                "RPRPlugin/Public/Scene",
                "RPRPlugin/Public/Scene/StaticMeshComponent",
				// ... add public include paths required here ...
			}
			);

        //bFasterWithoutUnity = true;
        
        string SDKRoot = GetSDKRoot(ModuleDirectory);

		PrivateIncludePaths.AddRange(
			new string[] {
				"RPRPlugin/Private",
                "RPRPlugin/Private/Material",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/Factory",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterArgs",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Bool",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/Enum",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoM",
                "RPRPlugin/Private/Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoMChannel1",
            }
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "RPRTools",
                "RPRImageManager",
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
				});
        }

        AddRPRIncludes(ModuleDirectory, PrivateIncludePaths);
        AddRPRStaticLibraries(ModuleDirectory, PublicAdditionalLibraries, Target);
        
        AddDynamicLibraries(ModuleDirectory, PublicLibraryPaths, RuntimeDependencies, PublicDelayLoadDLLs, Target);
	}

    public static void AddDynamicLibraries(string ModuleDirectory, List<string> PublicLibraryPaths, RuntimeDependencyList RuntimeDependencies, List<string> PublicDelayLoadDLLs, ReadOnlyTargetRules Target)
    {
        string libExtension = GetDynamicLibraryExtensionByPlatform(Target.Platform);

        string librarySuffix;
        string platformName;
        if (!GetPlatformDatas(Target.Platform, out platformName, out librarySuffix))
        {
            return;
        }

        string SDKRoot = GetSDKRoot(ModuleDirectory);
        string pluginRoot = GetPluginRoot(ModuleDirectory);

        string libPath = string.Format("{0}RadeonProRender/bin{1}", SDKRoot, platformName);
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

    public static void AddRPRIncludes(string ModuleDirectory, List<string> Includes)
    {
        string SDKRoot = GetSDKRoot(ModuleDirectory);

        Includes.AddRange(new string[] {
            SDKRoot + "RadeonProRender",
            SDKRoot + "RadeonProRender/inc",
            SDKRoot + "RadeonProRenderInterchange/include",
        });
    }
    
    public static void AddRPRStaticLibraries(string ModuleDirectory, List<string> PublicAdditionalLibraries, ReadOnlyTargetRules Target)
    {
        string libExtension = GetStaticLibraryExtensionByPlatform(Target.Platform);

        string librarySuffix;
        string platformName;
        if (!GetPlatformDatas(Target.Platform, out platformName, out librarySuffix))
        {
            return;
        }

        string SDKRoot = GetSDKRoot(ModuleDirectory);
        string librariesDirectoryPath = SDKRoot + "RadeonProRender/lib" + platformName + "/";
        
        for (int i = 0; i < StaticLibraryNames.Length; ++i)
        {
            PublicAdditionalLibraries.Add(librariesDirectoryPath + StaticLibraryNames[i] + librarySuffix + libExtension);
        }

        PublicAdditionalLibraries.Add(string.Format("{0}RadeonProRenderInterchange/lib{1}/RadeonProRenderInterchange{2}{3}", SDKRoot, platformName, librarySuffix, libExtension));
    }

    static string GetStaticLibraryExtensionByPlatform(UnrealTargetPlatform Platform)
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

    static string GetDynamicLibraryExtensionByPlatform(UnrealTargetPlatform Platform)
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

    static bool GetPlatformDatas(UnrealTargetPlatform Platform, out string platformName, out string librarySuffix)
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
