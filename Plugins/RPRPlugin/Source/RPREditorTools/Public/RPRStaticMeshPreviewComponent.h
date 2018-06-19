#pragma once

#include "Components/StaticMeshComponent.h"
#include "TransformCalculus2D.h"
#include "RPRStaticMeshPreviewComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("RPR Static Mesh Preview Component"), STATGROUP_RPRStaticMeshPreviewComponent, STATCAT_Advanced)

UCLASS(ClassGroup = (Rendering, Common), editinlinenew, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API URPRStaticMeshPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	friend class FRPRStaticMeshPreviewProxy;

public:

	URPRStaticMeshPreviewComponent();
	virtual ~URPRStaticMeshPreviewComponent() {}
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = UV)
	void	TransformUV(const FTransform& NewTransform, int32 UVChannel);

	void	TransformUV(const FTransform2D& NewTransform2D, int32 UVChannel);

	void	SelectSection(int32 SectionIndex);
	void	SelectSections(const TArray<int32>& Sections);
	bool	IsSectionSelected(int32 SectionIndex) const;
	void	DeselectSection(int32 SectionIndex);
	void	ClearSectionSelection();

private:

	class FRPRStaticMeshPreviewProxy* SceneProxy;
	
	TArray<int32> SelectedSections;

};