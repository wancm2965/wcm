#if !defined(AFX_DEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_DEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI 3.1415926535897932384626433832795

/*extern BRUSH_SIZE g_nBrushSize; //��Ƥ���ߴ�*/

extern DRAW_STATE g_nDrawStatus;
extern BOOL g_bIsMouseInput;
//��һ�λ�õ�·��
extern CString g_strCurFilePath;
extern DWORD g_systemVersion;

//�װ�����еĺ궨��
#define WB_CTRLBTNDLG_HEIGHT														72
#define WB_CTRLBTNDLG_WIDTH														72
#define WB_CTRLBTN_WIDTH																50
#define WB_WIDTH_SPACE																	9
#define WB_TOOLBARBTN_SPACE														4   //��ť֮����
#define WB_TOOLBARBTN_WIDTH														50  //�������͹������а�ť�Ŀ��
#define WB_TOOLBARBTN_HEIGHT														50  //�������͹������а�ť�ĸ߶�
#define WB_TOOLBOXBTN_SPEC_WIDTH												45 //�����������ⰴť�Ŀ��
#define WB_TOOLBOXBTN_SPEC_HEIGHT											34  //�����������ⰴť�ĸ߶�
#define WB_TOOLBARHIDESHOW_SPEED											4  //�������������ٶ�
#define WB_RESOURCEMANAGEBTN_WIDTH										90 //��Դ����Ի����а�ť�Ŀ��

/*#define	WB_FUNCTION_DLG_WIDTH												199	//�������Ի���Ŀ��*/
#define	WB_FUNCTION_DLG_WIDTH													215	//�������Ի���Ŀ��
#define WB_OPTIONDLG_WIDTH															29	//ѡ����Ի���Ŀ��

#define WB_FILLPAD_WIDTH																150	//��ɫ��������
#define WB_FILLPAD_HEIGHT																242	//��ɫ������߶�
#define WB_FILLPAD_BTNWIDTH															34	//��ɫ�������еı������ư�ť���
#define WB_FILLPAD_BTNHEIGHT														34	//��ɫ�������еı������ư�ť�߶�
#define WB_FILLPAD_BTN_SPACE														12	//��ɫ�������еı������ư�ť֮����
 
#define WB_VIEWHSCROLL																	2400
#define WB_VIEWVSCROLL																	1600


#define		WB_COLORBTN_SPACE														2 
#define		WB_COLORBTN_WIDTH														30
#define		WB_COLORBTN_HEIGHT														30

#define		MOVEISNEEDSENDDATA_TIME											33 //һ������ڼ����Ӧʱ��ͷ�������


//��Ϣ����

#define WM_WB_MODIFYLINE																WM_USER+101
#define WM_WB_ENDMODIFYLINE														WM_USER+102
#define WB_DELETEOBJ																		WM_USER+104
#define WB_PROCESSOBJ																	WM_USER+105
#define WB_MAQNOBJ																		WM_USER+107
#define WB_SPOTLIGHTOBJ																	WM_USER+108
#define WB_ADDFLASH																		WM_USER+106 //ͬ��ʱ���FLASH����
#define WB_SCREENDRAW																	WM_USER+109 //��Ļ��ͼͬ����Ϣ
#define ID_TIMER_FLASH																		1
#define ID_TIMER_DOCCONVER															2

#define WM_SELECTWBOBJ																	WM_USER+110
#define WM_NOSELECTWBOBJ															WM_USER+111
#define WM_SELECTWBPEN																	WM_USER+112	//��������еĻ���������Ӧ
#define WM_SELECTWBERASER                                                              WM_USER+113 //ѡ��������������Ӧ
//chenchangtao liushuanglong
#define WM_VEDIOOPEN																	WM_USER+114  //����Ƶ
#define WM_MEDIACLOSE																	WM_USER+115  //�ر���Ƶ������ͷ
#define WM_SHOWVEDIOCTRL															WM_USER+116  //��ʾ����
#define WM_HIDEVEDIOCTRL																WM_USER+117  //�رտ���
#define WM_MEDIAPAUSE																	WM_USER+118  //��ͣ��Ƶ������ͷ
#define WM_MEDIAPLAY																		WM_USER+119  //������Ƶ������ͷ
#define WM_MEDIASTOP																		WM_USER+120  //ֹͣ����
#define WM_CAMERASETTING																WM_USER+121  //����ͷ����
#define WM_MEDIASETMUTE																WM_USER+122  //����
#define WM_MEDIATOBMP																	WM_USER+123  //��Ƶ��ͼ
#define WM_MEDIALOOP																	WM_USER+124  //ѭ������

#define WM_TEXTARRIBUTE																	WM_USER+125  //�ı�����

