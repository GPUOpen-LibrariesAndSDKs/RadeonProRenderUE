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

#include "Scene/RPRScene.h"

#include "RprTools.h"

#include "Scene/RPRActor.h"
#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Scene/RPRCameraComponent.h"
#include "Scene/RPRViewportCameraComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "HAL/PlatformFileManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SceneViewport.h"

#include "RPRPlugin.h"

#include "UObject/UObjectIterator.h"

#if WITH_EDITOR
#	include "DesktopPlatformModule.h"
#endif

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/Texture2DDynamic.h"
#include "TextureResource.h"

#include "RPRStats.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRXHelpers.h"
#include "Helpers/RPRIHelpers.h"
#include "RenderingThread.h"
#include "RPR_SDKModule.h"
#include "Components/StaticMeshComponent.h"
#include "ContextHelper.h"
#include "RPRCoreModule.h"

#define LOCTEXT_NAMESPACE "ARPRScene"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateScene);
DEFINE_STAT(STAT_ProRender_CopyFramebuffer);


ARPRScene::ARPRScene()
	: m_RprScene(nullptr)
	, m_ActiveCamera(nullptr)
	, m_TriggerEndFrameResize(false)
	, m_TriggerEndFrameRebuild(false)
	, m_RendererWorker(nullptr)
	, m_Plugin(nullptr)
	, m_RenderTexture(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	m_Plugin = &FRPRPluginModule::Load();
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	RPRCoreResources = IRPRCore::GetResources();
}

static const FString	kViewportCameraName = "Active viewport camera";

void	ARPRScene::FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames)
{
	UWorld	*world = GetWorld();

	check(world != nullptr);
	for (TObjectIterator<UCameraComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			!it->HasBeenCreated() ||
			it->IsPendingKill())
			continue;
		AActor	*parent = Cast<AActor>(it->GetOwner());
		if (parent == nullptr)
			continue;
		outCameraNames.Add(MakeShared<FString>(parent->GetName()));
	}
	outCameraNames.Add(MakeShared<FString>(kViewportCameraName));
}

void	ARPRScene::SetActiveCamera(const FString &cameraName)
{
	if (!RPRCoreResources->IsInitialized())
		return;

	if (cameraName == kViewportCameraName)
	{
		if (ViewportCameraComponent != nullptr)
			ViewportCameraComponent->SetAsActiveCamera();
	}
	else
	{
		for (int32 iCamera = 0; iCamera < Cameras.Num(); ++iCamera)
		{
			if (Cameras[iCamera] == nullptr)
			{
				Cameras.RemoveAt(iCamera--);
				continue;
			}
			if (Cameras[iCamera]->GetCameraName() == cameraName)
			{
				Cameras[iCamera]->SetAsActiveCamera();
				break;
			}
		}
	}
	SetOrbit(m_Plugin->IsOrbitting());
}

void	ARPRScene::SetQualitySettings(ERPRQualitySettings qualitySettings)
{
	if (!m_RendererWorker.IsValid())
		return;
	m_RendererWorker->SetQualitySettings(qualitySettings);
}

uint32	ARPRScene::GetRenderIteration() const
{
	if (!m_RendererWorker.IsValid())
		return 0;
	return m_RendererWorker->Iteration();
}

bool	ARPRScene::QueueBuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass, bool checkIfContained)
{
	if (checkIfContained)
	{
		for (int32 iObject = 0; iObject < SceneContent.Num(); ++iObject)
		{
			if (SceneContent[iObject] == nullptr)
			{
				SceneContent.RemoveAt(iObject--);
				continue;
			}
			if (SceneContent[iObject]->SrcComponent == srcComponent)
				return false;
		}
	}

	FActorSpawnParameters	params;
	params.ObjectFlags = RF_Public | RF_Transactional;

	ARPRActor	*newActor = world->SpawnActor<ARPRActor>(ARPRActor::StaticClass(), params);
	check(newActor != nullptr);
	newActor->SrcComponent = srcComponent;

	URPRSceneComponent	*comp = NewObject<URPRSceneComponent>(newActor, typeClass);
	check(comp != nullptr);
	comp->SrcComponent = srcComponent;
	comp->Scene = this;
	newActor->SetRootComponent(comp);
	newActor->Component = comp;
	comp->RegisterComponent();

	if (typeClass == URPRCameraComponent::StaticClass())
		Cameras.Add(static_cast<URPRCameraComponent*>(comp));
	BuildQueue.Add(newActor);
	return true;
}

