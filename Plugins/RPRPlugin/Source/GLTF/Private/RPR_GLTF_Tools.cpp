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
#include "RPR_GLTF_Tools.h"
#include "Containers/Array.h"

namespace RPR
{
	namespace GLTF
	{

		FStatus ImportFromGLFT(
			const FString& Filename,
			RPR::FContext Context,
			RPR::FMaterialSystem MaterialSystem,
			RPRX::FContext RPRContext,
			RPR::FScene& OutScene)
		{
			return rprImportFromGLTF(TCHAR_TO_ANSI(*Filename), Context, MaterialSystem, RPRContext, &OutScene);
		}

		FStatus ExportToGLTF(
			const FString& Filename,
			RPR::FContext Context,
			RPR::FMaterialSystem MaterialSystem,
			RPRX::FContext RPRContext,
			const TArray<RPR::FScene>& Scenes)
		{
			return rprExportToGLTF(TCHAR_TO_ANSI(*Filename), Context, MaterialSystem, RPRContext, Scenes.GetData(), Scenes.Num());
		}

		bool IsResultSuccess(FStatus status)
		{
			return (status == GLTF_SUCCESS);
		}

		bool IsResultFailed(FStatus status)
		{
			return (status != GLTF_SUCCESS);
		}

		FString GetStatusText(FStatus status)
		{
			switch (status)
			{
				case GLTF_SUCCESS:
				return TEXT("Success");

				case GLTF_ERROR_SAVE:
				return TEXT("Error save");

				case GLTF_ERROR_LOAD:
				return TEXT("Error load");

				case GLTF_ERROR_IMPORT:
				return TEXT("Error import");

				case GLTF_ERROR_EXPORT:
				return TEXT("Error export");

				default:
				return TEXT("Unknown");
			}
		}

		namespace Import
		{
			/*
			* Generic way to list items (shapes, materials etc.) after a call to rprImportFromGLTF.
			* Since it is the same steps for every type of item,
			* we can use a template and pointer-to-function to not duplicate code
			*/
			template<typename ImportType>
			RPR::GLTF::FStatus GLTF_Import(
				TArray<ImportType>& OutItems, 
				RPR::GLTF::FStatus (*RPRNativeImportFunction)(ImportType* /* Items */, int /* sizeBytes */, int * /* numberOfItems */)
			)
			{
				OutItems.Empty();

				RPR::GLTF::FStatus status;
				int32 numItems = 0;
				status = RPRNativeImportFunction(nullptr, 0, &numItems);

				if (IsResultFailed(status))
				{
					return (status);
				}

				ImportType* items = new ImportType[numItems];
				status = RPRNativeImportFunction(items, numItems * sizeof(ImportType), nullptr);

				if (IsResultFailed(status))
				{
					delete[] items;
					return (status);
				}

				OutItems.Reserve(numItems);
				for (int32 i = 0; i < numItems; ++i)
				{
					OutItems.Add(items[i]);
				}

				delete[] items;
				return (status);
			}

			RPR::GLTF::FStatus GetShapes(TArray<RPR::FShape>& OutShapes)
			{
				return GLTF_Import<RPR::FShape>(OutShapes, rprGLTF_ListImported_Shapes);
			}

			RPR::GLTF::FStatus GetLights(TArray<RPR::FLight>& OutLights)
			{
				return GLTF_Import<RPR::FLight>(OutLights, rprGLTF_ListImported_Lights);
			}

			RPR::GLTF::FStatus GetImages(TArray<RPR::FImage>& OutImages)
			{
				return GLTF_Import<RPR::FImage>(OutImages, rprGLTF_ListImported_Images);
			}

			RPR::GLTF::FStatus GetCameras(TArray<RPR::FCamera>& OutCameras)
			{
				return GLTF_Import<RPR::FCamera>(OutCameras, rprGLTF_ListImported_Cameras);
			}

			RPR::GLTF::FStatus GetMaterialX(TArray<RPRX::FMaterial>& OutMaterials)
			{
				return GLTF_Import<RPRX::FMaterial>(OutMaterials, rprGLTF_ListImported_MaterialX);
			}

		}

	} // GLTF
} // RPR