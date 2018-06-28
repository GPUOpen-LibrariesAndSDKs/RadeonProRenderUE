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