void	ARPRScene::RemoveActor(ARPRActor *actor)
{
	check(actor->GetRootComponent() != nullptr);

	if (BuildQueue.Contains(actor))
	{
		// Can be deleted now
		BuildQueue.Remove(actor);

		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(actor->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
		actor->GetRootComponent()->ConditionalBeginDestroy();
		actor->Destroy();
	}
	else
	{
		check(m_RendererWorker.IsValid());
		m_RendererWorker->AddPendingKill(actor);
		SceneContent.Remove(actor);
		PendingKillQueue.AddUnique(actor);
	}
}

bool	ARPRScene::BuildViewportCamera()
{
	check(ViewportCameraComponent == nullptr);

	ViewportCameraComponent = NewObject<URPRViewportCameraComponent>(this, URPRViewportCameraComponent::StaticClass());
	check(ViewportCameraComponent != nullptr);

	ViewportCameraComponent->Scene = this;
	ViewportCameraComponent->SrcComponent = GetRootComponent();
	ViewportCameraComponent->RegisterComponent();

	if (!ViewportCameraComponent->Build() ||
		!ViewportCameraComponent->PostBuild())
	{
		ViewportCameraComponent->ReleaseResources();
		ViewportCameraComponent->ConditionalBeginDestroy();
		ViewportCameraComponent = nullptr;
		return false;
	}
	return true;
}

uint32	ARPRScene::BuildScene()
{
	UE_LOG(LogRPRScene, Verbose, TEXT("Build RPR scene"));

	UWorld	*world = GetWorld();

	check(world != nullptr);
	uint32	unbuiltObjects = 0;
	for (TObjectIterator<USceneComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			it->IsPendingKill() ||
			!it->HasBeenCreated())
			continue;
		if (Cast<UStaticMeshComponent>(*it) != nullptr)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRStaticMeshComponent::StaticClass(), false);
		else if (Cast<ULightComponentBase>(*it) != nullptr)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRLightComponent::StaticClass(), false);
		else if (Cast<UCameraComponent>(*it) != nullptr)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRCameraComponent::StaticClass(), false);
	}
	return unbuiltObjects;
}

bool	ARPRScene::ResizeRenderTarget()
{
	check(IsInGameThread());

	if (m_ActiveCamera == nullptr ||
		!m_RendererWorker.IsValid() ||
		m_RenderTexture == nullptr)
		return false;
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	const float	megapixels = settings->MegaPixelCount;
	float		horizontalRatio = 0.0f;
	if (m_ActiveCamera == ViewportCameraComponent)
		horizontalRatio = ViewportCameraComponent->GetAspectRatio();
	else
	{
		const UCameraComponent	*camera = Cast<UCameraComponent>(m_ActiveCamera->SrcComponent);
		if (camera == nullptr) // The object should have been destroyed, but ok
			return false;
		horizontalRatio = camera->AspectRatio;
	}
	const uint32	width = FGenericPlatformMath::Sqrt(megapixels * horizontalRatio * 1000000.0f);
	const uint32	height = width / horizontalRatio;

	if (width != m_RenderTexture->SizeX || height != m_RenderTexture->SizeY)
	{
		m_RenderTexture->Init(width, height, PF_R8G8B8A8, true);
		m_RendererWorker->ResizeFramebuffer(m_RenderTexture->SizeX, m_RenderTexture->SizeY);
	}
	m_TriggerEndFrameResize = false;
	return true;
}

void	ARPRScene::RefreshScene()
{
	// Don't queue other actors
	if (BuildQueue.Num() > 0 || m_RendererWorker->IsBuildingObjects())
	{
		return;
	}

	UWorld	*world = GetWorld();

	// No usable callback to get notified when a component is added outside the editor
	// We ll have to do that for runtime apps
	// If this takes too much time, it might be better to have several lists for cameras/lights/objects
	// to avoid finding in SceneComponents
	check(world != nullptr);
	bool	objectAdded = false;
	for (TObjectIterator<USceneComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			it->IsPendingKill() ||
			!it->HasBeenCreated())
			continue;
		
		if (Cast<UStaticMeshComponent>(*it) != nullptr)
		{
			objectAdded |= QueueBuildRPRActor(world, *it, URPRStaticMeshComponent::StaticClass(), true);
		}
		else if (Cast<ULightComponentBase>(*it) != nullptr)
		{
			objectAdded |= QueueBuildRPRActor(world, *it, URPRLightComponent::StaticClass(), true);
		}
		else if (Cast<UCameraComponent>(*it) != nullptr)
		{
			objectAdded |= QueueBuildRPRActor(world, *it, URPRCameraComponent::StaticClass(), true);
		}
	}
}

