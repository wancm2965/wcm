#pragma once
#ifndef _HPMAPELF_H_
#define _HPMAPELF_H_

#define HPMAP_ERRORNO_OPEN 1			//地图文件打开失败
#define HPMAP_ERRORNO_SAVE 2			//地图文件保存失败
#define HPMAP_ERRORNO_IMAGESIZE	3		//地图底图不符合约定大小 320X240以上
#define HPMAP_ERRORNO_IMAGETOOLARGE 4    //导入地图底图文件大于2M
#define HPMAP_ERRORNO_IMAGEFORMAT	5	//导入地图底图文件不符合Jpeg格式 
#define HPMAP_ERRORNO_BZ 6//标注名称已经存在


class AFX_EXT_CLASS CHPMAPElf_Notify
{
public:
	CHPMAPElf_Notify(){};
	~CHPMAPElf_Notify(){};

	virtual void OnObjDBClick(int id)=0; //双击一个标注
	virtual void OnObjSelect (int id)=0; //选择一个标注
	virtual void OnMapError(int nErrNo)=0; //回调出错信息
	virtual void OnSaveMap(CString &strFiletoSave)=0; //保存好地图的时候进行什么操作(上传?)
	virtual void OnOpenMap(CString &strFiletoOpen)=0; //打开地图的时候要做什么操作
	virtual void OnNewMap()=0; //新建或者关闭当前地图时触发
	virtual void OnLevel()=0;  //当点击了回到上一层的热点或者命令时触发。 

};
class AFX_EXT_CLASS CHPMAPElf
{
public:
		CHPMAPElf(CHPMAPElf_Notify &ntf);
		~CHPMAPElf(void);

public:
	    //******************************  通用接口   ******************************************
		//创建地图窗体
		CWnd* CreateMapCtrl(CHPMAPElf_Notify &not,CWnd* pParent);
		//释放地图窗体
		void ReleaseMapCtrl();
		
		//设置地图状态：编辑/浏览
		BOOL SetMapModel(BOOL bEdit);

		//打开地图文件 *.map
		BOOL OpenMap(CString strFiletoOpen);	
		//新建一张地图文件或者关闭地图文件
		BOOL NewMap();
        //保存地图文件为*.map
		BOOL SaveMap(CString strFiletoSave);

		//设置地图默认读取路径  
		BOOL SetMapRoot(CString str);

		//获取当前地图文件*.map的哈希值 64位字符串
		CString GetMapHash();
		
		//设置当前地图文件的信息 标题和描述
		void SetInfo(CString mapTitle,CString mapNote); 
		
		//设置一个标注是否闪动 
		void SetFlash(int nodeid,bool bfresh); 

		//增加一个热点/标注
		//输入 热点/标注类型 标注名称 X坐标 Y坐标
		int  AddMapPoint(int type,CString strName,int x=50,int y=50);

		//查询热点/标注的信息
		//输入：热点的ID
		//输出：热点的类型,名字
		//返回：热点/标注是否存在
		BOOL QueryMapPoint(int id,int &type,CString &strName,int &x,int &y);

		//获取地图控件的版本号
		CString GetMapVersion();

		//地图是否已经改变
		BOOL IsMapChange();

private:
	CHPMAPElf_Notify &notify;

	 
};

#endif