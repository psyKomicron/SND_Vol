#pragma once

#include "NewContentPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage>
    {
    private:
        

    public:
        NewContentPage();

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        winrt::Windows::Foundation::IAsyncAction Page_Loading(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const& args);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage, implementation::NewContentPage>
    {
    };
}
