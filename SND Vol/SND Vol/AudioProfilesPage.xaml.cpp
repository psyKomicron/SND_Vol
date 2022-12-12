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
using namespace winrt::Windows::Storage;


namespace winrt::SND_Vol::implementation
{
    AudioProfilesPage::AudioProfilesPage()
    {
        InitializeComponent();

        audioProfiles.VectorChanged([this](auto sender, IVectorChangedEventArgs args)
        {
            static bool reordering = false;

            if (args.CollectionChange() == CollectionChange::ItemInserted && reordering)
            {
                if (audioProfiles.Size() > 0)
                {
                    audioProfiles.GetAt(0).IsDefaultProfile(true);
                    // Save the new default profile.
                    ApplicationDataContainer audioProfilesContainer = ApplicationData::Current().LocalSettings().Containers().Lookup(L"AudioProfiles");
                    audioProfiles.GetAt(0).Save(audioProfilesContainer);

                    for (uint32_t i = 1; i < audioProfiles.Size(); i++)
                    {
                        audioProfiles.GetAt(i).IsDefaultProfile(false);
                        // Save the newly edited profile.
                        audioProfiles.GetAt(i).Save(audioProfilesContainer);
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


    void AudioProfilesPage::Page_Loading(FrameworkElement const&, IInspectable const&)
    {
        // Load profiles into the page
        ApplicationDataContainer audioProfilesContainer = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"AudioProfiles");
        if (!audioProfilesContainer)
        {
            audioProfilesContainer = ApplicationData::Current().LocalSettings().CreateContainer(L"AudioProfiles", ApplicationDataCreateDisposition::Always);
        }
        else
        {
            for (auto&& profile : audioProfilesContainer.Containers())
            {
                try
                {
                    hstring key = profile.Key();
                    AudioProfile audioProfile{};
                    audioProfile.Restore(profile.Value());
                    audioProfiles.Append(audioProfile);
                }
                catch (const hresult_error&)
                {
                }
            }

            if (audioProfiles.Size() == 1)
            {
                audioProfiles.GetAt(0).IsDefaultProfile(true);
            }
            else
            {
                for (uint32_t i = 0; i < static_cast<uint32_t>(audioProfiles.Size()); i++)
                {
                    if (audioProfiles.GetAt(i).IsDefaultProfile())
                    {
                        AudioProfile audioProfile = audioProfiles.GetAt(i);
                        audioProfiles.RemoveAt(i);
                        audioProfiles.InsertAt(0, audioProfile);
                    }
                }
            }
        }
    }

    void AudioProfilesPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        if (args.NavigationMode() != ::Navigation::NavigationMode::Back && 
            SecondWindow::Current().Breadcrumbs().GetAt(SecondWindow::Current().Breadcrumbs().Size() - 1).ItemTypeName().Name != xaml_typename<winrt::SND_Vol::AudioProfilesPage>().Name)
        {
            winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
            SecondWindow::Current().Breadcrumbs().Append(
                NavigationBreadcrumbBarItem{ loader.GetString(L"AudioProfilesPageDisplayName"), xaml_typename<winrt::SND_Vol::AudioProfilesPage>() }
            );
        }

        //AudioProfile profile = args.Parameter().try_as<AudioProfile>();

        //if (profile)
        //{
        //    for (auto&& audioProfile : audioProfiles)
        //    {
        //        if (audioProfile.ProfileName() == profile.ProfileName())
        //        {
        //            // TODO: Show error that the profile already exist.
        //            /*ErrorTextBlock().Text(L"Profile name already exists");
        //            ErrorTextBlock().Visibility(Visibility::Visible);*/
        //            return;
        //        }
        //    }

        //    // If we reach here the profile name is unique.
        //    audioProfiles.Append(profile);
        //}
    }

    void AudioProfilesPage::Page_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        DefaultProfileTeachingTip().Target(ProfilesListView());

        ApplicationDataContainer teachingTips = ApplicationData::Current().LocalSettings().Containers().TryLookup(L"TeachingTips");
        if (!teachingTips)
        {
            teachingTips = ApplicationData::Current().LocalSettings().CreateContainer(L"TeachingTips", ApplicationDataCreateDisposition::Always);
        }
        if (!teachingTips.Values().HasKey(L"ShowDefaultProfileTeachingTip"))
        {
            DefaultProfileTeachingTip().IsOpen(true);
            teachingTips.Values().Insert(L"ShowDefaultProfileTeachingTip", IReference<bool>(false));
        }
    }

    void AudioProfilesPage::AddProfileButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        // I18N: Audio profile.
        hstring profileName = L"Audio profile";
        uint32_t count = 1u;
        for (uint32_t j = 0; j < audioProfiles.Size(); j++)
        {
            hstring test = audioProfiles.GetAt(j).ProfileName();
            if (test == profileName)
            {
                profileName = L"Audio profile (" + to_hstring(count++) + L")";
            }
        }

        Frame().Navigate(xaml_typename<AudioProfileEditPage>(), box_value(profileName), ::Media::Animation::DrillInNavigationTransitionInfo());
    }

    void AudioProfilesPage::DeleteProfileButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        hstring tag = sender.as<Button>().Tag().as<hstring>();
        for (uint32_t i = 0; i < audioProfiles.Size(); i++)
        {
            if (audioProfiles.GetAt(i).ProfileName() == tag)
            {
                ApplicationDataContainer audioProfilesContainer = ApplicationData::Current().LocalSettings().Containers().Lookup(L"AudioProfiles");
                audioProfilesContainer.DeleteContainer(audioProfiles.GetAt(i).ProfileName());

                audioProfiles.RemoveAt(i);
                break;
            }
        }
    }

    IAsyncAction AudioProfilesPage::EditProfileButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        hstring tag = sender.as<Button>().Tag().as<hstring>();
        for (uint32_t i = 0; i < audioProfiles.Size(); i++)
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
        for (uint32_t i = 0; i < audioProfiles.Size(); i++)
        {
            if (audioProfiles.GetAt(i).ProfileName() == tag)
            {
                // TODO: Copy the profile values to the new one.

                AudioProfile profile{};
                hstring profileName = audioProfiles.GetAt(i).ProfileName();

                uint32_t count = 1u;
                for (uint32_t j = 0; j < audioProfiles.Size(); j++)
                {
                    hstring test = audioProfiles.GetAt(j).ProfileName();
                    if (test == profileName)
                    {
                        profileName = audioProfiles.GetAt(i).ProfileName() + L" (" + to_hstring(count++) + L")";
                    }
                }

                profile.ProfileName(profileName);
                profile.IsDefaultProfile(false);

                audioProfiles.Append(profile);

                break;
            }
        }
    }
}
