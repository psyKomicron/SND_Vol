// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "SplashScreen.xaml.h"
#if __has_include("SplashScreen.g.cpp")
#include "SplashScreen.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;


namespace winrt::SND_Vol::implementation
{
    SplashScreen::SplashScreen()
    {
        InitializeComponent();
    }


    void SplashScreen::OnScreenTimeSpan(winrt::Windows::Foundation::TimeSpan value)
    {
        _onScreenTimeSpan = value;
        TimerProgressBarAnimation().Duration(Duration(_onScreenTimeSpan));
    }

    void SplashScreen::UserControl_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (OnScreenTimeSpan().count() == 0ul)
        {
            ProgressRingControl().IsIndeterminate(true);
            ProgressBarControl().IsIndeterminate(true);

            ProgressRingControl().Visibility(Visibility::Visible);
        }
        else
        {
            TimerProgressBar_SizeChanged(nullptr, nullptr);

            TimerProgressBarAnimation().Duration(DurationHelper::FromTimeSpan(_onScreenTimeSpan));
            DeterminateProgressBarGrid().Visibility(Visibility::Visible);
            TimerProgressBarStoryboard().Begin();
        }
    }

    void SplashScreen::TimerProgressBar_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        TimerProgressBarClipping().Rect(Rect(0, 0, BackgroundTimerBorder().ActualWidth(), BackgroundTimerBorder().ActualHeight()));
    }

    void SplashScreen::TimerProgressBarAnimation_Completed(IInspectable const&, IInspectable const&)
    {
        DeterminateProgressBarGrid().Visibility(Visibility::Collapsed);
        NextButton().Visibility(Visibility::Visible);

    #ifdef NDEBUG
        e_dismissed(*this, nullptr);
    #endif // NDEBUG
    }

    void SplashScreen::NextButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        e_dismissed(*this, nullptr);
    }
}
