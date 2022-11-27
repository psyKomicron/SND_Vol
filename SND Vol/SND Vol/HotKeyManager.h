#pragma once
#include "HotKey.h"
#include <concurrent_unordered_map.h>

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
		void RegisterHotKey(const winrt::Windows::System::VirtualKeyModifiers& modifiers, const uint32_t& virtualKey);

		/**
		 * @brief Copy operator.
		*/
		HotKeyManager& operator=(const HotKeyManager& other) = delete;

	private:
		concurrency::concurrent_unordered_map<winrt::guid, std::unique_ptr<HotKey>> hotKeys{};

		/**
		 * @brief Default constructor.
		*/
		HotKeyManager() = default;
	};
}

