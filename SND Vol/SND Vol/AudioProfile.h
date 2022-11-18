#pragma once

#include "AudioProfile.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioProfile : AudioProfileT<AudioProfile>
    {
        AudioProfile() = default;

        inline winrt::hstring ProfileName()
        {
            return _profileName;
        };
        inline void ProfileName(const winrt::hstring& value)
        {
            _profileName = value;
            e_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(L"ProfileName"));
        };

        inline winrt::Windows::Foundation::Collections::IMap<hstring, float> AudioLevels()
        {
            return _audioLevels;
        };
        inline void AudioLevels(const winrt::Windows::Foundation::Collections::IMap<hstring, float>& value)
        {
            _audioLevels = value;
        };

        inline winrt::Windows::Foundation::Collections::IMap<hstring, bool> AudioStates()
        {
            return _audioStates;
        };
        inline void AudioStates(const winrt::Windows::Foundation::Collections::IMap<hstring, bool>& value)
        {
            _audioStates = value;
        };

        inline bool IsDefaultProfile()
        {
            return _isDefaultProfile;
        };
        inline void IsDefaultProfile(const bool& value)
        {
            _isDefaultProfile = value;
            e_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(L"IsDefaultProfile"));
        };

        inline winrt::event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            return e_propertyChanged.add(handler);
        };
        inline void PropertyChanged(winrt::event_token const& token)
        {
            e_propertyChanged.remove(token);
        };

    private:
        winrt::hstring _profileName;
        winrt::Windows::Foundation::Collections::IMap<hstring, float> _audioLevels{ winrt::single_threaded_map<hstring, float>() };
        winrt::Windows::Foundation::Collections::IMap<hstring, bool> _audioStates{ winrt::single_threaded_map<hstring, bool>() };
        bool _isDefaultProfile = false;

        winrt::event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> e_propertyChanged;
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioProfile : AudioProfileT<AudioProfile, implementation::AudioProfile>
    {
    };
}
