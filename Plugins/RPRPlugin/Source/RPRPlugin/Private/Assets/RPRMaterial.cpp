#include "RPRMaterial.h"

URPRMaterial::URPRMaterial()
#if WITH_EDITOR
	: bShouldCacheBeRebuild(true)
#endif
{

}

#if WITH_EDITOR

void URPRMaterial::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UProperty* memberProperty = PropertyChangedEvent.MemberProperty;
	if (memberProperty != nullptr && memberProperty->GetName() == GET_MEMBER_NAME_STRING_CHECKED(URPRMaterial, MaterialParameters))
	{
		bShouldCacheBeRebuild = true;
		OnRPRMaterialChanged.Broadcast(this);
	}
}

#endif