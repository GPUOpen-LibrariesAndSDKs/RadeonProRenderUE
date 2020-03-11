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
#include "Engine/Texture.h"
#include "Containers/Map.h"

namespace RPR
{
	/*
	* Cache for the RPR Image.
	* Once the image is added, the cache becomes owner of the image and will delete it when resources will be released.
	*/
	class FImagesCache
	{
	public:

		virtual ~FImagesCache();

		void	Add(UTexture* Texture, FImagePtr Image);
		void	Release(UTexture* Texture);

		// Free each image resources and clear the cache
		void	ReleaseAll();

		FImagePtr	Get(UTexture* Texture);

	private:

		void RemoveDeprecatedImages();

	private:

		TMap<TWeakObjectPtr<UTexture>, FImagePtr>	loadedImages;

	};

}
