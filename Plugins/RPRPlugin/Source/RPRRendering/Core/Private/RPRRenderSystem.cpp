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
#include "RPRRenderSystem.h"
#include "Engine/World.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRRenderSystem, Log, All)

void FRPRRenderSystem::SetRPRRenderUnit(FRPRRenderUnitPtr RenderUnit)
{
	CurrentRenderUnit = RenderUnit;
}

void FRPRRenderSystem::Initialize()
{
	Resources.Initialize();
	ConfigureRPRIContext();
}

void FRPRRenderSystem::Rebuild()
{

}

void FRPRRenderSystem::Render()
{

}

void FRPRRenderSystem::ConfigureRPRIContext()
{
	RPRI::FContext rpriContext = Resources.GetRPRIContext();

	const uint32 verbosityLevel = 5;
	const bool bShouldBreakOnError = false;
	const bool bShouldAbortOnError = false;
	if (!RPRI::SetErrorOptions(rpriContext, verbosityLevel, bShouldBreakOnError, bShouldAbortOnError))
	{
		// Log error but not fatal
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set RPRI options"));
	}

	if (!RPRI::SetLoggers(rpriContext,
		RPRILoggers::LogRPRIInfo,
		RPRILoggers::LogRPRIWarning,
		RPRILoggers::LogRPRIError))
	{
		// Log error but not fatal
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set RPRI loggers"));
	}
}

void FRPRRenderSystem::Shutdown()
{
	Resources.Shutdown();

}
