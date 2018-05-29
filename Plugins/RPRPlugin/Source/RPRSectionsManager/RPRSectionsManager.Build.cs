// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// Provides the tools for the sections editing in the RPR Static Mesh Editor
/// </summary>
public class RPRSectionsManager : ModuleRules
{
	public RPRSectionsManager(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRSectionsManager/Public",
				// ... add public include paths required here ...
                
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRSectionsManager/Private",
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "RPREditorTools", "MeshPaint"
				
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
                "PropertyEditor",
                "EditorStyle",
                "RenderCore"
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
}
