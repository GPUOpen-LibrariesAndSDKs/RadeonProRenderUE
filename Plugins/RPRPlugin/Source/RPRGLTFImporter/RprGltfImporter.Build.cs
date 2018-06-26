//~ RPR copyright

using UnrealBuildTool;
using System.IO;

public class RPRGLTFImporter : ModuleRules
{
    public RPRGLTFImporter(ReadOnlyTargetRules TargetRules) : base(TargetRules)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        PrivateIncludePaths.AddRange(
            new string[] {
                "RPRGLTFImporter/Private",
                "RPRGLTFImporter/Private/MaterialGraph",
                "RPRGLTFImporter/Private/MaterialGraph/NodeParamTypes",
        });

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "RPR_GLTF",
                "RPRGraphParser",
                "RPRPlugin",
                "RPRTools",
        });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "AssetRegistry",
                "EditorStyle",
                "Engine",
                "InputCore",
                "MainFrame",
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "RenderCore",
                "RawMesh",
                "UnrealEd",
        });
		
		string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
		string ThirdPartyDir = Path.Combine(PluginDir, "ThirdParty");
		string SDKDir = Path.Combine(PluginDir, "ProRenderSDK");
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ThirdPartyDir, "json", "include"),
				Path.Combine(SDKDir, "RadeonProRender", "inc"),
		});
		

        MinFilesUsingPrecompiledHeaderOverride = 1;
    }
}
