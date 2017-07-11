// RPR COPYRIGHT

#include "RPRStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "RPRHelpers.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshResources.h"
#include "rprigenericapi.h"
#include <map>
#include <memory>
#include "RadeonProRenderInterchange.h"

// chuck these up here for now. Move to own file asap
namespace 
{

class UE4InterchangeMaterialNode : public rpri::generic::IMaterialNode
{
public:
	UE4InterchangeMaterialNode(UMaterialExpression * _expression) :
		expression(_expression)
	{
		id = TCHAR_TO_ANSI(*expression->GetName());
		name = id;
		type = expression->StaticClass()->GetFName().GetPlainANSIString();
	}
	~UE4InterchangeMaterialNode(){}
	char const * GetId() const override 
	{
		return id.c_str();
	}

	char const * GetName() const override 
	{
		return name.c_str();;
	}

	char const * GetType() const override 
	{
		return type.c_str();
	}

	size_t GetNumberOfInputs() const override
	{
		return 0;
	}

	rpri::generic::IMaterialNodeMux const * GetInputAt(size_t _index) const override
	{
		return nullptr;
	}

	char const * GetInputNameAt(size_t _index) const override
	{
		return nullptr;
	}

	// this is a mistype. will be deleted in next API update
	char const * GetMetaData() const override { return nullptr; }

	// some nodes take a texture input (the *Map set + TextureTexelSize)
	// this allow access to it
	bool HasTextureInput() const override
	{
		return false;
	}

	struct rpri::generic::ITexture const * GetTextureInput() const override
	{
		return nullptr;
	}

	char const * GetMetadata() const override
	{
		return nullptr;
	}
	const UMaterialExpression * expression;

	std::string id;
	std::string name;
	std::string type;
};

class UE4InterchangeMaterialValue : public rpri::generic::IMaterialValue
{
public:
	UE4InterchangeMaterialValue(float _a) { values.push_back(_a); }
	UE4InterchangeMaterialValue(float _a, float _b)
	{
		values.push_back(_a);
		values.push_back(_b);
	}
	UE4InterchangeMaterialValue(float _a, float _b, float _c)
	{
		values.push_back(_a);
		values.push_back(_b);
		values.push_back(_c);
	}
	UE4InterchangeMaterialValue(float _a, float _b, float _c, float _d)
	{
		values.push_back(_a);
		values.push_back(_b);
		values.push_back(_c);
		values.push_back(_d);
	}
	UE4InterchangeMaterialValue(FLinearColor _col)
	{
		values.push_back(_col.R);
		values.push_back(_col.G);
		values.push_back(_col.B);
		values.push_back(_col.A);
	}
	UE4InterchangeMaterialValue(FColor _col)
	{
		FLinearColor lc = _col.ReinterpretAsLinear();
		values.push_back(lc.R);
		values.push_back(lc.G);
		values.push_back(lc.B);
		values.push_back(lc.A);
	}
	virtual ~UE4InterchangeMaterialValue() {};

	char const* GetId() const override
	{
		return id.c_str();	
	}

	// just floats for now
	ValueType GetType() const override { return ValueType::Float; }
	size_t GetNumberOfValues() const override { return values.size(); }
	uint8_t GetValueAtIndexAsUint8(size_t _index) const override { return 0; }
	uint16_t GetValueAtIndexAsUint16(size_t _index) const override { return 0; }
	uint32_t GetValueAtIndexAsUint32(size_t _index) const override { return 0; }
	uint64_t GetValueAtIndexAsUint64(size_t _index) const override { return 0; }
	int8_t GetValueAtIndexAsInt8(size_t _index) const override { return 0; }
	int16_t GetValueAtIndexAsInt16(size_t _index) const override { return 0; }
	int32_t GetValueAtIndexAsInt32(size_t _index) const override { return 0; }
	int64_t GetValueAtIndexAsInt64(size_t _index) const override { return 0; }
	float GetValueAtIndexAsFloat(size_t _index) const override { return values.at(_index); }
	double GetValueAtIndexAsDouble(size_t _index) const override { return values.at(_index); }
	char const* GetMetadata() const override { return ""; }
	std::string GetValueAsString() const override { return ""; };

	std::string id;
	std::vector<float> values;
};

class UE4InterchangeMaterialNodeMux : public rpri::generic::IMaterialNodeMux
{
public:
	// we hold a raw point, the material owns everybody
	UE4InterchangeMaterialNodeMux(std::shared_ptr<rpri::generic::IMaterialNode> _ptr) :
		nodeptr(_ptr), valueptr(nullptr) {}

