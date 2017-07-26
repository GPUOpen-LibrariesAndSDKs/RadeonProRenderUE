// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPRSceneComponent.generated.h"

#define	RPR_PROPERTY_REBUILD(LogClass, ErrorMessage, flag, function, ... )		\
	if (m_RebuildFlags & flag)													\
	{																			\
		if (function(__VA_ARGS__) != RPR_SUCCESS)								\
		{																		\
			UE_LOG(LogClass, Warning, TEXT(ErrorMessage));						\
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

	bool			IsSrcComponentValid() const;
protected:
	void			TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;

	void			TriggerRebuildTransforms();
protected:
	bool					m_Built;
	bool					m_Sync;
	class FRPRPluginModule	*m_Plugin;

	uint32					m_RebuildFlags;

	FCriticalSection		m_RefreshLock;
private:
	FTransform				m_CachedTransforms;
};
