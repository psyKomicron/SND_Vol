// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "MessageBar.xaml.h"
#if __has_include("MessageBar.g.cpp")
#include "MessageBar.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;


namespace winrt::SND_Vol::implementation
{
    MessageBar::MessageBar()
    {
        InitializeComponent();

        timer = DispatcherQueue().CreateTimer();
        auto duration = Application::Current().Resources().Lookup(box_value(L"MessageBarIntervalSeconds")).as<int32_t>();
        timer.Interval(
            std::chrono::seconds(duration)
        );
        timer.Tick({ this, &MessageBar::DispatcherQueueTimer_Tick });
    }

    void MessageBar::EnqueueMessage(const winrt::hstring& message)
    {
        {
            std::unique_lock<std::mutex> lock{ messageQueueMutex };
            messageQueue.push(message);
        }

        if (!timer.IsRunning()) 
        {
            DisplayMessage();
            timer.Start();
        }
    }

    void MessageBar::CloseButton_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        timer.Stop();
        TimerProgressBarStoryboard().Stop();
    }

    void MessageBar::UserControl_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        TimerProgressBar_SizeChanged(nullptr, nullptr);
    }

    void MessageBar::TimerProgressBar_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        TimerProgressBarClipping().Rect(Rect(0, 0, BackgroundTimerBorder().ActualWidth(), BackgroundTimerBorder().ActualHeight()));
    }


    void MessageBar::DisplayMessage()
    {
        // Dequeue a message, send it to the UI
        hstring message = L"";
        {
            std::unique_lock<std::mutex> lock{ messageQueueMutex };

            if (!messageQueue.empty())
            {
                message = messageQueue.front();
                messageQueue.pop();
            }
            else
            {
                timer.Stop();
                Visibility(Visibility::Collapsed);
                MainContentTextBlock().Text(L"");
            }
        }

        if (!message.empty())
        {
            if (Visibility() == Visibility::Collapsed)
            {
                Visibility(Visibility::Visible);
            }

            MainContentTextBlock().Text(message);
            TimerProgressBarStoryboard().Begin();
        }
    }

    void MessageBar::DispatcherQueueTimer_Tick(winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer, winrt::Windows::Foundation::IInspectable)
    {
        DisplayMessage();
    }
}
