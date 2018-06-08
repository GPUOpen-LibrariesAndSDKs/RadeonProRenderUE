#include "RPRMaterialFactory.h"
#include "RPRPlugin.h"
#include "Editor.h"
#include "RPRSettings.h"
#include "FeedbackContext.h"
#include "AssetTypeCategories.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRPlugin/Public/Assets/RPRMaterial.h"
#include "RPRUberMaterialToMaterialInstanceCopier.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RPRMaterialFactory"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialFactory, Log, All)

URPRMaterialFactory::URPRMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
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

	UMaterialInterface* uberMaterial = TryLoadUberMaterial(Warn);
	if (uberMaterial)
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	URPRMaterial* uRPRMaterial = NewObject<URPRMaterial>(InParent, InClass, InName, Flags);
	
	uRPRMaterial->SetParentEditorOnly(uberMaterial);
	uRPRMaterial->PostEditChange();

	if (!LoadRPRMaterialFromXmlFile(uRPRMaterial, Filename))
	{
		Warn->Serialize(*FString::Printf(TEXT("Cannot load the material from the xml '%s'"), *Filename), ELogVerbosity::Warning, NAME_None);
		bOutOperationCanceled = true;
		return (nullptr);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, uRPRMaterial);

	return (uRPRMaterial);
}

UObject* URPRMaterialFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMaterialInterface* uberMaterial = TryLoadUberMaterial(Warn);
	if (uberMaterial == nullptr)
	{
		return (nullptr);
	}

	URPRMaterial* material = NewObject<URPRMaterial>(InParent, InClass, InName, Flags);
	material->SetParentEditorOnly(uberMaterial);
	material->PostEditChange();
	CopyRPRMaterialParameterToMaterialInstance(material);

	return (material);
}

FText URPRMaterialFactory::GetDisplayName() const
{
	return (LOCTEXT("RPRMaterialAssetName", "ProRender Material"));
}

uint32 URPRMaterialFactory::GetMenuCategories() const
{
	return (EAssetTypeCategories::MaterialsAndTextures);
}

FText URPRMaterialFactory::GetToolTip() const
{
	return (LOCTEXT("RPRMaterialAssetTooltip", "Create a new RPR material that will be fully supported by ProRender"));
}

FString URPRMaterialFactory::GetDefaultNewAssetName() const
{
	return (TEXT("R_NewMaterial"));
}

FName URPRMaterialFactory::GetNewAssetThumbnailOverride() const
{
	return (TEXT("ClassThumbnail.Material"));
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

UMaterialInterface* URPRMaterialFactory::TryLoadUberMaterial(FFeedbackContext* Warn)
{
	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	check(rprSettings);

	UMaterialInterface* uberMaterial = rprSettings->UberMaterial.LoadSynchronous();
	if (uberMaterial == nullptr)
	{
		rprSettings->TryLoadUberMaterialFromDefaultLocation();
		uberMaterial = rprSettings->UberMaterial.LoadSynchronous();
		if (uberMaterial == nullptr)
		{
			Warn->Serialize(TEXT("No UberMaterial is set in the RPR settings. Open Project > RPR Settings and set the Ubermaterial."), ELogVerbosity::Warning, NAME_None);
			return (nullptr);
		}
		else
		{
			Warn->Serialize(TEXT("No UberMaterial set in the RPR settings. The UberMaterial from the default location will be used."), ELogVerbosity::Warning, NAME_None);
		}
	}
	return (uberMaterial);
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

#undef LOCTEXT_NAMESPACE