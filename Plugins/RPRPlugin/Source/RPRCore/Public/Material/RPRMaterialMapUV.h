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
#include "CoreMinimal.h"
#include "RPRMaterialMapUV.generated.h"

UENUM(BlueprintType)
enum class ETextureUVMode : uint8
{
	None,
	Planar,
	Cylindrical,
	Spherical,
	Projection,
	Triplanar
};

/*
* UV settings of a RPRMaterialMap
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialMapUV
{
	GENERATED_BODY()

	// Only 2 UV channels supported
	UPROPERTY(EditAnywhere, meta = (DisplayName = "UV Channel", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1), BlueprintReadWrite, Category = Material)
	int32	UVChannel;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "UV Mode"), BlueprintReadWrite, Category = Material)
	ETextureUVMode	UVMode;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "In degrees", UIMin = 0, UIMax = 360, ClampMin = 0, ClampMax = 360), BlueprintReadWrite, Category = Material)
	float		Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		UVWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector2D	Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Right axis"), Category = Material)
	FVector		XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Up axis"), Category = Material)
	FVector		ZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector2D	Scale;


public:

	FRPRMaterialMapUV();
	
	uint8	GetRPRValueFromTextureUVMode() const;

private:

	static TMap<ETextureUVMode, uint8> TextureUVModeToRPRValue;

};