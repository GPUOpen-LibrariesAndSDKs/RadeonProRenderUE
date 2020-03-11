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

#include "Helpers/RPRErrorsHelpers.h"
#include "Logging/LogMacros.h"
#include "Helpers/RPRHelpers.h"
#include "RadeonProRender.h"
#include <vector>

DEFINE_LOG_CATEGORY_STATIC(LogRPRErrors, Log, All)

namespace RPR
{
	namespace Error
	{

		FString GetLastError(FContext Context)
		{
			FString errorMessage;

			size_t sizeParamA = 0;
			RPR::FResult result = rprContextGetInfo(Context, RPR_CONTEXT_LAST_ERROR_MESSAGE, 0, 0, &sizeParamA);
			if (RPR::IsResultSuccess(result) && sizeParamA > 1)
			{
				std::vector<char> buffer(sizeParamA);
				result = rprContextGetInfo(Context, RPR_CONTEXT_LAST_ERROR_MESSAGE, buffer.size(), buffer.data(), 0);
				if (RPR::IsResultSuccess(result))
				{
					errorMessage = ANSI_TO_TCHAR(buffer.data());
				}
			}

			return (errorMessage);
		}

		void LogLastError(FContext Context)
		{
			FString errorMessage = GetLastError(Context);
			if (!errorMessage.IsEmpty())
			{
				UE_LOG(LogRPRErrors, Error, TEXT("%s"), *errorMessage);
			}
		}

		void LogLastErrorIfResultFailed(FContext Context, FResult Result)
		{
			if (RPR::IsResultFailed(Result))
			{
				LogLastError(Context);
			}
		}

	}
}
