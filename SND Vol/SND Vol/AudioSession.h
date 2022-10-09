#pragma once

#include "IComEventImplementation.h"

namespace Audio
{
    enum AudioState : uint32_t
    {
        Active,
        Inactive,
        Muted,
        Unmuted,
        Expired
    };

    class AudioSession : private IAudioSessionEvents, public IComEventImplementation
    {
    public:
        AudioSession(IAudioSessionControl2Ptr audioSessionControl, GUID globalAudioSessionID);

        GUID GroupingParam();
        bool IsSystemSoundSession();
        GUID Id();
        bool Muted();
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

        bool Register();
        bool Unregister();

        /// <summary>
        /// Sets the session audio level to mute if state == true.
        /// </summary>
        /// <param name="state"></param>
        /// <returns>True if the audio session has been muted.</returns>
        bool SetMute(bool const& state);

        winrt::event_token StateChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, uint32_t> const& handler);
        void StateChanged(winrt::event_token const& token);
        /// <summary>
        /// Volume changed event subscriber. IInspectable param of handler will contain a winrt::guid struct equal to the id
        /// of this AudioSession.
        /// </summary>
        /// <param name="handler">Event handler</param>
        /// <returns>event token</returns>
        winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float> const& handler);
        /// <summary>
        /// Volume changed event remover.
        /// </summary>
        /// <param name="eventToken">handler token to remove</param>
        void VolumeChanged(winrt::event_token const& eventToken);

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
        bool muted;
        DWORD processPID = 0;
        LONG refCount = 1;
        winrt::hstring sessionName{};

        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float>> e_volumeChanged{};
        winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, uint32_t>> e_stateChanged{};

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
        STDMETHOD(OnStateChanged)(::AudioSessionState NewState);
        STDMETHOD(OnSessionDisconnected)(AudioSessionDisconnectReason DisconnectReason);
    };
}

