// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// Primary editor module on which any other module can depend.
/// This module only provide tools that will *never* need other RPR dependencies.
/// </summary>
public class RPRRenderGLTFScene : ModuleRules
{
	public RPRRenderGLTFScene(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRRendering/RenderGLTFScene/Public",
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRRendering/RenderGLTFScene/Private",
                // ... add other private include paths required here ..
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
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
				// ... add private dependencies that you statically link with here ...	
			}
        );
    }
    
}
