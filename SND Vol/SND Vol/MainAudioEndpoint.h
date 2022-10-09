#pragma once

#include "IComEventImplementation.h"

namespace Audio
{
	class MainAudioEndpoint : IComEventImplementation, IAudioEndpointVolumeCallback
	{
	public:
		MainAudioEndpoint(IAudioEndpointVolume* audioEndpointVolume, GUID eventContextId);

		// IUnkown
		IFACEMETHODIMP_(ULONG) AddRef();
		IFACEMETHODIMP_(ULONG) Release();
		IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);

		// ComEventImplementation
		bool Register();
		bool Unregister();

		void Volume(const float& value);
		float Volume() const;
		void Mute();
		void Unmute();

		inline winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float> const& handler)
		{
			return e_volumeChanged.add(handler);
		};
		void VolumeChanged(winrt::event_token const& eventToken)
		{
			e_volumeChanged.remove(eventToken);
		};

	private:
		IAudioEndpointVolumePtr audioEndpointVolume = nullptr;
		::winrt::impl::atomic_ref_count refCount{ 1 };
		GUID eventContextId;

		winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float>> e_volumeChanged {};

		STDMETHOD(OnNotify)(__in PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
	};
}

