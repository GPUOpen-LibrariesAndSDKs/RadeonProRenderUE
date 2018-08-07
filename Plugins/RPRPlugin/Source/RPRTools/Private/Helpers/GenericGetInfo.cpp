#include "Helpers/GenericGetInfo.h"

RPR::FResult RPR::Generic::GetObjectName(RPR::Generic::FGetInfoFunction GetInfoFunction, void* Source, FString& OutName)
{
	TArray<uint8> buffer;
	RPR::FResult status = GetInfoToArray(GetInfoFunction, Source, RPR_OBJECT_NAME, buffer);
	if (RPR::IsResultSuccess(status))
	{
		OutName = FString((char*) buffer.GetData());
	}
	return status;
}

void RPR::Generic::TransposeTranslation(FMatrix& m)
{
	FVector temp(m.M[0][3], m.M[1][3], m.M[2][3]);
	m.M[0][3] = m.M[3][0];
	m.M[1][3] = m.M[3][1];
	m.M[2][3] = m.M[3][2];

	m.M[3][0] = temp.X;
	m.M[3][1] = temp.Y;
	m.M[3][2] = temp.Z;
}
