// RPR COPYRIGHT

#include "RPRScene.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "RPRPlugin.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/Texture2DDynamic.h"
#include "TextureResource.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

ARPRScene::ARPRScene()
:	m_RprContext(NULL)
,	m_RprScene(NULL)
,	m_ActiveCamera(NULL)
,	m_TriggerEndFrameRebuild(false)
,	m_RendererWorker(NULL)
{
	PrimaryActorTick.bCanEverTick = true;
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
}

// BeginPlay will be called after the object has been created and initialized
// Might not be the best entry point, probably best to expose a StartRender() function or something
void	ARPRScene::BeginPlay()
{
	Super::BeginPlay();

	FRPRPluginModule	&plugin = FModuleManager::GetModuleChecked<FRPRPluginModule>("RPRPlugin");
	if (!plugin.GetRenderTexture().IsValid())
		return;// No RPR viewport created
	RenderTexture = plugin.GetRenderTexture();

	FString	cachePath = FPaths::GameSavedDir() + "/RadeonProRender/Cache/"; // To get from settings ?
	FString	dllPath = FPaths::GameDir() + "/Binaries/Win64/Tahoe64.dll"; // To get from settings ?
	uint32	creationFlags = RPR_CREATION_FLAGS_ENABLE_GPU0; // for now

	rpr_int tahoePluginId = rprRegisterPlugin(TCHAR_TO_ANSI(*dllPath)); // Seems to be mandatory
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
	UE_LOG(LogRPRScene, Log, TEXT("ProRender scene created"));

	////////
	if (BackgroundImage != NULL)
	{
		m_RprBackgroundImage = BuildImage(BackgroundImage, m_RprContext);
		if (m_RprBackgroundImage != NULL)
			rprSceneSetBackgroundImage(m_RprScene, m_RprBackgroundImage);
	}

	BuildScene();

	m_RendererWorker = new FRPRRendererWorker(m_RprContext, RenderTexture->SizeX, RenderTexture->SizeY);
}

void	ARPRScene::Tick(float deltaTime)
{
	// TODO Set tick group correctly so the scene is updated last (avoid rebuilding the framebuffer two times)

	check(m_RendererWorker != NULL);
	if (!RenderTexture.IsValid())
	{
		// Stop rendering, no viewport
		return;
	}
	if (RenderTexture->Resource == NULL)
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
	}
}

void	ARPRScene::BeginDestroy()
{
	Super::BeginDestroy();

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
	if (m_RendererWorker != NULL)
	{
		m_RendererWorker->EnsureCompletion();
		delete m_RendererWorker;
		m_RendererWorker = NULL;
	}
	SceneContent.Empty();
	if (m_RprBackgroundImage != NULL)
	{
		rprObjectDelete(m_RprBackgroundImage);
		m_RprBackgroundImage = NULL;
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
