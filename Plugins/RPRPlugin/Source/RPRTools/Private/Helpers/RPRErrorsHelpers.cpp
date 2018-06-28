/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
