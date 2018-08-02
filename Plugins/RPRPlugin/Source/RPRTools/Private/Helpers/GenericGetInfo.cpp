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
