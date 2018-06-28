/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#if WITH_EDITOR

#include "RPRInterchangeMaterial.h"
#include "Components/SceneComponent.h"
#include "StaticMeshResources.h"
#include "RPRCrackers.h"

#include "Materials/Material.h"
#include "Materials/MaterialExpression.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionDesaturation.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionStaticSwitch.h"
#include "Materials/MaterialExpressionMakeMaterialAttributes.h"

#include <set>
#include <regex>
#include <sstream>
#include <cassert>
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "RPRCpMaterial.h"

// all texture dimesion greater than this will be reduced by 2
#define DIMENSION_LIMIT 8192
#define TRY_PLATFORM_DATA_FOR_IMAGES false

namespace {
template<int _elementCount, int _xinc = 1, int _yinc = 1, int _remap0 = 0, int _remap1 = 1, int _remap2 = 2, int _remap3 = 3>
void FloatToByteCopy(size_t _width,
	size_t _height,
	float const * _src,
	uint8_t * _dest)
{
	for (int y = 0; y < _height; y+= _yinc)
	{
		for (int x = 0; x < _width; x+= _xinc)
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
		if (_yinc > 1)
		{
			_src += _elementCount * _width *(_yinc - 1);
		}
	}
}

template<int _elementCount, int _xinc = 1, int _yinc = 1, int _remap0 = 0, int _remap1 = 1, int _remap2 = 2, int _remap3 = 3>
void ByteToByteCopy(size_t _width,
	size_t _height,
	uint8_t const * _src,
	uint8_t * _dest)
{
	for (int y = 0; y < _height; y+=_yinc)
	{
		for (int x = 0; x < _width; x+=_xinc)
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
		if (_yinc > 1)
		{
			_src += _elementCount * _width *(_yinc - 1);
		}
	}
}
template<int _elementCount, int _xinc = 1, int _yinc = 1, int _remap0 = 0, int _remap1 = 1, int _remap2 = 2, int _remap3 = 3>
void sRGBByteToLinearByteCopy(size_t _width,
	size_t _height,
	uint8_t const * _src,
	uint8_t * _dest)
{
	for (int y = 0; y < _height; y+= _yinc)
	{
		for (int x = 0; x < _width; x+= _xinc)
		{
			float f[4] = { 0,0,0,0 };

			switch (_elementCount)
			{
			case 4: f[3] = *(_src + _remap0);
			case 3: f[2] = *(_src + _remap1);
			case 2: f[1] = *(_src + _remap2);
			case 1: f[0] = *(_src + _remap3);
				break;
			default:;
			}
			_src += _elementCount * _xinc;

			// don't correct alpha (tho this is argueble re blinns,
			// ghost in a snow storm article)
			// sRGB to linear (not OPTIMISED!)
			for (int i = _elementCount - 1; i > 0; --i)
			{
				static float const a = 0.055f;
				static float const b = 0.04045f;
				f[i] *= (1.f / 255.f);
				if (f[i] <= b) { f[i] *= (1.0f / 12.92f); }
				else
				{
					f[i] = (f[i] + a) / (1.f + a);
					f[i] = powf(f[i], 2.4f);
				}
				f[i] = f[i] > 1.f ? 1 : f[i];
				f[i] = f[i] < 0.f ? 0 : f[i];
			}
			*_dest = uint8_t(f[3] * 255.f); _dest++;
			*_dest = uint8_t(f[2] * 255.f); _dest++;
			*_dest = uint8_t(f[1] * 255.f); _dest++;
			*_dest = uint8_t(f[0]); _dest++;
		}
		if(_yinc > 1)
		{
			_src += _elementCount * _width *(_yinc-1);
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
#pragma optimize("",off)
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
			valueName = _id + _fname.GetPlainANSIString() + fieldName;
			auto con = static_cast<UMaterialExpressionConstant *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(), con->R);
		}
		else if (_expression->IsA(UMaterialExpressionConstant2Vector::StaticClass()))
		{
			fieldName = "RG";
			valueName = _id + _fname.GetPlainANSIString() + fieldName;
			auto con = static_cast<UMaterialExpressionConstant2Vector *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->R, con->G);
		}
		else if (_expression->IsA(UMaterialExpressionConstant3Vector::StaticClass()))
		{
			fieldName = "Constant";
			valueName = _id + _fname.GetPlainANSIString() + fieldName;
			auto con = static_cast<UMaterialExpressionConstant3Vector *>(_expression);
			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->Constant);
		}
		else if (_expression->IsA(UMaterialExpressionConstant4Vector::StaticClass()))
		{
			fieldName = "Constant";
			valueName = _id + _fname.GetPlainANSIString() + fieldName;
			auto con = static_cast<UMaterialExpressionConstant4Vector *>(_expression);

			valuePtr = UE4InterchangeMaterialValue::New(_collection, valueName.c_str(),
				con->Constant);
		} else if(_expression->IsA(UMaterialExpressionVectorParameter::StaticClass()))
		{
			auto con = static_cast<UMaterialExpressionVectorParameter *>(_expression);

			FLinearColor overCol = con->DefaultValue;
			bool okay = _collection.ue4MatInterface->GetVectorParameterValue(con->ParameterName, overCol);
			if (okay)
			{
				fieldName = con->ParameterName.GetPlainANSIString();
			}
			else
			{
				fieldName = "DefaultValue";
			}

			valueName = _id + _fname.GetPlainANSIString() + fieldName;
			valuePtr = UE4InterchangeMaterialValue::New(_collection, 
														valueName.c_str(),
														overCol);
		} else if (_expression->IsA(UMaterialExpressionScalarParameter::StaticClass()))
		{
			auto con = static_cast<UMaterialExpressionScalarParameter *>(_expression);
			float overF = con->DefaultValue;
			bool okay = _collection.ue4MatInterface->GetScalarParameterValue(con->ParameterName, overF);
			if(okay)
			{
				fieldName = con->ParameterName.GetPlainANSIString();
			} else
			{
				fieldName = "DefaultValue";
			}

			valueName = _id + _fname.GetPlainANSIString() + fieldName;
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
		std::string valueName = _id + _fname.GetPlainANSIString();

		auto node = IMaterialNodePtr(new UE4InterchangeMaterialNode(_collection, valueName.c_str(), _expression));
		_collection.nodeStorage[valueName.c_str()] = node;
		return _collection.FindMux(valueName.c_str());

	}

}

