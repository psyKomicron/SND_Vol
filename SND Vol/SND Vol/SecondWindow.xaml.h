#pragma once

#include "SecondWindow.g.h"
#include "NavigationBreadcrumbBarItem.h"

namespace winrt::SND_Vol::implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow>
    {
    public:
        SecondWindow();

        static inline winrt::SND_Vol::SecondWindow Current()
        {
            return singleton;
        };

        inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::NavigationBreadcrumbBarItem> Breadcrumbs()
        {
            return breadcrumbs;
        };

        inline winrt::Windows::Graphics::RectInt32 DisplayRect()
        {
            if (appWindow)
            {
                winrt::Windows::Graphics::PointInt32 position = appWindow.Position();
                winrt::Windows::Graphics::SizeInt32 size = appWindow.Size();
                return winrt::Windows::Graphics::RectInt32(position.X, position.Y, size.Width, size.Height);
            }
            else
            {
                return winrt::Windows::Graphics::RectInt32();
            }
        };

        void Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavigationFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void NavigationBreadcrumbBar_ItemClicked(winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBar const& sender, winrt::Microsoft::UI::Xaml::Controls::BreadcrumbBarItemClickedEventArgs const& args);

    private:
        static winrt::SND_Vol::SecondWindow singleton;
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::NavigationBreadcrumbBarItem> breadcrumbs
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
