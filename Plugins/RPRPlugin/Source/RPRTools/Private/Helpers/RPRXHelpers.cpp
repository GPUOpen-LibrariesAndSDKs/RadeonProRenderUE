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
#include "Helpers/RPRXHelpers.h"
#include "RprSupport.h"

namespace RPRX
{
	RPR::FResult ShapeAttachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeAttachMaterial(Context, Shape, Material));
	}

	RPR::FResult ShapeDetachMaterial(FContext Context, RPR::FShape Shape, FMaterial Material)
	{
		return (rprxShapeDetachMaterial(Context, Shape, Material));
	}

	RPR::FResult MaterialCommit(FContext Context, FMaterial Material)
	{
		return (rprxMaterialCommit(Context, Material));
	}

	RPR::FResult ShapeGetMaterial(FContext Context, RPR::FShape Shape, FMaterial& OutMaterialX)
	{
		return (rprxShapeGetMaterial(Context, Shape, &OutMaterialX));
	}

	namespace Context
	{

		RPR::FResult Create(RPR::FMaterialSystem MaterialSystem, RPRX::FContextCreationFlags Flags, RPRX::FContext& OutContext)
		{
			return (rprxCreateContext(MaterialSystem, Flags, &OutContext));
		}

		RPR::FResult Delete(FContext context)
		{
			return (rprxDeleteContext(context));
		}
	}

}
