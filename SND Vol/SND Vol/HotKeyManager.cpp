#include "pch.h"
#include "HotKeyManager.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::System;


namespace System
{
	HotKeyManager::~HotKeyManager()
	{
		// TODO: Take read-write lock.
		for (auto&& pair : hotKeys)
		{
			delete pair.second;
		}
	}

	guid HotKeyManager::RegisterHotKey(const VirtualKeyModifiers& modifiers, const uint32_t& virtualKey)
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
				e_hotKeyFired(id, nullptr);
			});
			
			hotKeys.insert({ guid(hotKeyId), hotKey });
			return guid(move(hotKeyId));
		}
		else
		{
			// TODO: Throw exception.
		}
	}

	void HotKeyManager::EditKey(const winrt::guid& hotKeyId, const VirtualKeyModifiers& modifiers, const uint32_t& virtualKey)
	{
		HotKey* hotKeyPtr = hotKeys.at(hotKeyId);
		if (!hotKeyPtr)
		{
			throw exception("Hot key was null.");
		}
		delete hotKeyPtr;

		hotKeyPtr = new HotKey(modifiers, virtualKey);
		hotKeyPtr->Fired([this, id = guid(hotKeyId)](auto, auto)
		{
			e_hotKeyFired(id, nullptr);
		});

		hotKeys.insert({ guid(hotKeyId), hotKeyPtr });
	}

	void HotKeyManager::ReplaceOrInsertKey(System::HotKey* previousKey, System::HotKey* newKey)
	{
		for (auto&& pair : hotKeys)
		{
			//VirtualKeyModifiers modifiers = previousKey->KeyModifiers();
			//VirtualKeyModifiers currentModifiers = pair.second->KeyModifiers();

			if ((static_cast<uint32_t>(previousKey->KeyModifiers()) == static_cast<uint32_t>(pair.second->KeyModifiers())) &&
				(pair.second->Key() == previousKey->Key()))
			{
				hotKeys.insert({ pair.first, newKey }); // Could break iterator.
				break;
			}
		}
	}

}