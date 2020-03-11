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

#include "CoreTypes.h"
#include "RadeonProRender.h"

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
