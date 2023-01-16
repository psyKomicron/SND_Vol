#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include "SecondWindow.xaml.h"

using namespace winrt;

using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;

using namespace Windows::Foundation;

using namespace SND_Vol;
using namespace SND_Vol::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const& e)
{
    #if defined DEBUG & FALSE
    window = make<SecondWindow>();
    window.Activate();
    #else
    window = make<MainWindow>(e.Arguments());
    window.Activate();
    #endif
}