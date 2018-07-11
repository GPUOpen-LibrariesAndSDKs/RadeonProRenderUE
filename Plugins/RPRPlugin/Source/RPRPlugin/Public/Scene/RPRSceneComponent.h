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

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPRCoreErrorHelper.h"
#include "RPRSceneComponent.generated.h"

enum
{
	PROPERTY_REBUILD_TRANSFORMS = 0x01,
};

#define	RPR_PROPERTY_REBUILD(LogClass, ErrorMessage, flag, function, ... )		\
	if (m_RebuildFlags & flag)													\
	{																			\
		if (function(__VA_ARGS__) != RPR_SUCCESS)								\
		{																		\
			UE_LOG(LogClass, Warning, TEXT(ErrorMessage));						\
			FRPRCoreErrorHelper::LogLastError();								\
		}																		\
	}

#define RPR_PROPERTY_CHECK(value, cachedValue, flag)			\
	if (force || value != cachedValue)							\
	{															\
		cachedValue = value;									\
		m_RebuildFlags |= flag;									\
	}

/**
* Root of all RPR component, representation of an UE object
*/
UCLASS(Abstract, Transient)
class URPRSceneComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(Transient)
	USceneComponent		*SrcComponent;

	class ARPRScene		*Scene;
public:
	URPRSceneComponent();

	/* Build the RPR object based on the UE4 component */
	virtual bool	Build() { return false; };

	/* Called on the Game thread, after the object has been built */
	virtual bool	PostBuild();

	/* Rebuild the RPR transforms */
	virtual bool	RebuildTransforms() { return false; }

	/* Called on the RPR Thread, execute rpr calls to refresh object properties */
	virtual bool	RPRThread_Update();

	/** Safe call to release resources */
	virtual void	ReleaseResources();

	bool			IsSrcComponentValid() const;
protected:
	void			TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	void			TriggerRebuildTransforms();

	virtual void	BeginDestroy() override;
protected:
	bool					m_Built;
	bool					m_Sync;
	class FRPRPluginModule	*m_Plugin;

	uint32					m_RebuildFlags;

	FCriticalSection		m_RefreshLock;
private:
	FTransform				m_CachedTransforms;
};
