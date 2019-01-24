#include "Camera.h"

#pragma comment(lib,"Strmiids.lib") 

CCameraDS::CCameraDS()
{
	m_bConnected = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bLock = false;
	m_bChanged = false;
	m_nBufferSize = 0;

	m_pNullFilter = NULL;
	m_pMediaEvent = NULL;
	m_pSampleGrabberFilter = NULL;
	m_pGraph = NULL;

	CoInitialize(NULL);
}

CCameraDS::~CCameraDS()
{
	CloseCamera();
	CoUninitialize();
}

void CCameraDS::CloseCamera()
{
	if (m_bConnected)
		m_pMediaControl->Stop();

	m_pGraph = NULL;
	m_pDeviceFilter = NULL;
	m_pMediaControl = NULL;
	m_pSampleGrabberFilter = NULL;
	m_pSampleGrabber = NULL;
	m_pGrabberInput = NULL;
	m_pGrabberOutput = NULL;
	m_pCameraOutput = NULL;
	m_pMediaEvent = NULL;
	m_pNullFilter = NULL;
	m_pNullInputPin = NULL;

	m_bConnected = false;
	m_bIsCurrentDeviceBusy = false;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bLock = false;
	m_bChanged = false;
	m_nBufferSize = 0;
}

bool CCameraDS::OpenCamera(int nCamID, bool bDisplayProperties, int &nWidth, int &nHeight)
{
	char cam_name_temp[1024];
	int buffer_size = 1024;
	CameraName(nCamID, cam_name_temp, buffer_size);
	if (IsDeviceBusy(cam_name_temp))
	{
		m_bIsCurrentDeviceBusy = true;
		return false;
	}

	HRESULT hr = S_OK;

	CoInitialize(NULL);
	// Create the Filter Graph Manager.
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **)&m_pGraph);

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID *)&m_pSampleGrabberFilter);

	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pMediaEvent);

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (LPVOID*)&m_pNullFilter);


	hr = m_pGraph->AddFilter(m_pNullFilter, L"NullRenderer");

	hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pSampleGrabber);

	AM_MEDIA_TYPE   mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	//mt.subtype = MEDIASUBTYPE_RGB24;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.formattype = FORMAT_VideoInfo;
	hr = m_pSampleGrabber->SetMediaType(&mt);
	MYFREEMEDIATYPE(mt);

	m_pGraph->AddFilter(m_pSampleGrabberFilter, L"Grabber");

	// Bind Device Filter.  We know the device because the id was passed in
	BindFilter(nCamID, &m_pDeviceFilter);
	m_pGraph->AddFilter(m_pDeviceFilter, NULL);

	if (m_pDeviceFilter == NULL)
	{
		printf("\n+++++++++ 缺少摄像头，推荐使用 Logitech C920，安装官方驱动。 ++++++++\n");
		Sleep(2000);
		exit(1);
	}
	CComPtr<IEnumPins> pEnum;
	m_pDeviceFilter->EnumPins(&pEnum);

	hr = pEnum->Reset();
	hr = pEnum->Next(1, &m_pCameraOutput, NULL);

	pEnum = NULL;
	m_pSampleGrabberFilter->EnumPins(&pEnum);
	pEnum->Reset();
	hr = pEnum->Next(1, &m_pGrabberInput, NULL);

	pEnum = NULL;
	m_pSampleGrabberFilter->EnumPins(&pEnum);
	pEnum->Reset();
	pEnum->Skip(1);
	hr = pEnum->Next(1, &m_pGrabberOutput, NULL);

	pEnum = NULL;
	m_pNullFilter->EnumPins(&pEnum);
	pEnum->Reset();
	hr = pEnum->Next(1, &m_pNullInputPin, NULL);

	//SetCrossBar();

	if (bDisplayProperties)
	{
		CComPtr<ISpecifyPropertyPages> pPages;

		HRESULT hr = m_pCameraOutput->QueryInterface(IID_ISpecifyPropertyPages, (void**)&pPages);
		if (SUCCEEDED(hr))
		{
			PIN_INFO PinInfo;
			m_pCameraOutput->QueryPinInfo(&PinInfo);

			CAUUID caGUID;
			pPages->GetPages(&caGUID);

			OleCreatePropertyFrame(NULL, 0, 0,
				L"Property Sheet", 1,
				(IUnknown **)&(m_pCameraOutput.p),
				caGUID.cElems,
				caGUID.pElems,
				0, 0, NULL);
			CoTaskMemFree(caGUID.pElems);
			PinInfo.pFilter->Release();
		}
		pPages = NULL;
	}
	else
	{
		IAMStreamConfig*   iconfig;
		iconfig = NULL;
		hr = m_pCameraOutput->QueryInterface(IID_IAMStreamConfig, (void**)&iconfig);

		AM_MEDIA_TYPE* pmt;
		if (iconfig->GetFormat(&pmt) != S_OK)
		{
			//printf("GetFormat Failed ! \n");
			return   false;
		}

		VIDEOINFOHEADER*   phead;
		if (pmt->formattype == FORMAT_VideoInfo)
		{
			nWidth = 1280; nHeight = 720;
			phead = (VIDEOINFOHEADER*)pmt->pbFormat;
			phead->bmiHeader.biWidth = nWidth;
			phead->bmiHeader.biHeight = nHeight;
			if ((hr = iconfig->SetFormat(pmt)) != S_OK)
			{
				nWidth = 640; nHeight = 480;
				phead->bmiHeader.biWidth = 640;
				phead->bmiHeader.biHeight = 480;
				if ((hr = iconfig->SetFormat(pmt)) != S_OK)
				{
					nWidth = 320; nHeight = 240;
					phead->bmiHeader.biWidth = 320;
					phead->bmiHeader.biHeight = 240;
					if ((hr = iconfig->SetFormat(pmt)) != S_OK)
					{
						return   false;
					}
				}
				//return   false;
			}

		}

		iconfig->Release();
		iconfig = NULL;
		MYFREEMEDIATYPE(*pmt);
	}

	hr = m_pGraph->Connect(m_pCameraOutput, m_pGrabberInput);
	hr = m_pGraph->Connect(m_pGrabberOutput, m_pNullInputPin);

	if (FAILED(hr))
	{
		switch (hr)
		{
		case VFW_S_NOPREVIEWPIN:
			break;
		case E_FAIL:
			break;
		case E_INVALIDARG:
			break;
		case E_POINTER:
			break;
		}
	}

	m_pSampleGrabber->SetBufferSamples(TRUE);
	m_pSampleGrabber->SetOneShot(TRUE);

	hr = m_pSampleGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr))
		return false;

	VIDEOINFOHEADER *videoHeader;
	videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
	m_nWidth = videoHeader->bmiHeader.biWidth;
	m_nHeight = videoHeader->bmiHeader.biHeight;
	m_bConnected = true;

	pEnum = NULL;
	return true;
}

