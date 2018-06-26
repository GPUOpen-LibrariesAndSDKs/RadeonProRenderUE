//~ RPR copyright

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
