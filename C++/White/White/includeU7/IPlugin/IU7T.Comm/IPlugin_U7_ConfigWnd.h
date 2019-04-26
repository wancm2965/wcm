#ifndef _INTERFACE_PLUGIN_U7_CHILD_WINDOW_H_
#define _INTERFACE_PLUGIN_U7_CHILD_WINDOW_H_
#include <list>
#include "IConfigDlgBase.h"


/*----------------------------------------------------------------------------------
									[��      ��]
						������ṩ�Ӵ���Ĺ��������ʹ��
----------------------------------------------------------------------------------*/

namespace AVCONPlugin
{

	///�Ӵ�����Ϣ�ṹ
	struct SConfigWndInfo{
		IDlgBase	*pDlg;		//���ھ��
		std::string sControlID;		//�Ӵ��ڱ�ʶ����
		std::string sControlText;	//�Ӵ�����ʾ����
		HWND		hParentWnd;		//�Ӵ��ڵĸ����ھ��
		std::string sInternalID;	//����ID
		unsigned int nInternalID;	//����ID
	};


	/*----------------------------------------------------------------------------------
	����������ṩ�Ӵ���Ĳ���ӿ�[�ò���ṩ��ĳ�����彫��Ϊ���������ĳ��������Ӵ���]
	----------------------------------------------------------------------------------*/
	class IPlugin_U7_ConfigWnd
	{
	public:
		virtual ~IPlugin_U7_ConfigWnd(void){};
	public:
		/*------------------------��ȡ�Ӵ���-------------------------------
		����һ: �����ھ��
		������: U7״̬��������ڣ�������ȣ�
		������:	����ȡ(�򴴽�)�Ĵ��ڽṹ��Ϣ[�����]
		------------------------------------------------------------------*/
		virtual bool GetConfigWnd(const HWND hParent, const std::string& strName,
			const unsigned int nU7State, std::list<SConfigWndInfo> &pLstChildWnd) = 0;
		///�Ƴ��Ӵ���(����Ϊ�����ھ�����ɹ�����true�����򷵻�false)
		virtual bool RemoveConfigWnd(const HWND hParentWnd) = 0;
	};
	/////����ṩ������Ĳ���Ӵ���
	//class IPlugin_U7_ChildWnd : public IPlugin_U7_ChildWnd
	//{
	//public:
	//	///Ӧ�ø���
	//	virtual void OnApply(const HWND hParentWnd) = 0;
	//	///�ع����ϴ�Ӧ�ø���
	//	virtual void OnCancel(const HWND hParentWnd) = 0;
	//};
	
}//end namespace
#endif