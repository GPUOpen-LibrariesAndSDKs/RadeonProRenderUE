#include "SUVProjectionBase.h"
#include "Engine/StaticMesh.h"
#include "AlgorithmFactory.h"

void SUVProjectionBase::Construct(const FArguments& InArgs)
{

}

void SUVProjectionBase::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	staticMesh = InStaticMesh;
}

TSharedRef<SWidget> SUVProjectionBase::TakeWidget()
{
	return (SharedThis(this));
}

UStaticMesh* SUVProjectionBase::GetStaticMesh() const
{
	return (staticMesh);
}

void SUVProjectionBase::ConstructBase()
{
	Construct(FArguments());
}

void SUVProjectionBase::InitializeAlgorithm(EUVProjectionType ProjectionType)
{
	algorithm = FAlgorithmFactory::CreateAlgorithm(staticMesh, ProjectionType);
	SubscribeToAlgorithmCompletion();
}

void SUVProjectionBase::SubscribeToAlgorithmCompletion()
{
	algorithm->OnAlgorithmCompleted().AddRaw(this, &SUVProjectionBase::OnAlgorithmCompleted);
}

void SUVProjectionBase::StartAlgorithm()
{
	if (algorithm.IsValid())
	{
		algorithm->StartAlgorithm();
	}
}

void SUVProjectionBase::FinalizeAlgorithm()
{
	if (algorithm.IsValid())
	{
		algorithm->Finalize();
	}
}
