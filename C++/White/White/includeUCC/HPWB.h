#ifndef _HPWB_H
#define _HPWB_H


typedef struct {	
	unsigned long ulObjId;	
	unsigned char byPageId;	
	unsigned char byTabId;
	unsigned char byType;
}WB_COMMAND_HEAD,*LPWB_COMMAND_HEAD, *PWB_COMMAND_HEAD;

enum TAB_CTRL_FLAG
{
	TAB_CTRL_WHITEBOARD = 0,	
	TAB_CTRL_SHAREVIDEO
};

typedef struct {
	RECT			rtToolBar;
	POINT			ptWhiteBoard;	
	DWORD			dwWhiteBoardPage;	
	DWORD			dwWhiteBoardTotalPage;	
	UCHAR			byTabId;
	BOOL			bFullScreen;
}CLIENT_WINDOW_POSITION;

#define WB_TOOL_BG_COLOR RGB(236,233,216)
//#define WB_TOOL_BG_COLOR RGB(183,183,183)
#define WB_TOOL_SELECT_BG_COLOR RGB(254,254,254)
#define DATA_TRANS_ADDOBJECT			1
#define DATA_TRANS_LOADFILE				2
#define DATA_TRANS_MOVEOBJ				3
#define DATA_TRANS_OBJ_RESIZE			4
#define DATA_TRANS_DELETE_ALL			5
#define DATA_TRANS_DELETE_OBJECT		6
#define DATA_TRANS_SCROLL				7
#define DATA_TRANS_COLORREF_CHANGED		8
#define DATA_TRANS_LINE_WIDTH_CHANGED	9
#define DATA_TRANS_FONT_CHANGED			10
#define DATA_TRANS_TEXT_CHANGED			11
#define DATA_TRANS_POINTER_CMD			12
#define DATA_TRANS_LOCK					13
#define DATA_TRANS_PAGE					14
#define DATA_TRANS_SET_BACKBITMAP		15
#define DATA_TRANS_SET_BACKCOLOR		16
#define DATA_TRANS_CLEAR_ALL			17
#define DATA_TRANS_OBJREQUEST			18
#define DATA_TRANS_OBJRESPONSE			19
#define DATA_TRANS_OBJECT_ALL			20
#define DATA_TRANS_DOC_PAGE_CHANGED		21
#define DATA_TRANS_DOC_CLOSE			22
#define DATA_TRANS_DOC_LOADFILE			23
#define DATA_TRANS_TABCTRL_CHANGE		24
#define DATA_TRANS_FULLSCREEN			25
#define DATA_TRANS_TOOLBARMOVE			26
#define DATA_TRANS_WINDOWMOVE			27
#define DATA_TRANS_LASTPOSITION			28		// ȡ�õ�ǰ�λ�� data��ʽ  posx + posy (len=sizeof(int) * 2)


#define DATA_TRANS_REQUESTTOKEN				29      //��ѯ����״̬  //ADD BY WZF 2006.04.21
#define DATA_TRANS_RESPONSETOKENSTATE		30      //��������״̬   //ADD BY WZF 2006.04.21
#define DATA_TRANS_FILEPRINTFINISH			31		//��ӡ�����Ϣ   //ADD BY WZF 2006.04.21
#define DATA_TRANS_FILEPRINTCANCEL			32		//�ļ���ӡ�˳�

#define DATA_TRANS_REQUESTLASTPOSITION			33		// ȡ�õ�ǰ�λ�� data��ʽ  posx + posy (len=sizeof(int) * 2)
#define DATA_TRANS_RESPONSELASTPOSITION			34		// ȡ�õ�ǰ�λ�� data��ʽ  posx + posy (len=sizeof(int) * 2)

#define DATA_TRANS_SHAREFILE_END		37 //add by wzf on 2006.05.16
#define DATA_TRANS_REQUESTSHAREFILE		38
#define DATA_TRANS_RESPONSESHAREFILE	39

#define DATA_TRANS_DTOBJ_HEAD		40 //add by wzf on 2006.05.25
#define TRANS_REQUESTDTOBJDATA			41 //add by wzf on 2006.05.26
#define TRANS_RESPONSEDTOBJDATA			42 //add by wzf on 2006.05.26
#define TRANS_CLEAR_DTOBJDATA			44 //add by wzf on 2006.05.26
#define STOP_TRANS_SHARE_FILE			45

