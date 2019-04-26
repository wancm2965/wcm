
// ComConsole_API.h

#ifdef ComConsole_API
#define DllExport_ComConsole __declspec(dllexport)
#else
#define DllExport_ComConsole __declspec(dllimport)
#endif

// COM�˿ڰ�����Ϣ
// WPARAM������ʾ����״̬��0Ϊ����״̬������Ϊ����״̬
// LPARAM����Ϊ����ID 10�����ֱ�Ϊ0x70~0x79
#define WM_COM_KEY WM_USER+2000

// iPort���� COM�˿ں�
#define COM1	1
#define COM2	2
#define COM3	3
#define COM4	4

// iBaud���� ������
#define B50			0x00
#define B75			0x01
#define B110		0x02
#define B134		0x03
#define B150		0x04
#define B300		0x05
#define B600		0x06
#define B1200		0x07
#define B1800		0x08
#define B2400		0x09
#define B4800		0x0A
#define B7200		0x0B
#define B9600		0x0C
#define B19200		0x0D
#define B38400		0x0E
#define B57600		0x0F
#define B115200 	0x10
#define B230400 	0x11
#define B460800 	0x12
#define B921600 	0x13

// iBit���� ����λ
#define BIT_5		0x00
#define BIT_6		0x01
#define BIT_7		0x02
#define BIT_8		0x03

// iStop���� ֹͣλ
#define STOP_1		0x00
#define STOP_2		0x04

// iProve���� У��λ
#define P_EVEN		0x18
#define P_ODD		0x08
#define P_SPC		0x38
#define P_MRK		0x28
#define P_NONE		0x00

// ��������
// ������
//     hMsgRec��COM�˿ڰ�����Ϣ�Ľ��ܾ��
//     ��������������˵����һ��ֱ�ʹ�� COM1 B1200 BIT_8 STOP_1 P_NONE
// ����ֵ��
//     �����ɹ�����TRUE�����򷵻�FALSE
DllExport_ComConsole BOOL CreateComConsole(HWND hMsgRec, int iPort, int iBaud, int iBit, int iStop, int iProve);

// �������
DllExport_ComConsole void ReleaseComConsole();