bool	ARPRScene::RPRThread_Rebuild()
{
	bool			restartRender = false;
	for (int32 iObject = 0; iObject < SceneContent.Num(); ++iObject)
	{
		if (SceneContent[iObject] == nullptr)
		{
			SceneContent.RemoveAt(iObject--);
			continue;
		}
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(SceneContent[iObject]->GetRootComponent());
		check(comp != nullptr);

		restartRender |= comp->RPRThread_Update();
	}
	if (ViewportCameraComponent != nullptr)
		restartRender |= ViewportCameraComponent->RPRThread_Update();
	return restartRender;
}

void	ARPRScene::OnRender(uint32 &outObjectToBuildCount)
{
	UE_LOG(LogRPRScene, VeryVerbose, TEXT("Render RPR scene"));

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	if (m_RenderTexture == nullptr)
	{
		InitializeRPRRendering();
	}

	if (!m_RendererWorker.IsValid())
	{
		SetTrace(settings->bTrace);

		outObjectToBuildCount = BuildScene();

		// IF in editor
		if (!BuildViewportCamera())
		{
			return;
		}

		// Pickup the specified camera
		if (!m_Plugin->ActiveCameraName().IsEmpty()) // Otherwise, it'll just use the last found camera in the scene
		{
			SetActiveCamera(m_Plugin->ActiveCameraName());
		}
		else
		{
			// IF in editor
			SetActiveCamera(kViewportCameraName);
		}
		SetOrbit(m_Plugin->IsOrbitting());
		TriggerFrameRebuild();

		m_RendererWorker = MakeShareable(
			new FRPRRendererWorker(
				RPRCoreResources->GetRPRContext(), 
				m_RprScene, 
				m_RenderTexture->SizeX, m_RenderTexture->SizeY, 
				RPRCoreResources->GetNumDevicesCompatible(),
				this
			));

		m_RendererWorker->SetQualitySettings(settings->QualitySettings);
	}
	m_RendererWorker->SetPaused(false);
}

void	ARPRScene::InitializeRPRRendering()
{
	m_RenderTexture = m_Plugin->GetRenderTexture();

	RPR::FContext rprContext = IRPRCore::GetResources()->GetRPRContext();
	RPR::FResult result = RPR::Context::CreateScene(rprContext, m_RprScene);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRScene, Error, TEXT("Couldn't create RPR scene"));
	}
}

void	ARPRScene::Rebuild()
{
	if (!m_RendererWorker.IsValid())
		return; // Nothing to rebuild
				// Here, the renderer worker will pause itself at the next iteration
				// So, wait for it
	m_RendererWorker->EnsureCompletion();
	m_RendererWorker = nullptr; // TODO MAKE SURE TSharedPtr correctly deletes the renderer

	// Once the RPR thread is deleted, clean all scene resources
	RemoveSceneContent(false, false);

	RPRCoreResources->GetRPRMaterialLibrary()->ClearCache();
	RPRCoreResources->GetRPRImageManager()->ClearCache();

	RPR::FResult result = RPR::Context::ClearMemory(RPRCoreResources->GetRPRContext());
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRScene, Error, TEXT("Cannot clear RPR context memory"));
	}

	// NOTE: Right now, keeps mesh cache
}

void	ARPRScene::OnPause()
{
	UE_LOG(LogRPRScene, VeryVerbose, TEXT("Pause RPR rendering"));

	if (!m_RendererWorker.IsValid())
		return;
	m_RendererWorker->SetPaused(true);
}

void	ARPRScene::SetOrbit(bool orbit)
{
	if (m_ActiveCamera == nullptr)
		return;
	if (m_ActiveCamera == ViewportCameraComponent)
		ViewportCameraComponent->SetOrbit(orbit);
	else
	{
		URPRCameraComponent	*comp = Cast<URPRCameraComponent>(m_ActiveCamera);
		comp->SetOrbit(orbit);
	}
}

void	ARPRScene::StartOrbitting(const FIntPoint &mousePos)
{
	if (m_ActiveCamera == nullptr)
		return;
	if (m_ActiveCamera == ViewportCameraComponent)
		ViewportCameraComponent->StartOrbitting(mousePos);
	else
	{
		URPRCameraComponent	*comp = Cast<URPRCameraComponent>(m_ActiveCamera);
		comp->StartOrbitting(mousePos);
	}
}

