#pragma once

#include <stack>
#include "IComEventImplementation.h"
#include "AudioSession.h"
#include "MainAudioEndpoint.h"

namespace Audio
{
    using AudioSessionContainer = std::unique_ptr<AudioSession>;

    class LegacyAudioController : private IAudioSessionNotification, public IComEventImplementation
    {
    public:
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="guid">GUID to give audio sessions to ignore audio events</param>
        LegacyAudioController(GUID const& guid);

        // IUnknown
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);

        bool Register();
        bool Unregister();

        /// <summary>
        /// Enumerates current audio sessions.
        /// </summary>
        /// <returns>Audio sessions currently active</returns>
        std::vector<AudioSessionContainer>* GetSessions();
        /// <summary>
        /// Newly created sessions. Call in loop to get all the new sessions.
        /// </summary>
        /// <param name="handler"></param>
        /// <returns></returns>
        AudioSession* NewSession();
        MainAudioEndpoint* GetMainAudioEndpoint();
        IAudioMeterInformation* GetMainAudioEnpointMeterInfo();

        winrt::event_token SessionAdded(const winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>& handler);
        void SessionAdded(const ::winrt::event_token& token);

    private:
        GUID audioSessionID;
        IAudioSessionManager2Ptr audioSessionManager;
        ::winrt::impl::atomic_ref_count refCount{ 1 };
        bool ignoreNotification = false;
        std::stack<AudioSessionContainer> newSessions{};
        bool isRegistered = false;

        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> e_sessionAdded{};

        void CreateSessionManager();

        STDMETHOD(OnSessionCreated)(IAudioSessionControl* NewSession);
    };
}

