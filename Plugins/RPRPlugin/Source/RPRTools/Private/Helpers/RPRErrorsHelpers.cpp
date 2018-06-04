#include "RPRErrorsHelpers.h"
#include "LogMacros.h"
#include "RPRHelpers.h"
#include "RadeonProRender.h"

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
				char* buffer = new char[sizeParamA];
				result = rprContextGetInfo(Context, RPR_CONTEXT_LAST_ERROR_MESSAGE, sizeParamA, buffer, 0);
				if (RPR::IsResultSuccess(result))
				{
					errorMessage = ANSI_TO_TCHAR(buffer);
				}
				delete buffer;
				buffer = nullptr;
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