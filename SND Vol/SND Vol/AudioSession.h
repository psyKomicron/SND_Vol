#pragma once

namespace Audio
{
    class AudioSession : public IAudioSessionEvents
    {
    public:
        AudioSession(IAudioSessionControl2Ptr audioSessionControl, GUID globalAudioSessionID);
        ~AudioSession() = default;

        GUID GroupingParam();
        bool IsSystemSoundSession();
        GUID Id();
        /// <summary>
        /// Gets the name of the session. Can be the display name or the name of the executable
        /// associated with the audio session's PID.
        /// </summary>
        /// <returns>The name of the session</returns>
        winrt::hstring Name();
        /// <summary>
        /// Gets the volume of the session.
        /// </summary>
        /// <param name="desiredVolume"></param>
        /// <returns>True if the fonction succeeded</returns>
        bool Volume(float const& desiredVolume);
        /// <summary>
        /// Sets the volume of the session.
        /// </summary>
        /// <returns>The volume of the session</returns>
        float Volume() const;

        winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float> const& handler)
        {
            return e_volumeChanged.add(handler);
        };
        void VolumeChanged(winrt::event_token const& eventToken)
        {
            e_volumeChanged.remove(eventToken);
        };
        /// <summary>
        /// Sets the session audio level to mute if state == true.
        /// </summary>
        /// <param name="state"></param>
        /// <returns>True if the audio session has been muted.</returns>
        bool SetMute(bool const& state);
        /// <summary>
        /// Registers the audio session to audio events.
        /// </summary>
        /// <returns>Return true if the registration succeeded</returns>
        bool Register();
        /// <summary>
        /// Unregisters the audio session from audio events.
        /// </summary>
        /// <returns></returns>
        bool Unregister();

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);

    private:
        IAudioSessionControl2Ptr audioSessionControl = nullptr;
        GUID globalAudioSessionID;
        GUID groupingParam;
        GUID id;
        bool isRegistered = false;
        bool isSystemSoundSession = false;
        winrt::hstring sessionName{};
        DWORD processPID = 0;
        LONG refCount = 1;

        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float>> e_volumeChanged{};

        /// <summary>
        /// Gets the process name from the audio session's PID.
        /// </summary>
        /// <param name="pid">PID to get the name from</param>
        /// <returns>The name of the process</returns>
        winrt::hstring GetProcessName(DWORD const& pid);

        // IAudioSessionEvents
        STDMETHOD(OnDisplayNameChanged)(LPCWSTR NewDisplayName, LPCGUID EventContext);
        STDMETHOD(OnIconPathChanged)(LPCWSTR NewIconPath, LPCGUID EventContext);
        STDMETHOD(OnSimpleVolumeChanged)(float NewVolume, BOOL NewMute, LPCGUID EventContext);
        STDMETHOD(OnChannelVolumeChanged)(DWORD /*ChannelCount*/, float /*NewChannelVolumeArray*/[], DWORD /*ChangedChannel*/, LPCGUID /*EventContext*/) { return S_OK; };
        STDMETHOD(OnGroupingParamChanged)(LPCGUID /*NewGroupingParam*/, LPCGUID /*EventContext*/) { return S_OK; };
        STDMETHOD(OnStateChanged)(AudioSessionState NewState);
        STDMETHOD(OnSessionDisconnected)(AudioSessionDisconnectReason DisconnectReason);
    };
}

