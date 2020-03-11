/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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