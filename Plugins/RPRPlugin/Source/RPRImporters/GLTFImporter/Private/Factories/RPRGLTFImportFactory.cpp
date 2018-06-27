//~ RPR copyright

#include "Factories/RPRGLTFImportFactory.h"

#include "Editor.h"
#include "Engine/StaticMesh.h"
#include "RawMesh.h"
#include "StaticMeshResources.h"
#include "RenderingThread.h"
#include "AssetSelection.h"
#include "ActorFactories/ActorFactoryEmptyActor.h"
#include "Materials/Material.h"

#include "AssetRegistryModule.h"
#include "BlueprintEditorUtils.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"

#include <locale>
#include <codecvt>

#include "Factories/GLTFMaterialTranslationFactory.h"
#include "Factories/GLTFMaterialTranslationFactoryRPR.h"

#include "Widgets/SGLTFImportWindow.h"

#include "GLTF.h"

#include "GLTFImporterModule.h"

#define LOCTEXT_NAMESPACE "URPRGLTFImportFactory"

URPRGLTFImportFactory::URPRGLTFImportFactory(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
    , GLTF(nullptr)
    , CurrentSceneIndex(-1)
{
    SupportedClass = UStaticMesh::StaticClass();
    if (Formats.Num() > 0) Formats.Empty();
    Formats.Add(TEXT("gltf;Radeon ProRender glTF Scene"));

    bCreateNew = false;
    bText = true;
    bEditorImport = true;
}

FText URPRGLTFImportFactory::GetDisplayName() const
{
    return LOCTEXT("RPRGLTFImportFactory_Name", "Radeon ProRender glTF Importer");
}

bool URPRGLTFImportFactory::DoesSupportClass(UClass* InClass)
{
    return InClass == UStaticMesh::StaticClass();
}

bool URPRGLTFImportFactory::FactoryCanImport(const FString& InFilename)
{
    return FPaths::GetExtension(InFilename).Equals(TEXT("gltf"), ESearchCase::IgnoreCase);
}

UObject* URPRGLTFImportFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
    FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, InParms);

    // Parse the glTF json and load buffers
    TSharedPtr<GLTF::FData> GLTFData = nullptr;
    if (!LoadGLTFData(InFilename, GLTFData))
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("URPRGLTFImportFactory::FactoryCreateFile: Failed to load glTF file '%s'."), *InFilename);
        return nullptr;
    }

    // Create a new GLTFSettings object to keep track of options chosen in the ImportWindow
    UGLTFSettings* Settings = GetMutableDefault<UGLTFSettings>();
    // Note the export tool this file was created with
    Settings->FileGenerator = GLTFData->asset.generator.c_str();
    // Open the import options window
    if (!SGLTFImportWindow::Open(InFilename, InParent->GetPathName(), Settings))
    {
        bOutOperationCanceled = true;
        UE_LOG(LogRPRGLTFImporter, Log, TEXT("URPRGLTFImportFactory::FactoryCreateFile: Import of glTF file '%s' cancelled."), *InFilename);
        return nullptr;
    }

    // Load the glTF buffer files
    TSharedPtr<GLTF::FBufferCache> GLTFBuffers = nullptr;
    if (!LoadGLTFBuffers(FPaths::GetPath(InFilename), GLTFData, GLTFBuffers))
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("URPRGLTFImportFactory::FactoryCreateFile: Failed to load buffer files for '%s'."), *InFilename);
        return nullptr;
    }

    // Construct glTF context
    GLTF = FGLTFPtr(new FGLTF(GLTFData.ToSharedRef(), GLTFBuffers.ToSharedRef(), Settings));
    if (!GLTF.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("URPRGLTFImportFactory::FactoryCreateFile: glTF context is not valid."));
        return nullptr;
    }
    // Give ownership of the glTF context to the ImporterModule
    FGLTFImporterModule::Get().SetGLTF(GLTF);

    FlushRenderingCommands();

    // Begin import process
    TArray<UObject*> AllImportedObjects;
    CurrentSceneIndex = Settings->DefaultSceneOnly ? GLTF->Data->scene : 0;
    /* Iterate through all scenes in the glTF file (or only the default scene if selected)
       and create static meshes and materials from glTF nodes */
    do {
        TMap<int, UStaticMesh*> NewStaticMeshes; // Static meshes keyed by associated glTF node index (for blueprint hierarchy translation)
        const GLTF::FScene& GLTFScene = GLTF->Data->scenes.at(CurrentSceneIndex);
        if (ImportNodes(GLTFScene.nodes, NewStaticMeshes) && NewStaticMeshes.Num() > 0)
        {
            if (Settings->UseBlueprint)
            {
                UBlueprint* NewBlueprint = CreateBlueprint(NewStaticMeshes);
                AllImportedObjects.Add(NewBlueprint);
            }
            else
            {
                for (auto It : NewStaticMeshes) AllImportedObjects.Add(It.Value);
            }
        }
    } while (!Settings->DefaultSceneOnly && ++CurrentSceneIndex < GLTF->Data->scenes.size());

    // Destroy the glTF context
    GLTF.Reset();
    FGLTFImporterModule::Get().SetGLTF(nullptr);

    UObject* ImportedObject = nullptr;
    if (AllImportedObjects.Num() > 0)
    {
        ImportedObject = *AllImportedObjects.CreateIterator();
        FEditorDelegates::OnAssetPostImport.Broadcast(this, ImportedObject);
    }
    return ImportedObject;
}

