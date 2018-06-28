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

template<typename NumericType>
class FNumericRestriction
{
public:

	FNumericRestriction()
		: bHasMinimum(false)
		, bHasMaximum(false)
		, Minimum(TNumericLimits<NumericType>::Lowest())
		, Maximum(TNumericLimits<NumericType>::Max())
	{}

	void	SetRange(NumericType InMinimum, NumericType InMaximum)
	{
		SetMinimum(InMinimum);
		SetMaximum(InMaximum);
	}

	// Set the range from 0 to 1
	void	SetRange01()
	{
		SetRange(0.0f, 1.0f);
	}

	void	SetMinimum(NumericType InMinimum)
	{
		bHasMinimum = true;
		Minimum = InMinimum;
	}

	void	SetMaximum(NumericType InMaximum)
	{
		bHasMaximum = true;
		Maximum = InMaximum;
	}

	bool	IsConstrainedByMinimum() const
	{
		return (bHasMinimum);
	}

	bool	IsConstrainedByMaximum() const
	{
		return (bHasMaximum);
	}

	bool	IsConstrainedByMinimumAndMaximum() const
	{
		return (IsConstrainedByMinimum() && IsConstrainedByMaximum());
	}

	NumericType	GetMinimum() const
	{
		return (Minimum);
	}

	NumericType	GetMaximum() const
	{
		return (Maximum);
	}

private:

	bool	bHasMinimum;
	bool	bHasMaximum;
	
	NumericType	Minimum;
	NumericType	Maximum;

};
