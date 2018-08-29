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
#pragma once

#include "RadeonProRenderInterchange.h"
#include "RprSupport.h"
#include "Templates/SharedPointer.h"

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

	typedef rpr_framebuffer		FFrameBuffer;

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

	typedef void*						FMaterialRawDatas;
	typedef rpr_material_node			FMaterialNode;

	typedef rpr_material_system_type	FMaterialSystemType;

	typedef TSharedPtr<FImage>		FImagePtr;
}
