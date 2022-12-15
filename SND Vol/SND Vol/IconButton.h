#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "IconButton.g.h"

namespace winrt::SND_Vol::implementation
{
    struct IconButton : IconButtonT<IconButton>
    {
    public:
        IconButton();

        winrt::hstring Glyph() const;
        void Glyph(const winrt::hstring& value);
        winrt::hstring Text() const;
        void Text(const winrt::hstring& value);

        static Microsoft::UI::Xaml::DependencyProperty GlyphProperty()
        {
            return _glyphProperty;
        };

        static Microsoft::UI::Xaml::DependencyProperty TextProperty()
        {
            return _textNameProperty;
        };

        inline winrt::event_token Click(const Windows::Foundation::TypedEventHandler<winrt::SND_Vol::IconButton, Microsoft::UI::Xaml::RoutedEventArgs>& handler)
        {
            return e_click.add(handler);
        };

        inline void Click(const winrt::event_token token)
        {
            e_click.remove(token);
        };

        void OnPointerEntered(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerExited(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerPressed(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);
        void OnPointerReleased(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args);

    private:
        static Microsoft::UI::Xaml::DependencyProperty _glyphProperty;
        static Microsoft::UI::Xaml::DependencyProperty _textNameProperty;

        bool pointerExited = false;
        bool enabled = true;

        winrt::event<Windows::Foundation::TypedEventHandler<winrt::SND_Vol::IconButton, Microsoft::UI::Xaml::RoutedEventArgs>> e_click {};
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct IconButton : IconButtonT<IconButton, implementation::IconButton>
    {
    };
}
