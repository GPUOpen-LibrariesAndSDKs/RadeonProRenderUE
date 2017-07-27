// RPR COPYRIGHT

#include "RPRScene.h"

#include "_SDK/RprTools.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Scene/RPRViewportCameraComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "HAL/PlatformFileManager.h"
#include "Slate/SceneViewport.h"

#include "RPRPlugin.h"
#include "DesktopPlatformModule.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/Texture2DDynamic.h"
#include "TextureResource.h"

#include "RPRStats.h"

#include "LevelEditorViewport.h"

#define LOCTEXT_NAMESPACE "ARPRScene"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateScene);
DEFINE_STAT(STAT_ProRender_CopyFramebuffer);

ARPRScene::ARPRScene()
	: m_RprContext(NULL)
	, m_RprScene(NULL)
	, m_ActiveCamera(NULL)
	, m_TriggerEndFrameResize(false)
	, m_TriggerEndFrameRebuild(false)
	, m_RendererWorker(NULL)
	, m_Plugin(NULL)
	, m_RenderTexture(NULL)
	, m_NumDevices(0)
{
	PrimaryActorTick.bCanEverTick = true;

	m_Plugin = &FRPRPluginModule::Load();
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

static const FString	kViewportCameraName = "Active viewport camera";
void	ARPRScene::FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames)
{
	UWorld	*world = GetWorld();

	check(world != NULL);
	for (TObjectIterator<UCameraComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			!it->HasBeenCreated() ||
			it->IsPendingKill())
			continue;
		AActor	*parent = Cast<AActor>(it->GetOwner());
		if (parent == NULL)
			continue;
		outCameraNames.Add(MakeShared<FString>(parent->GetName()));
	}
	// IF in editor:
	outCameraNames.Add(MakeShared<FString>(kViewportCameraName));
}

void	ARPRScene::SetActiveCamera(const FString &cameraName)
{
	if (m_RprContext == NULL)
		return;

	// IF in editor
	if (cameraName == kViewportCameraName)
	{
		if (ViewportCameraComponent != NULL)
			ViewportCameraComponent->SetAsActiveCamera();
	}
	else
	{
		for (int32 iCamera = 0; iCamera < Cameras.Num(); ++iCamera)
		{
			if (Cameras[iCamera] == NULL)
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
			if (SceneContent[iObject] == NULL)
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
	check(newActor != NULL);
	newActor->SrcComponent = srcComponent;

	URPRSceneComponent	*comp = NewObject<URPRSceneComponent>(newActor, typeClass);
	check(comp != NULL);
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
	check(actor->GetRootComponent() != NULL);

	if (BuildQueue.Contains(actor))
	{
		// Can be deleted now
		BuildQueue.Remove(actor);

		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(actor->GetRootComponent());
		check(comp != NULL);

		comp->ReleaseResources();
		actor->GetRootComponent()->ConditionalBeginDestroy();
		actor->Destroy();
	}
	else
	{
		check(m_RendererWorker.IsValid());
		m_RendererWorker->AddPendingKill(actor);
		SceneContent.Remove(actor);
	}
}

bool	ARPRScene::BuildViewportCamera()
{
	check(ViewportCameraComponent == NULL);

	ViewportCameraComponent = NewObject<URPRViewportCameraComponent>(this, URPRViewportCameraComponent::StaticClass());
	check(ViewportCameraComponent != NULL);

	ViewportCameraComponent->Scene = this;
	ViewportCameraComponent->SrcComponent = GetRootComponent();
	ViewportCameraComponent->RegisterComponent();

	if (!ViewportCameraComponent->Build() ||
		!ViewportCameraComponent->PostBuild())
	{
		ViewportCameraComponent->ReleaseResources();
		ViewportCameraComponent->ConditionalBeginDestroy();
		ViewportCameraComponent = NULL;
		return false;
	}
	return true;
}

uint32	ARPRScene::BuildScene()
{
	UWorld	*world = GetWorld();

	check(world != NULL);
	uint32	unbuiltObjects = 0;
	for (TObjectIterator<USceneComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			it->IsPendingKill() ||
			!it->HasBeenCreated())
			continue;
		if (Cast<UStaticMeshComponent>(*it) != NULL)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRStaticMeshComponent::StaticClass(), false);
		else if (Cast<ULightComponentBase>(*it) != NULL)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRLightComponent::StaticClass(), false);
		else if (Cast<UCameraComponent>(*it) != NULL)
			unbuiltObjects += QueueBuildRPRActor(world, *it, URPRCameraComponent::StaticClass(), false);
	}
	return unbuiltObjects;
}

