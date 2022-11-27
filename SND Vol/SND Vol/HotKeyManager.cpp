#include "pch.h"
#include "HotKeyManager.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::System;


namespace System
{
	HotKeyManager::~HotKeyManager()
	{
	}

	void HotKeyManager::RegisterHotKey(const VirtualKeyModifiers& modifiers, const uint32_t& virtualKey)
	{
		// TODO:
		// - Check arguments validity
		// - Activate and throw properly if the key has failed to activate.
		// - Return an ID so that the caller can identify witch key has been fired.
		// - Handle Fired events and send dispatch them.
		UUID hotKeyId{};
		if (SUCCEEDED((UuidCreate(&hotKeyId))))
		{
			HotKey* hotKey = new HotKey(modifiers, virtualKey);
			hotKey->Fired([this, id = guid(hotKeyId)](auto, auto)
			{

			});

			//hotKeys.insert({ guid(hotKeyId), unique_ptr<HotKey>(hotKey) });
		}
		else
		{
			// TODO: Throw exception.
		}
	}

}