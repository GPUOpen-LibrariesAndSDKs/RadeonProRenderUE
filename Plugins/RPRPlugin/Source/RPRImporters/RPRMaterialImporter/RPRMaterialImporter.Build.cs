// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// Module allowing to import RPR materials
/// </summary>
public class RPRMaterialImporter : ModuleRules
{
	public RPRMaterialImporter(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRImporters/RPRMaterialImporter/Public",
                "RPRPlugin/Public",    
                            
                "Runtime/Slate/Private"
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRImporters/RPRMaterialImporter/Private",
                "RPRImporters/RPRMaterialImporter/Private/Node",
                "RPRImporters/RPRMaterialImporter/Private/Node/Factory",
                "RPRImporters/RPRMaterialImporter/Private/NodeParamTypes",
			}
            );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPR_SDK",
                "RPRPlugin",
                "RPRTools",
                "RPRGraphParser",
                "XmlParser",
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "PropertyEditor",
                "RPRMaterialEditor",
			}
        );
    }
}

