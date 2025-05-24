/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <functional>

#include "../../AvalonUnit/AvalonUnit.h"


template<class ... EventParams>
struct FEventDispatcher
{
public:
	typedef void(* Callback)(IAvalonUnit*, EventParams ... args);

	void BindEvent(IAvalonUnit* Listener, Callback Callback)
	{
		if (Listener != nullptr)
		{
			HardUnitRef ListerRef = Listener->GetSelfRef();
			mListeners.push_back(ListerRef);
			mFncPtrs.push_back(Callback);
		}
	}

	void UnBindEvent(IAvalonUnit* Listener)
	{
		for(int i = mListeners.size() - 1; i >= 0; --i)
		{
			HardUnitRef HardRef = mListeners[i].lock();
			if (HardRef == nullptr)
			{
				// This is a soft error, we should print something out here
				mListeners.erase(mListeners.begin() + i);
				mFncPtrs.erase(mFncPtrs.begin() + i);
			}
			else if (IAvalonUnit::Get<IAvalonUnit>(HardRef) == Listener)
			{
				mListeners.erase(mListeners.begin() + i);
				mFncPtrs.erase(mFncPtrs.begin() + i);
				return;
			}
		}
	}

	void BroadcastEvent(EventParams ... args)
	{
		std::vector<int> ToRemove;

		for (int i = 0; i < mListeners.size(); ++i)
		{
			HardUnitRef HardRef = mListeners[i].lock();
			if (HardRef != nullptr)
			{
				IAvalonUnit* Listener = IAvalonUnit::Get<IAvalonUnit>(HardRef);
				std::invoke(mFncPtrs[i], Listener, args...);
			}
			else
			{
				ToRemove.push_back(i);
			}
		}

		for (int i = ToRemove.size() - 1; i >= 0; --i)
		{
			// This is a soft error, we should print something out here
			int Index = ToRemove[i];
			mListeners.erase(mListeners.begin() + Index);
			mFncPtrs.erase(mFncPtrs.begin() + Index);
		}
	}

	bool IsBound() const
	{
		return mListeners.size() > 0;
	}

	void UnBindAllEvents()
	{
		mListeners.clear();
		mFncPtrs.clear();
	}

private:

	std::vector<SoftUnitRef> mListeners;
	std::vector<Callback> mFncPtrs;
};