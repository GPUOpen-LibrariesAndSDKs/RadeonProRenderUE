#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogRPRI, Log, All)

namespace RPRILoggers
{

	void LogRPRIInfo(const char* log)
	{
		UE_LOG(LogRPRI, Log, ANSI_TO_TCHAR(log));
	}

	void LogRPRIWarning(const char* log)
	{
		UE_LOG(LogRPRI, Warning, ANSI_TO_TCHAR(log));
	}

	void LogRPRIError(const char* log)
	{
		UE_LOG(LogRPRI, Error, ANSI_TO_TCHAR(log));
	}

}