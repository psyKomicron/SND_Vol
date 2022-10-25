// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "KeyIcon.g.h"

namespace winrt::SND_Vol::implementation
{
    struct KeyIcon : KeyIconT<KeyIcon>
    {
    public:
        KeyIcon();
        KeyIcon(const winrt::Windows::System::VirtualKey& key);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct KeyIcon : KeyIconT<KeyIcon, implementation::KeyIcon>
    {
    };
}