#define WM_ADDMEDIA																		WM_USER + 126 //ͬ�������Ƶ����
#define WM_ADDCAMERA																	WM_USER + 127 //ͬ���������ͷ����
#define	WM_WBCAMERA_NET_CONNECT_SYNCHRONOUS				WM_USER + 128//�װ�����ͷ������ͬ��

#define WM_WB_FUNCTIONVIEWSYN													WM_USER + 129//Ԥ���е�view�Ͱװ�ͬ��
#define WM_WB_FUNCTION_ADDPREVIEW											WM_USER + 130 //Ԥ���е�view���
#define WM_CLOSE_TOOBAR_MES														WM_USER + 131  //view�в����ر����е��ӹ����� 
#define WM_WB_SHOWPAGENUMBER												WM_USER + 132  //��ҳ�Ի�������ʾҳ��
#define WM_PALETTECOLOR																WM_USER + 133  //��ɫ����ɫ�����������ɫͬ��
#define WM_PROCESS_FILE_OPERATE													WM_USER + 134  //��Ӧ�ļ�����ͬ����Ϣ

#define WM_OPENTEACHTOOLDLG                                                 WM_USER + 135 //�򿪹������
#define WM_DEL_ALL_ATTRIBUTE														WM_USER + 136  //ɾ����������
#define WM_NOTICE_PRAVIEW_REFRESH											WM_USER + 137  //֪ͨԤ����ͼˢ��
#define WM_DELALLOBJECT																WM_USER + 138  //ɾ�����ж���
#define WM_DESTROYSCREEN																WM_USER + 139 //������Ļ��ͼ���ڵ���Ϣ
#define WM_EAGLEEYE_SYNCHRONIZER												WM_USER + 140	//ӥ�۵�ˢ����Ϣ
#define WM_SHOWOPTIONBTN															WM_USER + 141	//ѡ���ʾ����
#define WM_OPENBACKGROUNDDLG												WM_USER + 142	//�򿪱������
#define WM_OPENPREVIEWDLG															WM_USER + 143	//��Ԥ�����
#define WM_OPENRESDLG																	WM_USER + 144	//����Դ���
#define WM_OPENARRIBUTEDLG															WM_USER + 145	//���������
#define WM_PRAVIEW_REFRESH															WM_USER +	 146 //Ԥ��ˢ��
#define WM_CLOSEWBFUNCTIONDLG												WM_USER + 147 //�رչ������
#define WM_DEL_PAGE_ALL_OBJ															WM_USER + 148 //ɾ��ҳ���ж���
#define WM_CREATE_PRINT_LOADINGMSG										WM_USER + 149 //���ļ����ػ���
#define WM_REDROW_PRINT_LOADING_MSG									WM_USER + 150 //ˢ���ļ����ػ���
#define WM_SENDPAGEDATATO_CONFERENCE_RECORD					WM_USER + 151  //ҳ�����ж����͵�����¼����
#define WM_PROCESSBROADCASTOBJ													WM_USER + 153 //����㲥�������Ϣ����ʵʱͬ�����õ���

#define WM_PROCESS_TRANDLG														WM_USER + 155 //����㲥���ĵ�͸���Ի���
#define WM_PROCESS_WATERDLG														WM_USER + 156//����ˮ���ƹ㲥��Ϣ
#define WM_CREATE_WATER                                                         WM_USER + 157//����ˮ������Ϣ
#define UWM_CREATE_RICHEDIT															(WM_USER + 158)//����rich��
#define UWM_EDIT_RICHEDIT															(WM_USER + 159)//�༭richedit�ı�
#define UWM_DESTROY_RICHEDIT														(WM_USER + 160)//����richedit�ı�
#define WM_POPMENU																WM_USER + 161//�����˵���Ϣ

#define WM_MEDIA_MOVE_TOUCH                                                          WM_USER + 162//��Ƶ�����ƶ�
#define WM_MEDIA_CAPTURE                                                              WM_USER + 163//��Ƶ��ͼ
#define WM_RectMagn_CLOSE                                                             WM_USER + 164//�Ŵ󾵹ر�
#define WM_MEDIA_ZOON_TOUCH                                                          WM_USER + 165//��Ƶ��������
#define WM_MTSENDCURSOR                                                              WM_USER + 166 //�����ƶ����ʹ��
#define WM_CAREMA_MOVE_TOUCH                                                          WM_USER + 167//��Ƶ�����ƶ�


//��������KEY����
#define KEY_OBJ_ANGLE																		KEY_USER + 1 //������ת�Ƕ�
#define KEY_OBJ_ROTATECENTER_X														KEY_USER + 2 //��ת���ĵ�X����
#define KEY_OBJ_ROTATECENTER_Y														KEY_USER + 3 //��ת���ĵ�Y����
#define KEY_OBJ_ZOOMSCALE_X															KEY_USER + 4 //����X�����ű���
#define KEY_OBJ_ZOOMSCALE_Y															KEY_USER + 5 //����Y�����ű��� 
#define KEY_OBJ_ZOOMCENTER_X														KEY_USER + 6 //�������ĵ�X����
#define KEY_OBJ_ZOOMCENTER_Y														KEY_USER + 7 //�������ĵ�Y����
#define KEY_LAYER_CHANGETYPE														KEY_USER + 8//�ı�ͼ������	
#define KEY_PAGE_CMDTYPE																KEY_USER +9//�װ�ҳ������������	

