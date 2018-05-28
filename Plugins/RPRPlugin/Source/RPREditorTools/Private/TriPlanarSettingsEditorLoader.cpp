#include "TriPlanarSettingsEditorLoader.h"
#include "Materials/MaterialInterface.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditHelper.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"

DECLARE_DELEGATE_OneParam(FSettingRouterDelegate, UDEditorParameterValue*);

FTriPlanarSettingsEditorLoader::FTriPlanarSettingsEditorLoader(FTriPlanarSettings* InSettings)
    : Settings(InSettings)
{}

void FTriPlanarSettingsEditorLoader::LoadFromMaterial(UMaterialInterface* MaterialInterface)
{
    TMap<FName, FSettingRouterDelegate> settingsRouter;
    {
        settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar, FSettingRouterDelegate::CreateRaw(this, &FTriPlanarSettingsEditorLoader::LoadUseTriPlanar));
        settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale, FSettingRouterDelegate::CreateRaw(this, &FTriPlanarSettingsEditorLoader::LoadScale));
        settingsRouter.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle, FSettingRouterDelegate::CreateRaw(this, &FTriPlanarSettingsEditorLoader::LoadAngle));
    }

    FMaterialEditHelper::BindRouterAndExecute(MaterialInterface, settingsRouter);
}

void FTriPlanarSettingsEditorLoader::LoadUseTriPlanar(UDEditorParameterValue* ParameterValue)
{
    UDEditorStaticSwitchParameterValue* staticSwitch = Cast<UDEditorStaticSwitchParameterValue>(ParameterValue);
    if (staticSwitch)
    {
        Settings->bUseTriPlanar = (staticSwitch->ParameterValue != 0);
    }
}

void FTriPlanarSettingsEditorLoader::LoadScale(UDEditorParameterValue* ParameterValue)
{
    UDEditorScalarParameterValue* scalarParam = Cast<UDEditorScalarParameterValue>(ParameterValue);
    if (scalarParam)
    {
        Settings->Scale = scalarParam->ParameterValue;
    }
}

void FTriPlanarSettingsEditorLoader::LoadAngle(UDEditorParameterValue* ParameterValue)
{
    UDEditorScalarParameterValue* scalarParam = Cast<UDEditorScalarParameterValue>(ParameterValue);
    if (scalarParam)
    {
        Settings->Angle = scalarParam->ParameterValue;
    }
}