	UE4InterchangeMaterialNodeMux(std::shared_ptr<rpri::generic::IMaterialValue> _ptr) :
		nodeptr(nullptr), valueptr(_ptr) {}

	virtual ~UE4InterchangeMaterialNodeMux() {};

	char const* GetId() const override
	{
		return nodeptr ? nodeptr->GetId() : valueptr ? valueptr->GetId() : nullptr;
	}

	bool IsEmpty() const override
	{
		return nodeptr == nullptr;
	}

	bool IsNode() const override 
	{
		return true;
	}
	rpri::generic::IMaterialValue const* GetAsValue() const override
	{
		return valueptr.get();
	}
	rpri::generic::IMaterialNode const* GetAsNode() const override 
	{
		return nodeptr.get();
	}
private:
	std::shared_ptr<rpri::generic::IMaterialNode> nodeptr;
	std::shared_ptr<rpri::generic::IMaterialValue> valueptr;
};
static char const * PBRNodeFieldNames[10] {
	"BaseColor",
	"Roughness",
	"Metallic",
	"Specular",
	"Normal",
	"EmissiveColor",
	"Opacity",
	"Refraction",
	"ClearCoat",
	"ClearCoatRoughness"
};

class UE4InterchangePBRNode : public rpri::generic::IMaterialNode
{
public:
	UE4InterchangePBRNode(class UE4InterchangeMaterialGraph *);

	char const* GetId() const override
	{
		return id.c_str();
	}
	char const* GetName() const override
	{
		return id.c_str();
	}
	char const* GetType() const override {
		return "UE4PBRMaterial";
	}
	size_t GetNumberOfInputs() const override { return 10; }
	rpri::generic::IMaterialNodeMux const* GetInputAt(size_t _index) const override
	{
		return muxes[_index].get();
	}
	char const* GetInputNameAt(size_t _index) const override {
		return PBRNodeFieldNames[_index];
	}

	char const* GetMetaData() const override { return nullptr; }
	bool HasTextureInput() const override { return false; }
	rpri::generic::ITexture const* GetTextureInput() const override { return nullptr; }
	char const* GetMetadata() const override { return ""; }

private:
	std::string id;
	std::shared_ptr<rpri::generic::IMaterialNodeMux> muxes[10];
};

class UE4InterchangeMaterialGraph : public rpri::generic::IMaterialGraph
{
public:
	UE4InterchangeMaterialGraph(const UMaterial* _ue4Mat) :
		ue4Mat(_ue4Mat)
	{
		// UE4 Expressions == Interchange nodes/values/mux
		nodeStorage.resize(ue4Mat->Expressions.Num() +  1);
		for (int32 iExpression = 0; iExpression < ue4Mat->Expressions.Num(); ++iExpression)
		{
			UMaterialExpression * expression = ue4Mat->Expressions[iExpression];
			nodeStorage[iExpression] = std::make_shared<UE4InterchangeMaterialNode>(expression);
			interchangeMuxes.emplace_back(std::make_shared<UE4InterchangeMaterialNodeMux>(nodeStorage[iExpression]));
			stringToInterchangeMuxIndex[expression->GetName()] = interchangeMuxes.size() - 1;

		}
		// Interchange treats the destination PBR object as a node
		FString str = GetName();
		str = str + "PBRNode";
		nodeStorage[ue4Mat->Expressions.Num()] = std::make_unique<UE4InterchangePBRNode>(this);
		stringToInterchangeMuxIndex[str] = ue4Mat->Expressions.Num();
	}

	char const* GetId() const override
	{
		// TODO make sure ID is unique!
		return GetName();
	}

	char const* GetName() const override
	{
		return TCHAR_TO_ANSI(*ue4Mat->GetName());
	}

	size_t GetNumberOfMaterialValues() const override
	{
		return valueStorage.size();
	}

	rpri::generic::IMaterialValue const* GetMaterialValueAt(size_t _index) const override
	{
		return valueStorage.at(_index).get();
	}

	size_t GetNumberOfMaterialNodes() const override
	{
		return nodeStorage.size();
	}

	rpri::generic::IMaterialNode const* GetMaterialNodeAt(size_t _index) const override
	{
		return nodeStorage.at(_index).get();
	}

	char const* GetMetadata() const override
	{
		return "";
	}

	const UMaterial* ue4Mat;

	std::vector<std::shared_ptr<rpri::generic::IMaterialNode>> nodeStorage;
	std::vector<std::shared_ptr<rpri::generic::IMaterialValue>> valueStorage;

	std::vector<std::shared_ptr<rpri::generic::IMaterialNodeMux>> interchangeMuxes;

