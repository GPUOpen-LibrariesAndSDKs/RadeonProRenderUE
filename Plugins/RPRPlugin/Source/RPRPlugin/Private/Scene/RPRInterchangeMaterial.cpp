// RPR COPYRIGHT

#include "RPRInterchangeMaterial.h"
#include "Components/SceneComponent.h"
#include "StaticMeshResources.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include <set>
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include <regex>
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "RPRCrackers.h"
#include <sstream>
#include <cassert>
#include "Materials/MaterialExpressionScalarParameter.h"

namespace {
template<int _elementCount, int _remap0 = 0, int _remap1 = 1, int _remap2 = 2, int _remap3 = 3>
void FloatToByteCopy(size_t _width,
	size_t _height,
	float const * _src,
	uint8_t * _dest)
{
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			switch (_elementCount)
			{
			case 4: *_dest = uint8_t(*(_src + _remap0)*255.0f); _dest++;
			case 3: *_dest = uint8_t(*(_src + _remap1)*255.0f); _dest++;
			case 2: *_dest = uint8_t(*(_src + _remap2)*255.0f); _dest++;
			case 1: *_dest = uint8_t(*(_src + _remap3)*255.0f); _dest++;
				break;
			default:;
			}
			_src += _elementCount;
		}
	}
}

template<int _elementCount, int _remap0 = 0, int _remap1 = 1, int _remap2 = 2, int _remap3 = 3>
void ByteToByteCopy(size_t _width,
	size_t _height,
	uint8_t const * _src,
	uint8_t * _dest)
{
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			switch (_elementCount)
			{
			case 4: *_dest = *(_src + _remap0); _dest++;
			case 3: *_dest = *(_src + _remap1); _dest++;
			case 2: *_dest = *(_src + _remap2); _dest++;
			case 1: *_dest = *(_src + _remap3); _dest++;
				break;
			default:;
			}
			_src += _elementCount;
		}
	}
}
} // end anonymous namespace 

std::shared_ptr<rpri::generic::IMaterialNodeMux> 
UEInterchangeCollection::FindMux(char const * _name)
{
	auto it = muxStorage.find(_name);
	if (it == muxStorage.end())
	{
		// find it in the node or value store and add a mux it
		auto nit = nodeStorage.find(_name);
		if (nit != nodeStorage.end())
		{
			muxStorage[_name] =
				std::make_shared<UE4InterchangeMaterialNodeMux>(nit->second);
			return muxStorage[_name];
		}
		auto vit = valueStorage.find(_name);
		if(vit != valueStorage.end())
		{
			muxStorage[_name] =
				std::make_shared<UE4InterchangeMaterialNodeMux>(vit->second);
			return muxStorage[_name];
		}
		return std::shared_ptr<rpri::generic::IMaterialNodeMux>();
	}
	return it->second;
}

bool ShouldBeConvertedToMaterialValue(UMaterialExpression * _expression)
{
	// TODO add a hash switch rather than ifs
	if (_expression->IsA(UMaterialExpressionConstant::StaticClass()))
	{
		return true;
	}
	if (_expression->IsA(UMaterialExpressionConstant2Vector::StaticClass()))
	{
		return true;
	}
	if (_expression->IsA(UMaterialExpressionConstant3Vector::StaticClass()))
	{
		return true;
	}
	if (_expression->IsA(UMaterialExpressionConstant4Vector::StaticClass()))
	{
		return true;
	}
	if (_expression->IsA(UMaterialExpressionVectorParameter::StaticClass()))
	{
		return true;
	}
	if (_expression->IsA(UMaterialExpressionScalarParameter::StaticClass()))
	{
		return true;
	}
	return false;
}
IMaterialNodeMuxPtr ConvertUMaterialExpression(
	UEInterchangeCollection & _collection,
	std::string const & _id,
	UMaterialExpression * _expression,
	FName const _fname)
{

	// biggest split is node or value
	if(ShouldBeConvertedToMaterialValue(_expression))
	{
		IMaterialValuePtr valuePtr;
		std::string valueName;
		std::string fieldName;

		// value
		if (_expression->IsA(UMaterialExpressionConstant::StaticClass()))
		{
			fieldName = "R";
			valueName = _id + fieldName;
			auto con = static_cast<UMaterialExpressionConstant *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(), con->R);
		}
		else if (_expression->IsA(UMaterialExpressionConstant2Vector::StaticClass()))
		{
			fieldName = "RG";
			valueName = _id + fieldName;
			auto con = static_cast<UMaterialExpressionConstant2Vector *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->R, con->G);
		}
		else if (_expression->IsA(UMaterialExpressionConstant3Vector::StaticClass()))
		{
			fieldName = "Constant";
			valueName = _id + fieldName;
			auto con = static_cast<UMaterialExpressionConstant3Vector *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->Constant);
		}
		else if (_expression->IsA(UMaterialExpressionConstant4Vector::StaticClass()))
		{
			fieldName = "Constant";
			valueName = _id + fieldName;
			auto con = static_cast<UMaterialExpressionConstant4Vector *>(_expression);

			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->Constant);
		} else if(_expression->IsA(UMaterialExpressionVectorParameter::StaticClass()))
		{
			fieldName = "DefaultValue";
			valueName = _id + fieldName;
			auto con = static_cast<UMaterialExpressionVectorParameter *>(_expression);

			FLinearColor overCol = con->DefaultValue;
			_collection.ue4MatInterface->GetVectorParameterValue(_fname, overCol);

			valuePtr = UE4InterchangeMaterialValue::New(_collection, 
														valueName.c_str(),
														overCol);
		} else if (_expression->IsA(UMaterialExpressionScalarParameter::StaticClass()))
		{
			auto con = static_cast<UMaterialExpressionScalarParameter *>(_expression);

			float overF = con->DefaultValue;
			bool okay = _collection.ue4MatInterface->GetScalarParameterValue(_fname, overF);
			fieldName = "DefaultValue";

			if(okay)
			{
				fieldName = _fname.GetPlainANSIString();				
			}

			valueName = _id + fieldName;
			valuePtr = UE4InterchangeMaterialValue::New(_collection,
				valueName.c_str(),
				overF);
		}
		else
		{
			// should not get here
			assert(false);
		}
		_collection.valueStorage[valueName.c_str()] = valuePtr;

		return _collection.FindMux(valueName.c_str());

	} else
	{
		// node
		IMaterialValuePtr valuePtr;

		auto node = IMaterialNodePtr(new UE4InterchangeMaterialNode(_collection, _id, _expression));
		_collection.nodeStorage[_id.c_str()] = node;
		return _collection.FindMux(_id.c_str());

	}

}
#pragma optimize("",off)


