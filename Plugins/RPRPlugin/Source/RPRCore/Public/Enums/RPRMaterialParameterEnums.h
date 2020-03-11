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

#pragma once
#include "Templates/UnrealTypeTraits.h"
#include "RadeonProRender.h"

UENUM(BlueprintType)
enum class ERPRReflectionMode : uint8
{
	PBR = RPR_UBER_MATERIAL_IOR_MODE_PBR,
	Metalness = RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
};

UENUM(BlueprintType)
enum class ERPRCoatingMode : uint8
{
	PBR = RPR_UBER_MATERIAL_IOR_MODE_PBR,
	//Metalness = RPRX_UBER_MATERIAL_COATING_MODE_METALNESS,
};

UENUM(BlueprintType)
enum class ERPREmissionMode : uint8
{
	SingleSided = RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED,
	DoubleSided = RPR_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED
};

template<>
struct TNameOf<ERPRReflectionMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRReflectionMode");
	}
};

template<>
struct TNameOf<ERPRCoatingMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRCoatingMode");
	}
};

template<>
struct TNameOf<ERPREmissionMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPREmissionMode");
	}
};
