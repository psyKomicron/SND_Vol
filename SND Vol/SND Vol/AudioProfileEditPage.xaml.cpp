// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "AudioProfileEditPage.xaml.h"
#if __has_include("AudioProfileEditPage.g.cpp")
#include "AudioProfileEditPage.g.cpp"
#endif
#include "LegacyAudioController.h"
#include "AudioSession.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;


namespace winrt::SND_Vol::implementation
{
    AudioProfileEditPage::AudioProfileEditPage()
    {
        InitializeComponent();

        timer = DispatcherQueue().CreateTimer();
        timer.Interval(ProfileSavedAnimation().Duration().TimeSpan + std::chrono::milliseconds(200));
        timer.IsRepeating(false);
        timer.Tick([this](auto, auto)
        {
            Frame().Navigate(xaml_typename<AudioProfilesPage>(), audioProfile);
        });
    }

    void AudioProfileEditPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        audioProfile = args.Parameter().try_as<AudioProfile>();
        if (audioProfile == nullptr)
        {
            // Audio profile creation. The audio profiles page should send a unique temporary name for this audio profile.
            std::optional<hstring> tempName = args.Parameter().try_as<hstring>();
            if (!tempName)
            {
                navigationOutAllowed = true;
                Frame().GoBack();
            }
            else
            {
                audioProfile = AudioProfile();
                audioProfile.ProfileName(tempName.value());
                audioProfile.SystemVolume(-1.f);

                //winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
                SecondWindow::Current().Breadcrumbs().Append(
                    NavigationBreadcrumbBarItem{ L"Audio profile creation", xaml_typename<winrt::SND_Vol::AudioProfileEditPage>()}
                );
            }
        }
        else
        {
            SecondWindow::Current().Breadcrumbs().Append(
                NavigationBreadcrumbBarItem{ audioProfile.ProfileName(), xaml_typename<winrt::SND_Vol::AudioProfileEditPage>() }
            );
        }
    }

    IAsyncAction AudioProfileEditPage::OnNavigatingFrom(winrt::Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const& args)
    {
        // If either the audio profile is null or if we can navigate out of the page, we ignore the save instructions.
        if (!audioProfile || navigationOutAllowed)
        {
            co_return;
        }

        // Check if changes have been made
        // 1. Check audio changes.
        double systemVolume = 0.0;
        bool isDefaultProfile = false;
        hstring profileName;
        ::Collections::IMap<hstring, float> audioLevels;
        bool audioLevelsEqual = false;
        ::Collections::IMap<hstring, bool> audioStates;
        bool audioStatesEqual = false;

        // 2. Check settings changes.
        bool disableAnimations = DisableAnimationsCheckBox().IsChecked().GetBoolean();
        bool showAdditionalButtons = ShowAdditionalButtonsCheckBox().IsChecked().GetBoolean();
        bool keepOnTop = KeepOnTopCheckBox().IsChecked().GetBoolean();
        bool showMenu = ShowMenuCheckBox().IsChecked().GetBoolean();
        /*
           Layout combo box items should be arranged so that :
             -First item is Auto
             -Second item is Horizontal
             -Third item is Vertical
        */
        uint32_t layout = LayoutComboBox().SelectedIndex();

        if (
            systemVolume != audioProfile.SystemVolume() || isDefaultProfile != audioProfile.IsDefaultProfile() || profileName != audioProfile.ProfileName() ||
            !audioLevelsEqual || !audioStatesEqual ||
            disableAnimations != audioProfile.DisableAnimations() || showAdditionalButtons != audioProfile.ShowAdditionalButtons() || keepOnTop != audioProfile.KeepOnTop() || showMenu != audioProfile.ShowMenu() || layout != audioProfile.Layout()
            )
        {
            args.Cancel(true);
            winrt::Windows::UI::Xaml::Interop::TypeName sourcePageType = args.SourcePageType();
            IInspectable parameter = args.Parameter();
            auto navigationTransitionInfo = args.NavigationTransitionInfo();

            navigationOutAllowed = true;
            // Primary button is save.
            if (co_await ConfirmDialog().ShowAsync() == winrt::Microsoft::UI::Xaml::Controls::ContentDialogResult::Primary)
            {
                SaveProfile();
            }
            else // Secondary button is ignore/continue navigation
            {
                Frame().Navigate(sourcePageType, parameter, navigationTransitionInfo);
            }
        }
    }

    void AudioProfileEditPage::Page_Loading(FrameworkElement const&, IInspectable const& args)
    {
        ProfileNameEditTextBox().Text(audioProfile.ProfileName());
        DisableAnimationsCheckBox().IsChecked(audioProfile.DisableAnimations());
        ShowAdditionalButtonsCheckBox().IsChecked(audioProfile.ShowAdditionalButtons());
        KeepOnTopCheckBox().IsChecked(audioProfile.KeepOnTop());
        ShowMenuCheckBox().IsChecked(audioProfile.ShowMenu());
        LayoutComboBox().SelectedIndex(audioProfile.Layout());


        Audio::LegacyAudioController* controllerPtr = new Audio::LegacyAudioController(GUID());
        if (audioProfile.SystemVolume() < 0)
        {
            Audio::MainAudioEndpoint* mainAudioEndpointPtr = controllerPtr->GetMainAudioEndpoint();
            float value = mainAudioEndpointPtr->Volume();
            audioProfile.SystemVolume(value);
            mainAudioEndpointPtr->Release();
        }
        SystemVolumeSlider().Value(audioProfile.SystemVolume() * 100.f);


        if (audioProfile.AudioLevels().Size() > 0)
        {
            std::vector<AudioSessionView> views{};
            views.resize(audioProfile.SessionsIndexes().Size());

            for (auto&& pair : audioProfile.AudioLevels())
            {
                hstring sessionName = pair.Key();
                double volume = pair.Value() * 100.;
                bool muted = audioProfile.AudioStates().Lookup(pair.Key());
                uint32_t index = audioProfile.SessionsIndexes().Lookup(pair.Key());

                AudioSessionView view{};
                view.Header(sessionName);
                view.Volume(volume);
                view.Muted(muted);
                view.ContextFlyout(nullptr);

                //views.emplace(views.begin() + index, view);
                views[index] = view;
            }

            audioSessions = single_threaded_observable_vector<AudioSessionView>(std::move(views));
        }
        else
        {
            // Get audio sessions.
            std::vector<Audio::AudioSession*>* audioSessionsPtr = controllerPtr->GetSessions();
            for (size_t i = 0; i < audioSessionsPtr->size(); i++)
            {
                // Create view.
                AudioSessionView view{};
                view.Header(audioSessionsPtr->at(i)->Name());
                view.Volume(audioSessionsPtr->at(i)->Volume() * 100.);
                view.Muted(audioSessionsPtr->at(i)->Muted());
                view.ContextFlyout(nullptr);

                AudioSessions().Append(view);

                audioSessionsPtr->at(i)->Release(); // Directly release the AudioSession and release COM resources.
            }
            delete audioSessionsPtr;
        }

        controllerPtr->Release(); // Release audio controller and associated resources.
    }

    void AudioProfileEditPage::NextButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        navigationOutAllowed = true;

        SecondWindow::Current().Breadcrumbs().RemoveAtEnd();

        SaveProfile();
    }

    void AudioProfileEditPage::CancelButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        navigationOutAllowed = true;

        SecondWindow::Current().Breadcrumbs().RemoveAtEnd();

        Frame().GoBack();
    }

    void AudioProfileEditPage::ProfileNameEditTextBox_TextChanged(IInspectable const&, ::Controls::TextChangedEventArgs const&)
    {
        // TODO: Check if the profile name is already in use.
    }

    IAsyncAction AudioProfileEditPage::CreateAudioSessionButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (co_await AudioProfileCreationDialog().ShowAsync() == ContentDialogResult::Primary)
        {
            AudioSessionView view{};
            hstring sessionName = ProfileCreationTextBox().Text();
            double volume = ProfileCreationSlider().Value();
            view.Header(sessionName);
            view.Volume(volume);
            view.Muted(ProfileCreationCheckBox().IsChecked().GetBoolean());
            view.ContextFlyout(nullptr);

            audioSessions.Append(view);
        }
    }

    IAsyncAction AudioProfileEditPage::AddAudioSessionButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // Get audio sessions.
        Audio::LegacyAudioController* controllerPtr = new Audio::LegacyAudioController(GUID());
        std::vector<Audio::AudioSession*>* audioSessionsPtr = controllerPtr->GetSessions();
        for (size_t i = 0; i < audioSessionsPtr->size(); i++)
        {
            bool enabled = true;
            for (auto item : audioSessions)
            {
                AudioSessionView view = item.try_as<AudioSessionView>();
                if (view && view.Header() == audioSessionsPtr->at(i)->Name())
                {
                    enabled = false;
                }
            }

            if (enabled)
            {
                // Create view.
                AudioSessionView view{};
                view.Header(audioSessionsPtr->at(i)->Name());
                view.Volume(audioSessionsPtr->at(i)->Volume() * 100.);
                view.Muted(audioSessionsPtr->at(i)->Muted());
                view.ContextFlyout(nullptr);

                ProfileAddGridView().Items().Append(view);
            }

            audioSessionsPtr->at(i)->Release(); // Directly release the AudioSession and release COM resources.
        }
        delete audioSessionsPtr;
        controllerPtr->Release(); // Release audio controller and associated resources.

        co_await AudioProfileAddDialog().ShowAsync();

        ProfileAddGridView().Items().Clear();
    }


    void AudioProfileEditPage::SaveProfile()
    {
        VisualStateManager::GoToState(*this, L"ProfileSaved", true);

        audioProfile.ProfileName(ProfileNameEditTextBox().Text());
        audioProfile.SystemVolume(static_cast<float>(SystemVolumeSlider().Value()) / 100.f);

        audioProfile.AudioLevels().Clear();
        audioProfile.AudioStates().Clear();
        for (auto&& view : AudioSessions())
        {
            hstring header = view.Header();
            bool isMuted = view.Muted();
            float volume = static_cast<float>(view.Volume()) / 100.f;
            uint32_t index = 0;
            AudioSessions().IndexOf(view, index);

            audioProfile.AudioLevels().Insert(header, volume);
            audioProfile.AudioStates().Insert(header, isMuted);
            audioProfile.SessionsIndexes().Insert(header, index);
        }

        audioProfile.DisableAnimations(DisableAnimationsCheckBox().IsChecked().GetBoolean());
        audioProfile.ShowAdditionalButtons(ShowAdditionalButtonsCheckBox().IsChecked().GetBoolean());
        audioProfile.KeepOnTop(KeepOnTopCheckBox().IsChecked().GetBoolean());
        audioProfile.ShowMenu(ShowMenuCheckBox().IsChecked().GetBoolean());
        audioProfile.Layout(LayoutComboBox().SelectedIndex());

        timer.Start();

        // Save the profile.
        winrt::Windows::Storage::ApplicationDataContainer audioProfilesContainer = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Containers().TryLookup(L"AudioProfiles");
        if (!audioProfilesContainer)
        {
            audioProfilesContainer = ApplicationData::Current().LocalSettings().CreateContainer(L"AudioProfiles", ApplicationDataCreateDisposition::Always);
        }

        audioProfile.Save(audioProfilesContainer);
    }
}