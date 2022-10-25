// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "HotKeysViewer.xaml.h"
#if __has_include("HotKeysViewer.g.cpp")
#include "HotKeysViewer.g.cpp"
#endif

using namespace winrt;

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Microsoft::UI::Input;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::System;


namespace winrt::SND_Vol::implementation
{
    HotKeysViewer::HotKeysViewer()
    {
        InitializeComponent();

        keyTimer = DispatcherQueue().CreateTimer();
        keyTimer.Interval(TimeSpan(std::chrono::milliseconds(100)));
        keyTimer.Tick([this](auto, auto)
        {

        });
    }

    void HotKeysViewer::SetActiveKeys(const IVector<VirtualKey>& activeKeys)
    {
        for (VirtualKey key : activeKeys)
        {
            switch (key)
            {
                case VirtualKey::LeftButton:
                    LeftMouseButton().Background(GetActiveBrush());
                    break;
                case VirtualKey::RightButton:
                    RightMouseButton().Background(GetActiveBrush());
                    break;
                case VirtualKey::Cancel:
                    break;
                case VirtualKey::MiddleButton:
                    MiddleMouseButton().Background(GetActiveBrush());
                    break;
                case VirtualKey::XButton1:
                    XMouseButton1().Background(GetActiveBrush());
                    break;
                case VirtualKey::XButton2:
                    XMouseButton2().Background(GetActiveBrush());
                    break;
                case VirtualKey::Back:
                    break;
                case VirtualKey::Tab:
                    TabKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Clear:
                    break;
                case VirtualKey::Enter:
                    EnterKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Shift:
                    LeftShiftKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Control:
                    LeftControlKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Menu:
                    LeftAltKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Pause:
                    break;
                case VirtualKey::CapitalLock:
                    CapsLockKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Escape:
                    EscKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Space:
                    SpaceKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::PageUp:
                    break;
                case VirtualKey::PageDown:
                    break;
                case VirtualKey::End:
                    break;
                case VirtualKey::Home:
                    break;
                case VirtualKey::Left:
                    break;
                case VirtualKey::Up:
                    break;
                case VirtualKey::Right:
                    break;
                case VirtualKey::Down:
                    break;
                case VirtualKey::Select:
                    break;
                case VirtualKey::Print:
                    break;
                case VirtualKey::Execute:
                    break;
                case VirtualKey::Snapshot:
                    break;
                case VirtualKey::Insert:
                    break;
                case VirtualKey::Delete:
                    break;
                case VirtualKey::Help:
                    break;
                case VirtualKey::Number0:
                    ZeroKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number1:
                    OneKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number2:
                    TwoKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number3:
                    ThreeKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number4:
                    FourKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number5:
                    FiveKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number6:
                    SixKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number7:
                    SevenKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number8:
                    EightKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Number9:
                    NineKey().Background(GetActiveBrush());
                    break;

                case VirtualKey::A:
                    AKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::B:
                    BKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::C:
                    CKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::D:
                    DKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::E:
                    EKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::F:
                    FKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::G:
                    GKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::H:
                    HKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::I:
                    IKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::J:
                    JKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::K:
                    KKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::L:
                    LKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::M:
                    MKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::N:
                    NKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::O:
                    OKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::P:
                    PKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Q:
                    QKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::R:
                    RKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::S:
                    SKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::T:
                    TKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::U:
                    UKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::V:
                    VKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::W:
                    WKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::X:
                    XKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Y:
                    YKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Z:
                    ZKey().Background(GetActiveBrush());
                    break;

                case VirtualKey::LeftWindows:
                    LeftWindowsKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::RightWindows:
                    RightWindowsKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::Application:
                    break;
                case VirtualKey::Sleep:
                    break;
                case VirtualKey::NumberPad0:
                    break;
                case VirtualKey::NumberPad1:
                    break;
                case VirtualKey::NumberPad2:
                    break;
                case VirtualKey::NumberPad3:
                    break;
                case VirtualKey::NumberPad4:
                    break;
                case VirtualKey::NumberPad5:
                    break;
                case VirtualKey::NumberPad6:
                    break;
                case VirtualKey::NumberPad7:
                    break;
                case VirtualKey::NumberPad8:
                    break;
                case VirtualKey::NumberPad9:
                    break;
                case VirtualKey::Multiply:
                    break;
                case VirtualKey::Add:
                    break;
                case VirtualKey::Separator:
                    break;
                case VirtualKey::Subtract:
                    break;
                case VirtualKey::Decimal:
                    break;
                case VirtualKey::Divide:
                    break;
                case VirtualKey::F1:
                    F1Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F2:
                    F2Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F3:
                    F3Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F4:
                    F4Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F5:
                    F5Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F6:
                    F6Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F7:
                    F7Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F8:
                    F8Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F9:
                    F9Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F10:
                    F10Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F11:
                    F11Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F12:
                    F12Key().Background(GetActiveBrush());
                    break;
                case VirtualKey::F13:
                    break;
                case VirtualKey::F14:
                    break;
                case VirtualKey::F15:
                    break;
                case VirtualKey::F16:
                    break;
                case VirtualKey::F17:
                    break;
                case VirtualKey::F18:
                    break;
                case VirtualKey::F19:
                    break;
                case VirtualKey::F20:
                    break;
                case VirtualKey::F21:
                    break;
                case VirtualKey::F22:
                    break;
                case VirtualKey::F23:
                    break;
                case VirtualKey::F24:
                    break;
                case VirtualKey::NavigationView:
                    break;
                case VirtualKey::NavigationMenu:
                    break;
                case VirtualKey::NavigationUp:
                    break;
                case VirtualKey::NavigationDown:
                    break;
                case VirtualKey::NavigationLeft:
                    break;
                case VirtualKey::NavigationRight:
                    break;
                case VirtualKey::NavigationAccept:
                    break;
                case VirtualKey::NavigationCancel:
                    break;
                case VirtualKey::NumberKeyLock:
                    break;
                case VirtualKey::Scroll:
                    break;
                case VirtualKey::LeftShift:
                    LeftShiftKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::RightShift:
                    RightShiftKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::LeftControl:
                    LeftControlKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::RightControl:
                    RightControlKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::LeftMenu:
                    LeftAltKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::RightMenu:
                    RightAltKey().Background(GetActiveBrush());
                    break;
                case VirtualKey::GoBack:
                    break;
                case VirtualKey::GoForward:
                    break;
                case VirtualKey::Refresh:
                    break;
                case VirtualKey::Stop:
                    break;
                case VirtualKey::Search:
                    break;
                case VirtualKey::Favorites:
                    break;
                case VirtualKey::GoHome:
                    break;
            }
        }
    }

