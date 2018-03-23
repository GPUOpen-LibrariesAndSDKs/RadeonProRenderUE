#pragma once
#include "RawMesh.h"
#include "ProceduralMeshComponent.h"
#include "RPRPreviewMeshComponent.generated.h"

UCLASS()
class RPRPLUGINEDITOR_API URPRMeshPreviewComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	URPRMeshPreviewComponent();

	void			Regenerate();
	void			SetStaticMesh(UStaticMesh* InStaticMesh, FRawMesh* InRawMesh);
	UStaticMesh*	GetStaticMesh() const;

private:

	void	GenerateUVsAndDuplicateRequiredVertices();

private:

	UPROPERTY(Transient)
	UStaticMesh* StaticMesh;
	
	FRawMesh* RawMesh;

};
