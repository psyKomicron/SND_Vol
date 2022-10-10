#include "pch.h"
#include "LegacyAudioController.h"

#ifdef _DEBUG
#include <rpc.h>
#endif // _DEBUG

using namespace std;
using namespace winrt;

namespace Audio
{
    LegacyAudioController::LegacyAudioController(GUID const& guid) :
        audioSessionID(guid)
    {
        CreateSessionManager();
    }


    bool LegacyAudioController::Register()
    {
        if (!isRegistered)
        {
            isRegistered = audioSessionManager && SUCCEEDED(audioSessionManager->RegisterSessionNotification(this));
        }
        return isRegistered;
    }

    bool LegacyAudioController::Unregister()
    {
        return SUCCEEDED(audioSessionManager->UnregisterSessionNotification(this));
    }

    #pragma region IUnknown
    IFACEMETHODIMP_(ULONG) LegacyAudioController::AddRef()
    {
        return ++refCount;
    }

    IFACEMETHODIMP_(ULONG) LegacyAudioController::Release()
    {
        const uint32_t remaining = --refCount;

        if (remaining == 0)
        {
            delete this;
        }

        return remaining;
    }

    IFACEMETHODIMP LegacyAudioController::QueryInterface(REFIID riid, VOID** ppvInterface)
    {
        if (riid == IID_IUnknown)
        {
            *ppvInterface = static_cast<IUnknown*>(static_cast<IAudioSessionNotification*>(this));
            AddRef();
        }
        else if (riid == __uuidof(IAudioSessionNotification))
        {
            *ppvInterface = static_cast<IAudioSessionNotification*>(this);
            AddRef();
        }
        else
        {
            *ppvInterface = NULL;
            return E_POINTER;
        }
        return S_OK;
    }
    #pragma endregion

    vector<AudioSessionContainer>* LegacyAudioController::GetSessions()
    {
        if (!audioSessionManager)
        {
            CreateSessionManager();
        }

        vector<AudioSessionContainer>* sessions = nullptr;
        IAudioSessionEnumeratorPtr enumerator;

        if (SUCCEEDED(audioSessionManager->GetSessionEnumerator(&enumerator)))
        {
            sessions = new vector<AudioSessionContainer>();
            int sessionCount;
            if (SUCCEEDED(enumerator->GetCount(&sessionCount)))
            {
                for (int i = 0; i < sessionCount; i++)
                {
                    IAudioSessionControlPtr control;
                    if (SUCCEEDED(enumerator->GetSession(i, &control)))
                    {
                        GUID grouping{};
                        if (SUCCEEDED(control->GetGroupingParam(&grouping)))
                        {
                            IAudioSessionControl2Ptr control2;
                            if (SUCCEEDED(control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2)))
                            {
                                sessions->push_back(make_unique<AudioSession>(std::move(control2), audioSessionID));
                            }
                        }
                    }
                }
            }
        }
        return sessions;
    }

    AudioSession* LegacyAudioController::NewSession()
    {
        if (newSessions.empty())
        {
            return nullptr;
        }
        else
        {
            return newSessions.top().release();
        }
    }

    MainAudioEndpoint* LegacyAudioController::GetMainAudioEndpoint()
    {
        IMMDevicePtr pDevice;
        IMMDeviceEnumeratorPtr pEnumerator;

        // Create the device enumerator.
        check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator)));
        // Get the default audio device.
        check_hresult(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice));

        IAudioEndpointVolume* audioEndpointVolume = nullptr;
        check_hresult(pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&audioEndpointVolume));

        return new MainAudioEndpoint(audioEndpointVolume, audioSessionID);
    }

    IAudioMeterInformation* LegacyAudioController::GetMainAudioEnpointMeterInfo()
    {
        IMMDevicePtr pDevice;
        IMMDeviceEnumeratorPtr pEnumerator;

        // Create the device enumerator.
        if (FAILED(CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator))))
        {
            OutputDebugString(L"Failed to create device enumerator (LegacyAudioController::GetMainAudioEndpoint).\n");
            return nullptr;
        }

        // Get the default audio device.
        if (FAILED(pEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &pDevice)))
        {
            OutputDebugString(L"Failed to get default audio endpoint (LegacyAudioController::GetMainAudioEndpoint).\n");
            return nullptr;
        }

        IAudioMeterInformation* audioMeterInfo = nullptr;
        if (FAILED(pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&audioMeterInfo)))
        {
            return nullptr;
        }
        else
        {
            return audioMeterInfo;
        }
    }

    #pragma region Events
    winrt::event_token LegacyAudioController::SessionAdded(const winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>& handler)
    {
        return e_sessionAdded.add(handler);
    }

    void LegacyAudioController::SessionAdded(const winrt::event_token& token)
    {
        e_sessionAdded.remove(token);
    }
    #pragma endregion


    void LegacyAudioController::CreateSessionManager()
    {
        // TODO: Use exceptions or more meaningful error messages

        IMMDevicePtr pDevice;
        IMMDeviceEnumeratorPtr pEnumerator;

        // Create the device enumerator.
        check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator)));
        // Get the default audio device.
        check_hresult(pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice));

        // Get the session manager.
        check_hresult(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&audioSessionManager));
    }

    HRESULT __stdcall LegacyAudioController::OnSessionCreated(IAudioSessionControl* NewSession)
    {
        // HACK: Audio session creation notifications are sent in double. Once we receive one, we will ignore the next. This can cause non doubled events to be ignored.
        if (!ignoreNotification)
        {
            IAudioSessionControl2Ptr control2;
            if (SUCCEEDED(NewSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2)))
            {
                OutputDebugString(L"New session created\n");
                newSessions.push(AudioSessionContainer(new AudioSession(std::move(control2), audioSessionID)));

                e_sessionAdded(winrt::Windows::Foundation::IInspectable(), winrt::Windows::Foundation::IInspectable());
            }
        }
        ignoreNotification = !ignoreNotification;

        return S_OK;
    }
}