// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "HotKeysPage.xaml.h"
#if __has_include("HotKeysPage.g.cpp")
#include "HotKeysPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SND_Vol::implementation
{
    HotKeysPage::HotKeysPage()
    {
        InitializeComponent();

        /*winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeUpHotKeyName"), true, VirtualKey::Up, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeDownHotKeyName"), true, VirtualKey::Down, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumePageUpHotKeyName"), true, VirtualKey::PageUp, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumePageDownHotKeyName"), true, VirtualKey::PageDown, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });
        HotKeysViewer().AddActiveKey({ loader.GetString(L"SystemVolumeSwitchStateHotKeyName"), true, VirtualKey::M, VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift });*/
    }
}
