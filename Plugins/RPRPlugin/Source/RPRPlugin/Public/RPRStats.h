// RPR COPYRIGHT

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
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Render"), STAT_ProRender_Render, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Resolve"), STAT_ProRender_Resolve, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("RPR Thread: Readback framebuffer"), STAT_ProRender_Readback, STATGROUP_ProRender, );
