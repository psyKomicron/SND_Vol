#include "pch.h"
#include "AudioSessionView.xaml.h"
#if __has_include("AudioSessionView.g.cpp")
#include "AudioSessionView.g.cpp"
#endif

#include <math.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Windows::Foundation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SND_Vol::implementation
{
    AudioSessionView::AudioSessionView()
    {
        InitializeComponent();
    }

    AudioSessionView::AudioSessionView(winrt::hstring const& header, double const& volume) : AudioSessionView()
    {
        _header = header;
        _volume = round(volume);
        SetGlyph();
    }


    winrt::hstring AudioSessionView::VolumeGlyph()
    {
        return _volumeGlyph;
    }

    double AudioSessionView::Volume()
    {
        return _volume;
    }

    void AudioSessionView::Volume(double const& value)
    {
        if (value > 0.0 && value < 100.0)
        {
            _volume = value;
            e_propertyChanged(*this, PropertyChangedEventArgs(L"Volume"));
        }
    }

    hstring AudioSessionView::Header()
    {
        return _header;
    }

    void AudioSessionView::Header(hstring const& value)
    {
        _header = value;
        e_propertyChanged(*this, PropertyChangedEventArgs(L"Header"));
    }

    guid AudioSessionView::Id()
    {
        return _id;
    }

    void AudioSessionView::Id(guid const& value)
    {
        _id = value;
        //e_propertyChanged(*this, PropertyChangedEventArgs(L"Id"));
    }

    bool AudioSessionView::Muted()
    {
        return _muted;
    }

    void AudioSessionView::Muted(bool const& value)
    {
        _muted = value;
        e_propertyChanged(*this, PropertyChangedEventArgs(L"Muted"));

        if (_muted)
        {
            _volumeGlyph = L"\ue74f";
        }
        else
        {
            SetGlyph();
        }

        e_propertyChanged(*this, PropertyChangedEventArgs(L"VolumeGlyph"));
    }


    winrt::event_token AudioSessionView::PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
    {
        return e_propertyChanged.add(value);
    }

    void AudioSessionView::PropertyChanged(winrt::event_token const& token)
    {
        e_propertyChanged.remove(token);
    }

    winrt::event_token AudioSessionView::VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs> const& handler)
    {
        return e_volumeChanged.add(handler);
    }

    void AudioSessionView::VolumeChanged(winrt::event_token const& token)
    {
        e_volumeChanged.remove(token);
    }

    winrt::event_token AudioSessionView::VolumeStateChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::SND_Vol::AudioSessionView, bool> const& handler)
    {
        return e_volumeStateChanged.add(handler);
    }

    void AudioSessionView::VolumeStateChanged(winrt::event_token const& token)
    {
        e_volumeStateChanged.remove(token);
    }


    void AudioSessionView::Slider_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        _volume = e.NewValue();
        SetGlyph();

        if (!_muted)
        {
            e_propertyChanged(*this, PropertyChangedEventArgs(L"VolumeGlyph"));
        }
        e_propertyChanged(*this, PropertyChangedEventArgs(L"Volume"));
        e_volumeChanged(*this, e);
    }

    void AudioSessionView::MuteToggleButton_Click(IInspectable const&, RoutedEventArgs const& e)
    {
        if (MuteToggleButton().IsChecked().GetBoolean())
        {
            _volumeGlyph = L"\ue74f";
        }
        else
        {
            SetGlyph();
        }

        e_propertyChanged(*this, PropertyChangedEventArgs(L"VolumeGlyph"));
        e_volumeStateChanged(*this, MuteToggleButton().IsChecked().GetBoolean());
    }

    void AudioSessionView::SetGlyph()
    {
        if (_volume > 75.0)
        {
            _volumeGlyph = L"\ue995";// 🔊
        }
        else if (_volume > 50.0)
        {
            _volumeGlyph = L"\ue994";
        }
        else if (_volume > 5.0)
        {
            _volumeGlyph = L"\ue993";// 🔉
        }
        else if (_volume > 0)
        {
            _volumeGlyph = L"\ue992";// 🔈
        }
        else
        {
            _volumeGlyph = L"\ue74f";// 🔇
        }
    }
}