void UE4InterchangeMaterialNode::ConvertFExpressionInput(UEInterchangeCollection& _collection, 
														FExpressionInput* _input,
														char const *_name,
														FName const _fname)
{
	if (_input->Expression != nullptr)
	{
		std::string ename = TCHAR_TO_ANSI(*_input->Expression->GetName());
		auto childMux = UE4InterchangeMaterialNode::New(_collection,
														std::string(_name) + ename,
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

			inputNames.emplace_back(_fname.GetPlainANSIString());
		}
	}
}

void UE4InterchangeMaterialNode::ConvertTexture(
	UEInterchangeCollection& _collection,
	std::string const & _valname,
	std::string const & _texname,
	UTexture * _texture)
{
	std::string const texName = _texname + "Texture";
	auto texIt = _collection.textureStorage.find(texName);
	if (texIt == _collection.textureStorage.end()) 
	{
		auto image = std::make_shared<UE4InterchangeImage>(_texture);
		auto sampler = std::make_shared<UE4InterchangeSampler>(_texture);
		auto tex = std::make_shared<UE4InterchangeTexture>(sampler.get(), image.get());
		assert(texName == tex->GetId());
		_collection.imageStorage[image->GetId()] = image;
		_collection.samplerStorage[sampler->GetId()] = sampler;
		_collection.textureStorage[tex->GetId()] = tex;
		texture = tex.get();
	}
	else
	{
		texture = texIt->second.get();
	}
	auto texValue = UE4InterchangeMaterialValue::New(_collection,
													(_valname + "Mux").c_str(),
													_valname.c_str());

	muxes.emplace_back(_collection.FindMux(texValue->GetId()));
	inputNames.emplace_back("Tex");
	values.push_back(texValue.get());
	_collection.valueStorage[texValue->GetId()] = texValue;

}

