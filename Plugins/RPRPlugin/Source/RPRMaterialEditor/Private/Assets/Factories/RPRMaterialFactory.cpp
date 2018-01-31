#include "RPRMaterialFactory.h"
#include "RPRPlugin.h"
#include "Editor.h"
#include "RPRSettings.h"
#include "FeedbackContext.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRPlugin/Public/Assets/RPRMaterial.h"
#include "RPRUberMaterialToMaterialInstanceCopier.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialFactory, Log, All)

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
	FRPRMaterialXmlGraph materialXmlGraph;
	if (!materialXmlGraph.ParseFromXmlFile(Filename))
	{
		UE_LOG(LogRPRMaterialFactory, Error, TEXT("Could not parse correctly the Xml file!"));
		return (false);
	}

	LoadRPRMaterialParameters(RPRMaterial, materialXmlGraph, Filename);
	CopyRPRMaterialParameterToMaterialInstance(RPRMaterial);

	return (true);
}

void URPRMaterialFactory::LoadRPRMaterialParameters(URPRMaterial* RPRMaterial, FRPRMaterialXmlGraph& MaterialXmlGraph, const FString& Filename)
{
	FRPRMaterialNodeSerializationContext serializationContext;
	serializationContext.ImportedFilePath = Filename;
	serializationContext.MaterialParameters = &RPRMaterial->MaterialParameters;
	serializationContext.MaterialXmlGraph = &MaterialXmlGraph;

	MaterialXmlGraph.LoadRPRMaterialParameters(serializationContext);
}

void URPRMaterialFactory::CopyRPRMaterialParameterToMaterialInstance(class URPRMaterial* RPRMaterial)
{
	// Create a temporary material editor instance constant so we can set the variable correctly as if we were changing the value in the editor
	URPRMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstantFrom(RPRMaterial);
	FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(RPRMaterial->MaterialParameters, MaterialEditorInstance);
	MaterialEditorInstance->CopyToSourceInstance();
	RPRMaterial->PostEditChange();
}

URPRMaterialEditorInstanceConstant* URPRMaterialFactory::CreateMaterialEditorInstanceConstantFrom(class URPRMaterial* RPRMaterial)
{
	URPRMaterialEditorInstanceConstant* MaterialEditorInstance = 
		NewObject<URPRMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);

	MaterialEditorInstance->SetSourceInstance(RPRMaterial);
	return (MaterialEditorInstance);
}
