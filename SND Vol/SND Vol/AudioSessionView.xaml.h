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

        void SetState(const winrt::SND_Vol::AudioSessionState& state);
        void SetPeak(float peak);

        void Slider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
        void MuteToggleButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Grid_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

    private:
        double _volume = 50.0;
        winrt::hstring _volumeGlyph = L"\ue994";
        winrt::hstring _header = L"";
        winrt::guid _id{};
        bool _muted = false;
        float lastPeak = 0;
        bool isActive;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs>>
            e_volumeChanged;
        winrt::event< winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, bool> > e_volumeStateChanged;

        void SetGlyph();
    public:
        void UserControl_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioSessionView : AudioSessionViewT<AudioSessionView, implementation::AudioSessionView>
    {
    };
}
