#include "Material/RPRXMaterial.h"
#include "Assets/RPRMaterial.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreErrorHelper.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRXHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRXMaterial, Log, Verbose)

RPR::FRPRXMaterial::FRPRXMaterial(const URPRMaterial* InUE4MaterialLink)
	: Material(nullptr)
	, UE4MaterialLink(InUE4MaterialLink)
{
	check(InUE4MaterialLink);

	auto rprxContext = IRPRCore::GetResources()->GetRPRXSupportContext();
	RPR::FResult status = RPRX::FMaterialHelpers::CreateMaterial(rprxContext, RPRX::EMaterialType::Uber, Material);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRXMaterial, Warning, 
			TEXT("Native RPRX Material could not be created for material '%s'"), 
			*UE4MaterialLink->GetName());
	}
}

RPR::FRPRXMaterial::~FRPRXMaterial()
{
	ReleaseResources();
}


const URPRMaterial* RPR::FRPRXMaterial::GetUE4MaterialLink() const
{
	return UE4MaterialLink.Get();
}

void RPR::FRPRXMaterial::AddImage(RPR::FImagePtr Image)
{
	Images.Add(Image);
}

void RPR::FRPRXMaterial::RemoveImage(RPR::FImagePtr Image)
{
	Images.Remove(Image);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterBool(RPRX::FParameter Parameter, bool Value)
{
	return SetMaterialParameterUInt(Parameter, Value ? 1 : 0);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterUInt(RPRX::FParameter Parameter, uint32 Value)
{
	return RPRX::FMaterialHelpers::SetMaterialParameterUInt(GetRprxContext(), Material, Parameter, Value);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterColor(RPRX::FParameter Parameter, const FLinearColor& Color)
{
	return RPRX::FMaterialHelpers::SetMaterialParameterColor(GetRprxContext(), Material, Parameter, Color);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterFloats(RPRX::FParameter Parameter, float x, float y, float z, float w)
{
	return RPRX::FMaterialHelpers::SetMaterialParameterFloats(GetRprxContext(), Material, Parameter, x, y, z, w);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterFloat(RPRX::FParameter Parameter, float Value)
{
	return RPRX::FMaterialHelpers::SetMaterialParameterFloat(GetRprxContext(), Material, Parameter, Value);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterNode(RPRX::FParameter Parameter, RPR::FMaterialNode MaterialNode)
{
	return RPRX::FMaterialHelpers::SetMaterialParameterNode(GetRprxContext(), Material, Parameter, MaterialNode);
}

RPR::FResult RPR::FRPRXMaterial::Commit()
{
	return RPRX::MaterialCommit(GetRprxContext(), Material);
}

void RPR::FRPRXMaterial::RemoveImage(RPR::FImage Image)
{
	Images.RemoveAll([Image] (RPR::FImagePtr imagePtr)
	{
		return imagePtr.IsValid() && (imagePtr.Get() == Image);
	});
}

bool RPR::FRPRXMaterial::IsMaterialValid() const
{
	return Material != nullptr && UE4MaterialLink.IsValid();
}

const RPRX::FMaterial& RPR::FRPRXMaterial::GetRawMaterial() const
{
	return Material;
}

void RPR::FRPRXMaterial::ReleaseResources()
{
	ReleaseRPRXMaterial();
	UE4MaterialLink.Reset();
	Images.Empty();
}

void RPR::FRPRXMaterial::ReleaseRPRXMaterial()
{
	if (!IsMaterialValid())
	{
		return;
	}

	UE_LOG(LogRPRXMaterial, Verbose, TEXT("Delete material %s : %p"), *UE4MaterialLink->GetName(), Material);

	try
	{
		ReleaseMaterialNodes();

		RPRX::FContext rprxSupportCtx = IRPRCore::GetResources()->GetRPRXSupportContext();
		RPRX::FMaterialHelpers::DeleteMaterial(rprxSupportCtx, Material);
	}
	catch (std::exception ex)
	{
		UE_LOG(LogRPRXMaterial, Warning, TEXT("Couldn't delete an object/material correctly (%s)"), ANSI_TO_TCHAR(ex.what()));
		FRPRCoreErrorHelper::LogLastError();
	}
}

void RPR::FRPRXMaterial::ReleaseMaterialNodes()
{
#define ADD_CLASS_NAME_CHECKED(ClassName) \
		static_assert(TIsClass<ClassName>::Value, "Class doesn't exist!");	\
		materialMapClassNames.Add(TEXT(#ClassName));

	TArray<FString> materialMapClassNames;
	ADD_CLASS_NAME_CHECKED(FRPRMaterialMap);
	ADD_CLASS_NAME_CHECKED(FRPRMaterialCoM);
	ADD_CLASS_NAME_CHECKED(FRPRMaterialCoMChannel1);

	UStruct* parametersClass = FRPRUberMaterialParameters::StaticStruct();
	UProperty* parameterProperty = parametersClass->PropertyLink;

	while (parameterProperty != nullptr)
	{
		if (FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(parameterProperty))
		{
			const FRPRUberMaterialParameterBase* materialParameter =
				FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(&UE4MaterialLink->MaterialParameters, parameterProperty);

			FString parameterClassName = materialParameter->GetPropertyTypeName(parameterProperty);

			if (materialMapClassNames.Contains(parameterClassName))
			{
				const FRPRMaterialMap* materialMap = static_cast<const FRPRMaterialMap*>(materialParameter);
				ReleaseMaterialMapNodes(materialMap);
			}
		}

		parameterProperty = parameterProperty->PropertyLinkNext;
	}
}

void RPR::FRPRXMaterial::ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap)
{
	RPRX::FParameterType parameterType = MaterialMap->GetRprxParamType();

	RPR::FResult status;
	auto resources = IRPRCore::GetResources();

	RPRX::EMaterialParameterType materialParameterType;
	status = RPRX::FMaterialHelpers::GetMaterialParameterType(resources->GetRPRXSupportContext(), Material, parameterType, materialParameterType);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRXMaterial, Warning,
			TEXT("Cannot get RPR parameter type for parameter %s of the material %s"),
			*MaterialMap->GetParameterName(),
			*UE4MaterialLink->GetName());
		return;
	}

	if (materialParameterType != RPRX::EMaterialParameterType::Node)
	{
		return;
	}

	RPR::FMaterialNode materialNode = nullptr;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(resources->GetRPRXSupportContext(), Material, parameterType, materialNode);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRXMaterial, Warning,
			TEXT("Cannot get material node from the parameter %s of the material %s"),
			*MaterialMap->GetParameterName(),
			*UE4MaterialLink->GetName());
		return;
	}

	if (materialNode != nullptr)
	{
		ReleaseMaterialNodesHierarchy(materialNode);
		RPR::FMaterialHelpers::DeleteNode(materialNode);
	}
}

void RPR::FRPRXMaterial::ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode)
{
	uint64 numInputs = 0;
	RPR::FResult status = RPR::RPRMaterial::GetNodeInfo(MaterialNode, RPR::EMaterialNodeInfo::InputCount, &numInputs);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRXMaterial, Log, TEXT("Cannot get node input count"));
		return;
	}

	for (int32 inputIndex = 0; inputIndex < numInputs; ++inputIndex)
	{
		RPR::EMaterialNodeInputType inputType;
		status = RPR::RPRMaterial::GetNodeInputType(MaterialNode, inputIndex, inputType);
		if (RPR::IsResultFailed(status))
		{
			continue;
		}

		if (inputType == EMaterialNodeInputType::Node)
		{
			RPR::FMaterialNode childNode = nullptr;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, inputIndex, childNode);
			if (childNode != nullptr)
			{
				ReleaseMaterialNodesHierarchy(childNode);
				RPR::FMaterialHelpers::DeleteNode(childNode);
			}
		}
	}
}

RPRX::FContext RPR::FRPRXMaterial::GetRprxContext() const
{
	return (IRPRCore::GetResources()->GetRPRXSupportContext());
}
