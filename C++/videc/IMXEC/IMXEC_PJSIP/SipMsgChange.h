//add pjsip msg update
#ifndef __SIP_MSG_CHANGE_H__
#define __SIP_MSG_CHANGE_H__

#include "stdafx.h"
#include <pjsip/sip_msg.h>

#define SIPMSGMaxLen 3072
#define SIPAddMaxCount 6

struct SIPMSGLine
{
	int nType;
	char Line[256];
};


void WriteTestInfo(char * msg,char * info);

//msg divide two part dec line
//SIP ��Ϣ�����Թؼ��ַָ�Ϊ�����ֲ�ȥ���ؼ�����
int SIPMsgDivideTwoPartDecLine(const char * msg,char * part1,char * part2,char * divide);

//get msg line form keyName 
//ȡ���йؼ����� 
int SIPGetMsgKeyNameLine(const char * msg,char * line,char * divide,int &nPos);

//line divide two part dec divide  notice: divide is what as dec what
//���зָ�Ϊ�����ֲ�ȥ���ؼ��� notice:�ؼ����ж����ַ��ͻ�ȥ�������ַ�
int SIPLineDivideTwoPartDecDivide(const char * msg,char * part1,char * part2,char * divide);

//line divide two part divide strcat to part1
//�зָ�Ϊ������ �ؼ��ֽӵ���һ���ֺ���
int SIPLineDivideTwoPart(const char * msg,char * part1,char * part2,char * divide);

//line build type=1 line part2 change ,type =2 line part1 change ,type=3 ȥ��keyname //�齨��
int SIPMsgLineBuild(char * line,char * dst,char * src,char * divide,char * divide2,char * decname,int type);

//line update msg save last
//ʹ��Դ��Ϣ���ݵ��ж�Ŀ����Ϣ���ݵ��и���
int SIPMsgLineUpdate(char * msg,char * dst,char * src,char * divide);

//line part update msg save last,decname use to type is 3, 
//type=1 line part2 change ,type =2 line part1 change ,type=3 ȥ��keyname
//divide can find line ,divide2 can divide line is two part 
//Ŀ����Ϣ���ݹؼ������ڵ��е�һ����ʹ��Դ��Ϣ�Ĺؼ������ڵ��е�һ���ָ���
int SIPMsgLinePartUpdate(char * msg,char * dst,char * src,char * divide,char * divide2,char * decname,int type);

//reference pj_utoa_pad
int sip_utoa_pad( unsigned long val, char *buf, int min_dig, int pad);

//intercept sip msg process ,return <= 0 is failed , >0 is successful
int ChangeSIPMSG(char * msg,int len,pjsip_msg * sipStruct);

void GetChangeSIPMSGSetting();

int ChangeInviteSIPMSG(char * msg,int len,pjsip_msg * sipStruct);

int ChangeResponseSDP(char * msg,int len,pjsip_msg * sipStruct);

int ChangeRegisterSIPMSG(char * msg,int len,pjsip_msg * sipStruct);

int Change200OKSIPMSG(char * msg,int len,pjsip_msg * sipStruct);

void GetChangeSIPMSGSetting();

void ProcessEnterLine(char msgDST[],int option=0);

int ChangeUpdateRequestSDP(char * msg,int len,pjsip_msg * sipStruct);

int SIPMSGRemoveMediaPortZeroLine(char * msg,int len,pjsip_msg * sipStruct);
int ChangeSubcribeRequest(char * msg,int len,pjsip_msg * sipStruct);

int ChangeSIPMSGDualVideoPort(pjsip_msg * sipStruct,char * msgDstSDP,char * msgSrcSDP,char * divide,char * divide2,char * decname,int type);


class CFileIO  
{
public:
	CFileIO();
	virtual ~CFileIO();

public:
	bool Open(const char* strFileName, const char* strFlag);
	void Close();
	int	Write(char* pData, int nLen);
	int Read(char* pData, int nReadLen);
	int Seek(unsigned long ulPos, int nOrigin);
	unsigned long GetCurPos();
	bool IsFileEnd();
private:
	FILE*			m_fp;
	int				m_iLength;		//�������������ۻ�����
	int				m_nfflusherror;
};

#endif // __SIP_MSG_CHANGE_H__
