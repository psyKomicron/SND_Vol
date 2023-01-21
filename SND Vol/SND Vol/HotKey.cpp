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
			PostThreadMessage(threadId, WM_QUIT, 0, 0); // Post quit message to the hotkey thread (GetMessage will return 0).
			notificationThread->join(); // Wait for the thread to exit.
			delete notificationThread; // Free the memory.
		}
	}


	void HotKey::Activate()
	{
		notificationThread = new std::thread(&HotKey::ThreadFunction, this);
		threadFlag.wait(false); // Wait to check if the hotkey was registered successfully or not.
		if (!threadFlag.test()) // If the test returns false it means the hotkey registration has failed, throw exception.
		{
			throw std::invalid_argument("Hot key is not valid (rejected by the system).");
		}
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
			threadFlag.test_and_set();
			threadFlag.notify_one();
		}
		else
		{
			OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") registered.");
			threadFlag.test_and_set();
			threadFlag.notify_one();

			MSG message{};
			while (1)
			{
				//PeekMessage()
				if (GetMessage(&message, (HWND)(-1), 0, 0) & 1)
				{
					if (message.message == WM_HOTKEY)
					{
						OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") > Hotkey pressed.");

						if (keyEnabled.load())
						{
							e_fired(winrt::Windows::Foundation::IInspectable(), winrt::guid());
						}
					}
				}
				else
				{
					OutputDebugString(L"HotKey.cpp > GetMessage returned 0\n");
					break;
				}
			}

			// Unregister the hotkey when exiting the thread.
			UnregisterHotKey(nullptr, hotKeyId);
			OutputDebugHString(L"Hotkey (id: " + winrt::to_hstring(static_cast<uint64_t>(hotKeyId)) + L") thread exiting.");
		}
	}
}