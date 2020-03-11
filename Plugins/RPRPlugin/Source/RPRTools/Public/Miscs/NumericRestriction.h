/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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
