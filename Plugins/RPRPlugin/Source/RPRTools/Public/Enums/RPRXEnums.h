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

#include "RprSupport.h"
#include "Typedefs/RPRXTypedefs.h"

namespace RPRX
{
	enum EMaterialType
	{
		Uber			= RPRX_MATERIAL_UBER,
		Architectural	= RPRX_MATERIAL_ARCHITECTURAL,
		Skin			= RPRX_MATERIAL_SKIN,
		Carpaint		= RPRX_MATERIAL_CARPAINT
	};

	enum class EMaterialParameterType : RPRX::FParameterType
	{
		Float4 = RPRX_PARAMETER_TYPE_FLOAT4,
		UInt = RPRX_PARAMETER_TYPE_UINT,
		Node = RPRX_PARAMETER_TYPE_NODE
	};
}
