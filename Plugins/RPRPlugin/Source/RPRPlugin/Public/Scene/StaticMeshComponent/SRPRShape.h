#pragma once

#include "RprSupport.h"
#include "SRPRCachedMesh.h"

struct	SRPRShape
{
	rpr_shape			m_RprShape;
	rpr_material_node	m_RprMaterial;
	rprx_material       m_RprxMaterial;
	int32				m_UEMaterialIndex;

	SRPRShape(const SRPRCachedMesh &cached)
		: m_RprShape(cached.m_RprShape)
		, m_UEMaterialIndex(cached.m_UEMaterialIndex)
		, m_RprMaterial(NULL)
		, m_RprxMaterial(NULL) { }
};