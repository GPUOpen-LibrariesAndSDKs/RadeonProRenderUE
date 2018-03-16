#include "SlateRectHelper.h"

void FSlateRectHelper::SetWidth(FSlateRect& Rect, float Width)
{
	Rect.Right = Rect.Left + Width;
}

void FSlateRectHelper::SetHeight(FSlateRect& Rect, float Height)
{
	Rect.Bottom = Rect.Top + Height;
}

float FSlateRectHelper::GetWidth(const FSlateRect& Rect)
{
	return (Rect.Right - Rect.Left);
}

float FSlateRectHelper::GetHeight(const FSlateRect& Rect)
{
	return (Rect.Bottom - Rect.Top);
}

FSlateRect FSlateRectHelper::Create(float X, float Y, float Width, float Height)
{
	return (FSlateRect(X, Y, X + Width, Y + Height));
}
