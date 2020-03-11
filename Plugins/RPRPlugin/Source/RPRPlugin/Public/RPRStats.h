/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "Stats/Stats.h"

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
