#include "SUVProjectionBase.h"
#include "Engine/StaticMesh.h"
#include "AlgorithmFactory.h"

void SUVProjectionBase::Construct(const FArguments& InArgs)
{
}

void SUVProjectionBase::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;
}

void SUVProjectionBase::SetRPRStaticMeshEditor(FRPRStaticMeshEditorWeakPtr InRPRStaticMeshEditor)
{
	RPRStaticMeshEditor = InRPRStaticMeshEditor;
	InitializePostSetRPRStaticMeshEditor();
}

TSharedRef<SWidget> SUVProjectionBase::TakeWidget()
{
	return (SharedThis(this));
}

UStaticMesh* SUVProjectionBase::GetStaticMesh() const
{
	return (StaticMesh);
}

void SUVProjectionBase::ConstructBase()
{
	Construct(FArguments());
}

void SUVProjectionBase::AddShapePreviewToViewport()
{
	AddComponentToViewport(GetShapePreview());
}

void SUVProjectionBase::AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape /*= true*/)
{
	FRPRStaticMeshEditorPtr rprStaticMeshEditor = RPRStaticMeshEditor.Pin();
	if (rprStaticMeshEditor.IsValid())
	{
		rprStaticMeshEditor->AddComponentToViewport(InActorComponent, bSelectShape);
	}
}

void SUVProjectionBase::InitializeAlgorithm(EUVProjectionType ProjectionType)
{
	Algorithm = FAlgorithmFactory::CreateAlgorithm(StaticMesh, ProjectionType);
	SubscribeToAlgorithmCompletion();
}

void SUVProjectionBase::SubscribeToAlgorithmCompletion()
{
	Algorithm->OnAlgorithmCompleted().AddRaw(this, &SUVProjectionBase::OnAlgorithmCompleted);
}

void SUVProjectionBase::StartAlgorithm()
{
	if (Algorithm.IsValid())
	{
		Algorithm->StartAlgorithm();
	}
}

void SUVProjectionBase::FinalizeAlgorithm()
{
	if (Algorithm.IsValid())
	{
		Algorithm->Finalize();
	}
}

void SUVProjectionBase::InitializePostSetRPRStaticMeshEditor()
{
	AddShapePreviewToViewport();
}
