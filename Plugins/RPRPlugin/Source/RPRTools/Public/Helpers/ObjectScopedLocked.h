/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once
#include "Helpers/IObjectScopedLockable.h"

template<typename ObjectType>
class FObjectScopedLocked
{
public:
	
	FObjectScopedLocked(ObjectType* Object);
	FObjectScopedLocked(const ObjectType* Object);

	virtual ~FObjectScopedLocked();

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
