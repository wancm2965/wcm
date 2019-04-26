


#if !defined(AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_WBDEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WB_API  _declspec(dllexport)
class IHPNetNotify;
class IHPWBNotify;
enum DRAW_STATE   //����״̬
{
	
	DRAW_START = 0,   //��ʼ״̬
	DRAW_NORMAL,  //һ�㻯״̬
	TODUSTBIN_OVER,		//�϶��������䲢ɾ���ɹ�
	MOVEING_RULERBTN,  //�ƶ�ֱ��Բ
	ENTER_SETSQUARE,   //���ǰ�
	ENTER_COMPASS,     //Բ��
	DRAW_SELECTINT, //�þ���ѡ��ʱ��ѡ���
	ENTER_VIEW_BUTTON,	//���аװ��ϵİ�ť
	ENTER_BUTTON,	//����ѡ����ϵİ�ť
	ENTER_PROTRACTOR,//������

	ENTER_CONTROL,	//���п��ư�ť
	DRAW_LINE,    //��ֱ��״̬
	DRAW_POLYLINE, //��������
	DRAWING_LINE, //���ڻ�ֱ�ߣ��������Ѿ����£�
	DRAWING_POLYLINE,  //���ڻ�������
	DRAW_TEXT,		//��������
	DRAWING_TEXT,	//������������
	DRAWED_TEXT,    //���ֺ�ע���������
	DRAW_RECT,  //������
	DRAWING_RECT, //���ڻ�����
	DRAW_POLYGON, //�������
	DRAWING_POLYGON, //���ڻ������
	DRAWEND_POLYGON, //��һ������ν���
	DRAW_ELLIPSE,	//����Բ״̬
	DRAWING_ELLIPSE,	//���ڻ���Բ
	DRAW_TRIANGLE,	//��������
	DRAWING_TRIANGLE,	//���ڻ�������
	DRAW_RHOMBUS,	//������
	DRAWING_RHOMBUS,	//���ڻ�����
	MOVING_LINE,  //�ƶ�ֱ��
	DRAW_ROUND,    //��ת״̬
	DRAW_ERASE,   //����
	DRAW_ERASING,  //���ڲ���
	DRAW_CLEANUP,  //���
	DRAW_CLEANUPING, //�������
	DRAW_RGNDELETE, //����ɾ��
	DRAW_RGNDELETING, //��������ɾ��
	ADDPICTURE,		//���ͼƬ
	ADDDICOM,       //���DICOMҽ��ͼƬ
	ADDMOVIE,		//�����Ƶ
	ADDNEWMOVIE,	//�������Ƶ
	ADDCAMERA,		//�������ͷ
	ADDDOC,			//����ĵ�
	ADDMUISC,		//�������	
	ADDFLASH,		//���flash
	ADD_ANNOTATIONS, //��ӱ�ע
	CHANGE_ANNOTATIONS, //�ı��ע
	DRAW_RECTMAQN,   //�Ŵ�
	DRAW_SLIDER_MAQN,//�Ŵ��ϵĻ�����
	MEDIA_CTRLING,     //���ڿ�����Ƶ
	MOVE_DUSTBIN,		//�ƶ�������
	ADDFLASHFROMRESDLG,//��Դ�������flash
	ADDDOCFROMRESDLG,//��Դ��������ĵ�
	ADDPICTUREFROMRESDLG,//��Դ�������ͼƬ
	ADDNEWMOVIEFROMRESDLG,//��Դ�����������Ƶ
	ADDDOCFROMFOLDER,//�ⲿ�ļ��������ĵ�
	ADDFLASHFROMFOLDER,//�ⲿ�ļ�������flash
	ADDNEWMOVIEFROMFOLDER,//�ⲿ�ļ�����������Ƶ
	ADDPICTUREFROMFOLDER,//�ⲿ�ļ�������ͼƬ
    MOVE_CENTER_POINT,	//�ƶ����ĵ�	
	ADDRULER,//ֱ��
	DRAWEUP_TEXT, //���ֺ�ע���������,Upˢ��
	DRAWANYTRIANGLE, //������������
	DRAWINGANYTRIANGLE, //���ڻ�����������
	EDITTRIANGLE,       //�༭������
	EDITINGTRIANGLE,     //���ڱ༭������
	DRAW_PALLELOGAM,       //��ƽ���ı���
	DRAWING_PALLELOGAM,    //���ڻ�ƽ���ı���
	DRAW_PENTAGON,       //�������
	DRAWING_PENTAGON,    //���ڻ������
	DRAW_HEXAGON,       //��������
	DRAWING_HEXAGON,    //���ڻ�������
	DRAW_FIVEPOINTSTART,       //�������
	DRAWING_FIVEPOINTSTART,    //���ڻ������
	DRAW_TRAPEZIUM,       //������
	DRAWING_TRAPEZIUM,    //���ڻ�����
	MOVE_TRAPEZIUMPOINT,   //�ƶ����εĶ���
	DRAW_POLYICON,   //������ͼ������
	DRAWING_POLYICON,   //���ڻ�����ͼ������
	ADDDXF,  //���DXF�ļ�
	DXF_DRAG, //DXF��ק
	DXF_PREDRAG, //DXF��קǰ
	DRAW_MAGICLINE, //��ħ����
	DRAWING_MAGICLINE, //���ڻ�ħ����
	MIDI_DRAG,
	MIDI_DRAGING
};

