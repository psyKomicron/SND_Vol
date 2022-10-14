#include "pch.h"
#include "AudioSession.h"

#include <rpc.h>
#include <appmodel.h>
#include <ShObjIdl.h>
#include <objbase.h>

using namespace winrt;
using namespace std;


namespace Audio
{
    AudioSession::AudioSession(IAudioSessionControl2* _audioSessionControl, GUID globalAudioSessionID, uint32_t channel) :
        globalAudioSessionID(globalAudioSessionID),
        sessionName(L"Unknown"),
        channel(channel)
    {
        audioSessionControl = IAudioSessionControl2Ptr(_audioSessionControl);

        check_hresult(UuidCreate(&id));
        check_hresult(audioSessionControl->GetGroupingParam(&groupingParam));

        if (audioSessionControl->IsSystemSoundsSession() == S_OK)
        {
            sessionName = L"System";
            isSystemSoundSession = true;
        }
        else
        {
            LPWSTR wStr = nullptr;
            DWORD pid;
            if (SUCCEEDED(audioSessionControl->GetDisplayName(&wStr)))
            {
                hstring displayName = to_hstring(wStr);
                // Free memory allocated by audioSessionControl->GetDisplayName
                CoTaskMemFree(wStr);
                if (displayName.empty())
                {
                    if (SUCCEEDED(audioSessionControl->GetProcessId(&pid)))
                    {
                        sessionName = GetProcessName(pid);
                    }
                }
                else
                {
                    sessionName = displayName;
                }
            }
            else
            {
                if (SUCCEEDED(audioSessionControl->GetProcessId(&pid)))
                {
                    sessionName = GetProcessName(pid);
                }
            }
        }

        ISimpleAudioVolumePtr volume;
        if (FAILED(audioSessionControl->QueryInterface(_uuidof(ISimpleAudioVolume), (void**)&volume)) || FAILED(volume->GetMute((BOOL*)&muted)))
        {
            OutputDebugHString(L"Audio session '" + sessionName + L"' > Failed to get session state. Default (unmuted) assumed.");
        }

        if (FAILED(audioSessionControl->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&audioMeter)))
        {
            OutputDebugHString(L"Audio session '" + sessionName + L"' > Failed to get audio meter info. Peak values will be blank.");
        }
    }


    uint32_t AudioSession::Channel()
    {
        return channel;
    }

    GUID AudioSession::GroupingParam()
    {
        return groupingParam;
    }

    bool AudioSession::IsSystemSoundSession()
    {
        return isSystemSoundSession;
    }

    GUID AudioSession::Id()
    {
        return id;
    }

    bool AudioSession::Muted()
    {
        return muted;
    }

    hstring AudioSession::Name()
    {
        return sessionName;
    }

    bool AudioSession::Volume(float const& desiredVolume)
    {
        ISimpleAudioVolumePtr volume;
        if (SUCCEEDED(audioSessionControl->QueryInterface(_uuidof(ISimpleAudioVolume), (void**)&volume)))
        {
            HRESULT hResult = volume->SetMasterVolume(desiredVolume, &globalAudioSessionID);
            return SUCCEEDED(hResult);
        }
        return false;
    }

    float AudioSession::Volume() const
    {
        ISimpleAudioVolumePtr audioVolume;
        if (SUCCEEDED(audioSessionControl->QueryInterface(_uuidof(ISimpleAudioVolume), (void**)&audioVolume)))
        {
            float volume = 0.0f;
            audioVolume->GetMasterVolume(&volume);
            return volume;
        }
        return 0.0f;
    }

    AudioSessionState AudioSession::State() const
    {
        AudioSessionState state{};
        check_hresult(audioSessionControl->GetState(&state));
        return state;
    }


    bool AudioSession::SetMute(bool const& state)
    {
        ISimpleAudioVolumePtr volume;
        if (SUCCEEDED(audioSessionControl->QueryInterface(_uuidof(ISimpleAudioVolume), (void**)&volume)))
        {
            HRESULT hResult = volume->SetMute(state, &globalAudioSessionID);
            return SUCCEEDED(hResult);
        }
        return false;
    }

    float AudioSession::GetPeak() const
    {
        float peak = 0.f;
        audioMeter->GetPeakValue(&peak);
        return peak;
    }

    bool AudioSession::Register()
    {
        if (!isRegistered)
        {
            isRegistered =  SUCCEEDED(audioSessionControl->RegisterAudioSessionNotification(this));
        }
        return isRegistered;
    }

    bool AudioSession::Unregister()
    {
        if (isRegistered)
        {
            return SUCCEEDED(audioSessionControl->UnregisterAudioSessionNotification(this));
        }
        return true;
    }

    #pragma region Events
    winrt::event_token AudioSession::StateChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::guid, uint32_t> const& handler)
    {
        return e_stateChanged.add(handler);
    }

    void AudioSession::StateChanged(winrt::event_token const& token)
    {
        e_stateChanged.remove(token);
    }

    winrt::event_token AudioSession::VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::guid, float> const& handler)
    {
        return e_volumeChanged.add(handler);
    }

    void AudioSession::VolumeChanged(winrt::event_token const& eventToken)
    {
        e_volumeChanged.remove(eventToken);
    }
    #pragma endregion

    #pragma region IUnknown
    IFACEMETHODIMP_(ULONG)AudioSession::AddRef()
    {
        return InterlockedIncrement(&refCount);
    }

    IFACEMETHODIMP_(ULONG) AudioSession::Release()
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

    IFACEMETHODIMP AudioSession::QueryInterface(REFIID riid, VOID** ppvInterface)
    {
        if (riid == IID_IUnknown)
        {
            *ppvInterface = static_cast<IUnknown*>(static_cast<IAudioSessionEvents*>(this));
            AddRef();
        }
        else if (riid == __uuidof(IAudioSessionEvents))
        {
            *ppvInterface = static_cast<IAudioSessionEvents*>(this);
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


    hstring AudioSession::GetProcessName(DWORD const& pId)
    {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pId);
        if (processHandle != NULL && processHandle != INVALID_HANDLE_VALUE)
        {
            // Try to get display name for UWP-like apps
            uint32_t applicationUserModelIdLength = 0;
            PWSTR applicationUserModelId = nullptr;

            LONG retCode = GetApplicationUserModelId(processHandle, &applicationUserModelIdLength, applicationUserModelId);
            if (retCode != APPMODEL_ERROR_NO_APPLICATION && retCode == ERROR_INSUFFICIENT_BUFFER)
            {
                // TRACK: Not initializing memory to 0
                applicationUserModelId = new WCHAR[applicationUserModelIdLength](0);
                if (SUCCEEDED(GetApplicationUserModelId(processHandle, &applicationUserModelIdLength, applicationUserModelId)))
                {
                    hstring shellPath = L"shell:appsfolder\\" + to_hstring(applicationUserModelId);
                    delete applicationUserModelId;

                    IShellItem* shellItem = nullptr;
                    if (SUCCEEDED(SHCreateItemFromParsingName(shellPath.c_str(), nullptr, IID_PPV_ARGS(&shellItem))))
                    {
                        LPWSTR wStr = nullptr;
                        if (SUCCEEDED(shellItem->GetDisplayName(SIGDN::SIGDN_NORMALDISPLAY, &wStr)))
                        {
                            hstring displayName = to_hstring(wStr);
                            CoTaskMemFree(wStr);
                            return displayName;
                        }

                        shellItem->Release();
                    }
                }
            }

            // Default naming using PID.
            HMODULE moduleHandle = NULL;
            DWORD cbNeeded = 0;
            if (EnumProcessModulesEx(processHandle, &moduleHandle, sizeof(moduleHandle), &cbNeeded, LIST_MODULES_ALL))
            {
                WCHAR baseName[MAX_PATH](0);
                if (!GetModuleBaseName(processHandle, moduleHandle, baseName, MAX_PATH) || wcslen(baseName) == 0)
                {
                    return L"Unknown process";
                }
                return winrt::to_hstring(baseName);
            }

            CloseHandle(processHandle);
        }
        return winrt::hstring();
    }

    STDMETHODIMP AudioSession::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID)
    {
        OutputDebugHString(sessionName + L" > Display name changed : " + to_hstring(NewDisplayName));
        return S_OK;
    }

    STDMETHODIMP AudioSession::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID)
    {
        OutputDebugHString(sessionName + L" > Icon path changed : " + to_hstring(NewIconPath));
        return S_OK;
    }

    STDMETHODIMP AudioSession::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext)
    {
        if (*EventContext != globalAudioSessionID)
        {
            e_volumeChanged(guid(id), NewVolume);

            muted = NewMute;
            // 3 = AudioSessionState::Muted
            e_stateChanged(guid(id), muted ? 3u : 4u);
        }
        return S_OK;
    }

    STDMETHODIMP AudioSession::OnStateChanged(::AudioSessionState NewState)
    {
        e_stateChanged(guid(id), static_cast<uint32_t>(NewState));

        return S_OK;
    }

    STDMETHODIMP AudioSession::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason)
    {
#ifdef _DEBUG
        hstring reason{};
        switch (DisconnectReason)
        {
            case DisconnectReasonDeviceRemoval:
                reason = L"device removal";
                break;
            case DisconnectReasonServerShutdown:
                reason = L"server shutdown";
                break;
            case DisconnectReasonFormatChanged:
                reason = L"format changed";
                break;
            case DisconnectReasonSessionLogoff:
                reason = L"session log off";
                break;
            case DisconnectReasonSessionDisconnected:
                reason = L"session disconnected";
                break;
            case DisconnectReasonExclusiveModeOverride:
                reason = L"exclusive mode override";
                break;
            default:
                reason = L"unknown";
                break;
        }
        OutputDebugHString(sessionName + L" > Session disconnected : " + reason);
#endif // _DEBUG

        e_stateChanged(guid(id), static_cast<uint32_t>(AudioSessionState::AudioSessionStateExpired));

        return S_OK;
    }
}