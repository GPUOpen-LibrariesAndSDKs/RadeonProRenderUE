#pragma once
#include "IObjectScopedLockable.h"

template<typename ObjectType>
class FObjectScopedLocked
{
public:
	
	FObjectScopedLocked(ObjectType* Object);
	FObjectScopedLocked(const ObjectType* Object);

	~FObjectScopedLocked();

	ObjectType&			operator*();
	const ObjectType&	operator*() const;
	ObjectType*			operator->();
	const ObjectType*	operator->() const;


private:

	ObjectType* objectTypePtr;

};

template<typename ObjectType>
FObjectScopedLocked<ObjectType>::FObjectScopedLocked(ObjectType* Object)
	: objectTypePtr(Object)
{
	static_assert(TIsDerivedFrom<ObjectType, IObjectScopedLockable>::IsDerived, "The ObjectType must inherit from IObjectScopedLockable!");
	objectTypePtr->GetCriticalSection().Lock();
}

template<typename ObjectType>
FObjectScopedLocked<ObjectType>::FObjectScopedLocked(const ObjectType* Object)
{
	static_assert(TIsDerivedFrom<ObjectType, IObjectScopedLockable>::IsDerived, "The ObjectType must inherit from IObjectScopedLockable!");
	objectTypePtr = const_cast<ObjectType*>(Object);
	objectTypePtr->GetCriticalSection().Lock();
}

template<typename ObjectType>
FObjectScopedLocked<ObjectType>::~FObjectScopedLocked()
{
	objectTypePtr->GetCriticalSection().Unlock();
}

template<typename ObjectType>
const ObjectType* FObjectScopedLocked<ObjectType>::operator->() const
{
	return (*objectTypePtr);
}

template<typename ObjectType>
ObjectType* FObjectScopedLocked<ObjectType>::operator->()
{
	return (objectTypePtr);
}

template<typename ObjectType>
const ObjectType& FObjectScopedLocked<ObjectType>::operator*() const
{
	return (*objectTypePtr);
}

template<typename ObjectType>
ObjectType& FObjectScopedLocked<ObjectType>::operator*()
{
	return (objectTypePtr);
}
