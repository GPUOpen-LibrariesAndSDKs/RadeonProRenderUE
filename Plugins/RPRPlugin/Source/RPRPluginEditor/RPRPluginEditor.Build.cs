// Some copyright should be here...

using UnrealBuildTool;

/// <summary>
/// The editor version of the RPR Plugin module. Gathers the other editor module and use them.
/// </summary>
public class RPRPluginEditor : ModuleRules
{
	public RPRPluginEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        DefineIncludesAndDependencies();
    }

    void DefineIncludesAndDependencies()
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "RPRPluginEditor/Public",
                "RPRPlugin/Public",
				// ... add public include paths required here ...

                "RPRPlugin/Public/UVMappingEditor",
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRPluginEditor/Private",

                "RPRPluginEditor/Private/ShapePreviews",
                "RPRPluginEditor/Private/RPRViewport",

                "RPRPluginEditor/Private/Material/",
                "RPRPluginEditor/Private/Material/NodeParamTypes",
                
                "RPRPluginEditor/Private/Outliners",
                "RPRPluginEditor/Private/Outliners/ObjectsOutliner",
                "RPRPluginEditor/Private/Outliners/SceneOutliner",

                "RPRPlugin/Private"
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RPR_SDK",
                "RPRPlugin",
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
                "LevelEditor",
                "Settings",
                "StaticMeshEditor",
                "PropertyEditor",
                "AdvancedPreviewScene",
                "RenderCore",
                "RHI",
                "RawMesh",
                "RPREditorTools",
                "UVVisualizer",
                "RPRSectionsManager",
                "Outliners"
				// ... add private dependencies that you statically link with here ...	
			}
        );

    }
}
