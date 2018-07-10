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
#pragma once

#include "UVProjectionMappingEditor/Algorithms/IUVProjectionAlgorithm.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "Rendering/ColorVertexBuffer.h"
#include "UObject/GCObject.h"
#include "Templates/SharedPointer.h"
#include "RawMesh.h"
#include "RPRMeshData/RPRMeshDataContainer.h"

DECLARE_DELEGATE_TwoParams(FSectionWorker, FRPRMeshDataPtr /*MeshData*/, int32 /*SectionIndex*/)

/*
 * Abstraction class for UV projection algorithms
 */
class RPRPLUGINEDITOR_API FUVProjectionAlgorithmBase : public IUVProjectionAlgorithm
{
protected:

	typedef TArray<FVector2D>	FUVPack;

public:

	static bool	AreStaticMeshRenderDatasValid(UStaticMesh* InStaticMesh);

	virtual ~FUVProjectionAlgorithmBase() {}
	
	virtual void SetMeshDatas(const FRPRMeshDataContainer& MeshDatas) override;
	virtual void SetGlobalUVProjectionSettings(FUVProjectionSettingsPtr Settings);
	virtual FOnAlgorithmCompleted& OnAlgorithmCompleted() override;

	virtual void StartAlgorithm() override;
	virtual void AbortAlgorithm() override;
	virtual bool IsAlgorithimRunning() override;


protected:

	/* Clear the UV array and reserve space to fill it, based on the number of RawMesh.WedgeIndices. */
	void	PrepareUVs();

	void	StopAlgorithm();
	void	RaiseAlgorithmCompletion(bool bIsSuccess);
	void	StopAlgorithmAndRaiseCompletion(bool bIsSuccess);
	void	ApplyUVsOnMesh();
	void	SaveRawMesh();
	void	AddNewUVs(int32 RawMeshIndex, const FVector2D& UV);
	void	SetNewUV(int32 RawMeshIndex, int32 Index, const FVector2D& UV);
	void	FixInvalidUVsHorizontally(int32 MeshIndex);
	void	FixInvalidUVsHorizontally(int32 MeshIndex, int32 StartSection, int32 EndSection);

	void	OnEachSelectedSection(FSectionWorker Worker);


private:

	void	FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate);


protected:

	FRPRMeshDataContainer		MeshDatas;

	FOnAlgorithmCompleted		OnAlgorithmCompletedEvent;
	FUVProjectionSettingsPtr	UVProjectionSettings;

private:

	bool bIsAlgorithmRunning;
	TArray<FUVPack>	NewUVs;

};
