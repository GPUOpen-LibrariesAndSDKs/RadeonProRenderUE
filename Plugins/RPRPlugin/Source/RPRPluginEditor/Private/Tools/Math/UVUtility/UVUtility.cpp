#include "UVUtility.h"
#include "Vector2D.h"

const FVector2D FUVUtility::UVsRange(0, 1);

void FUVUtility::ShrinkUVsToBounds(TArray<FVector2D>& UVs)
{
	FVector2D min, max;
	GetUVsBounds(UVs, min, max);
	FVector2D scale = max - min;

	for (int32 i = 0 ; i < UVs.Num(); ++i)
	{
		UVs[i].X = (UVs[i].X - min.X) / scale.X;
		UVs[i].Y = (UVs[i].Y - min.Y) / scale.Y;
	}
}

void FUVUtility::GetUVsBounds(const TArray<FVector2D>& UVs, FVector2D& OutMin, FVector2D& OutMax)
{
	checkf(UVs.Num() > 0, TEXT("The UVs array must not be empty!"));

	OutMin = UVs[0];
	OutMax = UVs[0];

	for (int32 i = 1; i < UVs.Num(); ++i)
	{
		if (UVs[i].X < OutMin.X) OutMin.X = UVs[i].X;
		else if (UVs[i].X > OutMax.X) OutMax.X = UVs[i].X;

		if (UVs[i].Y < OutMin.Y) OutMin.Y = UVs[i].Y;
		else if (UVs[i].Y > OutMax.Y) OutMax.Y = UVs[i].Y;
	}
}

