#pragma once
namespace System
{
	typedef unsigned long PID;

	class ProcessInfo
	{
	public:
		ProcessInfo(const PID& pid);

		std::wstring_view Name();
		std::wstring_view IconPath();

	private:
		std::wstring name;

		void GetProcessInfo(const PID& pid);
		bool GetProcessInfoWin32(const HANDLE& processHandle);
		bool GetProcessInfoUWP(const HANDLE& processHandle);
	};
}

