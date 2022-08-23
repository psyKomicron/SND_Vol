#pragma once

#include "AudioController.g.h"

namespace winrt::SND_Vol::implementation
{
    struct AudioController : AudioControllerT<AudioController>
    {
        AudioController() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::SND_Vol::factory_implementation
{
    struct AudioController : AudioControllerT<AudioController, implementation::AudioController>
    {
    };
}
