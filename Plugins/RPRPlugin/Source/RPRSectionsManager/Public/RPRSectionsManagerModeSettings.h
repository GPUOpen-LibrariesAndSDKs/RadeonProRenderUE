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
#pragma once
#include "RPRSectionsManagerModeSettings.generated.h"

UCLASS()
class RPRSECTIONSMANAGER_API URPRSectionsManagerModeSettings : public UObject
{
	GENERATED_BODY()

public:

	URPRSectionsManagerModeSettings();

public:

	UPROPERTY(EditAnywhere, meta = (ToolTip="Defines the radius of the brush. You can also use 'Ctrl + Mouse Wheel'.", UIMin="0.001"), Category = Brush)
	float	BrushSize;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "If true, only the faces visible by the camera will be selected"), Category = Brush)
	bool	bOnlyFrontFacing;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Defines how much the size of the brush is changed when using 'Ctrl + Mouse Wheel'."), AdvancedDisplay, Category = Brush)
	float	IncrementalBrushSizeStep;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Brush)
	bool	bShowOnlySelection;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "It can take times to calculate the section selection. To not slow down your computer, you can use the async selection. However, it can takes few seconds before the operation ends."), AdvancedDisplay, Category = Brush)
	bool	bAsynchronousSelection;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Defines the quantity of work to do during the selection. Higher the number, faster the selection but can slow computer."), AdvancedDisplay, Category = Brush)
	int32	BlockOfWorkPerFrameForSelection;

};
