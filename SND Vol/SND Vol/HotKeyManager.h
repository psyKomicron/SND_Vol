#pragma once
#include "HotKey.h"

namespace System
{
	class HotKeyManager
	{
	public:
		/**
		 * @brief Default constructor.
		*/
		HotKeyManager();
		~HotKeyManager();

		/**
		 * @brief Registers a system wide hotkey.
		 * @param accelerator Key combination to register
		*/
		void RegisterHotKey();

	private:
		static std::atomic_int32_t id;
	};
}