#define DATA_TRANS_ROATE				  46
#define DATA_TRANS_DELETE_OBJATTRIBUTE    47
#define DATA_TRANS_DELETE_ALLOBJATTRIBUTE 48
#define DATA_TRANS_PUT_OBJATTRIBUTE       49
#define DATA_TRANS_GET_OBJATTRIBUTE       50
#define DATA_TRANS_GET_ALLOBJATTRIBUTE    51
//����
#define DATA_TRANS_PREMOVE_LAYOUT         52
#define DATA_TRANS_BACKMOVE_LAYOUT        53
#define DATA_TRANS_MOVETOHEAD_LAYOUT      54
#define DATA_TRANS_MOVETOTAIL_LAYOUT	  55
#define DATA_TRANS_BK_LAYOUT			  56
#define DATA_TRANS_REQUESTBKCOLOR         57
class IHPWhiteBoardNotify
{
public:
	/*���ذװ��������緢�ͽӿڡ�������a���װ��ڲ��Զ������ݽṹ b�����ݳ���*/
	virtual bool onWBDataOut(char* pData,long &nLen)= 0;
	virtual void onAllWBData(char* pData,long &nLen)= 0;

	//�ڲ��װ�ȫ����֪ͨ�ӿڡ� ����Ϊ�ı�����Ļ��ʾ״̬
	virtual void onWBFullScreen(bool bFull) = 0;

	virtual void MyRemoveWindowSkin(HWND hWnd)=0;
	virtual void MyEnableWindowScrollbarSkin(HWND hWnd)=0;
	virtual void UpdateMainWindow()=0;//���������򴰿�,��Ҫ���ڰװ�ץ��
	virtual bool IsInMeeting()=0;//�Ƿ��ڻ�����
	virtual bool IsMyAuditor()=0;  //������
	virtual void ShowMainWindow(int nShowCmd)=0; //�װ�ץ��ʱ�����ڵĴ���
	virtual void WBNotify_OnLButtonDown(UINT nFlags, CPoint point){};  //�߼������϶��װ���	

};

class IHPWhiteBoard
{
public:
	/*��ʼ���װ塣������a�������ھ�� b���ⲿ�ӿ�*/
	virtual bool Connect(CWnd *pParent)=0;

	/*ע���װ塣*/
	virtual void releaseConnection()=0;

	/*����װ��ڲ��Զ������ݽṹ��������a���װ����ݰ� b�����ݰ�����*/
	virtual bool processWBData(char * pData,long nLen)=0;

	/*���ðװ崰��λ��*/
	virtual void setWBPosition(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;

	/*���ù���������λ��*/
	virtual void setTOOLPosition(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;

	/*���ðװ崰�ڿɼ�����*/
	virtual void setShowWindow(bool bShow)=0;

	//virtual void On_DataOut(char *pData, long nLen)=0;

	/*ͨ��PageId, ObjId��ѯ����*/
	virtual void processWBObjRequest(unsigned char byPageId, unsigned long ulObjId)=0;

	//֪ͨ�װ�ȫ��
	virtual void processFullScreen(bool bFull) = 0;

	//�Ƿ�Ϊ�װ�ķ�����
	virtual void SetWBHost(bool bWBHost)=0;// add by wzf 2006.04.20

	virtual void SetJoinerNum(int nNum) =0;

	virtual void SetLocalUserID(CString strUserID)=0;

	virtual void On_DataOut(char *pData, long nLen) = 0;

	//��ȡ���аװ�����
	virtual void RequestAllWBData()=0;
	/*���ðװ崰�ڿɼ����� //����ר��*/
	virtual void setShowWBToolDlg(bool bShow)=0;

	/*Enables or Disables all whiteboard dialog*/
	virtual void EnableAllWBDlg(BOOL bEnable)=0;

	/*������аװ�����*/
	virtual void ClearLocalData(void)=0;

	//added by luoning 2010-8-5
	virtual BOOL OpenPic(CString strPath) = 0;
	//end

	// ���ذװ�ͼƬ������ [5/24/2011 %x.t%]
	virtual void HidePicBar() = 0;
};

AFX_EXT_CLASS IHPWhiteBoard * CreateWB(IHPWhiteBoardNotify * pNotify,CString strLanguagePath);

#endif