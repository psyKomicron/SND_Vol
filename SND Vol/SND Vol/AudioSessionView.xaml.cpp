#include "pch.h"
#include "AudioSessionView.xaml.h"
#if __has_include("AudioSessionView.g.cpp")
#include "AudioSessionView.g.cpp"
#endif

#include <math.h>
#include <limits>

using namespace winrt;

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Input;
using namespace winrt::Microsoft::UI::Xaml::Controls::Primitives;
using namespace winrt::Microsoft::UI::Xaml::Media;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;


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

    winrt::event_token AudioSessionView::Hidden(TypedEventHandler<winrt::SND_Vol::AudioSessionView, IInspectable> const& handler)
    {
        return e_hidden.add(handler);
    }

    void AudioSessionView::Hidden(winrt::event_token const& token)
    {
        e_hidden.remove(token);
    }
    #pragma endregion


    void AudioSessionView::SetState(const AudioSessionState& state)
    {
        auto resources = Application::Current().Resources();
        switch (state)
        {
            case AudioSessionState::Active:
                isActive = true;
                VolumeFontIcon().Foreground(::Media::SolidColorBrush(Windows::UI::Colors::LimeGreen()));
                VolumeFontIcon().Opacity(1);
                break;

            case AudioSessionState::Expired:
            case AudioSessionState::Inactive:
            default:
                isActive = false;
                VolumeFontIcon().Foreground(::Media::SolidColorBrush(Windows::UI::Colors::WhiteSmoke()));
                VolumeFontIcon().Opacity(0.6);
                break;
        }
    }

    void AudioSessionView::SetPeak(float peak)
    {
        if (!isActive) return;
        
        /*if (peak > 0.9f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::LimeGreen()));
        }
        else if (peak > 0.5f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::Green()));
        }
        else if (peak > 0.2f)
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::DarkGreen()));
        }
        else
        {
            StatusEllipse().Fill(SolidColorBrush(Colors::Transparent()));
        }*/

        LeftPeakAnimation().To(static_cast<double>(peak));
        LeftPeakStoryboard().Begin();
    }

    void AudioSessionView::SetPeak(const float& peak1, const float& peak2)
    {
        LeftPeakAnimation().To(static_cast<double>(peak1));
        LeftPeakStoryboard().Begin();

        RightPeakAnimation().To(static_cast<double>(peak2));
        RightPeakStoryboard().Begin();
    }


    void AudioSessionView::UserControl_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        Grid_SizeChanged(nullptr, nullptr);
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
        _muted = !_muted;
        if (_muted)
        {
            _volumeGlyph = L"\ue74f";
        }
        else
        {
            SetGlyph();
        }

        e_propertyChanged(*this, PropertyChangedEventArgs(L"VolumeGlyph"));
        e_volumeStateChanged(*this, _muted);
    }

    void AudioSessionView::Grid_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        /*double height = CenterRow().ActualHeight();
        VolumePeakBorder().Height(height);
        ::Numerics::float3 translation = VolumePeakBorder().Translation();
        translation.y = height;
        VolumePeakBorder().Translation(translation);*/
    }

    void AudioSessionView::RootGrid_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, PointerRoutedEventArgs const&)
    {
        VisualStateManager::GoToState(*this, L"PointerOver", true);
    }

    void AudioSessionView::RootGrid_PointerExited(IInspectable const&, PointerRoutedEventArgs const&)
    {
        VisualStateManager::GoToState(*this, L"Normal", true);
    }

    void AudioSessionView::RootGrid_RightTapped(IInspectable const&, RightTappedRoutedEventArgs const&)
    {
    }

    void AudioSessionView::LockMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (isLocked)
        {
            VisualStateManager::GoToState(*this, L"Unlocked", true);
        }
        else
        {
            VisualStateManager::GoToState(*this, L"Locked", true);
        }

        isLocked = !isLocked;
    }

    void AudioSessionView::HideMenuFlyoutItem_Click(IInspectable const&, RoutedEventArgs const&)
    {
        e_hidden(*this, IInspectable());
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
