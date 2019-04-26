VIDECRTP�ɼ�����ʹ��˵��:
1.��ʼ��
	::CoInitialize(NULL);
	VIDEC_Init();
	
2.�򿪲ɼ��豸
	VIDEC_CameraDev*	m_pCamera;
	m_pCamera=VIDEC_CameraDev::Create();
	if (m_pCamera!=NULL)
	{
		//���豸
		//nDevID�豸�ţ���0��ʼ
		//nWidth,ͼ����
		//nHeight,ͼ��߶�
		//nFrameRate,֡�ʣ�1��5��10��15��30
		//bDefaultFormat:�������ͼ���Ȼ�߶��豸��֧��ʱ�Ƿ����Ĭ�ϸ�ʽ
		//�ɹ�����0��ʧ�ܷ���-1
		//ע�⣺�󲿷ֵ�USB����ͷ��֧��704X576,ֻ֧��176X144,320X240,352X288,640X480;���bDefaultFormat==false����
		//�����ͼ���Ȼ�߶��豸��֧��ʱ,���豸��ʧ��
		if (0!=m_pCamera->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate,false))
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}
	}
	
3.������ʾ����
	VIDEC_Wnd*		m_pWnd;
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		//����ʾ����
		//hWndParent����ʾ���ڵĸ����ھ��������������Ч���
		//������������Ĭ��ֵ
		//�ɹ�����0��ʧ�ܷ���-1
		if (0!=m_pWnd->Open(m_hWnd))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
		else
		{
			//m_pWnd�Ǹ��Ӵ��ڣ����������ô��ڵ���ʾλ��
			CRect rc;
			GetClientRect(rc);
			m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	
4.���ɼ�������Ƶ�͵���ʾ����ȥ��ʾ
	if (m_pCamera!=NULL && m_pWnd!=NULL)
	{
		m_pCamera->AddImageSink(m_pWnd);
	}
5.����RTPѹ��ͨ��
	�̳�VIDEC_CapChanRTPCallback,�ں���OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)�л�ȡѹ��������RTP���ݰ�
	VIDEC_CapChanRTP*	m_pCapChanRTP;
	m_pCapChanRTP=VIDEC_CapChanRTP::Create(*this);
	if (m_pCapChanRTP!=NULL)
	{
		int nPayloadType=97;
		switch (m_CodecType)
		{
		case VIDEC_CODEC_H261:
			nPayloadType=31;
			break;
		case VIDEC_CODEC_H263:
		case VIDEC_CODEC_H263P:
		case VIDEC_CODEC_H263PP:
			nPayloadType=34;
			break;
		default:
			nPayloadType=97;
			break;
		};
		
		//�򿪲ɼ�ͨ��ѹ��ͨ��
		//nPayloadType:RTP�ĸ�������
		//nWidth,ͼ����
		//nHeight,ͼ��߶�
		//nFrameRate,֡�ʣ�1��5��10��15��30
		//nQuality,ͼ��������0��64,1:128,2:256,3:384,4:512,5:768,6:1024,7:1536,8:2048,9:4096:10:6144
		//CodecType����Ƶ���룬VIDEC_CODEC_H261��VIDEC_CODEC_H263��VIDEC_CODEC_H263P,VIDEC_CODEC_H263PP,VIDEC_CODEC_H264
		//�ɹ�����0��ʧ�ܷ���-1
		
		//ע�⣺VIDEC_CODEC_H261ֻ֧��QCIF(176X144)��CIF(352X288)
		//VIDEC_CODEC_H263ֻ֧��QCIF(176X144)��CIF(352X288)��4CIF(704X576)
		//VIDEC_CODEC_H263P,VIDEC_CODEC_H263PP��VIDEC_CODEC_H264֧��������Ƶ��ʽ
		if (0!=m_pCapChanRTP->Open(nPayloadType,m_nWidth,m_nHeight,m_nFrameRate,m_nQuality,m_CodecType))
		{
			m_pCapChanRTP->Close();
			delete m_pCapChanRTP;
			m_pCapChanRTP=NULL;
		}
6.���ɼ�������Ƶ��ȥѹ��
		if (m_pCamera!=NULL && m_pCapChanRTP!=NULL)
		{
			m_pCamera->AddImageSink(m_pCapChanRTP);
		}
7.�ں���OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)�н�RTP���ݰ����͵���·
	//pCapChanRTP:ѹ��ͨ��
	//pPacketData:RTP���ݰ�ָ��
	//nPacketLen:RTP���ݰ�����
	//��ʾ�������ݰ��Ѿ��ǰ�����RTPͷ�����ݰ���ֻҪ��ָ���ĵ�ַ�Ͷ˿ڷ��;Ϳ�����
	void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
	{
		//
	}
8.����ѹ��ͨ��
	if (m_pCapChanRTP!=NULL)
	{
		m_pCapChanRTP->Close();
		delete m_pCapChanRTP;
		m_pCapChanRTP=NULL;
	}
9.���ٲɼ��豸
	if (m_pCamera!=NULL)
	{
		m_pCamera->Close();
		delete m_pCamera;
		m_pCamera=NULL;
	}
10.������ʾ����
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
11.����ʼ��
	VIDEC_Terminate();
	::CoUninitialize();
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VIDECRTP������ʾʹ��˵��:
1.��ʼ��
	::CoInitialize(NULL);
	VIDEC_Init();

2.��������ͨ��
	VIDEC_PlayChanRTP*	m_pPlayChanRTP;
	m_pPlayChanRTP=VIDEC_PlayChanRTP::Create();
	if (m_pPlayChanRTP!=NULL)
	{
		int nPayloadType=97;
		switch (m_CodecType)
		{
		case VIDEC_CODEC_H261:
			nPayloadType=31;
			break;
		case VIDEC_CODEC_H263:
		case VIDEC_CODEC_H263P:
		case VIDEC_CODEC_H263PP:
			nPayloadType=34;
			break;
		default:
			nPayloadType=97;
			break;
		};
		if (0!=m_pPlayChanRTP->Open(nPayloadType,m_CodecType))
		{
			m_pPlayChanRTP->Close();
			delete m_pPlayChanRTP;
			m_pPlayChanRTP=NULL;
		}

3.������ʾ����
	VIDEC_Wnd*		m_pWnd;
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		//����ʾ����
		//hWndParent����ʾ���ڵĸ����ھ��������������Ч���
		//������������Ĭ��ֵ
		//�ɹ�����0��ʧ�ܷ���-1
		if (0!=m_pWnd->Open(m_hWnd))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
		else
		{
			//m_pWnd�Ǹ��Ӵ��ڣ����������ô��ڵ���ʾλ��
			CRect rc;
			GetClientRect(rc);
			m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	
4.����������Ƶ�͵���ʾ����ȥ��ʾ
	if (m_pPlayChanRTP!=NULL && m_pWnd!=NULL)
	{
		m_pPlayChanRTP->AddImageSink(m_pWnd);
	}

5.�����յ���RTP������ȥ����
	if (m_pPlayChanRTP!=NULL)
	{
		//pPacketData:RTP���ݰ�ָ��
		//nPacketLen:RTP���ݰ�����
		//ע�⣺�����ݰ������ǰ�����RTPͷ�����ݰ�
		m_pPlayChanRTP->OnRecvdRTPPacket((unsigned char*)pPacketData,nPacketLen);
	}

6.���ٽ�ѹ��ͨ��
	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->Close();
		delete m_pPlayChanRTP;
		m_pPlayChanRTP=NULL;
	}
7.������ʾ����
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
8.����ʼ��
	VIDEC_Terminate();
	::CoUninitialize();

