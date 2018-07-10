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
#include "RPRStaticMeshEditor/Modes/RPRStaticMeshEditorModesCommands.h"
#include "EditorStyleSet.h"
#include "RPREditorStyle.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditorModesCommands"

FRPRStaticMeshEditorModesCommands::FRPRStaticMeshEditorModesCommands()
	: TCommands<FRPRStaticMeshEditorModesCommands>(
		TEXT("RPRStaticMeshEditor"),
		NSLOCTEXT("Contexts", "RPRStaticMeshEditor", "RPRStaticMeshEditor"),
		NAME_None,
		FRPREditorStyle::GetStyleSetName())
{}

void FRPRStaticMeshEditorModesCommands::RegisterCommands()
{
	UI_COMMAND(Mode_UVModifier, "UV Modifier", "Allows to modify UV.", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(Mode_SectionsManagement, "Section Management", "Allows to edit sections of the meshes.", EUserInterfaceActionType::RadioButton, FInputChord());
}

TArray<TSharedPtr<FUICommandInfo>> FRPRStaticMeshEditorModesCommands::GetCommandInfos() const
{
	TArray<TSharedPtr<FUICommandInfo>> commandInfos;
	{
		commandInfos.Add(Mode_UVModifier);
		commandInfos.Add(Mode_SectionsManagement);
	}
	return (commandInfos);
}

#undef LOCTEXT_NAMESPACE
