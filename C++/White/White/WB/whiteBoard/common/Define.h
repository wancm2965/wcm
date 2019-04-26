#if !defined(AFX_DEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_)
#define AFX_DEFINE_H__A1525342_B768_4B7D_A3A1_0046D17848A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PI 3.1415926535897932384626433832795

/*extern BRUSH_SIZE g_nBrushSize; //橡皮擦尺寸*/

extern DRAW_STATE g_nDrawStatus;
extern BOOL g_bIsMouseInput;
//上一次获得的路径
extern CString g_strCurFilePath;
extern DWORD g_systemVersion;

//白板界面中的宏定义
#define WB_CTRLBTNDLG_HEIGHT														72
#define WB_CTRLBTNDLG_WIDTH														72
#define WB_CTRLBTN_WIDTH																50
#define WB_WIDTH_SPACE																	9
#define WB_TOOLBARBTN_SPACE														4   //按钮之间间隔
#define WB_TOOLBARBTN_WIDTH														50  //工具条和工具箱中按钮的宽度
#define WB_TOOLBARBTN_HEIGHT														50  //工具条和工具箱中按钮的高度
#define WB_TOOLBOXBTN_SPEC_WIDTH												45 //工具箱中特殊按钮的宽度
#define WB_TOOLBOXBTN_SPEC_HEIGHT											34  //工具箱中特殊按钮的高度
#define WB_TOOLBARHIDESHOW_SPEED											4  //工具条的伸缩速度
#define WB_RESOURCEMANAGEBTN_WIDTH										90 //资源管理对话框中按钮的宽度

/*#define	WB_FUNCTION_DLG_WIDTH												199	//控制面板对话框的宽度*/
#define	WB_FUNCTION_DLG_WIDTH													215	//控制面板对话框的宽度
#define WB_OPTIONDLG_WIDTH															29	//选项卡面板对话框的宽度

#define WB_FILLPAD_WIDTH																150	//颜色填充面板宽度
#define WB_FILLPAD_HEIGHT																242	//颜色填充面板高度
#define WB_FILLPAD_BTNWIDTH															34	//颜色填充面板中的背景控制按钮宽度
#define WB_FILLPAD_BTNHEIGHT														34	//颜色填充面板中的背景控制按钮高度
#define WB_FILLPAD_BTN_SPACE														12	//颜色填充面板中的背景控制按钮之间间隔
 
#define WB_VIEWHSCROLL																	2400
#define WB_VIEWVSCROLL																	1600


#define		WB_COLORBTN_SPACE														2 
#define		WB_COLORBTN_WIDTH														30
#define		WB_COLORBTN_HEIGHT														30

#define		MOVEISNEEDSENDDATA_TIME											33 //一般对象在间隔相应时间就发送数据


//消息定义

#define WM_WB_MODIFYLINE																WM_USER+101
#define WM_WB_ENDMODIFYLINE														WM_USER+102
#define WB_DELETEOBJ																		WM_USER+104
#define WB_PROCESSOBJ																	WM_USER+105
#define WB_MAQNOBJ																		WM_USER+107
#define WB_SPOTLIGHTOBJ																	WM_USER+108
#define WB_ADDFLASH																		WM_USER+106 //同步时添加FLASH对象
#define WB_SCREENDRAW																	WM_USER+109 //屏幕绘图同步信息
#define ID_TIMER_FLASH																		1
#define ID_TIMER_DOCCONVER															2

#define WM_SELECTWBOBJ																	WM_USER+110
#define WM_NOSELECTWBOBJ															WM_USER+111
#define WM_SELECTWBPEN																	WM_USER+112	//功能面板中的画笔属性响应
#define WM_SELECTWBERASER                                                              WM_USER+113 //选择擦子属性面板响应
//chenchangtao liushuanglong
#define WM_VEDIOOPEN																	WM_USER+114  //打开视频
#define WM_MEDIACLOSE																	WM_USER+115  //关闭视频或摄像头
#define WM_SHOWVEDIOCTRL															WM_USER+116  //显示控制
#define WM_HIDEVEDIOCTRL																WM_USER+117  //关闭控制
#define WM_MEDIAPAUSE																	WM_USER+118  //暂停视频或摄像头
#define WM_MEDIAPLAY																		WM_USER+119  //播放视频或摄像头
#define WM_MEDIASTOP																		WM_USER+120  //停止播放
#define WM_CAMERASETTING																WM_USER+121  //摄像头设置
#define WM_MEDIASETMUTE																WM_USER+122  //静音
#define WM_MEDIATOBMP																	WM_USER+123  //视频截图
#define WM_MEDIALOOP																	WM_USER+124  //循环播放

