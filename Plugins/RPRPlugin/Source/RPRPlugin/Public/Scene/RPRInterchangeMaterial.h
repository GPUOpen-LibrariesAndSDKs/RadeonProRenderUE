// RPR COPYRIGHT

#pragma once

#if WITH_EDITOR

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include "rprigenericapi.h"
#include "RadeonProRenderInterchange.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionStaticSwitch.h"
#include "Materials/MaterialExpressionStaticSwitchParameter.h"

class UMaterialExpressionMakeMaterialAttributes;
struct FColor;
struct FLinearColor;
class UMaterialExpression;
class UMaterial;

typedef std::shared_ptr<rpri::generic::IMaterialNode> IMaterialNodePtr;
typedef std::shared_ptr<rpri::generic::IMaterialValue> IMaterialValuePtr;
typedef std::shared_ptr<rpri::generic::IMaterialNodeMux> IMaterialNodeMuxPtr;
typedef std::shared_ptr<rpri::generic::IImage> IImagePtr;
typedef std::shared_ptr<rpri::generic::ITexture> ITexturePtr;
typedef std::shared_ptr<rpri::generic::ISampler> ISamplerPtr;

struct UEInterchangeCollection
{
	UMaterialInterface const * ue4MatInterface;

	std::map<std::string, IMaterialNodePtr> nodeStorage;
	std::map<std::string, IMaterialValuePtr> valueStorage;
	std::map<std::string, IImagePtr> imageStorage;
	std::map<std::string, ITexturePtr> textureStorage;
	std::map<std::string, ISamplerPtr> samplerStorage;

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
									UMaterialExpression * _expression,
									FName const _fname = "");
private:
	friend IMaterialNodeMuxPtr ConvertUMaterialExpression(
									UEInterchangeCollection & _collection,
									std::string const & _id,
									UMaterialExpression * _expression,
									FName const _fname);

	void ConvertFExpressionInput(UEInterchangeCollection& _collection, 
								FExpressionInput* _input,
								char const *_name,
								FName const _fname = "");
	void ConvertTextureSampleExpression(UEInterchangeCollection& _collection, 
										std::string const & _name,
										UMaterialExpressionTextureSample* con);
	void ConvertTextureSampleExpression(UEInterchangeCollection& _collection, 
										std::string const & _name,
										UMaterialExpressionTextureSampleParameter2D* con);
	FExpressionInput const & ConvertStaticSwitchExpression(UEInterchangeCollection& _collection,
										std::string const & _name,
										UMaterialExpressionStaticSwitch* con);
	FExpressionInput const & ConvertStaticSwitchParameterExpression(UEInterchangeCollection& _collection,
										std::string const & _name,
										UMaterialExpressionStaticSwitchParameter* con);
	void ConvertTexture(UEInterchangeCollection& _collection,
										std::string const & _valname,
										std::string const & _texname,
										UTexture * _texture);

	UE4InterchangeMaterialNode(UEInterchangeCollection & _collection,
								std::string const & _id,
								UMaterialExpression * _expression);
	UMaterialExpression * expression;

	std::vector<rpri::generic::IMaterialNode const *> nodes;
	std::vector<rpri::generic::IMaterialValue const *> values;
	std::vector<IMaterialNodeMuxPtr> muxes;
	std::vector<std::string> inputNames;

	rpri::generic::ITexture const * texture;

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
	static IMaterialValuePtr New(UEInterchangeCollection & _collection, char const * _id,
		std::string const & _string);

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
	UE4InterchangeMaterialValue(char const * _id, std::string const & _string);

	std::string id;
	std::string str;
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
	"Base.Color",
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


	static IMaterialNodePtr New(UEInterchangeCollection & _collection,
								std::string const & _id,
								UMaterial const * _ue4Mat);
	static IMaterialNodePtr New(UEInterchangeCollection & _collection,
								std::string const & _id,
								FMaterialAttributesInput const & _ue4MatAttr);
private:
	UE4InterchangePBRNode(	UEInterchangeCollection & _collection, 
							std::string const & _id,
							UMaterial const * _ue4Mat);
	UE4InterchangePBRNode(	UEInterchangeCollection & _collection,
							std::string const & _id,
							FMaterialAttributesInput const & _ue4MatAttr);
	std::string id;
	std::shared_ptr<rpri::generic::IMaterialNodeMux> muxes[10];
};


