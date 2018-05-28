#pragma once
#include "Materials/MaterialInterface.h"
#include "TriPlanarSettings.h"

class RPREDITORTOOLS_API FTriPlanarSettingsEditorLoader
{
public:

    FTriPlanarSettingsEditorLoader(FTriPlanarSettings* InSettings);

    void	LoadFromMaterial(class UMaterialInterface* MaterialInterface);

private:

    void	LoadUseTriPlanar(class UDEditorParameterValue* ParameterValue);
    void	LoadScale(class UDEditorParameterValue* ParameterValue);
    void	LoadAngle(class UDEditorParameterValue* ParameterValue);

private:

    FTriPlanarSettings* Settings;

};