bool URPRGLTFImportFactory::LoadGLTFData(const FString& InFilename, TSharedPtr<GLTF::FData>& OutGLTFData)
{
    OutGLTFData.Reset();
    FString Data;
    if (!FFileHelper::LoadFileToString(Data, *InFilename))
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("URPRGLTFImportFactory::LoadGLTFData: Failed to load glTF file '%s' to string."), *InFilename);
        return false;
    }

    // nlohmann json header doesn't support wstring, convert from wide strings
    std::wstring FileBufferW = *Data;
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    std::string FileBuffer = converter.to_bytes(FileBufferW);
    // Parse the glTF file through nlohmann json library
    nlohmann::json json;
    json = nlohmann::json::parse(FileBuffer.c_str());

	gltf::glTFAssetData gltfObj = json;
    TSharedPtr<GLTF::FData> GLTFData = MakeShareable(new gltf::glTFAssetData(gltfObj));
    if (GLTFData.IsValid())
    {
        // Parse the extensions that are used by this file
        const TArray<FString> SupportedExtensions = { "AMD_RPR_material" };
        bool UsesUnsupportedExtensions = false;
        for (auto ext : GLTFData->extensionsUsed)
        {
            FString Extension(ext.c_str());
            if (SupportedExtensions.Contains(Extension))
            {
                UE_LOG(LogRPRGLTFImporter, Log, TEXT("URPRGLTFImportFactory::LoadGLTFData: glTF file '%s' uses supported extension '%s'."), *InFilename, *Extension);
            }
            else
            {
                UsesUnsupportedExtensions = true;
                UE_LOG(LogRPRGLTFImporter, Warning, TEXT("URPRGLTFImportFactory::LoadGLTFData: glTF file '%s' uses unsupported extension '%s'."), *InFilename, *Extension);
            }
        }
        if (UsesUnsupportedExtensions)
        {
            UE_LOG(LogRPRGLTFImporter, Warning, TEXT("URPRGLTFImportFactory::LoadGLTFData: glTF file '%s' uses extensions that the importer does not support, see Output Log for details."), *InFilename);
        }
        // Set outbound glTF asset
        OutGLTFData = GLTFData;
    }
    return OutGLTFData.IsValid();
}

bool URPRGLTFImportFactory::LoadGLTFBuffers(const FString& InDirectory, TSharedPtr<GLTF::FData> InGLTFData, TSharedPtr<GLTF::FBufferCache>& OutGLTFBuffers)
{
    OutGLTFBuffers = MakeShareable(new GLTF::FBufferCache(InDirectory, InGLTFData->buffers));
    return OutGLTFBuffers.IsValid();
}

