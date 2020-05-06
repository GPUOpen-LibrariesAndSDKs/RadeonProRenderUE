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

#include "FImageSaver.h"

#ifdef TEXT
#undef TEXT
#endif

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif

#include "imageio.h"

namespace
{
	bool WriteImageToFile(const FString& filename, const OIIO::TypeDesc type, const void* imdata, const int width, const int height)
	{
		OIIO::ImageOutput* outImage = OIIO::ImageOutput::create(TCHAR_TO_ANSI(*filename));

		if (outImage)
		{
			OIIO::ImageSpec imgSpec(width, height, 4, type);

			outImage->open(TCHAR_TO_ANSI(*filename), imgSpec);
			outImage->write_image(type, imdata);
			outImage->close();
			delete outImage;

			return true;
		}

		return false;
	}
}

bool FImageSaver::WriteUint8ImageToFile(const FString& filename, const void* imdata, const int width, const int height)
{
	return WriteImageToFile(filename, OIIO::TypeDesc::UINT8, imdata, width, height);
}

bool FImageSaver::WriteFloatImageToFile(const FString& filename, const void* imdata, const int width, const int height)
{
	return WriteImageToFile(filename, OIIO::TypeDesc::FLOAT, imdata, width, height);
}