bool CCameraDS::IsConnected()
{
	return m_bConnected;
}

int CCameraDS::IsDeviceBusy(char *videoName)
{
	HRESULT hr;
	HRESULT hhr;
	int ret = 0;
	int videoBusy = 1;

	CoInitialize(NULL);

	ICreateDevEnum* pSysDevEnum = NULL;

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pSysDevEnum);

	IEnumMoniker* pEnumCat;

	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	if (hr == S_OK)
	{
		IMoniker* pMoniker = NULL;
		IMoniker* pm1 = NULL;
		ULONG cFetched;

		while (pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag* pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);

			if (SUCCEEDED(hr))
			{

				VARIANT varName;
				varName.vt = VT_BSTR;

				VariantInit(&varName);

				hr = pPropBag->Read(L"FriendlyName", &varName, 0);

				USES_CONVERSION;
				LPTSTR lpstrMsg = W2T(varName.bstrVal);

				if (SUCCEEDED(hr))
				{
					if (!strcmp(videoName, (const char *)lpstrMsg))//存在设备  
					{
						LPBC *pbc = NULL;
						IBaseFilter *P_VCamTrans = NULL;
						IBaseFilter *pCap = NULL;

						CreateBindCtx(0, pbc);

						hr = pMoniker->BindToObject((IBindCtx *)pbc, 0, IID_IBaseFilter, (void **)&pCap);

						ICaptureGraphBuilder2 *m_pCapGB;
						IGraphBuilder *m_pGB;
						IMediaControl *m_pMC;
						IVideoWindow   *m_pVW;

						hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB);

						if (FAILED(hr)) return hr;

						m_pGB->AddFilter(pCap, NULL);

						hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&m_pCapGB);

						if (FAILED(hr)) return hr;

						m_pCapGB->SetFiltergraph(m_pGB);

						IAMCrossbar *pXBar1 = NULL;

						hr = m_pCapGB->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, pCap, IID_IAMCrossbar, (void **)&pXBar1);

						if (SUCCEEDED(hr))
						{
							long OutputPinCount;
							long InputPinCount;
							long PinIndexRelated;
							long PhysicalType;
							long inPort = 0;
							long outPort = 0;

							pXBar1->get_PinCounts(&OutputPinCount, &InputPinCount);

							//对于存在输入输出引脚的摄像头。此处采用轮询所有的引脚   
							for (int i = 0; i < InputPinCount; i++)
							{
								pXBar1->get_CrossbarPinInfo(TRUE, i, &PinIndexRelated, &PhysicalType);

								if (PhysConn_Video_Composite == PhysicalType)
								{
									inPort = i;
									break;
								}

							}

							for (int i = 0; i < OutputPinCount; i++)
							{
								pXBar1->get_CrossbarPinInfo(FALSE, i, &PinIndexRelated, &PhysicalType);

								if (PhysConn_Video_VideoDecoder == PhysicalType)
								{
									outPort = i;
									break;
								}
							}

							for (int i = 0; i < InputPinCount; i++)
							{
								for (int j = 0; j < OutputPinCount; j++)
								{
									if (S_OK == pXBar1->CanRoute(j, i))
									{
										pXBar1->Route(j, i);

										m_pGB->AddFilter(pCap, L"Capture Filter");
										m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
										hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID*)&m_pVW);
										hr = m_pCapGB->RenderStream(NULL, NULL, pCap, NULL, P_VCamTrans);

										hr = m_pVW->put_Owner((OAHWND)NULL);
										hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
										hr = m_pVW->put_Visible(OAFALSE);
										hr = m_pVW->put_AutoShow(OAFALSE);

										hhr = m_pMC->StopWhenReady();

										if (SUCCEEDED(hhr))
										{
											videoBusy = 0;
										}

									}
								}
							}

							if (videoBusy == 1)
							{
								ret = -1; //视频设备占用  
							}
						}
						else
						{
							m_pGB->AddFilter(pCap, L"Capture Filter");
							m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
							hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID*)&m_pVW);
							hr = m_pCapGB->RenderStream(NULL, NULL, pCap, NULL, P_VCamTrans);

							hr = m_pVW->put_Owner((OAHWND)NULL);
							hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
							hr = m_pVW->put_Visible(OAFALSE);
							hr = m_pVW->put_AutoShow(OAFALSE);

							hhr = m_pMC->StopWhenReady();

							if (FAILED(hhr))
							{
								ret = -1;  //视频设备占用  
							}

						}

					}

				}

				pPropBag->Release();

			}
			pMoniker->Release();
		}

	}



	pSysDevEnum->Release();

	CoUninitialize();

	return ret;
}

