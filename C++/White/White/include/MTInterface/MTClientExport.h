#ifndef _AVCON_MTLIB_INTERFACE_MTCLIENT_EXPORT_H_
#define _AVCON_MTLIB_INTERFACE_MTCLIENT_EXPORT_H_

#include "MT_Export.h"
#include "MTObjectBase.h"
#include "MTProtocolType.h"

namespace AVCON_SDK_MTTouch
{
	///添加一个场景[添加成功返回1，已经存在返回0，发生其它错误返回错误编号]
	AVCON_API_MT_GESTURE bool AddMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject, bool bActive = false);
	///移除一个场景[移除成功返回1，不存在返回0，发生其它错误返回错误编号]
	AVCON_API_MT_GESTURE bool RemoveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///激活一个场景管理器[仅对顶层场景有效]
	AVCON_API_MT_GESTURE bool ActiveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);
	///挂起一个场景管理器[仅对顶层场景有效]
	AVCON_API_MT_GESTURE bool DeactiveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///判断触摸对象是否存在
	AVCON_API_MT_GESTURE bool HasMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);
	///判断触摸对象的激活状态
	AVCON_API_MT_GESTURE bool IsActive(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///重启触摸服务
	AVCON_API_MT_GESTURE bool StartTouchDataServer(EProtocolType type, bool bRestart = false);
	///刷新主显示器的分辨率（仅当带触摸的显示器是主显示器时，该接口仅针对使用HID协议的触摸）
	AVCON_API_MT_GESTURE void UpdateScreenResolution();
	//设置获取多点触摸数据所使用的协议方式（HID协议、TUIO协议）[重启后生效]
	AVCON_API_MT_GESTURE bool SetMTTouchProtocolType(EProtocolType type);

	///返回连接状态
	AVCON_API_MT_GESTURE bool GetConnectState();
	///获取触摸数据的方式[应用程序运行过程中保持不变]
	AVCON_API_MT_GESTURE EProtocolType GetConnectProtocol();
	///获取手势库版本号,若失败则返回NULL
	AVCON_API_MT_GESTURE TCHAR* GetVersionInfo();
}

#endif
