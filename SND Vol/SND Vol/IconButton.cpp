#include "pch.h"
#include "IconButton.h"
#if __has_include("IconButton.g.cpp")
#include "IconButton.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Input;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Controls;


namespace winrt::SND_Vol::implementation
{
    DependencyProperty IconButton::_glyphProperty = DependencyProperty::Register(
        L"Glyph",
        xaml_typename<hstring>(),
        xaml_typename<SND_Vol::IconButton>(),
        PropertyMetadata(box_value(L""))
    );
    DependencyProperty IconButton::_textNameProperty = DependencyProperty::Register(
        L"Text",
        xaml_typename<hstring>(),
        xaml_typename<SND_Vol::IconButton>(),
        PropertyMetadata(box_value(L""))
    );

    IconButton::IconButton()
    {
        DefaultStyleKey(winrt::box_value(L"SND_Vol.IconButton"));
        auto&& userButton = FindName(L"UserButton");
        if (userButton)
        {
            Button button = userButton.as<Button>();
        }
    }

    inline winrt::hstring IconButton::Glyph() const
    {
        return unbox_value<hstring>(GetValue(_glyphProperty));
    }

    inline void IconButton::Glyph(const winrt::hstring& value)
    {
        SetValue(_glyphProperty, box_value(value));
    }

    inline winrt::hstring IconButton::Text() const
    {
        return unbox_value<hstring>(GetValue(_textNameProperty));
    }

    inline void IconButton::Text(const winrt::hstring& value)
    {
        SetValue(_textNameProperty, box_value(value));
    }

    void IconButton::OnPointerPressed(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
    {
        if (enabled)
        {
            VisualStateManager::GoToState(*this, L"PointerPressed", true);
            e_click(*this, RoutedEventArgs());
            args.Handled(true);
        }
    }

    void IconButton::OnPointerReleased(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
    {
        if (pointerExited)
        {
            VisualStateManager::GoToState(*this, L"Normal", true);
        }
        else
        {
            VisualStateManager::GoToState(*this, L"PointerOver", true);
        }
    }

    void IconButton::OnPointerEntered(const PointerRoutedEventArgs& args)
    {
        pointerExited = false;

        bool leftButtonPressed = false;
        if (args.Pointer().PointerDeviceType() == PointerDeviceType::Mouse)
        {
            auto&& point = args.GetCurrentPoint(*this);
            leftButtonPressed = point.Properties().IsLeftButtonPressed();
        }

        if (!leftButtonPressed)
        {
            VisualStateManager::GoToState(*this, L"PointerOver", true);
        }
    }

    void IconButton::OnPointerExited(const Microsoft::UI::Xaml::Input::PointerRoutedEventArgs& args)
    {
        pointerExited = true;
        VisualStateManager::GoToState(*this, L"Normal", true);
    }
}