enum BRUSH_SIZE //��Ƥ���ߴ�
{
	BRUSH_SMALL = 10,
	BRUNSH_BIG = 20
};

enum WB_LAYER_CHARNG
{
	WB_LAYER_SUPER = 1, //ͼ������
	WB_LAYER_UNDER, //ͼ������
	WB_LAYER_TOP, //ͼ���ö�
	WB_LAYER_BOTTOM, //ͼ���õ�
	WB_LAYER_BACKGROUND, //��Ϊ���� 
	WB_LAYER_DELETEBACKGROUND, //ɾ������
	WB_LAYER_DONOTHING, //������ 
};

typedef enum _eCommend_Type
{
	CMD_SCREEN = 0,//֪ͨ����ȫ��
	CMD_SINGLE_NETWORK,//����������������
	CMD_SINGLE_LOCAL,//�������ж�������������
	CMD_SINGLE_SHOTDOWN,//�������йػ�
	CMD_SINGLE_QUIT,//���������˳�����
	CMD_DROP_NOTIFY , //֪ͨU7��Ӧ������ק��Ϣ
	CMD_DROP_NOTIFY_TO_WB,  //֪ͨU7��Ӧ������ק��Ϣ���װ�
	CMD_FOCUS_GET,   //��ñ༭�򽹵�
	CMD_FOCUS_KILL   //ʧȥ�༭�򽹵�
}eCommend_Type;


enum WB_MEDIA_OPERATION
{
	WB_MOVIE_START	= 1,//��Ƶ��FLASH���Ż���ͣ
	WB_MOVIE_PAUSE,
	WB_MOVIE_STOP,				//��Ƶ��FLASHֹͣ
	WB_MOVIE_LOOP,				//��Ƶ��FLASHѭ��
	WB_MOVIE_SINGLEPLAY,        //FLASH���β���
	WB_MOVIE_SYNCHRONISATION,	//��Ƶ��FLASHͬ��

	WB_DOCUMENT_PREPAGE,		//�ĵ���һҳ
	WB_DOCUMENT_NEXTPAGE,		//�ĵ���һҳ
	WB_DOCUMENT_FIRSTPAGE,		//�ĵ���һҳ
	WB_DOCUMENT_LASTPAGE		//�ĵ����һҳ
};

enum WB_TYPE//�汾
{
	WB_SINGLERVER = 1,		//�����汾
	WB_MULTIVER				//����汾
};

