#pragma once

#include "NewContentPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage>
    {
    public:
        NewContentPage();

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage, implementation::NewContentPage>
    {
    };
}
