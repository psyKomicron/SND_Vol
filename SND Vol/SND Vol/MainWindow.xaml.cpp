#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <rpc.h>

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
    }

    void MainWindow::OnLoaded(winrt::Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e)
    {
        const double padding = Application::Current().Resources().Lookup(box_value(L"WindowPadding")).as<double>();

        int32_t columnSpacing = static_cast<int32_t>(AudioSessionsPanel().ColumnSpacing()) * static_cast<int32_t>(AudioSessionsPanel().ColumnDefinitions().Size() > 1);
        int32_t width = padding/* + AudioSessionsPanel().Children().Size() * (85 + columnSpacing)*/;
        for (ColumnDefinition colDef : AudioSessionsPanel().ColumnDefinitions())
        {
            width += colDef.ActualWidth() + columnSpacing;
        }
        if (width < 140)
        {
            width = 140;
        }

        DisplayArea display = DisplayArea::GetFromPoint(PointInt32(0, 0), DisplayAreaFallback::Primary);
        int32_t height = appWindow.Size().Height;
        int32_t x = display.WorkArea().Width - width;
        int32_t y = display.WorkArea().Height - height;
        appWindow.MoveAndResize(RectInt32(x, y, width, height));

        loaded = true;
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
        if (!loaded) return;

        constexpr double minHeight = 220;
        if (ContentScrollViewer().VerticalScrollBarVisibility() == ScrollBarVisibility::Disabled && RootGrid().ActualHeight() < minHeight)
        {
            ContentScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Visible);
        }
        else if (ContentScrollViewer().VerticalScrollBarVisibility() == ScrollBarVisibility::Visible && RootGrid().ActualHeight() > minHeight)
        {
            ContentScrollViewer().VerticalScrollBarVisibility(ScrollBarVisibility::Disabled);
        }

        constexpr uint32_t arbitraryPadding = 10;
        const double padding = Application::Current().Resources().Lookup(box_value(L"WindowPadding")).as<double>();
        uint32_t columnSpacing = static_cast<int32_t>(AudioSessionsPanel().ColumnSpacing());
        uint32_t pixels = AudioSessionsPanel().Children().Size() * (85ul + columnSpacing) + padding;
        uint32_t width = appWindow.Size().Width;
        if ((width - arbitraryPadding) > pixels)
        {
            AudioSessionsPanel().HorizontalAlignment(HorizontalAlignment::Stretch);
            for (ColumnDefinition col : AudioSessionsPanel().ColumnDefinitions())
            {
                col.Width(GridLength(1, GridUnitType::Star));
            }
        }
        else if (width + arbitraryPadding < pixels)
        {
            for (ColumnDefinition col : AudioSessionsPanel().ColumnDefinitions())
            {
                col.Width(GridLength(85, GridUnitType::Pixel));
            }
            AudioSessionsPanel().HorizontalAlignment(HorizontalAlignment::Center);
        }
    }


    void MainWindow::InitWindow()
    {
        auto nativeWindow{ this->try_as<::IWindowNative>() };
        check_bool(nativeWindow);
        HWND handle{ nullptr };
        nativeWindow->get_WindowHandle(&handle);
        WindowId windowID = GetWindowIdFromWindow(handle);
        appWindow = AppWindow::GetFromWindowId(windowID);
        if (appWindow != nullptr)
        {
            const double padding = Application::Current().Resources().Lookup(box_value(L"WindowPadding")).as<double>();
            int32_t width = Application::Current().Resources().Lookup(box_value(L"WindowWidth")).as<int32_t>() + padding;
            int32_t height = unbox_value_or(
                ApplicationData::Current().LocalSettings().Values().TryLookup(L"WindowHeight"),
                Application::Current().Resources().Lookup(box_value(L"WindowHeight")).as<int32_t>()
            );
            DisplayArea display = DisplayArea::GetFromPoint(PointInt32(0, 0), DisplayAreaFallback::Primary);
            int32_t x = display.WorkArea().Width - width;
            int32_t y = display.WorkArea().Height - height;
            appWindow.MoveAndResize(RectInt32(x, y, width, height));

#ifdef _DEBUG
            TextBlock timestamp{};
            timestamp.IsHitTestVisible(false);
            timestamp.Opacity(0.6);
            timestamp.HorizontalAlignment(HorizontalAlignment::Right);
            timestamp.VerticalAlignment(VerticalAlignment::Bottom);
            timestamp.Text(to_hstring(__TIME__) + L", " + to_hstring(__DATE__));
            timestamp.Margin(Thickness(0, 0, 5, 2));

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
            }
        }
    }

    void MainWindow::LoadContent()
    {
        GUID appID{};
        if (SUCCEEDED(UuidCreate(&appID)))
        {
            audioController = make_unique<LegacyAudioController>(appID);
            audioSessions = unique_ptr<vector<AudioSessionContainer>>(audioController->GetSessions());

            if (!audioController->Register())
            {
                OutputDebugString(L"Failed to register audio controller.");
                // I18N
                WindowInfoBar().Title(L"Audio sessions notifications unavailable");
                WindowInfoBar().Severity(Controls::InfoBarSeverity::Warning);
            }
            else
            {
                WindowInfoBar().Title(L"Audio sessions notifications available");
                WindowInfoBar().Severity(Controls::InfoBarSeverity::Success);
            }

            audioController->SessionAdded([this](auto /*sender*/, auto /*args*/)
            {
                DispatcherQueue().TryEnqueue([this]()
                {
                    while (AudioSession* newSession = audioController->NewSession())
                    {
                        CreateAudioView(newSession, false);
                    }
                });
            });

            for (size_t i = 0; i < audioSessions->size(); i++)
            {
                CreateAudioView(audioSessions->at(i).get(), true);
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

        int16_t duplicate = -1;
        for (size_t j = 0; j < audioSessions->size(); j++)
        {
            if (audioSessions->at(j)->GroupingParam() == audioSession->GroupingParam())
            {
                duplicate++;
            }
        }

        audioSessionVolumeChanged.push_back(audioSession->VolumeChanged([&](IInspectable s, float newVolume)
        {
            DispatcherQueue().TryEnqueue([this, id = unbox_value<guid>(s), newVolume]()
            {
                auto children = AudioSessionsPanel().Children();
                for (UIElement const& uiElement : children)
                {
                    AudioSessionView view = uiElement.try_as<AudioSessionView>();
                    if (view && view.Id() == id)
                    {
                        view.Volume(static_cast<double>(newVolume) * 100.0);
                    }
                }
            });
        }));
        audioSessionsStateChanged.push_back(audioSession->StateChanged([&](IInspectable s, uint32_t state)
        {
            DispatcherQueue().TryEnqueue([this, id = unbox_value<guid>(s), state]()
            {
                auto children = AudioSessionsPanel().Children();
                for (UIElement const& uiElement : children)
                {
                    AudioSessionView view = uiElement.try_as<AudioSessionView>();
                    if (view && view.Id() == id)
                    {
                        AudioState audioState = (AudioState)state;
                        switch (audioState)
                        {
                            case AudioState::Muted:
                                view.Muted(true);
                                break;
                            case AudioState::Unmuted:
                                view.Muted(false);
                                break;
                            case AudioState::Active:
                                break;
                            case AudioState::Inactive:
                                break;
                            case AudioState::Expired:
                                uint32_t indexOf = 0;
                                if (AudioSessionsPanel().Children().IndexOf(view, indexOf))
                                {
                                    AudioSessionsPanel().Children().RemoveAt(indexOf);
                                    AudioSessionsPanel().ColumnDefinitions().GetAt(indexOf).Width(GridLengthHelper::FromPixels(0));
                                    Grid_SizeChanged(nullptr, nullptr);
                                }
                                break;
                        }
                    }
                }
            });
        }));

        if (duplicate > 0)
        {
            bool alreadyAdded = false;
            for (auto&& children : AudioSessionsPanel().Children())
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

        volumeChangedRevokers.push_back(view.VolumeChanged(auto_revoke, { this, &MainWindow::AudioSessionView_VolumeChanged }));
        volumeStateChangedRevokers.push_back(view.VolumeStateChanged(auto_revoke, { this, &MainWindow::AudioSessionView_VolumeStateChanged }));

        {
            ColumnDefinition colDef{};
            colDef.Width(GridLength(85.0, GridUnitType::Pixel));
            AudioSessionsPanel().ColumnDefinitions().Append(colDef);
            Grid::SetColumn(view, AudioSessionsPanel().ColumnDefinitions().GetView().Size() - 1);
            AudioSessionsPanel().Children().Append(view);
        }


        const double padding = Application::Current().Resources().Lookup(box_value(L"WindowPadding")).as<double>();
        int32_t columnSpacing = static_cast<int32_t>(AudioSessionsPanel().ColumnSpacing()) * static_cast<int32_t>(AudioSessionsPanel().ColumnDefinitions().Size() > 1);
        int32_t width = padding;

        int32_t colDefWidth = 85;
        for (ColumnDefinition colDef : AudioSessionsPanel().ColumnDefinitions())
        {
            width += colDefWidth + columnSpacing;
        }

        if (width < 140)
        {
            width = 140;
        }

        DisplayArea display = DisplayArea::GetFromPoint(PointInt32(0, 0), DisplayAreaFallback::Primary);
        int32_t height = appWindow.Size().Height;
        PointInt32 pos = appWindow.Position();

        int32_t posX = pos.X + width;
        if (posX > display.WorkArea().Width)
        {
            pos.X -= posX - display.WorkArea().Width;
        }

        appWindow.MoveAndResize(RectInt32(pos.X, pos.Y, width, height));
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
                    dispatcherQueueController = { ptr, take_ownership_from_abi };
                }

                systemBackdropConfiguration = SystemBackdropConfiguration();
                systemBackdropConfiguration.IsInputActive(true);
                systemBackdropConfiguration.Theme((SystemBackdropTheme)RootGrid().ActualTheme());

                /*activatedRevoker = Activated(auto_revoke, [this](IInspectable const&, WindowActivatedEventArgs const& args)
                {
                    systemBackdropConfiguration.IsInputActive(WindowActivationState::Deactivated != args.WindowActivationState());
                });*/

                themeChangedRevoker = RootGrid().ActualThemeChanged(auto_revoke, [this](FrameworkElement const&, IInspectable const&)
                {
                    systemBackdropConfiguration.Theme((SystemBackdropTheme)RootGrid().ActualTheme());
                });


                backdropController = BackdropController();
                backdropController.TintColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                backdropController.FallbackColor(Application::Current().Resources().TryLookup(box_value(L"SolidBackgroundFillColorBase")).as<Windows::UI::Color>());
                backdropController.TintOpacity(static_cast<float>(Application::Current().Resources().TryLookup(box_value(L"MicaControllerTintOpacity")).as<double>()));
                backdropController.LuminosityOpacity(static_cast<float>(Application::Current().Resources().TryLookup(box_value(L"MicaControllerLuminosityOpacity")).as<double>()));
                backdropController.SetSystemBackdropConfiguration(systemBackdropConfiguration);
                backdropController.AddSystemBackdropTarget(supportsBackdrop);
            }
        }
    }

    void MainWindow::AppWindow_Closing(winrt::Microsoft::UI::Windowing::AppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs)
    {
        ApplicationData::Current().LocalSettings().Values().Insert(L"WindowHeight", box_value(appWindow.Size().Height));

        appWindow.Closing(appWindowClosingEventToken);

        // unsuscribe to VolumeChanged event
        for (size_t i = 0; i < audioSessionVolumeChanged.size(); i++)
        {
            audioSessions->at(i)->VolumeChanged(audioSessionVolumeChanged[i]);
        }

        // unregister audio sessions from audio events and release com ptrs
        for (size_t i = 0; i < audioSessions->size(); i++)
        {
            audioSessions->at(i)->Unregister();
            audioSessions->at(i)->Release();
        }
    }
}