bool CCameraDS::BindFilter(int nCamID, IBaseFilter **pFilter)
{
	if (nCamID < 0)
		return false;

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		return false;
	}

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	if (hr != NOERROR)
	{
		return false;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= nCamID)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				if (index == nCamID)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}

	pCreateDevEnum = NULL;
	return true;
}

//将输入crossbar变成PhysConn_Video_Composite
void CCameraDS::SetCrossBar()
{
	int i;
	IAMCrossbar *pXBar1 = NULL;
	ICaptureGraphBuilder2 *pBuilder = NULL;


	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL,
		CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2,
		(void **)&pBuilder);

	if (SUCCEEDED(hr))
	{
		hr = pBuilder->SetFiltergraph(m_pGraph);
	}


	hr = pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY, NULL,
		m_pDeviceFilter, IID_IAMCrossbar, (void**)&pXBar1);

	if (SUCCEEDED(hr))
	{
		long OutputPinCount;
		long InputPinCount;
		long PinIndexRelated;
		long PhysicalType;
		long inPort = 0;
		long outPort = 0;

		pXBar1->get_PinCounts(&OutputPinCount, &InputPinCount);
		for (i = 0; i < InputPinCount; i++)
		{
			pXBar1->get_CrossbarPinInfo(TRUE, i, &PinIndexRelated, &PhysicalType);
			if (PhysConn_Video_Composite == PhysicalType)
			{
				inPort = i;
				break;
			}
		}
		for (i = 0; i < OutputPinCount; i++)
		{
			pXBar1->get_CrossbarPinInfo(FALSE, i, &PinIndexRelated, &PhysicalType);
			if (PhysConn_Video_VideoDecoder == PhysicalType)
			{
				outPort = i;
				break;
			}
		}

		if (S_OK == pXBar1->CanRoute(outPort, inPort))
		{
			pXBar1->Route(outPort, inPort);
		}
		pXBar1->Release();
	}
	pBuilder->Release();
}

std::tr1::shared_ptr<unsigned char> CCameraDS::QueryFrame()
{

	long evCode;
	long size = 0;

	m_pMediaControl->Run();
	m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);

	m_pSampleGrabber->GetCurrentBuffer(&size, NULL);

	//if the buffer size changed
	if (size != m_nBufferSize)
	{
		m_nBufferSize = size;

		m_frame = std::tr1::shared_ptr<unsigned char>(new unsigned char[size]);
	}

	m_pSampleGrabber->GetCurrentBuffer(&m_nBufferSize, (long*)m_frame.get());
	//printf("%08x", m_frame.get());
	return m_frame;
}

int CCameraDS::CameraCount()
{

	int count = 0;
	CoInitialize(NULL);

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	if (hr != NOERROR)
	{
		return count;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		count++;
	}

	pCreateDevEnum = NULL;
	pEm = NULL;
	return count;
}

int CCameraDS::CameraName(int nCamID, char* sName, int nBufferSize)
{
	int count = 0;
	CoInitialize(NULL);

	// enumerate all video capture devices
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);

	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
		&pEm, 0);
	if (hr != NOERROR) return 0;


	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
	{
		if (count == nCamID)
		{
			IPropertyBag *pBag = 0;
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if (SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...
				if (hr == NOERROR)
				{
					//获取设备名称			
					WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sName, nBufferSize, "", NULL);

					SysFreeString(var.bstrVal);
				}
				pBag->Release();
			}
			pM->Release();

			break;
		}
		count++;
	}

	pCreateDevEnum = NULL;
	pEm = NULL;

	return 1;
}

