#pragma once

#include "IUVProjectionSettingsWidget.h"
#include "UVProjectionType.h"
#include "SharedPointer.h"
#include "IUVProjectionModule.h"

class RPRPLUGINEDITOR_API FUVProjectionFactory
{
public:

	static void		RegisterUVProjection(IUVProjectionModule* ProjectionModule);
	static void		UnregisterUVProjection(IUVProjectionModule* ProjectionModule);

	static const TArray<IUVProjectionModule*>&	GetModules();

private:

	static TArray<IUVProjectionModule*> UVProjectionModules;
};