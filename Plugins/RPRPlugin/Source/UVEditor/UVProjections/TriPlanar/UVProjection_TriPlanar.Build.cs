// Some copyright should be here...

using UnrealBuildTool;

public class UVProjection_TriPlanar : ModuleRules
{
	public UVProjection_TriPlanar(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "UVProjections/TriPlanar/Public",
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "UVProjections/TriPlanar/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
				"RPRPlugin"
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
                "RawMesh",
                "EditorStyle",
                "RPRPluginEditor",
                "RPREditorTools"
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
    
}
