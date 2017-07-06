// RPR COPYRIGHT

#include "RPRScene.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"
#include "Renderer/RPRRendererWorker.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "TextureResource.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

ARPRScene::ARPRScene()
:	m_RprContext(NULL)
,	m_RprScene(NULL)
,	m_RprFrameBuffer(NULL)
,	m_CurrentCamera(NULL)
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

void	ARPRScene::SetCurrentCamera(class ACameraActor *camera)
{
	if (camera == NULL)
		return;
	// Get the default player controller
	APlayerController	*pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (pc == NULL)
		return;
	// No need for smooth transition
	pc->SetViewTarget(camera, FViewTargetTransitionParams());
	m_CurrentCamera = camera->GetCameraComponent();
}

// BeginPlay will be called after the object has been created and initialized
// Might not be the best entry point, probably best to expose a StartRender() function or something
void	ARPRScene::BeginPlay()
{
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

	if (m_CurrentCamera == NULL)
	{
		UE_LOG(LogRPRScene, Warning, TEXT("Couldn't render, no camera setup"));
		GetWorld()->DestroyActor(this);
		return;
	}
	APlayerController	*pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (pc == NULL)
	{
		UE_LOG(LogRPRScene, Error, TEXT("No PlayerController found !"));
		GetWorld()->DestroyActor(this);
		return;
	}
	int32	width = 0;
	int32	height = 0;
	pc->GetViewportSize(width, height);

	SceneTexture = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	if (SceneTexture == NULL)
	{
		UE_LOG(LogRPRScene, Error, TEXT("Couldn't create target image texture"));
		GetWorld()->DestroyActor(this);
		return;
	}
	SceneTexture->UpdateResource();

	m_RendererWorker = new FRPRRendererWorker(m_RprContext, width, height);
	Super::BeginPlay();
}

void	ARPRScene::Tick(float deltaTime)
{
	check(SceneTexture != NULL);
	check(m_RendererWorker != NULL);

	if (m_RendererWorker->Flush())
	{
		FTexture2DMipMap	&mip = SceneTexture->PlatformData->Mips[0];
		void				*textureData = mip.BulkData.Lock(LOCK_READ_WRITE);

		const bool	updateTexture = m_RendererWorker->LockCopyFramebufferInto(textureData);

		mip.BulkData.Unlock();
		if (updateTexture)
		{
			ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
				UpdateDynamicTextureCode,
				UTexture2D*, SceneTexture, SceneTexture,
				const uint8*, textureData, (const uint8*)textureData,
				{
					FUpdateTextureRegion2D	region;
					region.SrcX = 0;
					region.SrcY = 0;
					region.DestX = 0;
					region.DestY = 0;
					region.Width = SceneTexture->GetSizeX();
					region.Height = SceneTexture->GetSizeY();

					FTexture2DResource	*resource = (FTexture2DResource*)SceneTexture->Resource;
					RHIUpdateTexture2D(resource->GetTexture2DRHI(), 0, region, region.Width * sizeof(uint8) * 4, textureData);
				});
		}
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
	//if (m_RprFrameBuffer != NULL)
	//{
	//	rprObjectDelete(m_RprFrameBuffer);
	//	m_RprFrameBuffer = NULL;
	//}
	if (m_RprContext != NULL)
	{
		rprObjectDelete(m_RprContext);
		m_RprContext = NULL;
	}
}
