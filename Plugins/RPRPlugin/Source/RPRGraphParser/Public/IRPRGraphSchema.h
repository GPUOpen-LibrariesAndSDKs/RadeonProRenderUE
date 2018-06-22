#pragma once

template<typename TParsedElementType>
class IRPRGraphSchema
{
public:

	virtual bool				NextNode(const TParsedElementType& ElementType, IRPRSchemaNodePtr& OutNode) = 0;
	virtual IRPRSchemaNodePtr	CreateNode(const TParsedElementType& Item);

};