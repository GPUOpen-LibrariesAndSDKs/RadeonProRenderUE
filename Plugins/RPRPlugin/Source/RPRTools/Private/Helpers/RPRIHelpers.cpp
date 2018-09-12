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
#include "Helpers/RPRIHelpers.h"
#include "RadeonProRenderInterchange.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRIHelpers, Log, All)

namespace RPRI
{

	void DeleteContext(FContext Context)
	{
		rpriFreeContext(Context);
	}

	bool AllocateContext(FContext& OutContext)
	{
		return (rpriAllocateContext(&OutContext) == RPRI_TRUE);
	}

	bool SetErrorOptions(FContext Context, uint32 InfoVerbosity, bool bShouldBreakOnError, bool bShouldAbortOnError)
	{
		return (rpriErrorOptions(Context, InfoVerbosity, bShouldBreakOnError, bShouldAbortOnError) == RPRI_TRUE);
	}

	bool SetLoggers(FContext Context, FRPRILogCallback InfoCallback, FRPRILogCallback WarningCallback, FRPRILogCallback ErrorCallback)
	{
		return (rpriSetLoggers(Context, InfoCallback, WarningCallback, ErrorCallback) == RPRI_TRUE);
	}

}
