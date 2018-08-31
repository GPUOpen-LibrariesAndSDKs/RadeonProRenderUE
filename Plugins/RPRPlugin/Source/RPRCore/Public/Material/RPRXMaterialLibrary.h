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

#include "Typedefs/RPRTypedefs.h"
#include "Assets/RPRMaterial.h"
#include "Material/MaterialContext.h"
#include "Containers/Map.h"
#include "ImageManager/RPRImageManager.h"
#include "HAL/CriticalSection.h"
#include "Helpers/IObjectScopedLockable.h"
#include "Helpers/RPRIHelpers.h"
#include "Helpers/ObjectScopedLocked.h"

/*
* Library of RPR materials.
* Create native RPR material from FRPRMaterial and keep it in cache.
*/
class RPRCORE_API FRPRXMaterialLibrary : public IObjectScopedLockable
{
public:

	FRPRXMaterialLibrary();
	virtual ~FRPRXMaterialLibrary() {}

	void	Initialize();
	bool	IsInitialized() const;
	bool	Contains(const URPRMaterial* InMaterial) const;
	void	Close();

	bool	CacheAndRegisterMaterial(URPRMaterial* InMaterial);
	bool	RecacheMaterial(URPRMaterial* MaterialKey);
	bool	TryGetMaterial(const URPRMaterial* MaterialKey, RPR::FRPRXMaterialPtr& OutRPRXMaterial);
	bool	TryGetMaterial(const URPRMaterial* MaterialKey, RPR::FRPRXMaterialPtr& OutRPRXMaterial) const;
	void	ClearCache();

	RPR::FMaterialNode GetDummyMaterial() const;
	RPR::FRPRXMaterialPtr	GetMaterial(const URPRMaterial* MaterialKey);

	virtual FCriticalSection& GetCriticalSection() override;

private:

	const RPR::FRPRXMaterialPtr	FindMaterialCache(const URPRMaterial* MaterialKey) const;
	RPR::FRPRXMaterialPtr		FindMaterialCache(const URPRMaterial* MaterialKey);

	void	InitializeDummyMaterial();
	void	DestroyDummyMaterial();

	RPR::FRPRXMaterialPtr	CacheMaterial(URPRMaterial* InMaterial);
	RPR::FMaterialContext	CreateMaterialContext() const;

private:

	TMap<const URPRMaterial*, RPR::FRPRXMaterialPtr>	UEMaterialToRPRMaterialCaches;

	bool bIsInitialized;
	FCriticalSection CriticalSection;
	RPR::FMaterialNode DummyMaterial;
};

using FRPRXMaterialLibrarySL = FObjectScopedLocked<FRPRXMaterialLibrary>;