void UE4InterchangeMaterialNode::ConvertFExpressionInput(UEInterchangeCollection& _collection, 
														FExpressionInput* _input,
														char const *_name,
														FName const _fname)
{
	if (_input->Expression != nullptr)
	{
		std::string ename = TCHAR_TO_ANSI(*_input->Expression->GetName());
		auto childMux = UE4InterchangeMaterialNode::New(_collection,
		                                                ename,
														_input->Expression,
														_fname);
		if (!childMux->IsEmpty())
		{
			muxes.emplace_back(childMux);

			if (childMux->IsNode())
			{
				nodes.push_back(childMux->GetAsNode());
			}
			else
			{
				values.push_back(childMux->GetAsValue());
			}

			inputNames.emplace_back(_name);
		}
	}
}
void UE4InterchangeMaterialNode::ConvertTextureSampleExpression(
									UEInterchangeCollection& _collection, 
									UMaterialExpressionTextureSample* con)
{
	if (con->Texture != nullptr)
	{
		std::string texName = std::string(TCHAR_TO_ANSI(*con->Texture->GetName())) +
								"Texture";

		auto texIt = _collection.textureStorage.find(texName);
		if(texIt == _collection.textureStorage.end())
		{
			auto image = std::make_shared<UE4InterchangeImage>(con->Texture);
			auto sampler = std::make_shared<UE4InterchangeSampler>(con->Texture);
			auto tex = std::make_shared<UE4InterchangeTexture>(sampler.get(), image.get());
			_collection.imageStorage[image->GetId()] = image;
			_collection.samplerStorage[sampler->GetId()] = sampler;
			_collection.textureStorage[tex->GetId()] = tex;
			texture = tex.get();
		} else
		{
			texture = texIt->second.get();
		}
		auto texValue = UE4InterchangeMaterialValue::New(_collection, 
													(texName + "Mux").c_str(), 
													texName.c_str());

		muxes.emplace_back(_collection.FindMux(texValue->GetId()));
		inputNames.emplace_back("Tex");
		values.push_back(texValue.get());
		_collection.valueStorage[texValue->GetId()] = texValue;
	} else
	{
		// look up TextureObject?
	}
	ConvertFExpressionInput(_collection, &con->Coordinates, "UV");
}

UE4InterchangeMaterialNode::UE4InterchangeMaterialNode(
	UEInterchangeCollection & _collection,
	std::string const & _id,
	UMaterialExpression * _expression) :
	expression(_expression),
	texture(nullptr)
{
	id = _id;
	name = TCHAR_TO_ANSI(*expression->GetName());
	type = expression->GetClass()->GetFName().GetPlainANSIString();
	type = std::regex_replace(type, std::regex("MaterialExpression"), "UE4");

	// some classes don't expose inputs via the generic methods 
	// so handle each one we care about with custom handler
	if (_expression->IsA(UMaterialExpressionTextureSample::StaticClass()))
	{	
		auto con = static_cast<UMaterialExpressionTextureSample*>(_expression);
		ConvertTextureSampleExpression(_collection, con);
	}
	else
	{
		// do the inputs 
		for (int i = 0; i < _expression->GetInputs().Num(); ++i)
		{
			auto fname = FName(*expression->GetInputName(i));
			FExpressionInput* input = _expression->GetInput(i);
			if (input == nullptr)
				continue;

			auto name = TCHAR_TO_ANSI(*expression->GetInputName(i));
			ConvertFExpressionInput(_collection, input, name, fname);
		}
	}
}
#pragma optimize("",on)

