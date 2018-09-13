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
