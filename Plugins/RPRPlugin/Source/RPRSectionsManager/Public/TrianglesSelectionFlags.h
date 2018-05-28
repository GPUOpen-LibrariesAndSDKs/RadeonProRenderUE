#pragma once
#include "CoreTypes.h"
#include "Array.h"

class FTrianglesSelectionFlags
{
public:
	
	FTrianglesSelectionFlags(uint32 InNumTriangles);

	void SetFlagAsUsed(uint32 TriangleIndex);
	void SetFlagAsUsed(const TArray<uint32>& TriangleIndexes);
	void SetFlagAsUnused(uint32 TriangleIndex);
	void SetFlagAsUnused(const TArray<uint32>& TriangleIndexes);
	bool IsTriangleUsed(uint32 TriangleIndex) const;
	void ResetAllFlags();
	bool HasAtTrianglesSelected() const;
	int32 Num() const;

	TArray<uint32> GetSelectedTrianglesAsArray() const;

private:
	
	void GetFlagLocation(uint32 TriangleIndex, uint32& OutByteIndex, uint8& OutBitOffset) const;

private:

	const uint8 NumBitsPerByte = 8;

	uint32 NumTriangles;
	uint8* Flags;

};
