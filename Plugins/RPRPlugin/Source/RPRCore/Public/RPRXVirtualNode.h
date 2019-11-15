// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Typedefs/RPRTypedefs.h"

/**
 * In case of a color node, we don't create a real node and use a virtual node to hold the data.
 */
namespace RPR {
class RPRCORE_API RPRXVirtualNode
{
	FString name;
public:
	RPR::FMaterialNode realNode;
	enum VNType {
		COLOR,
		IMAGE,
		TEXTURE_CHANNEL,
		ARITHMETIC_2_OPERANDS,
		DEFAULT
	} type;

	union Data {
		float RGBA[4];
	} data;

public:
	RPRXVirtualNode(FString name = "", VNType t = VNType::DEFAULT) : name(name), realNode(nullptr), type(t) {
		data.RGBA[0] = 0.0f; data.RGBA[1] = 0.0f; data.RGBA[2] = 0.0f; data.RGBA[3] = 0.0f;
	}

	void SetData(float r, float g, float b, float a) {
		data.RGBA[0] = r; data.RGBA[1] = g; data.RGBA[2] = b; data.RGBA[3] = a;
	}



	~RPRXVirtualNode();
};
}//namespace RPR