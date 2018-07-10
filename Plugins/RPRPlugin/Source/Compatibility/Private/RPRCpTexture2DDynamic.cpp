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
#include "RPRCpTexture2DDynamic.h"
#include "Engine/Texture2DDynamic.h"

#if ENGINE_MINOR_VERSION < 20

UTexture2DDynamic* FRPRCpTexture2DDynamic::Create(int32 SizeX, int32 SizeY, const FCreateInfo& CreateInfo)
{
    return (UTexture2DDynamic::Create(SizeX, SizeY, CreateInfo.Format, CreateInfo.bIsResolveTarget));
}

#elif ENGINE_MINOR_VERSION >= 20

UTexture2DDynamic* FRPRCpTexture2DDynamic::Create(int32 SizeX, int32 SizeY, const FCreateInfo& CreateInfo)
{
    FTexture2DDynamicCreateInfo nativeCreateInfo(
        CreateInfo.Format,
        CreateInfo.bIsResolveTarget,
        CreateInfo.bSRGB,
        CreateInfo.Filter,
        CreateInfo.SamplerAddressMode
    );
    return (UTexture2DDynamic::Create(SizeX, SizeY, nativeCreateInfo));
}

#endif