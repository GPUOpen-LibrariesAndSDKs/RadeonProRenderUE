#pragma once
#include "Engine/Texture2D.h"
#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"

namespace RPR
{

	class RPRIMPORTERTOOLS_API FTextureImporter
	{
	public:

		static UTexture2D*	ImportTextureFromPath(const FString& BaseFilePath, const FString& ImportTexturePath);
		static RPR::FResult	ImportTextureFromImage(RPR::FImage Image, const FString& DestinationDirectory, UTexture2D*& OutTexture);
		static RPR::FResult	CreateTextureFromImage(RPR::FImage Image, UTexture2D*& InOutTexture);

	private:

		static UTexture2D*	TryLoadingTextureIfAvailable(const FString& FilePath);
		static void			SetDefaultRequiredTextureFormat(UTexture2D* Texture);

	};

}