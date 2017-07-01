// RPR COPYRIGHT

#include "RPRScene.h"

#include "Scene/RPRLightComponent.h"
#include "Scene/RPRStaticMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRScene, Log, All);

ARPRScene::ARPRScene()
:	m_RprContext(NULL)
,	m_RprScene(NULL)
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
		// Cameras
		// Lights
		// Post processes
		// ..
	}
}

// BeginPlay will be called after the object has been created and initialized
// Might not be the best entry point, probably best to expose a StartRender() function or something
void	ARPRScene::BeginPlay()
{
	Super::BeginPlay();

	FString	cachePath = FPaths::GameSavedDir() + "/RadeonProRender/Cache/"; // To get from settings ?
	FString	dllPath = FPaths::GameDir() + "/Binaries/Win64/Tahoe64.dll"; // To get from settings ?
	uint32	creationFlags = RPR_CREATION_FLAGS_ENABLE_CPU; // for now

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

	BuildScene();
}

void	ARPRScene::BeginDestroy()
{
	Super::BeginDestroy();
	const uint32	objectCount = SceneContent.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		if (SceneContent[iObject] == NULL)
			continue;
		rprObjectDelete(SceneContent[iObject]);
	}
	SceneContent.Empty();
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