bool	ARPRScene::ResizeRenderTarget()
{
	check(IsInGameThread());

	if (m_ActiveCamera == NULL ||
		!m_RendererWorker.IsValid() ||
		m_RenderTexture == NULL)
		return false;
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	const float	megapixels = settings->MegaPixelCount;
	float		horizontalRatio = 0.0f;
	if (m_ActiveCamera == ViewportCameraComponent)
		horizontalRatio = ViewportCameraComponent->GetAspectRatio();
	else
	{
		const UCameraComponent	*camera = Cast<UCameraComponent>(m_ActiveCamera->SrcComponent);
		if (camera == NULL) // The object should have been destroyed, but ok
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
	// Dont queue other actors
	if (BuildQueue.Num() > 0 || m_RendererWorker->IsBuildingObjects())
		return;
	UWorld	*world = GetWorld();

	// No usable callback to get notified when a component is added outside the editor
	// We ll have to do that for runtime apps
	// If this takes too much time, it might be better to have several lists for cameras/lights/objects
	// to avoid finding in SceneComponents
	check(world != NULL);
	bool	objectAdded = false;
	for (TObjectIterator<USceneComponent> it; it; ++it)
	{
		if (it->GetWorld() != world ||
			it->IsPendingKill() ||
			!it->HasBeenCreated())
			continue;
		if (Cast<UStaticMeshComponent>(*it) != NULL)
			objectAdded |= QueueBuildRPRActor(world, *it, URPRStaticMeshComponent::StaticClass(), true);
		else if (Cast<ULightComponentBase>(*it) != NULL)
			objectAdded |= QueueBuildRPRActor(world, *it, URPRLightComponent::StaticClass(), true);
		else if (Cast<UCameraComponent>(*it) != NULL)
			objectAdded |= QueueBuildRPRActor(world, *it, URPRCameraComponent::StaticClass(), true);
	}
}

uint32	ARPRScene::GetContextCreationFlags(const FString &dllPath)
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	rpr_creation_flags	maxCreationFlags = 0;
	if (settings->bEnableCPU)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_CPU;
	if (settings->bEnableGPU1)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU0;
	if (settings->bEnableGPU2)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU1;
	if (settings->bEnableGPU3)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU2;
	if (settings->bEnableGPU4)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU3;
	if (settings->bEnableGPU5)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU4;
	if (settings->bEnableGPU6)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU5;
	if (settings->bEnableGPU7)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU6;
	if (settings->bEnableGPU8)
		maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU7;
	rpr_creation_flags	creationFlags = 0;

	RPR_TOOLS_OS	os =
#if PLATFORM_WINDOWS
		RPRTOS_WINDOWS;
#elif PLATFORM_MAC
		RPRTOS_MC;
#elif PLATFORM_LINUX
		RPRTOS_LINUX;
#else
		return 0; // incompatible
#endif

	rprAreDevicesCompatible(TCHAR_TO_ANSI(*dllPath), TCHAR_TO_ANSI(*settings->RenderCachePath), false, maxCreationFlags, &creationFlags, os);
	if (creationFlags > 0)
	{
		if (creationFlags != RPR_CREATION_FLAGS_ENABLE_CPU)
			creationFlags &= ~RPR_CREATION_FLAGS_ENABLE_CPU;

		FString	usedDevices = "Device(s) used for ProRender: ";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_CPU)
			usedDevices += "[CPU]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU0)
			usedDevices += "[GPU1]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU1)
			usedDevices += "[GPU2]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU2)
			usedDevices += "[GPU3]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU3)
			usedDevices += "[GPU4]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU4)
			usedDevices += "[GPU5]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU5)
			usedDevices += "[GPU6]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU6)
			usedDevices += "[GPU7]";
		if (creationFlags & RPR_CREATION_FLAGS_ENABLE_GPU7)
			usedDevices += "[GPU8]";

		usedDevices += ".";

		UE_LOG(LogRPRScene, Log, TEXT("%s"), *usedDevices);
	}
	return creationFlags;
}

bool	ARPRScene::RPRThread_Rebuild()
{
	bool			restartRender = false;
	for (int32 iObject = 0; iObject < SceneContent.Num(); ++iObject)
	{
		if (SceneContent[iObject] == NULL)
		{
			SceneContent.RemoveAt(iObject--);
			continue;
		}
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(SceneContent[iObject]->GetRootComponent());
		check(comp != NULL);

		restartRender |= comp->RPRThread_Update();
	}
	if (ViewportCameraComponent != NULL)
		restartRender |= ViewportCameraComponent->RPRThread_Update();
	return restartRender;
}