IMaterialNodeMuxPtr
UE4InterchangeMaterialNode::New(UEInterchangeCollection & _collection,
								std::string const & _id,
								UMaterialExpression * _expression,
								FName const _fname)
{
	std::string id = _id;
	if(_id.empty())
	{
		id = TCHAR_TO_ANSI(*_expression->GetName());
	}

	auto it = _collection.nodeStorage.find(id);
	if(it == _collection.nodeStorage.end())
	{
		return ConvertUMaterialExpression(_collection, id, _expression, _fname);
	}
	else
	{
		return _collection.FindMux(it->second->GetId());
	}
}

IMaterialNodePtr
UE4InterchangePBRNode::New(UEInterchangeCollection & _collection,
							std::string const & _id,
							UE4InterchangeMaterialGraph * _mg)
{
	auto it = _collection.nodeStorage.find(_id);
	if (it == _collection.nodeStorage.end())
	{

		auto node = IMaterialNodePtr(new UE4InterchangePBRNode(_collection, _id, _mg));
		_collection.nodeStorage[_id] = node;
		return node;
	} else
	{
		return it->second;
	}
}

UE4InterchangePBRNode::UE4InterchangePBRNode(	UEInterchangeCollection & _collection, 
												std::string const & _id,
												UE4InterchangeMaterialGraph * _mg)
{
	const UMaterial* ue4Mat = _mg->ue4Mat;

	id = _id;

	if ((ue4Mat->BaseColor.UseConstant) || 
		(ue4Mat->BaseColor.Expression == nullptr))
	{
		std::string name = id + "BaseColor";
		// give up assign a horrible color (maybe should match unreal
		// default colour in release for minimal visible impact...)
		FColor col = FColor(255, 255, 0, 255);
		if(ue4Mat->BaseColor.UseConstant)
		{
			col = ue4Mat->BaseColor.Constant;
		}

		auto val = UE4InterchangeMaterialValue::New(_collection,
													name.c_str(),
													col);
		auto muxPtr = _collection.FindMux(name.c_str());
		assert(muxPtr);
		muxes[0] = muxPtr;
	}
	else if (ue4Mat->BaseColor.Expression != nullptr)
	{
		auto ue4Name = ue4Mat->BaseColor.Expression->GetName();
		auto cName = TCHAR_TO_ANSI(*ue4Name);

		auto mux = UE4InterchangeMaterialNode::New(_collection,
			cName,
			ue4Mat->BaseColor.Expression);
		muxes[0] = mux;
	}

	if ((ue4Mat->Metallic.UseConstant) ||
		(ue4Mat->Metallic.Expression == nullptr))
	{
		std::string name = id + "Metallic";
		float metal = 0.5f;
		if (ue4Mat->Metallic.UseConstant)
		{
			metal = ue4Mat->Metallic.Constant;
		}

		auto val = UE4InterchangeMaterialValue::New(_collection,
			name.c_str(),
			metal);
		auto muxPtr = _collection.FindMux(name.c_str());
		assert(muxPtr);
		muxes[1] = muxPtr;
	}
	else if (ue4Mat->Metallic.Expression != nullptr)
	{
		auto ue4Name = ue4Mat->Metallic.Expression->GetName();
		auto cName = TCHAR_TO_ANSI(*ue4Name);

		auto mux = UE4InterchangeMaterialNode::New(_collection,
			cName,
			ue4Mat->Metallic.Expression);
		muxes[1] = mux;
	}
}
IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, float _a)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _a));
		_collection.valueStorage[_id] = val;

		return val;
	} else
	{
		return it->second;
	}
}

IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _a, _b));
		_collection.valueStorage[_id] = val;

		return val;
	}
	else
	{
		return it->second;
	}
}

IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b, float _c)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _a, _b,_c));
		_collection.valueStorage[_id] = val;

		return val;
	}
	else
	{
		return it->second;
	}
}

IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b, float _c, float _d)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _a, _b, _c,_d));
		_collection.valueStorage[_id] = val;

		return val;
	}
	else
	{
		return it->second;
	}
}

IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, FLinearColor _col)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _col));
		_collection.valueStorage[_id] = val;

		return val;
	}
	else
	{
		return it->second;
	}
}
IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, std::string const & _string)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _string));
		_collection.valueStorage[_id] = val;

		return val;
	}
	else
	{
		return it->second;
	}
}

IMaterialValuePtr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, FColor _col)
{
	auto it = _collection.valueStorage.find(_id);
	if (it == _collection.valueStorage.end())
	{
		auto val = IMaterialValuePtr(new UE4InterchangeMaterialValue(_id, _col));
		_collection.valueStorage[_id] = val;
		return val;
	}
	else
	{
		return it->second;
	}
}

char const * UE4InterchangeMaterialNode::GetId() const
{
	return id.c_str();
}

char const * UE4InterchangeMaterialNode::GetName() const
{
	return name.c_str();
}

char const * UE4InterchangeMaterialNode::GetType() const
{
	return type.c_str();
}

size_t UE4InterchangeMaterialNode::GetNumberOfInputs() const
{
	return muxes.size();
}

