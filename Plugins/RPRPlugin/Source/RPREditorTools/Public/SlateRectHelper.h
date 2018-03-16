#pragma once
#include "SlateRect.h"

class RPREDITORTOOLS_API FSlateRectHelper
{
public:
	static void		SetWidth(FSlateRect& Rect, float Width);
	static void		SetHeight(FSlateRect& Rect, float Height);

	static float	GetWidth(const FSlateRect& Rect);
	static float	GetHeight(const FSlateRect& Rect);

	static FSlateRect	Create(float X, float Y, float Width, float Height);
};
