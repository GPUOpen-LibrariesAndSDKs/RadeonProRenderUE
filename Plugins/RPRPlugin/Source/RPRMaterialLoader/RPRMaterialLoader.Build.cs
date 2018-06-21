// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// Module allowing to import RPR materials
/// </summary>
public class RPRMaterialLoader : ModuleRules
{
	public RPRMaterialLoader(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRMaterialLoader/Public",
                "RPRPlugin/Public",    
                            
                "Runtime/Slate/Private"
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRMaterialLoader/Private",                
                "RPRMaterialLoader/Private/Node",
                "RPRMaterialLoader/Private/Node/Factory",
                "RPRMaterialLoader/Private/NodeParamTypes",
			}
            );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPR_SDK",
                "RPRPlugin",
                "RPRTools",
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "PropertyEditor",
                "XmlParser",
                "RPRMaterialEditor",
			}
        );
    }
}