rpri::generic::IMaterialNodeMux const *
UE4InterchangeMaterialNode::GetInputAt(size_t _index) const
{
	return muxes.at(_index).get();
}

char const * UE4InterchangeMaterialNode::GetInputNameAt(size_t _index) const
{
	return inputNames.at(_index).c_str();
}

// some nodes take a texture input (the *Map set + TextureTexelSize)
// this allow access to it
bool UE4InterchangeMaterialNode::HasTextureInput() const
{
	return texture != nullptr;
}
rpri::generic::ITexture const *
UE4InterchangeMaterialNode::GetTextureInput() const
{
	return texture;
}

char const * UE4InterchangeMaterialNode::GetMetadata() const
{
	return "";
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, float _a)
{
	id = _id;
	values.push_back(_a);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, float _a, float _b)
{
	id = _id;
	values.push_back(_a);
	values.push_back(_b);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, float _a, float _b, float _c)
{
	id = _id;
	values.push_back(_a);
	values.push_back(_b);
	values.push_back(_c);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, float _a, float _b, float _c, float _d)
{
	id = _id;
	values.push_back(_a);
	values.push_back(_b);
	values.push_back(_c);
	values.push_back(_d);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, FLinearColor _col)
{
	id = _id;
	values.push_back(_col.R);
	values.push_back(_col.G);
	values.push_back(_col.B);
	values.push_back(_col.A);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, FColor _col)
{
	id = _id;
	FLinearColor lc = _col.ReinterpretAsLinear();
	values.push_back(lc.R);
	values.push_back(lc.G);
	values.push_back(lc.B);
	values.push_back(lc.A);
}
UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(
	char const * _id, std::string const & _string)
{
	id = _id;
	str = _string;
}
char const* UE4InterchangeMaterialValue::GetId() const
{
	return id.c_str();
}

rpri::generic::IMaterialValue::ValueType UE4InterchangeMaterialValue::GetType() const
{
	if (str.empty())
	{
		return ValueType::Float;
	} else
	{
		return ValueType::String;
	}
}

size_t UE4InterchangeMaterialValue::GetNumberOfValues() const
{
	return values.size();
}

uint8_t UE4InterchangeMaterialValue::GetValueAtIndexAsUint8(size_t _index) const
{
	return 0;
}

uint16_t UE4InterchangeMaterialValue::GetValueAtIndexAsUint16(size_t _index) const
{
	return 0;
}

uint32_t UE4InterchangeMaterialValue::GetValueAtIndexAsUint32(size_t _index) const
{
	return 0;
}

uint64_t UE4InterchangeMaterialValue::GetValueAtIndexAsUint64(size_t _index) const
{
	return 0;
}

int8_t UE4InterchangeMaterialValue::GetValueAtIndexAsInt8(size_t _index) const
{
	return 0;
}

int16_t UE4InterchangeMaterialValue::GetValueAtIndexAsInt16(size_t _index) const
{
	return 0;
}

int32_t UE4InterchangeMaterialValue::GetValueAtIndexAsInt32(size_t _index) const
{
	return 0;
}

int64_t UE4InterchangeMaterialValue::GetValueAtIndexAsInt64(size_t _index) const
{
	return 0;
}

float UE4InterchangeMaterialValue::GetValueAtIndexAsFloat(size_t _index) const
{
	return values.at(_index);
}

double UE4InterchangeMaterialValue::GetValueAtIndexAsDouble(size_t _index) const
{
	return values.at(_index);
}

char const* UE4InterchangeMaterialValue::GetMetadata() const
{
	return "";
}

std::string UE4InterchangeMaterialValue::GetValueAsString() const
{
	if(str.empty())
	{
		std::stringstream ss;
		ss << "float" << GetNumberOfValues() << "(";
		for (size_t i = 0; i < GetNumberOfValues(); i++)
		{
			ss << values[i];
			if(i < GetNumberOfValues() - 1)
			{
				ss << ", ";
			} else
			{
				ss << ")" << std::endl;
			}
		}
		return ss.str();
	} else
	{
		return str;
	}
}

UE4InterchangeMaterialNodeMux::UE4InterchangeMaterialNodeMux(
	std::shared_ptr<rpri::generic::IMaterialNode> _ptr)
	: nodeptr(_ptr)
	, valueptr(nullptr)
{
}

UE4InterchangeMaterialNodeMux::UE4InterchangeMaterialNodeMux(
	std::shared_ptr<rpri::generic::IMaterialValue> _ptr)
	: nodeptr(nullptr)
	, valueptr(_ptr)
{
}

char const* UE4InterchangeMaterialNodeMux::GetId() const
{
	return nodeptr ? nodeptr->GetId() : valueptr ? valueptr->GetId() : nullptr;
}

bool UE4InterchangeMaterialNodeMux::IsEmpty() const
{
	return nodeptr == nullptr && valueptr == nullptr;
}

bool UE4InterchangeMaterialNodeMux::IsNode() const
{
	return nodeptr != nullptr;
}

