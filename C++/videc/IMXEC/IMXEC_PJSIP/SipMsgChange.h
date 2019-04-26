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
//SIP 消息内容以关键字分隔为两部分并去除关键字行
int SIPMsgDivideTwoPartDecLine(const char * msg,char * part1,char * part2,char * divide);

//get msg line form keyName 
//取含有关键字行 
int SIPGetMsgKeyNameLine(const char * msg,char * line,char * divide,int &nPos);

//line divide two part dec divide  notice: divide is what as dec what
//把行分隔为两部分并去除关键字 notice:关键字有多少字符就会去除多少字符
int SIPLineDivideTwoPartDecDivide(const char * msg,char * part1,char * part2,char * divide);

//line divide two part divide strcat to part1
//行分隔为两部分 关键字接到第一部分后面
int SIPLineDivideTwoPart(const char * msg,char * part1,char * part2,char * divide);

//line build type=1 line part2 change ,type =2 line part1 change ,type=3 去除keyname //组建行
int SIPMsgLineBuild(char * line,char * dst,char * src,char * divide,char * divide2,char * decname,int type);

//line update msg save last
//使用源消息内容的行对目标消息内容的行更新
int SIPMsgLineUpdate(char * msg,char * dst,char * src,char * divide);

//line part update msg save last,decname use to type is 3, 
//type=1 line part2 change ,type =2 line part1 change ,type=3 去除keyname
//divide can find line ,divide2 can divide line is two part 
//目标消息内容关键字所在的行的一部分使用源消息的关键字所在的行的一部分更新
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
	int				m_iLength;		//缓冲区中数据累积长度
	int				m_nfflusherror;
};

#endif // __SIP_MSG_CHANGE_H__
