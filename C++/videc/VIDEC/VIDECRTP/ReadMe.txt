VIDECRTP采集编码使用说明:
1.初始化
	::CoInitialize(NULL);
	VIDEC_Init();
	
2.打开采集设备
	VIDEC_CameraDev*	m_pCamera;
	m_pCamera=VIDEC_CameraDev::Create();
	if (m_pCamera!=NULL)
	{
		//打开设备
		//nDevID设备号，从0开始
		//nWidth,图象宽度
		//nHeight,图象高度
		//nFrameRate,帧率，1，5，10，15，30
		//bDefaultFormat:当输入的图像宽度或高度设备不支持时是否采用默认格式
		//成功返回0，失败返回-1
		//注意：大部分的USB摄像头不支持704X576,只支持176X144,320X240,352X288,640X480;如果bDefaultFormat==false并且
		//输入的图像宽度或高度设备不支持时,打开设备会失败
		if (0!=m_pCamera->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate,false))
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}
	}
	
3.创建显示窗口
	VIDEC_Wnd*		m_pWnd;
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		//打开显示窗口
		//hWndParent，显示窗口的父窗口句柄，必须输入有效句柄
		//其它参数请用默认值
		//成功返回0，失败返回-1
		if (0!=m_pWnd->Open(m_hWnd))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
		else
		{
			//m_pWnd是个子窗口，您可以设置窗口的显示位置
			CRect rc;
			GetClientRect(rc);
			m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	
4.将采集到的视频送到显示窗口去显示
	if (m_pCamera!=NULL && m_pWnd!=NULL)
	{
		m_pCamera->AddImageSink(m_pWnd);
	}
5.创建RTP压缩通道
	继承VIDEC_CapChanRTPCallback,在函数OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)中获取压缩大包后的RTP数据包
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
		
		//打开采集通道压缩通道
		//nPayloadType:RTP的负荷类型
		//nWidth,图象宽度
		//nHeight,图象高度
		//nFrameRate,帧率，1，5，10，15，30
		//nQuality,图象质量，0：64,1:128,2:256,3:384,4:512,5:768,6:1024,7:1536,8:2048,9:4096:10:6144
		//CodecType：视频编码，VIDEC_CODEC_H261，VIDEC_CODEC_H263，VIDEC_CODEC_H263P,VIDEC_CODEC_H263PP,VIDEC_CODEC_H264
		//成功返回0，失败返回-1
		
		//注意：VIDEC_CODEC_H261只支持QCIF(176X144)和CIF(352X288)
		//VIDEC_CODEC_H263只支持QCIF(176X144)和CIF(352X288)和4CIF(704X576)
		//VIDEC_CODEC_H263P,VIDEC_CODEC_H263PP和VIDEC_CODEC_H264支持所有视频格式
		if (0!=m_pCapChanRTP->Open(nPayloadType,m_nWidth,m_nHeight,m_nFrameRate,m_nQuality,m_CodecType))
		{
			m_pCapChanRTP->Close();
			delete m_pCapChanRTP;
			m_pCapChanRTP=NULL;
		}
6.将采集到的视频送去压缩
		if (m_pCamera!=NULL && m_pCapChanRTP!=NULL)
		{
			m_pCamera->AddImageSink(m_pCapChanRTP);
		}
7.在函数OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)中将RTP数据包发送到网路
	//pCapChanRTP:压缩通道
	//pPacketData:RTP数据包指针
	//nPacketLen:RTP数据包长度
	//提示：此数据包已经是包含了RTP头的数据包，只要向指定的地址和端口发送就可以了
	void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
	{
		//
	}
8.销毁压缩通道
	if (m_pCapChanRTP!=NULL)
	{
		m_pCapChanRTP->Close();
		delete m_pCapChanRTP;
		m_pCapChanRTP=NULL;
	}
9.销毁采集设备
	if (m_pCamera!=NULL)
	{
		m_pCamera->Close();
		delete m_pCamera;
		m_pCamera=NULL;
	}
10.销毁显示窗口
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
11.反初始化
	VIDEC_Terminate();
	::CoUninitialize();
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VIDECRTP解码显示使用说明:
1.初始化
	::CoInitialize(NULL);
	VIDEC_Init();

2.创建解码通道
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

3.创建显示窗口
	VIDEC_Wnd*		m_pWnd;
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		//打开显示窗口
		//hWndParent，显示窗口的父窗口句柄，必须输入有效句柄
		//其它参数请用默认值
		//成功返回0，失败返回-1
		if (0!=m_pWnd->Open(m_hWnd))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
		else
		{
			//m_pWnd是个子窗口，您可以设置窗口的显示位置
			CRect rc;
			GetClientRect(rc);
			m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	
4.将加码后的视频送到显示窗口去显示
	if (m_pPlayChanRTP!=NULL && m_pWnd!=NULL)
	{
		m_pPlayChanRTP->AddImageSink(m_pWnd);
	}

5.将接收到的RTP数据送去解码
	if (m_pPlayChanRTP!=NULL)
	{
		//pPacketData:RTP数据包指针
		//nPacketLen:RTP数据包长度
		//注意：此数据包必须是包含了RTP头的数据包
		m_pPlayChanRTP->OnRecvdRTPPacket((unsigned char*)pPacketData,nPacketLen);
	}

6.销毁解压缩通道
	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->Close();
		delete m_pPlayChanRTP;
		m_pPlayChanRTP=NULL;
	}
7.销毁显示窗口
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
8.反初始化
	VIDEC_Terminate();
	::CoUninitialize();

