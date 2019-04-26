#ifndef __HPMAPDLL_H_
#define __HPMAPDLL_H_

class HPMAPDLLNotify
{
public:
	HPMAPDLLNotify(){}
	virtual ~HPMAPDLLNotify(){}
	
	virtual void HPMAP_OnSaveOneMap(int code,CString strHash,CString strMsg)=0; //code�ص��ɹ����ߴ�������,strHash�����MD5,strMSG������Ϣ
	virtual void HPMAP_OnDownLoadOneMap(int code,CString strHash,CString strMsg)=0;//code�ص��ɹ����ߴ�������,strHash�����MD5,strMSG������Ϣ
	virtual void HPMAP_OnOpenOneMap(int code,CString strHash,CString strMsg)=0;//code�ص��ɹ����ߴ�������,strHash�����MD5,strMSG������Ϣ
	virtual void HPMAP_OnCloseOneMap(int code,CString strHash,CString strMsg)=0;//
	virtual void HPMAP_OnDeleteOneMap(int code,CString strHash,CString strMsg)=0;
	virtual void HPMAP_OnDeleteOneBZ(int id)=0;//ɾ��һ����ע
	virtual void HPMAP_OnObjDBClick(int id,UINT nFlags,CPoint point)=0; //˫��һ����ע
	virtual void HPMAP_OnObjSelect (int id)=0; //ѡ��һ����ע
	virtual void HPMAP_OnLevel(CString strHash)=0; //������˻ص���һ����ȵ��������ʱ������
	virtual void HPMAP_OnDragDown(HPMC_DROP* dropdata,HWND hWnd,int x,int y)=0;//����һ����������ͼ�ϣ��������ϵ�treeid�͵�ͼx��y���ꡣ
	virtual void HPMAP_DoCloseMap()=0;//��ͼ�ڲ��˵����֪ͨ�ⲿ���ر��Լ�
	virtual void HPMAP_DoSaveMap()=0;
	virtual void HPMAP_DoEditMap()=0;
	virtual void HPMAP_OnUserBtnDown()=0;
public:

};

class HPMAPDLL
{
public:
	HPMAPDLL() {};
	virtual ~HPMAPDLL() {};

public:
	virtual HWND GetMapCtrlHwnd(void) = 0;
	virtual void ShowMapCtrl(BOOL bShow)=0;
	virtual void MoveMapCtrl(int x,int y,int w,int h)=0;
	virtual void SetMapCtrlPath(CString maproot,CString mapServer,CString uploadPath,unsigned short	sServerPort)=0;//maproot ��ͼ��Ŀ¼ mapServer 
	virtual BOOL OpenOneMap(CString strHash)=0; //��һ�ŵ�ͼ
	virtual BOOL DownLoadMap(CString strHash)=0;//����hash����һ�ŵ�ͼ
	virtual BOOL DeleteMap(CString strHash)=0;//ɾ����ͼ�ļ�
	virtual void SaveOneMap()=0;//�����˱�����ϴ��Ĳ���
	virtual BOOL CloseOneMap()=0;//�ر�һ����ͼ
	virtual void SetMapState(int state)=0; //0 ��� 1 �༭ 2 �ϴ���
	virtual int  GetMapState()=0;
	virtual int  AddMapPoint(int type,CString strName,int x=50,int y=50)=0; //��ӱ�ע
	virtual int  ModMapPoint(int id,int type,CString strName)=0;//�޸ı�ע
	virtual BOOL QueryMapPoint(int id,int &type,CString &strName)=0;//��ѯ��ע
	virtual BOOL IsMapChanged()=0;
	virtual void SaveOneMapByTreeID(CString strMapName)=0;
	virtual CString GetMapMD5(CString strMapName)=0;
	virtual void SetFlash(int objIndex,BOOL bResh)=0;
	virtual	void SetMapPrivEdit(BOOL bPrivEdit,unsigned int uAviewType)=0;
	virtual void SetMapSaveName(string strMapName)=0;
	virtual string GetMapSaveName()=0;
	virtual void SetMapUseMode(int nMode)=0;//0:Ϊ���ģʽ1:Ϊ����ģʽ
	};

AFX_EXT_CLASS HPMAPDLL* HPMAPDLL_CreateHPMapCtrl(int nMode,HPMAPDLLNotify* pNotify,CWnd*pParent);
AFX_EXT_CLASS void HPMAPDLL_ReleaseHPMapCtrl();
 

#endif


