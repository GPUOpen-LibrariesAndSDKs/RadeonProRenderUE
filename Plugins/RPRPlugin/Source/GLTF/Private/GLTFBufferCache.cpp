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

#include "GLTFBufferCache.h"

#include "Misc/Base64.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace GLTF
{
	FBufferCache::FBufferCache(const FString& InDirectory, const std::vector<FBuffer>& InBuffers)
	{
		for (int32 BufferIndex = 0; BufferIndex < InBuffers.size(); ++BufferIndex)
		{
			const FBuffer& Buffer = InBuffers.at(BufferIndex);
			const FString& BufferUri = Buffer.uri.c_str();
			if (Buffers.Find(BufferUri) != nullptr) continue; // Buffer has already been loaded

			TArray<uint8> BufferData;
			if (BufferUri.Contains("data:application/octet-stream;base64,")) // Buffer is embedded
			{
				const FString& BufferBase64Str = BufferUri.RightChop(BufferUri.Find(";base64,") + FString(";base64,").Len());
				if (!FBase64::Decode(BufferBase64Str, BufferData)) continue;
			}
			else // Buffer is on disk
			{
				const FString& BufferFilePath = InDirectory / BufferUri;
				if (!FPaths::FileExists(BufferFilePath)) continue;
				if (!FFileHelper::LoadFileToArray(BufferData, *BufferFilePath)) continue;
			}

			IndexToUris.Add(BufferIndex, BufferUri);
			Buffers.Add(BufferUri, BufferData);
		}
	}
}
