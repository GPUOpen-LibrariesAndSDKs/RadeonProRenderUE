#include "RPRMaterialHelpers.h"
#include "RPRHelpers.h"
#include "LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialHelpers, Log, All)

namespace RPR
{
	const FName FMaterialHelpers::ImageDataInputName(TEXT("data"));

	FResult FMaterialHelpers::CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode)
	{
		FResult result = rprMaterialSystemCreateNode(MaterialSystem, NodeType, &OutMaterialNode);

		if (IsResultFailed(result))
		{
			UE_LOG(LogRPRMaterialHelpers, Warning, TEXT("Couldn't create RPR material node (%#08)"), result);
		}

		return (result);
	}

	RPR::FResult FMaterialHelpers::DeleteNode(FMaterialNode& MaterialNode)
	{
		rprObjectDelete(MaterialNode);
		MaterialNode = nullptr;
	}

	RPR::FResult FMaterialHelpers::CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, 
															UTexture2D* Texture, FMaterialNode& OutMaterialNode)
	{
		RPR::FResult result = CreateNode(MaterialSystem, EMaterialNodeType::ImageTexture, OutMaterialNode);
		if (IsResultSuccess(result))
		{
			// TODO : Cache the image built
			RPR::FImage image = BuildImage(Texture, RPRContext);
			result = rprMaterialNodeSetInputImageData(OutMaterialNode, ImageDataInputName, image);
		}

		return (result);
	}

}