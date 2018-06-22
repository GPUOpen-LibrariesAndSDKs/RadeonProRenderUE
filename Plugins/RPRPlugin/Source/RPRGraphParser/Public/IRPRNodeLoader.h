#pragma once

class IRPRNodeLoader
{
public:

	virtual void	Load(FRPRUberMaterialParameters& Parameters) = 0;

};

typedef TSharedPtr<IRPRNodeLoader> IRPRNodeLoaderPtr;
