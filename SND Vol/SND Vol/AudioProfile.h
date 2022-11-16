#pragma once

#include "AudioProfile.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioProfile : AudioProfileT<AudioProfile>
    {
        AudioProfile() = default;

        winrt::hstring ProfileName()
        {
            return _profileName;
        };
        void ProfileName(const winrt::hstring& value)
        {
            _profileName = value;
        };

        winrt::Windows::Foundation::Collections::IMap<hstring, float> AudioLevels()
        {
            return _audioLevels;
        };
        void AudioLevels(const winrt::Windows::Foundation::Collections::IMap<hstring, float>& value)
        {
            _audioLevels = value;
        };

        winrt::Windows::Foundation::Collections::IMap<hstring, bool> AudioStates()
        {
            return _audioStates;
        };
        void AudioStates(const winrt::Windows::Foundation::Collections::IMap<hstring, bool>& value)
        {
            _audioStates = value;
        };

        bool IsDefaultProfile()
        {
            return _isDefaultProfile;
        };
        void IsDefaultProfile(const bool& value)
        {
            _isDefaultProfile = value;
        };

    private:
        winrt::hstring _profileName;
        winrt::Windows::Foundation::Collections::IMap<hstring, float> _audioLevels{};
        winrt::Windows::Foundation::Collections::IMap<hstring, bool> _audioStates{};
        bool _isDefaultProfile = false;
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioProfile : AudioProfileT<AudioProfile, implementation::AudioProfile>
    {
    };
}
