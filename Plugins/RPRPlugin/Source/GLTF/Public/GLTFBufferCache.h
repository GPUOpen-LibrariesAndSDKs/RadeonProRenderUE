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

#include "NameTypes.h"
#include "GLTFTypedefs.h"

namespace GLTF
{
	class RPR_GLTF_API FBufferCache
	{
	public:

		explicit FBufferCache(const FString& InDirectory, const std::vector<FBuffer>& InBuffers);

		/** Get a buffer segment based on glTF buffer view / accessor. */
		template <typename T>
		bool Get(int32 InIndex, int32 InStart, int32 InLength, TArray<T>& OutBufferSegment) const;

	private:

		/** Uris keyed by buffer index in glTF structure. */
		TMap<int32, FString> IndexToUris;

		/** Buffers keyed by uri. */
		TMap<FString, TArray<uint8>> Buffers;
	};

	template<typename T>
	bool FBufferCache::Get(int32 InIndex, int32 InStart, int32 InLength, TArray<T>& OutBufferSegment) const
	{
		if ((sizeof(T) % sizeof(uint8)) != 0) return false;
		if (InStart < 0 || InLength <= 0 || (InLength % (sizeof(T) / sizeof(uint8))) != 0) return false;
		const FString& BufferUri = *IndexToUris.Find(InIndex);
		const TArray<uint8>& Buffer = *Buffers.Find(BufferUri);
		if (Buffer.Num() < (InStart + InLength)) return false;

		OutBufferSegment.SetNumUninitialized(InLength / (sizeof(T) / sizeof(uint8)));
		FMemory::Memcpy((void*)OutBufferSegment.GetData(), (void*)(Buffer.GetData() + InStart), sizeof(uint8) * InLength);
		return true;
	}
}