rpri::generic::IMaterialValue const* 
UE4InterchangeMaterialNodeMux::GetAsValue() const
{
	return valueptr.get();
}

rpri::generic::IMaterialNode const* 
UE4InterchangeMaterialNodeMux::GetAsNode() const
{
	return nodeptr.get();
}

char const* UE4InterchangePBRNode::GetId() const
{
	return id.c_str();
}

char const* UE4InterchangePBRNode::GetName() const
{
	return id.c_str();
}

char const* UE4InterchangePBRNode::GetType() const
{
	return "UE4PBRMaterial";
}

size_t UE4InterchangePBRNode::GetNumberOfInputs() const
{
	return NUMBER_OF_INPUTS_NODE;
}

rpri::generic::IMaterialNodeMux const* 
UE4InterchangePBRNode::GetInputAt(size_t _index) const
{
	return muxes[_index].get();
}

char const* UE4InterchangePBRNode::GetInputNameAt(size_t _index) const
{
	return PBRNodeFieldNames[_index];
}


bool UE4InterchangePBRNode::HasTextureInput() const
{
	return false;
}

rpri::generic::ITexture const* 
UE4InterchangePBRNode::GetTextureInput() const
{
	return nullptr;
}

char const* UE4InterchangePBRNode::GetMetadata() const
{
	return "";
}

UE4InterchangeMaterialGraph::UE4InterchangeMaterialGraph(
	UMaterialInterface const * _ue4MatInterface )
	: ue4MatInterface(_ue4MatInterface)
{
	using namespace rpri::generic;
	ue4Mat = ue4MatInterface->GetMaterial();

	UEInterchangeCollection collection;
	collection.ue4MatInterface = ue4MatInterface;

	std::string name = TCHAR_TO_ANSI(*ue4Mat->GetName());

	// Interchange treats the destination PBR object as a node
	rootNode = UE4InterchangePBRNode::New(collection, name + "PBRMaterial", this);

	for(auto && value : collection.valueStorage)
	{
		valueStorage.push_back(value.second);
	}
	for (auto && node : collection.nodeStorage)
	{
		nodeStorage.push_back(node.second);
	}
	for (auto && mux : collection.muxStorage)
	{
		muxStorage.push_back(mux.second);
	}
	for (auto && tex : collection.textureStorage)
	{
		textureStorage.push_back(tex.second);
	}
	for (auto && sampler : collection.samplerStorage)
	{
		samplerStorage.push_back(sampler.second);
	}
	for (auto && image : collection.imageStorage)
	{
		imageStorage.push_back(image.second);
	}

}

char const* UE4InterchangeMaterialGraph::GetId() const
{
	// TODO make sure ID is unique!
	return GetName();
}

UMaterial const* UE4InterchangeMaterialGraph::GetUE4Material() const
{
	return ue4Mat;
}

char const* UE4InterchangeMaterialGraph::GetName() const
{
	name = TCHAR_TO_ANSI(*ue4Mat->GetName());
	return name.c_str();
}

size_t UE4InterchangeMaterialGraph::GetNumberOfMaterialValues() const
{
	return valueStorage.size();
}

rpri::generic::IMaterialValue const* 
UE4InterchangeMaterialGraph::GetMaterialValueAt(size_t _index) const
{
	return valueStorage.at(_index).get();
}

size_t UE4InterchangeMaterialGraph::GetNumberOfMaterialNodes() const
{
	return nodeStorage.size();
}

rpri::generic::IMaterialNode const* 
UE4InterchangeMaterialGraph::GetMaterialNodeAt(size_t _index) const
{
	return nodeStorage.at(_index).get();
}

char const* UE4InterchangeMaterialGraph::GetMetadata() const
{
	return "";
}
rpri::generic::IMaterialNode const * 
UE4InterchangeMaterialGraph::GetRootNode() const
{
	return rootNode.get();
}

UE4InterchangeImage::UE4InterchangeImage(UTexture* _ueTexture) :
	ueTexture(_ueTexture)
{
}

char const* UE4InterchangeImage::GetId() const
{
	name = TCHAR_TO_ANSI(*ueTexture->GetName());
	return name.c_str();
}

size_t UE4InterchangeImage::GetWidth() const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if(plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		auto mip = plat0->Mips[0];
		return mip.SizeX;
	}
	else
	{
		return source.GetSizeX();
	}
}

size_t UE4InterchangeImage::GetHeight() const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		auto mip = plat0->Mips[0];
		return mip.SizeY;
	}
	else
	{
		return source.GetSizeY();
	}
}

size_t UE4InterchangeImage::GetDepth() const
{
	return 1;
}

size_t UE4InterchangeImage::GetSlices() const
{
	auto & source = ueTexture->Source;
	return source.GetNumSlices();
}

