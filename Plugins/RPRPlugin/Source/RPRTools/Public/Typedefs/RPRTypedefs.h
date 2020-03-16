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

#include "Templates/SharedPointer.h"
#include <RadeonProRender.h>
#include "FFrameBuffer.h"

/*
 * Use typedefs to associate native types with clearer names (and respecting the UE4 norm)
 */
namespace RPR
{
	typedef rpr_int		FResult;

	typedef rpr_int					FPluginId;
	typedef rpr_creation_flags		FCreationFlags;
	typedef rpr_context_properties	FContextProperties;

	typedef rpr_context			FContext;
	typedef rpr_material_system FMaterialSystem;
	typedef rpr_scene			FScene;

	typedef rpr_shape			FShape;
	typedef rpr_light			FLight;
	typedef rpr_image			FImage;
	typedef rpr_camera			FCamera;
	typedef rpr_image_format	FImageFormat;
	typedef rpr_image_desc		FImageDesc;

	typedef rpr_material_node_info			FMaterialNodeInfo;
	typedef rpr_material_node_input_info	FMaterialNodeInputInfo;
	typedef rpr_material_node_type			FMaterialNodeType;
	typedef rpr_material_node_input_type	FMaterialNodeInputType;

	typedef rpr_material_node_arithmetic_operation	FMaterialNodeArithmeticOperation;

	typedef void*						FMaterialRawDatas;
	typedef rpr_material_node			FMaterialNode;

	typedef rpr_material_system_type	FMaterialSystemType;

	typedef TSharedPtr<void>	FImagePtr;
	typedef TWeakPtr<void>		FImageWeakPtr;
}
