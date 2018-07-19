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
		static UTexture2D*	ImportTextureFromImageNode(RPR::FMaterialNode ImageNode, const FString& DestinationDirectory);

	private:

		static UTexture2D*	TryLoadingTextureIfAvailable(const FString& FilePath);
		static void			SetDefaultRequiredTextureFormat(UTexture2D* Texture);

	};

}