#define KEY_OBJ_LINEWIDTH																KEY_USER + 10 //�����߿�
#define KEY_OBJ_LINETYPE																	KEY_USER + 11 //��������
#define KEY_OBJ_COLOR																		KEY_USER + 12 //������ɫ
#define KEY_OBJ_SOLIDCOLOR															KEY_USER + 13 //���������ɫ

#define KEY_LINE_BEGIN_X																	KEY_USER + 14 //ֱ�����X����
#define KEY_LINE_BEGIN_Y																	KEY_USER + 15 //ֱ�����Y����
#define KEY_LINE_END_X																		KEY_USER + 16 //ֱ���յ�X����
#define KEY_LINE_END_Y																		KEY_USER + 17 //ֱ���յ�Y����
//#define KEY_LINE_ARROWTYPE															KEY_USER + 18 //ֱ�߼�ͷ����
#define KEY_LINE_STARTLINECAP														KEY_USER + 18 //ֱ�������ñ
#define KEY_LINE_ENDLINECAP															KEY_USER + 19 //ֱ���յ���ñ

#define KEY_CURVE_POINTCOUNT														KEY_USER + 20 //���ߵ���
#define KEY_CURVE_POINTS																KEY_USER + 21//���ߵ�

#define KEY_POLYGON_ISSOLID															KEY_USER + 22 //������Ƿ����
#define KEY_POLYGON_SOLIDCOLOR													KEY_USER + 23 //����������ɫ
#define KEY_POLYGON_POINTCOUNT												KEY_USER + 24 //����ε���
#define KEY_POLYGON_POINTS															KEY_USER + 25 //����ε�

#define KEY_ELLIPSE_TOPLEFT_X															KEY_USER + 26 //Բ���ϵ�X����
#define KEY_ELLIPSE_TOPLEFT_Y															KEY_USER + 27 //Բ���ϵ�Y����
#define KEY_ELLIPSE_BOTTOMRIGHT_X												KEY_USER + 28 //Բ���µ�X����
#define KEY_ELLIPSE_BOTTOMTIGHT_Y												KEY_USER + 29 //Բ���µ�Y����

#define KEY_TEXT_STRING																	KEY_USER + 30 //�ı�����
#define KEY_TEXT_FONT_HEIGHT															KEY_USER + 31 //����߶�
#define KEY_TEXT_FONT_WIDTH															KEY_USER + 32 //������
#define KEY_TEXT_FONT_ANGLE															KEY_USER + 33 //������ת�Ƕ�
#define KEY_TEXT_FONT_WEIGHT															KEY_USER + 34 //�������
#define KEY_TEXT_FONT_ITALIC															KEY_USER + 35 //�����Ƿ�б��
#define KEY_TEXT_FONT_UNDERLINE													KEY_USER + 36 //�����Ƿ���»���
#define KEY_TEXT_FONT_CHARSET														KEY_USER + 37 //����ʹ�õ��ַ���
#define KEY_TEXT_FONT_STRIKEOUT													KEY_USER + 38 //�����ַ��Ƿ�ͻ��
#define KEY_TEXT_FONT_FACENAME													KEY_USER + 39 //��������

#define KEY_TEXT_FONT_ESCAPEMENT												KEY_USER + 3900 //���������ҳ��׶˵ĽǶ�
#define KEY_TEXT_FONT_OUTPRECISION												KEY_USER + 3901 //�����������
#define KEY_TEXT_FONT_CLIPPRECISION												KEY_USER + 3902 //�����������
#define KEY_TEXT_FONT_QUALITY														KEY_USER + 3903 //�����������
#define KEY_TEXT_FONT_PITCHANDFAMILY										KEY_USER + 3904 //������ַ�������
#define KEY_TEXT_FONT_COLOR															KEY_USER + 40 //������ɫ
#define KEY_TEXT_VERTICAL																	KEY_USER + 41 //�ı�����

#define KEY_OBJ_RECT_LEFT																	KEY_USER + 42 //�������λ�õ����
#define KEY_OBJ_RECT_TOP																	KEY_USER + 43 //�������λ�õ��ϱ�
#define KEY_OBJ_RECT_RIGHT																KEY_USER + 44 //�������λ�õ��ұ�
#define KEY_OBJ_RECT_BOTTOM															KEY_USER + 45 //�������λ�õ��±�

//#define KEY_LAYER_CHANGETYPE														KEY_USER + 46//�ı�ͼ������	