enum RES_INPUT_METHOD
{
	NORMAL = 0,
	DOUBLE_CLK,//˫������Դ
	DRAG_AND_DROP//��ק��Դ
};
typedef struct tagMediaCamaraU7Data
{
	unsigned long nAudioID; //��ƵID��
	unsigned long nVideoID; //��ƵID��
	char strNodeID[MAX_PATH]; //�ڵ�ID
	char strNATIP[MAX_PATH];  //netID
	unsigned short nNATPort; // ����˿�
	char strLocalIP[MAX_PATH]; //����IP��ַ
	unsigned short nLocalPort; //���ض˿�
	char strMCUID[MAX_PATH];    //MCU
	char strMCUIP[MAX_PATH];   //MCUIP
	unsigned short nServerPort; //�������˿�
	char m_strUCCUerID[MAX_PATH];  //��Ƶӵ����ID
	bool bIsCaptureCarema;       //������(true)�򱻶���false���������ͷ

}MediaCamaraU7Data;


//WBAPI �װ�API��������
WB_API BOOL WB_ShowWhiteBoardDlg(void *pParent, IHPNetNotify* pDataNotify, const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);

//I7���ðװ�ӿ�
WB_API BOOL WB_ShowWhiteBoardDlgI7(HWND hWnd, IHPNetNotify* pDataNotify, RECT whiteboardRect, UINT nFlags);

//�ı䴰��λ��
WB_API void WB_MoveBorad(CRect rect, UINT nFlags);

//�ı�װ崰��λ��
WB_API void WB_MoveBorad(const CWnd* pWndInsertAfter, CRect rect, UINT nFlags);
//�������ݷ��ʹ���ָ��
WB_API void WB_SetDataNotify(void *pDataNotify);
//ȫ��
WB_API void WB_SetFullScreen(BOOL bFullScreen = TRUE);
//��ԭ
WB_API void WB_SetNormalScreen( );
//��������״̬�����а���ֱ�ߡ����ߡ������͵�
WB_API void WB_SetDrawStatus(DRAW_STATE nDrawStatus);

//�������ͣ�����ʵ�ߡ����ߡ��㻮�ߵ�PS_SOLID��PS_DOT;��ǰϵͳ��Ĭ������ΪPS_SOLIDʵ��
WB_API void WB_SetWBLineType(int nWBLineType);

//�����ߺ�ͼ�ε��߿�Ĭ��ֵΪ4
WB_API void WB_SetWBCurObjWidth(int nWBCurObjWidth);

//������ð
WB_API void WB_SetLineCap(int nStartLineCap,int nEndLineCap);

//���ö���͸����g_nTranspar
WB_API void WB_SetWBTranspar(float nTranspar);


//����
WB_API void WB_SetCharfont();

WB_API void WB_Combinefile();		//�ϲ��ĵ�


//ˮ������Ч����
WB_API void WB_SetWaterStatus();

