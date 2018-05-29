// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// Primary editor module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
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
                "RPREditorTools/Public/RPRMeshData",
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPREditorTools/Private",
                // ... add other private include paths required here ...

                "RPREditorTools/Private/Math",
                "RPREditorTools/Private/RPRMeshData",
                "RPREditorTools/Private/FaceAssignationHelper",
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RawMesh",
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
                "InputCore",
                "EditorStyle",
                "Slate",
                "LevelEditor",
                "Settings",
                "StaticMeshEditor",
                "PropertyEditor",
                "AdvancedPreviewScene",
                "RHI",
                "RenderCore",
                "ShaderCore",
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
    
}
