#include "pch.h"
#include "AudioSessionView.xaml.h"
#if __has_include("AudioSessionView.g.cpp")
#include "AudioSessionView.g.cpp"
#endif

#include <math.h>
#include <limits>
using namespace winrt::Windows::UI;

using namespace winrt;

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Media;

using namespace winrt::Windows::Foundation;


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


    #pragma region Properties
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
    #pragma endregion


    #pragma region Events
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
    #pragma endregion


    void AudioSessionView::SetStatus(const AudioSessionState& state)
    {
        auto resources = Application::Current().Resources();
        switch (state)
        {
            case AudioSessionState::Active:
                isActive = true;
                StatusEllipse().Fill(::Media::SolidColorBrush(Windows::UI::Colors::Green()));
                StatusEllipse().Stroke(::Media::SolidColorBrush(Windows::UI::Colors::Green()));
                break;

            case AudioSessionState::Expired:
            case AudioSessionState::Inactive:
            default:
                isActive = false;
                StatusEllipse().Fill(::Media::SolidColorBrush(Windows::UI::Colors::Transparent()));
                StatusEllipse().Stroke(::Media::SolidColorBrush(Windows::UI::Colors::Orange()));
                break;
        }
    }

    void AudioSessionView::SetPeak(float peak)
    {
        if (!isActive) return;

        if (peak > 0.9f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::LimeGreen()));
            //StatusEllipse().Stroke(SolidColorBrush(Colors::Transparent()));
        }
        else if (peak > 0.5f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::Green()));
        }
        else if (peak > 0.2f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::DarkGreen()));
            //StatusEllipse().Stroke(SolidColorBrush(Colors::Transparent()));
        }
        else //if (peak > 0.1f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::Transparent()));
            //StatusEllipse().Stroke(SolidColorBrush(Colors::Green()));
        }
    }


    void AudioSessionView::Slider_ValueChanged(IInspectable const&, RangeBaseValueChangedEventArgs const& e)
    {
        _volume = e.NewValue();

        if (!_muted)
        {
            SetGlyph();
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

    void AudioSessionView::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        /*double height = CenterRow().ActualHeight();
        VolumePeakBorder().Height(height);
        ::Numerics::float3 translation = VolumePeakBorder().Translation();
        translation.y = height;
        VolumePeakBorder().Translation(translation);*/
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


void winrt::SND_Vol::implementation::AudioSessionView::UserControl_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    Grid_SizeChanged(nullptr, nullptr);
}