//������Ƥ���ߴ�
WB_API void WB_SetBrushSize(int nBrushSize);
//��ͼ���½�һҳ
WB_API void WB_AddNewPage();
//��ͼ�в���һҳ
WB_API void WB_InsertNewPage();
WB_API void WB_InsertNewPage(UINT nPageIndex);
//��ͼ�е�ɾ��һҳ
WB_API void WB_DeletePage();
WB_API void WB_DeletePage(UINT nPageIndex);
//����ҳ
WB_API void WB_InterchangePage(UINT nFormerPage, UINT nTargetPage);
//��ͼ�е���һҳ
WB_API void WB_PrePage();
//��ͼ�е���һҳ
WB_API void WB_NextPage();
//��ͼ�еĵ�һҳ
WB_API void WB_FirstPage();
//��ͼ�е����ҳ
WB_API void WB_LastPage();
// ͼ����һ��
WB_API void WB_SuperLayer();
// ͼ����һ��
WB_API void WB_UnderLayer();
// ͼ��ײ�
WB_API void WB_BottomLayer();
// ͼ�㶥��
WB_API void WB_TopLayer();
// ���ͼƬ
WB_API void WB_AddPicture();
WB_API BOOL WB_AddPicture(const char *pFilePathW,CRect rect = CRect(0,0,0,0));
// �������Ƶ��jiabangping����ʱ�ã�
WB_API void WB_AddMedia();
WB_API BOOL WB_AddMedia(const char *pFilePathW);
WB_API void WB_AddCamera();
// �����ĵ�
WB_API void WB_AddDoc(CRect rect = CRect(0, 0, 0, 0));
WB_API BOOL WB_AddDoc(const char *pFilePathW, CRect rect = CRect(0, 0, 0, 0));
// ɾ��
WB_API void WB_DeleteSelectedObjects();
// ������Դ��
WB_API void WB_OpenResource();
//����flash
WB_API void WB_AddFlash();
WB_API BOOL WB_AddFlash(const char *pFilePathW,CRect rect = CRect(0,0,0,0));
//����dxf
WB_API BOOL WB_AddDxf(const char *pFilePath,CRect rect = CRect(0,0,0,0));
//����
WB_API void WB_SaveWBFile();
WB_API void WB_SaveWBFile(const char* pFilePathW);
//�����ͼƬ
WB_API void WB_SaveWBFileToPicture();
WB_API void WB_SaveWBFileToPicture(const char* pFilePath);
//���
WB_API void WB_Resavefile();
WB_API void WB_Resavefile(const char* pFilePathW);
//��ӡ
WB_API void WB_Printfile();
//���ļ�
WB_API void WB_Openfile();//�޲���Ĭ���ǻᵯ���Ƿ񱣴��ļ��Ի����Լ����ļ�·���Ի���
WB_API void WB_Openfile(const char* pFilePathW); 
//���ɫ�����͡��߿���ͷѡ��
//WB_API void WB_OpenAttributeDlg();
//�½��ļ�
WB_API void WB_Newfile();//�޲���Ĭ���ǻᵯ���Ƿ񱣴��ļ��Ի���
WB_API void WB_Newfile(bool bSaveFile);
//�ر��ļ�
WB_API void WB_Closefile();//�޲���Ĭ���ǻᵯ���Ƿ񱣴��ļ��Ի���
WB_API void WB_Closefile(bool bSaveFile);
//�Ƿ���ʾȫ����ť 
WB_API void WB_ShowFullScreenBtn(bool show = true);
//������������رհװ�
WB_API BOOL WB_SingleCloseFile();
//�˳�������װ�
WB_API BOOL WB_SingleExit();

//�رչ������
WB_API BOOL WB_CloseFuntion();

//����
WB_API void WB_Cutscreen();
// �˳��װ�Ի���
WB_API void WB_Close();
WB_API void WB_Undo(int );
WB_API void WB_Redo();

//����ͬ�����յ�����
WB_API void WB_ProcessDataIn(const char *pData, int nLen);

//�������ж�������
WB_API void WB_ReqAllObj();
//�������ж�������
WB_API void WB_SendAllObj();

 //ɾ�����ж��󣬵��ļ������Ǳ��ز����ǣ��ͷ���ɾ�����ж����ͬ����Ϣ
WB_API void WB_CloseLine();

//��⵱ǰӲ���Ƿ�֧�ֶ�㴥������(������avconָ����Ӳ���������ļ�⣩
WB_API bool WB_BHardwareSupportMTTouch();

//�Ƿ���Ȩ��������
WB_API bool WB_SetTouchAuthorization(bool bAuthorization);

//���յ���Ȩ���
WB_API bool WB_GetTouchAuthorization();

//��Ļ¼��
WB_API void WB_RECScreen(void );

//����
WB_API void WB_Baffle(void );
//ֱ��
WB_API void WB_Ruler(void );
//���ǰ�
WB_API void WB_SetSquare(void);
WB_API void WB_Protractor(void);//������
//Բ��
WB_API void WB_Compass(void);
//�������
WB_API void WB_FunctionPanel();

//���ô����ʱ�ļ���Ŀ¼
WB_API void WB_SetWBTempDir(const char *pWBTempDir);

