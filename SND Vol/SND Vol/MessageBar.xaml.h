// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "MessageBar.g.h"

#include <queue>

namespace winrt::SND_Vol::implementation
{
    struct MessageBar : MessageBarT<MessageBar>
    {
    public:
        MessageBar();

        void EnqueueMessage(const winrt::hstring& message);

        void CloseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UserControl_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void TimerProgressBar_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        std::mutex messageQueueMutex{};
        std::queue<winrt::hstring> messageQueue{};
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer timer = nullptr;

        void DisplayMessage();

        void DispatcherQueueTimer_Tick(winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer, winrt::Windows::Foundation::IInspectable);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct MessageBar : MessageBarT<MessageBar, implementation::MessageBar>
    {
    };
}
