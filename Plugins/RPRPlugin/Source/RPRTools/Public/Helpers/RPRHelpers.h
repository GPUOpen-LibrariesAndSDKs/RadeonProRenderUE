// RPR COPYRIGHT

#pragma once

#include "RadeonProRender.h"
#include "Math/mathutils.h"
#include "RPRToolsModule.h"
#include "RPRTypedefs.h"

RPRTOOLS_API RadeonProRender::matrix BuildMatrixNoScale(const struct FTransform &transform);
RPRTOOLS_API RadeonProRender::matrix BuildMatrixWithScale(const struct FTransform &transform);

namespace RPR
{
	/*
	* Returns true if the result is a success
	*/
	RPRTOOLS_API bool		IsResultSuccess(rpr_int Result);

	/*
	* Returns true if the results is a failure
	*/
	RPRTOOLS_API bool		IsResultFailed(rpr_int Result);

	/*
	* Delete a native object.
	* Use the native RPR function to delete object.
	*/
	RPRTOOLS_API FResult		DeleteObject(void* Object);

	RPRTOOLS_API FResult		SceneDetachShape(FScene Scene, FShape Shape);

	RPRTOOLS_API FResult		ShapeSetMaterial(FShape Shape, RPR::FMaterialNode MaterialNode);
}