// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "LetterKeyIcon.h"
#if __has_include("LetterKeyIcon.g.cpp")
#include "LetterKeyIcon.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SND_Vol::implementation
{
    DependencyProperty LetterKeyIcon::_keyProperty = DependencyProperty::Register(
        L"Key",
        xaml_typename<hstring>(),
        xaml_typename<SND_Vol::LetterKeyIcon>(),
        PropertyMetadata(box_value(L""))
    );

    LetterKeyIcon::LetterKeyIcon()
    {
        DefaultStyleKey(box_value(L"SND_Vol.LetterKeyIcon"));
    }

    LetterKeyIcon::LetterKeyIcon(const hstring& key) : LetterKeyIcon()
    {
        SetValue(_keyProperty, box_value(key));
    }


    hstring LetterKeyIcon::Key() const
    {
        return unbox_value<hstring>(GetValue(_keyProperty));
    }

    void LetterKeyIcon::Key(const hstring& value)
    {
        SetValue(_keyProperty, box_value(value));
    }
}
