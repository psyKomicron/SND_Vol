// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "AudioProfileEditPage.xaml.h"
#if __has_include("AudioProfileEditPage.g.cpp")
#include "AudioProfileEditPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SND_Vol::implementation
{
    AudioProfileEditPage::AudioProfileEditPage()
    {
        InitializeComponent();

        timer = DispatcherQueue().CreateTimer();
        timer.Interval(ProfileSavedAnimation().Duration().TimeSpan + std::chrono::milliseconds(500));
        timer.IsRepeating(false);
        timer.Tick([this](auto, auto)
        {
            Frame().GoBack();
        });
    }

    void AudioProfileEditPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        audioProfile = args.Parameter().try_as<AudioProfile>();

        if (!audioProfile)
        {
            Frame().GoBack();
        }
    }

    void AudioProfileEditPage::Page_Loading(FrameworkElement const&, IInspectable const& args)
    {
        ProfileNameEditTextBox().Text(audioProfile.ProfileName());

        // TODO: Use objects in Audio namespace.
        audioSessions = MainWindow::Current().AudioSessions().GetView();
        for (auto audioSession : audioSessions)
        {
            AudioSessionView copy{};
            copy.Header(audioSession.Header());
            copy.Volume(audioSession.Volume());
            copy.Muted(audioSession.Muted());
            copy.ContextFlyout(nullptr);

            AudioSessionsGridView().Items().Append(copy);
        }
    }

    void AudioProfileEditPage::NextButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        VisualStateManager::GoToState(*this, L"ProfileSaved", true);

        audioProfile.ProfileName(ProfileNameEditTextBox().Text());
        audioProfile.AudioLevels().Clear();
        audioProfile.AudioStates().Clear();

        for (auto&& item : AudioSessionsGridView().Items())
        {
            if (AudioSessionView view = item.try_as<AudioSessionView>())
            {
                hstring header = view.Header();
                bool isMuted = view.Muted();
                float volume = static_cast<float>(view.Volume()); // Cast should be safe.

                audioProfile.AudioLevels().Insert(header, volume);
                audioProfile.AudioStates().Insert(header, isMuted);
            }
        }

        timer.Start();
    }

    void AudioProfileEditPage::CancelButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Frame().GoBack();
    }
}
