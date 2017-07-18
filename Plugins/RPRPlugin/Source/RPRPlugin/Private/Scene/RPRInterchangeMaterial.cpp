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
	return false;
}
IMaterialNodeMuxPtr ConvertUMaterialExpression(
	UEInterchangeCollection & _collection,
	std::string const & _id,
	UMaterialExpression * _expression)
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
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->DefaultValue);
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

		// to investigate if some classes don't expose inputs in a generic
		// method sometimes (I think its when usign 'value' semantics on certain
		// inputs. If so will have to handle each expression subclass manually
		// *sob*
		if (_expression->IsA(UMaterialExpressionLinearInterpolate::StaticClass()))
		{
		}
		auto node = IMaterialNodePtr(new UE4InterchangeMaterialNode(_collection, _id, _expression));
		_collection.nodeStorage[_id.c_str()] = node;
		return _collection.FindMux(_id.c_str());

	}

}


UE4InterchangeMaterialNode::UE4InterchangeMaterialNode(
	UEInterchangeCollection & _collection,
	std::string const & _id,
	UMaterialExpression * _expression) :
	expression(_expression)
{
	id = _id;
	name = TCHAR_TO_ANSI(*expression->GetName());
	type = expression->GetClass()->GetFName().GetPlainANSIString();
	type = std::regex_replace(type, std::regex("MaterialExpression"), "UE4");
	// do the inputs 
	for (int i = 0; i < _expression->GetInputs().Num(); ++i)
	{
		if (_expression->GetInput(i) == nullptr)
			continue;

		FExpressionInput* input = _expression->GetInput(i);
		if (input->Expression != nullptr)
		{
			auto childMux = UE4InterchangeMaterialNode::New(_collection,
								std::string(),
								input->Expression);

			if(!childMux->IsEmpty())
			{
				muxes.emplace_back(childMux);

				if(childMux->IsNode())
				{
					nodes.push_back(childMux->GetAsNode());
				} else
				{
					values.push_back(childMux->GetAsValue());
				}

				inputNames.emplace_back(TCHAR_TO_ANSI(*expression->GetInputName(i)));
			}

		}
	}



}

IMaterialNodeMuxPtr
UE4InterchangeMaterialNode::New(UEInterchangeCollection & _collection,
								std::string const & _id,
								UMaterialExpression * _expression)
{
	std::string id = _id;
	if(_id.empty())
	{
		id = TCHAR_TO_ANSI(*_expression->GetName());
	}

	auto it = _collection.nodeStorage.find(id);
	if(it == _collection.nodeStorage.end())
	{
		return ConvertUMaterialExpression(_collection, id, _expression);
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

	if (ue4Mat->BaseColor.UseConstant)
	{
		std::string name = id + "BaseColor";

		auto val = UE4InterchangeMaterialValue::New(_collection,
													name.c_str(),
													ue4Mat->BaseColor.Constant);
		auto muxPtr = _collection.FindMux(name.c_str());
		assert(muxPtr);
		muxes[0] = muxPtr;
	}
	else
	{
		auto ue4Name = ue4Mat->BaseColor.Expression->GetName();
		auto cName = TCHAR_TO_ANSI(*ue4Name);

		auto mux = UE4InterchangeMaterialNode::New(_collection,
													cName,
													ue4Mat->BaseColor.Expression);
		muxes[0] = mux;
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
	return false;
}
rpri::generic::ITexture const *
UE4InterchangeMaterialNode::GetTextureInput() const
{
	return nullptr;
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

char const* UE4InterchangeMaterialValue::GetId() const
{
	return id.c_str();
}

rpri::generic::IMaterialValue::ValueType UE4InterchangeMaterialValue::GetType() const
{
	return ValueType::Float;
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
	return "";
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

UE4InterchangeMaterialGraph::UE4InterchangeMaterialGraph(const UMaterial* _ue4Mat)
	: ue4Mat(_ue4Mat)
{
	using namespace rpri::generic;

	UEInterchangeCollection collection;

	std::string name = TCHAR_TO_ANSI(*_ue4Mat->GetName());

	// Interchange treats the destination PBR object as a node
	UE4InterchangePBRNode::New(collection, name + "PBRMaterial", this);

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