#include "pch.h"
#include "MainAudioEndpoint.h"

namespace Audio
{
	MainAudioEndpoint::MainAudioEndpoint(IAudioEndpointVolume* audioEndpointVolume, GUID eventContextId) :
		audioEndpointVolume{ audioEndpointVolume },
		eventContextId(eventContextId)
	{
	}


	void MainAudioEndpoint::Volume(const float& value)
	{
		winrt::check_hresult(audioEndpointVolume->SetMasterVolumeLevelScalar(value, &eventContextId));
	}

	float MainAudioEndpoint::Volume() const
	{
		float volumeLevel = 0.0;
		winrt::check_hresult(audioEndpointVolume->GetMasterVolumeLevelScalar(&volumeLevel));
		return volumeLevel;
	}

	void MainAudioEndpoint::Mute()
	{
		winrt::check_hresult(audioEndpointVolume->SetMute(true, &eventContextId));
	}

	void MainAudioEndpoint::Unmute()
	{
		winrt::check_hresult(audioEndpointVolume->SetMute(false, &eventContextId));
	}

	bool MainAudioEndpoint::Register()
	{
		return SUCCEEDED(audioEndpointVolume->RegisterControlChangeNotify(this));
	}

	bool MainAudioEndpoint::Unregister()
	{
		return SUCCEEDED(audioEndpointVolume->UnregisterControlChangeNotify(this));
	}

    #pragma region IUnknown
	IFACEMETHODIMP_(ULONG) MainAudioEndpoint::AddRef()
	{
		return ++refCount;
	}

	IFACEMETHODIMP_(ULONG) MainAudioEndpoint::Release()
	{
		const uint32_t remaining = --refCount;

		if (remaining == 0)
		{
			delete this;
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


	STDMETHODIMP_(HRESULT __stdcall) MainAudioEndpoint::OnNotify(__in PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
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