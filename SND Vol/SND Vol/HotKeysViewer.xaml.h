// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "HotKeysViewer.g.h"
#include "HotKeyViewModel.h"

namespace winrt::SND_Vol::implementation
{
    struct HotKeysViewer : HotKeysViewerT<HotKeysViewer>
    {
    public:
        HotKeysViewer();

        bool ShowMouseMap()
        {
            return MouseMapViewBox().Visibility() == winrt::Microsoft::UI::Xaml::Visibility::Visible;
        };

        void ShowMouseMap(const bool& value);

        void SetActiveKeys(const winrt::Windows::Foundation::Collections::IVector<winrt::Windows::System::VirtualKey>& activeKeys);
        void AddActiveKey(const winrt::SND_Vol::HotKeyViewModel& hotKeyView);

    private:
        std::vector<winrt::SND_Vol::HotKeyViewModel> hotKeys{};

        winrt::Microsoft::UI::Xaml::Media::Brush GetActiveBrush();
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct HotKeysViewer : HotKeysViewerT<HotKeysViewer, implementation::HotKeysViewer>
    {
    };
}
