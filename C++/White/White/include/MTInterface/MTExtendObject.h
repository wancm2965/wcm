#ifndef _AVCON_MTLIB_MULTITOUCH_OBJECT_EXTEND_H_
#define _AVCON_MTLIB_MULTITOUCH_OBJECT_EXTEND_H_

#include "MTObject.h"
#include <list>

namespace AVCON_SDK_MTTouch
{
	///////////////////////////////////////////////////////////////////////////////
	// ��չCMTObject: CPanMTObject
	///(1):��������ƽ�Ʋ�����
	///(2):�ص㣺����ƽ�Ƴ���һ����ʱ���ŷ���ƽ�Ʋ��������ҵ���Խʱ�������ó�Խ����
	///////////////////////////////////////////////////////////////////////////////
	class  AVCON_CLASS_MT_GESTURE CMTPanReportObject : public CMTObject
	{
	public:
		CMTPanReportObject(void);
		virtual ~CMTPanReportObject(void);

	private:
		enum MTPanState{
			PAN_STATE_IDLE,//����Ԥ��ǰ��״̬
			PAN_STATE_HIT_THRESHHOLD,//����Ԥ��״̬��׼������
			PAN_STATE_PANNING,//������״̬"ƽ�Ʋ���"��ֱ������"����"�㶼�����Զ�תΪԤ��ǰ��
		};

	private:
		/// touch event handlers
		virtual void touchDownHandler(const AVCON_SDK_MTTouch::TouchDown& te);
		virtual void touchMoveHandler(const AVCON_SDK_MTTouch::TouchMove& te);
		virtual void touchUpHandler(const AVCON_SDK_MTTouch::TouchUp& te);

		/// do not override 
		virtual bool SuspendTemporary();

		//
		bool DoesCursorExist(unsigned int nCursorID);

	private:
		/// override below
		virtual void OnBeginPaning(){};
		virtual void OnPan(float fTotalMoveX, float fTotalMoveY, float fCurMoveX, float fCurMoveY){};

	protected:
		/// initial gesture event handlers
		virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);
		/// set gate value
		void SetMinMoveDistance(unsigned int uDistance);
	private:
		/// gate value for move
		unsigned int m_uThreshholdMinMoveDistance;
		unsigned int m_uCurCumulativeMoveDistance;
		POINT m_ptFirstDown;

		std::list<unsigned int> *m_pLstCursorID;
		/// be Panning state
		MTPanState m_ePanningState;
	};

	///////////////////////////////////////////////////////////////////////////////
	// ��չCMTObject: CMTInertialList �б���������࣬�ṩ���Թ���
	///////////////////////////////////////////////////////////////////////////////
	/// type of list
	enum EListType
	{
		LIST_UNCERTAIN,
		LIST_HORIZON,
		LIST_VERTICAL,
	};
	class  AVCON_CLASS_MT_GESTURE CMTInertialList : virtual public CMTObject
	{
	protected:
		CMTInertialList(const EListType listType);

	private:
		/// gesture event handlers[custom process]
		virtual void OnGestureCombineHandle(const GestureCombinePar &gesPar);

		/// override it to deal with detail action
		virtual void MTGesturePan(INT xTrains, INT yTrains) = 0;

	private:
		///Type of list
		const EListType m_listType;
	};
}
#endif