#define WM_TEXTARRIBUTE																	WM_USER+125  //文本属性

#define WM_ADDMEDIA																		WM_USER + 126 //同步添加视频对象
#define WM_ADDCAMERA																	WM_USER + 127 //同步添加摄像头对象
#define	WM_WBCAMERA_NET_CONNECT_SYNCHRONOUS				WM_USER + 128//白板摄像头的连接同步

#define WM_WB_FUNCTIONVIEWSYN													WM_USER + 129//预览中的view和白板同步
#define WM_WB_FUNCTION_ADDPREVIEW											WM_USER + 130 //预览中的view添加
#define WM_CLOSE_TOOBAR_MES														WM_USER + 131  //view中操作关闭所有的子工具条 
#define WM_WB_SHOWPAGENUMBER												WM_USER + 132  //在页对话框中显示页码
#define WM_PALETTECOLOR																WM_USER + 133  //调色板颜色和属性面板颜色同步
#define WM_PROCESS_FILE_OPERATE													WM_USER + 134  //相应文件操作同步消息

#define WM_OPENTEACHTOOLDLG                                                 WM_USER + 135 //打开工具面板
#define WM_DEL_ALL_ATTRIBUTE														WM_USER + 136  //删除所有属性
#define WM_NOTICE_PRAVIEW_REFRESH											WM_USER + 137  //通知预览视图刷新
#define WM_DELALLOBJECT																WM_USER + 138  //删除所有对象
#define WM_DESTROYSCREEN																WM_USER + 139 //销毁屏幕绘图窗口的消息
#define WM_EAGLEEYE_SYNCHRONIZER												WM_USER + 140	//鹰眼的刷新消息
#define WM_SHOWOPTIONBTN															WM_USER + 141	//选项卡显示更新
#define WM_OPENBACKGROUNDDLG												WM_USER + 142	//打开背景面板
#define WM_OPENPREVIEWDLG															WM_USER + 143	//打开预览面板
#define WM_OPENRESDLG																	WM_USER + 144	//打开资源面板
#define WM_OPENARRIBUTEDLG															WM_USER + 145	//打开属性面板
#define WM_PRAVIEW_REFRESH															WM_USER +	 146 //预览刷新
#define WM_CLOSEWBFUNCTIONDLG												WM_USER + 147 //关闭功能面板
#define WM_DEL_PAGE_ALL_OBJ															WM_USER + 148 //删除页所有对象
#define WM_CREATE_PRINT_LOADINGMSG										WM_USER + 149 //打开文件加载画面
#define WM_REDROW_PRINT_LOADING_MSG									WM_USER + 150 //刷新文件加载画面
#define WM_SENDPAGEDATATO_CONFERENCE_RECORD					WM_USER + 151  //页的所有对象发送到会议录像中
#define WM_PROCESSBROADCASTOBJ													WM_USER + 153 //处理广播对象的消息（在实时同步中用到）

#define WM_PROCESS_TRANDLG														WM_USER + 155 //处理广播光标的的透明对话框
#define WM_PROCESS_WATERDLG														WM_USER + 156//处理水波纹广播消息
#define WM_CREATE_WATER                                                         WM_USER + 157//创建水波纹消息
#define UWM_CREATE_RICHEDIT															(WM_USER + 158)//创建rich框
#define UWM_EDIT_RICHEDIT															(WM_USER + 159)//编辑richedit文本
#define UWM_DESTROY_RICHEDIT														(WM_USER + 160)//销毁richedit文本
#define WM_POPMENU																WM_USER + 161//弹出菜单消息

