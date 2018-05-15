#pragma once
#include "CoreTypes.h"

class FTrianglesSelectionFlags
{
public:
	
	FTrianglesSelectionFlags(uint32 InNumTriangles);

	void SetFlagAsUsed(uint32 TriangleIndex);
	void SetFlagAsUnused(uint32 TriangleIndex);
	bool IsTriangleUsed(uint32 TriangleIndex) const;
	void ResetAllFlags();

private:
	
	void GetFlagLocation(uint32 TriangleIndex, uint32& OutByteIndex, uint8& OutBitOffset) const;

private:

	const uint8 NumBitsPerByte = 8;

	uint32 NumTriangles;
	uint8* Flags;

};
