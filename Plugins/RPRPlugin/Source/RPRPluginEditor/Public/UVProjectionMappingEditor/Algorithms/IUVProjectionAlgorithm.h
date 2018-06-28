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

#include "SharedPointer.h"
#include "Delegate.h"
#include "UVProjectionSettings.h"
#include "RPRMeshDataContainer.h"

class IUVProjectionAlgorithm : public TSharedFromThis<IUVProjectionAlgorithm>
{
public:

	virtual ~IUVProjectionAlgorithm() {}

	virtual void	SetMeshDatas(const FRPRMeshDataContainer& MeshDatas) = 0;
	virtual void	StartAlgorithm() = 0;
	virtual void	AbortAlgorithm() = 0;
	virtual bool	IsAlgorithimRunning() = 0;
	virtual void	Finalize() = 0;
	virtual void	SetGlobalUVProjectionSettings(FUVProjectionSettingsPtr Settings) = 0;
	
	DECLARE_EVENT_TwoParams(IUVProjectionAlgorithm, FOnAlgorithmCompleted, TSharedPtr<IUVProjectionAlgorithm> /* ProjectionInstance */ , bool /* Does algorithm succeed? */)
	virtual FOnAlgorithmCompleted&	OnAlgorithmCompleted() = 0;
	
};

typedef TSharedPtr<IUVProjectionAlgorithm> IUVProjectionAlgorithmPtr;
