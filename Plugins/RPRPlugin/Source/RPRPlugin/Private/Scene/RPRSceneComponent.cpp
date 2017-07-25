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

bool	URPRSceneComponent::PostBuild()
{
	m_Plugin = &FRPRPluginModule::Get();

	check(SrcComponent != NULL);
	m_CachedTransforms = SrcComponent->ComponentToWorld;
	m_Built = true;
	return true;
}

bool	URPRSceneComponent::IsSrcComponentValid() const
{
	return SrcComponent != NULL &&
		SrcComponent->GetWorld() != NULL &&
		!SrcComponent->IsPendingKill();
}

void	URPRSceneComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	Super::TickComponent(deltaTime, tickType, tickFunction);

	if (!m_Built)
		return;
	check(Scene != NULL);
	if (!IsSrcComponentValid())
	{
		// Source object destroyed, remove ourselves
		Scene->RemoveActor(Cast<ARPRActor>(GetOwner()));
		return;
	}
	if (!m_Sync)
		return;
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
