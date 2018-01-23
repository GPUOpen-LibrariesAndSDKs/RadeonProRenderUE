#include "RPRMaterialHelpers.h"
#include "RPRHelpers.h"
#include "LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialHelpers, Log, All)

namespace RPR
{

	FResult FMaterialHelpers::CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode)
	{
		FResult result = rprMaterialSystemCreateNode(MaterialSystem, NodeType, &OutMaterialNode);

		if (IsResultFailed(result))
		{
			UE_LOG(LogRPRMaterialHelpers, Warning, TEXT("Couldn't create RPR material node (%#08)"), result);
		}

		return (result);
	}

	FResult FMaterialHelpers::DeleteMaterial(FContext Context, FMaterial MaterialData)
	{
		return (rprxMaterialDelete(Context, MaterialData));
	}

	
}