#define KEY_OBJ_EQUALSCALEZOOM													KEY_USER + 47 //������������

#define KEY_FULLSCREEN																	KEY_USER + 48 //ȫ������

#define KEY_OBJ_INFINITECLONE														KEY_USER + 49 //�������޿�¡����

#define KEY_OBJ_COMBINEMEMBER													KEY_USER + 50 //�����������
#define KEY_COMBINE_OBJCOUNT														KEY_USER + 51 //��϶�����
#define KEY_COMBINE_OBJLAYER														KEY_USER + 52 //��϶���ͼ���
#define KEY_COMBINE_SELECTCOUNT												KEY_USER + 53 //��϶���ѡ��򶥵���
#define KEY_COMBINE_SELECTPONITS												KEY_USER + 54 //��϶���ѡ��򶥵�����
#define KEY_COMBINE_COMOBJLAYER												KEY_USER + 55 //��������������϶����ͼ���

#define KEY_PAGEINDEX																		KEY_USER + 56 //��ҳ����
#define KEY_MEDIA_OPERATION															KEY_USER + 57//��ý��Ĳ�����������Ƶ��FLASH���ĵ��ȡ�
#define KEY_MOVIE_CURRENTTIME														KEY_USER + 58 //��Ƶ��ǰʱ��
#define KEY_DOCUMENT_CURPAGE													KEY_USER + 59 //�ĵ���ǰҳ��
#define KEY_PAGE_SCROLL_X																KEY_USER + 60  //ҳ��Ĺ�����λ��
#define KEY_PAGE_SCROLL_Y																KEY_USER + 61
#define KEY_CURVE_STARTLINECAP														KEY_USER + 62
#define KEY_CURVE_ENDLINECAP														KEY_USER + 63
#define KEY_MEDIA_VOLUME																KEY_USER + 64 //��ǰ�����Ĵ�С
#define KEY_MEDIA_SETMUTE																KEY_USER + 65 //�Ƿ���
#define KEY_MEDIA_PLAY																	KEY_USER + 66 //����Ƶ����
#define KEY_MEDIA_VOICE																	KEY_USER + 67 //����Ƶ����
#define KEY_MEDIA_CLOSE																	KEY_USER + 68 //����Ƶ�ر�
#define KEY_MEDIA_STOP																	KEY_USER + 69 //����Ƶֹͣ
#define KEY_MEDIA_CTRL_SHOWSTATE												KEY_USER + 70 //����Ƶ���ƶԻ���״̬
#define KEY_MEDIA_LOOP																	KEY_USER + 71 //����Ƶѭ������
#define KEY_MEDIA_FRAMECAPTURE													KEY_USER + 72 //����Ƶ��ͼ
#define KEY_MEDIA_CTRL_SHOWDROW												KEY_USER + 73//����Ƶ���ƿ��Ƿ���ʾ����ͼ��
#define KEY_MEDIA_FILE_PATH															KEY_USER + 74
#define  KEY_MEDIA_BITRATE_BOOL													KEY_USER + 75   //��Ƶ��������
#define  KEY_MEDIA_ADDSESSION														KEY_USER + 76   //��Ƶ�����
#define  KEY_MEDIACAMARA_CHANNELID														KEY_USER + 77   //
#define  KEY_MEDIACAMARA_USERID														KEY_USER + 78   //
#define  KEY_MOVE_EAGEL_EYE														KEY_USER + 79   //


#define KEY_CAMERA_SERVER_INFO													(KEY_USER + 80) //����ͷ���õ�MCU��Ϣ
#define KEY_CAMERA_LOCAL_INFO														(KEY_USER + 81) //����ͷ���õı���������Ϣ����������
#define KEY_CAMERA_REMOTE_INFO													(KEY_USER + 82) //����ͷ���õ�ͬ��������Ϣ���Ǵ�������

#define KEY_ANNOTATIONS_ISCHANGE												KEY_USER + 90 //��ע�ļ���Ƿ�ı��
#define KEY_ANNOTATIONS_POINT_X													KEY_USER + 91
//��ǵĵ������
#define KEY_ANNOTATIONS_POINT_Y													KEY_USER + 100
#define KEY_ANNOTATIONS_DIRECTION												KEY_USER + 101 //��ǵķ���
#define  KEY_CAREMA_ADDSESSION													KEY_USER + 102   //��Ƶ�����

#define KEY_OBJ_ALPHA																		KEY_USER + 110 //����͸����
#define KEY_OBJ_ALPHAINCOM															KEY_USER + 111 //�������ʱ��͸����

