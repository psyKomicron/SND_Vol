#include "pch.h"
#include "HotKeyManager.h"

using namespace std;
using namespace winrt::Windows::System;


namespace System
{
	atomic_int32_t HotKeyManager::id = 0;


	HotKeyManager::HotKeyManager()
	{
	}

	HotKeyManager::~HotKeyManager()
	{
	}


	void HotKeyManager::RegisterHotKey()
	{
	}

}