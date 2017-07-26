// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPRSceneComponent.generated.h"

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

	bool			IsSrcComponentValid() const;
protected:
	void			TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
protected:
	bool					m_Built;
	bool					m_Sync;
	class FRPRPluginModule	*m_Plugin;
private:
	FTransform				m_CachedTransforms;
};
