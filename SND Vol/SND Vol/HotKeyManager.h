#pragma once
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include "HotKey.h"

namespace System
{
	/**
	 * @brief Singleton class to manage hotkeys.
	*/
	class HotKeyManager
	{
	public:
		/**
		 * @brief Copy constructor.
		*/
		HotKeyManager(const HotKeyManager& other) = delete;
		~HotKeyManager();

		static HotKeyManager& GetHotKeyManager()
		{
			static HotKeyManager instance{};
			return instance;
		};

		winrt::guid RegisterHotKey(const winrt::Windows::System::VirtualKeyModifiers& modifiers, const uint32_t& virtualKey);
		void EditKey(const winrt::guid& hotKeyId, const winrt::Windows::System::VirtualKeyModifiers& modifiers, const uint32_t& virtualKey);
		void ReplaceOrInsertKey(System::HotKey* previousKey, System::HotKey* newKey);

		/**
		 * @brief Copy operator.
		*/
		HotKeyManager& operator=(const HotKeyManager& other) = delete;

	private:
		Concurrency::concurrent_unordered_map<winrt::guid, HotKey*> hotKeys{};

		winrt::Windows::Foundation::TypedEventHandler<winrt::guid, winrt::Windows::Foundation::IInspectable> e_hotKeyFired{};

		/**
		 * @brief Default constructor.
		*/
		HotKeyManager() = default;
	};
}

