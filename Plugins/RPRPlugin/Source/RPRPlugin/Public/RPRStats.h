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

#include "Stats.h"

DECLARE_STATS_GROUP(TEXT("ProRender"), STATGROUP_ProRender, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Copy framebuffer"), STAT_ProRender_CopyFramebuffer, STATGROUP_ProRender, );

DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Update cameras"), STAT_ProRender_UpdateCameras, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Update viewport camera"), STAT_ProRender_UpdateViewportCamera, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Update lights"), STAT_ProRender_UpdateLights, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Update meshes"), STAT_ProRender_UpdateMeshes, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Update scene"), STAT_ProRender_UpdateScene, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Game Thread: Viewport draw"), STAT_ProRender_DrawViewport, STATGROUP_ProRender, );

DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Pre-render"), STAT_ProRender_PreRender, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Rebuild scene"), STAT_ProRender_RebuildScene, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Render"), STAT_ProRender_Render, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Resolve"), STAT_ProRender_Resolve, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Readback framebuffer"), STAT_ProRender_Readback, STATGROUP_ProRender, );
