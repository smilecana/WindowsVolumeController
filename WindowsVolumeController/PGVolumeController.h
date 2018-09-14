#ifndef _PGVOLUMECONTROLLER_
#define _PGVOLUMECONTROLLER_

#include <atomic>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Audioclient.h>
#include <Objbase.h>


// Audio Volume Change Callback.(When changing the system volume, OnNotify function is called.)
class CAudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback
{
public:
	typedef HRESULT(*CallbackFuncPtr)(PAUDIO_VOLUME_NOTIFICATION_DATA, void*);

private:
	LONG _cRef;
	GUID m_ContextGuid;
	CallbackFuncPtr _funcPtr;
	void* _funcParam;

public:
	CAudioEndpointVolumeCallback() : _cRef(1), m_ContextGuid(GUID_NULL), _funcPtr(nullptr), _funcParam(nullptr)
	{
	}
	~CAudioEndpointVolumeCallback(){}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return ::InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvObj)
	{
		if (!ppvObj)
			return E_INVALIDARG;

		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvObj = (IUnknown*)this;
		}
		else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
		{
			AddRef();
			*ppvObj = (IAudioEndpointVolumeCallback*)this;
		}
		else
		{
			*ppvObj = nullptr;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
	{
		if (nullptr == pNotify)
			return E_INVALIDARG;

		if (m_ContextGuid != pNotify->guidEventContext)
		{
			if (_funcPtr != nullptr)
			{
				_funcPtr(pNotify, _funcParam);
			}
		}
		return S_OK;
	}

	// Initialization
	HRESULT STDMETHODCALLTYPE Initialize(GUID contextGuid, CallbackFuncPtr funcPtr, void* funcParam)
	{
		m_ContextGuid = std::move(contextGuid);
		_funcPtr = funcPtr;
		_funcParam = funcParam;

		return S_OK;
	}
};


class CPGVolumeController
{
public:
	CPGVolumeController();
	virtual ~CPGVolumeController();

public:
	enum DeviceType { Speaker = 0, Microphone };
	enum { Max_Volume = 100 };

public:
	HRESULT Initialize(DeviceType deviceType, CAudioEndpointVolumeCallback::CallbackFuncPtr funcPtr, void* funcParam);

	HRESULT SetMasterVolume(float volume);
	HRESULT SetMasterVolume(UINT volume);
	HRESULT SetMute(BOOL bMute);

	HRESULT GetMasterVolume(float *volume);
	HRESULT GetMute(BOOL* pbMute);
	GUID GetContextGuid();
	static HRESULT CalculateIntegerVolume(float volume, UINT& calculated_volume);

private:
	bool m_bInit;
	DeviceType m_deviceType;
	CComPtr<IAudioEndpointVolume> m_pEndpointVolume;
	GUID m_guidMyContext;
	CAudioEndpointVolumeCallback m_EndpointVolumeCallback;
};

#endif
