#include "pch.h"
#include "AudioSession.h"

#include <rpc.h>
#include <appmodel.h>
#include <ShObjIdl.h>
#include <objbase.h>
#include <strsafe.h>

using namespace winrt;
using namespace std;


namespace Audio
{
    AudioSession::AudioSession(IAudioSessionControl2* audioSessionControl, GUID eventContextId) :
        eventContextId{ eventContextId },
        sessionName{ L"Unknown" },
        audioSessionControl{ audioSessionControl }
    {
        check_bool(UuidCreate(&id) == 0);
        check_hresult(audioSessionControl->GetGroupingParam(&groupingParam));


        if (audioSessionControl->IsSystemSoundsSession() == S_OK)
        {
            winrt::Windows::ApplicationModel::Resources::ResourceLoader loader{};
            sessionName = loader.GetString(L"SystemAudioSessionName");
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
                        displayName = GetProcessName(pid);
                        if (!displayName.empty())
                        {
                            sessionName = std::move(displayName);
                        }
                    }
                }
                else
                {
                    sessionName = std::move(displayName);
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

        check_hresult(audioSessionControl->QueryInterface(_uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume));
        if (FAILED(simpleAudioVolume->GetMute((BOOL*)&muted)))
        {
            OutputDebugHString(L"Audio session '" + sessionName + L"' > Failed to get session state. Default (unmuted) assumed.");
        }

        if (FAILED(audioSessionControl->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&audioMeter)))
        {
            OutputDebugHString(L"Audio session '" + sessionName + L"' > Failed to get audio meter info. Peak values will be blank.");
        }
    }


    #pragma region Properties
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

    void AudioSession::Muted(const bool& isMuted)
    {
        simpleAudioVolume->SetMute(isMuted, &eventContextId);
    }

    hstring AudioSession::Name()
    {
        return sessionName;
    }

    void AudioSession::Volume(float const& desiredVolume)
    {
        check_hresult(simpleAudioVolume->SetMasterVolume(desiredVolume, &eventContextId));
    }

    float AudioSession::Volume() const
    {
        float volume = 0.0f;
        check_hresult(simpleAudioVolume->GetMasterVolume(&volume));
        return volume;
    }

    AudioSessionState AudioSession::State() const
    {
        AudioSessionState state{};
        check_hresult(audioSessionControl->GetState(&state));
        return state;
    }
    #pragma endregion


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


    bool AudioSession::SetMute(bool const& state)
    {
        return SUCCEEDED(simpleAudioVolume->SetMute(state, nullptr));
    }

    void AudioSession::SetVolume(const float& volume)
    {
        check_hresult(simpleAudioVolume->SetMasterVolume(volume, nullptr));
    }

    float AudioSession::GetPeak() const
    {
        float peak = 0.f;
        audioMeter->GetPeakValue(&peak);
        return peak;
    }

    pair<float, float> Audio::AudioSession::GetChannelsPeak() const
    {
        // TODO: Channel count
        pair<float, float> peaks{};

        UINT meteringChannelCount = 0;
        if (SUCCEEDED(audioMeter->GetMeteringChannelCount(&meteringChannelCount)) && meteringChannelCount == 2) 
        {
            float channelsPeak[2]{ 0 };
            check_hresult(audioMeter->GetChannelsPeakValues(2, channelsPeak));
            peaks.first = channelsPeak[0];
            peaks.second = channelsPeak[1];
        }
        return peaks;
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

    #pragma region IUnknown
    IFACEMETHODIMP_(ULONG)AudioSession::AddRef()
    {
        return ++refCount;
    }

    IFACEMETHODIMP_(ULONG) AudioSession::Release()
    {
        const uint32_t remaining = --refCount;

        if (remaining == 0)
        {
            delete this;
        }

        return remaining;
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
        hstring processName{};

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pId);

        if (processHandle != NULL && processHandle != INVALID_HANDLE_VALUE)
        {
            // Try to get display name for UWP-like apps
            uint32_t applicationUserModelIdLength = 0;
            PWSTR applicationUserModelId = nullptr;

            if (GetApplicationUserModelId(processHandle, &applicationUserModelIdLength, applicationUserModelId) == ERROR_INSUFFICIENT_BUFFER)
            {
                applicationUserModelId = new WCHAR[applicationUserModelIdLength](0);
                if (SUCCEEDED(GetApplicationUserModelId(processHandle, &applicationUserModelIdLength, applicationUserModelId)))
                {
                    hstring shellPath = L"shell:appsfolder\\" + to_hstring(applicationUserModelId);
                    delete[] applicationUserModelId;

                    IShellItem* shellItem = nullptr;
                    if (SUCCEEDED(SHCreateItemFromParsingName(shellPath.c_str(), nullptr, IID_PPV_ARGS(&shellItem))))
                    {
                        LPWSTR wStr = nullptr;
                        if (SUCCEEDED(shellItem->GetDisplayName(SIGDN::SIGDN_NORMALDISPLAY, &wStr)))
                        {
                            processName = to_hstring(wStr);

                            CoTaskMemFree(wStr);

                            // If the shell item name is not empty, clean up resources and return name.
                            if (!processName.empty())
                            {
                                shellItem->Release();
                                CloseHandle(processHandle);
                                return processName;
                            }
                        }
                        else // Failed to get shell item display name, release the object and continue to default naming using PID.
                        {
                            shellItem->Release();
                        }
                    }
                }
            }
            
            // Default naming using PID.

            wchar_t executableName[MAX_PATH]{};
            DWORD executableNameLength = MAX_PATH;
            if (QueryFullProcessImageName(processHandle, 0, executableName, &executableNameLength))
            {
                CloseHandle(processHandle); // Close handle since we don't need it anymore.

                uint64_t fileVersionInfoSize = GetFileVersionInfoSize(executableName, NULL);
                if (fileVersionInfoSize > 0)
                {
                    void* lpData = malloc(fileVersionInfoSize);
                    if (lpData)
                    {
                        ZeroMemory(lpData, fileVersionInfoSize);

                        // TODO: Numeric narrowing
                        if (GetFileVersionInfo(executableName, 0, fileVersionInfoSize, lpData))
                        {
                            uint32_t cbTranslate = 0;
                            struct LANGANDCODEPAGE
                            {
                                WORD wLanguage;
                                WORD wCodePage;
                            } *lpTranslate = nullptr;

                            wchar_t strSubBlock[MAX_PATH]{ 0 };
                            if (VerQueryValue(lpData, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate))
                            {
                                if ((cbTranslate / sizeof(LANGANDCODEPAGE)) == 1)
                                {
                                    HRESULT hr = StringCchPrintf(
                                        strSubBlock,
                                        50,
                                        L"\\StringFileInfo\\%04x%04x\\FileDescription",
                                        lpTranslate[0].wLanguage,
                                        lpTranslate[0].wCodePage
                                    );

                                    if (SUCCEEDED(hr))
                                    {
                                        wchar_t* lpBuffer = nullptr;
                                        uint32_t lpBufferSize = 0;
                                        if (VerQueryValue(lpData, strSubBlock, (void**)&lpBuffer, &lpBufferSize) && lpBufferSize > 0)
                                        {
                                            processName = to_hstring(lpBuffer);
                                        }
                                    }
                                }

                                /*
                                for (size_t i = 0; i < (cbTranslate / sizeof(LANGANDCODEPAGE)); i++)
                                {
                                    HRESULT hr = StringCchPrintf(
                                        strSubBlock,
                                        50,
                                        L"\\StringFileInfo\\%04x%04x\\FileDescription",
                                        lpTranslate[i].wLanguage,
                                        lpTranslate[i].wCodePage
                                    );

                                    if (SUCCEEDED(hr))
                                    {
                                        wchar_t* lpBuffer = nullptr;
                                        uint32_t lpBufferSize = 0;
                                        if (VerQueryValue(lpData, strSubBlock, (void**)&lpBuffer, &lpBufferSize) && lpBufferSize > 0)
                                        {
                                            OutputDebugString(lpBuffer);
                                        }
                                    }
                                }
                                */
                            }
                        }

                        free(lpData);
                    }
                }
            }
            else
            {
                CloseHandle(processHandle);
            }
        }

        return processName;
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
        if (*EventContext != eventContextId)
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

    STDMETHODIMP AudioSession::OnSessionDisconnected(AudioSessionDisconnectReason /*DisconnectReason*/)
    {
        e_stateChanged(guid(id), static_cast<uint32_t>(AudioSessionState::AudioSessionStateExpired));

        return S_OK;
    }
}