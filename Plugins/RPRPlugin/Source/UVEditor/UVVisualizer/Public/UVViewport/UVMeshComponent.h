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

	virtual int32 GetNumMaterials() const override;

	void	SetUVChannel(int32 InUVChannel);
	void	SetMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas);

	FRPRMeshDataContainerWkPtr	GetMeshDatas() const;
	int32	GetUVChannel() const;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:

	void UpdateRPRMeshDatasFromTemplateMesh();

public:

	UPROPERTY(EditAnywhere, Category = UV)
	UStaticMesh*	TemplateMesh;

private:

	int32 UVChannel;
	FRPRMeshDataContainerWkPtr RPRMeshDatas;


	FRPRMeshDataContainerPtr TempMeshDataPtr;
};
