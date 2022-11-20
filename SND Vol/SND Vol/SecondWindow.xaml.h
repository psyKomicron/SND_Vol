#pragma once

#include "SecondWindow.g.h"
#include "NavigationBreadcrumbBarItem.h"

namespace winrt::SND_Vol::implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow>
    {
    public:
        SecondWindow();

        inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::HotKeyView> HotKeys()
        {
            return hotKeyViews;
        };

        void Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavigationFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void NavigationBreadcrumbBar_ItemClicked(winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar const& sender, winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBarItemClickedEventArgs const& args);

    private:
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::HotKeyView> hotKeyViews
        {
            single_threaded_observable_vector<winrt::SND_Vol::HotKeyView>()
        }; 
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::NavigationBreadcrumbBarItem> breadCrumbs
        {
            single_threaded_observable_vector<winrt::SND_Vol::NavigationBreadcrumbBarItem>()
        };

        void InitializeWindow();
        void SetBackground();
        void AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs);
    public:
        void NavigationFrame_NavigationFailed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationFailedEventArgs const& e);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow, implementation::SecondWindow>
    {
    };
}
