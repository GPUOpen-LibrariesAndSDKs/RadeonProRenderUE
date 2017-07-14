// RPR COPYRIGHT

#pragma once

#include "rprigenericapi.h"
#include "RadeonProRenderInterchange.h"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>

struct FColor;
struct FLinearColor;
class UMaterialExpression;
class UMaterial;

typedef std::shared_ptr<rpri::generic::IMaterialNode> IMaterialNodePtr;
typedef std::shared_ptr<rpri::generic::IMaterialValue> IMaterialValuePtr;
typedef std::shared_ptr<rpri::generic::IMaterialNodeMux> IMaterialNodeMuxPtr;

struct UEInterchangeCollection
{
	std::map<std::string, IMaterialNodePtr> nodeStorage;
	std::map<std::string, IMaterialValuePtr> valueStorage;

	// mux storage / cache
	std::map<std::string, std::shared_ptr<rpri::generic::IMaterialNodeMux>> muxStorage;

	std::shared_ptr<rpri::generic::IMaterialNodeMux> FindMux(char const * _name);
};


class UE4InterchangeMaterialNode :public rpri::generic::IMaterialNode
{
public:
	~UE4InterchangeMaterialNode() {};

	char const * GetId() const override;
	char const * GetName() const override;
	char const * GetType() const override;
	size_t GetNumberOfInputs() const override;
	rpri::generic::IMaterialNodeMux const * GetInputAt(size_t _index) const override;
	char const * GetInputNameAt(size_t _index) const override;

	// some nodes take a texture input (the *Map set + TextureTexelSize)
	// this allow access to it
	bool HasTextureInput() const override;
	rpri::generic::ITexture const * GetTextureInput() const override;
	char const * GetMetadata() const override;

	static IMaterialNodeMuxPtr New(	UEInterchangeCollection & _collection,
									std::string const & _id,
									UMaterialExpression * _expression);
private:
	friend IMaterialNodeMuxPtr ConvertUMaterialExpression(
									UEInterchangeCollection & _collection,
									std::string const & _id,
									UMaterialExpression * _expression);

	UE4InterchangeMaterialNode(UEInterchangeCollection & _collection,
								std::string const & _id,
								UMaterialExpression * _expression);
	UMaterialExpression * expression;

	std::vector<rpri::generic::IMaterialNode const *> nodes;
	std::vector<rpri::generic::IMaterialValue const *> values;
	std::vector<IMaterialNodeMuxPtr> muxes;
	std::vector<std::string> inputNames;

	std::string id;
	std::string name;
	std::string type;
};

class UE4InterchangeMaterialValue : public rpri::generic::IMaterialValue
{
public:

	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					float _a);
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					float _a, float _b);
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					float _a, float _b, float _c);
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					float _a, float _b, float _c, float _d);
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					FLinearColor _col);
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
					FColor _col);

	virtual ~UE4InterchangeMaterialValue() {};

	char const* GetId() const override;

	// just floats for now
	ValueType GetType() const override;
	size_t GetNumberOfValues() const override;
	uint8_t GetValueAtIndexAsUint8(size_t _index) const override;
	uint16_t GetValueAtIndexAsUint16(size_t _index) const override;
	uint32_t GetValueAtIndexAsUint32(size_t _index) const override;
	uint64_t GetValueAtIndexAsUint64(size_t _index) const override;
	int8_t GetValueAtIndexAsInt8(size_t _index) const override;
	int16_t GetValueAtIndexAsInt16(size_t _index) const override;
	int32_t GetValueAtIndexAsInt32(size_t _index) const override;
	int64_t GetValueAtIndexAsInt64(size_t _index) const override;
	float GetValueAtIndexAsFloat(size_t _index) const override;
	double GetValueAtIndexAsDouble(size_t _index) const override;
	char const* GetMetadata() const override;
	std::string GetValueAsString() const override;

private:
	UE4InterchangeMaterialValue(char const * _id, float _a);
	UE4InterchangeMaterialValue(char const * _id, float _a, float _b);
	UE4InterchangeMaterialValue(char const * _id, float _a, float _b, float _c);
	UE4InterchangeMaterialValue(char const * _id, float _a, float _b, float _c, float _d);
	UE4InterchangeMaterialValue(char const * _id, FLinearColor _col);
	UE4InterchangeMaterialValue(char const * _id, FColor _col);

	std::string id;
	std::vector<float> values;
};

class UE4InterchangeMaterialNodeMux : public rpri::generic::IMaterialNodeMux
{
public:
	// we hold a raw point, the material owns everybody
	UE4InterchangeMaterialNodeMux(std::shared_ptr<rpri::generic::IMaterialNode> _ptr);
	UE4InterchangeMaterialNodeMux(std::shared_ptr<rpri::generic::IMaterialValue> _ptr);
	virtual ~UE4InterchangeMaterialNodeMux() {};

	char const* GetId() const override;
	bool IsEmpty() const override;
	bool IsNode() const override;
	rpri::generic::IMaterialValue const* GetAsValue() const override;
	rpri::generic::IMaterialNode const* GetAsNode() const override;

private:
	std::shared_ptr<rpri::generic::IMaterialNode> nodeptr;
	std::shared_ptr<rpri::generic::IMaterialValue> valueptr;
};

static char const * PBRNodeFieldNames[10]{
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

#define NUMBER_OF_INPUTS_NODE	10

class UE4InterchangePBRNode : public rpri::generic::IMaterialNode
{
public:

	char const* GetId() const override;
	char const* GetName() const override;
	char const* GetType() const override;
	size_t GetNumberOfInputs() const override;
	rpri::generic::IMaterialNodeMux const* GetInputAt(size_t _index) const override;
	char const* GetInputNameAt(size_t _index) const override;

	bool HasTextureInput() const override;
	rpri::generic::ITexture const* GetTextureInput() const override;
	char const* GetMetadata() const override;


	static IMaterialNodePtr New(	UEInterchangeCollection & _collection,
									std::string const & _id,
									class UE4InterchangeMaterialGraph * _mg);

private:
	UE4InterchangePBRNode(	UEInterchangeCollection & _collection, 
							std::string const & _id,
							class UE4InterchangeMaterialGraph *);
	std::string id;
	std::shared_ptr<rpri::generic::IMaterialNodeMux> muxes[10];
};


class UE4InterchangeMaterialGraph : public rpri::generic::IMaterialGraph
{
public:
	UE4InterchangeMaterialGraph(const UMaterial* _ue4Mat);

	char const* GetId() const override;
	char const* GetName() const override;
	size_t GetNumberOfMaterialValues() const override;
	rpri::generic::IMaterialValue const* GetMaterialValueAt(size_t _index) const override;
	size_t GetNumberOfMaterialNodes() const override;
	rpri::generic::IMaterialNode const* GetMaterialNodeAt(size_t _index) const override;
	char const* GetMetadata() const override;

	UMaterial const* GetUE4Material() const;

private:	
	std::vector<std::shared_ptr<rpri::generic::IMaterialNode>> nodeStorage;
	std::vector<std::shared_ptr<rpri::generic::IMaterialValue>> valueStorage;
	std::vector<std::shared_ptr<rpri::generic::IMaterialNodeMux>> muxStorage;


	friend class UE4InterchangePBRNode;
	const UMaterial* ue4Mat;
};