/*
//////////////////////////////////�ӿ�˵��////////////////////////////////////////
 
 GetMapCtrlHwnd 
 Description: ��õ�ͼ�ؼ�����WIN32���.
 Usage: virtual HWND GetMapCtrlHWnd ();;
 Parameters: None
 Return Value: HWND
 Example: None
 Remarks: None

 ShowMapCtrl  
 Description:��ʾ�������ص�ͼ�ؼ�.
 Usage: virtual void ShowMapCtrl(BOOL bShow);;
 Parameters: BOOL
 Return Value: void
 Example: ShowMapCtrl(False);//���ص�ͼ�ؼ�
 Remarks: None

 MoveMapCtrl 
 Description: �ƶ���ͼ�ؼ���λ��
 Usage: virtual void MoveMapCtrl (int x,int y,int w,int h);
 Parameters: int x:�ؼ��������Ͻ��ƶ���X���� 
 int y:�ƶ���Y����
 int w:�ؼ�����Ŀ�� 
 int h:�ؼ�����ĸ߶�
 Return Value: None
 Example: None
 Remarks: 

 SetMapCtrlPath 
 Description: ���õ�ͼ�ؼ�·����Ϣ
 Usage: virtual SetMapCtrlPath(Cstring maproot,CString mapServer ,CString uploadPath)
 Parameters: CString maproot: ��ͼ�ļ���·��
 CString mapServer:��ͼ���صĸ�Ŀ¼
 CString uploadPath:��ͼ�ϴ���HTTP·��
 Return Value: None
 Example: None
 Remarks: 

 OpenOneMap 
 Description: ��һ�ŵ�ͼ
 Usage: virtual BOOL OpenOneMap(CString strHash);
 Parameters: CString strHash ��ͼ�ļ��Ĺ�ϣֵ������64λ���ַ���
 Return Value: �򿪳ɹ�����ʧ��
 Example: None
 Remarks: ���ô˰취�������յ��ص�OnOpenOneMap(int code,CString strHash,CString strMsg)�¼����ص��¼��Ĳ���˵�����£�
 Int Code:�ص��¼��� 1 ��ʾ�򿪳ɹ� 0��ʾ��ͼ�����ڻ��߻�û������
 CString strHash:�õ�ͼ�ļ��Ĺ�ϣֵ
 Cstring strMsg:���ص���ʾ�ַ�����Ϣ


 DownLoadMap () 
 Description: ����һ�ŵ�ͼ
 Usage: virtual virtual BOOL DownLoadMap(CString strHash);
 Parameters: ulRoomID������ID
 Return Value: None
 Example: None
 Remarks: ���ô˷������첽�õ�OnDownLoadOneMap(int code,CString strHash,CString strMsg)�¼� �ص��¼��Ĳ������£�
 Int Code:�ص��¼��� 1 ��ʾ���سɹ� 0����ʧ��(�����жϻ���ȡ�����ߵ�ͼ�����������ڸõ�ͼ)
 CString strHash:�õ�ͼ�ļ��Ĺ�ϣֵ
 Cstring strMsg:���ص���ʾ�ַ�����Ϣ


 SaveOneMap  
 Description: �յ�׼���������
 Usage: virtual void SaveOneMap ();
 Parameters: ulRoomID������ID
 Return Value: None
 Example: None
 Remarks: ����SaveOneMap���첽�յ�OnSaveOneMap�¼���
 �¼���������
 Int Code:�ص��¼��� 1 ��ʾ���沢�ϴ��ɹ� 0��ʾ����ʧ��
 CString strHash:�õ�ͼ�ļ��¹�ϣֵ
 Cstring strMsg:���ص���ʾ�ַ�����Ϣ

 CloseOneMap  
 Description: �رյ�ǰ�����ĵ�ͼ���ָ�����ͼ�ĳ�ʼ״̬
 Usage: virtual void CloseOneMap ();
 Parameters: None
 Return Value: None
 Example: None
 Remarks: �ص�OnCloseOneMap(int code,CString strHash,CString strMsg)=0;//
 �¼����¼���������
 Int Code:�ص��¼��� 1 ��ʾ��ͼ�Ѿ��ر� 0��ʾ�ر�ʧ��
 CString strHash:�رյĵ�ͼ�ļ���ϣֵ
 Cstring strMsg:���ص���ʾ�ַ�����Ϣ

 DeleteMap  
 Description: ���µ�ͼ�󣬿ͻ���ͨ���˽ӿ�ɾ��ָ����ϣ�ĵ�ͼ
 Usage: virtual void DeleteMap (Cstring strHash);
 Parameters: Cstring strHash ��ͼ��ϣֵ
 Return Value: BOOL TRUE �ɹ� FALSE ʧ��
 Example: None
 Remarks:

 SetMapState 
 Description: ���õ�ͼ״̬
 Usage: virtual void SetMapState(int state);
 Parameters: int state ״̬ 0 ��� 1 �༭ 2 �ϴ���
 Return Value: None
 Example: None
 Remarks:��

 GetMapState  
 Description: ��ȡ��ǰ��ͼ״̬
 Usage: virtual int GetMapState ()
 Parameters: None
 Return Value: int state ״̬ 0 ��� 1 �༭ 2 �ϴ���


*/