#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "HotKey.h"
#include "SecondWindow.xaml.h"
#include "HotKeyManager.h"

#define USE_TIMER 1
#define DEACTIVATE_TIMER 0
#define ENABLE_HOTKEYS 1
#define FORCE_SHOW_SPLASH_SCREEN 0

using namespace Audio;

using namespace std;

using namespace winrt;
using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::ApplicationModel;

using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Resources;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    winrt::SND_Vol::MainWindow MainWindow::singleton{ nullptr };

    MainWindow::MainWindow()
    {
        singleton = *this;

        InitializeComponent();
        InitializeWindow();

    #ifdef DEBUG
        Application::Current().UnhandledException([&](IInspectable const&/*sender*/, UnhandledExceptionEventArgs const& e)
        {
            TextBlock block{};
            block.TextWrapping(TextWrapping::Wrap);
            block.Text(e.Message());
            WindowMessageBar().EnqueueMessage(e.Message());
        });
    #endif // DEBUG

    #if FORCE_SHOW_SPLASH_SCREEN & defined DEBUG
        ApplicationData::Current().LocalSettings().Values().Remove(L"ShowSplashScreen");
    #endif
        if (unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ShowSplashScreen"), true))
        {
            ApplicationData::Current().LocalSettings().Values().Insert(L"ShowSplashScreen", IReference(false));

            winrt::Windows::ApplicationModel::PackageId packageId = winrt::Windows::ApplicationModel::Package::Current().Id();
            ApplicationVersionTextBlock().Text(
                to_hstring(packageId.Version().Major) + L"." + to_hstring(packageId.Version().Minor) + L"." + to_hstring(packageId.Version().Build)
            );
        }
        else
        {
            WindowSplashScreen().Visibility(Visibility::Collapsed);
            ContentGrid().Visibility(Visibility::Visible);
        }
    }

    MainWindow::~MainWindow()
    {
    }


    #pragma region Event handlers
    void MainWindow::OnLoaded(IInspectable const&, RoutedEventArgs const&)
    {
        loaded = true;

        #if USE_TIMER
        if (!DisableAnimationsMenuFlyoutItem().IsChecked())
        {
            if (mainAudioEndpoint)
            {
                mainAudioEndpointPeakTimer.Start();
                VolumeStoryboard().Begin();
            }

            if (audioSessions.get())
            {
                audioSessionsPeakTimer.Start();
            }
        }
        #endif // USE_TIMER

        // Generate size changed event to get correct clipping rectangle size
        SystemVolumeActivityBorder_SizeChanged(nullptr, nullptr);
        Grid_SizeChanged(nullptr, nullptr);

        // Teaching tips
        SettingsButtonTeachingTip().Target(SettingsButton());
        ApplicationDataContainer teachingTips = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"TeachingTips");
        if (!teachingTips)
        {
            teachingTips = ApplicationData::Current().LocalSettings().CreateContainer(L"TeachingTips", ApplicationDataCreateDisposition::Always);
        }
        if (!teachingTips.Values().HasKey(L"ShowSettingsTeachingTip"))
        {
            teachingTips.Values().Insert(L"ShowSettingsTeachingTip", IReference(false));
            SettingsButtonTeachingTip().IsOpen(true);
        }
    }

    void MainWindow::Window_Activated(IInspectable const&, WindowActivatedEventArgs const&)
    {
        #if DEACTIVATE_TIMER
        if (args.WindowActivationState() == WindowActivationState::Deactivated && audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Stop();
        }
        else if (!audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Start();
        }
        #endif
    }

    void MainWindow::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        if (usingCustomTitleBar)
        {
            SetDragRectangles();
        }

        if (appWindow.Size().Width < 210 && !compact)
        {
            compact = true;

            MuteToggleButtonColumn().Width(GridLengthHelper::FromPixels(0));
            SystemVolumeSliderTextColumn().Width(GridLengthHelper::FromPixels(0));

            MuteToggleButton().Visibility(Visibility::Collapsed);
            SystemVolumeSliderText().Visibility(Visibility::Collapsed);
        }
        else if (appWindow.Size().Width > 210 && compact)
        {
            compact = false;

            MuteToggleButtonColumn().Width(GridLengthHelper::Auto());
            SystemVolumeSliderTextColumn().Width(GridLengthHelper::FromPixels(26));

            MuteToggleButton().Visibility(Visibility::Visible);
            SystemVolumeSliderText().Visibility(Visibility::Visible);
        }
    }

    void MainWindow::AudioSessionView_VolumeChanged(AudioSessionView const& sender, RangeBaseValueChangedEventArgs const& args)
    {
        unique_lock lock{ audioSessionsMutex };

        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            guid id = audioSessions->at(i)->Id();
            if (id == sender.Id())
            {
                audioSessions->at(i)->Volume(static_cast<float>(args.NewValue() / 100.0));
            }
        }
    }

    void MainWindow::AudioSessionView_VolumeStateChanged(winrt::SND_Vol::AudioSessionView const& sender, bool const& args)
    {
        unique_lock lock{ audioSessionsMutex };

        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            guid id = audioSessions->at(i)->Id();
            if (id == sender.Id() && audioSessions->at(i)->Muted() != args)
            {
                audioSessions->at(i)->Muted(args);
            }
        }
    }

    void MainWindow::AudioSessionsPanel_Loading(FrameworkElement const&, IInspectable const&)
    {
        LoadContent();

#if ENABLE_HOTKEYS
        // Activate hotkeys.
        try
        {
            volumeUpHotKeyPtr.Activate();
        }
        catch (const std::invalid_argument& ex)
        {
            WindowMessageBar().EnqueueMessage(L"Failed to activate system volume up hot key");
        }

        try
        {
            volumeDownHotKeyPtr.Activate();
        }
        catch (const std::invalid_argument& ex)
        {
            WindowMessageBar().EnqueueMessage(L"Failed to activate system volume down hot key");
        }

        try
        {
            volumePageUpHotKeyPtr.Activate();
        }
        catch (const std::invalid_argument& ex)
        {
            WindowMessageBar().EnqueueMessage(L"Failed to activate system volume up (PageUp) hot key");
        }

        try
        {
            volumePageDownHotKeyPtr.Activate();
        }
        catch (const std::invalid_argument& ex)
        {
            WindowMessageBar().EnqueueMessage(L"Failed to activate system volume down (PageDown) hot key");
        }

        try
        {
            muteHotKeyPtr.Activate();
        }
        catch (const std::invalid_argument& ex)
        {
            WindowMessageBar().EnqueueMessage(L"Failed to activate mute/unmute hot key");
        }
#endif // ENABLE_HOTKEYS
    }

    void MainWindow::SystemVolumeSlider_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        if (mainAudioEndpoint)
        {
            mainAudioEndpoint->Volume(static_cast<float>(e.NewValue() / 100.));
        }

        SystemVolumeSliderText().Text(to_hstring(round(e.NewValue())));
    }

    void MainWindow::SystemVolumeActivityBorder_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        SystemVolumeActivityBorderClippingRight().Rect(
            Rect(0, 0, static_cast<float>(SystemVolumeActivityBorderRight().ActualWidth()), static_cast<float>(SystemVolumeActivityBorderRight().ActualHeight()))
        );
        SystemVolumeActivityBorderClippingLeft().Rect(
            Rect(0, 0, static_cast<float>(SystemVolumeActivityBorderLeft().ActualWidth()), static_cast<float>(SystemVolumeActivityBorderLeft().ActualHeight()))
        );
    }

    void MainWindow::HideMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        appWindow.Presenter().as<OverlappedPresenter>().Minimize();
    }

    void MainWindow::RestartMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        if (Microsoft::Windows::AppLifecycle::AppInstance::Restart(L"") != AppRestartFailureReason::RestartPending)
        {
            ResourceLoader loader{};
            WindowMessageBar().EnqueueMessage(loader.GetString(L"ErrorAppFailedRestart"));
        }
    }

    void MainWindow::SwitchStateMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        bool setMute = false;

        uint32_t count = 0;
        if (globalSessionAudioState == winrt::SND_Vol::AudioSessionState::Unmuted)
        {
            for (auto&& view : audioSessionViews)
            {
                if (view.Muted())
                {
                    count++;
                }
            }

            if (count != audioSessionViews.Size())
            {
                setMute = true;
                globalSessionAudioState = AudioSessionState::Muted;
            }
        }
        else if (globalSessionAudioState == winrt::SND_Vol::AudioSessionState::Muted)
        {
            for (auto&& view : audioSessionViews)
            {
                if (!view.Muted())
                {
                    count++;
                }
            }

            if (count == audioSessionViews.Size())
            {
                setMute = true;
                globalSessionAudioState = AudioSessionState::Muted;
            }
            else
            {
                setMute = false;
                globalSessionAudioState = AudioSessionState::Unmuted;
            }
        }

        {
            unique_lock lock{ audioSessionsMutex };
            for (AudioSession* session : *audioSessions)
            {
                session->Muted(setMute);
            }
        }

        for (AudioSessionView view : audioSessionViews)
        {
            view.Muted(setMute);
        }
    }

    void MainWindow::HorizontalViewMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AudioSessionsScrollViewer().HorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
        AudioSessionsScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Disabled);
        AudioSessionsPanel().Style(
            RootGrid().Resources().Lookup(box_value(L"GridViewHorizontalLayout")).as<Style>()
        );
        layout = 1;
    }

    void MainWindow::VerticalViewMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AudioSessionsScrollViewer().HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
        AudioSessionsScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
        AudioSessionsPanel().Style(
            RootGrid().Resources().Lookup(box_value(L"GridViewVerticalLayout")).as<Style>()
        );
        layout = 2;
    }

    void MainWindow::AutoViewMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AudioSessionsScrollViewer().HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
        AudioSessionsScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Auto);

        AudioSessionsPanel().Style(
            RootGrid().Resources().Lookup(box_value(L"GridViewHorizontalLayout")).as<Style>()
        );
        layout = 0;
    }

    void MainWindow::ReloadMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Stop();
        }
        if (mainAudioEndpointPeakTimer.IsRunning())
        {
            mainAudioEndpointPeakTimer.Stop();
        }

        ResourceLoader loader{};
        WindowMessageBar().EnqueueMessage(loader.GetString(L"InfoReloadingAudioSessions"));

        audioSessionViews.Clear();
        VolumeStoryboard().Stop();

        // Clean up ComPtr/IUnknown objects
        if (audioController)
        {
            if (mainAudioEndpoint)
            {
                mainAudioEndpoint->VolumeChanged(mainAudioEndpointVolumeChangedToken);
                mainAudioEndpoint->Unregister();
                mainAudioEndpoint->Release();
            }

            audioController->EndpointChanged(audioControllerEndpointChangedToken);
            audioController->SessionAdded(audioControllerSessionAddedToken);

            audioController->Unregister();
            audioController->Release();
        }

        // Unregister VolumeChanged event handler & unregister audio sessions from audio events and release com ptrs.
        {
            unique_lock lock{ audioSessionsMutex };

            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                audioSessions->at(i)->VolumeChanged(audioSessionVolumeChanged[audioSessions->at(i)->Id()]);
                audioSessions->at(i)->Unregister();
                audioSessions->at(i)->Release();
            }

            audioSessions->clear();
        }


        // Reload content
        LoadContent();
        WindowMessageBar().EnqueueMessage(loader.GetString(L"InfoAudioSessionsReloaded"));
    }

    void MainWindow::KeepOnTopToggleMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        bool alwaysOnTop = KeepOnTopToggleMenuFlyoutItem().IsChecked();
        appWindow.Presenter().as<OverlappedPresenter>().IsAlwaysOnTop(alwaysOnTop);
        ApplicationData::Current().LocalSettings().Values().Insert(L"IsAlwaysOnTop", box_value(alwaysOnTop));
    }

    void MainWindow::SettingsMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (!secondWindow)
        {
            secondWindow = make<SecondWindow>();
            secondWindow.Closed([this](auto, auto)
            {
                secondWindow = nullptr;
            });
        }
        secondWindow.Activate();
    }

    void MainWindow::MuteToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        mainAudioEndpoint->SetMute(!mainAudioEndpoint->Muted());
        MuteToggleButtonFontIcon().Glyph(mainAudioEndpoint->Muted() ? L"\ue74f" : L"\ue767");
    }

    void MainWindow::DisableAnimationsToggleMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Stop();
        }
        else
        {
            audioSessionsPeakTimer.Start();
        }

        if (mainAudioEndpointPeakTimer.IsRunning())
        {
            mainAudioEndpointPeakTimer.Stop();
        }
        else
        {
            mainAudioEndpointPeakTimer.Start();
        }


        LeftVolumeAnimation().To(0.);
        RightVolumeAnimation().To(0.);
        VolumeStoryboard().Begin();

        for (auto&& view : audioSessionViews)
        {
            view.SetPeak(0, 0);
        }
    }

    void MainWindow::SwitchPresenterStyleMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        OverlappedPresenter presenter = appWindow.Presenter().as<OverlappedPresenter>();
        presenter.IsMaximizable(!presenter.IsMaximizable());
        presenter.IsMinimizable(!presenter.IsMinimizable());

        RightPaddingColumn().Width(GridLengthHelper::FromPixels(
            presenter.IsMinimizable() ? 135 : 45
        ));
    }

    void MainWindow::ViewHotKeysHyperlinkButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SettingsMenuFlyoutItem_Click(nullptr, nullptr);
    }

    void MainWindow::SplashScreen_Dismissed(winrt::SND_Vol::SplashScreen const&, IInspectable const&)
    {
        WindowSplashScreen().Visibility(Visibility::Collapsed);
        ContentGrid().Visibility(Visibility::Visible);
    }

    void MainWindow::ShowAppBarMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        AppBarGrid().Visibility(AppBarGrid().Visibility() == Visibility::Visible ? Visibility::Collapsed : Visibility::Visible);
    }

    void MainWindow::DisableHotKeysMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
