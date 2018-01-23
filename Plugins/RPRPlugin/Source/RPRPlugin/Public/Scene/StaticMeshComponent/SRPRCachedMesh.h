#pragma once

#include "CoreTypes.h"
#include "RprSupport.h"

struct	SRPRCachedMesh
{
	rpr_shape	m_RprShape;
	int32		m_UEMaterialIndex;

	SRPRCachedMesh(rpr_shape shape, int32 materialIndex)
		: m_RprShape(shape)
		, m_UEMaterialIndex(materialIndex) { }

	SRPRCachedMesh(int32 materialIndex)
		: m_UEMaterialIndex(materialIndex) { }
};