size_t UE4InterchangeImage::GetNumberofComponents() const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		auto mip = plat0->Mips[0];
		EPixelFormat format = plat0->PixelFormat;
		size_t compCount = CrackNumofComponents(format);
		if (compCount > 0) 
		{
			assert(mip.BulkData.GetElementCount() == compCount);
			return compCount;
		}
	}

	ETextureSourceFormat sformat = source.GetFormat();
	size_t compCount = CrackNumofComponents(sformat);
	return compCount;
}
rpri::generic::IImage::ComponentFormat UE4InterchangeImage::GetComponentFormat(size_t _index) const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		auto mip = plat0->Mips[0];
		EPixelFormat format = plat0->PixelFormat;
		ComponentFormat cformat = CrackComponentFormat(format);
		if (cformat != ComponentFormat::Unknown) return cformat;
	}
	ETextureSourceFormat sformat = source.GetFormat();
	ComponentFormat cformat = CrackComponentFormat(sformat);
	return cformat;
}

rpri::generic::IImage::ColourSpace UE4InterchangeImage::GetColourSpace() const
{
	using cs = rpri::generic::IImage::ColourSpace;
	return ueTexture->SRGB ? cs::sRGB : cs::Linear;
}

size_t UE4InterchangeImage::GetPixelSizeInBits() const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		auto mip = plat0->Mips[0];
		EPixelFormat format = plat0->PixelFormat;
		size_t bitSize = CrackPixelSizeInBits(format);
		if (bitSize > 0)
		{
			assert(mip.BulkData.GetElementSize() * 8 == bitSize);
			return bitSize;
		}
	}
	ETextureSourceFormat sformat = source.GetFormat();
	size_t bitSize = CrackPixelSizeInBits(sformat);
	return bitSize;
}

size_t UE4InterchangeImage::GetRowStrideInBits() const
{	
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		FTexture2DMipMap const mip = plat0->Mips[0];
		return mip.BulkData.GetElementSize() * mip.SizeX * 8;
	}
	return source.GetBytesPerPixel() * source.GetSizeX() * 8;
}
size_t UE4InterchangeImage::GetRawSizeInBytes() const
{
	return GetRowStrideInBits() * GetHeight() / 8;
}

uint8_t const * UE4InterchangeImage::GetRawByteData() const
{	
	// we don't support this access method (too complicated)
	return nullptr;
}

std::string UE4InterchangeImage::GetOriginalPath() const
{
	// don't think this is right but should be okay
	FTextureSource & source = ueTexture->Source;
	std::string txt = TCHAR_TO_ANSI(*ueTexture->GetName());
	return txt;
}

bool UE4InterchangeImage::GetBulk2DAsFloats(float * _dest) const
{	
	return false; // TODO will fall back to slow general case
}



bool UE4InterchangeImage::GetBulk2DAsUint8(uint8_t * _dest) const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		FTexture2DMipMap const mip = plat0->Mips[0];
		EPixelFormat const format = plat0->PixelFormat;
		void const * rawData = mip.BulkData.LockReadOnly();

		switch (format)
		{
		case PF_R32_FLOAT:
			FloatToByteCopy<1>(mip.SizeX, mip.SizeY, (float*)rawData, _dest);
			break;
		case PF_A32B32G32R32F:
			FloatToByteCopy<4, 1, 2, 3, 0>(mip.SizeX, mip.SizeY, (float*)rawData, _dest);
			break;						 
		case PF_L8:
		case PF_G8:
		case PF_R8_UINT:
			ByteToByteCopy<1>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
			break;
		case PF_R8G8:
		case PF_V8U8:
			ByteToByteCopy<2>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
			break;
		case PF_R8G8B8A8:
			ByteToByteCopy<4>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
			break;
		case PF_A8R8G8B8:
			ByteToByteCopy<4, 1, 2, 3, 0>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
			break;
		case PF_B8G8R8A8:
			ByteToByteCopy<4, 2, 1, 0, 3>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
			break;
		default:
			mip.BulkData.Unlock();
			{
				// lets try source data
				rawData = source.LockMip(0);
				ETextureSourceFormat sformat = source.GetFormat();
				uint8_t const * src = reinterpret_cast<uint8_t const*>(rawData);
				switch (sformat)
				{
				case TSF_G8:
					ByteToByteCopy<1>(source.GetSizeX(), source.GetSizeY(), (uint8_t*)rawData, _dest);
					return true;
				case TSF_BGRA8:
					ByteToByteCopy<4, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), (uint8_t*)rawData, _dest);
					return true;
				case TSF_RGBA8:
					ByteToByteCopy<4>(source.GetSizeX(), source.GetSizeY(), (uint8_t*)rawData, _dest);
					return true;
				default: 
					return false; // bulk can't handle it
				}
			}
		}
		mip.BulkData.Unlock();
		return true;
	}
	return false; // bulk can't handle it
}