void UE4InterchangeMaterialNode::ConvertTextureSampleExpression(
								UEInterchangeCollection& _collection, 
								std::string const & _name,
								UMaterialExpressionTextureSample* con)
{
	ConvertFExpressionInput(_collection, &con->Coordinates, "UV", FName("UV"));
	if (con->Texture != nullptr)
	{
		UTexture* tex = con->Texture;
		std::string texName = TCHAR_TO_ANSI(*tex->GetName());
		std::string valName = _name + texName + "Texture";
		ConvertTexture(_collection, valName, texName, tex);
	} else
	{
		// look up TextureObject?
	}
	ConvertFExpressionInput(_collection, &con->Coordinates, "UV");
}
void UE4InterchangeMaterialNode::ConvertTextureSampleExpression(
	UEInterchangeCollection& _collection,
	std::string const & _name,
	UMaterialExpressionTextureSampleParameter2D* con)
{
	ConvertFExpressionInput(_collection, &con->Coordinates, "UV", FName("UV"));

	if (con->Texture != nullptr)
	{
		std::string paramName;
		UTexture* tex = con->Texture;
		bool okay = _collection.ue4MatInterface->GetTextureParameterValue(con->ParameterName, tex);
		if (okay)
		{
			paramName = con->ParameterName.GetPlainANSIString();
		}
		std::string texName = TCHAR_TO_ANSI(*tex->GetName());
		std::string valName = _name + paramName + texName + "Texture";
		ConvertTexture(_collection, valName, texName, tex);
	}
	else
	{
		// look up TextureObject?
	}
}

FExpressionInput const & UE4InterchangeMaterialNode::ConvertStaticSwitchExpression(
	UEInterchangeCollection& _collection,
	std::string const & _name,
	UMaterialExpressionStaticSwitch* con)
{
	std::string paramName;
	bool which = con->DefaultValue;
	if (con->Value.Expression != NULL)
	{
		auto childExp = con->Value.Expression;
		if (childExp->IsA(UMaterialExpressionStaticBoolParameter::StaticClass()))
		{
			auto sbp = static_cast<UMaterialExpressionStaticBoolParameter*>(childExp);
			which = sbp->DefaultValue;
		}
	}

	if (which)
	{
		return con->A;
	}
	else
	{
		return con->B;
	}
}

