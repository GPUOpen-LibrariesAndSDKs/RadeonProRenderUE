#pragma once

template<typename TMaterialNodeType>
class ITypedNode
{
public:
	virtual TMaterialNodeType GetNodeType() const = 0;
};