#define WM_MEDIA_MOVE_TOUCH                                                          WM_USER + 162//视频窗口移动
#define WM_MEDIA_CAPTURE                                                              WM_USER + 163//视频截图
#define WM_RectMagn_CLOSE                                                             WM_USER + 164//放大镜关闭
#define WM_MEDIA_ZOON_TOUCH                                                          WM_USER + 165//视频窗口缩放
#define WM_MTSENDCURSOR                                                              WM_USER + 166 //触摸移动光标使用
#define WM_CAREMA_MOVE_TOUCH                                                          WM_USER + 167//视频窗口移动


//对象属性KEY定义
#define KEY_OBJ_ANGLE																		KEY_USER + 1 //对象旋转角度
#define KEY_OBJ_ROTATECENTER_X														KEY_USER + 2 //旋转中心点X坐标
#define KEY_OBJ_ROTATECENTER_Y														KEY_USER + 3 //旋转中心点Y坐标
#define KEY_OBJ_ZOOMSCALE_X															KEY_USER + 4 //对象X轴缩放比例
#define KEY_OBJ_ZOOMSCALE_Y															KEY_USER + 5 //对象Y轴缩放比例 
#define KEY_OBJ_ZOOMCENTER_X														KEY_USER + 6 //缩放中心点X坐标
#define KEY_OBJ_ZOOMCENTER_Y														KEY_USER + 7 //缩放中心点Y坐标
#define KEY_LAYER_CHANGETYPE														KEY_USER + 8//改变图层类型	
#define KEY_PAGE_CMDTYPE																KEY_USER +9//白板页操作命令类型	

#define KEY_OBJ_LINEWIDTH																KEY_USER + 10 //对象线宽
#define KEY_OBJ_LINETYPE																	KEY_USER + 11 //对象线型
#define KEY_OBJ_COLOR																		KEY_USER + 12 //对象颜色
#define KEY_OBJ_SOLIDCOLOR															KEY_USER + 13 //对象填充颜色

#define KEY_LINE_BEGIN_X																	KEY_USER + 14 //直线起点X坐标
#define KEY_LINE_BEGIN_Y																	KEY_USER + 15 //直线起点Y坐标
#define KEY_LINE_END_X																		KEY_USER + 16 //直线终点X坐标
#define KEY_LINE_END_Y																		KEY_USER + 17 //直线终点Y坐标
//#define KEY_LINE_ARROWTYPE															KEY_USER + 18 //直线箭头类型
#define KEY_LINE_STARTLINECAP														KEY_USER + 18 //直线起点线帽
#define KEY_LINE_ENDLINECAP															KEY_USER + 19 //直线终点线帽

#define KEY_CURVE_POINTCOUNT														KEY_USER + 20 //曲线点数
#define KEY_CURVE_POINTS																KEY_USER + 21//曲线点

#define KEY_POLYGON_ISSOLID															KEY_USER + 22 //多边形是否填充
#define KEY_POLYGON_SOLIDCOLOR													KEY_USER + 23 //多边形填充颜色
#define KEY_POLYGON_POINTCOUNT												KEY_USER + 24 //多边形点数
#define KEY_POLYGON_POINTS															KEY_USER + 25 //多边形点

#define KEY_ELLIPSE_TOPLEFT_X															KEY_USER + 26 //圆左上点X坐标
#define KEY_ELLIPSE_TOPLEFT_Y															KEY_USER + 27 //圆左上点Y坐标
#define KEY_ELLIPSE_BOTTOMRIGHT_X												KEY_USER + 28 //圆右下点X坐标
#define KEY_ELLIPSE_BOTTOMTIGHT_Y												KEY_USER + 29 //圆右下点Y坐标

