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

        winrt::hstring Glyph() const { return glyph; };
        void Glyph(const winrt::hstring& value) { glyph = value; };
        winrt::hstring Text() const { return text; };
        void Text(const winrt::hstring& value) { text = value; };

        inline winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return e_propertyChanged.add(handler);
        };
        inline void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
        };

    private:
        winrt::hstring glyph = L"";
        winrt::hstring text = L"";

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged {};
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct IconButton : IconButtonT<IconButton, implementation::IconButton>
    {
    };
}
