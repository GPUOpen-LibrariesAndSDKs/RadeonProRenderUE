// RPR COPYRIGHT

#pragma once

#include "Stats.h"

DECLARE_STATS_GROUP(TEXT("ProRender"), STATGROUP_ProRender, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("CopyFramebuffer"), STAT_ProRender_CopyFramebuffer, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Render"), STAT_ProRender_Render, STATGROUP_ProRender, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("Readback Framebuffer"), STAT_ProRender_Readback, STATGROUP_ProRender, );
