// RPR COPYRIGHT

#include "RPRScene.h"

#include "RprTools.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Scene/RPRViewportCameraComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "RPRPlugin.h"
#include "DesktopPlatformModule.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/Texture2DDynamic.h"
#include "TextureResource.h"

#include "RPRStats.h"

#include "LevelEditorViewport.h"

#define LOCTEXT_NAMESPACE "ARPRScene"

DEFINE_STAT(STAT_ProRender_CopyFramebuffer);

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

ARPRScene::ARPRScene()
:	m_RprContext(NULL)
,	m_RprScene(NULL)
,	m_ActiveCamera(NULL)
,	m_TriggerEndFrameResize(false)
,	m_TriggerEndFrameRebuild(false)
,	m_RendererWorker(NULL)
{
	PrimaryActorTick.bCanEverTick = true;

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
			it->HasAnyFlags(RF_Transient | RF_BeginDestroyed) ||
			!it->HasBeenCreated())
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
		return;
	}

	const uint32	cameraCount = Cameras.Num();
	for (uint32 iCamera = 0; iCamera < cameraCount; ++iCamera)
	{
		check(Cameras[iCamera] != NULL);
		if (Cameras[iCamera]->GetCameraName() == cameraName)
		{
			Cameras[iCamera]->SetAsActiveCamera();
			break;
		}
	}
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
		// TODO: Profile this
		const uint32	objectCount = SceneContent.Num();
		for (uint32 iObject = 0; iObject < objectCount; ++iObject)
		{
			if (!ensure(SceneContent[iObject] != NULL))
				continue;
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

	const bool	immediateBuild = Cast<USkyLightComponent>(srcComponent) != NULL; // Unwrapping cubemaps can't be done on another thread
	if (immediateBuild)
	{
		// Profile that, if too much, do one "immediate build object" per frame ?
		comp->Build();
		SceneContent.Add(newActor);
		return false;
	}
	else
	{
		if (typeClass == URPRCameraComponent::StaticClass())
			Cameras.Add(static_cast<URPRCameraComponent*>(comp));
		BuildQueue.Add(newActor);
	}
	return true;
}

void	ARPRScene::RemoveActor(ARPRActor *actor)
{
	check(Cast<ARPRActor>(actor) != NULL);
	check(actor->GetRootComponent() != NULL);

	SceneContent.Remove(Cast<ARPRActor>(actor));
	BuildQueue.Remove(Cast<ARPRActor>(actor));

	actor->GetRootComponent()->ConditionalBeginDestroy();
	actor->Destroy();

	TriggerFrameRebuild();
}