#define KEY_TEXT_STRING																	KEY_USER + 30 //文本内容
#define KEY_TEXT_FONT_HEIGHT															KEY_USER + 31 //字体高度
#define KEY_TEXT_FONT_WIDTH															KEY_USER + 32 //字体宽度
#define KEY_TEXT_FONT_ANGLE															KEY_USER + 33 //字体旋转角度
#define KEY_TEXT_FONT_WEIGHT															KEY_USER + 34 //字体磅数
#define KEY_TEXT_FONT_ITALIC															KEY_USER + 35 //字体是否斜体
#define KEY_TEXT_FONT_UNDERLINE													KEY_USER + 36 //字体是否带下划线
#define KEY_TEXT_FONT_CHARSET														KEY_USER + 37 //字体使用的字符集
#define KEY_TEXT_FONT_STRIKEOUT													KEY_USER + 38 //字体字符是否突出
#define KEY_TEXT_FONT_FACENAME													KEY_USER + 39 //字体名称

#define KEY_TEXT_FONT_ESCAPEMENT												KEY_USER + 3900 //字体相对于页面底端的角度
#define KEY_TEXT_FONT_OUTPRECISION												KEY_USER + 3901 //字体输出精度
#define KEY_TEXT_FONT_CLIPPRECISION												KEY_USER + 3902 //字体剪辑精度
#define KEY_TEXT_FONT_QUALITY														KEY_USER + 3903 //字体输出质量
#define KEY_TEXT_FONT_PITCHANDFAMILY										KEY_USER + 3904 //字体的字符间距和族
#define KEY_TEXT_FONT_COLOR															KEY_USER + 40 //字体颜色
#define KEY_TEXT_VERTICAL																	KEY_USER + 41 //文本竖排

#define KEY_OBJ_RECT_LEFT																	KEY_USER + 42 //对象矩形位置的左边
#define KEY_OBJ_RECT_TOP																	KEY_USER + 43 //对象矩形位置的上边
#define KEY_OBJ_RECT_RIGHT																KEY_USER + 44 //对象矩形位置的右边
#define KEY_OBJ_RECT_BOTTOM															KEY_USER + 45 //对象矩形位置的下边

//#define KEY_LAYER_CHANGETYPE														KEY_USER + 46//改变图层类型	

#define KEY_OBJ_EQUALSCALEZOOM													KEY_USER + 47 //对象缩放属性

#define KEY_FULLSCREEN																	KEY_USER + 48 //全屏属性

#define KEY_OBJ_INFINITECLONE														KEY_USER + 49 //对象无限克隆属性

#define KEY_OBJ_COMBINEMEMBER													KEY_USER + 50 //对象组合属性
#define KEY_COMBINE_OBJCOUNT														KEY_USER + 51 //组合对象数
#define KEY_COMBINE_OBJLAYER														KEY_USER + 52 //组合对象图层号
#define KEY_COMBINE_SELECTCOUNT												KEY_USER + 53 //组合对象选择框顶点数
#define KEY_COMBINE_SELECTPONITS												KEY_USER + 54 //组合对象选择框顶点坐标
#define KEY_COMBINE_COMOBJLAYER												KEY_USER + 55 //组合里对象所在组合对象的图层号

#define KEY_PAGEINDEX																		KEY_USER + 56 //翻页属性
#define KEY_MEDIA_OPERATION															KEY_USER + 57//对媒体的操作，包括视频、FLASH、文档等。
#define KEY_MOVIE_CURRENTTIME														KEY_USER + 58 //视频当前时间
#define KEY_DOCUMENT_CURPAGE													KEY_USER + 59 //文档当前页码
#define KEY_PAGE_SCROLL_X																KEY_USER + 60  //页面的滚动条位置
#define KEY_PAGE_SCROLL_Y																KEY_USER + 61
#define KEY_CURVE_STARTLINECAP														KEY_USER + 62
#define KEY_CURVE_ENDLINECAP														KEY_USER + 63
#define KEY_MEDIA_VOLUME																KEY_USER + 64 //当前音量的大小
#define KEY_MEDIA_SETMUTE																KEY_USER + 65 //是否静音
#define KEY_MEDIA_PLAY																	KEY_USER + 66 //新视频播放
#define KEY_MEDIA_VOICE																	KEY_USER + 67 //新视频音量
#define KEY_MEDIA_CLOSE																	KEY_USER + 68 //新视频关闭
#define KEY_MEDIA_STOP																	KEY_USER + 69 //新视频停止
#define KEY_MEDIA_CTRL_SHOWSTATE												KEY_USER + 70 //新视频控制对话框状态
#define KEY_MEDIA_LOOP																	KEY_USER + 71 //新视频循环播放
#define KEY_MEDIA_FRAMECAPTURE													KEY_USER + 72 //新视频截图
#define KEY_MEDIA_CTRL_SHOWDROW												KEY_USER + 73//新视频控制框是否显示（画图）
#define KEY_MEDIA_FILE_PATH															KEY_USER + 74
#define  KEY_MEDIA_BITRATE_BOOL													KEY_USER + 75   //视频码流控制
#define  KEY_MEDIA_ADDSESSION														KEY_USER + 76   //视频添加者
#define  KEY_MEDIACAMARA_CHANNELID														KEY_USER + 77   //
#define  KEY_MEDIACAMARA_USERID														KEY_USER + 78   //
#define  KEY_MOVE_EAGEL_EYE														KEY_USER + 79   //


