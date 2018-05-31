#include "RPRMaterialMapPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "STextBlock.h"
#include "SBoxPanel.h"
#include "RPRMaterialMap.h"
#include "SButton.h"
#include "RPRImageManager.h"
#include "RPRConstAway.h"

#define LOCTEXT_NAMESPACE "RPRMaterialMapPropertiesLayout"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapPropertiesLayout));
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::GetPropertyValueRowWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			CreateCheckedTexturePropertyWidget()
		];
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetTexturePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Texture)));
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::CreateCheckedTexturePropertyWidget() const
{
	return 
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			GetTexturePropertyHandle()->CreatePropertyValueWidget()
		]
		+SHorizontalBox::Slot()
		[
			SNew(SSpacer)
			.Size(FVector2D(10, 0))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Visibility(this, &FRPRMaterialMapPropertiesLayout::GetTextureFormatFixerAreaVisibility)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Bottom)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("InfoTextureNotSupported", "The format of the texture is not supported!"))
					.ToolTipText(LOCTEXT("InfoTextureNotSupportedTooltip", "The current texture format doesn't now allow the RPR plugin to read raw datas from it. Click the button belozw to fix it."))
				]
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.AutoHeight()
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("ConvertTexture", "Fix texture format"))
					.OnClicked(this, &FRPRMaterialMapPropertiesLayout::OnFixTextureFormatButtonClicked)
				]
			]
		];
}

EVisibility FRPRMaterialMapPropertiesLayout::GetTextureFormatFixerAreaVisibility() const
{
	const UTexture2D* texture = GetTexture();
	if (texture == nullptr)
	{
		// Just ignore the fact that it is not supported
		return (EVisibility::Collapsed);
	}

	return (RPR::FImageManager::IsFormatSupported(texture->GetPixelFormat()) ? EVisibility::Collapsed : EVisibility::Visible);
}

FReply FRPRMaterialMapPropertiesLayout::OnFixTextureFormatButtonClicked()
{
	UTexture2D* texture = GetTexture();
	if (texture == nullptr)
	{
		return (FReply::Unhandled());
	}

	texture->CompressionSettings = TC_EditorIcon;
	texture->MipGenSettings = TMGS_NoMipmaps;
	texture->PostEditChange();
	texture->MarkPackageDirty();

	return (FReply::Handled());
}

UTexture2D* FRPRMaterialMapPropertiesLayout::GetTexture()
{
	const FRPRMaterialMapPropertiesLayout* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetTexture()));
}

const UTexture2D* FRPRMaterialMapPropertiesLayout::GetTexture() const
{
	TSharedPtr<IPropertyHandle> propertyHandle = GetTexturePropertyHandle();
	const UObject* textureObject;
	propertyHandle->GetValue(textureObject);
	return (Cast<const UTexture2D>(textureObject));
}

#undef LOCTEXT_NAMESPACE