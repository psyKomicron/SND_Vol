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
using namespace winrt::Windows::Foundation::Collections;


namespace winrt::SND_Vol::implementation
{
    AudioProfilesPage::AudioProfilesPage()
    {
        InitializeComponent();

        AudioProfile profile{};
        profile.ProfileName(L"Youtube profile");
        profile.IsDefaultProfile(true);
        audioProfiles.Append(profile);

        profile = AudioProfile();
        profile.ProfileName(L"Game profile");
        profile.IsDefaultProfile(false);
        audioProfiles.Append(profile);

        audioProfiles.VectorChanged([this](auto sender, IVectorChangedEventArgs args)
        {
            static bool reordering = false;

            if (args.CollectionChange() == CollectionChange::ItemInserted && reordering)
            {
                if (audioProfiles.Size() > 0)
                {
                    audioProfiles.GetAt(0).IsDefaultProfile(true);

                    for (size_t i = 1; i < audioProfiles.Size(); i++)
                    {
                        audioProfiles.GetAt(i).IsDefaultProfile(false);
                    }
                }
            }

            if (args.CollectionChange() == CollectionChange::ItemRemoved && !reordering)
            {
                reordering = true;
            }
            else
            {
                reordering = false;
            }
        });

        ProfilesListView().ItemsSource(audioProfiles);
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
        bool defaultProfile = DefaultProfileSwitch().IsChecked().GetBoolean();

        for (auto&& audioProfile : audioProfiles)
        {
            hstring audioProfileName = audioProfile.ProfileName();
            if (audioProfileName == name)
            {
                ErrorTextBlock().Text(L"Profile name already exists");
                ErrorTextBlock().Visibility(Visibility::Visible);
                return;
            }

            if (defaultProfile && audioProfile.IsDefaultProfile())
            {
                audioProfile.IsDefaultProfile(false);
            }
        }

        AudioProfile profile{};
        profile.ProfileName(name);
        profile.IsDefaultProfile(defaultProfile);

        if (defaultProfile)
        {
            audioProfiles.InsertAt(0, profile);
        }
        else
        {
            audioProfiles.Append(profile);
        }
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

    IAsyncAction AudioProfilesPage::EditProfileButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        hstring tag = sender.as<Button>().Tag().as<hstring>();
        for (size_t i = 0; i < audioProfiles.Size(); i++)
        {
            if (audioProfiles.GetAt(i).ProfileName() == tag)
            {
                AudioProfile editedProfile = audioProfiles.GetAt(i);
                Frame().Navigate(xaml_typename<AudioProfileEditPage>(), editedProfile, ::Media::Animation::DrillInNavigationTransitionInfo());
                break;
            }
        }

        co_return;
    }

    void AudioProfilesPage::DuplicateProfileButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        hstring tag = sender.as<Button>().Tag().as<hstring>();
        for (size_t i = 0; i < audioProfiles.Size(); i++)
        {
            if (audioProfiles.GetAt(i).ProfileName() == tag)
            {
                // TODO: Copy the profile values to the new one.

                AudioProfile profile{};
                hstring profileName = audioProfiles.GetAt(i).ProfileName();

                uint32_t count = 1u;
                for (size_t j = 0; j < audioProfiles.Size(); j++)
                {
                    hstring test = audioProfiles.GetAt(j).ProfileName();
                    if (test == profileName)
                    {
                        profileName = audioProfiles.GetAt(i).ProfileName() + L" (" + to_hstring(count) + L")";
                    }
                }

                profile.ProfileName(profileName);
                profile.IsDefaultProfile(false);

                audioProfiles.Append(profile);

                break;
            }
        }
    }

    void AudioProfilesPage::MuteToggleButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
    }
}
