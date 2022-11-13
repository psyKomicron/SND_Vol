#pragma once
#include "MainWindow.g.h"

#include <vector>
#include <map>
#include "AudioSession.h"
#include "LegacyAudioController.h"
#include "MainAudioEndpoint.h"
#include "HotKeyManager.h"
#include "HotKey.h"

using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
    public:
        MainWindow();
        ~MainWindow();

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
        void DisableAnimationsToggleMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SwitchPresenterStyleMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ViewHotKeysHyperlinkButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        using BackdropController = winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController;

        // Logic related attributes
        std::mutex audioSessionsMutex{};
        Audio::MainAudioEndpoint* mainAudioEndpoint = nullptr;
        Audio::LegacyAudioController* audioController = nullptr;
        std::unique_ptr<std::vector<Audio::AudioSession*>> audioSessions{ nullptr };
        winrt::event_token mainAudioEndpointVolumeChangedToken;
        winrt::event_token mainAudioEndpointStateChangedToken;
        winrt::event_token audioControllerSessionAddedToken;
        winrt::event_token audioControllerEndpointChangedToken;
        std::map<winrt::guid, winrt::event_token> audioSessionVolumeChanged{};
        std::map<winrt::guid, winrt::event_token> audioSessionsStateChanged{};
        //System::HotKeyManager hotKeyManager{};
        System::HotKey volumeUpHotKeyPtr = System::HotKey(VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift, VK_UP);
        System::HotKey volumeDownHotKeyPtr = System::HotKey(VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift, VK_DOWN);
        System::HotKey volumePageUpHotKeyPtr = System::HotKey(VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift, VK_PRIOR);
        System::HotKey volumePageDownHotKeyPtr = System::HotKey(VirtualKeyModifiers::Control | VirtualKeyModifiers::Shift, VK_NEXT);
        System::HotKey muteHotKeyPtr = System::HotKey(VirtualKeyModifiers::Menu | VirtualKeyModifiers::Shift, static_cast<uint32_t>('M'));
        // UI related attributes
        bool loaded = false;
        bool compact = false;
        bool usingCustomTitleBar = false;
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = nullptr;
        winrt::event_token appWindowClosingEventToken;
        BackdropController backdropController = nullptr;
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController = nullptr;
        winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration systemBackdropConfiguration = nullptr;
        winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer audioSessionsPeakTimer = nullptr;
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer mainAudioEndpointPeakTimer = nullptr;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioSessionView> audioSessionViews
        {
            winrt::multi_threaded_observable_vector<winrt::SND_Vol::AudioSessionView>()
        };
        winrt::SND_Vol::AudioSessionState globalSessionAudioState = winrt::SND_Vol::AudioSessionState::Unmuted;
        winrt::Microsoft::UI::Xaml::Window secondWindow{ nullptr };

        void InitWindow();
        void SetBackground();
        void LoadContent();
        void CreateAudioView(Audio::AudioSession* audioSession, bool enabled);
        void SetDragRectangles();
        void SaveAudioLevels();
        void LoadHotKeys();

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
