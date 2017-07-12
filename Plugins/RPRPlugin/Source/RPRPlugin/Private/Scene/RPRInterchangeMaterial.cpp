// RPR COPYRIGHT

#include "RPRInterchangeMaterial.h"
#include "Components/SceneComponent.h"
#include "StaticMeshResources.h"

UE4InterchangeMaterialNode::UE4InterchangeMaterialNode(UMaterialExpression * _expression)
	: expression(_expression)
{
	id = TCHAR_TO_ANSI(*expression->GetName());
	name = id;
	type = expression->StaticClass()->GetFName().GetPlainANSIString();
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
	return 0;
}

rpri::generic::IMaterialNodeMux const *
UE4InterchangeMaterialNode::GetInputAt(size_t _index) const
{
	return nullptr;
}

char const * UE4InterchangeMaterialNode::GetInputNameAt(size_t _index) const
{
	return nullptr;
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
	return nullptr;
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(float _a)
{
	values.push_back(_a);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(float _a, float _b)
{
	values.push_back(_a);
	values.push_back(_b);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(float _a
	, float _b, float _c)
{
	values.push_back(_a);
	values.push_back(_b);
	values.push_back(_c);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(float _a
	, float _b, float _c, float _d)
{
	values.push_back(_a);
	values.push_back(_b);
	values.push_back(_c);
	values.push_back(_d);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(FLinearColor _col)
{
	values.push_back(_col.R);
	values.push_back(_col.G);
	values.push_back(_col.B);
	values.push_back(_col.A);
}

UE4InterchangeMaterialValue::UE4InterchangeMaterialValue(FColor _col)
{
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

void UE4InterchangeMaterialValue::SetId(const std::string& _id)
{
	id.assign(_id);
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
	return nodeptr == nullptr;
}

bool UE4InterchangeMaterialNodeMux::IsNode() const
{
	return true;
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

UE4InterchangePBRNode::UE4InterchangePBRNode(UE4InterchangeMaterialGraph * _mg)
{
	const UMaterial* ue4Mat = _mg->GetUE4Material();

	FString str = _mg->GetName();
	str + "PBRNode";
	id = TCHAR_TO_ANSI(*str);

	if (ue4Mat->BaseColor.UseConstant)
	{

		auto val = std::make_shared<UE4InterchangeMaterialValue>(ue4Mat->BaseColor.Constant);
		val->SetId(id + "BaseColor");
		auto mux = std::make_shared<UE4InterchangeMaterialNodeMux>(val);
		muxes[0] = mux;

		_mg->valueStorage.emplace_back(val);
		_mg->interchangeMuxes.emplace_back(mux);
	}
	else
	{
		auto it = _mg->stringToInterchangeMuxIndex.find(ue4Mat->BaseColor.Expression->GetName());
		if (it != _mg->stringToInterchangeMuxIndex.end())
		{
			muxes[0] = _mg->interchangeMuxes[it->second];
		}
		else
		{
			muxes[0] = nullptr;
		}
	}
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

char const* UE4InterchangePBRNode::GetMetaData() const
{
	return nullptr;
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
	// UE4 Expressions == Interchange nodes/values/mux
	nodeStorage.resize(ue4Mat->Expressions.Num() + 1);
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