class UE4InterchangeMaterialGraph : public rpri::generic::IMaterialGraph
{
public:
	UE4InterchangeMaterialGraph(UMaterialInterface const * _ue4MatInterface);

	char const* GetId() const override;
	char const* GetName() const override;
	size_t GetNumberOfMaterialValues() const override;
	rpri::generic::IMaterialValue const* GetMaterialValueAt(size_t _index) const override;
	size_t GetNumberOfMaterialNodes() const override;
	rpri::generic::IMaterialNode const* GetMaterialNodeAt(size_t _index) const override;
	char const* GetMetadata() const override;

	UMaterial const* GetUE4Material() const;
	rpri::generic::IMaterialNode const * GetRootNode() const override;

private:	
	std::vector<std::shared_ptr<rpri::generic::IMaterialNode>> nodeStorage;
	std::vector<std::shared_ptr<rpri::generic::IMaterialValue>> valueStorage;
	std::vector<std::shared_ptr<rpri::generic::IMaterialNodeMux>> muxStorage;
	std::vector<std::shared_ptr<rpri::generic::IImage>> imageStorage;
	std::vector<std::shared_ptr<rpri::generic::ISampler>> samplerStorage;
	std::vector<std::shared_ptr<rpri::generic::ITexture>> textureStorage;

	std::shared_ptr<rpri::generic::IMaterialNode> rootNode;
	mutable std::string name;
	friend class UE4InterchangePBRNode;
	UMaterialInterface const * ue4MatInterface;
	UMaterial const * ue4Mat;
};

class UE4InterchangeImage : public rpri::generic::IImage
{
public:
	UE4InterchangeImage(UTexture* _ueTexture);

	~UE4InterchangeImage() {};

	char const* GetId() const override;

	size_t GetWidth() const override;
	size_t GetHeight() const override;
	size_t GetDepth() const override;
	size_t GetSlices() const override;

	size_t GetNumberofComponents() const override;
	ComponentFormat GetComponentFormat(size_t _index) const override;
	ColourSpace GetColourSpace() const override;

	size_t GetPixelSizeInBits() const override;
	size_t GetRowStrideInBits() const override;
	size_t GetRawSizeInBytes() const override;

	uint8_t const * GetRawByteData() const override;
	float const * GetRawFloatData() const override;
	std::string GetOriginalPath() const override;

	float GetComponent2DAsFloat(size_t _x, size_t _y, size_t _comp) const override;
	uint8_t GetComponent2DAsUint8(size_t _x, size_t _y, size_t _comp) const override;

	bool GetBulk2DAsFloats(float * _dest) const override;
	bool GetBulk2DAsUint8s(uint8_t * _dest) const override;


	char const* GetMetadata() const override;
protected:
	friend class UE4InterchangeTexture;
	mutable std::string name;
	UTexture* ueTexture;
};

class UE4InterchangeSampler : public rpri::generic::ISampler
{
public:
	UE4InterchangeSampler(UTexture* _ueTexture);

	char const * GetId() const override;

	FilterType GetMinFilter() const override;
	FilterType GetMagFilter() const override;

	WrapType GetWrapS() const override;
	WrapType GetWrapT() const override;
	WrapType GetWrapR() const override;

	char const * GetMetadata() const override;
protected:
	mutable std::string name;
	UTexture* ueTexture;
};


class UE4InterchangeTexture : public rpri::generic::ITexture
{
public:
	UE4InterchangeTexture(	UE4InterchangeSampler * _sampler,
							UE4InterchangeImage * _image);

	char const * GetId() const override;

	rpri::generic::ISampler const * GetSampler() const override;

	rpri::generic::IImage const * GetImage() const override;

	bool GetImageYUp() const override;

	ITexture::TextureType GetTextureType() const override;

	char const * GetMetadata() const override;
protected:
	mutable std::string name;

	UE4InterchangeSampler *sampler;
	UE4InterchangeImage * image;
};

#endif // WITH_EDITOR
