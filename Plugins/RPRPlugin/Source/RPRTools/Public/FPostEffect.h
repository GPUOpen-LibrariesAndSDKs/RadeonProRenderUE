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

#include <RadeonProRender.h>

namespace RPR
{

class RPRTOOLS_API FPostEffect
{
public:
	FPostEffect();
	~FPostEffect();

	int Create(rpr_context context, rpr_post_effect_type type);
	int Destroy();

	int Attach(rpr_context context);
	int Detach(rpr_context context);

	int SetFloat(const char* key, float value);
	int SetUInt(const char* key, unsigned int value);

	operator bool() const { return PostEffect != nullptr; }

	FPostEffect(const FPostEffect&)            = delete;
	FPostEffect& operator=(const FPostEffect&) = delete;
private:
	rpr_post_effect	PostEffect;
};

inline int ContextCreatePostEffect(rpr_context context, rpr_post_effect_type type, FPostEffect* out_post_effect)
{
	return out_post_effect->Create(context, type);
}

inline int ContextAttachPostEffect(rpr_context context, FPostEffect* postEffect)
{
	return postEffect->Attach(context);
}

inline int ContextDetachPostEffect(rpr_context context, FPostEffect* postEffect)
{
	return postEffect->Detach(context);
}

inline int DestroyPostEffect(FPostEffect* postEffect)
{
	return postEffect->Destroy();
}

} // namespace RPR