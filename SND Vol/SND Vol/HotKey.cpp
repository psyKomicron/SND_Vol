#include "pch.h"
#include "HotKey.h"

using namespace winrt::Windows::System;

namespace System
{
	HotKey::HotKey(const VirtualKeyModifiers& modifiers, const uint32_t& key) :
		key{ key }
	{
		this->modifiers = TranslateModifiers(modifiers);
	}

	HotKey::~HotKey()
	{
		threadFlag.test_and_set();
		notificationThread.join();
	}


	uint32_t HotKey::TranslateModifiers(const winrt::Windows::System::VirtualKeyModifiers& virtualKeyModifiers)
	{
		uint32_t mod = 0;
		if (static_cast<uint32_t>(virtualKeyModifiers & VirtualKeyModifiers::Control))
		{
			mod |= MOD_CONTROL;
		}
		if (static_cast<uint32_t>(virtualKeyModifiers & VirtualKeyModifiers::Menu))
		{
			mod |= MOD_ALT;

		}
		if (static_cast<uint32_t>(virtualKeyModifiers & VirtualKeyModifiers::Shift))
		{
			mod |= MOD_SHIFT;

		}
		if (static_cast<uint32_t>(virtualKeyModifiers & VirtualKeyModifiers::Windows))
		{
			mod |= MOD_WIN;
		}

		return mod;
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
		if (!::RegisterHotKey(nullptr, hotKeyId, modifiers | MOD_NOREPEAT, key))
		{
			OutputDebugHString(L"Failed to register hot key");
		}
		else
		{
			MSG message{};
			while (!threadFlag.test() && (GetMessage(&message, nullptr, 0, 0) & 1))
			{
				if (message.message == WM_QUIT)
				{
					break;
				}

				if (message.message == WM_HOTKEY)
				{
					OutputDebugHString(L"HotkeyManager > Hotkey pressed");
				}
			}

			UnregisterHotKey(nullptr, hotKeyId);
		}

		OutputDebugHString(L"HotKeyManager shutdown.");
	}
}