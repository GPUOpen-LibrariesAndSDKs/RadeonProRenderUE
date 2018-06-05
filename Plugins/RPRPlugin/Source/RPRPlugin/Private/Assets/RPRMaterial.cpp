#include "RPRMaterial.h"

URPRMaterial::URPRMaterial()
	: bShouldCacheBeRebuild(true)
{

}

#if WITH_EDITOR

void URPRMaterial::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UProperty* memberProperty = PropertyChangedEvent.MemberProperty;
	if (memberProperty != nullptr && memberProperty->GetName() == GET_MEMBER_NAME_STRING_CHECKED(URPRMaterial, MaterialParameters))
	{
		MarkMaterialDirty();
	}
}

#endif

void URPRMaterial::MarkMaterialDirty()
{
	bShouldCacheBeRebuild = true;
	OnRPRMaterialChangedEvent.Broadcast(this);
}

void URPRMaterial::ResetMaterialDirtyFlag()
{
	bShouldCacheBeRebuild = false;
}

bool URPRMaterial::IsMaterialDirty() const
{
	return (bShouldCacheBeRebuild);
}