#define KEY_MQAN_RECT_LEFT															KEY_USER + 115//�Ŵ󾵾���λ�õ����
#define KEY_MQAN_RECT_TOP															KEY_USER + 116 //�Ŵ󾵾���λ�õ��ϱ�
#define KEY_MQAN_RECT_RIGHT															KEY_USER + 117 //�Ŵ󾵾���λ�õ��ұ�
#define KEY_MQAN_RECT_BOTTOM														KEY_USER + 118 //�Ŵ󾵾���λ�õ��±�
#define KEY_MAQN_ZOOMBTN_POS													KEY_USER + 119 //�Ŵ󾵻�����λ��
#define KEY_MAQN_STATE																	KEY_USER + 120 //�Ŵ�״̬
#define KEY_SPOTLIGHT_ROUND_LEFT												KEY_USER + 121 //�۹��Բ�ε����
#define KEY_SPOTLIGHT_ROUND_TOP													KEY_USER + 122 //�۹��Բ�ε��ϱ�
#define KEY_SPOTLIGHT_ROUND_RIGHT												KEY_USER + 123 //�۹��Բ�ε��ұ�
#define KEY_SPOTLIGHT_ROUND_BOTTOM											KEY_USER + 124 //�۹��Բ�ε��±�
//#define KEY_SPOTLIGHT_STATE															KEY_USER + 125 //�۹��״̬

#define KEY_OBJ_LAYER_NUM																KEY_USER + 129  //ͼ�� ��  add by huanglongquan
#define KEY_OBJ_LAYER_TYPE																KEY_USER + 130 //����ͼ������

#define KEY_SCREENDRAW																	KEY_USER + 131 //��Ļ��ͼ
#define KEY_SCREENDRAW_FULLSCREEN											KEY_USER + 132 //��Ļ��ͼȫ��Ч��
#define KEY_SCREENDRAW_GUISE														KEY_USER + 133 //��Ļ��ͼ����Ч��
#define KEY_SCREENDRAW_ALPHA														KEY_USER + 134 //��Ļ��ͼ͸����

#define KEY_OBJ_LIMIT																		KEY_USER + 137//���ڲ����Ķ��󣬻���������������ö��� 
#define KEY_EVENT_OBJNUMBER															KEY_USER + 138 //ͬ�¼���ͬ���Ķ������
#define KEY_EVENT_DOCSELECTOBJ														KEY_USER + 139 //ͬ���¼����ĵ�����Ƶ��ͼ��ѡ�еĶ����ͼ���

//����ͷ���������ݣ��������صģ�����صģ�
#define KEY_OBJ_SYNCHRONOUS_TYPE												KEY_USER + 140 
#define KEY_CAMERA_REMOTE_INFO_STRING_NODEID						KEY_USER + 141
#define KEY_CAMERA_REMOTE_INFO_STRING_NATIP							KEY_USER + 142 
#define KEY_CAMERA_REMOTE_INFO_INTEGER_NATPORT					KEY_USER + 143
#define KEY_CAMERA_REMOTE_INFO_STRING_LOCALIP						KEY_USER + 144
#define KEY_CAMERA_REMOTE_INFO_INTEGER_LOCALPORT				KEY_USER + 145
#define KEY_CAMERA_REMOTE_INFO_STRING_MCUID						KEY_USER + 146
#define KEY_CAMERA_REMOTE_INFO_STRING_MCUIP						KEY_USER + 147 
#define KEY_CAMERA_REMOTE_INFO_INTERGER_SERVERPORT			KEY_USER + 148

#define KEY_CAMERA_LOCAL_INFO_INTEGER_AUDIOID						KEY_USER + 149
#define KEY_CAMERA_LOCAL_INFO_INTEGER_DEVID							KEY_USER + 150
#define KEY_CAMERA_LOCAL_INFO_INTEGER_VIDEO_CODECTYPE		KEY_USER + 151
#define KEY_CAMERA_LOCAL_INFO_INTERGER_FRAMERATE				KEY_USER + 152
#define KEY_CAMERA_LOCAL_INFO_INTERGER_BITITRATE					KEY_USER + 153
#define KEY_CAMERA_LOCAL_INFO_INTERGER_WIDTH						KEY_USER + 154
#define KEY_CAMERA_LOCAL_INFO_INTERGER_HEIGHT						KEY_USER + 155
#define KEY_CAMERA_LOCAL_INFO_INTERGER_AUDIO_CODEID			KEY_USER + 156
//������ͷ�Ĺ����û�(UCC��¼���û���)
#define KEY_CAMERA_ID_OF_UCC_LOGIN_USER									(KEY_USER + 157)
#define KEY_CAMERA_ID_OF_UCC_LOGIN_NAME									(KEY_USER + 158)

