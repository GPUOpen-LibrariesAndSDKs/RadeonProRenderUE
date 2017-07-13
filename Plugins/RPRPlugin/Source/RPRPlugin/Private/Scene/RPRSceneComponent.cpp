// RPR COPYRIGHT

#include "RPRSceneComponent.h"

URPRSceneComponent::URPRSceneComponent()
:	m_Plugin(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
}

bool	URPRSceneComponent::Build()
{
	check(SrcComponent != NULL);

	m_Plugin = FModuleManager::GetModulePtr<FRPRPluginModule>("RPRPlugin");

	m_CachedTransforms = SrcComponent->ComponentToWorld;
	return m_Plugin != NULL;
}

void	URPRSceneComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	check(Scene != NULL);
	if (SrcComponent == NULL)
	{
		// Source object destroyed, remove ourselves
		Scene->RemoveActor(GetOwner());
		return;
	}
	check(m_Plugin != NULL);

	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Plugin->SyncEnabled())
		return;
	// Seem to be the only way..
	// There is no runtime enabled callbacks
	// UEngine::OnActorMoved() and UEngine::OnComponentTransformChanged() are editor only..
	if (!m_CachedTransforms.Equals(SrcComponent->ComponentToWorld, 0.0001f))
	{
		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
	}
	m_CachedTransforms = SrcComponent->ComponentToWorld;
}
