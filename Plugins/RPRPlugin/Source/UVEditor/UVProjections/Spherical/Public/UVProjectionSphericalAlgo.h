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

#include "UVProjectionMappingEditor/Algorithms/UVProjectionAlgorithmBase.h"

class FUVProjectionSphericalAlgo : public FUVProjectionAlgorithmBase
{
public:

	struct FSettings
	{
		FSettings();

		FVector	SphereCenter;
		FQuat	SphereRotation;
	};

public:

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	void	ProjectVerticesOnSphere(TArray<FVector>& VertexPositions, TArray<uint32>& WedgeIndices);
	void	ProjectVertexOnSphere(const FVector& Vertex, FVector2D& OutUV);

	void	SetSettings(const FSettings& InSettings);
	
private:

	FSettings	Settings;

	int32		CurrentMeshIndex;
	int32		CurrentStartSection;
	int32		CurrentEndSection;
};