#define KEY_CAMERA_SERVER_INFO													(KEY_USER + 80) //摄像头设置的MCU信息
#define KEY_CAMERA_LOCAL_INFO														(KEY_USER + 81) //摄像头设置的本地连接信息（创建方）
#define KEY_CAMERA_REMOTE_INFO													(KEY_USER + 82) //摄像头设置的同步连接信息（非创建方）

#define KEY_ANNOTATIONS_ISCHANGE												KEY_USER + 90 //标注的尖角是否改变过
#define KEY_ANNOTATIONS_POINT_X													KEY_USER + 91
//尖角的点的坐标
#define KEY_ANNOTATIONS_POINT_Y													KEY_USER + 100
#define KEY_ANNOTATIONS_DIRECTION												KEY_USER + 101 //尖角的方向
#define  KEY_CAREMA_ADDSESSION													KEY_USER + 102   //视频添加者

#define KEY_OBJ_ALPHA																		KEY_USER + 110 //对象透明度
#define KEY_OBJ_ALPHAINCOM															KEY_USER + 111 //对象组合时的透明度

#define KEY_MQAN_RECT_LEFT															KEY_USER + 115//放大镜矩形位置的左边
#define KEY_MQAN_RECT_TOP															KEY_USER + 116 //放大镜矩形位置的上边
#define KEY_MQAN_RECT_RIGHT															KEY_USER + 117 //放大镜矩形位置的右边
#define KEY_MQAN_RECT_BOTTOM														KEY_USER + 118 //放大镜矩形位置的下边
#define KEY_MAQN_ZOOMBTN_POS													KEY_USER + 119 //放大镜滑动块位置
#define KEY_MAQN_STATE																	KEY_USER + 120 //放大镜状态
#define KEY_SPOTLIGHT_ROUND_LEFT												KEY_USER + 121 //聚光灯圆形的左边
#define KEY_SPOTLIGHT_ROUND_TOP													KEY_USER + 122 //聚光灯圆形的上边
#define KEY_SPOTLIGHT_ROUND_RIGHT												KEY_USER + 123 //聚光灯圆形的右边
#define KEY_SPOTLIGHT_ROUND_BOTTOM											KEY_USER + 124 //聚光灯圆形的下边
//#define KEY_SPOTLIGHT_STATE															KEY_USER + 125 //聚光灯状态

#define KEY_OBJ_LAYER_NUM																KEY_USER + 129  //图层 号  add by huanglongquan
#define KEY_OBJ_LAYER_TYPE																KEY_USER + 130 //对象图层类型

#define KEY_SCREENDRAW																	KEY_USER + 131 //屏幕绘图
#define KEY_SCREENDRAW_FULLSCREEN											KEY_USER + 132 //屏幕绘图全屏效果
#define KEY_SCREENDRAW_GUISE														KEY_USER + 133 //屏幕绘图玻璃效果
#define KEY_SCREENDRAW_ALPHA														KEY_USER + 134 //屏幕绘图透明度

