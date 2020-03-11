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

DECLARE_DELEGATE_RetVal_OneParam(FDelegateHandle, FDelegateHandleManagerSubscriber, void* /* Key */)
DECLARE_DELEGATE_TwoParams(FDelegateHandleManagerUnsubscriber, void* /* Key */, FDelegateHandle)

template<typename TKey>
class FDelegateHandleManager
{
	struct FData
	{
		TKey* Key;
		FDelegateHandle DelegateHandle;
		int32 NumSubscriptions;
	};

public:

	void	Initialize(
		FDelegateHandleManagerSubscriber InSubscriberCallback, 
		FDelegateHandleManagerUnsubscriber InUnsubscriberCallback)
	{
		SubscriberCallback = InSubscriberCallback;
		UnsubscriberCallback = InUnsubscriberCallback;
	}

	void	Subscribe(TKey* Key)
	{
		check(SubscriberCallback.IsBound());

		FData* datum = FindDatumByKey(Key);
		if (datum == nullptr)
		{
			FData newData;
			newData.NumSubscriptions = 1;
			newData.DelegateHandle = SubscriberCallback.Execute(Key);
			newData.Key = Key;

			Data.Add(newData);
		}
		else
		{
			++datum->NumSubscriptions;
		}
	}
	
	void	Unsubscribe(TKey* Key)
	{
		check(UnsubscriberCallback.IsBound());

		int32 index = Data.IndexOfByPredicate([Key] (FData datum) { return datum.Key == Key; });
		if (index == INDEX_NONE)
		{
			return;
		}

		FData& datum = Data[index];
		--datum.NumSubscriptions;

		if (datum.NumSubscriptions == 0)
		{
			UnsubscriberCallback.Execute(Key, datum.DelegateHandle);
			Data.RemoveAt(index);
		}
	}

	void	UnsubscribeAll()
	{
		for (int32 i = 0; i < Data.Num(); ++i)
		{
			UnsubscriberCallback.Execute(Data[i].Key, Data[i].DelegateHandle);
		}
		Data.Empty();
	}


private:

	bool	IsKeyRegistered(const TKey* Key)
	{
		return FindDatumByKey(Key) != nullptr;
	}

	FData*	FindDatumByKey(const TKey* Key)
	{
		return Data.FindByPredicate([Key] (FData datum) { return datum.Key == Key; });
	}


private:

	TArray<FData> Data;

	FDelegateHandleManagerSubscriber SubscriberCallback;
	FDelegateHandleManagerUnsubscriber UnsubscriberCallback;

};
