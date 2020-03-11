/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Assets/RPRMaterial.h"

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