#define KEY_OBJ_LIMIT																		KEY_USER + 137//正在操作的对象，互动方限制其操作该对象 
#define KEY_EVENT_OBJNUMBER															KEY_USER + 138 //同事件中同步的对象个数
#define KEY_EVENT_DOCSELECTOBJ														KEY_USER + 139 //同步事件中文档或视频截图中选中的对象的图层号

//摄像头的连接数据（包含本地的，和异地的）
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
//此摄像头的归属用户(UCC登录的用户名)
#define KEY_CAMERA_ID_OF_UCC_LOGIN_USER									(KEY_USER + 157)
#define KEY_CAMERA_ID_OF_UCC_LOGIN_NAME									(KEY_USER + 158)

#define KEY_DEL_COUNT																		KEY_USER + 159
#define KEY_EVENT_OPENFILE																KEY_USER + 160//同步事件中打开文件操作
#define KEY_OBJ_BKG																			KEY_USER + 161 //对象背景属性
#define	KEY_DEL_ALL_OBJECT																KEY_USER + 162 //删除所有对象
/*#define KEY_CHANGE_OLD_PAGE														KEY_USER + 163 //要交换的页（交换页操作）*/
//CWBComponent
#define KEY_COMPONENT_INI															KEY_USER + 164 //对象的子组件
#define	KEY_COMPONENT_COUNT														KEY_USER + 165 //子组件个数
#define	KEY_COMPONENT_PAGE                                                        KEY_USER + 166 //文档页号
#define KEY_COMPONENT_FIRST															KEY_USER + 167 //第一个子组件，
	//若需要添加新的键值，添加在该键值之上。

// #define  KEY_CURVE_SELWIDTH														KEY_USER + 167
// #define KEY_CURVE_SELHEIGHT														KEY_USER + 168

#define KEY_PICTURE_WIDTH																KEY_USER + 169 //图片在文件夹中的实际大小
#define KEY_PICTURE_HEIGHT																KEY_USER + 170
#define KEY_PICTURE_STRING                                                              KEY_USER + 171 //图片中的字
#define KEY_PICTURE_SENDSTING                                                           KEY_USER + 172 //判断字
#define KEY_PICTURE_CHANGNEL_ID                                                         KEY_USER + 173 //摄像头拖拽图片，摄像头通道
#define KEY_PICTURE_ISPRINTSCREEN                                                       KEY_USER + 174 //是否是视频截图

#define	 KEY_FILEPROC_MARCH															KEY_USER + 179	//文件处理进展
#define KEY_BEGIN_EVENT																	KEY_USER + 180  //事件开始
#define KEY_END_EVENT																		KEY_USER + 181	//事件结束


//录播数据
#define KEY_CONFERENCE_RECORD													KEY_USER + 186//录播数据

//广播数据命令类型
#define KEY_BROADCAST_CMD															KEY_USER + 210 //广播数据类型命令（比如：水波纹、用户光标等命令）
//鼠标移动的时候发送同步key值
#define KEY_ADDENDUM_CURVE														KEY_USER + 211	//判断曲线是否追加点
#define KEY_MOVE_OFFSET																	KEY_USER + 212 //对象移动偏移量
#define KEY_DRAWING_CURVE                                                       KEY_USER + 213 //曲线正在画过程不让擦除
//广播光标操作key值
#define KEY_BROADCASTUSERCURSOR												KEY_USER + 215 //广播本地用户光标类型
#define KEY_STRAVCONUSERID															KEY_USER + 216 //本地用户名
#define KEY_CURSORPOINT																	KEY_USER + 217//本地光标的点
//广播点名
#define KEY_BROADCASTUSERSTYLE                                                  KEY_USER + 218 //
#define KEY_BROADCASTSENDROLLCALLUSER                                           KEY_USER + 219 //
#define KEY_BROADCASTROLLCALLOKUSERNAME                                         KEY_USER + 185 
#define KEY_ERASE_ADD_CURVE_NUM                                         KEY_USER + 311//判断大小擦，擦曲线
#define KEY_ERASE_RECT_LEFT                                             KEY_USER + 312 //擦除框矩形
#define KEY_ERASE_RECT_RIGHT                                            KEY_USER + 313 //擦除框矩形
#define KEY_ERASE_RECT_TOP                                              KEY_USER + 314 //擦除框矩形
#define KEY_ERASE_RECT_BOTTON                                           KEY_USER + 315 //擦除框矩形
#define KEY_CAMARA_RECT_BOOL                                             KEY_USER + 316 //u7拖拽摄像头位置bool


