#pragma once

#include "IUVCubeLayout.h"

/*
 * Arrange UVs to get this layout :
 * 
 * V
 * |        [ +Y ]
 * |  [ -X ][ +Z ][ +X ][ -Z ]
 * |        [ -Y ]
 * |
 * ________ U
 *
 */
class FUVCubeLayout_CubemapRight : public IUVCubeLayout
{

public:

	virtual void ArrangeUVs(const TArray<class FCubeProjectionFace>& CubeProjectionFaces, TArray<struct FVector2D>& InOutUVs) override;

private:

	FVector2D	GetCell(int32 ColumnIdx, int32 RowIdx);

private:

	static const FVector2D FaceUVBoxSize;

};