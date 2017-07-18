// RPR COPYRIGHT

#include "RPRSceneComponent.h"

URPRSceneComponent::URPRSceneComponent()
:	m_Built(false)
,	m_Sync(true)
,	m_Plugin(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;
}

bool	URPRSceneComponent::Build()
{
	m_Plugin = &FRPRPluginModule::Get();

	check(SrcComponent != NULL);
	m_CachedTransforms = SrcComponent->ComponentToWorld;
	m_Built = true;
	return true;
}

void	URPRSceneComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built || !m_Sync)
		return;
	check(Scene != NULL);
	if (SrcComponent == NULL)
	{
		// Source object destroyed, remove ourselves
		Scene->RemoveActor(Cast<ARPRActor>(GetOwner()));
		return;
	}
	check(m_Plugin != NULL);
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (!settings->bSync)
		return;
	// Seem to be the only way..
	// There is no runtime enabled callbacks
	// UEngine::OnActorMoved() and UEngine::OnComponentTransformChanged() are editor only..
	if (!m_CachedTransforms.Equals(SrcComponent->ComponentToWorld, 0.0001f))
	{
		if (RebuildTransforms())
			Scene->TriggerFrameRebuild();
		m_CachedTransforms = SrcComponent->ComponentToWorld;
	}
}
