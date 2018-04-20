#include "RPRStaticMeshPreview.h"

ARPRStaticMeshPreview::ARPRStaticMeshPreview()
{
	PreviewComponent = CreateDefaultSubobject<URPRStaticMeshPreviewComponent>(TEXT("RPRMeshPreviewComponent"));
}

URPRStaticMeshPreviewComponent* ARPRStaticMeshPreview::GetPreviewComponent() const
{
	return (PreviewComponent);
}
