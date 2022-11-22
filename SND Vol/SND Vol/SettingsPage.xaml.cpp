#include "pch.h"
#include "SettingsPage.xaml.h"
#if __has_include("SettingsPage.g.cpp")
#include "SettingsPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Navigation;
using namespace winrt::Windows::UI::Xaml::Interop;


namespace winrt::SND_Vol::implementation
{
    SettingsPage::SettingsPage()
    {
        InitializeComponent();
    }

    void SettingsPage::OnNavigatedTo(NavigationEventArgs const& args)
    {
        winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
        SecondWindow::Current().Breadcrumbs().Append(
            NavigationBreadcrumbBarItem{ loader.GetString(L"SettingsPageDisplayName"), xaml_typename<winrt::SND_Vol::SettingsPage>() }
        );
    }

    void SettingsPage::AudioProfilesButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Frame().Navigate(xaml_typename<AudioProfilesPage>());
    }

    void SettingsPage::HotKeysButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Frame().Navigate(xaml_typename<HotKeysPage>());

        winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
        SecondWindow::Current().Breadcrumbs().Append(
            winrt::SND_Vol::NavigationBreadcrumbBarItem{ loader.GetString(L"HotKeysPageDisplayName"), xaml_typename<winrt::SND_Vol::HotKeysPage>() }
        );
    }
}
