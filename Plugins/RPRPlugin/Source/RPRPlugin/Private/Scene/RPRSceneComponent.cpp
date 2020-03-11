/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Scene/RPRSceneComponent.h"

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "Scene/RPRActor.h"
#include "Scene/RPRScene.h"

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
	m_CachedTransforms = SrcComponent->GetComponentToWorld();
	m_Built = true;
	return true;
}

bool	URPRSceneComponent::IsSrcComponentValid() const
{
	return SrcComponent != NULL &&
		SrcComponent->GetWorld() != NULL &&
		!SrcComponent->IsPendingKill();
}

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
	if (!m_CachedTransforms.Equals(SrcComponent->GetComponentToWorld(), 0.0001f))
	{
		m_RebuildFlags |= PROPERTY_REBUILD_TRANSFORMS;
		m_CachedTransforms = SrcComponent->GetComponentToWorld();
	}
	m_RefreshLock.Unlock();
}

void	URPRSceneComponent::ReleaseResources()
{
	m_Built = false;
}

void	URPRSceneComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if (m_Built)
	{
		// Object deleted before the scene has been destroyed
		check(Scene != NULL);
		Scene->ImmediateRelease(this);
	}
}
