#include <CoUI/UIlibDeclare.h>
/*
 *	2013.11.18
 *	Ӧ�������ֻ�����Ϊ������Ӧ�ó���
 *	�����edit�ؼ���edit�ؼ���ȡ�����㣨ʧȥ���㣩ʱ��
 *	����ײ������Ϸ�����Ӧ����Ϣ��
 */
//�༭���ý����ź�
#define LPSZ_COUI_EDIT_SETFOCUS_EVENT	TEXT("165F2C34-96B2-4403-A8B1-87E63CE28EAD_CoUI_SETFOCUS_EVENT")
//�༭���ý����ź�(ǰ���ź�)
#define LPSZ_COUI_EDIT_PRESETFOCUS_EVENT	TEXT("E9B0DB7F-1067-448a-B945-D0B8CCC06EE6_CoUI_PRESETFOCUS_EVENT")
//�༭��ʧȥ�����ź�
#define LPSZ_COUI_EDIT_KILLFOCUS_EVENT	TEXT("E4D5A7B0-05E3-4c69-B7A0-780612348C02_CoUI_KILLFOCUS_EVENT}")
//�༭��ʧȥ�����ź�(ǰ���ź�)
#define LPSZ_COUI_EDIT_PREKILLFOCUS_EVENT	TEXT("7FB5D36B-63A9-4866-83C6-FC893293E97A_CoUI_PreKILLFOCUS_EVENT}")

namespace CoUI
{
	//����(�ر�)Edit�������
	void UILIB_API SetEditFocusListeningState(bool bOpen);
	//��ȡ��ǰEdit�������״̬���Ƿ��ڼ�����
	bool UILIB_API GetEditFocusListeningState();
}//end of namespace CoUI