#ifndef _AVCON_MTLIB_MULTITOUCH_OBJECT_EXTEND_H_
#define _AVCON_MTLIB_MULTITOUCH_OBJECT_EXTEND_H_

#include "MTObject.h"
#include <list>

namespace AVCON_SDK_MTTouch
{
	///////////////////////////////////////////////////////////////////////////////
	// 扩展CMTObject: CPanMTObject
	///(1):仅仅处理平移操作。
	///(2):特点：仅当平移超过一定量时，才反馈平移操作；并且当超越时，反馈该超越操作
	///////////////////////////////////////////////////////////////////////////////
	class  AVCON_CLASS_MT_GESTURE CMTPanReportObject : public CMTObject
	{
	public:
		CMTPanReportObject(void);
		virtual ~CMTPanReportObject(void);

	private:
		enum MTPanState{
			PAN_STATE_IDLE,//命中预置前的状态
			PAN_STATE_HIT_THRESHHOLD,//命中预置状态，准备报告
			PAN_STATE_PANNING,//报告后的状态"平移操作"，直到所有"触摸"点都弹起自动转为预置前的
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
	// 扩展CMTObject: CMTInertialList 列表触摸对象基类，提供惯性功能
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