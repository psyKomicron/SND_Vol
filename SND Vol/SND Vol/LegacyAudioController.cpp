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
#ifdef _DEBUG
        if (FAILED(UuidCreate(&managerID)))
        {
            canUseGuid = false;
            OutputDebugString(L"Failed to create manager GUID.");
        }
#endif // _DEBUG

    }

    vector<AudioSessionContainer>* LegacyAudioController::GetSessions()
    {
        if (!CreateSessionManager())
        {
            return nullptr;
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

    bool LegacyAudioController::Register()
    {
        return audioSessionManager && SUCCEEDED(audioSessionManager->RegisterSessionNotification(this));
    }

    bool LegacyAudioController::Unregister()
    {
        return SUCCEEDED(audioSessionManager->UnregisterSessionNotification(this));
    }

    IFACEMETHODIMP_(ULONG) LegacyAudioController::AddRef()
    {
        return InterlockedIncrement(&refCount);
    }

    IFACEMETHODIMP_(ULONG) LegacyAudioController::Release()
    {
        if (InterlockedDecrement(&refCount) == 0ul)
        {
            delete this;
            return 0;
        }
        else
        {
            return refCount;
        }
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


    bool LegacyAudioController::CreateSessionManager()
    {
        // TODO: Use exceptions or more meaningful error messages

        IMMDevicePtr pDevice;
        IMMDeviceEnumeratorPtr pEnumerator;

        // Create the device enumerator.
        if (FAILED(CoCreateInstance(
            __uuidof(MMDeviceEnumerator),
            NULL, CLSCTX_ALL,
            __uuidof(IMMDeviceEnumerator),
            (void**)&pEnumerator)))
        {
            return false;
        }
        
        // Get the default audio device.
        if (FAILED(pEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &pDevice)))
        {
            return false;
        }

        // Get the session manager.
        if (FAILED(pDevice->Activate(
            __uuidof(IAudioSessionManager2), CLSCTX_ALL,
            NULL, (void**)&audioSessionManager)))
        {
            return false;
        }

        return true;
    }

    HRESULT __stdcall LegacyAudioController::OnSessionCreated(IAudioSessionControl* NewSession)
    {
        if (!ignoreNotification)
        {
            IAudioSessionControl2Ptr control2;
            if (SUCCEEDED(NewSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&control2)))
            {
                AudioSession audioSession = AudioSession(std::move(control2), audioSessionID);
#ifdef _DEBUG
                if (canUseGuid)
                {
                    OutputDebugHString(winrt::to_hstring(managerID) + L" > New session created : " + audioSession.Name());
                }
                else
                {
                    OutputDebugHString(L"New session created : " + audioSession.Name());
                }
#else
                OutputDebugHString(L"New session created : " + audioSession.Name());
#endif // _DEBUG
            }
        }
        ignoreNotification = !ignoreNotification;

        return S_OK;
    }
}