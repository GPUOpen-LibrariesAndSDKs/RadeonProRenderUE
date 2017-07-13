// RPR COPYRIGHT

#include "RPRScene.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "RPRPlugin.h"
#include "DesktopPlatformModule.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/Texture2DDynamic.h"
#include "TextureResource.h"

#define LOCTEXT_NAMESPACE "ARPRScene"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

ARPRScene::ARPRScene()
:	m_RprContext(NULL)
,	m_RprScene(NULL)
,	m_ActiveCamera(NULL)
,	m_TriggerEndFrameRebuild(false)
,	m_Synchronize(false)
,	m_RendererWorker(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
}

void	ARPRScene::FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames)
{
	if (m_RprContext != NULL)
	{
		// Scene was already built
		const uint32	cameraCount = Cameras.Num();
		for (uint32 iCamera = 0; iCamera < cameraCount; ++iCamera)
		{
			check(Cameras[iCamera] != NULL);
			FString	name = Cameras[iCamera]->GetCameraName();

			if (name.IsEmpty())
				continue;
			outCameraNames.Add(MakeShared<FString>(name));
		}
	}
	else
	{
		// Scene isn't built yet
		UWorld	*world = GetWorld();

		check(world != NULL);
		for (TObjectIterator<USceneComponent> it; it; ++it)
		{
			if (it->GetWorld() != world)
				continue;
			if (Cast<UCineCameraComponent>(*it) == NULL)
				continue;
			AActor	*parent = Cast<AActor>(it->GetOwner());
			if (parent == NULL)
				continue;
			outCameraNames.Add(MakeShared<FString>(parent->GetName()));
		}
	}
}

void	ARPRScene::SetActiveCamera(const FString &cameraName)
{
	if (m_RprContext == NULL)
		return;
	const uint32	cameraCount = Cameras.Num();
	for (uint32 iCamera = 0; iCamera < cameraCount; ++iCamera)
	{
		check(Cameras[iCamera] != NULL);
		if (Cameras[iCamera]->GetCameraName() == cameraName)
		{
			Cameras[iCamera]->SetActiveCamera();
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

void	ARPRScene::BuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass)
{
	FActorSpawnParameters	params;
	params.ObjectFlags = RF_Public | RF_Transactional;

	ARPRActor	*newActor = world->SpawnActor<ARPRActor>(ARPRActor::StaticClass(), params);
	check(newActor != NULL);

	URPRSceneComponent	*comp = NewObject<URPRSceneComponent>(newActor, typeClass);
	check(comp != NULL);
	comp->SrcComponent = srcComponent;
	comp->Scene = this;
	newActor->SetRootComponent(comp);
	comp->RegisterComponent();

	if (!comp->Build())
	{
		world->DestroyActor(newActor);
		return;
	}

	if (typeClass == URPRCameraComponent::StaticClass())
		Cameras.Add(static_cast<URPRCameraComponent*>(comp));
	SceneContent.Add(newActor);
}

void	ARPRScene::BuildScene()
{
	UWorld	*world = GetWorld();

	check(world != NULL);
	for (TObjectIterator<USceneComponent> it; it; ++it)
	{
		if (it->GetWorld() != world)
			continue;
		if (Cast<UStaticMeshComponent>(*it) != NULL)
			BuildRPRActor(world, *it, URPRStaticMeshComponent::StaticClass());
		else if (Cast<ULightComponentBase>(*it) != NULL)
			BuildRPRActor(world, *it, URPRLightComponent::StaticClass());
		else if (Cast<UCineCameraComponent>(*it) != NULL)
			BuildRPRActor(world, *it, URPRCameraComponent::StaticClass());
	}

	// Pickup the specified camera
	FRPRPluginModule	&plugin = FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin");
	if (!plugin.m_ActiveCameraName.IsEmpty()) // Otherwise, it'll just use the last found camera in the scene
		SetActiveCamera(plugin.m_ActiveCameraName);
}

void	ARPRScene::OnRender()
{
	if (m_RprContext == NULL)
	{
		URPRSettings	*settings = GetMutableDefault<URPRSettings>();
		check(settings != NULL);

		// Initialize everything
		FRPRPluginModule	&plugin = FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin");
		if (!plugin.GetRenderTexture().IsValid())
			return;// No RPR viewport created
		RenderTexture = plugin.GetRenderTexture();

		FString	cachePath = settings->RenderCachePath;
		FString	dllPath = FPaths::GameDir() + "/Binaries/Win64/Tahoe64.dll"; // To get from settings ?
		uint32	creationFlags = RPR_CREATION_FLAGS_ENABLE_GPU0; // for now

		rpr_int	tahoePluginId = rprRegisterPlugin(TCHAR_TO_ANSI(*dllPath)); // Seems to be mandatory
		if (tahoePluginId == -1)
		{
			UE_LOG(LogRPRScene, Error, TEXT("\"%s\" not registered by \"%s\" path."), "Tahoe64.dll", *dllPath);
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
		SetTrace(plugin.TraceEnabled());
		UE_LOG(LogRPRScene, Log, TEXT("ProRender scene created"));

	}

	// For now, always rebuild the scene
	RemoveSceneContent();
	BuildScene();
	TriggerFrameRebuild();

	if (!m_RendererWorker.IsValid())
	{
		FRPRPluginModule	&plugin = FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin");

		m_RendererWorker = MakeShareable(new FRPRRendererWorker(m_RprContext, RenderTexture->SizeX, RenderTexture->SizeY));
		m_RendererWorker->SetQualitySettings(plugin.m_QualitySettings);
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

void	ARPRScene::OnTriggerSync()
{
	m_Synchronize = !m_Synchronize;
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
									  "|All files (*tga;*.bmp;*.png;*.jpg)|*tga;*.bmp;*.png;*.jpg");

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
	if (extension != "tga" && extension != "bmp" && extension != "png" && extension != "jpg")
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

	if (/*m_RendererWorker->ResizeFramebuffer(RenderTexture->SizeX, RenderTexture->SizeY) ||*/
		m_TriggerEndFrameRebuild)
	{
		// Restart render, skip frame copy
		m_RendererWorker->RestartRender();
		m_TriggerEndFrameRebuild = false;
	}
	else if (m_RendererWorker->Flush())
	{
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

		FRPRPluginModule	&plugin = FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin");
		plugin.m_Viewport->Draw();
	}
}

void	ARPRScene::RemoveSceneContent()
{
	UWorld	*world = GetWorld();
	if (world != NULL)
	{
		// TODO make sure objects were correctly deleted
		const uint32	objectCount = SceneContent.Num();
		for (uint32 iObject = 0; iObject < objectCount; ++iObject)
		{
			if (SceneContent[iObject] == NULL)
				continue;
			world->DestroyActor(SceneContent[iObject]);
		}
	}
	SceneContent.Empty();
	Cameras.Empty();
}

void	ARPRScene::BeginDestroy()
{
	Super::BeginDestroy();

	RemoveSceneContent();
	if (m_RendererWorker.IsValid())
	{
		m_RendererWorker->EnsureCompletion();
		m_RendererWorker = NULL; // TODO MAKE SURE TSharedPtr correctly deletes the renderer
	}
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
