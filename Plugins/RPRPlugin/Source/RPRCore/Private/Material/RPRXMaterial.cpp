/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Material/RPRXMaterial.h"
#include "Assets/RPRMaterial.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Helpers/RPRHelpers.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreErrorHelper.h"
#include "Enums/RPREnums.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRXMaterial, Log, Verbose)

RPR::FRPRXMaterial::FRPRXMaterial(const URPRMaterial* InUE4MaterialLink)
	: Material(nullptr)
	, UE4MaterialLink(InUE4MaterialLink)
{
	check(InUE4MaterialLink);

	RPR::FResult status;

	auto materialSystem = IRPRCore::GetResources()->GetMaterialSystem();
	status = rprMaterialSystemCreateNode(materialSystem, RPR_MATERIAL_NODE_UBERV2, &Material);
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

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterBool(unsigned int Parameter, bool Value)
{
	return SetMaterialParameterUInt(Parameter, Value ? 1 : 0);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterUInt(unsigned int Parameter, uint32 Value)
{
	return rprMaterialNodeSetInputUByKey(Material, Parameter, Value);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterColor(unsigned int Parameter, const FLinearColor& Color)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, Color.R, Color.G, Color.B, Color.A);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterFloats(unsigned int Parameter, float x, float y, float z, float w)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, x, y, z, w);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterFloat(unsigned int Parameter, float Value)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, Value, Value, Value, Value);
}

RPR::FResult RPR::FRPRXMaterial::SetMaterialParameterNode(unsigned int Parameter, RPR::FMaterialNode MaterialNode)
{
	return rprMaterialNodeSetInputNByKey(Material, Parameter, MaterialNode);
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

rpr_material_node RPR::FRPRXMaterial::GetRawMaterial() const
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

		(void)DeleteObject(Material);

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
	/*unsigned intType parameterType = MaterialMap->GetRprxParamType();

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

	if (materialNode != nullptr && RPR::RPRMaterial::IsMaterialNode(materialNode))
	{
		ReleaseMaterialNodesHierarchy(materialNode);

		UE_LOG(LogRPRXMaterial, Verbose, TEXT("Delete node %s"), *RPR::RPRMaterial::GetNodeName(materialNode));
		RPR::FMaterialHelpers::DeleteNode(materialNode);
	}*/
}

void RPR::FRPRXMaterial::ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode)
{
	/*uint64 numInputs = 0;
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
	}*/
}

// -----------------------------------------------------------------------------------------------

RPR::FRPRXMaterialNode::FRPRXMaterialNode(FString name, unsigned int type) :
 Material(nullptr),
 m_name(name),
 m_type(type)
{
	auto materialSystem = IRPRCore::GetResources()->GetMaterialSystem();

	RPR::FResult status;
	status = rprMaterialSystemCreateNode(materialSystem, m_type, &Material);
	if (status != RPR_SUCCESS) {
		UE_LOG(LogRPRXMaterial, Warning, TEXT("Can't create material node"));
		Material = nullptr;
	}
}

RPR::FRPRXMaterialNode::~FRPRXMaterialNode()
{
	ReleaseResources();
}

void RPR::FRPRXMaterialNode::AddImage(RPR::FImagePtr Image)
{
	Images.Add(Image);
}

void RPR::FRPRXMaterialNode::RemoveImage(RPR::FImagePtr Image)
{
	Images.Remove(Image);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterBool(unsigned int Parameter, bool Value)
{
	return SetMaterialParameterUInt(Parameter, Value ? 1 : 0);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterUInt(unsigned int Parameter, uint32 Value)
{
	return rprMaterialNodeSetInputUByKey(Material, Parameter, Value);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterColor(unsigned int Parameter, const FLinearColor& Color)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, Color.R, Color.G, Color.B, Color.A);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterFloats(unsigned int Parameter, float x, float y, float z, float w)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, x, y, z, w);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterFloat(unsigned int Parameter, float Value)
{
	return rprMaterialNodeSetInputFByKey(Material, Parameter, Value, Value, Value, Value);
}

RPR::FResult RPR::FRPRXMaterialNode::SetMaterialParameterNode(unsigned int Parameter, RPR::FMaterialNode MaterialNode)
{
	return rprMaterialNodeSetInputNByKey(Material, Parameter, MaterialNode);
}

void RPR::FRPRXMaterialNode::RemoveImage(RPR::FImage Image)
{
	Images.RemoveAll([Image] (RPR::FImagePtr imagePtr)
	{
		return imagePtr.IsValid() && (imagePtr.Get() == Image);
	});
}

bool RPR::FRPRXMaterialNode::IsMaterialValid() const
{
	return Material != nullptr;
}

rpr_material_node RPR::FRPRXMaterialNode::GetRawMaterial() const
{
	return Material;
}

void RPR::FRPRXMaterialNode::ReleaseResources()
{
	ReleaseRPRXMaterial();
	Images.Empty();
}

void RPR::FRPRXMaterialNode::ReleaseRPRXMaterial()
{
	if (!IsMaterialValid())
		return;

	ReleaseMaterialNodes();

	if (Material) {
		(void)rprObjectDelete(Material);
		Material = nullptr;
	}
}

void RPR::FRPRXMaterialNode::ReleaseMaterialNodes()
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
		if (!FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(parameterProperty)) {
			parameterProperty = parameterProperty->PropertyLinkNext;
			continue;
		}

		/* const FRPRUberMaterialParameterBase* materialParameter =
			FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(&UE4MaterialLink->MaterialParameters, parameterProperty);

		FString parameterClassName = materialParameter->GetPropertyTypeName(parameterProperty);

		if (materialMapClassNames.Contains(parameterClassName))
		{
			const FRPRMaterialMap* materialMap = static_cast<const FRPRMaterialMap*>(materialParameter);
			ReleaseMaterialMapNodes(materialMap);
		} */

		parameterProperty = parameterProperty->PropertyLinkNext;
	}
}

void RPR::FRPRXMaterialNode::ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap)
{
	/*unsigned int parameterType = MaterialMap->GetRprxParamType();

	RPR::FResult status;
	auto resources = IRPRCore::GetResources();

	RPRX::EMaterialParameterType materialParameterType;
	status = RPRX::FMaterialHelpers::GetMaterialParameterType(resources->GetRPRXSupportContext(), Material, parameterType, materialParameterType);
	scheck(status);

	if (materialParameterType != RPRX::EMaterialParameterType::Node)
		return;

	RPR::FMaterialNode materialNode = nullptr;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(resources->GetRPRXSupportContext(), Material, parameterType, materialNode);
	scheck(status);

	if (materialNode != nullptr && RPR::RPRMaterial::IsMaterialNode(materialNode))
	{
		ReleaseMaterialNodesHierarchy(materialNode);

		UE_LOG(LogRPRXMaterial, Verbose, TEXT("Delete node %s"), *RPR::RPRMaterial::GetNodeName(materialNode));
		RPR::FMaterialHelpers::DeleteNode(materialNode);
	}*/
}

void RPR::FRPRXMaterialNode::ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode)
{
	/*
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
	}*/
}