//沙盘特效key值 
#define  KEY_WATER_EVENT																	KEY_USER + 220  //水波纹事件
#define  KEY_WATER_USER																	KEY_USER + 221 //水波纹触发者
#define  KEY_WATER_POINT																	KEY_USER + 222 //水波纹点位置
#define  KEY_BROADCASTUSERWATER                                                             KEY_USER + 223 

//DXF定义
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

#define KEY_END																					KEY_USER + 300 //关键字结束		
//本地操作命令不是key值
#define CMD_CHANGE_LAYER																KEY_END + 1//层改变
#define CMD_MEDIA_OPERATION														KEY_END + 2//对媒体的操作
#define CMD_CONFERENCE_RECORDDATA											KEY_END + 3//接收到的数据是录播数据
#define CMD_MEDIA_AUDIO_ID															KEY_END + 4//录播接收视频音频数据
#define CMD_MEDIA_USER_LINEOUT													KEY_END + 5//视频主动下线，删除服务器视频，自己不接收

//触摸移动时候发送广播消息不发送CHANGOBJ
#define  CMD_TOUCH_MOVE_OBJ														KEY_END + 6//触摸移动时命令

#define  CMD_CAMARA_DROP_OBJ													KEY_END + 7//摄像头
#define  CMD_OWEN_SEND_OBJ													KEY_END + 8//

#define KEY_WHITEBOARD_WIDTH																	KEY_USER + 317 //白板VIEW的宽度
#define KEY_WHITEBOARD_HEIGHT																	KEY_USER + 318 //白板VIEW的高度
#define KEY_CAMERA_UNDO_REDO													    KEY_USER + 319 //摄像头撤销修改通道

#define KEY_TRIANGLETYPE                                                          KEY_USER + 320 //三角形类型

#define KEY_PAGE_SCROLL_XY																KEY_USER + 321  //页面的滚动条位置
#define KEY_PAGE_SCROLL_YX																KEY_USER + 322
#define KEY_OWEN_SEND_OBJ														  KEY_USER + 323 //u7拖拽摄像头位置bool
#define KEY_DRAWRORATTION_CENTERX                                               KEY_USER + 324 //画任意中心点的值的X
#define KEY_DRAWRORATTION_CENTERY                                               KEY_USER + 325 //画任意中心点的值的Y
#define KEY_PREROTATIONCENTERPOINTX                                             KEY_USER + 326 //上次的旋转中心点X值
#define KEY_PREROTATIONCENTERPOINTY                                             KEY_USER + 327 //上次的旋转中心点Y值

#define KEY_ICOCURVE_FILEPATH                                                       KEY_USER + 328 //曲线图标路径
#define KEY_ICOCURVE_PRESCALEWIDTH                                           KEY_USER + 329 //曲线图标最开始的宽度
#define KEY_SENDFILEMARK                                                               KEY_USER + 330 //曲线图标从文件加载发送标记
#define KEY_ICOCURVE_ISLOADFROMFILE                                         KEY_USER + 331 //判断当前图标是白板的or从文件加载的

