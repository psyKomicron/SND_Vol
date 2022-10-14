#pragma once

#include "IComEventImplementation.h"

namespace Audio
{
	class MainAudioEndpoint : public IComEventImplementation, private IAudioEndpointVolumeCallback
	{
	public:
		/**
		 * @brief Default constructor.
		 * @param devicePtr IMMDevice pointer.
		 * @param eventContextId GUID used by this app to generate events.
		*/
		MainAudioEndpoint(IMMDevice* devicePtr, GUID eventContextId);
		~MainAudioEndpoint();

		/**
		 * @brief Gets the friendly name of the audio endpoint.
		 * @return Friendly name of the audio endpoint
		*/
		winrt::hstring Name() const;
		/**
		 * @brief Set the volume of the audio endpoint.
		 * @param value Volume expressed in percentage (0 to 1)
		*/
		void Volume(const float& value);
		/**
		 * @brief Gets the volume of the audio endpoint.
		 * @return Volume expressed in percentage (0 to 1)
		*/
		float Volume() const;
		/**
		 * @brief Gets the number of channels for the endpoint.
		 * @return Number of channels
		*/
		uint32_t Channels() const;

		inline winrt::event_token VolumeChanged(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float> const& handler)
		{
			return e_volumeChanged.add(handler);
		};
		void VolumeChanged(winrt::event_token const& eventToken)
		{
			e_volumeChanged.remove(eventToken);
		};

		IFACEMETHODIMP_(ULONG) AddRef();
		IFACEMETHODIMP_(ULONG) Release();
		IFACEMETHODIMP QueryInterface(REFIID riid, VOID** ppvInterface);
		bool Register();
		bool Unregister();
		void Mute();
		void Unmute();
		IAudioMeterInformation* GetEndpointMeterInfo();

	private:
		IAudioEndpointVolumePtr audioEndpointVolume{ nullptr };
		IMMDevicePtr device{ nullptr };
		::winrt::impl::atomic_ref_count refCount{ 1 };
		GUID eventContextId;
		LPWSTR deviceId = nullptr;

		winrt::event<winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Foundation::IInspectable, float>> e_volumeChanged{};

#pragma region IAudioEndpointVolumeCallback
		STDMETHODIMP OnNotify(__in PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
#pragma endregion
	};
}