void	ARPRScene::OnRender(uint32 &outObjectToBuildCount)
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (m_RprContext == NULL)
	{
		if (!ensure(m_Plugin->GetRenderTexture() != NULL))
			return;

		const FString	dllPath = FPaths::GameDir() + "/Binaries/Win64/Tahoe64.dll"; // To get from settings ?
		rpr_int			tahoePluginId = rprRegisterPlugin(TCHAR_TO_ANSI(*dllPath)); // Seems to be mandatory
		if (tahoePluginId == -1)
		{
			UE_LOG(LogRPRScene, Error, TEXT("\"%s\" not registered by \"%s\" path."), "Tahoe64.dll", *dllPath);
			return;
		}
		uint32	creationFlags = GetContextCreationFlags(dllPath);
		if (creationFlags == 0)
		{
			UE_LOG(LogRPRScene, Error, TEXT("Couldn't find a compatible device"));
			return;
		}

		m_NumDevices = 0;
		for (uint32 s = RPR_CREATION_FLAGS_ENABLE_GPU7; s; s >>= 1)
			m_NumDevices += (creationFlags & s) != 0;

		if (rprCreateContext(RPR_API_VERSION, &tahoePluginId, 1, creationFlags, NULL, TCHAR_TO_ANSI(*settings->RenderCachePath), &m_RprContext) != RPR_SUCCESS ||
			rprContextSetParameter1u(m_RprContext, "aasamples", m_NumDevices) != RPR_SUCCESS ||
			rprContextSetParameter1u(m_RprContext, "preview", 1) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRScene, Error, TEXT("RPR Context creation failed: check your OpenCL runtime and driver versions."));
			return;
		}
		if (rprContextSetActivePlugin(m_RprContext, tahoePluginId) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRScene, Error, TEXT("RPR Context setup failed: Couldn't set tahoe plugin."));
			return;
		}
		UE_LOG(LogRPRScene, Log, TEXT("ProRender context initialized"));

		if (rprContextCreateScene(m_RprContext, &m_RprScene) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRScene, Error, TEXT("RPR Scene creation failed"));
			return;
		}
		if (rprContextSetScene(m_RprContext, m_RprScene) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRScene, Error, TEXT("RPR Scene setup failed"));
			return;
		}

		m_RenderTexture = m_Plugin->GetRenderTexture();

		// Set the master material mappings file.
		m_materialLibrary.LoadMasterMappingFile(TCHAR_TO_ANSI(*(FPaths::GameDir() + "/Plugins/RPRPlugin/Content/MaterialMappings.xml")));

		// Add material search paths to material library.
		m_materialLibrary.AddMaterialSearchPaths(TCHAR_TO_ANSI(*settings->MaterialsSearchPaths));

		// Add image search paths to material library.
		m_materialLibrary.AddImageSearchPaths(TCHAR_TO_ANSI(*settings->ImageSearchPaths));

		// Initialize material library for UE material to RPR replacement.	 
		m_materialLibrary.AddDirectory(TCHAR_TO_ANSI(*(FPaths::GameDir() + "/Plugins/RPRPlugin/Content/Materials")));

		// Initialize the UMSControl
		m_UMSControl.LoadControlData(TCHAR_TO_ANSI(*(FPaths::GameDir() + "/Plugins/RPRPlugin/Content/UMSControl.xml")));
		UE_LOG(LogRPRScene, Log, TEXT("ProRender scene created"));
	}

	if (!m_RendererWorker.IsValid())
	{
		check(m_RprContext != NULL);

		SetTrace(settings->bTrace);

		outObjectToBuildCount = BuildScene();

		// IF in editor
		if (!BuildViewportCamera())
			return;
		// Pickup the specified camera
		if (!m_Plugin->ActiveCameraName().IsEmpty()) // Otherwise, it'll just use the last found camera in the scene
			SetActiveCamera(m_Plugin->ActiveCameraName());
		else
		{
			// IF in editor
			SetActiveCamera(kViewportCameraName);
		}
		SetOrbit(m_Plugin->IsOrbitting());
		TriggerFrameRebuild();

		m_RendererWorker = MakeShareable(new FRPRRendererWorker(m_RprContext, m_RprScene, m_RenderTexture->SizeX, m_RenderTexture->SizeY, m_NumDevices, this));
		m_RendererWorker->SetQualitySettings(settings->QualitySettings);
	}
	m_RendererWorker->SetPaused(false);
}

