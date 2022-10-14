#include "pch.h"
#include "LegacyAudioController.h"

using namespace std;
using namespace winrt;


namespace Audio
{
    LegacyAudioController::LegacyAudioController(GUID const& guid) :
        audioSessionID(guid)
    {
        IMMDevicePtr pDevice;

        // Create the device enumerator.
        check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&deviceEnumerator)));
        // Get the default audio device.
        check_hresult(deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice));
        // Get the session manager.
        check_hresult(pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&audioSessionManager));
        // Get session enumerator
        check_hresult(audioSessionManager->GetSessionEnumerator(&audioSessionEnumerator));
    }


    #pragma region  Events
    winrt::event_token LegacyAudioController::SessionAdded(const winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>& handler)
    {
        return e_sessionAdded.add(handler);
    }

    void LegacyAudioController::SessionAdded(const winrt::event_token& token)
    {
        e_sessionAdded.remove(token);
    }
    #pragma endregion


    bool LegacyAudioController::Register()
    {
        if (!isRegistered)
        {
            isRegistered = 
                SUCCEEDED(audioSessionManager->RegisterSessionNotification(this)) && 
                SUCCEEDED(deviceEnumerator->RegisterEndpointNotificationCallback(this));
        }
        return isRegistered;
    }

    bool LegacyAudioController::Unregister()
    {
        return SUCCEEDED(audioSessionManager->UnregisterSessionNotification(this)) && SUCCEEDED(deviceEnumerator->UnregisterEndpointNotificationCallback(this));
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
        else if (riid == __uuidof(IMMNotificationClient))
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
        vector<AudioSessionContainer>* sessions = new vector<AudioSessionContainer>();
        int sessionCount;
        if (SUCCEEDED(audioSessionEnumerator->GetCount(&sessionCount)))
        {
            for (int i = 0; i < sessionCount; i++)
            {
                GUID groupingParam{};
                IAudioSessionControlPtr control;
                IAudioSessionControl2* control2 = nullptr;
                if (SUCCEEDED(audioSessionEnumerator->GetSession(i, &control)) &&
                    SUCCEEDED(control->GetGroupingParam(&groupingParam)) &&
                    SUCCEEDED(control->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2)))
                {
                    sessions->push_back(make_unique<AudioSession>(control2, audioSessionID, i));
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
        IMMDevice* pDevice = nullptr;
        // Get the default audio device.
        check_hresult(deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice));
        return new MainAudioEndpoint(pDevice, audioSessionID);
    }


    STDMETHODIMP LegacyAudioController::OnSessionCreated(IAudioSessionControl* NewSession) noexcept
    {
        // HACK: Audio session creation notifications are sent in double. Once we receive one, we will ignore the next. This can cause non doubled events to be ignored.
        if (!ignoreNotification)
        {
            IAudioSessionControl2* control2 = nullptr;
            if (SUCCEEDED(NewSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2)))
            {
                int count = 0;
                audioSessionEnumerator->GetCount(&count);
                if (count > 0)
                {
                    count--;
                }
                newSessions.push(AudioSessionContainer(new AudioSession(control2, audioSessionID, count)));

                e_sessionAdded(winrt::Windows::Foundation::IInspectable(), winrt::Windows::Foundation::IInspectable());
            }
        }
        ignoreNotification = !ignoreNotification;

        return S_OK;
    }

    STDMETHODIMP LegacyAudioController::OnDefaultDeviceChanged(__in EDataFlow flow, __in  ERole /*role*/, __in_opt LPCWSTR pwstrDefaultDeviceId) noexcept
    {
        static bool notified = false;

        if (!notified && flow == EDataFlow::eRender)
        {
            OutputDebugHString(L"Default device changed (id: " + to_hstring(pwstrDefaultDeviceId) + L").");

            e_endpointChanged(winrt::Windows::Foundation::IInspectable(), winrt::Windows::Foundation::IInspectable());
        }
        else
        {
            OutputDebugHString(L"Ignored notification: Default device changed (id: " + to_hstring(pwstrDefaultDeviceId) + L").");
        }

        notified = !notified;
        return S_OK;
    }

    STDMETHODIMP LegacyAudioController::OnDeviceStateChanged(__in LPCWSTR pwstrDeviceId, __in DWORD /*dwNewState*/) noexcept
    {
        return S_OK;
    }
}