#include "pch.h"
#include "HotKeysPage.xaml.h"
#if __has_include("HotKeysPage.g.cpp")
#include "HotKeysPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    HotKeysPage::HotKeysPage()
    {
        InitializeComponent();

        winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeUpHotKeyName"), true, VirtualKey::Up, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeDownHotKeyName"), true, VirtualKey::Down, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumePageUpHotKeyName"), true, VirtualKey::PageUp, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumePageDownHotKeyName"), true, VirtualKey::PageDown, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeSwitchStateHotKeyName"), true, VirtualKey::M, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
    }

    void HotKeysPage::OnKeyDown(const winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs&)
    {
        OutputDebugHString(L"Key pressed.");
    }

    void HotKeysPage::Button_Click(IInspectable const&, RoutedEventArgs const&)
    {
        
    }
}
