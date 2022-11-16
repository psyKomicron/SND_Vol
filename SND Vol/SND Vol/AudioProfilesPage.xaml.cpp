// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "AudioProfilesPage.xaml.h"
#if __has_include("AudioProfilesPage.g.cpp")
#include "AudioProfilesPage.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;


namespace winrt::SND_Vol::implementation
{
    AudioProfilesPage::AudioProfilesPage()
    {
        InitializeComponent();
    }


    void AudioProfilesPage::NewProfileNameTextBox_TextChanged(IInspectable const&, TextChangedEventArgs const&)
    {
        if (ErrorTextBlock().Visibility() == Visibility::Visible)
        {
            ErrorTextBlock().Visibility(Visibility::Collapsed);
        }
    }

    void AudioProfilesPage::AddProfileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        hstring name = NewProfileNameTextBox().Text();
        for (auto&& audioProfile : audioProfiles)
        {
            hstring audioProfileName = audioProfile.ProfileName();
            if (audioProfileName == name)
            {
                ErrorTextBlock().Text(L"Profile name already exists");
                ErrorTextBlock().Visibility(Visibility::Visible);
                return;
            }
        }

        AudioProfile profile{};
        profile.ProfileName(name);

        audioProfiles.Append(profile);
    }

    void AudioProfilesPage::DeleteProfileButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        hstring tag = sender.as<Button>().Tag().as<hstring>();
        for (size_t i = 0; i < audioProfiles.Size(); i++)
        {
            if (audioProfiles.GetAt(i).ProfileName() == tag)
            {
                audioProfiles.RemoveAt(i);
                break;
            }
        }
    }
}
