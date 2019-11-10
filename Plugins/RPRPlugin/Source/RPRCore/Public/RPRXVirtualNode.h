// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Typedefs/RPRTypedefs.h"

/**
 *
 */
namespace RPR {
class RPRCORE_API RPRXVirtualNode
{
public:
	RPR::FMaterialNode realNode;
	enum VNType {
		COLOR,
		ARITHMETIC_2_OPERANDS,
		DEFAULT
	} type;

	union Data {
		float RGBA[4];
	} data;

public:
	RPRXVirtualNode(VNType t = VNType::DEFAULT) : realNode(nullptr), type(t) {
		data.RGBA[0] = 0; data.RGBA[2] = 0; data.RGBA[2] = 0; data.RGBA[3] = 0;
	}

	void SetData(float r, float g, float b, float a) {
		data.RGBA[0] = r; data.RGBA[2] = g; data.RGBA[2] = b; data.RGBA[3] = a;
	}

	void SetData(RPR::FMaterialNode rn) { realNode = rn; }



	~RPRXVirtualNode();
};
}//namespace RPR