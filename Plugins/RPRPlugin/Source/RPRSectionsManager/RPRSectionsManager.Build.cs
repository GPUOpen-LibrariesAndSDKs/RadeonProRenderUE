// Some copyright should be here...

using UnrealBuildTool;

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
                "Core", "RPREditorTools"
				
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
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
}
