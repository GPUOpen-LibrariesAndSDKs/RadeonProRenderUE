#include "RPRImageManagerModule.h"
#include "ISettingsModule.h"
#include "ModuleManager.h"

DEFINE_LOG_CATEGORY(LogRPRImageManager)

void FRPRImageManagerModule::StartupModule() {}
void FRPRImageManagerModule::ShutdownModule() {}

IMPLEMENT_MODULE(FRPRImageManagerModule, LogRPRImageManager)
