// Some copyright should be here...

using UnrealBuildTool;
using System;
using System.IO;

public class RPR_GLTF : ModuleRules
{
    public string ThirdPartyDirectory
    {
        get
        {
            return (Path.Combine(ModuleDirectory, "ThirdParty"));
        }
    }

    public string ThirdPartyBinDirectory
    {
        get
        {
            return (Path.Combine(ThirdPartyDirectory, "Binaries"));
        }
    }

    public string ThirdPartyIncludesDirectory
    {
        get
        {
            return (Path.Combine(ThirdPartyDirectory, "Includes"));
        }
    }


    public RPR_GLTF(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        DefineIncludesAndDependencies(Target);
        DefineIncludesAndDependenciesForThirdParty(Target);
    }

    void DefineIncludesAndDependencies(ReadOnlyTargetRules Target)
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "GLTF/Public",
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "GLTF/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPRTools",
                "RPR_SDK",
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
