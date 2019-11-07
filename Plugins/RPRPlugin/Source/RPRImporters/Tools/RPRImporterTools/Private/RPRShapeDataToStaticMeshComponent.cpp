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
	RPR::FResult status = RPR::Shape::GetWorldTransform(Shape, transform);
	check(RPR::IsResultSuccess(status));

	if (RootActor != nullptr)
	{
		RootActor->SetActorTransform(transform);
	}
	else
	{
		StaticMeshComponent->SetRelativeTransform(transform);
	}
}

void RPR::GLTF::Import::FRPRShapeDataToMeshComponent::SetupShapeInstance(
	RPR::FShape ShapeInstance, 
	UStaticMeshComponent* StaticMeshComponent, 
	RPR::GLTF::FStaticMeshResourcesPtr MeshResources, 
	AActor* RootActor /*= nullptr*/)
{
	RPR::FShape meshShape;
	RPR::FResult status = RPR::Shape::GetInstanceBaseShape(ShapeInstance, meshShape);
	check(RPR::IsResultSuccess(status));

	auto resourceData = MeshResources->FindResourceByShape(meshShape);
	UStaticMesh* staticMesh = resourceData->ResourceUE4;

	StaticMeshComponent->SetStaticMesh(staticMesh);

	FTransform transform;
	status = RPR::Shape::GetWorldTransform(ShapeInstance, transform);
	check(RPR::IsResultSuccess(status));

	if (RootActor != nullptr)
	{
		RootActor->SetActorTransform(transform);
	}
	else
	{
		StaticMeshComponent->SetRelativeTransform(transform);
	}
}
