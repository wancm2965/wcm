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
	void SetVisible(bool bVisible = true);//���ÿؼ�������
	void PaintBkImage(HDC hDC);
	//bContinue��־�Ǽ����ϴα���ͣ�Ķ����������¿�ʼ��loopCountָ��ѭ��������-1Ϊ����ѭ��
	//��bContinueΪtrueʱ��loopCount=0��ʾ����ϴα���ͣ��ѭ��������������
	void Start(bool bContinue = true , int loopCount=-1 );
	void Stop( bool bBackUp = false );//bBackUp��־�Ƿ���Ҫ���ֵ�ǰ֡
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
	bool m_bUpdateImage;//����Ƿ���Ҫ���¼���ͼƬ
	RECT rcItem;    //�����ʱ������֧�ֲü������ڽ�ͳһ��Render�еķ���
	RECT rcBmpPart; //�ô�����Ϊ��ʱ��������������û�н���ͳһ�����ڽ�ɾ��
};
}// namespace CoUI

#endif // __UIANIMLABEL_H__