UStaticMesh* URPRGLTFImportFactory::CreateStaticMesh(int InMeshIndex)
{
    const GLTF::FMesh& GLTFMesh = GLTF->Data->meshes.at(InMeshIndex);
    FRawMesh NewRawMesh;
    TArray<UMaterialInterface*> MaterialsToAdd;
    // Traverse mesh primitives and gather indices, position, normal, etc from gltf buffers
    for (const GLTF::FMeshPrimitive& GLTFMeshPrim : GLTFMesh.primitives)
    {
        TArray<uint32> TriangleIndices;
        TArray<FVector> Points;
        TArray<FVector> Normals;
        TArray<FVector2D> TextureCoords;

        /** Extract triangle indices and attributes */
        ExtractTriangleIndices(GLTFMeshPrim, TriangleIndices);
        ExtractAttributeData(GLTFMeshPrim, GLTF::AttributeNames::POSITION, Points);
        ExtractAttributeData(GLTFMeshPrim, GLTF::AttributeNames::NORMAL, Normals);
        ExtractAttributeData(GLTFMeshPrim, GLTF::AttributeNames::TEXCOORD_0, TextureCoords);

        /** Get materials */
        int MaterialIndex = GLTFMeshPrim.material;
        if (MaterialIndex >= 0 && MaterialIndex < GLTF->Data->materials.size())
        {
            UMaterialInterface* Material = CreateMaterial(GLTFMeshPrim.material);
            if (Material)
            {
                MaterialsToAdd.Add(Material);
            }
        }

        if (Points.Num() <= 0)
        {
            UE_LOG(LogRPRGLTFImporter, Warning, TEXT("URPRGLTFImportFactory::CreateStaticMesh: glTF Mesh Primitive has no vertices."));
            break;
        }

        NewRawMesh.WedgeIndices.Append(TriangleIndices);
        NewRawMesh.VertexPositions.Append(Points);
        NewRawMesh.WedgeTangentZ.Append(Normals);
        NewRawMesh.WedgeTexCoords[0].Append(TextureCoords);
    }

    int32 WedgeIndicesCount = NewRawMesh.WedgeIndices.Num();
    if (WedgeIndicesCount > 0 && (WedgeIndicesCount % 3 == 0))
    {
        int32 TriangleCount = NewRawMesh.WedgeIndices.Num() / 3;

        NewRawMesh.FaceMaterialIndices.Empty(TriangleCount);
        NewRawMesh.FaceMaterialIndices.AddZeroed(TriangleCount);
        NewRawMesh.FaceSmoothingMasks.Empty(TriangleCount);
        NewRawMesh.FaceSmoothingMasks.AddZeroed(TriangleCount);

        if (NewRawMesh.WedgeTexCoords[0].Num() != WedgeIndicesCount)
        {
            if (NewRawMesh.WedgeTexCoords[0].Num() > 0
                && NewRawMesh.WedgeTexCoords[0].Num() == NewRawMesh.VertexPositions.Num())
            {
                TArray<FVector2D> WedgeTexCoords = NewRawMesh.WedgeTexCoords[0];
                NewRawMesh.WedgeTexCoords[0].Empty();
                NewRawMesh.WedgeTexCoords[0].SetNumUninitialized(WedgeIndicesCount);
                for (int32 i = 0; i < NewRawMesh.WedgeIndices.Num(); ++i)
                {
                    NewRawMesh.WedgeTexCoords[0][i] = WedgeTexCoords[NewRawMesh.WedgeIndices[i] % WedgeTexCoords.Num()];
                }
            }
            else
            {
                NewRawMesh.WedgeTexCoords[0].Init(FVector2D::ZeroVector, WedgeIndicesCount);
            }
        }
    }

    // Scale vertex positions by scale factor
    for (FVector& Position : NewRawMesh.VertexPositions)
    {
        Position = Position * GLTF->Settings->ScaleFactor * -1.0f;
    }

    // Create asset package for static mesh
    FString PackageName = MakeUniquePackageName(GLTFMesh.name);
    UPackage* Package = LoadPackage(nullptr, *PackageName, 0);
    if (!Package)
    {
        Package = CreatePackage(nullptr, *PackageName);
        Package->FullyLoad();
        Package->Modify();
    }

    FName StaticMeshName = FPackageName::GetShortFName(*PackageName);
    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, StaticMeshName, RF_Public | RF_Standalone);
    StaticMesh->LightingGuid = FGuid::NewGuid();
    StaticMesh->LightMapResolution = 64;
    StaticMesh->LightMapCoordinateIndex = 0;

    StaticMesh->SourceModels.Empty();
    StaticMesh->SectionInfoMap.Clear();
    StaticMesh->StaticMaterials.Empty();

    // Get imported materials and assign them to the static mesh
    if (MaterialsToAdd.Num() > 0)
    {
        for (auto Mat : MaterialsToAdd) StaticMesh->StaticMaterials.Add(Mat);
    }
    else
    {
        StaticMesh->StaticMaterials.Add(UMaterial::GetDefaultMaterial(MD_Surface));
    }

    int32 NumLODs = 1;
    // Create source models
    for (int32 LODIndex = 0; LODIndex < NumLODs; ++LODIndex)
    {
        if (StaticMesh->SourceModels.Num() < LODIndex + 1)
        {
            new(StaticMesh->SourceModels) FStaticMeshSourceModel();
        }
        FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
        SrcModel.RawMeshBulkData->SaveRawMesh(NewRawMesh);
        if (!NewRawMesh.IsValid())
        {
            UE_LOG(LogRPRGLTFImporter, Warning, TEXT("URPRGLTFImportFactory::CreateStaticMesh: Invalid FRawMesh for glTF mesh '%s'"), *StaticMeshName.ToString());
        }
        NewRawMesh.CompactMaterialIndices();

        for (int32 SlotIndex = 0; SlotIndex < NewRawMesh.MaterialIndexToImportIndex.Num(); ++SlotIndex)
        {
            FMeshSectionInfo Info = StaticMesh->SectionInfoMap.Get(LODIndex, SlotIndex);

            Info.MaterialIndex = NewRawMesh.MaterialIndexToImportIndex[SlotIndex];

            StaticMesh->SectionInfoMap.Remove(LODIndex, SlotIndex);
            StaticMesh->SectionInfoMap.Set(LODIndex, SlotIndex, Info);
        }
    }
    // Build the static mesh
    if (NewRawMesh.IsValidOrFixable())
    {
        TArray<FText> BuildErrors;
        StaticMesh->Build(false, &BuildErrors);
        for (auto err : BuildErrors)
        {
            UE_LOG(LogRPRGLTFImporter, Error, TEXT("URPRGLTFImportFactory::CreateStaticMesh: BuildError: %s"), *err.ToString());
        }
        // After build, clean up section info map
        FMeshSectionInfoMap OldSectionInfoMap = StaticMesh->SectionInfoMap;
        StaticMesh->SectionInfoMap.Clear();
        for (int32 LODResoureceIndex = 0; LODResoureceIndex < StaticMesh->RenderData->LODResources.Num(); ++LODResoureceIndex)
        {
            FStaticMeshLODResources& LOD = StaticMesh->RenderData->LODResources[LODResoureceIndex];
            int32 NumSections = LOD.Sections.Num();
            for (int32 SectionIndex = 0; SectionIndex < NumSections; ++SectionIndex)
            {
                FMeshSectionInfo Info = OldSectionInfoMap.Get(LODResoureceIndex, SectionIndex);
                if (StaticMesh->StaticMaterials.IsValidIndex(Info.MaterialIndex))
                {
                    StaticMesh->SectionInfoMap.Set(LODResoureceIndex, SectionIndex, Info);
                }
            }
        }
        FAssetRegistryModule::AssetCreated(StaticMesh);
        Package->MarkPackageDirty();
        StaticMesh->PostEditChange();
    }
    else
    {
        StaticMesh = nullptr;
    }
    return StaticMesh;
}

