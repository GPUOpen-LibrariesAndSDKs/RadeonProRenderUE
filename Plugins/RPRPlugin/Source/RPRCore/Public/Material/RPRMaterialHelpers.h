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

#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPREnums.h"
#include "ImageManager/RPRImageManager.h"
#include "Templates/IsEnum.h"

namespace RPR
{
	/*
	* Interface between the RPR material native functions and UE4
	*/
	class RPRCORE_API FMaterialHelpers
	{
	public:

		static const TCHAR*	ImageDataInputName;

		static FResult	CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode);
		static FResult	CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, const FString& NodeName, FMaterialNode& OutMaterialNode);
		static FResult	DeleteNode(FMaterialNode& MaterialNode);

		static FResult	CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
										UTexture2D* Texture, RPR::FImagePtr& OutImage, RPR::FMaterialNode& MaterialNode, RPR::FMaterialNode& OutImageNode);

		static FResult	CreateNormalMap(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
										UTexture2D* Texture, RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode);

		static FResult	CreateBumpMap(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
										UTexture2D* Texture, float BumpScale,
										RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode);

		class FMaterialNode
		{
		public:

			static FResult	SetInputUInt(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, unsigned int Value);
			static FResult	SetInputNode(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, RPR::FMaterialNode InMaterialNode);
			static FResult	SetInputImageData(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, RPR::FImage InImage);

			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, float x, float y = 0.0f, float z = 0.0f, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector2D& Value, float z = 0.0f, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector& Value, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector4& Value);

			template<typename TEnum>
			static FResult	SetInputEnum(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, TEnum Value)
			{
				static_assert(TIsEnum<TEnum>::Value, "Value must be an enum!");
				return SetInputUInt(MaterialNode, ParameterName, (uint8) Value);
			}

		};

		class FArithmeticNode
		{
		public:

			class FRotationNode
			{
			public:

				static FResult	CreateRotationNode(FMaterialSystem MaterialSystem, const FString& NodeName, FRotationNode& OutRotationNode);

				void SetRotationAngle(float RadAngle);
				void SetInputVector2D(RPR::FMaterialNode MaterialNode);

				RPR::FMaterialNode	GetOutputNode() const;

			private:
				RPR::FMaterialNode OutputNode;
				RPR::FMaterialNode InputVector2DNode;
				RPR::FMaterialNode InputRotationCenter;
				RPR::FMaterialNode InputRotationAngleA;
				RPR::FMaterialNode InputRotationAngleB;
			};

		public:

			static FResult	CreateArithmeticNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeArithmeticOperation Operation, RPR::FMaterialNode& OutMaterialNode);
			static FResult	CreateArithmeticNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeArithmeticOperation Operation, const FString& NodeName, RPR::FMaterialNode& OutMaterialNode);

			static FResult	CreateVector2DRotationNode(FMaterialSystem MaterialSystem, const FString& NodeName, FRotationNode& OutRotationNode);

		};

	};


}