	typedef std::map<FString, uint32_t> stringToIndexMap;
	stringToIndexMap stringToInterchangeMuxIndex;
};

UE4InterchangePBRNode::UE4InterchangePBRNode(UE4InterchangeMaterialGraph * _mg)
{
	const UMaterial* ue4Mat = _mg->ue4Mat;

	FString str = _mg->GetName();
	str + "PBRNode";
	id = TCHAR_TO_ANSI(*str);

	if(ue4Mat->BaseColor.UseConstant)
	{

		auto val = std::make_shared<UE4InterchangeMaterialValue>(ue4Mat->BaseColor.Constant);
		val->id = id + "BaseColor";
		auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
		muxes[0] = mux;

		_mg->valueStorage.emplace_back(val);
		_mg->interchangeMuxes.emplace_back(mux);
	} else
	{
		auto it = _mg->stringToInterchangeMuxIndex.find(ue4Mat->BaseColor.Expression->GetName());
		if(it != _mg->stringToInterchangeMuxIndex.end())
		{
			muxes[0] = _mg->interchangeMuxes[it->second];
		} else
		{
			muxes[0] = nullptr;
		}
	}
}

}

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

TMap<UStaticMesh*, TArray<rpr_shape>>	URPRStaticMeshComponent::Cache;

URPRStaticMeshComponent::URPRStaticMeshComponent()
:	m_RprMaterialSystem(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<rpr_shape>	URPRStaticMeshComponent::GetMeshInstances(UStaticMesh *mesh)
{
	if (!Cache.Contains(mesh))
		return TArray<rpr_shape>();
	TArray<rpr_shape>		instances;
	const TArray<rpr_shape>	&srcShapes = Cache[mesh];

	const uint32	shapeCount = srcShapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	newShape = NULL;
		if (rprContextCreateInstance(Scene->m_RprContext, srcShapes[iShape], &newShape) != RPR_SUCCESS)
		{
			for (int32 jShape = 0; jShape < instances.Num(); ++jShape)
				rprObjectDelete(instances[jShape]);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *SrcComponent->GetName());
			return TArray<rpr_shape>();
		}
		instances.Add(newShape);
	}
	return instances;
}

void	URPRStaticMeshComponent::CleanCache()
{
	// Obviously this is context dependent
	// TODO : Put a safer cache system in place *or* ensure there can only be one context
	Cache.Empty();
}

bool	URPRStaticMeshComponent::BuildMaterials()
{
	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape			shape = m_Shapes[iShape].m_RprShape;
		rpr_material_node	material = NULL;
		if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
		{
			rprObjectDelete(shape);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
			return false;
		}
		if (rprMaterialNodeSetInputF(material, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS ||
			rprShapeSetMaterial(shape, material) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
			rprObjectDelete(shape);
			rprObjectDelete(material);
			return false;
		}
		RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->ComponentToWorld);
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			rprObjectDelete(shape);
			return false;
		}
	}
	return true;
}


bool	URPRStaticMeshComponent::Build()
{
	if (Scene == NULL || SrcComponent == NULL)
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	//	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == NULL)
	//		return false;
	if (Cast<ACameraActor>(SrcComponent->GetOwner()) != NULL ||
		Cast<APawn>(SrcComponent->GetOwner()) != NULL)
		return false;

	// Not sure if material systems should be created on a per mesh level or per section
	if (rprContextCreateMaterialSystem(Scene->m_RprContext, 0, &m_RprMaterialSystem) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material system"));
		return false;
	}

	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent	*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != NULL);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == NULL ||
		staticMesh->RenderData == NULL ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;
	TArray<FStaticMaterial>	const	   &staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	std::map<UMaterial const *, UE4InterchangeMaterialGraph*> materialMap;

	for(int i =0;i < staticMeshComponent->GetNumMaterials();++i)
	{
		const UMaterialInterface	*ueMatIF = staticMeshComponent->GetMaterial(i);
		const UMaterial			 *ue4Mat = ueMatIF->GetMaterial();

		UE4InterchangeMaterialGraph *mg = nullptr;
		if (materialMap.find(ue4Mat) == materialMap.end())
		{
			mg = new UE4InterchangeMaterialGraph(ue4Mat);
			materialMap[ue4Mat] = mg;
		}
		else
		{
			mg = materialMap[ue4Mat];
		}
	}

	std::vector<rpri::generic::IMaterialGraph*> flatGraphs;
	for(auto&& it : materialMap)
	{
		flatGraphs.push_back(it.second);
	}

	std::string importer("Generic");
	std::string exporter("");

	rpriImportProperty properties[] = {
		{"input", reinterpret_cast<uintptr_t>("UE4 Importer")},
		{"Num Materials", 1 },
		{"Materials Input Array", reinterpret_cast<uintptr_t>(flatGraphs.data()) }
	};

	uint32_t numProps = sizeof(properties) / sizeof(properties[0]);
	rpriContext ctx;
	rpriAllocateContext(&ctx);