    void HotKeysViewer::Grid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        
    }

    void HotKeysViewer::Grid_Unloaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {

    }

    void HotKeysViewer::Grid_KeyDown(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
    {
        std::vector<VirtualKey> virtualKeys
        {
            VirtualKey::None,
            VirtualKey::LeftButton,
            VirtualKey::RightButton,
            VirtualKey::Cancel,
            VirtualKey::MiddleButton,
            VirtualKey::XButton1,
            VirtualKey::XButton2,
            VirtualKey::Back,
            VirtualKey::Tab,
            VirtualKey::Clear,
            VirtualKey::Enter,
            VirtualKey::Shift,
            VirtualKey::Control,
            VirtualKey::Menu,
            VirtualKey::Pause,
            VirtualKey::CapitalLock,
            VirtualKey::Kana,
            VirtualKey::Hangul,
            VirtualKey::ImeOn,
            VirtualKey::Junja,
            VirtualKey::Final,
            VirtualKey::Hanja,
            VirtualKey::Kanji,
            VirtualKey::ImeOff,
            VirtualKey::Escape,
            VirtualKey::Convert,
            VirtualKey::NonConvert,
            VirtualKey::Accept,
            VirtualKey::ModeChange,
            VirtualKey::Space,
            VirtualKey::PageUp,
            VirtualKey::PageDown,
            VirtualKey::End,
            VirtualKey::Home,
            VirtualKey::Left,
            VirtualKey::Up,
            VirtualKey::Right,
            VirtualKey::Down,
            VirtualKey::Select,
            VirtualKey::Print,
            VirtualKey::Execute,
            VirtualKey::Snapshot,
            VirtualKey::Insert,
            VirtualKey::Delete,
            VirtualKey::Help,
            VirtualKey::Number0,
            VirtualKey::Number1,
            VirtualKey::Number2,
            VirtualKey::Number3,
            VirtualKey::Number4,
            VirtualKey::Number5,
            VirtualKey::Number6,
            VirtualKey::Number7,
            VirtualKey::Number8,
            VirtualKey::Number9,
            VirtualKey::A,
            VirtualKey::B,
            VirtualKey::C,
            VirtualKey::D,
            VirtualKey::E,
            VirtualKey::F,
            VirtualKey::G,
            VirtualKey::H,
            VirtualKey::I,
            VirtualKey::J,
            VirtualKey::K,
            VirtualKey::L,
            VirtualKey::M,
            VirtualKey::N,
            VirtualKey::O,
            VirtualKey::P,
            VirtualKey::Q,
            VirtualKey::R,
            VirtualKey::S,
            VirtualKey::T,
            VirtualKey::U,
            VirtualKey::V,
            VirtualKey::W,
            VirtualKey::X,
            VirtualKey::Y,
            VirtualKey::Z,
            VirtualKey::LeftWindows,
            VirtualKey::RightWindows,
            VirtualKey::Application,
            VirtualKey::Sleep,
            VirtualKey::NumberPad0,
            VirtualKey::NumberPad1,
            VirtualKey::NumberPad2,
            VirtualKey::NumberPad3,
            VirtualKey::NumberPad4,
            VirtualKey::NumberPad5,
            VirtualKey::NumberPad6,
            VirtualKey::NumberPad7,
            VirtualKey::NumberPad8,
            VirtualKey::NumberPad9,
            VirtualKey::Multiply,
            VirtualKey::Add,
            VirtualKey::Separator,
            VirtualKey::Subtract,
            VirtualKey::Decimal,
            VirtualKey::Divide,
            VirtualKey::F1,
            VirtualKey::F2,
            VirtualKey::F3,
            VirtualKey::F4,
            VirtualKey::F5,
            VirtualKey::F6,
            VirtualKey::F7,
            VirtualKey::F8,
            VirtualKey::F9,
            VirtualKey::F10,
            VirtualKey::F11,
            VirtualKey::F12,
            VirtualKey::F13,
            VirtualKey::F14,
            VirtualKey::F15,
            VirtualKey::F16,
            VirtualKey::F17,
            VirtualKey::F18,
            VirtualKey::F19,
            VirtualKey::F20,
            VirtualKey::F21,
            VirtualKey::F22,
            VirtualKey::F23,
            VirtualKey::F24,
            VirtualKey::NavigationView,
            VirtualKey::NavigationMenu,
            VirtualKey::NavigationUp,
            VirtualKey::NavigationDown,
            VirtualKey::NavigationLeft,
            VirtualKey::NavigationRight,
            VirtualKey::NavigationAccept,
            VirtualKey::NavigationCancel,
            VirtualKey::NumberKeyLock,
            VirtualKey::Scroll,
            VirtualKey::LeftShift,
            VirtualKey::RightShift,
            VirtualKey::LeftControl,
            VirtualKey::RightControl,
            VirtualKey::LeftMenu,
            VirtualKey::RightMenu,
            VirtualKey::GoBack,
            VirtualKey::GoForward,
            VirtualKey::Refresh,
            VirtualKey::Stop,
            VirtualKey::Search,
            VirtualKey::Favorites,
            VirtualKey::GoHome,
            VirtualKey::GamepadA,
            VirtualKey::GamepadB,
            VirtualKey::GamepadX,
            VirtualKey::GamepadY,
            VirtualKey::GamepadRightShoulder,
            VirtualKey::GamepadLeftShoulder,
            VirtualKey::GamepadLeftTrigger,
            VirtualKey::GamepadRightTrigger,
            VirtualKey::GamepadDPadUp,
            VirtualKey::GamepadDPadDown,
            VirtualKey::GamepadDPadLeft,
            VirtualKey::GamepadDPadRight,
            VirtualKey::GamepadMenu,
            VirtualKey::GamepadView,
            VirtualKey::GamepadLeftThumbstickButton,
            VirtualKey::GamepadRightThumbstickButton,
            VirtualKey::GamepadLeftThumbstickUp,
            VirtualKey::GamepadLeftThumbstickDown,
            VirtualKey::GamepadLeftThumbstickRight,
            VirtualKey::GamepadLeftThumbstickLeft,
            VirtualKey::GamepadRightThumbstickUp,
            VirtualKey::GamepadRightThumbstickDown,
            VirtualKey::GamepadRightThumbstickRight,
            VirtualKey::GamepadRightThumbstickLeft,
        };

        IVector<VirtualKey> activeKeys{ single_threaded_vector<VirtualKey>() };
        for (VirtualKey key : virtualKeys)
        {
            if (InputKeyboardSource::GetKeyStateForCurrentThread(key) != winrt::Windows::UI::Core::CoreVirtualKeyStates::None)
            {
                activeKeys.Append(key);
            }
        }

        SetActiveKeys(activeKeys);
    }


    Brush HotKeysViewer::GetActiveBrush()
    {
        return Application::Current().Resources().Lookup(box_value(L"AccentFillColorDefaultBrush")).as<Brush>();
    }
}