UMaterialInterface* URPRGLTFImportFactory::CreateMaterial(int InMaterialIndex)
{
    GLTF::FMaterial& GLTFMaterial = GLTF->Data->materials.at(InMaterialIndex);
    UMaterial* Material = nullptr;
    if (Material == nullptr)
    {
        // Check for the AMD_RPR_material extension
        GLTF::FRPRMaterial GLTFRPRMaterial;
        bool bUsesRPRMaterial = amd::ImportExtension(GLTFMaterial.extensions, GLTFRPRMaterial);

        // Create package name (differs based on glTF or RPR material structure)
        FString PackageName;
        if (bUsesRPRMaterial)
        {
            GLTF::FRPRNode RootNode = GLTFRPRMaterial.nodes[0];
            PackageName = MakeUniquePackageName(RootNode.name); // Use Uber root node as name instead
        }
        else
        {
            PackageName = MakeUniquePackageName(GLTFMaterial.name);
        }

        UPackage* Package = LoadPackage(nullptr, *PackageName, 0);
        if (!Package)
        {
            Package = CreatePackage(nullptr, *PackageName);
            Package->FullyLoad();
            Package->Modify();
        }
        FName MaterialName = FPackageName::GetShortFName(*PackageName);

        UGLTFMaterialTranslationFactoryInterface* GLTFMaterialFactory = nullptr;
        if (bUsesRPRMaterial)
        {
            GLTFMaterialFactory = NewObject<UGLTFMaterialTranslationFactoryRPR>();
        }
        else
        {
            GLTFMaterialFactory = NewObject<UGLTFMaterialTranslationFactory>();
        }
        if (GLTFMaterialFactory->InitFromGLTF(GLTFMaterial))
        {
            Material = StaticCast<UMaterial*>(GLTFMaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), Package, MaterialName, RF_Public | RF_Standalone, nullptr, GWarn));
        }
    }
    return Material;
}

