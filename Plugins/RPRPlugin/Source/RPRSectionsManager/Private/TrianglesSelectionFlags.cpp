#include "TrianglesSelectionFlags.h"
#include "UnrealMemory.h"

FTrianglesSelectionFlags::FTrianglesSelectionFlags(uint32 InNumTriangles)
	: NumTriangles(InNumTriangles)
{
	const uint32 sizeFlags = NumTriangles / NumBitsPerByte;
	Flags = new uint8[sizeFlags];
	ResetAllFlags();
}

void FTrianglesSelectionFlags::SetFlagAsUsed(uint32 TriangleIndex)
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	Flags[flaggedByte] |= 1 << bitOffset;
}

bool FTrianglesSelectionFlags::IsTriangleUsed(uint32 TriangleIndex) const
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	return ((Flags[flaggedByte] >> bitOffset) & 1);
}

void FTrianglesSelectionFlags::ResetAllFlags()
{
	FMemory::Memzero(Flags, NumTriangles / NumBitsPerByte);
}

void FTrianglesSelectionFlags::SetFlagAsUnused(uint32 TriangleIndex)
{
	uint32 flaggedByte;
	uint8 bitOffset;
	GetFlagLocation(TriangleIndex, flaggedByte, bitOffset);

	Flags[flaggedByte] &= ~(1 << bitOffset);
}

void FTrianglesSelectionFlags::GetFlagLocation(uint32 TriangleIndex, uint32& OutByteIndex, uint8& OutBitOffset) const
{
	OutByteIndex = TriangleIndex / NumBitsPerByte;
	OutBitOffset = TriangleIndex % NumBitsPerByte;
}