void	ARPRScene::SetTrace(bool trace)
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	if (settings == nullptr)
		return;

	FString	tracePath = settings->TraceFolder;
	if (tracePath.IsEmpty())
		return;

	if (!FPaths::DirectoryExists(tracePath))
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*tracePath))
		{
			UE_LOG(LogRPRScene, Warning, TEXT("Couldn't enable tracing: Couldn't create directory tree %s"), *tracePath);
			return;
		}
	}
	if (m_RendererWorker.IsValid())
		m_RendererWorker->SetTrace(trace, tracePath);
	else
	{
		RPR::FContext context = RPRCoreResources->GetRPRContext();
		if (context != nullptr)
		{
			if (rprContextSetParameterString(context, "tracingfolder", TCHAR_TO_ANSI(*tracePath)) != RPR_SUCCESS ||
				rprContextSetParameter1u(context, "tracing", trace) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRScene, Warning, TEXT("Couldn't enable RPR trace."));
				return;
			}
			if (trace)
			{
				UE_LOG(LogRPRScene, Log, TEXT("RPR Tracing enabled"));
			}
			else
			{
				UE_LOG(LogRPRScene, Log, TEXT("RPR Tracing disabled"));
			}
		}
	}
}

void	ARPRScene::OnSave()
{
#if WITH_EDITOR
	if (!m_RendererWorker.IsValid())
		return; // Nothing to save
	IDesktopPlatform	*desktopPlatform = FDesktopPlatformModule::Get();
	if (desktopPlatform == nullptr)
		return;

	static FString	kSaveDialogTitle = "Save Radeon ProRender Framebuffer";
	static FString	kFileTypes = TEXT("Targa (*.TGA)|*.tga"
		"|Windows Bitmap (*.BMP)|*.bmp"
		"|PNG (*.PNG)|*.png"
		"|JPG (*.JPG)|*.jpg"
		"|FireRender Scene (*.FRS)|*.frs"
		"|All files (*TGA;*.BMP;*.PNG;*.JPG;*.FRS)|*tga;*.bmp;*.png;*.jpg;*.frs");

	TArray<FString>		saveFilenames;
	const bool	save = desktopPlatform->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		kSaveDialogTitle,
		*LastSavedExportPath,
		*LastSavedFilename,
		*kFileTypes,
		EFileDialogFlags::None,
		saveFilenames);

	if (saveFilenames.Num() == 0)
		return;
	FString	saveFilename = FPaths::ChangeExtension(saveFilenames[0], FPaths::GetExtension(saveFilenames[0]).ToLower());
	FString	extension = FPaths::GetExtension(saveFilename);
	if (extension != "tga" && extension != "bmp" && extension != "png" && extension != "jpg" && extension != "frs")
		return;

	LastSavedExportPath = saveFilename;
	LastSavedFilename = FPaths::GetCleanFilename(saveFilename);
	if (save)
#else
		const FString	saveFilename = "C:/ProRender-Export.png"; // fix that or use default export path in settings
#endif
	{
		m_RendererWorker->SaveToFile(saveFilename); // UE4 already prompts the user to override existing files
	}
}

void	ARPRScene::CheckPendingKills()
{
	const bool	canSafelyKill = !m_RendererWorker.IsValid();
	for (int32 iKill = 0; iKill < PendingKillQueue.Num(); ++iKill)
	{
		AActor	*actor = PendingKillQueue[iKill];
		if (actor == nullptr)
		{
			PendingKillQueue.RemoveAt(iKill);
			break;
		}
		if (canSafelyKill ||
			m_RendererWorker->CanSafelyKill(PendingKillQueue[iKill]))
		{
			check(Cast<URPRSceneComponent>(actor->GetRootComponent()) != nullptr);
			actor->GetRootComponent()->ConditionalBeginDestroy();
			actor->Destroy();
			PendingKillQueue.RemoveAt(iKill);
		}
	}
}