#define KEY_DEL_COUNT																		KEY_USER + 159
#define KEY_EVENT_OPENFILE																KEY_USER + 160//ͬ���¼��д��ļ�����
#define KEY_OBJ_BKG																			KEY_USER + 161 //���󱳾�����
#define	KEY_DEL_ALL_OBJECT																KEY_USER + 162 //ɾ�����ж���
/*#define KEY_CHANGE_OLD_PAGE														KEY_USER + 163 //Ҫ������ҳ������ҳ������*/
//CWBComponent
#define KEY_COMPONENT_INI															KEY_USER + 164 //����������
#define	KEY_COMPONENT_COUNT														KEY_USER + 165 //���������
#define	KEY_COMPONENT_PAGE                                                        KEY_USER + 166 //�ĵ�ҳ��
#define KEY_COMPONENT_FIRST															KEY_USER + 167 //��һ���������
	//����Ҫ����µļ�ֵ������ڸü�ֵ֮�ϡ�

// #define  KEY_CURVE_SELWIDTH														KEY_USER + 167
// #define KEY_CURVE_SELHEIGHT														KEY_USER + 168

#define KEY_PICTURE_WIDTH																KEY_USER + 169 //ͼƬ���ļ����е�ʵ�ʴ�С
#define KEY_PICTURE_HEIGHT																KEY_USER + 170
#define KEY_PICTURE_STRING                                                              KEY_USER + 171 //ͼƬ�е���
#define KEY_PICTURE_SENDSTING                                                           KEY_USER + 172 //�ж���
#define KEY_PICTURE_CHANGNEL_ID                                                         KEY_USER + 173 //����ͷ��קͼƬ������ͷͨ��
#define KEY_PICTURE_ISPRINTSCREEN                                                       KEY_USER + 174 //�Ƿ�����Ƶ��ͼ

#define	 KEY_FILEPROC_MARCH															KEY_USER + 179	//�ļ������չ
#define KEY_BEGIN_EVENT																	KEY_USER + 180  //�¼���ʼ
#define KEY_END_EVENT																		KEY_USER + 181	//�¼�����


//¼������
#define KEY_CONFERENCE_RECORD													KEY_USER + 186//¼������

//�㲥������������
#define KEY_BROADCAST_CMD															KEY_USER + 210 //�㲥��������������磺ˮ���ơ��û��������
//����ƶ���ʱ����ͬ��keyֵ
#define KEY_ADDENDUM_CURVE														KEY_USER + 211	//�ж������Ƿ�׷�ӵ�
#define KEY_MOVE_OFFSET																	KEY_USER + 212 //�����ƶ�ƫ����
#define KEY_DRAWING_CURVE                                                       KEY_USER + 213 //�������ڻ����̲��ò���
//�㲥������keyֵ
#define KEY_BROADCASTUSERCURSOR												KEY_USER + 215 //�㲥�����û��������
#define KEY_STRAVCONUSERID															KEY_USER + 216 //�����û���
#define KEY_CURSORPOINT																	KEY_USER + 217//���ع��ĵ�
//�㲥����
#define KEY_BROADCASTUSERSTYLE                                                  KEY_USER + 218 //
#define KEY_BROADCASTSENDROLLCALLUSER                                           KEY_USER + 219 //
#define KEY_BROADCASTROLLCALLOKUSERNAME                                         KEY_USER + 185 
#define KEY_ERASE_ADD_CURVE_NUM                                         KEY_USER + 311//�жϴ�С����������
#define KEY_ERASE_RECT_LEFT                                             KEY_USER + 312 //���������
#define KEY_ERASE_RECT_RIGHT                                            KEY_USER + 313 //���������
#define KEY_ERASE_RECT_TOP                                              KEY_USER + 314 //���������
#define KEY_ERASE_RECT_BOTTON                                           KEY_USER + 315 //���������
#define KEY_CAMARA_RECT_BOOL                                             KEY_USER + 316 //u7��ק����ͷλ��bool


//ɳ����Чkeyֵ 
#define  KEY_WATER_EVENT																	KEY_USER + 220  //ˮ�����¼�
#define  KEY_WATER_USER																	KEY_USER + 221 //ˮ���ƴ�����
#define  KEY_WATER_POINT																	KEY_USER + 222 //ˮ���Ƶ�λ��
#define  KEY_BROADCASTUSERWATER                                                             KEY_USER + 223 

//DXF����
#define KEY_DXF_ZOOMLEVEL				                                                KEY_USER + 224
#define KEY_DXF_VIEWBOTTOM				                                            KEY_USER + 225
#define KEY_DXF_VIEWLEFT				                                                    KEY_USER + 226
#define KEY_DXF_VIEWTOP					                                                KEY_USER + 227
#define KEY_DXF_VIEWRIGHT				                                                KEY_USER + 228
#define KEY_DXF_BK						                                                        KEY_USER + 229
#define KEY_DXF_WINDOWLEFT                                                          KEY_USER + 230
#define KEY_DXF_WINDOWRIGHT                                                       KEY_USER + 231
#define KEY_DXF_WINDOWTOP                                                           KEY_USER + 232
#define KEY_DXF_WINDOWBOTTOM                                                   KEY_USER + 233
#define KEY_DXF_VIEWPPU                                                                  KEY_USER + 234    

