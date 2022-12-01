#pragma once

#include "NewContentPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage>
    {
        NewContentPage();
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct NewContentPage : NewContentPageT<NewContentPage, implementation::NewContentPage>
    {
    };
}
