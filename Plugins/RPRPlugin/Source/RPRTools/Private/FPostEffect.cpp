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

#include "FPostEffect.h"
#include <Logging/LogMacros.h>

DEFINE_LOG_CATEGORY_STATIC(LogFPostEffect, Log, All);

#define CHECK_ERROR(status, msg) \
	CA_CONSTANT_IF(status != RPR_SUCCESS) { \
		UE_LOG(LogFPostEffect, Error, msg); \
		return status; \
	}

#define CHECK_WARNING(status, msg) \
	CA_CONSTANT_IF(status != RPR_SUCCESS) { \
		UE_LOG(LogFPostEffect, Warning, msg); \
	}

namespace RPR
{

FPostEffect::FPostEffect()
: PostEffect(nullptr)
{
}

FPostEffect::~FPostEffect()
{
	(void)Destroy();
}

int FPostEffect::Create(rpr_context context, rpr_post_effect_type type)
{
	int status;

	status = Destroy();
	CHECK_WARNING(status, TEXT("destroy old post effect: failed"));

	status = rprContextCreatePostEffect(context, type, &PostEffect);
	CHECK_ERROR(status, TEXT("can't create post effect"));

	return status;
}

int FPostEffect::Destroy()
{
	int status;

	if (!PostEffect)
		return RPR_SUCCESS;

	status = rprObjectDelete(PostEffect);
	CHECK_WARNING(status, TEXT("can't destroy post effect"));

	PostEffect = nullptr;

	return RPR_SUCCESS;
}

int FPostEffect::Attach(rpr_context context)
{
	int status;

	if (!context) {
		CHECK_ERROR(RPR_ERROR_INVALID_CONTEXT, TEXT("rpr context is null"));
	}
	if (!PostEffect) {
		CHECK_ERROR(RPR_ERROR_INVALID_CONTEXT, TEXT("can't attach empty (not created) post effect to context"));
	}

	status = rprContextAttachPostEffect(context, PostEffect);
	CHECK_ERROR(status, TEXT("can't attach post effect to rpr context"));

	return RPR_SUCCESS;
}

int FPostEffect::Detach(rpr_context context)
{
	int status;

	if (!context) {
		CHECK_ERROR(RPR_ERROR_INVALID_CONTEXT, TEXT("rpr context is null"));
	}
	if (!PostEffect) {
		CHECK_ERROR(RPR_ERROR_INVALID_CONTEXT, TEXT("can't detach empty (not created) post effect from context"));
	}

	status = rprContextDetachPostEffect(context, PostEffect);
	CHECK_ERROR(status, TEXT("can't detach post effect from context"));

	return RPR_SUCCESS;
}

int FPostEffect::SetFloat(const char* key, float value)
{
	int status;

	status = rprPostEffectSetParameter1f(PostEffect, key, value);
	CHECK_ERROR(status, TEXT("can't set post effect float parameter"));

	return RPR_SUCCESS;
}

int FPostEffect::SetUInt(const char* key, unsigned int value)
{
	int status;

	status = rprPostEffectSetParameter1u(PostEffect, key, value);
	CHECK_ERROR(status, TEXT("can't set post effect uint parameter"));

	return RPR_SUCCESS;
}

} // namespace RPR