void	ARPRScene::Tick(float deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateScene);
	if (!m_RendererWorker.IsValid() ||
		m_RenderTexture == nullptr ||
		m_RenderTexture->Resource == nullptr ||
		!m_Plugin->m_Viewport.IsValid())
		return;

	if (m_Plugin->RenderPaused())
		return;

	CheckPendingKills();

	// First, launch build of queued actors on the RPR thread
	const uint32	actorCount = SceneContent.Num();
	m_RendererWorker->SyncQueue(BuildQueue, SceneContent);
	if (actorCount != SceneContent.Num())
		TriggerFrameRebuild();

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	if (settings->bSync)
	{
		RefreshScene();
	}

	if (m_TriggerEndFrameResize)
	{
		ResizeRenderTarget();
	}

	if (m_TriggerEndFrameRebuild)
	{
		// Restart render, skip frame copy
		if (m_RendererWorker->RestartRender()) // Trylock, might fail
		{
			m_TriggerEndFrameRebuild = false;
		}
	}
	else if (m_RendererWorker->Flush())
	{
		DrawRPRBufferToViewport();
	}
}

void ARPRScene::DrawRPRBufferToViewport()
{
	CopyRPRRenderBufferToViewportRenderTexture();
	m_Plugin->m_Viewport->Draw();
}

void ARPRScene::CopyRPRRenderBufferToViewportRenderTexture()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_CopyFramebuffer);

	m_RendererWorker->m_DataLock.Lock();
	const uint8	*textureData = m_RendererWorker->GetFramebufferData();
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		UpdateDynamicTextureCode,
		UTexture2DDynamic*, renderTexture, m_RenderTexture,
		const uint8*, textureData, textureData,
		{
			FUpdateTextureRegion2D	region;
			region.SrcX = 0;
			region.SrcY = 0;
			region.DestX = 0;
			region.DestY = 0;
			region.Width = renderTexture->SizeX;
			region.Height = renderTexture->SizeY;

			const uint32	pitch = region.Width * sizeof(uint8) * 4;
			FRHITexture2D	*resource = (FRHITexture2D*)renderTexture->Resource->TextureRHI.GetReference();
			RHIUpdateTexture2D(resource, 0, region, pitch, textureData);
		}
	);

	FlushRenderingCommands();
	m_RendererWorker->m_DataLock.Unlock();
}

void	ARPRScene::RemoveSceneContent(bool clearScene, bool clearCache)
{
	check(!m_RendererWorker.IsValid()); // RPR Thread HAS to be destroyed
	DestroyRPRActors(SceneContent);
	DestroyRPRActors(BuildQueue);

	if (ViewportCameraComponent != nullptr)
	{
		ViewportCameraComponent->ReleaseResources();
		ViewportCameraComponent->ConditionalBeginDestroy();
		ViewportCameraComponent = nullptr;
	}
	Cameras.Empty();

	CheckPendingKills();
	check(PendingKillQueue.Num() == 0);

	if (m_RprScene != nullptr)
	{
		if (clearCache)
		{
			RPRCoreResources->GetRPRMaterialLibrary()->ClearCache();

			try
			{
				URPRStaticMeshComponent::ClearCache(m_RprScene);
			}
			catch (std::exception)
			{
				UE_LOG(LogRPRScene, Warning, TEXT("RPRScene could not clean the materials correctly!"));
			}
		}

		if (clearScene)
		{
			RPR::SceneClear(m_RprScene);
		}
	}
}

void	ARPRScene::DestroyRPRActors(TArray<ARPRActor*>& Actors)
{
	for (int32 iObject = 0; iObject < Actors.Num(); ++iObject)
	{
		if (Actors[iObject] == nullptr)
		{
			continue;
		}

		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(Actors[iObject]->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();
		Actors[iObject]->Destroy();
	}
	Actors.Empty();
}

void	ARPRScene::ImmediateRelease(URPRSceneComponent *component)
{
	ARPRActor	*actor = Cast<ARPRActor>(component->GetOwner());

	check(component != nullptr);
	if (BuildQueue.Contains(actor))
	{
		// Can be deleted now
		BuildQueue.Remove(actor);

		component->ReleaseResources();
	}
	else
	{
		SceneContent.Remove(actor);

		if (m_RendererWorker.IsValid())
			m_RendererWorker->SafeRelease_Immediate(component);
		else
			component->ReleaseResources();

		m_TriggerEndFrameRebuild = true;
	}
}

void	ARPRScene::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (m_RendererWorker.IsValid())
	{
		m_RendererWorker->EnsureCompletion();
		m_RendererWorker = nullptr; // TODO MAKE SURE TSharedPtr correctly deletes the renderer
	}
	RemoveSceneContent(true, true);

	if (m_RprScene != nullptr)
	{
		RPR::DeleteObject(m_RprScene);
		m_RprScene = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
