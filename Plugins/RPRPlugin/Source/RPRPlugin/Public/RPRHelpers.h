// RPR COPYRIGHT

#pragma once

#include "RadeonProRender.h"
#include "Math/mathutils.h"

RadeonProRender::matrix	BuildMatrixNoScale(const struct FTransform &transform);
RadeonProRender::matrix	BuildMatrixWithScale(const struct FTransform &transform);

/*
* Create a RPR image from a UTexture2D
*/
rpr_image	BuildImage(class UTexture2D *source, rpr_context context);

/*
* Create a RPR image from a UTextureCube
*/
rpr_image	BuildCubeImage(class UTextureCube *source, rpr_context context);

namespace RPR
{
	/*
	* Returns true if the result is a success
	*/
	bool		IsResultSuccess(rpr_int Result);

	/*
	* Returns true if the results is a failure
	*/
	bool		IsResultFailed(rpr_int Result);

	/*
	* Delete a native object.
	* Use the native RPR function to delete object.
	*/
	void		DeleteObject(void* Object);
}