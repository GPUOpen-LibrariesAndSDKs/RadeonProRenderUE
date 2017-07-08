// RPR COPYRIGHT

#include "RPRSceneComponent.h"

URPRSceneComponent::URPRSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool	URPRSceneComponent::Build()
{
	check(SrcComponent != NULL);
	m_CachedTransforms = SrcComponent->ComponentToWorld;
	return true;
}

void	URPRSceneComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	check(SrcComponent != NULL);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	// Seem to be the only way..
	// There is no runtime enabled callbacks
	// UEngine::OnActorMoved() and UEngine::OnComponentTransformChanged() are editor only..
	if (!m_CachedTransforms.Equals(SrcComponent->ComponentToWorld, 0.0001f))
	{
		RebuildTransforms();
		Scene->TriggerFrameRebuild();
	}
	m_CachedTransforms = SrcComponent->ComponentToWorld;
}
