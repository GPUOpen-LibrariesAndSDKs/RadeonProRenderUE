/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRShapeHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Class.h"
#include "EngineMinimal.h"

namespace RPR
{
	namespace Context
	{

		FResult Create(int32 ApiVersion, TArray<FPluginId>& PluginIds, FCreationFlags CreationFlags, const FContextProperties* ContextProperties, const FString& CachePath, FContext& OutContext)
		{
			return rprCreateContext(ApiVersion, PluginIds.GetData(), PluginIds.Num(), CreationFlags, ContextProperties, TCHAR_TO_ANSI(*CachePath), &OutContext);
		}

		FResult Create(int32 ApiVersion, FPluginId PluginId, FCreationFlags CreationFlags, const FContextProperties* ContextProperties, const FString& CachePath, FContext& OutContext)
		{
			TArray<FPluginId> pluginIds;
			pluginIds.Add(PluginId);
			return (Create(ApiVersion, pluginIds, CreationFlags, ContextProperties, CachePath, OutContext));
		}

		FResult CreateScene(FContext Context, FScene& OutScene)
		{
			RPR::FResult status = rprContextCreateScene(Context, &OutScene);
			UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprContextCreateScene(context=%p) -> status=%d, scene=%p"), Context, status, OutScene);
			return status;
		}

		FResult SetActivePlugin(FContext Context, FPluginId PluginId)
		{
			return (rprContextSetActivePlugin(Context, PluginId));
		}

		FResult ClearMemory(FContext Context)
		{
			return (rprContextClearMemory(Context));
		}

		FResult SetScene(FContext Context, FScene Scene)
		{
			RPR::FResult status = rprContextSetScene(Context, Scene);
			UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprContextSetScene(context=%p, scene=%p) -> %d"), Context, Scene, status);
			return status;
		}

		FResult SetAOV(FContext Context, RPR::EAOV AOV, FFrameBuffer FrameBuffer)
		{
			RPR::FResult status = rprContextSetAOV(Context, (rpr_aov) AOV, FrameBuffer);

			UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprContextSetAOV(context=%p, AOV=%d, framebuffer=%p) -> %d"),
				Context,
				(uint8) AOV,
				FrameBuffer,
				status);

			return status;
		}

		FResult Render(FContext Context)
		{
			RPR::FResult status = rprContextRender(Context);
			UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprContextRender(context=%p) -> %d"), Context, status);
			return status;
		}

		FResult ResolveFrameBuffer(FContext Context, FFrameBuffer SrcFrameBuffer, FFrameBuffer DstFrameBuffer, bool bShouldNormalizeOnly)
		{
			RPR::FResult status = rprContextResolveFrameBuffer(Context, SrcFrameBuffer, DstFrameBuffer, bShouldNormalizeOnly);

			UE_LOG(LogRPRTools_Step, VeryVerbose,
				TEXT("rprContextResolveFrameBuffer(context=%p, srcFrameBuffer=%p, dstFrameBuffer=%p, shouldNormalizeOnly=%s) -> %d"),
				Context, SrcFrameBuffer, DstFrameBuffer, bShouldNormalizeOnly ? TEXT("true") : TEXT("false"), status);

			return status;
		}

		FResult CreateInstance(FContext Context, RPR::FShape Shape, RPR::FShape& OutShapeInstance)
		{
			RPR::FResult status = rprContextCreateInstance(Context, Shape, &OutShapeInstance);

			UE_LOG(LogRPRTools_Step, VeryVerbose,
				TEXT("rprContextCreateInstance(context=%p, shape=%s) -> status=%d, instance=%p"),
				Context, *RPR::Shape::GetName(Shape), status, OutShapeInstance);

			return status;
		}

		FResult CreateInstance(FContext Context, RPR::FShape Shape, const FString& InstanceName, RPR::FShape& OutShapeInstance)
		{
			RPR::FResult status = CreateInstance(Context, Shape, OutShapeInstance);

			if (RPR::IsResultSuccess(status))
			{
				RPR::SetObjectName(OutShapeInstance, *InstanceName);
			}

			return status;
		}

		FResult CreateMesh(FContext Context, const TCHAR* MeshName,
					const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<uint32>& Indices,
					const TArray<FVector2D>& Texcoords, const TArray<uint32>& NumFaceVertices, FShape& OutMesh)
		{
			RPR::FResult status = rprContextCreateMesh(Context,
				(rpr_float const *) Vertices.GetData(),		Vertices.Num(),		sizeof(float) * 3,
				(rpr_float const *) Normals.GetData(),		Normals.Num(),		sizeof(float) * 3,
				(rpr_float const *) Texcoords.GetData(),	Texcoords.Num(),	sizeof(float) * 2,
				(rpr_int const *) Indices.GetData(), sizeof(uint32),
				(rpr_int const *) Indices.GetData(), sizeof(uint32),
				(rpr_int const *) Indices.GetData(), sizeof(uint32),
				(rpr_int const *) NumFaceVertices.GetData(), NumFaceVertices.Num(),
				&OutMesh);

			UE_LOG(LogRPRTools_Step, VeryVerbose,
				TEXT("rprContextCreateMesh(context=%p, verticesNum=%d, normalsNum=%d, texCoordsNum=%d, indicesNum=%d, numFaceVertices=%d) -> status=%d, mesh=%p"),
				Context,
				Vertices.Num(), Normals.Num(), Texcoords.Num(), Indices.Num(), NumFaceVertices.Num(),
				status, OutMesh);

			if (RPR::IsResultSuccess(status))
			{
				status = RPR::SetObjectName(OutMesh, MeshName);
			}

			return status;
		}

		namespace Parameters
		{
			FResult Set1u(FContext Context, unsigned int ParamName, uint32 Value)
			{
				return (rprContextSetParameterByKey1u(Context, ParamName, Value));
			}

			FResult Set1f(FContext Context, unsigned int ParamName, float Value)
			{
				return (rprContextSetParameterByKey1f(Context, ParamName, Value));
			}
		}

		namespace MaterialSystem
		{
			FResult Create(RPR::FContext Context, RPR::FMaterialSystemType Type, RPR::FMaterialSystem& OutMaterialSystem)
			{
				return (rprContextCreateMaterialSystem(Context, Type, &OutMaterialSystem));
			}
		}

	}
}
