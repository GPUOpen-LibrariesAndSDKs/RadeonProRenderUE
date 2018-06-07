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
