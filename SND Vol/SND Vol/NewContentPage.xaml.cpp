#include "pch.h"
#include "NewContentPage.xaml.h"
#if __has_include("NewContentPage.g.cpp")
#include "NewContentPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SND_Vol::implementation
{
    NewContentPage::NewContentPage()
    {
        InitializeComponent();
    }

    void NewContentPage::OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
    {
        SecondWindow::Current().Breadcrumbs().Append(NavigationBreadcrumbBarItem(L"What's new", xaml_typename<winrt::SND_Vol::NewContentPage>()));
    }
}