#define KEY_END																					KEY_USER + 300 //�ؼ��ֽ���		
//���ز��������keyֵ
#define CMD_CHANGE_LAYER																KEY_END + 1//��ı�
#define CMD_MEDIA_OPERATION														KEY_END + 2//��ý��Ĳ���
#define CMD_CONFERENCE_RECORDDATA											KEY_END + 3//���յ���������¼������
#define CMD_MEDIA_AUDIO_ID															KEY_END + 4//¼��������Ƶ��Ƶ����
#define CMD_MEDIA_USER_LINEOUT													KEY_END + 5//��Ƶ�������ߣ�ɾ����������Ƶ���Լ�������

//�����ƶ�ʱ���͹㲥��Ϣ������CHANGOBJ
#define  CMD_TOUCH_MOVE_OBJ														KEY_END + 6//�����ƶ�ʱ����

#define  CMD_CAMARA_DROP_OBJ													KEY_END + 7//����ͷ
#define  CMD_OWEN_SEND_OBJ													KEY_END + 8//

#define KEY_WHITEBOARD_WIDTH																	KEY_USER + 317 //�װ�VIEW�Ŀ��
#define KEY_WHITEBOARD_HEIGHT																	KEY_USER + 318 //�װ�VIEW�ĸ߶�
#define KEY_CAMERA_UNDO_REDO													    KEY_USER + 319 //����ͷ�����޸�ͨ��

#define KEY_TRIANGLETYPE                                                          KEY_USER + 320 //����������

#define KEY_PAGE_SCROLL_XY																KEY_USER + 321  //ҳ��Ĺ�����λ��
#define KEY_PAGE_SCROLL_YX																KEY_USER + 322
#define KEY_OWEN_SEND_OBJ														  KEY_USER + 323 //u7��ק����ͷλ��bool
#define KEY_DRAWRORATTION_CENTERX                                               KEY_USER + 324 //���������ĵ��ֵ��X
#define KEY_DRAWRORATTION_CENTERY                                               KEY_USER + 325 //���������ĵ��ֵ��Y
#define KEY_PREROTATIONCENTERPOINTX                                             KEY_USER + 326 //�ϴε���ת���ĵ�Xֵ
#define KEY_PREROTATIONCENTERPOINTY                                             KEY_USER + 327 //�ϴε���ת���ĵ�Yֵ

#define KEY_ICOCURVE_FILEPATH                                                       KEY_USER + 328 //����ͼ��·��
#define KEY_ICOCURVE_PRESCALEWIDTH                                           KEY_USER + 329 //����ͼ���ʼ�Ŀ��
#define KEY_SENDFILEMARK                                                               KEY_USER + 330 //����ͼ����ļ����ط��ͱ��
#define KEY_ICOCURVE_ISLOADFROMFILE                                         KEY_USER + 331 //�жϵ�ǰͼ���ǰװ��or���ļ����ص�

//Բ��
#define  KEY_ARC_PTCT_X                                                      KEY_USER + 332 //Բ�����ĵ�X
#define  KEY_ARC_PTCT_Y                                                       KEY_USER + 333//Բ�����ĵ�Y
#define  KEY_ARC_PTSELECTLT_X												KEY_USER + 334   //Բ�����߿�LTX
#define  KEY_ARC_PTSELECTLT_Y                                                  KEY_USER + 335 //Բ�����߿�LTY
#define  KEY_ARC_PTSELECTRB_X													KEY_USER + 336 //Բ�����߿�RBX
#define  KEY_ARC_PTSELECTRB_Y													KEY_USER + 337 //Բ�����߿�RBY
#define  KEY_ARC_PTDRAWSTART_X													KEY_USER + 338 //Բ�����X
#define  KEY_ARC_PTDRAWSTART_Y													KEY_USER + 339 //Բ�����Y
#define  KEY_ARC_PTDRAWEND_X													KEY_USER + 340 //Բ���յ�X
#define  KEY_ARC_PTDRAWEND_Y													KEY_USER + 341 //Բ���յ�Y
#define  KEY_ARC_STARTANGLE														KEY_USER + 342 //��ʼ�Ƕ�
#define  KEY_ARC_SWEEPANGLE														KEY_USER + 343 //ɨ���Ƕ�
#define  KEY_ARC_RADIUS															KEY_USER + 344 //Բ���뾶

