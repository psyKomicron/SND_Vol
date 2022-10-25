// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "LetterKeyIcon.g.h"

namespace winrt::SND_Vol::implementation
{
    struct LetterKeyIcon : LetterKeyIconT<LetterKeyIcon>
    {
    public:
        LetterKeyIcon();
        LetterKeyIcon(const winrt::hstring& key);

        winrt::hstring Key() const;
        void Key(const winrt::hstring& value);

    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty _keyProperty;
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct LetterKeyIcon : LetterKeyIconT<LetterKeyIcon, implementation::LetterKeyIcon>
    {
    };
}
