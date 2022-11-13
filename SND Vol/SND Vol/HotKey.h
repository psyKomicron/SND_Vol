#pragma once

namespace System
{
	//TODO: Add documentation.
	class HotKey
	{
	public:
		HotKey() = delete;
		HotKey(const winrt::Windows::System::VirtualKeyModifiers& modifiers, const uint32_t& key);
		~HotKey();

		inline winrt::Windows::System::VirtualKeyModifiers KeyModifiers()
		{
			return TranslateModifiers(modifiers);
		};
		inline uint32_t Key()
		{
			return key;
		};

		inline winrt::event_token Fired(const winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, winrt::guid>& handler)
		{
			return e_fired.add(handler);
		};
		inline void Fired(const winrt::event_token& token)
		{
			e_fired.remove(token);
		};

		/**
		 * @brief Activates the key to fire events.
		*/
		void Activate();

	private:
		static std::atomic_int32_t id;

		uint32_t modifiers;
		uint32_t key;
		std::atomic_flag threadFlag{};
		int32_t hotKeyId = 0;
		std::thread* notificationThread = nullptr;
		std::atomic_bool threadRunning = false;
		DWORD threadId;

		winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, winrt::guid>> e_fired{};

		winrt::Windows::System::VirtualKeyModifiers TranslateModifiers(const uint32_t& win32Mods) const;
		void ThreadFunction();
	};
}