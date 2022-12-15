// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "AudioProfileEditPage.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioProfileEditPage : AudioProfileEditPageT<AudioProfileEditPage>
    {
    public:
        AudioProfileEditPage();

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioSessionView> AudioSessions() const
        {
            return audioSessions;
        };

        inline winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return e_propertyChanged.add(handler);
        };
        inline void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
        };

        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);
        winrt::Windows::Foundation::IAsyncAction OnNavigatingFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const& args);
        void Page_Loading(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const& args);
        void NextButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void CancelButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ProfileNameEditTextBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction CreateAudioSessionButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction AddAudioSessionButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LockSessionAppBarButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void RemoveSessionAppBarButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        winrt::SND_Vol::AudioProfile audioProfile{ nullptr };
        winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer timer{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::SND_Vol::AudioSessionView> audioSessions = winrt::single_threaded_observable_vector< winrt::SND_Vol::AudioSessionView>();
        bool navigationOutAllowed = false;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;

        void SaveProfile();
        winrt::SND_Vol::AudioSessionView CreateAudioSessionView(winrt::hstring header, float volume, bool muted);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioProfileEditPage : AudioProfileEditPageT<AudioProfileEditPage, implementation::AudioProfileEditPage>
    {
    };
}