//圆弧
#define  KEY_ARC_PTCT_X                                                      KEY_USER + 332 //圆弧中心点X
#define  KEY_ARC_PTCT_Y                                                       KEY_USER + 333//圆弧中心点Y
#define  KEY_ARC_PTSELECTLT_X												KEY_USER + 334   //圆弧虚线框LTX
#define  KEY_ARC_PTSELECTLT_Y                                                  KEY_USER + 335 //圆弧虚线框LTY
#define  KEY_ARC_PTSELECTRB_X													KEY_USER + 336 //圆弧虚线框RBX
#define  KEY_ARC_PTSELECTRB_Y													KEY_USER + 337 //圆弧虚线框RBY
#define  KEY_ARC_PTDRAWSTART_X													KEY_USER + 338 //圆弧起点X
#define  KEY_ARC_PTDRAWSTART_Y													KEY_USER + 339 //圆弧起点Y
#define  KEY_ARC_PTDRAWEND_X													KEY_USER + 340 //圆弧终点X
#define  KEY_ARC_PTDRAWEND_Y													KEY_USER + 341 //圆弧终点Y
#define  KEY_ARC_STARTANGLE														KEY_USER + 342 //起始角度
#define  KEY_ARC_SWEEPANGLE														KEY_USER + 343 //扫过角度
#define  KEY_ARC_RADIUS															KEY_USER + 344 //圆弧半径

//圆弧、扇形的外切矩形
#define  KEY_RECTF_LEFT                          								KEY_USER + 345//外切矩形LTX
#define  KEY_RECTF_TOP                          								KEY_USER + 346//外切矩形LTY
#define  KEY_RECTF_WIDTH                          								KEY_USER + 347//外切矩形宽
#define  KEY_RECTF_HEIGHT                          								KEY_USER + 348//外切矩形高
//扇形
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
//#define  KEY_TOUCH_UP																KEY_USER + 362//判断是否为触摸弹起时的操作

#define  KEY_MIDIVIEW_CHANGE                                                        KEY_USER + 362
#define  KEY_MIDIVIEW_SCALE                                                         KEY_USER + 363



//防特殊处理的
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
#define MK_WB_ULBUTTON																	0x0400//不会与系统重复，而且目前系统只用了前面几个标识位

//跟KEY_BROADCAST_CMD关联
typedef enum _eBroadcastCmd			 //广播数据命令
{
	_E_ROADCASTCMD_NONE = 0,
	_E_OBJDRAW_MOVE,						//画对象时实时广播
	_E_WATER_SPECIAL_EFFICIENCY,		//水波纹特效
	_E_CURSOR_MOVE,							//广播用户光标移动
	_E_OBJ_MOVE,							//	移动对象实时同步
	_E_PROCESS_FILE,						//打开文件操作 或者文件广播
	_E_BROADCASTCMD_USER,				//用户自定义
	_E_BROADE_ERASE_LINE,                //大小擦除线时时同步广播点数据
	_E_BROAD_ERASE_UP,                     //判断是擦除UP操作
	_E_RELEASE_LIMITOBJS,					//发送广播让互动方取消限制
	_E_BROAD_UNDO_REDO_MEDIA,                //undo,redo撤销视频操作，修改视频音频的ID
	_E_BROAD_MEDIA_CAMARA_OPEN,     
	_E_BROAD_ROLL_CALL ,                        //广播点名
	_E_BROAD_UNDO_REDO_CAMERA,				 //undo,redo撤销视频操作，修改摄像头
	_E_BROAD_UNDO_REDO_SPOLITE,					//聚关灯
	_E_BOARD_CAMERA_NAME,                    //拖拽摄像头名
}eBroadcastCmd; 

typedef enum _eBroadcast_Move_Cmd			//广播移动命令
{
	_E_CURSOR_MOVE_Cmd_NONE = 0,
	_E_MOVE_TRANARENT_CURSOR,			//移动透明广播及提示的用户名
	_E_CLOSE_TRANARENT_CURSOR,			//关闭广播光标命令
	_E_MOVE_EAGEL_EYE,                  //移动到鹰眼标记
}eBroadcast_Move_Cmd;

typedef enum _eBroadcast_Water_Cmd			//广播水波纹
{
	_E_WATER_MOVE_Cmd_NONE = 0,
	_E_SHOWWATER,			//显示水波纹
	_E_CLOSEWATER,			//关闭水波纹
}eBroadcast_Water_Cmd;

typedef enum _eDrawingObj
{
	_E_DRAWING_NONE = 0,
	_E_DRAWING_ADDENDUMCURVE,
	_E_DRAWING_CURVE,
	_E_DRAWING_OBJEND,
}eDrawingObj;
#endif