UBlueprint* URPRGLTFImportFactory::CreateBlueprint(const TMap<int, UStaticMesh*>& InStaticMeshes)
{
    UBlueprint* Blueprint = nullptr;
    AActor* RootActorContainer = nullptr;
    USceneComponent* ActorRootComponent = nullptr;
    EComponentMobility::Type MobilityType = EComponentMobility::Static;

    FScopedSlowTask SlowTask(InStaticMeshes.Num(), LOCTEXT("CreateBlueprint", "Creating Blueprint..."));
    SlowTask.MakeDialog();

    // Create the root component that everything will be imported under
    UActorFactory* ActorFactory = GEditor->FindActorFactoryByClass(UActorFactoryEmptyActor::StaticClass());
    FAssetData EmptyActorAssetData = FAssetData(ActorFactory->GetDefaultActorClass(FAssetData()));
    UObject* EmptyActorAsset = EmptyActorAssetData.GetAsset();
    // Place empty actor
    RootActorContainer = FActorFactoryAssetProxy::AddActorForAsset(EmptyActorAsset, false);
    check(RootActorContainer != nullptr);
    ActorRootComponent = NewObject<USceneComponent>(RootActorContainer, USceneComponent::GetDefaultSceneRootVariableName());
    check(ActorRootComponent != nullptr);
    ActorRootComponent->Mobility = MobilityType;
    ActorRootComponent->bVisualizeComponent = true;
    RootActorContainer->SetRootComponent(ActorRootComponent);
    RootActorContainer->AddInstanceComponent(ActorRootComponent);
    ActorRootComponent->RegisterComponent();
    RootActorContainer->SetActorLabel(FPaths::GetBaseFilename(GLTF->Settings->FilePathInEngine));
    RootActorContainer->SetFlags(RF_Transactional);
    ActorRootComponent->SetFlags(RF_Transactional);

    // Recursively recreates glTF tree
    std::function<void(USceneComponent*, const std::vector<int>&)> CreateBlueprintHierarchy = [&](USceneComponent* InParent, const std::vector<int>& InNodeIndices) -> void
    {
        /* Iterate node indices of current scene, given that we have a static mesh map keyed by node index
           Create static mesh components and create same hierarchy as glTF nodes */
        for (int NodeIndex : InNodeIndices)
        {
            const GLTF::FNode& GLTFNode = GLTF->Data->nodes.at(NodeIndex);
            auto StaticMeshPtr = InStaticMeshes.Find(NodeIndex);
            if (!StaticMeshPtr) // Node has no associated mesh, but may have children nodes that do. Parent those meshes to the parent of this node instead.
            {
                CreateBlueprintHierarchy(InParent, GLTFNode.children);
                continue;
            }

            UStaticMesh* StaticMesh = *StaticMeshPtr;
            FString MeshName = StaticMesh->GetName();
            SlowTask.EnterProgressFrame(1.0f, FText::Format(LOCTEXT("CreateBlueprint_StaticMesh", "Placing: ({0}/{1}): {2}"), FText::AsNumber(NodeIndex), FText::AsNumber(InStaticMeshes.Num()), FText::FromString(MeshName)));

            UStaticMeshComponent* NewStaticMeshComponent = NewObject<UStaticMeshComponent>(RootActorContainer, NAME_None);
            NewStaticMeshComponent->SetStaticMesh(StaticMesh);
            NewStaticMeshComponent->DepthPriorityGroup = SDPG_World;
            NewStaticMeshComponent->Mobility = MobilityType;

            NewStaticMeshComponent->SetFlags(RF_Transactional);
            // Ensure the scene component does not share a name with another scene component under the root node
            FString NewUniqueName = MeshName;
            if (!NewStaticMeshComponent->Rename(*NewUniqueName, nullptr, REN_Test))
            {
                NewUniqueName = MakeUniqueObjectName(RootActorContainer, UStaticMeshComponent::StaticClass(), FName(*MeshName)).ToString();
            }
            NewStaticMeshComponent->Rename(*NewUniqueName, nullptr, REN_DontCreateRedirectors);

            // Add the component to the owner actor and register it
            RootActorContainer->AddInstanceComponent(NewStaticMeshComponent);
            NewStaticMeshComponent->RegisterComponent();
            // Attach to the root component
            NewStaticMeshComponent->AttachToComponent(InParent, FAttachmentTransformRules::KeepWorldTransform);
            // Notify that the component has been created
            NewStaticMeshComponent->PostEditChange();

            CreateBlueprintHierarchy(NewStaticMeshComponent, GLTFNode.children);
        }
    };

    const GLTF::FScene& GLTFScene = GLTF->Data->scenes.at(CurrentSceneIndex);
    CreateBlueprintHierarchy(ActorRootComponent, GLTFScene.nodes);

    FString PackageName = MakeUniqueObjectName(nullptr, UPackage::StaticClass(), *GLTF->Settings->FilePathInEngine).ToString();
    // Create the blueprint from the BlueprintHierarchy actor, replacing the actor in-place with the blueprint
    Blueprint = FKismetEditorUtilities::CreateBlueprintFromActor(PackageName, RootActorContainer, true, true);

    if (Blueprint)
    {
        FAssetRegistryModule::AssetCreated(Blueprint);
        Blueprint->MarkPackageDirty();
    }

    return Blueprint;
}

