#include "stdafx.h"
#include "PGVolumeController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPGVolumeController::CPGVolumeController(void) : m_bInit(false), m_deviceType(DeviceType::Speaker), m_pEndpointVolume(nullptr), m_guidMyContext(GUID_NULL)
{
}


CPGVolumeController::~CPGVolumeController(void)
{
	if (nullptr != m_pEndpointVolume)
	{
		m_pEndpointVolume->UnregisterControlChangeNotify((IAudioEndpointVolumeCallback*)&m_EndpointVolumeCallback);
	}
}


HRESULT CPGVolumeController::Initialize(DeviceType deviceType, CAudioEndpointVolumeCallback::CallbackFuncPtr funcPtr, void* funcParam)
{
	m_deviceType = deviceType;

	CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;
	CComPtr<IMMDevice> pDevice = nullptr;

	HRESULT hr = CoCreateGuid(&m_guidMyContext);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&pEnumerator);
	if (FAILED(hr))
	{
		return hr;
	}

	if (DeviceType::Microphone == m_deviceType)
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eMultimedia, &pDevice);
	}
	else
	{
		hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
	}
	if (FAILED(hr))
	{
		return hr;
	}

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&m_pEndpointVolume);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pEndpointVolume->RegisterControlChangeNotify((IAudioEndpointVolumeCallback*)&m_EndpointVolumeCallback);
	if (FAILED(hr))
	{
		return hr;
	}
	// Need more code...
	m_EndpointVolumeCallback.Initialize(m_guidMyContext, funcPtr, funcParam);
	m_bInit = TRUE;
	return 0;
}


HRESULT CPGVolumeController::SetMasterVolume(float fVol)
{
	if (!m_bInit || NULL == m_pEndpointVolume)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = m_pEndpointVolume->SetMasterVolumeLevelScalar(fVol, &m_guidMyContext);
	return hr;
}


HRESULT CPGVolumeController::GetMasterVolume(float *pfLevel)
{
	if (!m_bInit || NULL == m_pEndpointVolume)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = m_pEndpointVolume->GetMasterVolumeLevelScalar(pfLevel);

	return hr;
}


HRESULT CPGVolumeController::SetMute(BOOL bMute)
{
	if (!m_bInit || NULL == m_pEndpointVolume)
	{
		return E_INVALIDARG;
	}

	return m_pEndpointVolume->SetMute(bMute, &m_guidMyContext);
}


HRESULT CPGVolumeController::GetMute(BOOL* pbMute)
{
	if (!m_bInit || NULL == m_pEndpointVolume)
	{
		return E_INVALIDARG;
	}

	return m_pEndpointVolume->GetMute(pbMute);
}


GUID CPGVolumeController::GetContextGuid()
{
	return m_guidMyContext;
}