//Բ�������ε����о���
#define  KEY_RECTF_LEFT                          								KEY_USER + 345//���о���LTX
#define  KEY_RECTF_TOP                          								KEY_USER + 346//���о���LTY
#define  KEY_RECTF_WIDTH                          								KEY_USER + 347//���о��ο�
#define  KEY_RECTF_HEIGHT                          								KEY_USER + 348//���о��θ�
//����
#define  KEY_SECTOR_PTCT_X                                                      KEY_USER + 349  //
#define  KEY_SECTOR_PTCT_Y                                                       KEY_USER + 350
#define  KEY_SECTOR_PTSELECTLT_X												KEY_USER + 351
#define  KEY_SECTOR_PTSELECTLT_Y                                                  KEY_USER + 352
#define  KEY_SECTOR_PTSELECTRB_X													KEY_USER + 353
#define  KEY_SECTOR_PTSELECTRB_Y													KEY_USER + 354
#define  KEY_SECTOR_PTDRAWSTART_X													KEY_USER + 355
#define  KEY_SECTOR_PTDRAWSTART_Y													KEY_USER + 356
#define  KEY_SECTOR_PTDRAWEND_X													KEY_USER + 357
#define  KEY_SECTOR_PTDRAWEND_Y													KEY_USER + 358
#define  KEY_SECTOR_STARTANGLE														KEY_USER + 359
#define  KEY_SECTOR_SWEEPANGLE														KEY_USER + 360
#define  KEY_SECTOR_RADIUS															KEY_USER + 361
//#define  KEY_TOUCH_UP																KEY_USER + 362//�ж��Ƿ�Ϊ��������ʱ�Ĳ���

#define  KEY_MIDIVIEW_CHANGE                                                        KEY_USER + 362
#define  KEY_MIDIVIEW_SCALE                                                         KEY_USER + 363



//�����⴦���
/*
 * Key State Masks for Mouse Messages
 #define MK_LBUTTON																			0x0001
 #define MK_RBUTTON																			0x0002
 #define MK_SHIFT																				0x0004
 #define MK_CONTROL																		0x0008
 #define MK_MBUTTON																		0x0010
 #if(_WIN32_WINNT >= 0x0500)
 #define MK_XBUTTON1																		0x0020
 #define MK_XBUTTON2																		0x0040
 */
/*
void OnLButtonDown(UINT nFlags, CPoint point)
void OnLButtonUp(UINT nFlags, CPoint point)
void OnMouseMove(UINT nFlags, CPoint point)
*/
#define MK_WB_ULBUTTON																	0x0400//������ϵͳ�ظ�������Ŀǰϵͳֻ����ǰ�漸����ʶλ

//��KEY_BROADCAST_CMD����
typedef enum _eBroadcastCmd			 //�㲥��������
{
	_E_ROADCASTCMD_NONE = 0,
	_E_OBJDRAW_MOVE,						//������ʱʵʱ�㲥
	_E_WATER_SPECIAL_EFFICIENCY,		//ˮ������Ч
	_E_CURSOR_MOVE,							//�㲥�û�����ƶ�
	_E_OBJ_MOVE,							//	�ƶ�����ʵʱͬ��
	_E_PROCESS_FILE,						//���ļ����� �����ļ��㲥
	_E_BROADCASTCMD_USER,				//�û��Զ���
	_E_BROADE_ERASE_LINE,                //��С������ʱʱͬ���㲥������
	_E_BROAD_ERASE_UP,                     //�ж��ǲ���UP����
	_E_RELEASE_LIMITOBJS,					//���͹㲥�û�����ȡ������
	_E_BROAD_UNDO_REDO_MEDIA,                //undo,redo������Ƶ�������޸���Ƶ��Ƶ��ID
	_E_BROAD_MEDIA_CAMARA_OPEN,     
	_E_BROAD_ROLL_CALL ,                        //�㲥����
	_E_BROAD_UNDO_REDO_CAMERA,				 //undo,redo������Ƶ�������޸�����ͷ
	_E_BROAD_UNDO_REDO_SPOLITE,					//�۹ص�
	_E_BOARD_CAMERA_NAME,                    //��ק����ͷ��
}eBroadcastCmd; 

typedef enum _eBroadcast_Move_Cmd			//�㲥�ƶ�����
{
	_E_CURSOR_MOVE_Cmd_NONE = 0,
	_E_MOVE_TRANARENT_CURSOR,			//�ƶ�͸���㲥����ʾ���û���
	_E_CLOSE_TRANARENT_CURSOR,			//�رչ㲥�������
	_E_MOVE_EAGEL_EYE,                  //�ƶ���ӥ�۱��
}eBroadcast_Move_Cmd;

typedef enum _eBroadcast_Water_Cmd			//�㲥ˮ����
{
	_E_WATER_MOVE_Cmd_NONE = 0,
	_E_SHOWWATER,			//��ʾˮ����
	_E_CLOSEWATER,			//�ر�ˮ����
}eBroadcast_Water_Cmd;

typedef enum _eDrawingObj
{
	_E_DRAWING_NONE = 0,
	_E_DRAWING_ADDENDUMCURVE,
	_E_DRAWING_CURVE,
	_E_DRAWING_OBJEND,
}eDrawingObj;
#endif
