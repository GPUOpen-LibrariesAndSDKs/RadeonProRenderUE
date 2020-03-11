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

#include "Material/RPRMaterialMapUV.h"
#include "RadeonProRender.h"

TMap<ETextureUVMode, uint8> FRPRMaterialMapUV::TextureUVModeToRPRValue({
	{ETextureUVMode::None, 0},
	{ETextureUVMode::Planar, RPR_MATERIAL_NODE_UVTYPE_PLANAR},
	{ETextureUVMode::Cylindrical, RPR_MATERIAL_NODE_UVTYPE_CYLINDICAL},
	{ETextureUVMode::Spherical, RPR_MATERIAL_NODE_UVTYPE_SPHERICAL},
	{ETextureUVMode::Projection, RPR_MATERIAL_NODE_UVTYPE_PROJECT},
	{ETextureUVMode::Triplanar, RPR_MATERIAL_NODE_UV_TRIPLANAR}
});

FRPRMaterialMapUV::FRPRMaterialMapUV()
	: UVMode(ETextureUVMode::None)
	, Rotation(0.0f)
	, UVWeight(0.0f)
	, Threshold(0.5f, 0.5f, 0.5f)
	, XAxis(1.0f, 0.0f, 0.0f)
	, ZAxis(0.0f, 0.0f, 1.0f)
	, Scale(1.0f, 1.0f)
{
}

uint8 FRPRMaterialMapUV::GetRPRValueFromTextureUVMode() const
{
	return TextureUVModeToRPRValue[UVMode];
}
