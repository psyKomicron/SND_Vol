#pragma once
#include "MainWindow.g.h"

#include <vector>
#include "AudioSession.h"
#include "LegacyAudioController.h"

namespace winrt::SND_Vol::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();
        ~MainWindow() = default;

        void OnLoaded(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AudioSessionView_VolumeChanged(winrt::SND_Vol::AudioSessionView const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);
        void AudioSessionView_VolumeStateChanged(winrt::SND_Vol::AudioSessionView const& sender, bool const& args);
        void AudioSessionsPanel_Loading(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        using BackdropController = winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController;

        bool loaded = false;
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        BackdropController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        std::unique_ptr<Audio::LegacyAudioController> audioController = nullptr;
        std::unique_ptr<std::vector<Audio::AudioSessionContainer>> audioSessions = nullptr;

        std::vector<winrt::event_token> audioSessionVolumeChanged{};
        std::vector<winrt::SND_Vol::AudioSessionView::VolumeChanged_revoker> volumeChangedRevokers{};
        std::vector<winrt::SND_Vol::AudioSessionView::VolumeStateChanged_revoker> volumeStateChangedRevokers{};
        std::vector<winrt::event_token> audioSessionsStateChanged{};

        void InitWindow();
        void LoadContent();
        void SetBackground();
        void AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
