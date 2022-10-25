#pragma once
#include "MainWindow.g.h"

#include <vector>
#include <map>
#include "AudioSession.h"
#include "LegacyAudioController.h"
#include "MainAudioEndpoint.h"
#include "HotKeyManager.h"

namespace winrt::SND_Vol::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();
        ~MainWindow() = default;

        inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioSessionView> AudioSessions() const
        {
            return audioSessionViews;
        };

        void OnLoaded(winrt::Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AudioSessionView_VolumeChanged(winrt::SND_Vol::AudioSessionView const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args);
        void AudioSessionView_VolumeStateChanged(winrt::SND_Vol::AudioSessionView const& sender, bool const& args);
        void AudioSessionsPanel_Loading(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void SystemVolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void Window_Activated(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void SystemVolumeActivityBorder_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const&);
        void HideMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RestartMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SwitchStateMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void HorizontalViewMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void VerticalViewMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AutoViewMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ReloadMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void KeepOnTopToggleMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void HotKeySettingsMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MuteToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        using BackdropController = winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController;

        // Logic related attributes
        Audio::MainAudioEndpoint* mainAudioEndpoint = nullptr;
        Audio::LegacyAudioController* audioController = nullptr;
        std::unique_ptr<std::vector<Audio::AudioSession*>> audioSessions{ nullptr };
        winrt::event_token mainAudioEndpointVolumeChangedToken;
        winrt::event_token mainAudioEndpointStateChangedToken;
        winrt::event_token audioControllerSessionAddedToken;
        winrt::event_token audioControllerEndpointChangedToken;
        std::map<winrt::guid, winrt::event_token> audioSessionVolumeChanged{};
        std::map<winrt::guid, winrt::event_token> audioSessionsStateChanged{};
        System::HotKeyManager hotKeyManager{};
        // UI related attributes
        std::mutex audioSessionsMutex{};
        bool loaded = false;
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        BackdropController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        std::vector<winrt::SND_Vol::AudioSessionView::VolumeChanged_revoker> volumeChangedRevokers{};
        std::vector<winrt::SND_Vol::AudioSessionView::VolumeStateChanged_revoker> volumeStateChangedRevokers{};
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer oneMsTimer = nullptr;
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer fiveMstimer = nullptr;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioSessionView> audioSessionViews
        {
            winrt::multi_threaded_observable_vector<winrt::SND_Vol::AudioSessionView>()
        };
        winrt::SND_Vol::AudioSessionState globalSessionAudioState = winrt::SND_Vol::AudioSessionState::Unmuted;

        void InitWindow();
        void SetBackground();
        void LoadContent();
        void CreateAudioView(Audio::AudioSession* audioSession, bool enabled);
        void SetDragRectangles();
        void AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs);
        void UpdatePeakMeters(winrt::Windows::Foundation::IInspectable /*sender*/, winrt::Windows::Foundation::IInspectable /*args*/);
        void MainAudioEndpoint_VolumeChanged(winrt::Windows::Foundation::IInspectable /*sender*/, const float& newVolume);
        void AudioSession_VolumeChanged(const winrt::guid& sender, const float& newVolume);
        void AudioSession_StateChanged(const winrt::guid& sender, const uint32_t& state);
        void AudioController_SessionAdded(winrt::Windows::Foundation::IInspectable /*sender*/, winrt::Windows::Foundation::IInspectable /*args*/);
        void AudioController_EndpointChanged(winrt::Windows::Foundation::IInspectable /*sender*/, winrt::Windows::Foundation::IInspectable /*args*/);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
