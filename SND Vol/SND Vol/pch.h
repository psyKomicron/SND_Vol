﻿#pragma once
#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Audiopolicy.h>
#include <comdef.h>
#include <comip.h>
#include <Psapi.h>
#include <dispatcherqueue.h>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>

#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>

#include <wil/cppwinrt_helpers.h>
#include <microsoft.ui.xaml.window.h>

#pragma comment(lib, "Rpcrt4.lib")

_COM_SMARTPTR_TYPEDEF(IMMDevice, __uuidof(IMMDevice));
_COM_SMARTPTR_TYPEDEF(IMMDeviceEnumerator, __uuidof(IMMDeviceEnumerator));
_COM_SMARTPTR_TYPEDEF(IAudioSessionManager2, __uuidof(IAudioSessionManager2));
_COM_SMARTPTR_TYPEDEF(IAudioSessionManager2, __uuidof(IAudioSessionManager2));
_COM_SMARTPTR_TYPEDEF(IAudioSessionEnumerator, __uuidof(IAudioSessionEnumerator));
_COM_SMARTPTR_TYPEDEF(IAudioSessionControl2, __uuidof(IAudioSessionControl2));
_COM_SMARTPTR_TYPEDEF(IAudioSessionControl, __uuidof(IAudioSessionControl));
_COM_SMARTPTR_TYPEDEF(ISimpleAudioVolume, __uuidof(ISimpleAudioVolume));
_COM_SMARTPTR_TYPEDEF(IAudioSessionNotification, __uuidof(IAudioSessionNotification));

inline void OutputDebugHString(winrt::hstring const& text)
{
    OutputDebugString(text.c_str());
    OutputDebugString(L"\n");
};