float UE4InterchangeImage::GetComponent2DAsFloat(size_t _x, size_t _y, size_t _comp) const
{
	FTextureSource & source = ueTexture->Source;
	FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
	if (plats != nullptr)
	{
		FTexturePlatformData * plat0 = plats[0];
		FTexture2DMipMap const mip = plat0->Mips[0];
		EPixelFormat const format = plat0->PixelFormat;
		void const * rawData = mip.BulkData.LockReadOnly();
		if (_comp >= GetNumberofComponents()) return float();

		uint8_t const * src = reinterpret_cast<uint8_t const*>(rawData);

		src = src + (_y * mip.BulkData.GetElementSize() * mip.SizeX) +
			(_x * mip.BulkData.GetElementSize());
		// We swizzle for the RGBA vs BGRA etc. but no other reason
		// TODO need to match the GPU HW swizzle which might occur
		// on some formats..
		switch (format)
		{
		case PF_G32R32F:
		case PF_R32_FLOAT:
		case PF_A32B32G32R32F: {
			float const * srcData = reinterpret_cast<float const*>(src);
			float r = srcData[_comp];
			mip.BulkData.Unlock();
			return r;
		}

		case PF_A8:
		case PF_L8:
		case PF_G8:
		case PF_R8G8:
		case PF_V8U8:
		case PF_R8_UINT:
		case PF_R8G8B8A8:
		{
			uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
			float r = float(srcData[_comp]) / 255.0f;
			mip.BulkData.Unlock();
			return r;
		}
		case PF_A8R8G8B8:
		{
			uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
			float r = 0;
			switch (_comp)
			{
			case 0: r = float(srcData[3]); break;
			case 1: r = float(srcData[0]); break;
			case 2: r = float(srcData[1]); break;
			case 3: r = float(srcData[2]); break;
			default:;
			}
			r = r / 255.0f;
			mip.BulkData.Unlock();
			return r;
		}
		case PF_B8G8R8A8:
		{
			uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
			float r = 0;
			switch (_comp)
			{
			case 0: r = float(srcData[2]); break;
			case 1: r = float(srcData[1]); break;
			case 2: r = float(srcData[0]); break;
			case 3: r = float(srcData[3]); break;
			default:;
			}
			r = r / 255.0f;
			mip.BulkData.Unlock();
			return r;
		}
		case PF_G16:
		case PF_G16R16:
		case PF_R16_UINT:
		case PF_R16G16_UINT:
		case PF_R16G16B16A16_UINT:
		case PF_A16B16G16R16:
		{
			// todo check if non UINT type are normalized and UINT aren't
			// this may be wrong for UINT
			uint16_t const * srcData = reinterpret_cast<uint16_t const *>(src);
			float r = float(srcData[_comp]) / 65536.0f;
			mip.BulkData.Unlock();
			return r;

		}
		case PF_R16_SINT:
		case PF_R16G16B16A16_SINT:
		{
			// todo check if non SINT type are normalized and SINT aren't
			// this may be wrong for SINT
			int16_t const * srcData = reinterpret_cast<int16_t const *>(src);
			float r = float(srcData[_comp]) / (65536.0f / 2.0f);
			mip.BulkData.Unlock();
			return r;
		}
		case PF_R32_UINT:
		case PF_R32G32B32A32_UINT:
		{
			// this is lossy, we lose 9 bits (or 8 or 10 would have to check)
			uint32_t const * srcData = reinterpret_cast<uint32_t const *>(src);
			float r = float(srcData[_comp]);
			mip.BulkData.Unlock();
			return r;
		}
		case PF_R32_SINT:
		{
			// this is lossy, we lose 9 bits
			int32_t const * srcData = reinterpret_cast<int32_t const *>(src);
			float r = float(srcData[_comp]);
			mip.BulkData.Unlock();
			return r;
		}

		case PF_A1:
		{
			src = src + (_y * mip.BulkData.GetElementSize() * mip.SizeX) +
				(_x * (mip.BulkData.GetElementSize() / 8));
			uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
			uint8_t bit = srcData[_comp] & (1 << (_x & 0x7));
			mip.BulkData.Unlock();
			return bit ? float(1) : float(0);
		}

		// todo FP16
		case PF_R16F:
		case PF_G16R16F:
		case PF_R16F_FILTER:
		case PF_G16R16F_FILTER:
		case PF_FloatRGB:
		case PF_FloatRGBA:
			break;

		case PF_FloatR11G11B10:
		case PF_R5G6B5_UNORM:
		case PF_BC5:
		case PF_BC4:
		case PF_DepthStencil:
		case PF_ShadowDepth:
		case PF_A2B10G10R10:
		case PF_D24:
		case PF_DXT1:
		case PF_DXT3:
		case PF_DXT5:
		case PF_UYVY:
		case PF_PVRTC2:
		case PF_PVRTC4:
		case PF_ATC_RGB:
		case PF_ATC_RGBA_E:
		case PF_ATC_RGBA_I:
		case PF_X24_G8:
		case PF_ETC1:
		case PF_ETC2_RGB:
		case PF_ETC2_RGBA:
		case PF_ASTC_4x4:
		case PF_ASTC_6x6:
		case PF_ASTC_8x8:
		case PF_ASTC_10x10:
		case PF_ASTC_12x12:
		case PF_BC6H:
		case PF_BC7:
		case PF_Unknown:
		case PF_MAX:
		default: 
			mip.BulkData.Unlock();
			break;
		}
	}
	void const * rawData = source.LockMip(0);
	if (_comp >= GetNumberofComponents()) return float();

	uint8_t const * src = reinterpret_cast<uint8_t const*>(rawData);

	src = src + (_y * source.GetBytesPerPixel() * source.GetSizeX()) +
		(_x * source.GetBytesPerPixel());

	// TODO swizzle in this case as its source
	ETextureSourceFormat sformat = source.GetFormat();
	switch (sformat)
	{
	case TSF_G8:
	case TSF_BGRA8:
	{
		uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
		float r = 0;
		switch(_comp)
		{
		case 0: r = float(srcData[2]); break;
		case 1: r = float(srcData[1]); break;
		case 2: r = float(srcData[0]); break;
		case 3: r = float(srcData[3]); break;
		}
		r = r / 255.0f;
		return r;
	}
	case TSF_BGRE8:
	{
		// TODO the exponent part
		uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
		float r = 0;
		switch (_comp)
		{
		case 0: r = float(srcData[2]); break;
		case 1: r = float(srcData[1]); break;
		case 2: r = float(srcData[0]); break;
		case 3: r = float(1); break;
		}
		r = r / 255.0f;
		return r;
	}
	case TSF_RGBA8:
	{
		uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
		float r = 0;
		switch (_comp)
		{
		case 0: r = float(srcData[0]); break;
		case 1: r = float(srcData[1]); break;
		case 2: r = float(srcData[2]); break;
		case 3: r = float(srcData[3]); break;
		}
		r = r / 255.0f;
		return r;
	}
	case TSF_RGBE8:
	{
		// TODO the exponent part
		uint8_t const * srcData = reinterpret_cast<uint8_t const *>(src);
		float r = 0;
		switch (_comp)
		{
		case 0: r = float(srcData[2]); break;
		case 1: r = float(srcData[1]); break;
		case 2: r = float(srcData[0]); break;
		case 3: r = float(1); break;
		}
		r = r / 255.0f;
		return r;
	}

	// todo fp16
	case TSF_RGBA16:
	case TSF_RGBA16F:
		break;
	case TSF_Invalid:
	case TSF_MAX:
	default: break;
	}

	// return green error texture (assuming RGB format)
	if (_comp == 1 || _comp == 3)
	{
		return float(1);
	}
	else
	{
		return float(0);
	}
}


