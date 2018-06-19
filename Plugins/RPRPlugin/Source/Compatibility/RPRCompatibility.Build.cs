// Some copyright should be here...

using UnrealBuildTool;

public class RPRCompatibility : ModuleRules
{
	public RPRCompatibility(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        DefineIncludesAndDependencies(Target);
    }

    void DefineIncludesAndDependencies(ReadOnlyTargetRules Target)
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "Compatibility/Public",
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "Compatibility/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
				
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine"
				// ... add private dependencies that you statically link with here ...	
			}
        );

        if (Target.bBuildEditor)
        {
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "UnrealEd"
                }
            );
        }
    }
    
}