FExpressionInput const & UE4InterchangeMaterialNode::ConvertStaticSwitchParameterExpression(
	UEInterchangeCollection& _collection,
	std::string const & _name,
	UMaterialExpressionStaticSwitchParameter* con)
{
	std::string paramName;
	bool which = con->DefaultValue;

	FGuid outGuid;
	bool okay = _collection.ue4MatInterface->GetStaticSwitchParameterValue(con->ParameterName, which, outGuid);
	if (which)
	{
		return con->A;
	}
	else
	{
		return con->B;
	}
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
	// switch go first!
	if (_expression->IsA(UMaterialExpressionTextureSampleParameter2D::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionTextureSampleParameter2D*>(_expression);
		ConvertTextureSampleExpression(_collection, name.c_str(), con);
	}
	else if (_expression->IsA(UMaterialExpressionStaticSwitch::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionStaticSwitch*>(_expression);
		auto sel = ConvertStaticSwitchExpression(_collection, name.c_str(), con);
		auto fname = FName(sel.ExpressionName);
		type = "PassThrough";
		ConvertFExpressionInput(_collection, &sel, name.c_str(), fname);
	}

	else if (_expression->IsA(UMaterialExpressionStaticSwitchParameter::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionStaticSwitchParameter*>(_expression);
		auto sel = ConvertStaticSwitchParameterExpression(_collection, name.c_str(), con);
		auto fname = FName("color");
		type = "PassThrough";
		ConvertFExpressionInput(_collection, &sel, name.c_str(), fname);
	}
	else if (_expression->IsA(UMaterialExpressionTextureSample::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionTextureSample*>(_expression);
		ConvertTextureSampleExpression(_collection, name.c_str(), con);
	}
/*	TODO pick up UV tilling and mirror setting
	else if (_expression->IsA(UMaterialExpressionTextureCoordinate::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionTextureCoordinate*>(_expression);
		ConvertTextureSampleExpression(_collection, name.c_str(), con);
	}
	*/
/* TODO need UE4 MinimalAPI support for UMaterialExpressionDesaturation
	else if (_expression->IsA(UMaterialExpressionDesaturation::StaticClass()))
	{
		auto con = static_cast<UMaterialExpressionDesaturation*>(_expression);
		ConvertFExpressionInput(_collection, &con->Input, "Input");
		ConvertFExpressionInput(_collection, &con->Fraction, "Fraction");
		std::string const valueName = (_id + "LuminanceFactors");
		auto valuePtr = UE4InterchangeMaterialValue::New(_collection,
										valueName.c_str(),
										con->LuminanceFactors);
		_collection.valueStorage[valueName.c_str()] = valuePtr;
		muxes.emplace_back(_collection.FindMux(valuePtr->GetId()));
		inputNames.emplace_back("LuminanceFactors");
	}*/
	else
	{
		// do the inputs 
		for (int i = 0; i < _expression->GetInputs().Num(); ++i)
		{
			FName fname = FRPRCpMaterial::GetMaterialExpressionInputName(expression, i);
			FExpressionInput* input = _expression->GetInput(i);
			if (input == nullptr)
				continue;

			ConvertFExpressionInput(_collection, input, name.c_str(), fname);
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
							FMaterialAttributesInput const & _ue4MatAttr)
{
	auto it = _collection.nodeStorage.find(_id);
	if (it == _collection.nodeStorage.end())
	{

		auto node = IMaterialNodePtr(new UE4InterchangePBRNode(_collection, _id, _ue4MatAttr));
		_collection.nodeStorage[_id] = node;
		return node;
	} else
	{
		return it->second;
	}
}
IMaterialNodePtr
UE4InterchangePBRNode::New(UEInterchangeCollection & _collection,
	std::string const & _id,
	UMaterial const * _ue4Mat)
{
	auto it = _collection.nodeStorage.find(_id);
	if (it == _collection.nodeStorage.end())
	{

		auto node = IMaterialNodePtr(new UE4InterchangePBRNode(_collection, _id, _ue4Mat));
		_collection.nodeStorage[_id] = node;
		return node;
	}
	else
	{
		return it->second;
	}
}
#define PBR_DEFAULT_BLACK FColor(0,0,0,255)
#define PBR_DEFAULT_GREY FColor(128,128,128,255)

#define HOOKUP_PBR_EXPRESSION_NO_CONSTANT(_base, _name, _index) \
if (_base->_name.Expression != nullptr) \
{ \
	auto mux = UE4InterchangeMaterialNode::New(_collection, \
		(id + #_name + TCHAR_TO_ANSI(*_base->_name.Expression->GetName())).c_str(), \
		_base->_name.Expression); \
	muxes[_index] = mux; \
}

#define HOOKUP_PBR_EXPRESSION(_base, _name, _index, _type, _default) \
if ((_base->_name.UseConstant) || (_base->_name.Expression == nullptr)) \
{ \
	_type v = _default; \
	if (_base->_name.UseConstant) { v = _base->_name.Constant; } \
	auto val = UE4InterchangeMaterialValue::New(_collection, \
		(id + #_name).c_str(), v); \
	auto muxPtr = _collection.FindMux((id + #_name).c_str()); \
	assert(muxPtr); \
	muxes[_index] = muxPtr; \
} \
else HOOKUP_PBR_EXPRESSION_NO_CONSTANT(_base, _name, _index)

UE4InterchangePBRNode::UE4InterchangePBRNode(	UEInterchangeCollection & _collection, 
												std::string const & _id,
												UMaterial const * _ue4Mat)
{
	id = _id;

	HOOKUP_PBR_EXPRESSION(_ue4Mat, BaseColor, 0, FColor, PBR_DEFAULT_GREY);
	HOOKUP_PBR_EXPRESSION(_ue4Mat, Roughness, 1, float, 0.0f);
	HOOKUP_PBR_EXPRESSION(_ue4Mat, Metallic, 2, float, 0.5f);
	HOOKUP_PBR_EXPRESSION(_ue4Mat, Specular, 3, float, 0.0f);
	HOOKUP_PBR_EXPRESSION_NO_CONSTANT(_ue4Mat, Normal, 4);

//	HOOKUP_PBR_EXPRESSION(EmissiveColor, 5, FColor, PBR_DEFAULT_BLACK);
	if (_ue4Mat->Opacity.Expression != NULL)
	{
		HOOKUP_PBR_EXPRESSION(_ue4Mat, Opacity, 6, float, 1.0f);
	} else
	{
		HOOKUP_PBR_EXPRESSION(_ue4Mat, OpacityMask, 6, float, 1.0f);
	}
// refraction
// clear coat here
	// clear coat roughness
		
}
UE4InterchangePBRNode::UE4InterchangePBRNode(UEInterchangeCollection & _collection,
	std::string const & _id,
	FMaterialAttributesInput const & _ue4MatAttr)
{
	id = _id;

	FMaterialAttributesInput const * _ue4Mat = &_ue4MatAttr;
	auto ma = static_cast<UMaterialExpressionMakeMaterialAttributes const*>(_ue4MatAttr.Expression);
	if (ma != nullptr)
	{
		HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, BaseColor, 0);
		HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, Roughness, 1);
		HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, Metallic, 2);
		HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, Specular, 3);
		HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, Normal, 4);
		//	HOOKUP_PBR_EXPRESSION(EmissiveColor, 5, FColor, PBR_DEFAULT_BLACK);
		if( ma->Opacity.Expression != NULL)
		{
			HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, Opacity, 6);
		}
		else
		{
			HOOKUP_PBR_EXPRESSION_NO_CONSTANT(ma, OpacityMask, 6);
		}
		// refraction
		// clear coat here
		// clear coat roughness
	}


}

#undef HOOKUP_PBR_EXPRESSION_NO_CONSTANT
#undef HOOKUP_PBR_EXPRESSION
#undef PBR_DEFAULT_YELLOW
#undef PBR_DEFAULT_BLACK


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

	for (int i = 0 - 1; i < 3; i++)
	{
		static float const a = 0.055f;
		static float const b = 0.04045f;
		if (values[i] <= b) { values[i] *= (1.0f / 12.92f); }
		else
		{
			values[i] = (values[i] + a) / (1.f + a);
			values[i] = powf(values[i], 2.4f);
		}
	}


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

	std::string matName = TCHAR_TO_ANSI(*ue4MatInterface->GetName());

	// Interchange treats the destination PBR object as a node
	if(ue4Mat->MaterialAttributes.Expression != NULL)
	{
		rootNode = UE4InterchangePBRNode::New(collection, matName + "PBRMaterial", ue4Mat->MaterialAttributes);
	}
	{
		rootNode = UE4InterchangePBRNode::New(collection, matName + "PBRMaterial", ue4Mat);
	}

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
	name = TCHAR_TO_ANSI(*ue4MatInterface->GetName());
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
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
		FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
		if (plats != nullptr)
		{
			FTexturePlatformData * plat0 = plats[0];
			auto mip = plat0->Mips[0];
			return mip.SizeX;
		}
	}
	return (source.GetSizeX() > DIMENSION_LIMIT) ? (source.GetSizeX()/2) : source.GetSizeX();
}

size_t UE4InterchangeImage::GetHeight() const
{
	FTextureSource & source = ueTexture->Source;
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
		FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
		if (plats != nullptr)
		{
			FTexturePlatformData * plat0 = plats[0];
			auto mip = plat0->Mips[0];
			return mip.SizeY;
		}
	}
	return (source.GetSizeY() > DIMENSION_LIMIT) ? (source.GetSizeY()/2) : source.GetSizeY();
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
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
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
	}

	ETextureSourceFormat sformat = source.GetFormat();
	size_t compCount = CrackNumofComponents(sformat);
	return compCount;
}
rpri::generic::IImage::ComponentFormat UE4InterchangeImage::GetComponentFormat(size_t _index) const
{
	FTextureSource & source = ueTexture->Source;
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
		FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
		if (plats != nullptr)
		{
			FTexturePlatformData * plat0 = plats[0];
			auto mip = plat0->Mips[0];
			EPixelFormat format = plat0->PixelFormat;
			ComponentFormat cformat = CrackComponentFormat(format);
			if (cformat != ComponentFormat::Unknown) return cformat;
		}
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
	
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
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
	}

	ETextureSourceFormat sformat = source.GetFormat();
	size_t bitSize = CrackPixelSizeInBits(sformat);
	return bitSize;
}

