﻿#pragma once

#include "App.xaml.g.h"
#include "KeyIcon.h"
#include "LetterKeyIcon.h"
#include "IconButton.h"
#include "IconToggleButton.h"
#include "NumberBlock.h"

namespace winrt::SND_Vol::implementation
{
    enum AudioSessionState
    {
        Active,
        Inactive,
        Muted,
        Expired
    };

    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
}
