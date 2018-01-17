#include "RPRMaterialFactory.h"
#include "RPRPlugin.h"
#include "Editor.h"
#include "XmlFile.h"
#include "RPRSettings.h"
#include "FeedbackContext.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRPlugin/Public/Assets/RPRMaterial.h"

URPRMaterialFactory::URPRMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = true;
	bText = true;
	bEditorImport = true;
	
	SupportedClass = URPRMaterial::StaticClass();

	Formats.Add(TEXT("rprmat;Radeon Pro Render Material"));
}

UObject* URPRMaterialFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, 
												FName InName, EObjectFlags Flags, 
												const FString& Filename, const TCHAR* Parms, 
												FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();

	if (rprSettings == nullptr)
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	UMaterialInterface* uberMaterial = rprSettings->UberMaterial.LoadSynchronous();
	if (uberMaterial == nullptr)
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	URPRMaterial* uRPRMaterial = NewObject<URPRMaterial>(InParent, InClass, InName, Flags);
	
	uRPRMaterial->SetParentEditorOnly(uberMaterial);
	uRPRMaterial->PostEditChange();

	if (!LoadRPRMaterialFromXmlFile(uRPRMaterial, Filename))
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, uRPRMaterial);

	return (uRPRMaterial);
}

bool URPRMaterialFactory::LoadRPRMaterialFromXmlFile(URPRMaterial* RPRMaterial, const FString& Filename)
{
	FXmlFile xmlFile(Filename);

	if (!xmlFile.IsValid())
	{
		//UE_LOG(LogRPRPluginEditor, Error, TEXT("%s"), *xmlFile.GetLastError());
		return (false);
	}

	RPRMaterial->SetMaterialFilePath(Filename);

	// Get root node
	const FXmlNode* materialNode = xmlFile.GetRootNode();
	if (materialNode)
	{
		FRPRMaterialXmlGraph materialXmlGraph;
		if (!materialXmlGraph.ParseFromXml(*materialNode))
		{
			//UE_LOG(LogRPRPluginEditor, Error, TEXT("Could not parse correctly the Xml file!"));
			return (false);
		}

		LoadRPRMaterialParameter(RPRMaterial, materialXmlGraph, Filename);
	}

	return (true);
}

void URPRMaterialFactory::LoadRPRMaterialParameter(URPRMaterial* RPRMaterial, FRPRMaterialXmlGraph& MaterialXmlGraph, const FString& Filename)
{
	FRPRUberMaterialParameters& materialParameters = RPRMaterial->MaterialParameters;

	FRPRMaterialXmlNode* materialXmlNode = MaterialXmlGraph.GetFirstMaterial();
	if (materialXmlNode != nullptr)
	{
		FRPRMaterialNodeSerializationContext serializationContext;
		serializationContext.bIsLoading = true;
		serializationContext.ImportedFilePath = Filename;
		serializationContext.MaterialParameters = &RPRMaterial->MaterialParameters;
		serializationContext.MaterialXmlGraph = &MaterialXmlGraph;

		materialXmlNode->Serialize(serializationContext);
	}

	RPRMaterial->ReloadMaterialParametersToMaterial();
}