size_t UE4InterchangeImage::GetRowStrideInBits() const
{	
	FTextureSource & source = ueTexture->Source;
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
		FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
		if (plats != nullptr)
		{
			FTexturePlatformData * plat0 = plats[0];
			FTexture2DMipMap const mip = plat0->Mips[0];
			return mip.BulkData.GetElementSize() * mip.SizeX * 8;
		}
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

float const * UE4InterchangeImage::GetRawFloatData() const
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

bool UE4InterchangeImage::GetBulk2DAsUint8s(uint8_t * _dest) const
{
	FTextureSource & source = ueTexture->Source;

	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
		FTexturePlatformData ** plats = ueTexture->GetRunningPlatformData();
		if (plats != nullptr)
		{
			bool okay = true; // assume this will work
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
				if (GetColourSpace() == ColourSpace::Linear)
				{
					ByteToByteCopy<4>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				} else
				{
					sRGBByteToLinearByteCopy<4>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				}
				break;
			case PF_A8R8G8B8:
				if (GetColourSpace() == ColourSpace::Linear)
				{
					ByteToByteCopy<4, 1, 2, 3, 0>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				}
				else
				{
					sRGBByteToLinearByteCopy<4, 1, 2, 3, 0>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				}
				break;
			case PF_B8G8R8A8:
				if (GetColourSpace() == ColourSpace::Linear)
				{
					ByteToByteCopy<4, 2, 1, 0, 3>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				}
				else
				{
					sRGBByteToLinearByteCopy<4, 2, 1, 0, 3>(mip.SizeX, mip.SizeY, (uint8_t*)rawData, _dest);
				}
				break;
			default: 
				okay = false; // platform data is a bust
			}
			mip.BulkData.Unlock();
			if(okay == true)
			{
				// if we okay we are done
				return true;
			}
		}
	}

	// lets try source data
	void const * rawData = source.LockMip(0);
	ETextureSourceFormat sformat = source.GetFormat();
	uint8_t const * src = reinterpret_cast<uint8_t const*>(rawData);


	switch (sformat)
	{
	case TSF_G8:
		if(source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
		{
			ByteToByteCopy<1,2,2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
		}
		else if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() <= DIMENSION_LIMIT)
		{
			ByteToByteCopy<1, 2, 1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
		}
		else if (source.GetSizeX() <= DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
		{
			ByteToByteCopy<1, 1, 2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
		}
		else
		{
			ByteToByteCopy<1,1,1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
		}
		source.UnlockMip(0);
		return true;
	case TSF_BGRA8:
		if (GetColourSpace() == ColourSpace::Linear)
		{
			if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				ByteToByteCopy<4, 2, 2, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() <= DIMENSION_LIMIT)
			{
				ByteToByteCopy<4, 2, 1, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() <= DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				ByteToByteCopy<4, 1, 2, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else
			{
				ByteToByteCopy<4, 1, 1, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
		} else
		{
			if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 2, 2, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() <= DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 2, 1, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() <= DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 1, 2, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else
			{
				sRGBByteToLinearByteCopy<4, 1, 1, 2, 1, 0, 3>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
		}
		source.UnlockMip(0);
		return true;
	case TSF_RGBA8:
		if (GetColourSpace() == ColourSpace::Linear)
		{
			if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				ByteToByteCopy<4,2,2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() <= DIMENSION_LIMIT)
			{
				ByteToByteCopy<4, 2, 1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			} else if(source.GetSizeX() <= DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				ByteToByteCopy<4, 1, 2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			} else
			{
				ByteToByteCopy<4,1,1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
		} else
		{
			if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 2, 2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() > DIMENSION_LIMIT && source.GetSizeY() <= DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 2, 1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else if (source.GetSizeX() <= DIMENSION_LIMIT && source.GetSizeY() > DIMENSION_LIMIT)
			{
				sRGBByteToLinearByteCopy<4, 1, 2>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
			else
			{
				sRGBByteToLinearByteCopy<4,1,1>(source.GetSizeX(), source.GetSizeY(), src, _dest);
			}
		}
		source.UnlockMip(0);
		return true;
	default:
		source.UnlockMip(0);
		break;
	}

	return false; // bulk can't handle it
}

float UE4InterchangeImage::GetComponent2DAsFloat(size_t _x, size_t _y, size_t _comp) const
{
	FTextureSource & source = ueTexture->Source;
	bool tryPlatformData = TRY_PLATFORM_DATA_FOR_IMAGES;
	if (tryPlatformData)
	{
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
				mip.BulkData.Unlock();
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

		source.UnlockMip(0);
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

		source.UnlockMip(0);
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

		source.UnlockMip(0);
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

		source.UnlockMip(0);
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
	source.UnlockMip(0);

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

#endif // WITH_EDITOR
