#pragma once

#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Version.lib")

#include <Audiopolicy.h>
#include <chrono>
#include <comdef.h>
#include <comip.h>
#include <dispatcherqueue.h>
#include <endpointvolume.h>
#include <hstring.h>
#include <math.h>
#include <mmdeviceapi.h>
#include <mutex>
#include <Psapi.h>
#include <restrictederrorinfo.h>
#include <rpc.h>
#include <Shobjidl.h>
#include <unknwn.h>
#include <windows.h>


#ifdef _DEBUG
 #define DEBUG
#else
 #undef DEBUG
#endif
// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.Windows.AppLifecycle.h>

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>

#include <wil/cppwinrt_helpers.h>
#include <microsoft.ui.xaml.window.h>


// IMMDevicePtr
_COM_SMARTPTR_TYPEDEF(IMMDevice, __uuidof(IMMDevice));

// IMMDeviceEnumeratorPtr
_COM_SMARTPTR_TYPEDEF(IMMDeviceEnumerator, __uuidof(IMMDeviceEnumerator));

// IAudioSessionManager2Ptr
_COM_SMARTPTR_TYPEDEF(IAudioSessionManager2, __uuidof(IAudioSessionManager2));

// IAudioSessionManager2Ptr
_COM_SMARTPTR_TYPEDEF(IAudioSessionManager2, __uuidof(IAudioSessionManager2));

// IAudioSessionEnumeratorPtr
_COM_SMARTPTR_TYPEDEF(IAudioSessionEnumerator, __uuidof(IAudioSessionEnumerator));

// IAudioSessionControl2Ptr
_COM_SMARTPTR_TYPEDEF(IAudioSessionControl2, __uuidof(IAudioSessionControl2));

// IAudioSessionControlPtr
_COM_SMARTPTR_TYPEDEF(IAudioSessionControl, __uuidof(IAudioSessionControl));

// ISimpleAudioVolumePtr
_COM_SMARTPTR_TYPEDEF(ISimpleAudioVolume, __uuidof(ISimpleAudioVolume));

// IAudioSessionNotificationPtr
_COM_SMARTPTR_TYPEDEF(IAudioSessionNotification, __uuidof(IAudioSessionNotification));

// IAudioEndpointVolumePtr
_COM_SMARTPTR_TYPEDEF(IAudioEndpointVolume, __uuidof(IAudioEndpointVolume));

// IAudioMeterInformationPtr
_COM_SMARTPTR_TYPEDEF(IAudioMeterInformation, __uuidof(IAudioMeterInformation));


inline void OutputDebugHString(winrt::hstring const& text)
{
    OutputDebugString(text.c_str());
    OutputDebugString(L"\n");
};