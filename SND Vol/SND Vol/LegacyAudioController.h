#pragma once

#include "AudioSession.h"
#include <stack>

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
        ~LegacyAudioController();

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
        /// <summary>
        /// Newly created sessions. Call in loop to get all the new sessions.
        /// </summary>
        /// <param name="handler"></param>
        /// <returns></returns>
        AudioSession* NewSession();

        winrt::event_token SessionAdded(const winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>& handler);
        void SessionAdded(const ::winrt::event_token& token);

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);

    private:
        GUID audioSessionID;
        IAudioSessionManager2Ptr audioSessionManager;
        ::winrt::impl::atomic_ref_count refCount{ 1 };
        bool ignoreNotification = false;
        std::stack<AudioSessionContainer> newSessions{};
        bool isRegistered = false;

        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> e_sessionAdded{};

        bool CreateSessionManager();

        STDMETHOD(OnSessionCreated)(IAudioSessionControl* NewSession);
    };
}

