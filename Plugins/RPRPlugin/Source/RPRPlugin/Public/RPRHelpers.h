// RPR COPYRIGHT

#pragma once

#include "RadeonProRender.h"
#include "Math/mathutils.h"

RadeonProRender::matrix	BuildMatrixNoScale(const struct FTransform &transform);
RadeonProRender::matrix	BuildMatrixWithScale(const struct FTransform &transform);

rpr_image	BuildImage(class UTexture2D *source, rpr_context context);
rpr_image	BuildCubeImage(class UTextureCube *source, rpr_context context);

namespace RPR
{
	bool		IsResultSuccess(rpr_int Result);
	bool		IsResultFailed(rpr_int Result);

	void		DeleteObject(void* Object);
}