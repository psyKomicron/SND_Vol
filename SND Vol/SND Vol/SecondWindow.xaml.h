#pragma once

#include "SecondWindow.g.h"

namespace winrt::SND_Vol::implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow>
    {
        SecondWindow();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct SecondWindow : SecondWindowT<SecondWindow, implementation::SecondWindow>
    {
    };
}
