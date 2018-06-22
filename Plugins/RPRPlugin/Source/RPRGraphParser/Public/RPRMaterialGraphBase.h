#pragma once

template<typename TParsedElementType>
class FRPRMaterialGraphBase
{
public:

	virtual bool	Parse(const TParsedElementType& Element) = 0;
	virtual void	Load(FRPRMaterialGraphSerializationContext& SerializationContext) = 0;

	const FName&							GetName() const;
	FRPRMaterialXmlUberNodePtr				GetUberMaterial() const;
	FRPRMaterialXmlNodePtr					GetFirstMaterial();
	const FRPRMaterialXmlNodePtr			GetFirstMaterial() const;
	const TArray<FRPRMaterialXmlNodePtr>&	GetMaterials() const;

	FRPRMaterialXmlNodePtr		FindNodeByName(const FName& NodeName);

	template<typename NodeType>
	TSharedPtr<NodeType>		FindNodeByName(const FName& NodeName)
	{
		FRPRMaterialXmlNodePtr nodePtr = FindNodeByName(NodeName);
		if (nodePtr.IsValid())
		{
			return (StaticCastSharedPtr<NodeType>(nodePtr));
		}
		return (nullptr);
	}

protected:

	FName	Name;

};

template<typename TParsedElementType>
const FName& FRPRMaterialGraphBase<TParsedElementType>::GetName() const
{
	return (Name);
}
