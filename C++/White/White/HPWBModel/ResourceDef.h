#pragma once
#include <string>
#include <map>
using namespace std;
#define CHANGE_BUT_TYPE 1
typedef enum _eType
{
	TYPE_NONE = 0,//
	TYPE_SELECT,		//选择
	TYPE_PAGE,			//页
	TYPE_FULLSCREEN,	//全屏
	TYPE_EXITFULLSCREEN,//退出全屏
	TYPE_ARROWS,		//直线和箭头
	TYPE_BIGHT,			//曲线
	TYPE_LINE,			//直线
	TYPE_RUBBER,		//橡皮
	TYPE_RUBBER_SMALL,	//小橡皮
	TYPE_RUBBER_BIG,	//大橡皮
	TYPE_RUBBER_OBJ,	//对象橡皮
	TYPE_RUBBER_CURVE,	//曲线橡皮
	TYPE_IMAGE,			//插入图片
	TYPE_VIDEO,			//插入视频
	TYPE_DOCUMENT,		//插入文档
	TYPE_FLASH,			//插入flash
	TYPE_ALLTOOL,		//工具箱
	TYPE_RESOURCE,		//显示资源对话框
	TYPE_DEL,			//删除
	TYPE_PRE_PAGE,		//上一页
	TYPE_NEXT_PAGE,		//下一页
	TYPE_T,				//文本
	TYPE_REDO,			//上一步
	TYPE_UNDO,			//下一步
	TYPE_TRIANGLE,		//三角形
	TYPE_RECTANGLE,		//矩形
	TYPE_ROUNDNESS,     //圆形
	TYPE_RHOMB,			//菱形
	TYPE_POLYGON,       //多边形
	TYPE_FONT,			//字体
	TYPE_ATTRIBUTE,		//属性
	TYPE_SCREENSHOTS,	//截屏
	TYPE_MAQN,	        //放大镜
	TYPE_SPOTLIGHT,     //聚光灯
	TYPE_DESKTOPDRAW,   //桌面绘图
	TYPE_DDOPEN,        //桌面绘图打开,其中DD代表的desktopDraw
	TYPE_DDFULLSCREEN,  //桌面绘图全屏
	TYPE_DDEFFECT,      //桌面绘图效果切换
	TYPE_DDCTRL,        //桌面绘图控制条
	TYPE_DDCLOSE,       //桌面绘图关闭
	TYPE_SAVE,			//保存
	TYPE_PRINT,			//打印
	TYPE_CLOSE,         //关闭
	TYPE_OPEN,          //打开
	TYPE_RESAVE,		//另存
    TYPE_DLG_RESOURCE,  //显示和关闭资源工具条
	TYPE_DLG_RUBBER,    //擦子工具条
	TYPE_DLG_GRAPHICS,  //图形工具条
	TYPE_DLG_FILE,      //文件工具条
	TYPE_DLG_PAGE,      //页工具栏
    TYPE_CLR_REN,       //红
	TYPE_CLR_BLACK,     //黑
	TYPE_CLR_YELLOW,    //黄
	TYPE_CLR_ORANGE,    //橙
	TYPE_CLR_BLUE,      //蓝
	TYPE_CLR_PURPLE,    //紫
	TYPE_FUNCTIONDLG     //功能面板
}eType;

typedef std::map<int,string> MAP_PATH;