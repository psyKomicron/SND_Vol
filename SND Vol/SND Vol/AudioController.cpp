#include "pch.h"
#include "AudioController.h"
#if __has_include("AudioController.g.cpp")
#include "AudioController.g.cpp"
#endif

namespace winrt::SND_Vol::implementation
{
    int32_t AudioController::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void AudioController::MyProperty(int32_t /*value*/)
    {
        throw hresult_not_implemented();
    }
}