//#define RPR_UMS_INTEGRATION
#if RPR_UMS_INTEGRATION
	rpriImportFromMemory(ctx, importer.c_str(), numProps, properties);
//	rpriExport(ctx, exporter.c_str(), numProps, props);

#endif
	rpriFreeContext(ctx);


	TArray<rpr_shape>	shapes = GetMeshInstances(staticMesh);
	if (shapes.Num() == 0) // No mesh in cache ?
	{
		FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
		const FStaticMeshVertexBuffer	&srcVertices = lodRes.VertexBuffer;
		const FPositionVertexBuffer		&srcPositions = lodRes.PositionVertexBuffer;
		const uint32					uvCount = srcVertices.GetNumTexCoords();

		// Guess: we need to create several RprObject
		// One for each section
		// To check with ProRender API
		uint32	sectionCount = lodRes.Sections.Num();
		for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
		{
			const FStaticMeshSection	&section = lodRes.Sections[iSection];
			const uint32				srcIndexStart = section.FirstIndex;
			const uint32				indexCount = section.NumTriangles * 3;

			TArray<FVector>		positions;
			TArray<FVector>		normals;
			TArray<FVector2D>	uvs;

			const uint32	vertexCount = (section.MaxVertexIndex - section.MinVertexIndex) + 1;
			if (vertexCount == 0)
				continue;

			// Lots of resizing ..
			positions.SetNum(vertexCount);
			normals.SetNum(vertexCount);
			if (uvCount > 0) // For now force set only one uv set
				uvs.SetNum(vertexCount * 1/*uvCount*/);

			TArray<int32>	indices;
			TArray<int32>	numFaceVertices;

			indices.SetNum(indexCount);
			numFaceVertices.SetNum(section.NumTriangles);

			const uint32	offset = section.MinVertexIndex;
			for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
			{
				const uint32	index = srcIndices[srcIndexStart + iIndex];
				const uint32	remappedIndex = index - offset;

				indices[iIndex] = remappedIndex;

				FVector	pos = srcPositions.VertexPosition(index) * 0.1f;
				FVector	normal = srcVertices.VertexTangentZ(index);

				positions[remappedIndex] = FVector(pos.X, pos.Z, pos.Y);
				normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);
				if (uvCount > 0)
					uvs[remappedIndex] = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
			}

			for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
				numFaceVertices[iTriangle] = 3;

			rpr_shape	shape = NULL;

			if (rprContextCreateMesh(Scene->m_RprContext,
				(rpr_float const *)positions.GetData(), positions.Num(), sizeof(float) * 3,
				(rpr_float const *)normals.GetData(), normals.Num(), sizeof(float) * 3,
				(rpr_float const *)uvs.GetData(), uvs.Num(), sizeof(float) * 2,
				(rpr_int const *)indices.GetData(), sizeof(int32),
				(rpr_int const *)indices.GetData(), sizeof(int32),
				(rpr_int const *)indices.GetData(), sizeof(int32),
					(rpr_int const *)numFaceVertices.GetData(), numFaceVertices.Num(),
				&shape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"), *SrcComponent->GetName(), iSection, indices.Num(), positions.Num());
				return false;
			}



			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s'"), *SrcComponent->GetName());
			if (!Cache.Contains(staticMesh))
				Cache.Add(staticMesh);
			Cache[staticMesh].Add(shape);
			m_Shapes.Add(shape);
		}
	}
	else
	{
		const uint32	shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
			m_Shapes.Add(shapes[iShape]);
	}
	if (!BuildMaterials())
		return false;

	return Super::Build();
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->ComponentToWorld);

	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		if (rprShapeSetTransform(m_Shapes[iShape].m_RprShape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't refresh RPR mesh transforms"));
			return false;
		}
	}
	return true;
}

void	URPRStaticMeshComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprMaterialSystem != NULL)
	{
		rprObjectDelete(m_RprMaterialSystem);
		m_RprMaterialSystem = NULL;
	}
	// TODO: Check if we need to call rprSceneDetachShape or rprObjectDelete does this thing for us
	uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		if (m_Shapes[iShape].m_RprShape != NULL)
			rprObjectDelete(m_Shapes[iShape].m_RprShape);
		if (m_Shapes[iShape].m_RprMaterial != NULL)
			rprObjectDelete(m_Shapes[iShape].m_RprMaterial);
	}
	m_Shapes.Empty();
}
