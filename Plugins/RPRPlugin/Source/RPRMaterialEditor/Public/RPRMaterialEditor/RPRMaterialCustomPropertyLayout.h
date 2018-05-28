#pragma once
#include "IDetailCustomization.h"
#include "TriPlanarSettings.h"
#include "NotifyHook.h"
#include "PropertyHandle.h"
#include "DetailCategoryBuilder.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "RPRMaterialEditorInstanceConstant.h"

class FRPRMaterialCustomPropertyLayout : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance(URPRMaterialEditorInstanceConstant* MaterialEditorConstant);

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

    FRPRMaterialCustomPropertyLayout(URPRMaterialEditorInstanceConstant* InMaterialEditorConstant);

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
    URPRMaterialEditorInstanceConstant* MaterialEditorConstant;
    FTriPlanarSettings TriPlanarSettings;

};