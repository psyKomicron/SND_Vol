// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "AudioProfilesPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioProfilesPage : AudioProfilesPageT<AudioProfilesPage>
    {
        AudioProfilesPage();

        inline winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioProfile> AudioProfiles()
        {
            return audioProfiles;
        };

        void NewProfileNameTextBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void AddProfileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DeleteProfileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction EditProfileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DuplicateProfileButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MuteToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioProfile> audioProfiles
        {
            single_threaded_observable_vector<winrt::SND_Vol::AudioProfile>()
        };
        winrt::Windows::Foundation::Collections::IVectorView<winrt::SND_Vol::AudioSessionView> audioSessions;
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioProfilesPage : AudioProfilesPageT<AudioProfilesPage, implementation::AudioProfilesPage>
    {
    };
}
