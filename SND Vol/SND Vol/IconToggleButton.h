#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "IconToggleButton.g.h"

namespace winrt::SND_Vol::implementation
{
    struct IconToggleButton : IconToggleButtonT<IconToggleButton>
    {
        IconToggleButton();

        inline winrt::Windows::Foundation::IInspectable OnContent();
        inline void OnContent(const winrt::Windows::Foundation::IInspectable& value);
        inline winrt::Windows::Foundation::IInspectable OffContent();
        inline void OffContent(const winrt::Windows::Foundation::IInspectable& value);
        inline winrt::hstring Glyph();
        inline void Glyph(const winrt::hstring& value);
        inline bool IsOn();
        inline void IsOn(const bool& value);
        inline winrt::Microsoft::UI::Xaml::Style IconStyle();
        inline void IconStyle(const winrt::Microsoft::UI::Xaml::Style& value);

        static Microsoft::UI::Xaml::DependencyProperty OnContentProperty()
        {
            return _onContentProperty;
        }

        static Microsoft::UI::Xaml::DependencyProperty OffContentProperty()
        {
            return _offContentProperty;
        }

        static Microsoft::UI::Xaml::DependencyProperty GlyphProperty()
        {
            return _glyphProperty;
        }

        static Microsoft::UI::Xaml::DependencyProperty IconStyleProperty()
        {
            return _iconStyleProperty;
        }

        inline winrt::event_token Click(const Windows::Foundation::TypedEventHandler<winrt::SND_Vol::IconToggleButton, Microsoft::UI::Xaml::RoutedEventArgs>& handler)
        {
            return e_click.add(handler);
        }

        inline void Click(const winrt::event_token token)
        {
            e_click.remove(token);
        }

        void OnPointerEntered(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerExited(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerPressed(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerReleased(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);

    private:
        static Microsoft::UI::Xaml::DependencyProperty _onContentProperty;
        static Microsoft::UI::Xaml::DependencyProperty _offContentProperty;
        static Microsoft::UI::Xaml::DependencyProperty _glyphProperty;
        static Microsoft::UI::Xaml::DependencyProperty _iconStyleProperty;

        bool pointerExited = false;
        bool enabled = true;
        bool isOn = false;

        winrt::event<Windows::Foundation::TypedEventHandler<winrt::SND_Vol::IconToggleButton, Microsoft::UI::Xaml::RoutedEventArgs>> e_click {};
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct IconToggleButton : IconToggleButtonT<IconToggleButton, implementation::IconToggleButton>
    {
    };
}
