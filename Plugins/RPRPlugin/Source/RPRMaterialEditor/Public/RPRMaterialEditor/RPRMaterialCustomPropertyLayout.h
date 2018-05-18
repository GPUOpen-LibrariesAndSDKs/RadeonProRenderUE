#pragma once
#include "IDetailCustomization.h"
#include "TriPlanarSettings.h"
#include "NotifyHook.h"
#include "PropertyHandle.h"
#include "DetailCategoryBuilder.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditor/DEditorParameterValue.h"

class FRPRMaterialCustomPropertyLayout : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

    void LoadTriPlanarSettings();
    void HideDefaultUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder);
    void AddUberMaterialParameters(IDetailLayoutBuilder& DetailBuilder);
    void AddTriPlanarParameters(IDetailLayoutBuilder& DetailBuilder);
    TSharedRef<IPropertyHandle> GetUberMaterialParametersPropertyHandle(IDetailLayoutBuilder& DetailBuilder);
    IDetailCategoryBuilder& GetMaterialCategory(IDetailLayoutBuilder& DetailBuilder);
    void OnTriPlanarValueChanged(UProperty* Property);

    bool IsTriPlanarUsed() const;

    void UpdateParam_UseTriPlanar(UDEditorParameterValue* ParameterValue);
    void UpdateParam_Scale(UDEditorParameterValue* ParameterValue);
    void UpdateParam_Angle(UDEditorParameterValue* ParameterValue);

private:

    TArray<TWeakObjectPtr<UObject>> MaterialsBeingEdited;
    FTriPlanarSettings TriPlanarSettings;

};