#ifndef _AVCON_MTLIB_INTERFACE_MTCLIENT_EXPORT_H_
#define _AVCON_MTLIB_INTERFACE_MTCLIENT_EXPORT_H_

#include "MT_Export.h"
#include "MTObjectBase.h"
#include "MTProtocolType.h"

namespace AVCON_SDK_MTTouch
{
	///���һ������[��ӳɹ�����1���Ѿ����ڷ���0�������������󷵻ش�����]
	AVCON_API_MT_GESTURE bool AddMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject, bool bActive = false);
	///�Ƴ�һ������[�Ƴ��ɹ�����1�������ڷ���0�������������󷵻ش�����]
	AVCON_API_MT_GESTURE bool RemoveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///����һ������������[���Զ��㳡����Ч]
	AVCON_API_MT_GESTURE bool ActiveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);
	///����һ������������[���Զ��㳡����Ч]
	AVCON_API_MT_GESTURE bool DeactiveMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///�жϴ��������Ƿ����
	AVCON_API_MT_GESTURE bool HasMTObject(AVCON_SDK_MTTouch::CMTObjectBase *pObject);
	///�жϴ�������ļ���״̬
	AVCON_API_MT_GESTURE bool IsActive(AVCON_SDK_MTTouch::CMTObjectBase *pObject);

	///������������
	AVCON_API_MT_GESTURE bool StartTouchDataServer(EProtocolType type, bool bRestart = false);
	///ˢ������ʾ���ķֱ��ʣ���������������ʾ��������ʾ��ʱ���ýӿڽ����ʹ��HIDЭ��Ĵ�����
	AVCON_API_MT_GESTURE void UpdateScreenResolution();
	//���û�ȡ��㴥��������ʹ�õ�Э�鷽ʽ��HIDЭ�顢TUIOЭ�飩[��������Ч]
	AVCON_API_MT_GESTURE bool SetMTTouchProtocolType(EProtocolType type);

	///��������״̬
	AVCON_API_MT_GESTURE bool GetConnectState();
	///��ȡ�������ݵķ�ʽ[Ӧ�ó������й����б��ֲ���]
	AVCON_API_MT_GESTURE EProtocolType GetConnectProtocol();
	///��ȡ���ƿ�汾��,��ʧ���򷵻�NULL
	AVCON_API_MT_GESTURE TCHAR* GetVersionInfo();
}

#endif
