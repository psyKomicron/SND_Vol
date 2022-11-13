#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "AudioSessionView.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioSessionView : AudioSessionViewT<AudioSessionView>
    {
    public:
        AudioSessionView();
        AudioSessionView(winrt::hstring const& header, double const& volume);

        winrt::hstring Header();
        void Header(winrt::hstring const& value);
        winrt::guid Id();
        void Id(winrt::guid const& value);  
        bool Muted();
        void Muted(bool const& value);
        double Volume();
        void Volume(double const& value);
        winrt::hstring VolumeGlyph();

        winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value);
        void PropertyChanged(winrt::event_token const& token);
        winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs> const& handler);
        void VolumeChanged(winrt::event_token const& token);
        winrt::event_token VolumeStateChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, bool> const& handler);
        void VolumeStateChanged(winrt::event_token const& token);
        winrt::event_token Hidden(winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, winrt::Windows::Foundation::IInspectable> const& handler);
        void Hidden(winrt::event_token const& token);

        void SetState(const winrt::SND_Vol::AudioSessionState& state);
        void SetPeak(float peak);
        void SetPeak(const float& peak1, const float& peak2);

        void Slider_ValueChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void MuteToggleButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void UserControl_Loaded(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void RootGrid_PointerEntered(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&);
        void RootGrid_PointerExited(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&);
        void RootGrid_RightTapped(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::RightTappedRoutedEventArgs const& e);
        void LockMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void HideMenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        double _volume = 50.0;
        winrt::hstring _volumeGlyph = L"\ue994";
        winrt::hstring _header = L"";
        winrt::guid _id{};
        bool _muted = false;
        float lastPeak = 0;
        bool isActive = false;
        bool isLocked = false;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs>>
            e_volumeChanged;
        winrt::event< winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, bool> > e_volumeStateChanged;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, winrt::Windows::Foundation::IInspectable>> e_hidden;

        void SetGlyph();
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioSessionView : AudioSessionViewT<AudioSessionView, implementation::AudioSessionView>
    {
    };
}
