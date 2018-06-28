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
#include "UVProjection_CubicModule.h"
#include "SUVProjectionTypeEntry.h"
#include "UVProjectionFactory.h"
#include "SUVProjectionCubic.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "UVProjection_CubicModule"

void FUVProjection_CubicModule::StartupModule()
{
	FUVProjectionFactory::RegisterUVProjection(this);
}

void FUVProjection_CubicModule::ShutdownModule()
{
	FUVProjectionFactory::UnregisterUVProjection(this);
}

IUVProjectionSettingsWidgetPtr FUVProjection_CubicModule::CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor)
{
	return SNew(SUVProjectionCubic)
		.RPRStaticMeshEditorPtr(RPRStaticMeshEditor);
}

FText FUVProjection_CubicModule::GetProjectionName() const
{
	return (LOCTEXT("ProjectionType_Cubic", "Cubic"));
}

const FSlateBrush* FUVProjection_CubicModule::GetProjectionIcon() const
{
	return (FEditorStyle::GetBrush("ClassThumbnail.Cube"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUVProjection_CubicModule, UVProjection_Cubic);
