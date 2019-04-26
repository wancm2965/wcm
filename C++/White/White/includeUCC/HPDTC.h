
//========================================================================================
//��Ȩ����: ��Ȩ����(C) 2008���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
//ϵͳ����: AVCON7
//��������: duquansheng
//��������: 2008-5-19
//����˵��: �������ݿͻ��˶�̬��DTC.dll(�ĵ�����flash����)
//

#ifndef _HPDTC_H
#define _HPDTC_H


#define USE_AVCONDOC_PRINT	//����avcondoc�����ӡ
#undef  USE_AVCONDOC_PRINT


//�ĵ�����ص�
class IHPDTCDocShareNotify
{
public:
	IHPDTCDocShareNotify(){};
	~IHPDTCDocShareNotify(){};	

public:
	virtual void onDocShareNotifyData(const char* pData,int nLen) = 0;			//��������
	virtual void onDocShareNotifyRecordData(const char* pData,int nLen) = 0;	//¼������
	virtual void onDocShareNotifyClose(void) = 0;								//�ڲ��رջص�
	virtual void onDocShareNotifyOnLButtonDown(UINT nFlags, CPoint point){};	//�����϶�������		
	virtual void onDocShareNotifyFullScreen(bool bFull,bool bSelf){};			//�Ƿ�ȫ������bSelf:�Ƿ��Լ������ȫ��
	virtual void onDocShareNotifyShowMode(bool bMode){};						//�ı���ʾģʽbMode���Ƿ��ĵ�������ʾ
	virtual std::string GetLocalMemberID(void) = 0;								//��ȡ�����û�ID
	virtual void onDocShareNotifyShowImgFile(void) = 0;							//�ĵ�������ϲ�����ʾ
	virtual	bool IsConferencePlayer(void) = 0;									//�Ƿ�Ϊ���鲥����
	virtual	bool IsPrivilegeMaxWindow(void) = 0;								//�Ƿ���ȫ������Ȩ��
	virtual void onDocShareNotifyActive(const std::string& strMemberID, unsigned long ulDocIndex) =0;
};

//�ĵ�����
class IHPDTCDocShare
{
public:
	IHPDTCDocShare(){};
	~IHPDTCDocShare(){};
public:
	virtual bool Connect(void) = 0;										//��������
	virtual void Release(void) = 0;										//�ر��ͷ���Դ
	virtual void processDocShareData(const char* pData,int nLen) = 0;	//��������
	virtual void SetLabelRight(bool bAllow) = 0;						//���ñ�עȨ�ޣ�Ĭ������	
	virtual void SetCloseRight(bool bAllow) = 0;						//���ùر�Ȩ�ޣ�Ĭ������	
	virtual void StartRecord(void) = 0;									//����¼��
	virtual void StopRecord(void) = 0;									//ֹͣ¼��
	virtual void processRecordData(const char* pData,int nLen) = 0;		//����¼������
	virtual unsigned short GetDocWidth(void) = 0;						//��ȡ�ĵ����
	virtual unsigned short GetDocHeight(void) = 0;						//��ȡ�ĵ��߶�
	virtual HWND GetDocHwnd(void) = 0;									//��ȡ����
	virtual void SetDocWindowPos(int x,int y,int cx,int cy) = 0;		//�����ĵ����ڴ�С
	virtual void ShowDocWindow(bool bShow) = 0;							//��ʾ/���ش���
	virtual void SetFullScreen(bool bFull) = 0;							//����ȫ��
	virtual void SetMemberName(std::string strMemberName) = 0;			//�����û�����
	virtual std::string GetMemberName(void) = 0;						//�����û�����
	virtual std::string GetMemberID(void) = 0;							//��ȡ�û�ID
	virtual unsigned long GetDocIndex(void) = 0;						//��ȡDocIndex:�ĵ����
	virtual bool IsShowImgFile(void) = 0;								//�Ƿ��Ѵ��ĵ�
	virtual bool IsShowMode(void) = 0;									//�Ƿ��ĵ�������ʾ
	virtual void HideShortDlg(void) = 0;								//�������п�ݰ�ť
	virtual void OnParentMove(bool IsParentShow) = 0;
	virtual void OnControlDirect(DWORD dDerict)=0;						//�����
};

//�����ĵ�����ʵ����strFile ��Ҫ��ӡ���ļ����˺����ӿ����ĵ���ӡ��������ӡʧ�ܷ��� NULL
AFX_EXT_CLASS IHPDTCDocShare* CreateDocShare(const std::string& strFile,unsigned long ulDocIndex,const std::string& strMemberID,IHPDTCDocShareNotify& rNotify,CWnd* pWnd);


//�ļ��ַ��ص�
class AFX_EXT_CLASS IHPDTCFileSendNotify
{
public:
	IHPDTCFileSendNotify(){};
	~IHPDTCFileSendNotify(){};	

public:
	virtual void onFileSendNotifyData(const char* pData,int nLen) = 0;			//�������ݵ�����
    virtual void AutoPopupFileTip(CString UpFileUserName) = 0;
};
//�ļ��ַ�
class AFX_EXT_CLASS IHPDTCFileSend
{
public:
	IHPDTCFileSend(){};
	~IHPDTCFileSend(){};
public:
	virtual bool Connect(void) = 0;										//��������
	virtual void Release(void) = 0;										//�ر��ͷ���Դ
	virtual void processFileSendData(const char* pData,int nLen) = 0;	//������������	
	virtual void ShowFileWindow(bool bShow, bool bIsMyHost = true) = 0;	//��ʾ����	nShowType����ʾ���� ��Ϊ�����ˣ���Ϊ�����
	virtual void SetIsMyHost(bool bIsHost)=0;
};

//�����ĵ�����ʵ����strFile ��Ҫ��ӡ���ļ����˺����ӿ����ĵ���ӡ��������ӡʧ�ܷ��� NULL
AFX_EXT_CLASS IHPDTCFileSend* CreateFileSend(const std::string& strMemberID, IHPDTCFileSendNotify& rNotify, CWnd* pWnd, bool bIsMyHost = true);
AFX_EXT_CLASS void DeteleFileSend(IHPDTCFileSend* pHPDTCFileSend);
/*


//Flash����ص�
class IHPDTCFlashShareNotify
{
public:
	IHPDTCFlashShareNotify(){};
	~IHPDTCFlashShareNotify(){};	

public:
	virtual void onFlashShareNotifyNetData(const char* pData,int nLen) = 0;		//���͵���������
	virtual void onFlashShareNotifyRecordData(const char* pData,int nLen) = 0;	//¼������
	virtual void onFlashShareNotifyClose() = 0;									//�ڲ��رջص�
	virtual void onFlashShareNotifyOnLButtonDown(UINT nFlags, CPoint point){};	//�����϶�������		

};

//Flash����
class IHPDTCFlashShare
{
public:
	IHPDTCFlashShare(){};
	~IHPDTCFlashShare(){};
public:
	virtual void Release() = 0;											//�ر��ͷ���Դ
	virtual void processFlashShareData(const char* pData,int nLen) = 0;	//������������
	virtual void StartRecord(void) = 0;									//����¼��
	virtual void StopRecord(void) = 0;									//ֹͣ¼��

};

//����Flash����
AFX_EXT_CLASS IHPDTCFlashShare * CreateFlashShare(const std::string& strFile,const std::string& strUserID,IHPDTCFlashShareNotify& rNotify,CWnd* pWnd);

*/

#endif
