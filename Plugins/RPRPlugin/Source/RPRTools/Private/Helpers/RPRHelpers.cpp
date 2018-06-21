// RPR COPYRIGHT

#include "RPRHelpers.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "RprSupport.h"
#include "RprTools.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRHelpers, Log, All);

static const float RPR_PI = 3.14159265f;

// Probably not the ideal way of converting UE4 matrices to RPR
// If you find a better way, have fun :)
RadeonProRender::matrix	BuildMatrixNoScale(const FTransform &transform)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

RadeonProRender::matrix	BuildMatrixWithScale(const FTransform &transform)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FVector	&scale = transform.GetScale3D();
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::float3		rprScale(scale.X, scale.Z, scale.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion) * RadeonProRender::scale(rprScale);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

namespace RPR
{

	bool IsResultSuccess(rpr_int Result)
	{
		return (Result == RPR_SUCCESS);
	}

	bool IsResultFailed(rpr_int Result)
	{
		return (!IsResultSuccess(Result));
	}

	FResult DeleteObject(void* Object)
	{
		return rprObjectDelete(Object);
	}

	FResult SceneDetachShape(FScene Scene, FShape Shape)
	{
		return rprSceneDetachShape(Scene, Shape);
	}

	FResult ShapeSetMaterial(FShape Shape, RPR::FMaterialNode MaterialNode)
	{
		return (rprShapeSetMaterial(Shape, MaterialNode));
	}

	FResult SceneClear(FScene Scene)
	{
		return (rprSceneClear(Scene));
	}

	FResult RegisterPlugin(const FString& DllPath)
	{
		return rprRegisterPlugin(TCHAR_TO_ANSI(*DllPath));
	}

	bool AreDevicesCompatible(FPluginId PluginId, const FString& RenderCachePath, bool bDoWhiteListTest, FCreationFlags DevicesUsed, FCreationFlags& OutDevicesCompatible, RPR_TOOLS_OS ToolsOS)
	{
		rprAreDevicesCompatible(PluginId, TCHAR_TO_ANSI(*RenderCachePath), bDoWhiteListTest, DevicesUsed, &OutDevicesCompatible, ToolsOS);
		return (OutDevicesCompatible > 0);
	}
}