#ifndef __UIANIMLABEL_H__
#define __UIANIMLABEL_H__
#include <CoUI/Control/UILabel.h>
#include <GdiPlus.h>
using namespace Gdiplus;
namespace CoUI
{
/////////////////////////////////////////////////////////////////////////////////////
//

struct CGifImage;
class UILIB_API CAnimLabelUI : public CLabelUI
{
public:	
	CAnimLabelUI();
	~CAnimLabelUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface( LPCTSTR pstrName );
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetBkImage(LPCTSTR pStrImage);
	void DoEvent(TEventUI& event);
	void SetVisible(bool bVisible = true);//设置控件可视性
	void PaintBkImage(HDC hDC);
	//bContinue标志是继续上次被暂停的动画还是重新开始，loopCount指定循环次数，-1为无限循环
	//当bContinue为true时，loopCount=0表示完成上次被暂停的循环，否则无意义
	void Start(bool bContinue = true , int loopCount=-1 );
	void Stop( bool bBackUp = false );//bBackUp标志是否需要保持当前帧
private:
	Image* LoadgifImage( LPCTSTR pstrName ,int** pDelay ,int &nFrameCount );
protected:
	enum
	{
		GIF_TIMER_ID = 15
	};
	bool m_isUpdateTime;
	int m_loopCount;
	bool m_bActive;
	CGifImage* m_pGif;
	bool m_bUpdateImage;//标记是否需要重新加载图片
	RECT rcItem;    //添加临时变量以支持裁剪，后期将统一用Render中的方法
	RECT rcBmpPart; //该处理方法为临时处理方法，故命名没有进行统一，后期将删除
};
}// namespace CoUI

#endif // __UIANIMLABEL_H__