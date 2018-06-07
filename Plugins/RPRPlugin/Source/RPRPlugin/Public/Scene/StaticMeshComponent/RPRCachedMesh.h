#pragma once

#include "CoreTypes.h"
#include "RprSupport.h"

struct FRPRCachedMesh
{
	rpr_shape	m_RprShape;
	int32		m_UEMaterialIndex;

	FRPRCachedMesh(rpr_shape shape, int32 materialIndex)
		: m_RprShape(shape)
		, m_UEMaterialIndex(materialIndex) { }

	FRPRCachedMesh(int32 materialIndex)
		: m_UEMaterialIndex(materialIndex) { }
};
