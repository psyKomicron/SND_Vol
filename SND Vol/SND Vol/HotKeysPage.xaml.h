// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "HotKeysPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct HotKeysPage : HotKeysPageT<HotKeysPage>
    {
    public:
        HotKeysPage();

        void OnKeyDown(const winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs& args);
        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct HotKeysPage : HotKeysPageT<HotKeysPage, implementation::HotKeysPage>
    {
    };
}
