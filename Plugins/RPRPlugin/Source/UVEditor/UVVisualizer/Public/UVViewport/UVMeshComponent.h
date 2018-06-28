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
#include "Components/MeshComponent.h"
#include "RPRMeshDataContainer.h"
#include "UVMeshComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Rendering, Common), editinlinenew, BlueprintType, meta = (BlueprintSpawnableComponent))
class UVVISUALIZER_API UUVMeshComponent : public UMeshComponent
{
	GENERATED_BODY()
		
public:
	
	UUVMeshComponent();

	virtual void PostLoad() override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual int32 GetNumMaterials() const override;
	
	void	SetUVChannel(int32 InUVChannel);
	void	SetMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas);
	void	UpdateMeshDatas();

	FRPRMeshDataContainerWkPtr	GetMeshDatas() const;
	int32	GetUVChannel() const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:

	void UpdateLocalBounds();
	void UpdateRPRMeshDatasFromTemplateMesh();
	void UpdateUVs();

public:

	UPROPERTY(EditAnywhere, Category = UV)
	UStaticMesh*	TemplateMesh;

	UPROPERTY(EditAnywhere, Category = UV)
	bool			bCenterUVs;

private:

	int32 UVChannel;
	FRPRMeshDataContainerWkPtr RPRMeshDatas;
	FBoxSphereBounds LocalBounds;

	class FUVMeshComponentProxy* SceneProxy;

	FRPRMeshDataContainerPtr TempMeshDataPtr;
};
