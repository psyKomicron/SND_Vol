// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "HotKeysViewer.g.h"

namespace winrt::SND_Vol::implementation
{
    struct HotKeysViewer : HotKeysViewerT<HotKeysViewer>
    {
    public:
        HotKeysViewer();

        void SetActiveKeys(const winrt::Windows::Foundation::Collections::IVector<winrt::Windows::System::VirtualKey>& activeKeys);
        void Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Grid_Unloaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Grid_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);

        void OnKeyDown(winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
        {

        };

    private:
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer keyTimer = nullptr;

        winrt::Microsoft::UI::Xaml::Media::Brush GetActiveBrush();
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct HotKeysViewer : HotKeysViewerT<HotKeysViewer, implementation::HotKeysViewer>
    {
    };
}
