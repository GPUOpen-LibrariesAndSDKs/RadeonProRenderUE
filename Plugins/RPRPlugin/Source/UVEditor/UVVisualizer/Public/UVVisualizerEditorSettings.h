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

#include "UObject/ObjectMacros.h"
#include "Engine/Texture2D.h"
#include "UVVisualizerEditorSettings.generated.h"

UENUM(DisplayName = "UV Update Method")
enum class EUVUpdateMethod : uint8
{
	Auto	UMETA(ToolTip="Automatically rebuild the static mesh after UV have been transformed."),
	Manual	UMETA(ToolTip="Wait to manually rebuild the static mesh so you can make multiple transformations on UV before rebuilding.")
};

UCLASS()
class UUVVisualizerEditorSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Background)
	UTexture2D*	BackgroundTexture;

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "1"), Category = Background)
	float		BackgroundOpacity;

	UPROPERTY(EditAnywhere, Category = Update)
	EUVUpdateMethod	UpdateMethod;


	UUVVisualizerEditorSettings()
		: BackgroundTexture(nullptr)
		, BackgroundOpacity(0.75f)
		, UpdateMethod(EUVUpdateMethod::Auto)
	{}

};
