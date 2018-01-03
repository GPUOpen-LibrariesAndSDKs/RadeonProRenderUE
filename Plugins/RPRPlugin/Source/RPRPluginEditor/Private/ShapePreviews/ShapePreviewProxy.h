#pragma once

#include "HitProxies.h"

struct HShapePreviewProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	HShapePreviewProxy() : HHitProxy(HPP_World) {}
};