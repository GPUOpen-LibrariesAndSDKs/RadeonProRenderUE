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
#include "gltf/gltf2.h"
#include "Resources/ImageResources.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class FImagesImporter
			{
			public:

				static bool	ImportImages(const FString& GLTFFileDirectory, const gltf::glTFAssetData& GLTFFileData, RPR::GLTF::FImageResourcesPtr ImageResources);

			private:

				static void	GetImagePathsFromGLTF(const FString& GLTFFileDirectory, const gltf::glTFAssetData& GLTFFileData, TArray<FString>& OutImagePaths);
				static void	LoadTextures(const TArray<FString>& ImagePaths, RPR::GLTF::FImageResourcesPtr ImageResources);

				// Set required compression settings on the new imported textures
				static void ConvertTexturesToBeSupported(const TArray<UObject*>& Objects);

				static bool ShouldTextureBeConverted(UTexture* Texture);
			};

		}
	}
}