bool URPRGLTFImportFactory::ImportNodes(const std::vector<int>& InNodeIndices, TMap<int, UStaticMesh*>& OutStaticMeshes)
{
    for (int NodeIndex : InNodeIndices)
    {
        if (ImportNode(NodeIndex, OutStaticMeshes)) continue;
        return false;
    }
    return true;
}

bool URPRGLTFImportFactory::ImportNode(int InNodeIndex, TMap<int, UStaticMesh*>& OutStaticMeshes)
{
    const GLTF::FNode& GLTFNode = GLTF->Data->nodes.at(InNodeIndex);
    int MeshIndex = GLTFNode.mesh;
    if (MeshIndex >= 0 && MeshIndex < GLTF->Data->meshes.size())
    {
        UStaticMesh* NewStaticMesh = CreateStaticMesh(MeshIndex);
        if (NewStaticMesh)
        {
            OutStaticMeshes.Add(InNodeIndex, NewStaticMesh);
        }
    }
    return ImportNodes(GLTFNode.children, OutStaticMeshes);
}

bool URPRGLTFImportFactory::ExtractTriangleIndices(const GLTF::FMeshPrimitive& InPrim, TArray<uint32>& OutTriangleIndices)
{
    const GLTF::FAccessor& GLTFAccessor = GLTF->Data->accessors.at(InPrim.indices);
    const GLTF::FBufferView& GLTFBufferView = GLTF->Data->bufferViews.at(GLTFAccessor.bufferView);

    /** Preprocessor macro that declares a temp array of the required size based on the component type,
        gets the cached buffer data and puts it into that array, then converts that array to 32-bit uints */
#define GetTriangleIndicesFromBuffer(ComponentType, Size) \
    case ComponentType: { \
        TArray<Size> WedgeIndicesTemp; \
        GLTF->Buffers->Get(GLTFBufferView.buffer, GLTFBufferView.byteOffset + GLTFAccessor.byteOffset, MemLen, WedgeIndicesTemp); \
        OutTriangleIndices = TArray<uint32>(WedgeIndicesTemp); \
    break; }

    int32 MemLen = GLTFAccessor.count * GetAccessorTypeSize(GLTFAccessor);
    if (MemLen <= GLTFBufferView.byteLength) {
        switch (GLTFAccessor.componentType)
        {
            GetTriangleIndicesFromBuffer(GLTF::FAccessor::ComponentType::BYTE, int8);
            GetTriangleIndicesFromBuffer(GLTF::FAccessor::ComponentType::UNSIGNED_BYTE, uint8);
            GetTriangleIndicesFromBuffer(GLTF::FAccessor::ComponentType::SHORT, int16);
            GetTriangleIndicesFromBuffer(GLTF::FAccessor::ComponentType::UNSIGNED_SHORT, uint16);
            GetTriangleIndicesFromBuffer(GLTF::FAccessor::ComponentType::UNSIGNED_INT, uint32);
        }
    }
    return OutTriangleIndices.Num() > 0;
}

