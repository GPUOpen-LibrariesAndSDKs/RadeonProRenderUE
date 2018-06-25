//~ RPR copyright

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
