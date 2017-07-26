// RPR COPYRIGHT

#include "RPRSceneComponent.h"

URPRSceneComponent::URPRSceneComponent()
:	Scene(NULL)
,	m_Built(false)
,	m_Sync(true)
,	m_RebuildFlags(0)
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

enum
{
	PROPERTY_REBUILD_TRANSFORMS	= 0x01,
};

bool	URPRSceneComponent::RPRThread_Update()
{
	bool	rebuild = false;

	m_RefreshLock.Lock();

	if (m_RebuildFlags & PROPERTY_REBUILD_TRANSFORMS)
		rebuild = RebuildTransforms();
	m_RebuildFlags = 0;

	m_RefreshLock.Unlock();

	return rebuild;
}

void	URPRSceneComponent::TriggerRebuildTransforms()
{
	m_RefreshLock.Lock();
	m_RebuildFlags |= PROPERTY_REBUILD_TRANSFORMS;
	m_RefreshLock.Unlock();
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
	m_RefreshLock.Lock();
	if (!m_CachedTransforms.Equals(SrcComponent->ComponentToWorld, 0.0001f))
	{
		m_RebuildFlags |= PROPERTY_REBUILD_TRANSFORMS;
		m_CachedTransforms = SrcComponent->ComponentToWorld;
	}
	m_RefreshLock.Unlock();
}
