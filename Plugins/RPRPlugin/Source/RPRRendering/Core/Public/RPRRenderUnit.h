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
#include "Templates/SharedPointer.h"

// Represents a system that can be plugged into the RenderSystem
// to manage what will be rendered in the RPR viewport
// See it as a system that will describe what should be displayed in the RPR viewport
class FRPRRenderUnit
{
public:

	FRPRRenderUnit();

	void			Initialize();
	void			Shutdown();


	virtual FName	GetName() = 0;

protected:

	virtual void	OnInitialize() = 0;
	virtual void	OnShutdown() = 0;

private:

	void			DeleteScene();

private:

	RPR::FScene		RPRScene;
	bool			bIsInitialized;

};

typedef TSharedPtr<FRPRRenderUnit> FRPRRenderUnitPtr;