#if ENABLE_HOTKEYS
        volumeUpHotKeyPtr.Enabled(!volumeUpHotKeyPtr.Enabled());
        volumeDownHotKeyPtr.Enabled(!volumeDownHotKeyPtr.Enabled());
        volumePageUpHotKeyPtr.Enabled(!volumePageUpHotKeyPtr.Enabled());
        volumePageDownHotKeyPtr.Enabled(!volumePageDownHotKeyPtr.Enabled());
        muteHotKeyPtr.Enabled(!muteHotKeyPtr.Enabled());

        ResourceLoader loader{};
        if (muteHotKeyPtr.Enabled())
        {
            WindowMessageBar().EnqueueMessage(loader.GetString(L"InfoHotKeysEnabled"));
        }
        else
        {
            WindowMessageBar().EnqueueMessage(loader.GetString(L"InfoHotKeysDisabled"));
        }
#endif // ENABLE_HOTKEYS

    }

    void MainWindow::MenuFlyout_Opening(IInspectable const&, IInspectable const&)
    {
        ProfilesMenuFlyoutItem().Items().Clear();
        ApplicationDataContainer audioProfilesContainer = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"AudioProfiles");
        if (audioProfilesContainer)
        {
            for (auto&& profile : audioProfilesContainer.Containers())
            {
                hstring key = profile.Key();
                MenuFlyoutItem item{};
                item.Text(key);
                item.Tag(box_value(key));
                item.Click([this](const IInspectable& sender, RoutedEventArgs)
                    {
                        LoadProfile(sender.as<FrameworkElement>().Tag().as<hstring>());
                    });

                ProfilesMenuFlyoutItem().Items().Append(item);
            }
        }
    }
    #pragma endregion


    void MainWindow::InitializeWindow()
    {
        auto nativeWindow{ this->try_as<::IWindowNative>() };
        check_bool(nativeWindow);

        HWND handle = nullptr;
        nativeWindow->get_WindowHandle(&handle);
        WindowId windowID = GetWindowIdFromWindow(handle);
        appWindow = AppWindow::GetFromWindowId(windowID);
        if (appWindow != nullptr)
        {   
        #ifdef DEBUG
            TextBlock timestamp{};
            timestamp.IsHitTestVisible(false);
            timestamp.Opacity(0.6);
            timestamp.HorizontalAlignment(HorizontalAlignment::Right);
            timestamp.VerticalAlignment(VerticalAlignment::Bottom);
            timestamp.Text(to_hstring(__TIME__) + L", " + to_hstring(__DATE__));
            //timestamp.Margin(Thickness(0, 0, 5, 0));

            Grid::SetRow(timestamp, RootGrid().RowDefinitions().GetView().Size() - 1);
            RootGrid().Children().Append(timestamp);
        #endif // _DEBUG

            appWindow.Title(Application::Current().Resources().Lookup(box_value(L"AppTitle")).as<hstring>());

            appWindowClosingEventToken = appWindow.Closing({ this, &MainWindow::AppWindow_Closing });
            appWindow.Changed([this](auto&&, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs args)
            {
                OverlappedPresenter presenter = nullptr;
                if ((presenter = appWindow.Presenter().try_as<OverlappedPresenter>()))
                {
                    if (args.DidSizeChange())
                    {
                        int32_t minimum = presenter.IsMinimizable() ? 250 : 130;
                        if (appWindow.Size().Width < minimum)
                        {
                            appWindow.Resize(SizeInt32(minimum, appWindow.Size().Height));
                        }
                    }

                    if (presenter.State() != OverlappedPresenterState::Maximized)
                    {
                        if (args.DidPositionChange())
                        {
                            displayRect.X = appWindow.Position().X;
                            displayRect.Y = appWindow.Position().Y;
                        }

                        if (args.DidSizeChange())
                        {
                            displayRect.Width = appWindow.Size().Width;
                            displayRect.Height = appWindow.Size().Height;
                        }
                    }
                }
            });

            if (appWindow.TitleBar().IsCustomizationSupported())
            {
                usingCustomTitleBar = true;

                appWindow.TitleBar().ExtendsContentIntoTitleBar(true);
                appWindow.TitleBar().IconShowOptions(IconShowOptions::HideIconAndSystemMenu);

                LeftPaddingColumn().Width(GridLengthHelper::FromPixels(static_cast<double>(appWindow.TitleBar().LeftInset())));

                appWindow.TitleBar().ButtonBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonInactiveForegroundColor(
                    Application::Current().Resources().TryLookup(box_value(L"AppTitleBarHoverColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverBackgroundColor(
                    Application::Current().Resources().TryLookup(box_value(L"ButtonHoverBackgroundColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());
            }

            LoadSettings();
        }

        SetBackground();
        LoadHotKeys();
    }

    void MainWindow::SetBackground()
    {
        if (DesktopAcrylicController::IsSupported())
        {
            auto&& supportsBackdrop = try_as<ICompositionSupportsSystemBackdrop>();
            if (supportsBackdrop)
            {
                RootGrid().Background(SolidColorBrush(Colors::Transparent()));
                if (!DispatcherQueue::GetForCurrentThread() && !dispatcherQueueController)
                {
                    DispatcherQueueOptions options
                    {
                        sizeof(DispatcherQueueOptions),
                        DQTYPE_THREAD_CURRENT,
                        DQTAT_COM_NONE
                    };

                    ABI::Windows::System::IDispatcherQueueController* ptr{ nullptr };
                    check_hresult(CreateDispatcherQueueController(options, &ptr));
                    dispatcherQueueController = DispatcherQueueController(ptr, take_ownership_from_abi);
                }

                systemBackdropConfiguration = SystemBackdropConfiguration();
                systemBackdropConfiguration.IsInputActive(true);
                systemBackdropConfiguration.Theme((SystemBackdropTheme)RootGrid().ActualTheme());

                backdropController = BackdropController();
                backdropController.TintColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                backdropController.FallbackColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                backdropController.TintOpacity(static_cast<float>(Application::Current().Resources().TryLookup(box_value(L"BackdropTintOpacity")).as<double>()));
                backdropController.LuminosityOpacity(static_cast<float>(Application::Current().Resources().TryLookup(box_value(L"BackdropLuminosityOpacity")).as<double>()));
                backdropController.SetSystemBackdropConfiguration(systemBackdropConfiguration);
                backdropController.AddSystemBackdropTarget(supportsBackdrop);
            }
        }
    }

    void MainWindow::LoadContent()
    {
        ResourceLoader loader{};
        GUID appID{};
        if (SUCCEEDED(UuidCreate(&appID)))
        {
            try
            {
                // Create and setup audio interfaces.
                audioController = new LegacyAudioController(appID);

                if (audioController->Register())
                {
                    audioController->SessionAdded({ this, &MainWindow::AudioController_SessionAdded });
                    audioController->EndpointChanged({ this, &MainWindow::AudioController_EndpointChanged });
                }
                else
                {
                    WindowMessageBar().EnqueueMessage(loader.GetString(L"ErrorAudioSessionsUnavailable"));
                }


                mainAudioEndpoint = audioController->GetMainAudioEndpoint();
                if (mainAudioEndpoint->Register())
                {
                    mainAudioEndpointVolumeChangedToken = mainAudioEndpoint->VolumeChanged({ this, &MainWindow::MainAudioEndpoint_VolumeChanged });
                    mainAudioEndpointStateChangedToken = mainAudioEndpoint->StateChanged([this](IInspectable, bool muted)
                    {
                        DispatcherQueue().TryEnqueue([this, muted]()
                        {
                            MuteToggleButton().IsChecked(mainAudioEndpoint->Muted());
                            MuteToggleButtonFontIcon().Glyph(muted ? L"\ue74f" : L"\ue767");
                        });
                    });
                }

                audioSessions = unique_ptr<vector<AudioSession*>>(audioController->GetSessions());
                for (size_t i = 0; i < audioSessions->size(); i++)
                {
                    CreateAudioView(audioSessions->at(i), true);
                }


                // Create and setup peak meters timers
                audioSessionsPeakTimer = DispatcherQueue().CreateTimer();
                mainAudioEndpointPeakTimer = DispatcherQueue().CreateTimer();

                audioSessionsPeakTimer.Interval(TimeSpan(std::chrono::milliseconds(83)));
                audioSessionsPeakTimer.Tick({ this, &MainWindow::UpdatePeakMeters });

                mainAudioEndpointPeakTimer.Interval(TimeSpan(std::chrono::milliseconds(83)));
                mainAudioEndpointPeakTimer.Tick([&](auto, auto)
                {
                    if (!loaded)
                    {
                        return;
                    }

                    try
                    {
                        float volume = mainAudioEndpoint->GetPeak();
                        LeftVolumeAnimation().To(static_cast<double>(volume));
                        VolumeStoryboard().Begin();
                    }
                    catch (const hresult_error&)
                    {
                        // TODO: Handle error.
                    }
                });

                MainEndpointNameTextBlock().Text(mainAudioEndpoint->Name());
                SystemVolumeSlider().Value(static_cast<double>(mainAudioEndpoint->Volume()) * 100.);
                MuteToggleButton().IsChecked(mainAudioEndpoint->Muted());
                MuteToggleButtonFontIcon().Glyph(mainAudioEndpoint->Muted() ? L"\ue74f" : L"\ue767");
            }
            catch (const hresult_error& ex)
            {
                WindowMessageBar().EnqueueMessage(loader.GetString(L"ErrorFatalFailure"));
            }
        }
        else
        {
            WindowMessageBar().EnqueueMessage(loader.GetString(L"ErrorFatalFailure"));
        }
    }

    void MainWindow::CreateAudioView(AudioSession* audioSession, bool enabled)
    {
        if (!audioSession->Register())
        {
            OutputDebugHString(L"Failed to register audio session '" + audioSession->Name() + L"'.");
            enabled = false;
        }

        // Check for duplicates, multiple audio sessions might be grouped under one by the app/system owning the sessions.
        {
            unique_lock lock{ audioSessionsMutex };

            char hitcount = 0;
            for (size_t j = 0; j < audioSessions->size(); j++)
            {
                if (audioSessions->at(j)->GroupingParam() == audioSession->GroupingParam())
                {
                    if (hitcount > 1)
                    {
                        return;
                    }
                    hitcount++;
                }
            }
        }

        audioSessionVolumeChanged.insert({ audioSession->Id(), audioSession->VolumeChanged({ this, &MainWindow::AudioSession_VolumeChanged }) });
        audioSessionsStateChanged.insert({ audioSession->Id(), audioSession->StateChanged({ this, &MainWindow::AudioSession_StateChanged }) });

        AudioSessionView view{ audioSession->Name(), audioSession->Volume() * 100.0 };
        view.Id(guid(audioSession->Id()));
        view.Muted(audioSession->Muted());
        view.SetState((AudioSessionState)audioSession->State());
        view.IsEnabled(enabled);

        view.VolumeChanged({ this, &MainWindow::AudioSessionView_VolumeChanged });
        view.VolumeStateChanged({ this, &MainWindow::AudioSessionView_VolumeStateChanged });
        view.Hidden([this](AudioSessionView sender, auto)
        {
        #ifdef DEBUG
            
        #else
            for (auto const& view : audioSessionViews)
            {
                if (view.Id() == sender.Id())
                {
                    uint32_t indexOf = 0;
                    if (audioSessionViews.IndexOf(view, indexOf))
                    {
                        audioSessionViews.RemoveAt(indexOf);
                    }

                    return;
                }
            }
        #endif // DEBUG

        });

        audioSessionViews.InsertAt(0, view);
    }

    void MainWindow::SetDragRectangles()
    {
        RectInt32 dragRectangle{};

        dragRectangle.X = static_cast<int32_t>(LeftPaddingColumn().ActualWidth() + SettingsButtonColumn().ActualWidth());
        dragRectangle.Y = 0;
        dragRectangle.Width = static_cast<int32_t>(TitleBarGrid().ActualWidth() - (LeftPaddingColumn().ActualWidth() + SettingsButtonColumn().ActualWidth()));
        dragRectangle.Height = static_cast<int32_t>(TitleBarGrid().ActualHeight());

        appWindow.TitleBar().SetDragRectangles(vector<RectInt32>{ dragRectangle });
    }

    void MainWindow::SaveAudioLevels()
    {
        ApplicationDataContainer audioLevels = ApplicationData::Current().LocalSettings().CreateContainer(
            L"AudioLevels",
            ApplicationData::Current().LocalSettings().Containers().HasKey(L"AudioLevels") ? ApplicationDataCreateDisposition::Existing : ApplicationDataCreateDisposition::Always
        );

        // TODO: Check if i need to take the mutex or not.
        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            ApplicationDataCompositeValue compositeValue{};
            compositeValue.Insert(L"Muted", box_value(audioSessions->at(i)->Muted()));
            compositeValue.Insert(L"Level", box_value(audioSessions->at(i)->Volume()));
            audioLevels.Values().Insert(audioSessions->at(i)->Name(), compositeValue);
        }
    }

    void MainWindow::LoadHotKeys()
    {
        System::HotKeyManager manager = System::HotKeyManager::GetHotKeyManager();

        #if ENABLE_HOTKEYS
        #pragma warning(push)
        #pragma warning(disable:4305)
        #pragma warning(disable:4244)
        /*
        * Currently used hotkeys:
        *  - Control + Shift + Up : system volume up
        *  - Control + Shift + Down : system volume down
        *  - Control + Shift + PageUp : system volume big up
        *  - Control + Shift + PageDown : system volume big down
        *  - Alt/Menu + Shift + M : system volume mute/unmute
        */

        volumeUpHotKeyPtr.Fired([this](auto, auto)
        {
            try
            {
                mainAudioEndpoint->SetVolume(mainAudioEndpoint->Volume() + .02f < 1.f ? mainAudioEndpoint->Volume() + 0.02f : 1.f);
            }
            catch (...)
            {
            }
        });

        volumeDownHotKeyPtr.Fired([this](auto, auto)
        {
            try
            {
                mainAudioEndpoint->SetVolume(mainAudioEndpoint->Volume() - 0.02f > 0.f ? mainAudioEndpoint->Volume() - 0.02f : 0.f);
            }
            catch (...)
            {
            }
        });

        volumePageUpHotKeyPtr.Fired([this](auto, auto)
        {
            try
            {
                constexpr float stepping = 0.07f;
                mainAudioEndpoint->SetVolume(mainAudioEndpoint->Volume() + stepping < 1.f ? mainAudioEndpoint->Volume() + stepping : 1.f);
            }
            catch (...)
            {
            }
        });

        volumePageDownHotKeyPtr.Fired([this](auto, auto)
        {
            try
            {
                constexpr float stepping = 0.07f;
                mainAudioEndpoint->SetVolume(mainAudioEndpoint->Volume() - stepping > 0.f ? mainAudioEndpoint->Volume() - stepping : 0.f);
            }
            catch (...)
            {
            }
        });

        muteHotKeyPtr.Fired([this](auto, auto)
        {
            try
            {
                mainAudioEndpoint->SetMute(!mainAudioEndpoint->Muted());
                DispatcherQueue().TryEnqueue([this]()
                {
                    MuteToggleButtonFontIcon().Glyph(mainAudioEndpoint->Muted() ? L"\ue74f" : L"\ue767");
                MuteToggleButton().IsChecked(IReference(mainAudioEndpoint->Muted()));
                });
            }
            catch (...)
            {
            }
        });

        #pragma warning(pop)  
        #endif // DEBUG

    }

    void MainWindow::LoadSettings()
    {
        IPropertySet settings = ApplicationData::Current().LocalSettings().Values();

        RectInt32 rect{};
        rect.Width = unbox_value_or(
            settings.TryLookup(L"WindowWidth"),
            Application::Current().Resources().Lookup(box_value(L"WindowWidth")).as<int32_t>()
        );
        rect.Height = unbox_value_or(
            settings.TryLookup(L"WindowHeight"),
            Application::Current().Resources().Lookup(box_value(L"WindowHeight")).as<int32_t>()
        );
        rect.X = unbox_value_or(settings.TryLookup(L"WindowPosX"), -1);
        rect.Y = unbox_value_or(settings.TryLookup(L"WindowPosY"), -1);
        DisplayArea display = DisplayArea::GetFromPoint(PointInt32(rect.X, rect.Y), DisplayAreaFallback::None);
        if (!display || rect.X < 0 || rect.Y < 0)
        {
            display = DisplayArea::GetFromPoint(PointInt32(rect.X, rect.Y), DisplayAreaFallback::Primary);
            rect.X = display.WorkArea().X + 10;
            rect.Y = display.WorkArea().X + 10;
        }
        appWindow.MoveAndResize(rect);


        auto&& presenter = appWindow.Presenter().as<OverlappedPresenter>();

        bool alwaysOnTop = unbox_value_or(settings.TryLookup(L"IsAlwaysOnTop"), true);
        bool additionalButtons = unbox_value_or(settings.TryLookup(L"ShowAdditionalWindowButtons"), false);
        OverlappedPresenterState presenterState = static_cast<OverlappedPresenterState>(unbox_value_or(settings.TryLookup(L"PresenterState"), 2));
        switch (presenterState)
        {
            case OverlappedPresenterState::Maximized:
                presenter.Maximize();
                break;
            case OverlappedPresenterState::Minimized:
                presenter.Minimize();
                break;
            /*case OverlappedPresenterState::Restored:
            default:
                break;*/
        }

        presenter.IsMaximizable(additionalButtons);
        presenter.IsMinimizable(additionalButtons);
        RightPaddingColumn().Width(GridLengthHelper::FromPixels(
            additionalButtons ? 135 : 45
        ));
        presenter.IsAlwaysOnTop(alwaysOnTop);

        layout = unbox_value_or(settings.TryLookup(L"SessionsLayout"), 0);
        switch (layout)
        {
            case 1:
                HorizontalViewMenuFlyoutItem_Click(nullptr, nullptr);
                break;
            case 2:
                VerticalViewMenuFlyoutItem_Click(nullptr, nullptr);
                break;
            /*case 0:
            default:
                break;*/
        }

        SwitchPresenterStyleMenuFlyoutItem().IsChecked(additionalButtons);
        KeepOnTopToggleMenuFlyoutItem().IsChecked(alwaysOnTop);
        DisableAnimationsMenuFlyoutItem().IsChecked(unbox_value_or(settings.TryLookup(L"DisableAnimations"), false));
        bool showMenu = unbox_value_or(settings.TryLookup(L"ShowAppBar"), false);
        ShowAppBarMenuFlyoutItem().IsChecked(showMenu);
        if (showMenu)
        {
            AppBarGrid().Visibility(Visibility::Visible);
        }
    }

    void MainWindow::SaveSettings()
    {
        IPropertySet settings = ApplicationData::Current().LocalSettings().Values();
        settings.Insert(L"WindowHeight", box_value(displayRect.Height));
        settings.Insert(L"WindowWidth", box_value(displayRect.Width));
        settings.Insert(L"WindowPosX", box_value(displayRect.X));
        settings.Insert(L"WindowPosY", box_value(displayRect.Y));

        auto presenter = appWindow.Presenter().as<OverlappedPresenter>();
        settings.Insert(L"IsAlwaysOnTop", box_value(presenter.IsAlwaysOnTop()));
        settings.Insert(L"ShowAdditionalWindowButtons", box_value(presenter.IsMinimizable()));
        settings.Insert(L"PresenterState", box_value(static_cast<int32_t>(presenter.State())));

        settings.Insert(L"DisableAnimations", box_value(DisableAnimationsMenuFlyoutItem().IsChecked()));
        settings.Insert(L"SessionsLayout", IReference<int32_t>(layout));
        settings.Insert(L"ShowAppBar", box_value(ShowAppBarMenuFlyoutItem().IsChecked()));
    }

    void MainWindow::LoadProfile(const hstring& profileName)
    {
        ApplicationDataContainer audioProfilesContainer = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"AudioProfiles");
        if (audioProfilesContainer)
        {
            for (auto&& profile : audioProfilesContainer.Containers())
            {
                try
                {
                    hstring name = profile.Key();
                    
                    if (name == profileName)
                    {
                        AudioProfile audioProfile{};
                        audioProfile.Restore(profile.Value()); // If the restore fails, the next instructions will not be ran.

                        float systemVolume = audioProfile.SystemVolume();
                        auto audioLevels = audioProfile.AudioLevels();
                        auto audioStates = audioProfile.AudioStates();
                        bool disableAnimations = audioProfile.DisableAnimations();
                        bool showAdditionalButtons = audioProfile.ShowAdditionalButtons();
                        bool keepOnTop = audioProfile.KeepOnTop();
                        bool showMenu = audioProfile.ShowMenu();
                        uint32_t windowLayout = audioProfile.Layout();

                        // Set system volume.
                        if (mainAudioEndpoint)
                        {
                            mainAudioEndpoint->SetVolume(systemVolume);
                        }
                        // Set audio sessions volume.
                        {
                            unique_lock lock{ audioSessionsMutex };

                            for (auto pair : audioLevels)
                            {
                                for (size_t i = 0; i < audioSessions->size(); i++)
                                {
                                    if (audioSessions->at(i)->Name() == pair.Key())
                                    {
                                        audioSessions->at(i)->SetVolume(pair.Value());
                                    }
                                }
                            }

                            for (auto pair : audioStates)
                            {
                                for (size_t i = 0; i < audioSessions->size(); i++)
                                {
                                    if (audioSessions->at(i)->Name() == pair.Key())
                                    {
                                        audioSessions->at(i)->SetMute(pair.Value());
                                    }
                                }
                            }
                        }

                        if (disableAnimations)
                        {
                            DisableAnimationsToggleMenuFlyoutItem_Click(nullptr, nullptr);
                        }

                        if (OverlappedPresenter presenter = appWindow.Presenter().try_as<OverlappedPresenter>())
                        {
                            presenter.IsAlwaysOnTop(keepOnTop);
                            presenter.IsMinimizable(showAdditionalButtons);
                            presenter.IsMinimizable(showAdditionalButtons);

                            RightPaddingColumn().Width(GridLengthHelper::FromPixels(
                                showAdditionalButtons ? 135 : 45
                            ));
                        }

                        if (showMenu)
                        {
                            ShowAppBarMenuFlyoutItem_Click(nullptr, nullptr);
                        }

                        switch (windowLayout)
                        {
                        case 1:
                            HorizontalViewMenuFlyoutItem_Click(nullptr, nullptr);
                            break;
                        case 2:
                            VerticalViewMenuFlyoutItem_Click(nullptr, nullptr);
                            break;
                        case 0:
                        default:
                            AutoViewMenuFlyoutItem_Click(nullptr, nullptr);
                            break;
                        }


                        WindowMessageBar().EnqueueMessage(L"Loaded profile " + profileName);
                    }

                }
                catch (const hresult_error&)
                {
                    // I18N: Failed to load profile [profile name]
                    WindowMessageBar().EnqueueMessage(L"Failed to load profile \"" + profileName + L"\"");
                }
            }
        }
    }

    void MainWindow::UpdatePeakMeters(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
    {
        if (!loaded) return;

        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            if (audioSessions->at(i))
            {
                guid id = audioSessions->at(i)->Id();

                for (auto const& view : audioSessionViews)
                {
                    if (view.Id() == id)
                    {
                        //view.SetPeak(audioSessions->at(i)->GetPeak());
                        auto&& pair = audioSessions->at(i)->GetChannelsPeak();
                        view.SetPeak(pair.first, pair.second);
                    }
                }
            }
        }
    }

    void MainWindow::AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs)
    {
        if (audioSessionsPeakTimer && audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Stop();
        }

        if (mainAudioEndpointPeakTimer && mainAudioEndpointPeakTimer.IsRunning())
        {
            mainAudioEndpointPeakTimer.Stop();
        }

        VolumeStoryboard().Stop();

        // Clean up ComPtr/IUnknown objects
        if (audioController)
        {
            if (mainAudioEndpoint)
            {
                mainAudioEndpoint->VolumeChanged(mainAudioEndpointVolumeChangedToken);
                mainAudioEndpoint->Unregister();
                mainAudioEndpoint->Release();
            }

            audioController->EndpointChanged(audioControllerEndpointChangedToken);
            audioController->SessionAdded(audioControllerSessionAddedToken);

            audioController->Unregister();
            audioController->Release();
        }

        // Unregister VolumeChanged event handler & unregister audio sessions from audio events and release com ptrs.
        if (audioSessions.get())
        {
            unique_lock lock{ audioSessionsMutex };

            SaveAudioLevels();

            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                audioSessions->at(i)->VolumeChanged(audioSessionVolumeChanged[audioSessions->at(i)->Id()]);
                audioSessions->at(i)->Unregister();
                audioSessions->at(i)->Release();
            }
        }

        SaveSettings();
    }

    void MainWindow::MainAudioEndpoint_VolumeChanged(winrt::Windows::Foundation::IInspectable, const float& newVolume)
    {
        if (!loaded) return;

        DispatcherQueue().TryEnqueue([this, newVolume]()
        {
            SystemVolumeSlider().Value(newVolume * 100.);
        });
    }

    void MainWindow::AudioSession_VolumeChanged(const winrt::guid& id, const float& newVolume)
    {
        if (!loaded) return;

        DispatcherQueue().TryEnqueue([this, id, newVolume]()
        {
            for (auto const& view : audioSessionViews)
            {
                if (view.Id() == id)
                {
                    view.Volume(static_cast<double>(newVolume) * 100.0);
                }
            }
        });
    }

    void MainWindow::AudioSession_StateChanged(const winrt::guid& id, const uint32_t& state)
    {
        if (!loaded) return;

        // Cast state to AudioSessionState, uint32_t is only used to cross ABI
        AudioSessionState audioState = (AudioSessionState)state;

        if (audioState == AudioSessionState::Expired)
        {
            unique_lock lock{ audioSessionsMutex };

            audioSessionsPeakTimer.Stop();

            GUID sessionID = id;
            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                if (audioSessions->at(i)->Id() == sessionID)
                {
                    AudioSession* session = audioSessions->at(i);
                    session->Unregister();
                    session->Release();

                    audioSessions->erase(audioSessions->begin() + i);
                }
            }

            audioSessionsPeakTimer.Start();
        }

        DispatcherQueue().TryEnqueue([this, id, audioState]()
        {
            for (auto const& view : audioSessionViews)
            {
                if (view.Id() == id)
                {
                    switch (audioState)
                    {
                        case AudioSessionState::Muted:
                            view.Muted(true);
                            break;

                        case AudioSessionState::Unmuted:
                            view.Muted(false);
                            break;

                        case AudioSessionState::Active:
                        case AudioSessionState::Inactive:
                            view.SetState(audioState);
                            break;

                        case AudioSessionState::Expired:
                            uint32_t indexOf = 0;
                            if (audioSessionViews.IndexOf(view, indexOf))
                            {
                                audioSessionViews.RemoveAt(indexOf);
                            }
                            break;
                    }

                    return;
                }
            }
        });
    }

    void MainWindow::AudioController_SessionAdded(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
    {
        DispatcherQueue().TryEnqueue([this]()
        {
            audioSessionsPeakTimer.Stop();
        while (AudioSession* newSession = audioController->NewSession())
        {
            {
                unique_lock lock{ audioSessionsMutex };
                audioSessions->push_back(newSession);
            }
            CreateAudioView(newSession, true);
        }
        audioSessionsPeakTimer.Start();
        });
    }

    void MainWindow::AudioController_EndpointChanged(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
    {
        mainAudioEndpoint->VolumeChanged(mainAudioEndpointVolumeChangedToken);
        mainAudioEndpoint->StateChanged(mainAudioEndpointStateChangedToken);
        mainAudioEndpoint->Unregister();
        mainAudioEndpoint->Release();

        mainAudioEndpoint = audioController->GetMainAudioEndpoint();
        if (mainAudioEndpoint->Register())
        {
            mainAudioEndpointVolumeChangedToken = mainAudioEndpoint->VolumeChanged({ this, &MainWindow::MainAudioEndpoint_VolumeChanged });
            mainAudioEndpointStateChangedToken = mainAudioEndpoint->StateChanged([this](IInspectable, bool muted)
            {
                DispatcherQueue().TryEnqueue([this, muted]()
                {
                    MuteToggleButton().IsChecked(mainAudioEndpoint->Muted());
                    MuteToggleButtonFontIcon().Glyph(muted ? L"\ue74f" : L"\ue767");
                });
            });
        }

        DispatcherQueue().TryEnqueue([&]()
        {
            MainEndpointNameTextBlock().Text(mainAudioEndpoint->Name());
            SystemVolumeSlider().Value(static_cast<double>(mainAudioEndpoint->Volume()) * 100.);
            MuteToggleButton().IsChecked(mainAudioEndpoint->Muted());

            unique_lock lock{ audioSessionsMutex };

            for (auto const& item : AudioSessionsPanel().Items())
            {
                AudioSessionView view = item.as<AudioSessionView>();

                for (size_t i = 0; i < audioSessions->size(); i++)
                {
                    guid id = audioSessions->at(i)->Id();

                    if (view.Id() == id)
                    {
                        audioSessions->at(i)->Volume(view.Volume() / 100.f);
                    }
                }
            }
        });
    }
}
