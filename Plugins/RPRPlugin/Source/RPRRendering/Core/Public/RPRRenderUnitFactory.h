#pragma once

class FRPRRenderUnityFactory
{
public:

	static FRPRRenderUnityFactory&	Get();

	void	RegisterRPRRenderUnit(TSharedPtr<class FRPRRenderUnit> InRenderUnit);
	void	UnregisterRPRRenderUnit(TSharedPtr<class FRPRRenderUnit> InRenderUnit);

	const TArray<TSharedPtr<class FRPRRenderUnit>>&	GetRPRRenderUnits();

private:

	static TSharedPtr<FRPRRenderUnityFactory> Instance;

	TArray<TSharedPtr<class FRPRRenderUnit>> RenderUnits;

};