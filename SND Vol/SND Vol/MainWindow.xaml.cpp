#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "HotKey.h"
#include "SecondWindow.xaml.h"

#define USE_TIMER 1
#define DEACTIVATE_TIMER 0
#define ENABLE_HOTKEYS 1

using namespace Audio;

using namespace std;

using namespace winrt;

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Windowing;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        InitWindow();
        LoadHotKeys();
        SetBackground();

    #ifdef DEBUG
        Application::Current().UnhandledException([&](IInspectable const&/*sender*/, UnhandledExceptionEventArgs const& e)
        {
            TextBlock block{};
            block.TextWrapping(TextWrapping::Wrap);
            block.Text(e.Message());
            WindowMessageBar().EnqueueMessage(e.Message());
        });
    #else
        HotKeySettingsMenuFlyoutItem().IsEnabled(false);
    #endif // DEBUG
    }

    MainWindow::~MainWindow()
    {
    }

    #pragma region Event handlers
    void MainWindow::OnLoaded(IInspectable const&, RoutedEventArgs const&)
    {
        loaded = true;

    #if USE_TIMER
        if (audioSessions.get())
        {
            audioSessionsPeakTimer.Start();
            mainAudioEndpointPeakTimer.Start();
            VolumeStoryboard().Begin();

            WindowMessageBar().EnqueueMessage(L"Sessions loaded.");
        }
    #endif // USE_TIMER

        // Generate size changed event to get correct clipping rectangle size
        SystemVolumeActivityBorder_SizeChanged(nullptr, nullptr);
        Grid_SizeChanged(nullptr, nullptr);

        // Teaching tips
        SettingsButtonTeachingTip().Target(SettingsButton());
        if (unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"ShowSettingsTeachingTip"), true))
        {
            ApplicationData::Current().LocalSettings().Values().Insert(L"ShowSettingsTeachingTip", IReference(false));
            SettingsButtonTeachingTip().IsOpen(true);
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
            if (id == sender.Id())
            {
                audioSessions->at(i)->SetMute(args);
            }
        }
    }

    void MainWindow::AudioSessionsPanel_Loading(FrameworkElement const&, IInspectable const&)
    {
        LoadContent();

        // Activate hotkeys.
        volumeUpHotKeyPtr.Activate();
        volumeDownHotKeyPtr.Activate();
        volumePageUpHotKeyPtr.Activate();
        volumePageDownHotKeyPtr.Activate();
        muteHotKeyPtr.Activate();
    }

    void MainWindow::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        SetDragRectangles();

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

    void MainWindow::SystemVolumeSlider_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        if (mainAudioEndpoint)
        {
            mainAudioEndpoint->Volume(static_cast<float>(e.NewValue() / 100.));
        }

        SystemVolumeSliderText().Text(to_hstring(round(e.NewValue())));
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

    void MainWindow::SystemVolumeActivityBorder_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        SystemVolumeActivityBorderClipping().Rect(
            Rect(0, 0, static_cast<float>(SystemVolumeActivityBorder().ActualWidth()), static_cast<float>(SystemVolumeActivityBorder().ActualHeight()))
        );
    }

    void MainWindow::HideMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        appWindow.Presenter().as<OverlappedPresenter>().Minimize();
    }

    void MainWindow::RestartMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        // TODO: Restart app with restart API
        if (Microsoft::Windows::AppLifecycle::AppInstance::Restart(L"") != AppRestartFailureReason::RestartPending)
        {
            // I18N:
            WindowMessageBar().EnqueueMessage(L"Could not restart. Close the window.");
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
                session->SetMute(setMute);
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
    }

    void MainWindow::VerticalViewMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AudioSessionsScrollViewer().HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
        AudioSessionsScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
        AudioSessionsPanel().Style(
            RootGrid().Resources().Lookup(box_value(L"GridViewVerticalLayout")).as<Style>()
        );
    }

    void MainWindow::AutoViewMenuFlyoutItem_Click(IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        AudioSessionsScrollViewer().HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
        AudioSessionsScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Auto);

        AudioSessionsPanel().Style(
            RootGrid().Resources().Lookup(box_value(L"GridViewHorizontalLayout")).as<Style>()
        );
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

        // I18N:
        WindowMessageBar().EnqueueMessage(L"Reloading audio sessions...");

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
        GUID appID{};
        if (SUCCEEDED(UuidCreate(&appID)))
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
                // I18N:
                // ------------------------------------------------------------------------------------------------------------------------------------

                /*WindowInfoBar().Title(L"Audio sessions notifications unavailable");
                WindowInfoBar().Severity(Controls::InfoBarSeverity::Warning);
                WindowInfoBar().IsOpen(true);*/
                WindowMessageBar().EnqueueMessage(L"Audio sessions notifications unavailable");
            }


            mainAudioEndpoint = audioController->GetMainAudioEndpoint();
            if (mainAudioEndpoint->Register())
            {
                mainAudioEndpointVolumeChangedToken = mainAudioEndpoint->VolumeChanged({ this, &MainWindow::MainAudioEndpoint_VolumeChanged });
            }

            MainEndpointNameTextBlock().Text(mainAudioEndpoint->Name());
            SystemVolumeSlider().Value(static_cast<double>(mainAudioEndpoint->Volume()) * 100.);


            audioSessions = unique_ptr<vector<AudioSession*>>(audioController->GetSessions());
            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                CreateAudioView(audioSessions->at(i), true);
            }

            // I18N:
            // ------------------------------------------------------------------------------------------------------------------------------------

            /*WindowInfoBar().Content(nullptr);
            WindowInfoBar().Title(L"Reloaded");
            WindowInfoBar().IsClosable(true);*/
            WindowMessageBar().EnqueueMessage(L"Reloaded");
        }
        else
        {
            // I18N:
        // ------------------------------------------------------------------------------------------------------------------------------------

            /*WindowInfoBar().Title(L"Hard failure, app needs to restart.");
            WindowInfoBar().IsOpen(true);*/
            WindowMessageBar().EnqueueMessage(L"Hard failure, app needs to restart.");
        }
    }

    void MainWindow::KeepOnTopToggleMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        bool alwaysOnTop = KeepOnTopToggleMenuFlyoutItem().IsChecked();
        appWindow.Presenter().as<OverlappedPresenter>().IsAlwaysOnTop(alwaysOnTop);
        ApplicationData::Current().LocalSettings().Values().Insert(L"IsAlwaysOnTop", box_value(alwaysOnTop));
    }

    void MainWindow::HotKeySettingsMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
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


        VolumeAnimation().To(0.);
        VolumeStoryboard().Begin();

        for (auto&& view : audioSessionViews)
        {
            view.SetPeak(0, 0);
        }
    }

    void MainWindow::SwitchPresenterStyleMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        OverlappedPresenter presenter = appWindow.Presenter().as<OverlappedPresenter>();

        RightPaddingColumn().Width(GridLengthHelper::FromPixels(
            presenter.IsMinimizable() ? 45 : 135
        ));

        presenter.IsMaximizable(!presenter.IsMaximizable());
        presenter.IsMinimizable(!presenter.IsMinimizable());
    }

    void MainWindow::ViewHotKeysHyperlinkButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        HotKeySettingsMenuFlyoutItem_Click(nullptr, nullptr);
    }
    #pragma endregion


    void MainWindow::InitWindow()
    {
        auto nativeWindow{ this->try_as<::IWindowNative>() };
        check_bool(nativeWindow);

        HWND handle = nullptr;
        nativeWindow->get_WindowHandle(&handle);
        WindowId windowID = GetWindowIdFromWindow(handle);
        appWindow = AppWindow::GetFromWindowId(windowID);
        if (appWindow != nullptr)
        {
            int32_t width = unbox_value_or(
                ApplicationData::Current().LocalSettings().Values().TryLookup(L"WindowWidth"),
                Application::Current().Resources().Lookup(box_value(L"WindowWidth")).as<int32_t>()
            );
            int32_t height = unbox_value_or(
                ApplicationData::Current().LocalSettings().Values().TryLookup(L"WindowHeight"),
                Application::Current().Resources().Lookup(box_value(L"WindowHeight")).as<int32_t>()
            );

            int32_t lastX = unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"WindowPosX"), 0);
            int32_t lastY = unbox_value_or(ApplicationData::Current().LocalSettings().Values().TryLookup(L"WindowPosY"), 0);
            PointInt32 lastPosition{ lastX, lastY };

            DisplayArea display = DisplayArea::GetFromPoint(lastPosition, DisplayAreaFallback::None);
            if (!display)
            {
                lastPosition.X = display.WorkArea().Width - width;
                lastPosition.Y = display.WorkArea().Height - height;
            }

            appWindow.MoveAndResize(RectInt32(lastPosition.X, lastPosition.Y, width, height));


            #ifdef _DEBUG
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

            appWindowClosingEventToken = appWindow.Closing({this, &MainWindow::AppWindow_Closing});
            appWindow.Changed([this](auto&& sender, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs args)
            {
                if (args.DidSizeChange())
                {
                    int32_t minimum = appWindow.Presenter().as<OverlappedPresenter>().IsMinimizable() ? 250 : 130;
                    if (appWindow.Size().Width < minimum)
                    {
                        appWindow.Resize(SizeInt32(minimum, appWindow.Size().Height));
                    }
                }
            });

            if (appWindow.TitleBar().IsCustomizationSupported())
            {
                appWindow.TitleBar().ExtendsContentIntoTitleBar(true);
                appWindow.TitleBar().IconShowOptions(IconShowOptions::ShowIconAndSystemMenu);

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

                auto&& presenter = appWindow.Presenter().as<OverlappedPresenter>();
                presenter.IsMaximizable(false);
                presenter.IsMinimizable(false);
                bool alwaysOnTop = unbox_value_or(
                    ApplicationData::Current().LocalSettings().Values().TryLookup(L"IsAlwaysOnTop"),
                    true
                );
                presenter.IsAlwaysOnTop(alwaysOnTop);
                KeepOnTopToggleMenuFlyoutItem().IsChecked(alwaysOnTop);
            }
        }
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
                    // I18N
                    WindowMessageBar().EnqueueMessage(L"Audio sessions notifications unavailable.");
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

                audioSessionsPeakTimer.Interval(TimeSpan(std::chrono::milliseconds(166)));
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
                    VolumeAnimation().To(static_cast<double>(volume));
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
                WindowMessageBar().EnqueueMessage(L"Fatal error : " + ex.message());
            }
        }
        else
        {
            // I18N
            WindowMessageBar().EnqueueMessage(L"Hard failure, app needs to restart.");
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
            DispatcherQueue().TryEnqueue([this, id = sender.Id()]()
            {
                for (auto const& view : audioSessionViews)
                {
                    if (view.Id() == id)
                    {
                        uint32_t indexOf = 0;
                        if (audioSessionViews.IndexOf(view, indexOf))
                        {
                            audioSessionViews.RemoveAt(indexOf);
                        }
                        
                        return;
                    }
                }
            });
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
        ApplicationDataContainer audioLevels = ApplicationData::Current().LocalSettings().CreateContainer(L"AudioLevels", 
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
        /*
        * Currently used hotkeys:
        *  - Control + Shift + Up : system volume up
        *  - Control + Shift + Down : system volume down
        *  - Control + Shift + PageUp : system volume big up
        *  - Control + Shift + PageDown : system volume big down
        *  - Alt/Menu + Shift + M : system volume mute/unmute
        */

    #if ENABLE_HOTKEYS
    #pragma warning(push)
    #pragma warning(disable:4305)
    #pragma warning(disable:4244)

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
        if (audioSessionsPeakTimer.IsRunning())
        {
            audioSessionsPeakTimer.Stop();
        }

        if (mainAudioEndpointPeakTimer.IsRunning())
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

        // Save settings
        IPropertySet settings = ApplicationData::Current().LocalSettings().Values();
        settings.Insert(L"WindowHeight", box_value(appWindow.Size().Height));
        settings.Insert(L"WindowWidth", box_value(appWindow.Size().Width));
        settings.Insert(L"WindowPosX", box_value(appWindow.Position().X));
        settings.Insert(L"WindowPosY", box_value(appWindow.Position().Y));
        settings.Insert(L"IsAlwaysOnTop", box_value(appWindow.Presenter().as<OverlappedPresenter>().IsAlwaysOnTop()));
        settings.Insert(L"Layout", box_value(0));
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
            for (auto const&view : audioSessionViews)
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
                    MuteToggleButtonFontIcon().Glyph(
                        muted ? L"\ue74f" : L"\ue767");
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
