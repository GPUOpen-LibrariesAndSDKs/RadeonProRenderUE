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
#include "RPRToolsModule.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Typedefs/RPRTypedefs.h"

namespace RPRX
{
	RPRTOOLS_API RPR::FResult ShapeAttachMaterial(RPRX::FContext Context, RPR::FShape Shape, RPRX::FMaterial Material);
	RPRTOOLS_API RPR::FResult ShapeDetachMaterial(RPRX::FContext Context, RPR::FShape Shape, RPRX::FMaterial Material);
	RPRTOOLS_API RPR::FResult MaterialCommit(RPRX::FContext Context, RPRX::FMaterial Material);

	namespace Context
	{
		RPRTOOLS_API RPR::FResult Create(RPR::FMaterialSystem MaterialSystem, RPRX::FContextCreationFlags Flags, RPRX::FContext& OutContext);
		RPRTOOLS_API RPR::FResult Delete(RPRX::FContext Context);
	}
}