bool	ARPRScene::BuildViewportCamera()
{
	check(ViewportCameraComponent == NULL);

	ViewportCameraComponent = NewObject<URPRViewportCameraComponent>(this, URPRViewportCameraComponent::StaticClass());
	check(ViewportCameraComponent != NULL);

	ViewportCameraComponent->Scene = this;
	ViewportCameraComponent->SrcComponent = GetRootComponent();
	ViewportCameraComponent->RegisterComponent();

	// Profile that, if too much, do one "immediate build object" per frame ?
	if (!ViewportCameraComponent->Build())
	{
		ViewportCameraComponent->ConditionalBeginDestroy();
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
			it->HasAnyFlags(RF_Transient | RF_BeginDestroyed) ||
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
		!m_RendererWorker.IsValid())
		return false;
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	const float	megapixels = settings->MegaPixelCount;
	float		horizontalRatio = 0.0f;
	if (m_ActiveCamera == ViewportCameraComponent)
	{
		if (GEditor->GetActiveViewport() == NULL ||
			GEditor->GetActiveViewport()->GetClient() == NULL)
			return false;
		FLevelEditorViewportClient	*client = (FLevelEditorViewportClient*)GEditor->GetActiveViewport()->GetClient();
		horizontalRatio = client->AspectRatio;
	}
	else
	{
		const UCameraComponent	*camera = Cast<UCameraComponent>(m_ActiveCamera->SrcComponent);
		if (camera == NULL) // The object should have been destroyed, but ok
			return false;
		horizontalRatio = camera->AspectRatio;
	}
	const uint32	width = FGenericPlatformMath::Sqrt(megapixels * horizontalRatio * 1000000.0f);
	const uint32	height = width / horizontalRatio;

	FRPRPluginModule	*plugin = FRPRPluginModule::Get();
	check(plugin != NULL);
	UTexture2DDynamic	*texture = plugin->GetRenderTexture().Get();
	check(texture != NULL);
	if (width != texture->SizeX || height != texture->SizeY)
	{
		texture->Init(width, height, PF_R8G8B8A8);
		m_RendererWorker->ResizeFramebuffer(RenderTexture->SizeX, RenderTexture->SizeY);
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
			it->HasAnyFlags(RF_Transient | RF_BeginDestroyed) ||
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

	rprAreDevicesCompatible(TCHAR_TO_ANSI(*dllPath), false, maxCreationFlags, &creationFlags, os);
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

void	ARPRScene::OnRender(uint32 &outObjectToBuildCount)
{
	if (m_RprContext == NULL)
	{
		URPRSettings	*settings = GetMutableDefault<URPRSettings>();
		check(settings != NULL);

		// Initialize everything
		FRPRPluginModule	*plugin = FRPRPluginModule::Get();
		if (!plugin->GetRenderTexture().IsValid())
			return;// No RPR viewport created
		RenderTexture = plugin->GetRenderTexture();

		FString	cachePath = settings->RenderCachePath;
		FString	dllPath = FPaths::GameDir() + "/Binaries/Win64/Tahoe64.dll"; // To get from settings ?

		rpr_int	tahoePluginId = rprRegisterPlugin(TCHAR_TO_ANSI(*dllPath)); // Seems to be mandatory
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
		if (rprCreateContext(RPR_API_VERSION, &tahoePluginId, 1, creationFlags, NULL, TCHAR_TO_ANSI(*cachePath), &m_RprContext) != RPR_SUCCESS)
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
		SetTrace(plugin->TraceEnabled());
		UE_LOG(LogRPRScene, Log, TEXT("ProRender scene created"));

		outObjectToBuildCount = BuildScene();

		// IF in editor
		if (!BuildViewportCamera())
			return;
		// Pickup the specified camera
		if (!plugin->ActiveCameraName().IsEmpty()) // Otherwise, it'll just use the last found camera in the scene
			SetActiveCamera(plugin->ActiveCameraName());
		else
		{
			// IF in editor
			SetActiveCamera(kViewportCameraName);
		}
		TriggerFrameRebuild();

		m_RendererWorker = MakeShareable(new FRPRRendererWorker(m_RprContext, m_RprScene, RenderTexture->SizeX, RenderTexture->SizeY));
		m_RendererWorker->SetQualitySettings(settings->QualitySettings);
	}
	m_RendererWorker->SetPaused(false);
}

void	ARPRScene::OnPause()
{
	if (!m_RendererWorker.IsValid())
		return;
	m_RendererWorker->SetPaused(true);
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
	if (!m_RendererWorker.IsValid() ||
		!RenderTexture.IsValid() ||
		RenderTexture->Resource == NULL)
		return;

	FRPRPluginModule	*plugin = FRPRPluginModule::Get();
	check(plugin != NULL);
	if (plugin->RenderPaused())
		return;

	// First, launch build of queued actors on the RPR thread
	const uint32	actorCount = SceneContent.Num();
	m_RendererWorker->SyncQueue(BuildQueue, SceneContent);
	if (actorCount != SceneContent.Num())
		TriggerFrameRebuild();

	if (plugin->SyncEnabled())
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
			UTexture2DDynamic*, RenderTexture, RenderTexture.Get(),
			const uint8*, textureData, textureData,
			{
				FUpdateTextureRegion2D	region;
				region.SrcX = 0;
				region.SrcY = 0;
				region.DestX = 0;
				region.DestY = 0;
				region.Width = RenderTexture->SizeX;
				region.Height = RenderTexture->SizeY;

				const uint32	pitch = region.Width * sizeof(uint8) * 4;
				FRHITexture2D	*resource = (FRHITexture2D*)RenderTexture->Resource->TextureRHI.GetReference();
				RHIUpdateTexture2D(resource, 0, region, pitch, textureData);
			});
		FlushRenderingCommands();
		m_RendererWorker->m_DataLock.Unlock();

		plugin->m_Viewport->Draw();
	}
}

void	ARPRScene::RemoveSceneContent()
{
	for (int32 iObject = 0; iObject < SceneContent.Num(); ++iObject)
	{
		if (SceneContent[iObject] == NULL)
			continue;
		SceneContent[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		SceneContent[iObject]->Destroy();
	}
	SceneContent.Empty();
	for (int32 iObject = 0; iObject < BuildQueue.Num(); ++iObject)
	{
		if (BuildQueue[iObject] == NULL)
			continue;
		BuildQueue[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		BuildQueue[iObject]->Destroy();
	}
	BuildQueue.Empty();
	if (ViewportCameraComponent != NULL)
	{
		ViewportCameraComponent->ConditionalBeginDestroy();
		ViewportCameraComponent = NULL;
	}
	Cameras.Empty();
}

void	ARPRScene::BeginDestroy()
{
	Super::BeginDestroy();

	if (m_RendererWorker.IsValid())
	{
		m_RendererWorker->EnsureCompletion();
		m_RendererWorker = NULL; // TODO MAKE SURE TSharedPtr correctly deletes the renderer
	}
	RemoveSceneContent();
	if (m_RprScene != NULL)
	{
		rprObjectDelete(m_RprScene);
		m_RprScene = NULL;
	}
	if (m_RprContext != NULL)
	{
		rprObjectDelete(m_RprContext);
		m_RprContext = NULL;
	}
}

#undef LOCTEXT_NAMESPACE
