#pragma once

namespace System
{
	class HotKey
	{
	public:
		HotKey() = default;
		HotKey(const winrt::Windows::System::VirtualKeyModifiers& modifiers, const uint32_t& key);
		~HotKey();

		inline winrt::Windows::System::VirtualKeyModifiers KeyModifiers()
		{
			return TranslateModifiers(modifiers);
		};
		inline void KeyModifiers(const winrt::Windows::System::VirtualKeyModifiers& value)
		{
			modifiers = TranslateModifiers(value);
		};
		inline uint32_t Key()
		{
			return key;
		};


	private:
		static std::atomic_int32_t id;

		uint32_t modifiers;
		uint32_t key;
		std::atomic_flag threadFlag{};
		int32_t hotKeyId = 0;
		std::thread notificationThread;
		std::atomic_bool threadRunning = false;

		uint32_t TranslateModifiers(const winrt::Windows::System::VirtualKeyModifiers& modifiers);
		winrt::Windows::System::VirtualKeyModifiers TranslateModifiers(const uint32_t& win32Mods) const;
		uint32_t TranslateVirtualKey(const winrt::Windows::System::VirtualKey& key);
		void ThreadFunction();
	};
}