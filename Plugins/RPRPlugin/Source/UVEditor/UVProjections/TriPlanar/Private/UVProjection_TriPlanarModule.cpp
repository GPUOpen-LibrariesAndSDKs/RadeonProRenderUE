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
#include "UVProjection_TriPlanarModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionTriPlanar.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "UVProjection_TriPlanarModule"

void FUVProjection_TriPlanarModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_TriPlanarModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_TriPlanarModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionTriPlanar)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_TriPlanarModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_TriPlanar", "TriPlanar"));
}

const FSlateBrush* FUVProjection_TriPlanarModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Plane"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_TriPlanarModule, UVProjection_TriPlanar);
