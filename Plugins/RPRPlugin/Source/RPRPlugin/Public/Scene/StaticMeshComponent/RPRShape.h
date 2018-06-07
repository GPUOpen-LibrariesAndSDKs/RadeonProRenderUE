#pragma once

#include "RprSupport.h"
#include "RPRCachedMesh.h"

struct	FRPRShape
{
	rpr_shape			m_RprShape;
	rpr_material_node	m_RprMaterial;
	rprx_material       m_RprxMaterial;
	int32				m_UEMaterialIndex;

	FRPRShape(const FRPRCachedMesh &cached)
		: m_RprShape(cached.m_RprShape)
		, m_UEMaterialIndex(cached.m_UEMaterialIndex)
		, m_RprMaterial(NULL)
		, m_RprxMaterial(NULL) { }
};