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

#if WITH_EDITOR

#include "RPREditorStyle.h"

#include "Styling/SlateStyle.h"
#include "Interfaces/IEditorStyleModule.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( style.RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FRPREditorStyle::RPRStyleInstance = NULL;

void	FRPREditorStyle::Initialize()
{
	if (!RPRStyleInstance.IsValid())
		RPRStyleInstance = Create();

	SetStyle(RPRStyleInstance.ToSharedRef());
}

void	FRPREditorStyle::Shutdown()
{
	ResetToDefault();
	check(RPRStyleInstance.IsUnique());
	RPRStyleInstance.Reset();
}

TSharedRef<FSlateStyleSet>	FRPREditorStyle::Create()
{
	IEditorStyleModule				&editorStyle = FModuleManager::LoadModuleChecked<IEditorStyleModule>(TEXT("EditorStyle"));
	TSharedRef< FSlateStyleSet >	styleRef = editorStyle.CreateEditorStyleInstance();
	FSlateStyleSet					&style = styleRef.Get();

	const FVector2D	Icon40x40(40.0f, 40.0f);
	const FVector2D	Icon20x20(20.0f, 20.0f);
	const FVector2D	Icon16x16(16.0f, 16.0f);

	style.Set("RPRViewport.Render", new IMAGE_BRUSH("Icons/Record_16x", Icon20x20));
	style.Set("RPRViewport.Render.Small", new IMAGE_BRUSH("Icons/Record_16x", Icon16x16));

	style.Set("RPRViewport.Pause", new IMAGE_BRUSH("Icons/icon_SCueEd_Stop_40x", Icon20x20));
	style.Set("RPRViewport.Pause.Small", new IMAGE_BRUSH("Icons/icon_SCueEd_Stop_40x", Icon16x16));

	style.Set("RPRViewport.SyncOff", new IMAGE_BRUSH("Icons/SourceControlOff_16x", Icon20x20));
	style.Set("RPRViewport.SyncOff.Small", new IMAGE_BRUSH("Icons/SourceControlOff_16x", Icon16x16));

	style.Set("RPRViewport.SyncOn", new IMAGE_BRUSH("Icons/SourceControlOn_16x", Icon20x20));
	style.Set("RPRViewport.SyncOn.Small", new IMAGE_BRUSH("Icons/SourceControlOn_16x", Icon16x16));

	style.Set("RPRViewport.DisplayPostEffectsOff", new IMAGE_BRUSH("ContentBrowser/sourcestoggle_16x_expanded", Icon16x16));
	style.Set("RPRViewport.DisplayPostEffectsOff.Small", new IMAGE_BRUSH("ContentBrowser/sourcestoggle_16x_expanded", Icon16x16));

	style.Set("RPRViewport.DisplayPostEffectsOn", new IMAGE_BRUSH("ContentBrowser/sourcestoggle_16x_collapsed", Icon16x16));
	style.Set("RPRViewport.DisplayPostEffectsOn.Small", new IMAGE_BRUSH("ContentBrowser/sourcestoggle_16x_collapsed", Icon16x16));

	style.Set("RPRViewport.CameraOrbitOff", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_CameraLocked_40x", Icon16x16));
	style.Set("RPRViewport.CameraOrbitOff.Small", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_CameraLocked_40x", Icon16x16));

	style.Set("RPRViewport.CameraOrbitOn", new IMAGE_BRUSH("Icons/icon_Camera_Reset_40px", Icon16x16));
	style.Set("RPRViewport.CameraOrbitOn.Small", new IMAGE_BRUSH("Icons/icon_Camera_Reset_40px", Icon16x16));

	style.Set("RPRViewport.Rebuild", new IMAGE_BRUSH("Icons/icon_Cascade_RestartSim_40x", Icon16x16));
	style.Set("RPRViewport.Rebuild.Small", new IMAGE_BRUSH("Icons/icon_Cascade_RestartSim_40x", Icon16x16));

	style.Set("RPRViewport.Save", new IMAGE_BRUSH("Icons/icon_file_save_40x", Icon20x20));
	style.Set("RPRViewport.Save.Small", new IMAGE_BRUSH("Icons/icon_file_save_16x", Icon16x16));

	style.Set("RPRViewport.Trace", new IMAGE_BRUSH("Icons/icon_tab_Stats_40x", Icon20x20));
	style.Set("RPRViewport.Trace.Small", new IMAGE_BRUSH("Icons/icon_tab_Stats_16x", Icon16x16));

#if WITH_EDITOR
	style.Set("RPRStaticMeshEditor.Mode_UVModifier", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_UVOverlay_40x", Icon40x40));
	style.Set("RPRStaticMeshEditor.Mode_UVModifier.Small", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_UVOverlay_40x", Icon20x20));

	style.Set("RPRStaticMeshEditor.Mode_SectionsManagement", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_Wireframe_40x", Icon40x40));
	style.Set("RPRStaticMeshEditor.Mode_SectionsManagement.Small", new IMAGE_BRUSH("Icons/icon_StaticMeshEd_Wireframe_40x", Icon20x20));
#endif

	return styleRef;
}

#endif
