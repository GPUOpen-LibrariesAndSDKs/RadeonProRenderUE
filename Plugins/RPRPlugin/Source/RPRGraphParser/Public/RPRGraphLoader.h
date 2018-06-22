#pragma once
#include "IRPRGraphSchema.h"

template<typename TParsedElement>
class FRPRGraphLoader
{
	using TRPRGraphSchema = IRPRGraphSchema<TParsedElement>;
	using TRPRGraphSchemaPtr = TSharedPtr<TRPRGraphSchema>;

public:

	FRPRGraphLoader(TRPRGraphSchemaPtr InSchema);

	void	Parse(const TParsedElement& Element);
	void	Load(FRPRUberMaterialParameters& Parameters);

private:

	TRPRGraphSchemaPtr			Schema;
	TArray<IRPRGraphNodePtr>	Nodes;

};

template<typename TParsedElement>
void FRPRGraphLoader<TParsedElement>::Load(FRPRUberMaterialParameters& Parameters)
{
	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		IRPRNodeLoaderPtr nodeLoaderPtr = Schema->CreateLoaderFromNode(Nodes[i]);
		nodeLoaderPtr->Load(Parameters);
	}
}

template<typename TParsedElement>
FRPRGraphLoader<TParsedElement>::FRPRGraphLoader(TRPRGraphSchemaPtr InSchema)
	: Schema(InSchema)
{
	check(Schema.IsValid());
}

template<typename TParsedElement>
void FRPRGraphLoader<TParsedElement>::Parse(const TParsedElement& Element)
{
	Nodes.Empty();

	IRPRGraphNodePtr node;
	while (Schema->NextNode(Element, node))
	{
		Nodes.Add(node);
	}
}
