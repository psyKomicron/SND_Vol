#include "pch.h"
#include "HotKey.h"

using namespace winrt::Windows::System;


namespace System
{
	std::atomic_int32_t HotKey::id = 1;

	HotKey::HotKey(const VirtualKeyModifiers& modifiers, const uint32_t& key) :
		key{ key }
	{
		hotKeyId = id.fetch_add(1);

		if (static_cast<uint32_t>(modifiers & VirtualKeyModifiers::Control))
		{
			this->modifiers |= MOD_CONTROL;
		}
		if (static_cast<uint32_t>(modifiers & VirtualKeyModifiers::Menu))
		{
			this->modifiers |= MOD_ALT;

		}
		if (static_cast<uint32_t>(modifiers & VirtualKeyModifiers::Shift))
		{
			this->modifiers |= MOD_SHIFT;
		}
		if (static_cast<uint32_t>(modifiers & VirtualKeyModifiers::Windows))
		{
			this->modifiers |= MOD_WIN;
		}
	}

	HotKey::~HotKey()
	{
		if (notificationThread != nullptr)
		{
			PostThreadMessage(threadId, WM_QUIT, 0, 0);
			threadFlag.test_and_set();
			notificationThread->join();
			delete notificationThread;
		}
	}


	void HotKey::Activate()
	{
		notificationThread = new std::thread(&HotKey::ThreadFunction, this);
	}


	VirtualKeyModifiers HotKey::TranslateModifiers(const uint32_t& win32Mods) const
	{
		VirtualKeyModifiers virtualKeyModifiers = VirtualKeyModifiers::None;

		if (win32Mods & MOD_CONTROL)
		{
			virtualKeyModifiers |= VirtualKeyModifiers::Control;
		}
		if (win32Mods & MOD_ALT)
		{
			virtualKeyModifiers |= VirtualKeyModifiers::Menu;
		}
		if (win32Mods & MOD_SHIFT)
		{
			virtualKeyModifiers |= VirtualKeyModifiers::Shift;
		}
		if (win32Mods & MOD_WIN)
		{
			virtualKeyModifiers |= VirtualKeyModifiers::Windows;
		}

		return virtualKeyModifiers;
	}

	void HotKey::ThreadFunction()
	{
		threadId = GetCurrentThreadId();

		if (!::RegisterHotKey(nullptr, hotKeyId, modifiers, key))
		{
			OutputDebugHString(L"Failed to register hot key.");
		}
		else
		{
			OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") registered.");

			MSG message{};
			while (!threadFlag.test())
			{
				//PeekMessage()
				if (GetMessage(&message, (HWND)(-1), 0, 0) & 1)
				{
					if (message.message == WM_HOTKEY)
					{
						OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") > Hotkey pressed.");

						e_fired(winrt::Windows::Foundation::IInspectable(), winrt::guid());
					}
				}
				else
				{
					OutputDebugString(L"HotKey.cpp > GetMessage returned 0\n");
				}
			}

			// Unregister the hotkey when exiting the thread.
			UnregisterHotKey(nullptr, hotKeyId);
			OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") thread exiting.");
		}
	}
}