/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "RadeonProRender.h"
#include "Scene/StaticMeshComponent/RPRCachedMesh.h"
#include "Material/RPRXMaterial.h"

struct	FRPRShape
{
	rpr_shape				m_RprShape;
	rpr_material_node		m_RprMaterial;
	RPR::FRPRXMaterialPtr	m_RprxMaterial;
	RPR::FRPRXMaterialNodePtr	m_RprxNodeMaterial;
	int32					m_UEMaterialIndex;
	uint32					m_InstanceIndex;

	FRPRShape(const FRPRCachedMesh &cached, uint32 iInstance)
		: m_RprShape(cached.m_RprShape)
		, m_UEMaterialIndex(cached.m_UEMaterialIndex)
		, m_RprMaterial(NULL)
		, m_InstanceIndex(iInstance) { }
};
