#include "pch.h"
#include "MainAudioEndpoint.h"

#include <Functiondiscoverykeys_devpkey.h>
#include "LegacyAudioController.h"

using namespace winrt;


namespace Audio
{
	MainAudioEndpoint::MainAudioEndpoint(IMMDevice* pDevice, GUID eventContextId) :
		eventContextId{ eventContextId },
		device{ pDevice }
	{
		check_hresult(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&audioEndpointVolume));

		if (FAILED(device->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&audioMeterInfo)))
		{
			OutputDebugHString(L"Main audio endpoint failed to get audio meter information, peak values will be blank.");
		}
	}


	winrt::hstring MainAudioEndpoint::Name() const
	{
		IPropertyStore* pProps = nullptr;
		if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &pProps)))
		{
			PROPVARIANT deviceFriendlyName{};
			PropVariantInit(&deviceFriendlyName);
			if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &deviceFriendlyName)))
			{
				return hstring(deviceFriendlyName.pwszVal);
			}
		}
		return hstring();
	}

	void MainAudioEndpoint::Volume(const float& value)
	{
		if (value < 0.) return;

		winrt::check_hresult(audioEndpointVolume->SetMasterVolumeLevelScalar(value, &eventContextId));
	}

	float MainAudioEndpoint::Volume() const
	{
		float volumeLevel = 0.0;
		winrt::check_hresult(audioEndpointVolume->GetMasterVolumeLevelScalar(&volumeLevel));
		return volumeLevel;
	}

	uint32_t MainAudioEndpoint::Channels() const
	{
		uint32_t channelCount = 0;
		check_hresult(audioEndpointVolume->GetChannelCount(&channelCount));
		return channelCount;
	}

	bool MainAudioEndpoint::Muted() const
	{
		BOOL mute = false;
		check_hresult(audioEndpointVolume->GetMute(&mute));
		return mute & 1;
	}


	float MainAudioEndpoint::GetPeak() const
	{
		float peak = 0.f;
		audioMeterInfo->GetPeakValue(&peak);
		return peak;
	}

	bool MainAudioEndpoint::Register()
	{
		return SUCCEEDED(audioEndpointVolume->RegisterControlChangeNotify(this));
	}

	bool MainAudioEndpoint::Unregister()
	{
		return SUCCEEDED(audioEndpointVolume->UnregisterControlChangeNotify(this));
	}

	void MainAudioEndpoint::SetMute(const bool& mute)
	{
		winrt::check_hresult(audioEndpointVolume->SetMute(mute, &eventContextId));
	}

	void MainAudioEndpoint::SetVolume(const float& newVolume)
	{
		check_hresult(audioEndpointVolume->SetMasterVolumeLevelScalar(newVolume, nullptr));
	}

    #pragma region  IUnknown
	IFACEMETHODIMP_(ULONG) MainAudioEndpoint::AddRef()
	{
		return ++refCount;
	}

	IFACEMETHODIMP_(ULONG) MainAudioEndpoint::Release()
	{
		const uint32_t remaining = --refCount;

		if (remaining == 0)
		{
			delete(static_cast<void*>(this));
		}

		return remaining;
	}

	IFACEMETHODIMP MainAudioEndpoint::QueryInterface(REFIID riid, VOID** ppvInterface)
	{
		if (riid == IID_IUnknown)
		{
			*ppvInterface = static_cast<IUnknown*>(static_cast<IAudioEndpointVolumeCallback*>(this));
			AddRef();
		}
		else if (riid == __uuidof(IAudioEndpointVolumeCallback))
		{
			*ppvInterface = static_cast<IAudioEndpointVolumeCallback*>(this);
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


	STDMETHODIMP MainAudioEndpoint::OnNotify(__in PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
	{
		if (pNotify->guidEventContext != eventContextId)
		{
			// Handle notifications
			e_volumeChanged(winrt::Windows::Foundation::IInspectable(), pNotify->fMasterVolume);
			e_stateChanged(winrt::Windows::Foundation::IInspectable(), pNotify->bMuted & 1);
		}

		return S_OK;
	}
}