uint8_t UE4InterchangeImage::GetComponent2DAsUint8(size_t _x, size_t _y, size_t _comp) const
{

	float r = GetComponent2DAsFloat(_x, _y, _comp);
	return uint8_t(r * 255.0f);
}

char const* UE4InterchangeImage::GetMetadata() const
{
	return "";
}


UE4InterchangeSampler::UE4InterchangeSampler(UTexture* _ueTexture) :
	ueTexture(_ueTexture)
{
}
char const * UE4InterchangeSampler::GetId() const
{
	name = TCHAR_TO_ANSI(*ueTexture->GetName());
	name = name +"Sampler";
	return name.c_str();
}

rpri::generic::ISampler::FilterType UE4InterchangeSampler::GetMinFilter() const
{
	return CrackTextureFilter(ueTexture->Filter);
}

rpri::generic::ISampler::FilterType UE4InterchangeSampler::GetMagFilter() const
{
	return CrackTextureFilter(ueTexture->Filter);
}

rpri::generic::ISampler::WrapType UE4InterchangeSampler::GetWrapS() const
{
	using wt = rpri::generic::ISampler::WrapType;
	if(ueTexture->bNoTiling)
	{
		return wt::ClampToEdge;
	} else
	{
		return wt::Repeat;
	}
}

rpri::generic::ISampler::WrapType UE4InterchangeSampler::GetWrapT() const
{
	using wt = rpri::generic::ISampler::WrapType;
	if (ueTexture->bNoTiling)
	{
		return wt::ClampToEdge;
	}
	else
	{
		return wt::Repeat;
	}
}

rpri::generic::ISampler::WrapType UE4InterchangeSampler::GetWrapR() const
{
	using wt = rpri::generic::ISampler::WrapType;
	if (ueTexture->bNoTiling)
	{
		return wt::ClampToEdge;
	}
	else
	{
		return wt::Repeat;
	}
}

char const * UE4InterchangeSampler::GetMetadata() const
{
	return "";
}

UE4InterchangeTexture::UE4InterchangeTexture(
										UE4InterchangeSampler * _sampler,
										UE4InterchangeImage * _image) :
	sampler(_sampler), image(_image)
{
}

char const * UE4InterchangeTexture::GetId() const
{
	name = image->GetId();
	name = name + "Texture";
	return name.c_str();

}

rpri::generic::ISampler const * UE4InterchangeTexture::GetSampler() const
{
	return sampler;
}

rpri::generic::IImage const * UE4InterchangeTexture::GetImage() const
{
	return image;
}

bool UE4InterchangeTexture::GetImageYUp() const
{
	return true;
}

rpri::generic::ITexture::TextureType UE4InterchangeTexture::GetTextureType() const
{
	using tt = rpri::generic::ITexture::TextureType;

	if(image->ueTexture->IsNormalMap())
	{
		return tt::Normal;
	} else
	{
		// todo better classifications
		return tt::General;
	}
}
char const* UE4InterchangeTexture::GetMetadata() const
{
	return "";
}
