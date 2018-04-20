#pragma once
#include "GameFramework/Actor.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "RPRStaticMeshPreview.generated.h"

UCLASS()
class RPREDITORTOOLS_API ARPRStaticMeshPreview : public AActor
{
	GENERATED_BODY()

public:

	ARPRStaticMeshPreview();

	URPRStaticMeshPreviewComponent*	GetPreviewComponent() const;

private:

	UPROPERTY()
	URPRStaticMeshPreviewComponent*	PreviewComponent;

};