void	ARPRScene::Rebuild()
{
	if (!m_RendererWorker.IsValid())
		return; // Nothing to rebuild
				// Here, the renderer worker will pause itself at the next iteration
				// So, wait for it
	m_RendererWorker->EnsureCompletion();
	m_RendererWorker = NULL; // TODO MAKE SURE TSharedPtr correctly deletes the renderer

	// Once the RPR thread is deleted, clean all scene resources
	RemoveSceneContent(false, false);

	rprContextClearMemory(m_RprContext);
	// NOTE: Right now, keeps mesh cache
}

void	ARPRScene::OnPause()
{
	if (!m_RendererWorker.IsValid())
		return;
	m_RendererWorker->SetPaused(true);
}

void	ARPRScene::SetOrbit(bool orbit)
{
	if (m_ActiveCamera == NULL)
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
	if (m_ActiveCamera == NULL)
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
	if (m_RprContext == NULL)
		return;
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	if (settings == NULL)
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
		if (rprContextSetParameterString(NULL, "tracingfolder", TCHAR_TO_ANSI(*tracePath)) != RPR_SUCCESS ||
			rprContextSetParameter1u(NULL, "tracing", trace) != RPR_SUCCESS)
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

void	ARPRScene::OnSave()
{
	if (!m_RendererWorker.IsValid())
		return; // Nothing to save
	IDesktopPlatform	*desktopPlatform = FDesktopPlatformModule::Get();
	if (desktopPlatform == NULL)
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
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(NULL),
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
		m_RendererWorker->SaveToFile(saveFilename); // UE4 already prompts the user to override existing files
}

void	ARPRScene::Tick(float deltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateScene);
	if (!m_RendererWorker.IsValid() ||
		m_RenderTexture == NULL ||
		m_RenderTexture->Resource == NULL ||
		!m_Plugin->m_Viewport.IsValid())
		return;

	if (m_Plugin->RenderPaused())
		return;

	// First, launch build of queued actors on the RPR thread
	const uint32	actorCount = SceneContent.Num();
	m_RendererWorker->SyncQueue(BuildQueue, SceneContent);
	if (actorCount != SceneContent.Num())
		TriggerFrameRebuild();

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (settings->bSync)
		RefreshScene();

	if (m_TriggerEndFrameResize)
		ResizeRenderTarget();
	if (m_TriggerEndFrameRebuild)
	{
		// Restart render, skip frame copy
		if (m_RendererWorker->RestartRender()) // Trylock, might fail
			m_TriggerEndFrameRebuild = false;
	}
	else if (m_RendererWorker->Flush())
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
			});
		FlushRenderingCommands();
		m_RendererWorker->m_DataLock.Unlock();

		m_Plugin->m_Viewport->Draw();
	}
}

void	ARPRScene::RemoveSceneContent(bool clearScene, bool clearCache)
{
	check(!m_RendererWorker.IsValid()); // RPR Thread HAS to be destroyed
	for (int32 iObject = 0; iObject < SceneContent.Num(); ++iObject)
	{
		if (SceneContent[iObject] == NULL)
			continue;
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(SceneContent[iObject]->GetRootComponent());
		check(comp != NULL);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();

		SceneContent[iObject]->Destroy();
	}
	SceneContent.Empty();
	for (int32 iObject = 0; iObject < BuildQueue.Num(); ++iObject)
	{
		if (BuildQueue[iObject] == NULL)
			continue;
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(BuildQueue[iObject]->GetRootComponent());
		check(comp != NULL);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();
		BuildQueue[iObject]->Destroy();
	}
	BuildQueue.Empty();
	if (ViewportCameraComponent != NULL)
	{
		ViewportCameraComponent->ReleaseResources();
		ViewportCameraComponent->ConditionalBeginDestroy();
		ViewportCameraComponent = NULL;
	}
	Cameras.Empty();

	if (m_RprScene != NULL)
	{
		if (clearCache)
			URPRStaticMeshComponent::ClearCache(m_RprScene);
		if (clearScene)
			rprSceneClear(m_RprScene);
	}
}

void	ARPRScene::ImmediateRelease(URPRSceneComponent *component)
{
	ARPRActor	*actor = Cast<ARPRActor>(component->GetOwner());

	check(component != NULL);
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
		m_RendererWorker = NULL; // TODO MAKE SURE TSharedPtr correctly deletes the renderer
	}
	RemoveSceneContent(true, true);
	if (m_RprScene != NULL)
	{
		rprObjectDelete(m_RprScene);
		m_RprScene = NULL;
	}
	if (m_RprContext != NULL)
	{
		rprContextClearMemory(m_RprContext);
		rprObjectDelete(m_RprContext);
		m_RprContext = NULL;
	}
}

#undef LOCTEXT_NAMESPACE
