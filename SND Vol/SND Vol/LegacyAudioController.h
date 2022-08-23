#pragma once

#include "AudioSession.h"

namespace Audio
{
    using AudioSessionContainer = std::unique_ptr<AudioSession>;

    class LegacyAudioController : public IAudioSessionNotification
    {
    public:
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="guid">GUID to give audio sessions to ignore audio events</param>
        LegacyAudioController(GUID const& guid);

        /// <summary>
        /// Enumerates current audio sessions.
        /// </summary>
        /// <returns>Audio sessions currently active</returns>
        std::vector<AudioSessionContainer>* GetSessions();
        /// <summary>
        /// Registers the controller for changes in audio sessions.
        /// </summary>
        /// <returns>Returns true if the registration succeeded</returns>
        bool Register();
        /// <summary>
        /// Unregisters the audio controller from audio events.
        /// </summary>
        /// <returns>Returns true if the audio controller unregistered successfully</returns>
        bool Unregister();

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);

    private:
        GUID audioSessionID;
        IAudioSessionManager2Ptr audioSessionManager;
        bool ignoreNotification = false;
#ifdef _DEBUG
        bool canUseGuid = true;
        GUID managerID{};
#endif
        LONG refCount = 1;

        bool CreateSessionManager();

        STDMETHOD(OnSessionCreated)(IAudioSessionControl* NewSession);
    };
}

