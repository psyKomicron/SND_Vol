#include "pch.h"
#include "MainAudioEndpoint.h"

#include <Functiondiscoverykeys_devpkey.h>
#include "LegacyAudioController.h"

using namespace winrt;


namespace Audio
{
	MainAudioEndpoint::MainAudioEndpoint(IMMDevice* pDevice, GUID eventContextId) :
		device{ pDevice },
		eventContextId(eventContextId)
	{
		/*if (FAILED(device->GetId(&deviceId)))
		{
			deviceId = nullptr;
			OutputDebugString(L"MainAudioEndpoint failed to get ID\n");
		}*/

		check_hresult(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&audioEndpointVolume));
	}

	MainAudioEndpoint::~MainAudioEndpoint()
	{
		/*if (deviceId)
		{
			CoTaskMemFree(deviceId);
		}*/
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
		if (audioEndpointVolume.GetInterfacePtr())
		{
			winrt::check_hresult(audioEndpointVolume->SetMasterVolumeLevelScalar(value, &eventContextId));
		}
	}

	float MainAudioEndpoint::Volume() const
	{
		float volumeLevel = 0.0;
		if (audioEndpointVolume.GetInterfacePtr())
		{
			winrt::check_hresult(audioEndpointVolume->GetMasterVolumeLevelScalar(&volumeLevel));
		}
		return volumeLevel;
	}

	uint32_t MainAudioEndpoint::Channels() const
	{
		uint32_t channelCount = 0;
		if (audioEndpointVolume.GetInterfacePtr())
		{
			check_hresult(audioEndpointVolume->GetChannelCount(&channelCount));
		}
		return channelCount;
	}


	void MainAudioEndpoint::Mute()
	{
		if (audioEndpointVolume.GetInterfacePtr())
		{
			winrt::check_hresult(audioEndpointVolume->SetMute(true, &eventContextId));
		}
	}

	void MainAudioEndpoint::Unmute()
	{
		if (audioEndpointVolume.GetInterfacePtr())
		{
			winrt::check_hresult(audioEndpointVolume->SetMute(false, &eventContextId));
		}
	}

	IAudioMeterInformation* MainAudioEndpoint::GetEndpointMeterInfo()
	{
		IAudioMeterInformation* audioMeterInfo = nullptr;
		if (FAILED(device->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, NULL, (void**)&audioMeterInfo)))
		{
			return nullptr;
		}
		else
		{
			return audioMeterInfo;
		}
	}

	bool MainAudioEndpoint::Register()
	{
		return audioEndpointVolume.GetInterfacePtr() && SUCCEEDED(audioEndpointVolume->RegisterControlChangeNotify(this));
	}

	bool MainAudioEndpoint::Unregister()
	{
		return audioEndpointVolume.GetInterfacePtr() && SUCCEEDED(audioEndpointVolume->UnregisterControlChangeNotify(this));
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
			OutputDebugHString(L"Master volume changed: " + winrt::to_hstring(pNotify->fMasterVolume));
			e_volumeChanged(winrt::Windows::Foundation::IInspectable(), pNotify->fMasterVolume);
		}

		return S_OK;
	}
}