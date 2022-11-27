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