//��ȡ�����ʱ�ļ���Ŀ¼
WB_API CString WB_GetWBTempDir();
//PreTranslateMessage��Ϣ
WB_API BOOL  DllPreTranslateMessage(MSG *pMsg);

//�޸�Ĭ��������ɫ
WB_API void WB_SetDefaultLineColoe(int nRed, int nGreen, int nBlue);

//�����Ƿ������ɫ
WB_API void WB_SetIsSolid(bool isSolid);

//����ͼ�������ɫ
WB_API void WB_SetSolidColor(int nRed, int nGreen, int nBlue);

//���ðװ屳����ɫ
WB_API void WB_SetWBPageColor(int nRed, int nGreen, int nBlue);

WB_API void WB_DeletePage();

WB_API void WB_InsertNewPage();

//������Ļ��ͼ
WB_API void WB_DrawScreen(const CRect* rect);
//ȫ���л�
WB_API void WB_FullScreenShift(bool isFullScreen);
//����͸��Ч���л�
WB_API void WB_GuiseShift(bool isTransparent);
//�ر���Ļ��ͼ���ڣ�Ĭ�Ϸ���ͬ����Ϣ��bIsSendΪfalseʱ������
WB_API void WB_DestroyWnd(bool bIsSend = TRUE);
//��ѯ��Ļ��ͼ�����Ƿ�͸����͸���Ļ�����true���򷵻�false
WB_API bool WB_IsScreenDrawTrans();
//��ѯ�Ƿ�ȫ��
WB_API bool WB_IsFullScreen();
//�򿪿�����
WB_API void WB_OnCtrlDlg();
//�װ�������Ϊ���ʱ���Խ��չ㲥
WB_API void WB_ShowBroadCast(bool bShow);
//��㴰�ڴ���С���ָ�ʱ���Ƿ�Ҫ�򿪰װ��ע����
WB_API void WB_OpenSCDraw();
//�װ��Ƿ���С���ˣ���С����ΪTRUE������ΪFALSE
WB_API void WB_SetIsMinSize(BOOL bIsMinSize);
//��ǰ�����Ƿ���ȫ��
WB_API void WB_IsWBFullScreen(bool isFullScreen);
//�۹��
WB_API void WB_NewSpotlight();
//�Ŵ�
WB_API void WB_Maqnifier();
//������
WB_API void WB_Dustbin();
//ӥ��
WB_API void WB_EagleEye();
WB_API void WB_EagleEyeSetposAndShow(CRect rect);
//���ñ���AVCON�û�ID
WB_API void WB_SetLocalUserID(const char *pUserID);
//���ñ����û����ƣ��㲥����ʱ���ã�
WB_API void WB_SetLocalUserName(const char* str);
//AVCON�û�����֪ͨ
WB_API void WB_MemberOffline(const char *pUserID);

//����Ƕ���ת����ʱ��Ϊ����˳ʱ��Ϊ��
WB_API void WB_RotateByAngle(int angle);

//����ˮƽ��ֱ��ת��ˮƽΪ0�ȣ���ֱΪ90��
WB_API void WB_RotateByHV(int HVAngle);

//��¡
WB_API void WB_CloneObject();
//���޿�¡
 WB_API void  WB_InfiniteCloneObj();
//����
WB_API void WB_CutObject();
//����
WB_API void WB_CopyObject();
//ճ��
WB_API void WB_PasteObject();
//ɾ��ȫ��
WB_API void WB_DeleteAll();
//����վ
WB_API void WB_Dustbin();
//����DICOM�ӿ�
WB_API void WB_AddDICOM();
//����DICOM�ӿ�,��·��
WB_API BOOL WB_AddDICOM(const char *pFilePathW,CRect rect = CRect(0,0,0,0));