int32 URPRGLTFImportFactory::GetAccessorTypeSize(const GLTF::FAccessor& InAccessor)
{
    int32 size = 0;
    switch (InAccessor.componentType)
    {
    case GLTF::FAccessor::ComponentType::BYTE:
    case GLTF::FAccessor::ComponentType::UNSIGNED_BYTE:
        size = sizeof(uint8);
        break;
    case GLTF::FAccessor::ComponentType::SHORT:
    case GLTF::FAccessor::ComponentType::UNSIGNED_SHORT:
        size = sizeof(uint16);
        break;
    case GLTF::FAccessor::ComponentType::UNSIGNED_INT:
    case GLTF::FAccessor::ComponentType::FLOAT:
        size = sizeof(uint32);
        break;
    }
    switch (InAccessor.type)
    {
    case GLTF::FAccessor::Type::SCALAR:
        break;
    case GLTF::FAccessor::Type::VEC2:
        size *= 2;
        break;
    case GLTF::FAccessor::Type::VEC3:
        size *= 3;
        break;
    case GLTF::FAccessor::Type::VEC4:
        size *= 4;
        break;
    case GLTF::FAccessor::Type::MAT2:
        size *= 4;
        break;
    case GLTF::FAccessor::Type::MAT3:
        size *= 9;
        break;
    case GLTF::FAccessor::Type::MAT4:
        size *= 16;
        break;
    }
    return size;
}

FString URPRGLTFImportFactory::MakeUniquePackageName(const std::string& InGLTFName)
{
    FString PackagePath = FPackageName::GetLongPackagePath(GLTF->Settings->FilePathInEngine);
    FString AssetName = InGLTFName.c_str();
    if (AssetName.IsEmpty())
    {
        // If the glTF object name is blank, just use the glTF filename with a number
        static int instance = 0;
        AssetName = FPaths::GetBaseFilename(GLTF->Settings->FilePathInEngine);
        if (instance > 0)
            AssetName += FString::FromInt(instance);
        instance++;
    }
    FString PackageName = MakeUniqueObjectName(
        nullptr, UPackage::StaticClass(),
        *(PackagePath / AssetName)).ToString();
    return PackageName;
}

#undef LOCTEXT_NAMESPACE
