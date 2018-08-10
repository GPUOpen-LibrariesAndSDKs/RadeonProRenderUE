#include "RPRShapeDataToStaticMeshComponent.h"
#include "Helpers/RPRShapeHelpers.h"
#include "Typedefs/RPRTypedefs.h"
#include "Resources/StaticMeshResources.h"
#include "RPR_GLTF_Tools.h"

void RPR::GLTF::Import::FRPRShapeDataToMeshComponent::Setup(RPR::FShape Shape, 
	UStaticMeshComponent* StaticMeshComponent, 
	RPR::GLTF::FStaticMeshResourcesPtr MeshResources, 
	AActor* RootActor)
{
	auto resourceData = MeshResources->FindResourceByShape(Shape);
	UStaticMesh* staticMesh = resourceData->ResourceUE4;

	StaticMeshComponent->SetStaticMesh(staticMesh);

	FTransform transform;
	RPR::FResult status = RPR::Shape::GetLocalTransform(Shape, transform);
	if (RPR::IsResultFailed(status))
	{
		return;
	}

	if (RootActor != nullptr)
	{
		RootActor->SetActorTransform(transform);
	}
	else
	{
		StaticMeshComponent->SetRelativeTransform(transform);
	}
}