//����
WB_API void WB_AboutAvconWB();
//��AVCON�ṩ����ʱ·��
WB_API void WB_SetFilePathsByavcon(const char *pLanguageFile,const char *pIniPath,const char *pTempPath);
//��AVCON�����Ƿ������װ幦��
WB_API void WB_AvconLock(bool bLock);
//��AVCON���ÿ�ʼ���������¼��
WB_API BOOL WB_StartConferenceRecord(bool bStart);
//���û���¼��ָ��
WB_API void WB_SetConferenceRecordNotify(IHPWBNotify *pConferenceRecordNotify);

//���ðװ�İ汾����
WB_API void WB_SetTypeOfWB(WB_TYPE typeOfWB);
//�ر������ӹ�����
WB_API void WB_CloseAllChildBox();
WB_API void WB_MediaSingelToNet();//��Ƶ���絽��������������Ƶɾ��
WB_API void WB_SetMediaSinToIntnet( BOOL bMedia );
//���ô��U7��Ĭ��·��Ŀ¼
WB_API void WB_SetU7FileDir(const char * lpszU7FileDir);

//���洫��ҳ�ű���װ��о���ҳλͼ����������
WB_API BOOL WB_SaveWBPageToBitmap(UINT PageIndex,HBITMAP hBitmap);
WB_API UINT WB_GetCurPageCount();
//������½�ɹ����
WB_API void WB_IsSuccessLoad(bool bLoadSuccess);
//�����Ƿ�Ϊ����ʹ�õ��Ӱװ����, ע�⣺�����ж�Ӳ����Ȩ֮ǰ����һ�Σ�֮��Ҫ
WB_API void WB_SetIsLoacalWhiteboard(bool isLoacalWhiteboard);
//���õ�¼��������ʱ��,�������ж�Ӳ����Ȩ֮ǰ
WB_API void WB_SetServerTime(const char *pLoadServerTime);

//��U7���õ���Ƶ����ͷ���ݴ�����ק�ӿ�
WB_API void WB_SetDropNotifyMediaCamara(MediaCamaraU7Data *pMediaCamaraU7Data);
//����U7����ק�װ崰�ں�ı����λ��
WB_API void WB_ChangCourTranDlgPosition(int x,int y); 

//��ȡ��ǰѡ�аװ��������
WB_API int WB_GetSelectWhiteBoardObjectCount();

//��ȡ�װ��Ƿ��и��ƻ���ж���
WB_API bool WB_GetIsCutOrCopyObject();

//��ȡѡ�ж������Ƿ�����϶���
WB_API bool WB_GetIsCombinObject();

//��ȡ��ǰ���ͼ���
WB_API int WB_GetPageMaxLayer();
//��ȡ��ǰ��Сͼ���
WB_API int WB_GetPageMinLayer();
//��ȡ��ǰ����ͼ���
WB_API int WB_GetPageObjCurrentLayer();
//��Ϲ��ܣ�����϶���֮ǰ�豣֤��������ѡ��״̬
WB_API void WB_SetCombineObj();

//ȡ����Ϲ��ܣ���ȡ����϶���֮ǰ�豣֤��ȡ������϶�������ѡ��״̬
WB_API void WB_SetUnCombineObj();

//��ȡ�װ�Ŀ��
WB_API int WB_GetWhiteboardWidth();
//��ȡ�װ�� �߶�
WB_API int WB_GetWhiteboardHeight();
//�õ��������ڵ�ǰ��ͼ�е���ʼ��λ��
WB_API POINT WB_GetScrollPosition();
//���ù������ڵ�ǰ��ͼ�е���ʼ��λ��
WB_API void WB_SetScrollPosition(POINT scrollPoint);
//�������������װ��й�����λ��
WB_API void WB_SendScrollPosToNet();
//�жϵ��������Ƿ�������ֻ��Ե���������Ч���������ʱ������
WB_API bool WB_GetSingleObjIsLock();
//��ȡ�װ�������
WB_API int WB_GetObjCount();
//ˢ�°װ�
WB_API void WB_Refresh();
//�����첽��Ϣ���װ�
WB_API void WB_PostMessageToWhiteBoard(UINT msg, WPARAM wparam, LPARAM lparam);

#endif
