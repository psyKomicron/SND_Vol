#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <rpc.h>
#include <chrono>

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

using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    MainWindow::MainWindow()
    {
        InitializeComponent();
        InitWindow();
        SetBackground();

        timer = DispatcherQueue().CreateTimer();

#ifdef DEBUG
        Application::Current().UnhandledException([&](IInspectable const& /*sender*/, UnhandledExceptionEventArgs const& e)
        {
            TextBlock block{};
            block.TextWrapping(TextWrapping::Wrap);
            block.Text(e.Message());
            WindowInfoBar().Content(block);
            WindowInfoBar().IsOpen(true);
        });
#endif // DEBUG

    }

    void MainWindow::OnLoaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        loaded = true;
        timer.Start();
    }

    void MainWindow::AudioSessionView_VolumeChanged(AudioSessionView const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& args)
    {
        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            if (audioSessions->at(i)->Name() == sender.Header())
            {
                audioSessions->at(i)->Volume(static_cast<float>(args.NewValue() / 100.0));
            }
        }
    }

    void MainWindow::AudioSessionView_VolumeStateChanged(winrt::SND_Vol::AudioSessionView const& sender, bool const& args)
    {
        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            if (audioSessions->at(i)->Name() == sender.Header())
            {
                audioSessions->at(i)->SetMute(args);
            }
        }
    }

    void MainWindow::AudioSessionsPanel_Loading(FrameworkElement const&, IInspectable const&)
    {
        LoadContent();
    }

    void MainWindow::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
    }

    void MainWindow::SystemVolumeSlider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
    {
        if (mainAudioEndpoint.get())
        {
            mainAudioEndpoint->Volume(static_cast<float>(e.NewValue() / 100.));
        }
    }

    void MainWindow::Window_Activated(IInspectable const&, WindowActivatedEventArgs const& args)
    {
        #if FALSE
        if (args.WindowActivationState() == WindowActivationState::Deactivated && timer.IsRunning())
        {
            timer.Stop();
        }
        else if (!timer.IsRunning())
        {
            timer.Start();
        }
        #endif
    }


    void MainWindow::LoadContent()
    {
        timer.Interval(TimeSpan(std::chrono::milliseconds(1)));
        timer.Tick({ this, &MainWindow::UpdatePeakMeters });

        GUID appID{};
        if (SUCCEEDED(UuidCreate(&appID)))
        {
            audioController = make_unique<LegacyAudioController>(appID);

            if (audioController->Register())
            {
                audioController->SessionAdded([this](auto /*sender*/, auto /*args*/)
                {
                    DispatcherQueue().TryEnqueue([this]()
                    {
                        while (AudioSession* newSession = audioController->NewSession())
                        {
                            audioSessions->push_back(AudioSessionContainer(newSession));
                            CreateAudioView(newSession, false);
                        }
                    });
                });

                audioController->EndpointChanged([&](auto /*sender*/, auto /*args*/) noexcept
                {
                    OutputDebugHString(mainAudioEndpoint->Name());

                    #if TRUE
                    MainAudioEndpoint* mainAudioEndpointPtr = mainAudioEndpoint.release();
                    mainAudioEndpointPtr->VolumeChanged(mainAudioEndpointVolumeChangedToken);
                    mainAudioEndpointPtr->Unregister();
                    mainAudioEndpointPtr->Release();

                    mainAudioEndpoint = unique_ptr<MainAudioEndpoint>(audioController->GetMainAudioEndpoint());;
                    if (mainAudioEndpoint->Register())
                    {
                        mainAudioEndpointVolumeChangedToken = mainAudioEndpoint->VolumeChanged({ this, &MainWindow::MainAudioEndpoint_VolumeChanged });
                    }

                    DispatcherQueue().TryEnqueue([&]()
                    {
                        MainEndpointNameTextBlock().Text(mainAudioEndpoint->Name());
                        SystemVolumeSlider().Value(static_cast<double>(mainAudioEndpoint->Volume()) * 100.);

                        auto children = AudioSessionsPanel().Items();
                        for (auto const& uiElement : children)
                        {
                            AudioSessionView view = uiElement.as<AudioSessionView>();

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
                    #endif // FALSE
                });

            }
            else
            {
                // I18N
                WindowInfoBar().Title(L"Audio sessions notifications unavailable");
                WindowInfoBar().Severity(Controls::InfoBarSeverity::Warning);
                WindowInfoBar().IsOpen(true);
            }
            

            mainAudioEndpoint = unique_ptr<MainAudioEndpoint>(audioController->GetMainAudioEndpoint());
            if (mainAudioEndpoint->Register())
            {
                mainAudioEndpointVolumeChangedToken = mainAudioEndpoint->VolumeChanged({ this, &MainWindow::MainAudioEndpoint_VolumeChanged });
            }

            MainEndpointNameTextBlock().Text(mainAudioEndpoint->Name());
            SystemVolumeSlider().Value(static_cast<double>(mainAudioEndpoint->Volume()) * 100.);


            audioSessions = unique_ptr<vector<AudioSessionContainer>>(audioController->GetSessions());
            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                CreateAudioView(audioSessions->at(i).get(), true);
            }

            // Get audio meter.
            audioMeterInfo = IAudioMeterInformationPtr(mainAudioEndpoint->GetEndpointMeterInfo());
            if (!audioMeterInfo.GetInterfacePtr())
            {
                OutputDebugString(L"\nFailed to create IAudioMeterInformation object.\n");
            }
        }
        //TODO: if the GUID cannot be created, try another time. Show error message if still no GUID.
    }

    void MainWindow::CreateAudioView(AudioSession* audioSession, bool enabled)
    {
        if (!audioSession->Register())
        {
            OutputDebugHString(L"Failed to register audio session '" + audioSession->Name() + L"'");
            enabled = false;
        }

        // Check for duplicates, multiple audio sessions might be grouped under one by the app/system owning the sessions.
        int16_t duplicate = -1;
        for (size_t j = 0; j < audioSessions->size(); j++)
        {
            if (audioSessions->at(j)->GroupingParam() == audioSession->GroupingParam())
            {
                duplicate++;
            }
        }

        audioSessionVolumeChanged.push_back(
            audioSession->VolumeChanged({ this, &MainWindow::AudioSession_VolumeChanged })
        );
        audioSessionsStateChanged.push_back(
            audioSession->StateChanged({ this, &MainWindow::AudioSession_StateChanged })
        );

        if (duplicate > 0)
        {
            bool alreadyAdded = false;
            for (auto&& children : AudioSessionsPanel().Items())
            {
                AudioSessionView audioSessionView = children.try_as<AudioSessionView>();
                if (audioSessionView && audioSessionView.Header() == audioSession->Name())
                {
                    alreadyAdded = true;
                    break;
                }
            }
            if (alreadyAdded)
            {
                return;
            }
        }


        AudioSessionView view = AudioSessionView(audioSession->Name(), audioSession->Volume() * 100.0);
        view.Id(guid(audioSession->Id()));
        view.Muted(audioSession->Muted());
        view.SetStatus((AudioSessionState)audioSession->State());

        volumeChangedRevokers.push_back(view.VolumeChanged(auto_revoke, { this, &MainWindow::AudioSessionView_VolumeChanged }));
        volumeStateChangedRevokers.push_back(view.VolumeStateChanged(auto_revoke, { this, &MainWindow::AudioSessionView_VolumeStateChanged }));

        AudioSessionsPanel().Items().Append(view);
    }

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
            timestamp.Margin(Thickness(0, 0, 5, 0));

            Grid::SetRow(timestamp, RootGrid().RowDefinitions().GetView().Size() - 1);
            RootGrid().Children().Append(timestamp);
#endif // _DEBUG

            appWindow.Title(Application::Current().Resources().Lookup(box_value(L"AppTitle")).as<hstring>());

            appWindowClosingEventToken = appWindow.Closing({this, &MainWindow::AppWindow_Closing});

            if (appWindow.TitleBar().IsCustomizationSupported())
            {
                appWindow.TitleBar().ExtendsContentIntoTitleBar(true);
                appWindow.TitleBar().IconShowOptions(IconShowOptions::HideIconAndSystemMenu);

                appWindow.TitleBar().ButtonBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonInactiveBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonInactiveForegroundColor(Application::Current().Resources().TryLookup(box_value(L"AppTitleBarHoverColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverBackgroundColor(Application::Current().Resources().TryLookup(box_value(L"ButtonHoverBackgroundColor")).as<Windows::UI::Color>());
                appWindow.TitleBar().ButtonHoverForegroundColor(Colors::White());
                appWindow.TitleBar().ButtonPressedBackgroundColor(Colors::Transparent());
                appWindow.TitleBar().ButtonPressedForegroundColor(Colors::White());

                auto&& presenter = appWindow.Presenter().as<OverlappedPresenter>();
                presenter.IsMaximizable(false);
                presenter.IsMinimizable(false);
                presenter.IsAlwaysOnTop(true);
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

    void MainWindow::AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs)
    {
        if (timer.IsRunning())
        {
            timer.Stop();
        }

        if (audioController.get())
        {
            if (mainAudioEndpoint.get())
            {
                mainAudioEndpoint->VolumeChanged(mainAudioEndpointVolumeChangedToken);
                mainAudioEndpoint->Unregister();
                mainAudioEndpoint->Release();
            }

            audioController->Unregister();
            audioController->Release();
        }

        // Unregister VolumeChanged event handler.
        for (size_t i = 0; i < audioSessionVolumeChanged.size(); i++)
        {
            audioSessions->at(i)->VolumeChanged(audioSessionVolumeChanged[i]);
        }

        // Unregister audio sessions from audio events and release com ptrs.
        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            audioSessions->at(i)->Unregister();
            audioSessions->at(i)->Release();
        }

        ApplicationData::Current().LocalSettings().Values().Insert(L"WindowHeight", box_value(appWindow.Size().Height));
        ApplicationData::Current().LocalSettings().Values().Insert(L"WindowWidth", box_value(appWindow.Size().Width));
        ApplicationData::Current().LocalSettings().Values().Insert(L"WindowPosX", box_value(appWindow.Position().X));
        ApplicationData::Current().LocalSettings().Values().Insert(L"WindowPosY", box_value(appWindow.Position().Y));
    }


    void MainWindow::UpdatePeakMeters(winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable)
    {
        constexpr uint32_t refreshCount = 5u;
        static uint32_t count = 0u;

        if (!loaded) return;

        if (count > refreshCount)
        {
            float volume = 0.f;
            if (SUCCEEDED(audioMeterInfo->GetPeakValue(&volume)))
            {
                SystemVolumeActivityBorder().Scale(::Numerics::float3(volume, 1, 1));
            }
            count = 0u;
        }

        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            guid id = audioSessions->at(i)->Id();
            auto children = AudioSessionsPanel().Items();
            for (auto const& uiElement : children)
            {
                AudioSessionView view = uiElement.as<AudioSessionView>();
                if (view.Id() == id)
                {
                    view.SetPeak(audioSessions->at(i)->GetPeak());
                }
            }
        }

        count++;
    }

    void MainWindow::MainAudioEndpoint_VolumeChanged(winrt::Windows::Foundation::IInspectable, const float& newVolume)
    {
        if (!loaded) return;

        DispatcherQueue().TryEnqueue([this, newVolume]()
        {
            SystemVolumeSlider().Value(newVolume * 100.);
        });
    }

    void MainWindow::AudioSession_VolumeChanged(const winrt::guid& sender, const float& newVolume)
    {
        if (!loaded) return;

        DispatcherQueue().TryEnqueue([this, id = sender, newVolume]()
        {
            auto children = AudioSessionsPanel().Items();
            for (auto const& uiElement : children)
            {
                AudioSessionView view = uiElement.as<AudioSessionView>();
                if (view.Id() == id)
                {
                    view.Volume(static_cast<double>(newVolume) * 100.0);
                }
            }
        });
    }

    void MainWindow::AudioSession_StateChanged(const winrt::guid& sender, const uint32_t& state)
    {
        if (!loaded) return;

        DispatcherQueue().TryEnqueue([this, id = sender, state]()
        {
            auto children = AudioSessionsPanel().Items();
            for (auto const& uiElement : children)
            {
                AudioSessionView view = uiElement.as<AudioSessionView>();
                if (view.Id() == id)
                {
                    AudioSessionState audioState = (AudioSessionState)state;
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
                            view.SetStatus(audioState);
                            break;
                        case AudioSessionState::Expired:
                            uint32_t indexOf = 0;
                            if (AudioSessionsPanel().Items().IndexOf(view, indexOf))
                            {
                                AudioSessionsPanel().Items().RemoveAt(indexOf);
                            }
                            break;
                    }
                }
            }
        });
    }
}
