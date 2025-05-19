/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <functional>

// Those who can bind to events!
class IEventListener
{ };

template<class ... EventParams>
struct FEventDispatcher
{
public:
	typedef void(* Callback)(IEventListener*, EventParams ... args);

	void BindEvent(IEventListener* Listener, Callback Callback)
	{
		mListeners.push_back(Listener);
		mFncPtrs.push_back(Callback);
	}

	void UnBindEvent(IEventListener* Listener)
	{
		for (int i = 0; i < mListeners.size(); ++i)
		{
			if (mListeners[i] == Listener)
			{
				mListeners.erase(mListeners.begin() + i);
				mFncPtrs.erase(mFncPtrs.begin() + i);
				return;
			}
		}
	}

	void BroadcastEvent(EventParams ... args)
	{
		for (int i = 0; i < mListeners.size(); ++i)
		{
			IEventListener* Listener = mListeners[i];
			std::invoke(mFncPtrs[i], Listener, args...);
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
	std::vector<IEventListener*> mListeners;
	std::vector<Callback> mFncPtrs;
};