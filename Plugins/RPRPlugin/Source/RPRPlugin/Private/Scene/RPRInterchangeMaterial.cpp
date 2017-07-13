// RPR COPYRIGHT

#include "RPRInterchangeMaterial.h"
#include "Components/SceneComponent.h"
#include "StaticMeshResources.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include <set>

UE4InterchangeMaterialNode::UE4InterchangeMaterialNode(
	UEInterchangeCollection & _collection,
	UMaterialExpression * _expression) :
	expression(_expression)
{
	id = TCHAR_TO_ANSI(*expression->GetName());
	name = id;
	type = expression->StaticClass()->GetFName().GetPlainANSIString();
   
	UE4InterchangeMaterialValue::Ptr valuePtr;
	std::string valueName;
	std::string fieldName;

	// TODO add a hash switch rather than ifs
	if(expression->IsA(UMaterialExpressionConstant::StaticClass()))
	{
		fieldName = "R";
		valueName = id + fieldName;
		auto con = static_cast<UMaterialExpressionConstant *>(expression);
		valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(), con->R);
	} else if (expression->IsA(UMaterialExpressionConstant2Vector::StaticClass()))
	{
		fieldName = "RG";
		valueName = id + fieldName;
		auto con = static_cast<UMaterialExpressionConstant2Vector *>(expression);
		valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
																con->R, con->G);
	}
	else if (expression->IsA(UMaterialExpressionConstant3Vector::StaticClass()))
	{
		fieldName = "Constant";
		valueName = id + fieldName;
		auto con = static_cast<UMaterialExpressionConstant3Vector *>(expression);
		valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
																con->Constant);
	}
	else if (expression->IsA(UMaterialExpressionConstant4Vector::StaticClass()))
	{
		fieldName = "Constant";
		valueName = id + fieldName;
		auto con = static_cast<UMaterialExpressionConstant4Vector *>(expression);
		valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
																con->Constant);
	} else
	{
		for (int i = 0; i < expression->GetInputs().Num(); ++i)
		{
			if (expression->GetInput(i) == nullptr)
				continue;
			FExpressionInput* input = expression->GetInput(i);
			if (input->Expression != nullptr)
			{
				auto childNode = UE4InterchangeMaterialNode::New(_collection, input->Expression);
				nodes.emplace_back(childNode);

				inputNames.emplace_back(TCHAR_TO_ANSI(*expression->GetInputName(i)));
				auto muxPtr = _collection.FindMux(childNode->GetId());
				if(muxPtr)
				{
					muxes.emplace_back(muxPtr);

				}
			}
		}
	}
	if(valuePtr)
	{
		values.emplace_back(valuePtr);
		auto muxPtr = _collection.FindMux(valueName.c_str());
		if (muxPtr)
		{
			muxes.emplace_back(muxPtr);
			inputNames.emplace_back(fieldName);
		}
	}

	// transfer all the nodes, values and muxes to the material graphs
	// collection

	_collection.nodeStorage.insert(nodes.begin(), nodes.end());
	_collection.valueStorage.insert(values.begin(), values.end());
	// set the muxes into muxstorage and the lookup table
	for(auto&& mux : muxes)
	{
		auto it = 
			_collection.muxStorage.insert(_collection.muxStorage.end(), mux);

		_collection.nameToMuxIndex[mux->GetId()] = 
			std::distance(_collection.muxStorage.begin(), it);
	}

}

UE4InterchangeMaterialNode::Ptr 
UE4InterchangeMaterialNode::New(UEInterchangeCollection & _collection,
								UMaterialExpression * _expression)
{
	auto node = Ptr(new UE4InterchangeMaterialNode(_collection, _expression));
	_collection.nodeStorage.insert(node);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(node);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return node;
}

UE4InterchangePBRNode::Ptr
UE4InterchangePBRNode::New(UEInterchangeCollection & _collection,
							UE4InterchangeMaterialGraph * _mg)
{
	auto node = Ptr( new UE4InterchangePBRNode(_collection, _mg) );
	_collection.nodeStorage.insert(node);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(node);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return node;
}

UE4InterchangePBRNode::UE4InterchangePBRNode(	UEInterchangeCollection & _collection, 
												UE4InterchangeMaterialGraph * _mg)
{
	const UMaterial* ue4Mat = _mg->ue4Mat;

	FString str = _mg->GetName();
	str + "PBRNode";
	id = TCHAR_TO_ANSI(*str);

	if (ue4Mat->BaseColor.UseConstant)
	{
		std::string name = id + "BaseColor";

		auto val = UE4InterchangeMaterialValue::New(_collection,
													name.c_str(),
													ue4Mat->BaseColor.Constant);
		auto muxPtr = _collection.FindMux(name.c_str());
		if (muxPtr)
		{
			muxes[0] = muxPtr;
		}
	}
	else
	{
		auto ue4Name = ue4Mat->BaseColor.Expression->GetName();
		auto cName = TCHAR_TO_ANSI(*ue4Name);

		auto node = UE4InterchangeMaterialNode::New(_collection,
													ue4Mat->BaseColor.Expression);

		auto muxPtr = _collection.FindMux(cName);
		if (muxPtr)
		{
			muxes[0] = muxPtr;
		}
		else
		{
			muxes[0] = nullptr;
		}
	}
}
UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, float _a)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _a));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
}

UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _a, _b));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
}

UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b, float _c)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _a, _b,_c));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
}

UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id,
	float _a, float _b, float _c, float _d)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _a, _b, _c,_d));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
}

UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, FLinearColor _col)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _col));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
}

UE4InterchangeMaterialValue::Ptr UE4InterchangeMaterialValue::New(
	UEInterchangeCollection & _collection, char const * _id, FColor _col)
{
	auto val = Ptr(new UE4InterchangeMaterialValue(_id, _col));
	_collection.valueStorage.insert(val);

	auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
	_collection.muxStorage.emplace_back(mux);

	auto it = _collection.muxStorage.insert(_collection.muxStorage.end(), mux);
	_collection.nameToMuxIndex[mux->GetId()] =
		std::distance(_collection.muxStorage.begin(), it);

	return val;
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

	// Interchange treats the destination PBR object as a node
	UE4InterchangePBRNode::New(collection, this);

	for(auto && value : collection.valueStorage)
	{
		valueStorage.push_back(value);
	}
	for (auto && node : collection.nodeStorage)
	{
		nodeStorage.push_back(node);
	}
	for (auto && mux : collection.muxStorage)
	{
		muxStorage.push_back(mux);
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
	return TCHAR_TO_ANSI(*ue4Mat->GetName());
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