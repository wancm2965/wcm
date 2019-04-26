#pragma once
//#include <string>
#include <map>
using namespace std;
#define CHANGE_BUT_TYPE 1
typedef enum _eType
{   
	TYPE_NONE = 0,//
	TYPE_BACKGROUND,    //按钮背景
	TYPE_CHILDBG,       //子按钮背景
	TYPE_MEDIOVIDEOSCREEN,//屏幕录制按钮的背景
	TYPE_RECYCLE,       //回收站
	TYPE_DICOM,			//医疗DICOM
	TYPE_SELECT,		//选择
	TYPE_PAGE,			//页
	TYPE_FULLSCREEN,	//全屏
	TYPE_BROADCAST,     //广播
	TYPE_BIGHT,			//曲线
	TYPE_LINE,			//直线
	TYPE_RUBBER_BIG,	//大橡皮
	TYPE_RUBBER_OBJ,	//对象橡皮
	TYPE_RUBBER_CURVE,	//曲线橡皮
	TYPE_IMAGE,			//插入图片
	TYPE_VIDEO,			//插入视频
	TYPE_DOCUMENT,		//插入文档
	TYPE_FLASH,			//插入flash
	TYPE_CAMERAHEAD,    //摄像头
	TYPE_ALLTOOL,		//工具箱
	TYPE_RESOURCE,		//显示资源回廊
	TYPE_DEL,			//删除
	TYPE_PRE_PAGE,		//上一页
	TYPE_NEXT_PAGE,		//下一页
	TYPE_T_H,			//水平文本
	TYPE_T_V,			//垂直文本
	TYPE_REDO,			//上一步
	TYPE_UNDO,			//下一步
	TYPE_CANTOUCH,      //触摸授权
	TYPE_NOTOUCH,       //触摸未授权
	TYPE_TRIANGLE,		//三角形
	TYPE_RECTANGLE,		//矩形
	TYPE_ROUNDNESS,     //圆形
	TYPE_RHOMB,			//菱形
	TYPE_POLYGON,       //多边形
	TYPE_SCREENSHOTS,	//截屏
	TYPE_MAQN,	        //放大镜
	TYPE_SPOTLIGHT,     //聚光灯
	TYPE_BAFFLEPLATE,   //挡板
	TYPE_PROTRACTOR,    //量角器
	TYPE_COMPASSES,     //圆规

	TYPE_RULER,         //直尺
    TYPE_SETSQUARE,     //三角板
	TYPE_SCREENREC,     //屏幕录制
	TYPE_DDOPEN,        //桌面绘图打开,其中DD代表的desktopDraw
	TYPE_DDFULLSCREEN,  //桌面绘图全屏
	TYPE_DDEFFECT,      //桌面绘图效果切换
	TYPE_DDCTRL,        //桌面绘图控制条
	TYPE_DDCLOSE,       //桌面绘图关闭
	TYPE_SAVE,			//保存
	TYPE_PRINT,			//打印
	TYPE_CLOSE,         //关闭
	TYPE_OPEN,          //打开
	TYPE_COMBINE,       //合并
	TYPE_RESAVE,		//另存
	TYPE_DLG_LINETYPE,  //线型
	TYPE_DLG_RUBBER,    //擦子
	TYPE_DLG_GRAPHICS,  //图形
	TYPE_DLG_RESOURCEFILE,//资源文件
	TYPE_DLG_TOOL,        //工具
	TYPE_DLG_DESKTOPDRAW,//标注
	TYPE_DLG_FILE,      //文件工具条
	TYPE_DLG_T,				//文本工具条
	TYPE_DLG_COLORPALETTE,   //调色板对话框
	TYPE_FUNCTIONDLG,       //功能面板
	TYPE_TOOLBAR_SHOW,         //显示和隐藏工具条
	TYPE_TOOLBAR_HIDE,         //显示和隐藏工具条
	TYPE_TOOLBOX_CLOSE ,       //工具箱关闭
	TYPE_EAGLE_EYE,                  //鹰眼
	//屏幕录制对话框中按钮
	TYPE_FULLSCREENRECORD,   //全屏录制
	TYPE_AREASCREENRECORD,   //区域录制
	TYPE_WINDOWSCREENRECORD,   //窗口录制
	TYPE_PAUSESCREENRECORD,   //录制暂停
	TYPE_STOPSCREENRECORD,   //录制停止
	TYPE_PROPERTYSCREENRECORD,   //录制的属性
	TYPE_PATHSCREENRECORD,   //录制的路径选择
	//属性对话框面板上
	TYPE_LINECOLORBUTTON,	//线颜色按钮
	TYPE_COLORRADIO,//
	TYPE_LINECOLORBUTTONDOWN,//按下颜色按钮
	//TYPE_ATTRIPADCOLORBUTTONDOWN,//按下颜色按钮
	TYPE_SOLIDCOLORBUTTON, //填充颜色按钮
	TYPE_SOLIDCOLORBUTTONDOWN, //按下填充颜色按钮
	//单机版时的联网和关机按钮
	TYPE_SINGLE_NETWORK,//单机版联网
	TYPE_SINGLE_LOCAL,//单机版本地
	TYPE_SINGLE_SHOTDOWN,//单机版时如果为终端关机
	TYPE_SINGLE_QUIT,     //单机版如果为pc机退出程序
	TYPE_ATRTRIANGLE , //任意三角形
	TYPE_DYTRIANGLE,   //等腰三角形
	TYPE_ZJTRIANGLE,   //直角三角形
	TYPE_DYZJTRIANGLE,  //等腰直角三角形
	TYPE_PALLELOGAM,  //平行四边形
	TYPE_PENTAGON,		//五边形
	TYPE_HEXAGON,		//六边形
	TYPE_FIVEPOINTSTART, //五角星
	TYPE_TRAPEZIUM,     //梯形
	TYPE_SAVE_PIC,		//保存成图片
	TYPE_ICOCURVE,    //图标曲线
	TYPE_DXF,      //插入Dxf
	TYPE_MAGCURVE, //魔术线
	TYPE_MIDI //midi文件
}eType;

typedef std::map<int,CString> MAP_PATH;
