// Some copyright should be here...

using UnrealBuildTool;

public class RPREditorTools : ModuleRules
{
	public RPREditorTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPREditorTools/Public",
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPREditorTools/Private",
                // ... add other private include paths required here ...

                "RPREditorTools/Private/Math",
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ProceduralMeshComponent"
				
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
                "StaticMeshEditor",
                "PropertyEditor",
                "AdvancedPreviewScene",
                "RenderCore",
                "RawMesh",
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
    
}
