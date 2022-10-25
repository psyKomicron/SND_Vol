#pragma once

#include "SecondWindow.g.h"

namespace winrt::SND_Vol::implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow>
    {
    public:
        SecondWindow();

        void Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void CloseHotKeysViewerButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;

        void InitWindow();
        void SetBackground();
        void AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs);
    public:
        void HotKeysViewerGrid_PreviewKeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void RootGrid_KeyUp(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow, implementation::SecondWindow>
    {
    };
}
