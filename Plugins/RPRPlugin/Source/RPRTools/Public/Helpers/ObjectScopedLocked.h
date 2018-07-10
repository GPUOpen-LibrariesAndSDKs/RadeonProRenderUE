/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#pragma once
#include "Helpers/IObjectScopedLockable.h"

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
