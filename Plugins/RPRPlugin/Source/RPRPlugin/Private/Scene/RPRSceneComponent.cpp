/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#include "Scene/RPRSceneComponent.h"

#include "RPRPlugin.h"
#include "RPRSettings.h"

#include "Scene/RPRActor.h"
#include "Scene/RPRScene.h"

URPRSceneComponent::URPRSceneComponent()
:	Scene(NULL)
,	m_Built(false)
,	m_Sync(true)
,	m_Plugin(NULL)
,	m_RebuildFlags(0)
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
