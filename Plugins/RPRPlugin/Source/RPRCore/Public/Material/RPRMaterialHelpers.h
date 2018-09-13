/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#pragma once

#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPREnums.h"
#include "ImageManager/RPRImageManager.h"

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
													UTexture2D* Texture, RPR::FImageManager::EImageType ImageType, 
													RPR::FImagePtr& OutImage, FMaterialNode& MaterialNode, FMaterialNode& OutImageNode);

		class FMaterialNode
		{
		public:

			static FResult	SetInputUInt(RPR::FMaterialNode MaterialNode, const FString& ParameterName, uint8 Value);
			static FResult	SetInputNode(RPR::FMaterialNode MaterialNode, const FString& ParameterName, RPR::FMaterialNode InMaterialNode);

			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, float x, float y = 0.0f, float z = 0.0f, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector2D& Value, float z = 0.0f, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector& Value, float w = 0.0f);
			static FResult	SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector4& Value);

		};

	};


}
