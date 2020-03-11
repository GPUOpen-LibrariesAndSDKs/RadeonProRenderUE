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

#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "UObject/WeakObjectPtr.h"

struct FRPRMaterialMap;
class URPRMaterial;

namespace RPR
{

	class RPRCORE_API FRPRXMaterial
	{
	public:
		FRPRXMaterial(const URPRMaterial* InUE4MaterialLink);
		virtual ~FRPRXMaterial();

		const URPRMaterial*	GetUE4MaterialLink() const;

		void	AddImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImage Image);

		RPR::FResult	SetMaterialParameterBool(unsigned int Parameter, bool Value);
		RPR::FResult	SetMaterialParameterUInt(unsigned int Parameter, uint32 Value);
		RPR::FResult	SetMaterialParameterColor(unsigned int Parameter, const FLinearColor& Color);
		RPR::FResult	SetMaterialParameterFloats(unsigned int Parameter, float x, float y, float z, float w);
		RPR::FResult	SetMaterialParameterFloat(unsigned int Parameter, float Value);
		RPR::FResult	SetMaterialParameterNode(unsigned int Parameter, RPR::FMaterialNode MaterialNode);

		void	ReleaseResources();

		/*
		* Provide access to the native RPRX material pointer.
		* DO NOT DESTROY IT OR ANYTHING LIKE THAT.
		*/
		rpr_material_node		GetRawMaterial() const;

		bool	IsMaterialValid() const;

	private:

		void	ReleaseRPRXMaterial();
		void	ReleaseMaterialNodes();
		void	ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap);
		void	ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode);
	private:

		TWeakObjectPtr<const URPRMaterial> UE4MaterialLink;

		TArray<RPR::FImagePtr> Images;
		rpr_material_node	Material;
	};

	typedef TSharedPtr<FRPRXMaterial> FRPRXMaterialPtr;

	// ----------------------------------------------------
	class RPRCORE_API FRPRXMaterialNode final
	{
	public:

		FRPRXMaterialNode(FString name, unsigned int type = RPR_MATERIAL_NODE_UBERV2);
		~FRPRXMaterialNode();

		void	AddImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImagePtr Image);
		void	RemoveImage(RPR::FImage Image);

		RPR::FResult	SetMaterialParameterBool(unsigned int Parameter, bool Value);
		RPR::FResult	SetMaterialParameterUInt(unsigned int Parameter, uint32 Value);
		RPR::FResult	SetMaterialParameterColor(unsigned int Parameter, const FLinearColor& Color);
		RPR::FResult	SetMaterialParameterFloats(unsigned int Parameter, float x, float y, float z, float w);
		RPR::FResult	SetMaterialParameterFloat(unsigned int Parameter, float Value);
		RPR::FResult	SetMaterialParameterNode(unsigned int Parameter, RPR::FMaterialNode MaterialNode);

		void	ReleaseResources();

		rpr_material_node		GetRawMaterial() const;

		bool	IsMaterialValid() const;

		// avoid copy material node
		FRPRXMaterialNode(const FRPRXMaterialNode&)            = delete;
		FRPRXMaterialNode& operator=(const FRPRXMaterialNode&) = delete;
	private:

		void	ReleaseRPRXMaterial();
		void	ReleaseMaterialNodes();
		void	ReleaseMaterialMapNodes(const FRPRMaterialMap* MaterialMap);
		void	ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode);


		TArray<RPR::FImagePtr> Images;
		rpr_material_node Material;
		FString m_name;
		unsigned int m_type;
	};

	using FRPRXMaterialNodePtr = TSharedPtr<FRPRXMaterialNode>;

}
