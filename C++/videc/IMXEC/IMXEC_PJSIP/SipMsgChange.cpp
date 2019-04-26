#include "stdafx.h"
#include "SipMsgChange.h"
#include "SIPCallMgr.h"

#include <pjmedia/sdp.h>
#include <pjsua-lib/pjsua.h>

#define SIPAddMaxCount 6

//save ini
int nUseFileMSG=0;
char SIPReadFileName[256]={0};
int nRecordMsg=0;
char SIPRecordFileName[256]={0};
int nInviteID=0;
int nRegisterID=0;
int nChangeHeader=0;
int nChangeSDP=0;
int nHeaderLineADD=0;
int nSDPLineADD=0;
int nHeaderLineDEC=0;
int nSDPLineDEC=0;
SIPMSGLine SIPHeaderLineADD[8];
SIPMSGLine SIPHeaderLineDEC[8];
SIPMSGLine SIPSDPLineADD[8];
SIPMSGLine SIPSDPLineDEC[8];
int nRemoveMediaPortZero=0;

char g_sipmsgsrc[2048]=
{
	"INVITE sip:ppppp@hq.cmcc SIP/2.0\r\n"
	"Via: SIP/2.0/SCTP server10.biloxi.com;branch=z9hG4bKnashds8;rport;received=192.0.2.1\r\n"
	"Max-Forwards: 70\r\n"
	"To: Bob <sip:bob@biloxi.com>;tag=a6c85cf\r\n"
	"From: Alice <sip:alice@atlanta.com>;tag=1928301774\r\n"
	"Call-ID: a84b4c76e66710@pc33.atlanta.com\r\n"
	"CSeq: 314159 INVITE\r\n"
	"Contact: <sips:bob@192.0.2.4>\r\n"
	"Content-Type: application/sdp\r\n"
	"Content-Length: 150\r\n"
	"\r\n"
	"v=0\r\n"
	"o=alice 53655765 2353687637 IN IP4 srcsrc.com\r\n"
	"s=-\r\n"
	"t=0 0\r\n"
	"c=IN IP4 srcsrc.com\r\n"
	"m=audio srcsrc RTP/AVP 0 1 3 99\r\n"
	"a=rtpmap:0 PCMU/8000\r\n"
	"m=video srcsrc RTP/AVP 31\r\n"
	"a=rtpmap:106 H264/90000\r\n"
};

CFileIO m_TextFileSave;
CFileIO m_TextFileRead;

int ChangeSIPMSG(char * msg,int len,pjsip_msg * sipStruct)
{
	if( SIPCallMgr::Instance().m_nSendMultipartSDP > 0 )
	{
		return 0;
	}
	GetChangeSIPMSGSetting();
	pjsip_method_e id = sipStruct->line.req.method.id;
	pjsip_msg_type_e  nType = sipStruct->type;
	int nRemoveLen=0;
	//移除端口为0的视频或辅流media line之后的所有能力集,此用于中兴对通测试
	if(nRemoveMediaPortZero > 0)
	{
		if( (nType == PJSIP_RESPONSE_MSG) && ( (sipStruct->line.status.code==200) || (sipStruct->line.status.code== 183) ) && sipStruct->body )
			nRemoveLen=SIPMSGRemoveMediaPortZeroLine(msg,len,sipStruct);
	}

	if(nRegisterID>0 && id==PJSIP_REGISTER_METHOD && nType ==PJSIP_REQUEST_MSG )
		return ChangeRegisterSIPMSG(msg,len,sipStruct);

	if( (nType == PJSIP_REQUEST_MSG) && (id==PJSIP_INVITE_METHOD) && (nInviteID>0) && sipStruct->body )
		return ChangeInviteSIPMSG(msg,len,sipStruct);

	if( (nType == PJSIP_RESPONSE_MSG) && ( (sipStruct->line.status.code==200) || (sipStruct->line.status.code== 183) ) && sipStruct->body )
		return ChangeResponseSDP(msg,len,sipStruct);

	const pjsip_method pjsip_update_method ={ PJSIP_OTHER_METHOD, { "UPDATE",6}};
	if (pjsip_method_cmp(&sipStruct->line.req.method,&pjsip_update_method) == 0)
	{
		if( (nType == PJSIP_REQUEST_MSG) && (nInviteID>0) && sipStruct->body )
			return ChangeUpdateRequestSDP(msg,len,sipStruct);
	}
	const pjsip_method pjsip_subscribe_method ={ PJSIP_OTHER_METHOD, { "SUBSCRIBE",9}};
	if (pjsip_method_cmp(&sipStruct->line.req.method,&pjsip_subscribe_method) == 0)
	{
		//if( (nType == PJSIP_REQUEST_MSG) && (nInviteID>0) && sipStruct->body )
		//	return ChangeSubcribeRequest(msg,len,sipStruct);
	}

	if(nRemoveMediaPortZero > 0)
	{
		return nRemoveLen;
	}
	return 0;
}

void WriteTestInfo(char * msg,char * info)
{
	//test
	m_TextFileSave.Write("------\r\n",8);
	int msgLen=strlen(msg);
	char overLenBuf[256]={0};
	sprintf(overLenBuf,"Test:Info:%s\r\nMSGLength:%d\r\n",info,msgLen);
	m_TextFileSave.Write(overLenBuf,strlen(overLenBuf));
	m_TextFileSave.Write(msg,msgLen);
	m_TextFileSave.Write("\r\n",2);
	m_TextFileSave.Write("------",8);
}

//SIP 消息内容以关键字分隔为两部分并去除关键字行
int SIPMsgDivideTwoPartDecLine(const char * msg,char * part1,char * part2,char * divide)
{
	char * p=(char *)msg;
	char * pnBegin=NULL;
	char * pnEnd=NULL;
	int nLen=strlen(p);
	int nPart2Len=0;
	pnBegin=strstr(p,divide);

	char * crlf="\r\n";//notice file save is 0x0a 0x0a
	if(pnBegin)
		pnEnd=strstr(pnBegin,crlf);
	if(pnBegin && pnEnd)
	{
		pnEnd=pnEnd+2;
		strncpy(part1,p,pnBegin-p);
		part1[pnBegin-p]='\0';
		nPart2Len=nLen-(pnEnd-p);
		strncpy(part2,pnEnd,nPart2Len);
		part2[nPart2Len]='\0';
		return 1;
	}
	return 0;
}

//消息内容去除关键字行
int SIPMsgPartBuild(char * msg,char * dst,char * divide)
{
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char line[SIPMSGMaxLen]={0};
	if(! SIPMsgDivideTwoPartDecLine(dst,part1,part2,divide) ) return 0;
	strcpy(msg,part1);
	strcat(msg,part2);
	int len=strlen(msg);
	return len;
}

//取含有关键字行 
int SIPGetMsgKeyNameLine(const char * msg,char * line,char * divide,int &nPos)
{
	char * p=(char *)msg;
	char * pnBegin=NULL;
	char * pnEnd=NULL;
	int nLen=strlen(p);
	int nPart2Len=0;
	pnBegin=strstr(p,divide);
	char * crlf="\r\n";//notice file save is 0x0a 0x0a
	if(pnBegin)
		pnEnd=strstr(pnBegin,crlf);
	if(pnBegin && pnEnd)
	{
		pnEnd=pnEnd+2;
		strncpy(line,pnBegin,pnEnd-pnBegin);
		line[pnEnd-pnBegin]='\0';
		nPos=pnEnd-msg;
		return 1;
	}
	return 0;
}

//把行分隔为两部分并去除关键字 notice:关键字有多少字符就会去除多少字符
int SIPLineDivideTwoPartDecDivide(const char * msg,char * part1,char * part2,char * divide)
{
	char * p=(char *)msg;
	char * pnBegin=NULL;
	int nLen=strlen(p);
	int nPart2Len=0;
	int nDivideLen=strlen(divide);
	pnBegin=strstr(p,divide);
	if(pnBegin)
	{
		strncpy(part1,p,pnBegin-p);
		part1[pnBegin-p]='\0';
		pnBegin=pnBegin+nDivideLen;
		nPart2Len=nLen-(pnBegin-p);
		strncpy(part2,pnBegin,nPart2Len);
		part2[nPart2Len]='\0';
		return 1;
	}
	return 0;
}

//行分隔为两部分 关键字接到第一部分后面
int SIPLineDivideTwoPart(const char * msg,char * part1,char * part2,char * divide)
{
	char * p=(char *)msg;
	char * pnBegin=NULL;
	int nLen=strlen(p);
	int nPart2Len=0;
	int nDivideLen=strlen(divide);
	pnBegin=strstr(p,divide);
	if(pnBegin)
		pnBegin=pnBegin+nDivideLen;
	if(pnBegin)
	{
		strncpy(part1,p,pnBegin-p);
		part1[pnBegin-p]='\0';
		nPart2Len=nLen-(pnBegin-p);
		strncpy(part2,pnBegin,nPart2Len);
		part2[nPart2Len]='\0';
		return 1;
	}
	return 0;
}

//组建行 type=1 line part2 change ,type =2 line part1 change ,type=3 去除keyname
int SIPMsgLineBuild(char * line,char * dst,char * src,char * divide,char * divide2,char * decname,int type)
{
	char srcpart1[SIPMSGMaxLen]={0};
	char srcpart2[SIPMSGMaxLen]={0};
	char dstpart1[SIPMSGMaxLen]={0};
	char dstpart2[SIPMSGMaxLen]={0};
	char srcLine[SIPMSGMaxLen]={0};
	char dstLine[SIPMSGMaxLen]={0};
	int nEndPos=0;
	if(type==1 || type==2 )
	{
		if(! SIPGetMsgKeyNameLine(src,srcLine,divide,nEndPos)) return 0;
		if(! SIPGetMsgKeyNameLine(dst,dstLine,divide,nEndPos)) return 0;
		if(! SIPLineDivideTwoPart(srcLine,srcpart1,srcpart2,divide2)) return 0;
		if(! SIPLineDivideTwoPart(dstLine,dstpart1,dstpart2,divide2)) return 0;
		if(type==2)
		{
			strcpy(line,srcpart1);
			strcat(line,dstpart2);
		}
		if(type==1)
		{
			strcpy(line,dstpart1);
			strcat(line,srcpart2);
		}
	}
	if(type==3)
	{
		if(! SIPGetMsgKeyNameLine(dst,dstLine,divide,nEndPos)) return 0;
		if(! SIPLineDivideTwoPartDecDivide(dstLine,dstpart1,dstpart2,decname))  return 0;
		strcpy(line,dstpart1);
		strcat(line,dstpart2);
	}
	return 1;
}

//使用源消息内容的行对目标消息内容的行更新
int SIPMsgLineUpdate(char * msg,char * dst,char * src,char * divide)
{
	//if(! SIPMsgDivideTwoPartDecLine(dst,part1,part2,divide) ) return 0;
	//if(! SIPGetMsgKeyNameLine(src,line,divide) ) return 0;
	//strcpy(msg,part1);
	//strcat(msg,line);
	//strcat(msg,part2);
	int nRet=0;
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char line[SIPMSGMaxLen]={0};
	char destline[SIPMSGMaxLen]={0};
	int nDestLineEndPos=0;
	int nLineEndPos=0;
	int nt1=0;
	int nt2=0;
	int nGet=0;
	char * pDst=dst;
	char * pSrc=src;
	char * pMsg=msg;
	int nMsgPos=0;
	while(1)
	{
		nt1=SIPMsgDivideTwoPartDecLine(pDst,part1,part2,divide);
		if(nt1)
		{
			SIPGetMsgKeyNameLine(pDst,destline,divide,nDestLineEndPos);
			nt2=SIPGetMsgKeyNameLine(pSrc,line,divide,nLineEndPos);
			if(nt2)
			{
				nRet=1;
				strcpy(destline,line);//change new line
			}
			//else destline[0]='\0'; // the is dispute  not find is del?
			strcpy(pMsg,part1);      //cover part
			strcat(pMsg,destline);   //cover line
			nMsgPos=strlen(pMsg);
			pDst=part2;
			pSrc=pSrc+nLineEndPos;
			pMsg=pMsg+nMsgPos;
			nGet=1;
		}
		else break;
	}
	if(nGet == 1)  strcat(msg,pDst);//cover bottom
	else strcpy(msg,dst);
	return nRet;
}

//SIP消息SDP行添加
int SIPMsgSDPLineADD(char * msg,char * divide,char * newLine)
{
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char line[SIPMSGMaxLen]={0};
	int nEndPos=0;
	if(! SIPGetMsgKeyNameLine(msg,line,divide,nEndPos) ) return 0;
	if(! SIPMsgDivideTwoPartDecLine(msg,part1,part2,divide) ) return 0;
	strcpy(msg,part1);
	strcat(msg,line);
	strcat(msg,newLine);
	strcat(msg,part2);
	return 1;
}

//目标消息内容关键字所在的行的一部分使用源消息的关键字所在的行的一部分更新,decname use to type is 3, 
//type=1 line part2 change ,type =2 line part1 change ,type=3 去除keyname
//divide can find line ,divide2 can divide line is two part 
int SIPMsgLinePartUpdate(char * msg,char * dst,char * src,char * divide,char * divide2,char * decname,int type)
{
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char line[SIPMSGMaxLen]={0};
	if(! SIPMsgDivideTwoPartDecLine(dst,part1,part2,divide) ) return 0;
	if(! SIPMsgLineBuild(line,dst,src,divide,divide2,decname,type)) return 0;
	strcpy(msg,part1);
	strcat(msg,line);
	strcat(msg,part2);
	return 1;
}

//reference pj_utoa_pad
int sip_utoa_pad( unsigned long val, char *buf, int min_dig, int pad)
{
	char *p;
	int len;
	p = buf;
	do {
		unsigned long digval = (unsigned long) (val % 10);
		val /= 10;
		*p++ = (char) (digval + '0');
	} while (val > 0);
	len = p-buf;
	while (len < min_dig) {
		*p++ = (char)pad;
		++len;
	}
	*p-- = '\0';
	do {
		char temp = *p;
		*p = *buf;
		*buf = temp;
		--p;
		++buf;
	} while (buf < p);
	return len;
}

void GetChangeSIPMSGSetting()
{
	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Left(n);
	strPath += _T("\\USEFILESIPMSG.ini");
	nRemoveMediaPortZero=GetPrivateProfileInt("SIPMSG","RemoveMediaPortZero",0,strPath);
	nUseFileMSG=GetPrivateProfileInt("SIPMSG","UseFileMSG",0,strPath);
	nRecordMsg=GetPrivateProfileInt("SIPMSG","RecordMsg",0,strPath);
	nInviteID=GetPrivateProfileInt("SIPMSG","InviteID",0,strPath);
	nRegisterID=GetPrivateProfileInt("SIPMSG","RegisterID",0,strPath);
	nChangeHeader=GetPrivateProfileInt("SIPMSG","ChangeHeader",0,strPath);
	nChangeSDP=GetPrivateProfileInt("SIPMSG","ChangeSDP",0,strPath);
	nHeaderLineADD=GetPrivateProfileInt("SIPMSG","HeaderLineADD",0,strPath);
	nSDPLineADD=GetPrivateProfileInt("SIPMSG","SDPLineADD",0,strPath);
	nHeaderLineDEC=GetPrivateProfileInt("SIPMSG","HeaderLineDEC",0,strPath);
	nSDPLineADD=GetPrivateProfileInt("SIPMSG","SDPLineDEC",0,strPath);
	GetPrivateProfileString("SIPMSG", "ReadFileName", "SIPMSGRead.log",SIPReadFileName,255, strPath );
	GetPrivateProfileString("SIPMSG", "RecordFileName", "SIPMSGSave.log",SIPRecordFileName,255, strPath );
	char type[32]={0};
	char line[32]={0};
	int nCount=nHeaderLineADD;
	char * section="SIPHeaderLineADD";
	SIPMSGLine * msgLine=SIPHeaderLineADD;
	for(int i=0;i<nCount;i++)
	{
		sprintf(type,"Type%d",i+1);
		sprintf(line,"Line%d",i+1);
		msgLine[i].nType=GetPrivateProfileInt(section,type,0,strPath);
		GetPrivateProfileString(section, line, "",msgLine[i].Line,255, strPath );
	}
	nCount=nSDPLineADD;
	section="SIPSDPLineADD";
	msgLine=SIPSDPLineADD;
	for(int i=0;i<nCount;i++)
	{
		sprintf(type,"Type%d",i+1);
		sprintf(line,"Line%d",i+1);
		msgLine[i].nType=GetPrivateProfileInt(section,type,0,strPath);
		GetPrivateProfileString(section, line, "",msgLine[i].Line,255, strPath );
	}
	nCount=nHeaderLineDEC;
	section="SIPHeaderLineDEC";
	msgLine=SIPHeaderLineDEC;
	for(int i=0;i<nCount;i++)
	{
		sprintf(type,"Type%d",i+1);
		sprintf(line,"Line%d",i+1);
		msgLine[i].nType=GetPrivateProfileInt(section,type,0,strPath);
		GetPrivateProfileString(section, line, "",msgLine[i].Line,255, strPath );
	}
	nCount=nSDPLineDEC;
	section="SIPSDPLineDEC";
	msgLine=SIPSDPLineDEC;
	for(int i=0;i<nCount;i++)
	{
		sprintf(type,"Type%d",i+1);
		sprintf(line,"Line%d",i+1);
		msgLine[i].nType=GetPrivateProfileInt(section,type,0,strPath);
		GetPrivateProfileString(section, line, "",msgLine[i].Line,255, strPath );
	}
}

//处理回车行 使之每一行以\r\n结束
void ProcessEnterLine(char msgDST[],int option)
{
	//process read file data ensure a line is end with \r\n 
	//end \r\n\r\n check add
	int changeLen=strlen(msgDST);
	char * pCh=msgDST;
	char dstp2[SIPMSGMaxLen]={0};
	int addcount=0;
	while(pCh!='\0')
	{
		if(option<=0)
		{
			//\n\n notice two line process no way
			if( (*pCh == '\n') && (*(pCh+1) == '\n') )
				*pCh='\r';
		}
		//\r\r
		if( (*pCh == '\r') && (*(pCh+1) == '\r') )
			*(pCh+1)='\n';
		//one \r
		if( (*pCh == '\r') && (*(pCh+1) != '\n') )
		{
			if(*(pCh+1)=='\0') strcat(pCh,"\n");
			else
			{
				strcpy(dstp2,pCh+1);
				*(pCh+1)='\n';
				strcpy(pCh+2,dstp2);
			}
			addcount++;
		}
		//one \n
		if( (*pCh == '\n') && (*(pCh-1) != '\r') )
		{
			strcpy(dstp2,pCh);
			*pCh='\r';
			strcpy(pCh+1,dstp2);
			addcount++;
		}
		pCh++;
		if( (pCh-msgDST)>=(changeLen+addcount) ) break;
	}
	changeLen+=addcount;
	if (strcmp(msgDST+changeLen-2,"\r\n") != 0)
	{
		strcat(msgDST,"\r\n");//add
		changeLen+=2;
	}
	if (strcmp(msgDST+changeLen-4,"\r\n\r\n") == 0)
	{
		msgDST[changeLen-2]='\0';
	}
}

int ReadFileContent(char * dst)
{
	int i=0;
	while(m_TextFileRead.IsFileEnd()==false)
	{
		int length=SIPMSGMaxLen;
		char buf[SIPMSGMaxLen]={0};
		m_TextFileRead.Read(buf,length);
		strcpy(dst+i*length,buf);
		i++;
	}
	return 0;
}

int SIPMSGBuild(char * msg,char * msgDstSDP,char * msgDstHeader)
{
	//6 header dec
	//7 sdp dec
	//8 calculate sdp length
	char * pMsgDstSDP=msgDstSDP+2;//move pointer 2 byte \r\n 
	int nSDPLength=strlen(pMsgDstSDP);
	//9 build content length
	char contentLine[128]={0};
	char tmp[16]={0};
	int contentL = sip_utoa_pad(nSDPLength, tmp,0,0);
	sprintf(contentLine,"Content-Length: %s\r\n",tmp);
	//10 add content length line to header
	strcat(msgDstHeader,contentLine);
	strcat(msgDstHeader,"\r\n");
	//11 calculate header length
	int nHeaderLength=strlen(msgDstHeader);
	//12 build msg
	int nTotalLen=nHeaderLength+nSDPLength;
	strcpy(msg,msgDstHeader);
	strcat(msg,pMsgDstSDP);
	return nTotalLen;
}

void RecordMsgInfo(char * msg,int srclentest,int dstlentest,int nTotalLength)
{
	char overLenBuf[256]={0};
	sprintf(overLenBuf,"SIPMSG Finish:\r\nSrcLength:%d,DestLength:%d\r\nTotalProcesssLength:%d\r\n",srclentest,dstlentest,nTotalLength);
	m_TextFileSave.Write(overLenBuf,strlen(overLenBuf));
	WriteTestInfo(msg,"MSG Test");
}

int ChangeSubcribeRequest(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgSRC[SIPMSGMaxLen]={0};
	char msgDST[SIPMSGMaxLen]={0};
	char msgDstHeader[SIPMSGMaxLen]={0};
	char msgDstSDP[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	char sipInvReadFileName[256]={0};
	sprintf(sipInvReadFileName,"Subscr%s",SIPReadFileName);
	bool bReadOpen=false;
	{
		memcpy(msgSRC,msg,len);
		msgSRC[len]='\0';
		if(nUseFileMSG>0) bReadOpen=m_TextFileRead.Open(sipInvReadFileName,"r");
		if(nRecordMsg>0) m_TextFileSave.Open(SIPRecordFileName,"w");
		if(bReadOpen)
		{
			ReadFileContent(msgDST);
		}
		else
		{
			return 0;
		}
		//process read file data ensure a line is end with \r\n 
		ProcessEnterLine(msgDST,1);
		int srclentest=strlen(msgSRC);
		int dstlentest=strlen(msgDST);

		//change SIP msg
		char * divide="Content-Length:";
		if(! SIPMsgDivideTwoPartDecLine(msgDST,msgDstHeader,msgDstSDP,divide) ) return 0;
		if(! SIPMsgDivideTwoPartDecLine(msgSRC,msgSrcHeader,msgSrcSDP,divide) ) return 0;

		//2 change sdp
		if(nChangeSDP)
		{
			//if(! SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"o=","IN IP4","",1) ) return 0;
			//if(! SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"c=IN IP4") ) return 0;
			//SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=audio","RTP/AVP","",2);
			//SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video","RTP/AVP","",2);
			////SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video");
			////SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=application");//zhong xing not 
			//SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"a=rtcp");
		}
		//3 change headerRequest-Line: SUBSCRIBE sip:996601172@pgw.ims SIP/2.0
		if(nChangeHeader)
		{
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"SUBSCRIBE sip:") ) return 0;
			SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Via: SIP") ;
			SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Max-Forwards:");
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"From:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"To:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Contact:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Call-ID:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"CSeq:") ) return 0;
		}
		//4 header add
		if(nHeaderLineADD>0)
		{
			for(int j=0;j<nHeaderLineADD;j++)
			{
				strcat(msgDstHeader,SIPHeaderLineADD[j].Line);
				strcat(msgDstHeader,"\r\n");
			}
		}
		//5 sdp add
		if(nSDPLineADD>0)
		{
			char * divideName="m=audio";
			for(int j=0;j<nSDPLineADD;j++)
			{
				if(SIPSDPLineADD[j].nType==2) divideName="m=video";
				strcat(SIPSDPLineADD[j].Line,"\r\n");
				SIPMsgSDPLineADD(msgDstSDP,divideName,SIPSDPLineADD[j].Line);
			}
		}

		int nTotalLength=SIPMSGBuild(msg,msgDstSDP,msgDstHeader);
		//write msg file log to see
		if(nRecordMsg>0)
		{
			RecordMsgInfo(msg,srclentest,dstlentest,nTotalLength);
		}

		if(nUseFileMSG>0) m_TextFileRead.Close();
		if(nRecordMsg>0) m_TextFileSave.Close();
		return nTotalLength;
	}
}




int ChangeUpdateRequestSDP(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgSRC[SIPMSGMaxLen]={0};
	char msgDST[SIPMSGMaxLen]={0};
	char msgDstHeader[SIPMSGMaxLen]={0};
	char msgDstSDP[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	char sipInvReadFileName[256]={0};
	sprintf(sipInvReadFileName,"UPDA%s",SIPReadFileName);
	bool bReadOpen=false;
	{
		memcpy(msgSRC,msg,len);
		msgSRC[len]='\0';
		if(nUseFileMSG>0) bReadOpen=m_TextFileRead.Open(sipInvReadFileName,"r");
		if(nRecordMsg>0) m_TextFileSave.Open(SIPRecordFileName,"w");
		if(bReadOpen)
		{
			ReadFileContent(msgDST);
		}
		else
		{
			return 0;
		}
		//process read file data ensure a line is end with \r\n 
		ProcessEnterLine(msgDST,1);
		int srclentest=strlen(msgSRC);
		int dstlentest=strlen(msgDST);

		//change SIP msg
		char * divide="Content-Length:";
		if(! SIPMsgDivideTwoPartDecLine(msgDST,msgDstHeader,msgDstSDP,divide) ) return 0;
		if(! SIPMsgDivideTwoPartDecLine(msgSRC,msgSrcHeader,msgSrcSDP,divide) ) return 0;

		//2 change sdp
		if(nChangeSDP)
		{
			if(! SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"o=","IN IP4","",1) ) return 0;
			if(! SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"c=IN IP4") ) return 0;
			SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=audio","RTP/AVP","",2);
			SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video","RTP/AVP","",2);
			ChangeSIPMSGDualVideoPort(sipStruct,msgDstSDP,msgSrcSDP,"m=video2","RTP/AVP","",2);
			//SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video");
			//SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=application");//zhong xing not 
			SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"a=rtcp");
		}
		//3 change header
		if(nChangeHeader)
		{
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"UPDATE sip:") ) return 0;
			SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Via: SIP") ;
			SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Max-Forwards:");
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"From:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"To:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Contact:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Call-ID:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"CSeq:") ) return 0;
		}
		//4 header add
		if(nHeaderLineADD>0)
		{
			for(int j=0;j<nHeaderLineADD;j++)
			{
				strcat(msgDstHeader,SIPHeaderLineADD[j].Line);
				strcat(msgDstHeader,"\r\n");
			}
		}
		//5 sdp add
		if(nSDPLineADD>0)
		{
			char * divideName="m=audio";
			for(int j=0;j<nSDPLineADD;j++)
			{
				if(SIPSDPLineADD[j].nType==2) divideName="m=video";
				strcat(SIPSDPLineADD[j].Line,"\r\n");
				SIPMsgSDPLineADD(msgDstSDP,divideName,SIPSDPLineADD[j].Line);
			}
		}

		int nTotalLength=SIPMSGBuild(msg,msgDstSDP,msgDstHeader);
		//write msg file log to see
		if(nRecordMsg>0)
		{
			RecordMsgInfo(msg,srclentest,dstlentest,nTotalLength);
		}

		if(nUseFileMSG>0) m_TextFileRead.Close();
		if(nRecordMsg>0) m_TextFileSave.Close();
		return nTotalLength;
	}
}


//test
int SIPMsgLinePartUpdateMediaPort(char * msg,char * dst,char * src,char * divide,char * divide2,char * decname,int type,int port)
{
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char line[SIPMSGMaxLen]={0};
	if(! SIPMsgDivideTwoPartDecLine(dst,part1,part2,divide) ) return 0;
	char dstpart1[SIPMSGMaxLen]={0};
	char dstpart2[SIPMSGMaxLen]={0};
	char dstLine[SIPMSGMaxLen]={0};
	int nEndPos=0;
	if(! SIPGetMsgKeyNameLine(dst,dstLine,divide,nEndPos)) return 0;
	if(! SIPLineDivideTwoPart(dstLine,dstpart1,dstpart2,divide2)) return 0;
	if(port >0 )
	{
		sprintf(line,"m=video %d RTP/AVP%s",port,dstpart2);
	}
	else
	{
		int len=strlen(divide);
		sprintf(line,"m=video%s",dstLine+len);
	}

	strcpy(msg,part1);
	strcat(msg,line);
	strcat(msg,part2);
	return 1;
}

int ChangeSIPMSGDualVideoPort(pjsip_msg * sipStruct,char * msgDstSDP,char * msgSrcSDP,char * divide,char * divide2,char * decname,int type)
{
	pjmedia_sdp_session *sdp =(pjmedia_sdp_session *) sipStruct->body->data;
	int nPort=0;
	unsigned int i=2;
	while(i<sdp->media_count)
	{
		if( (pj_stricmp(&(sdp->media[i]->desc.media),&(pj_str("video")))==0) )
		{
			nPort=sdp->media[i]->desc.port;
			break;
		}
		i++;
	}
	return SIPMsgLinePartUpdateMediaPort(msgDstSDP,msgDstSDP,msgSrcSDP,divide,divide2,decname,type,nPort);
}

int ChangeResponseSDP(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgSRC[SIPMSGMaxLen]={0};
	char msgDST[SIPMSGMaxLen]={0};
	char msgDstHeader[SIPMSGMaxLen]={0};
	char msgDstSDP[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	bool bReadOpen=false;
	{
		memcpy(msgSRC,msg,len);
		msgSRC[len]='\0';
		if(nUseFileMSG>0) bReadOpen=m_TextFileRead.Open(SIPReadFileName,"r");
		if(nRecordMsg>0) m_TextFileSave.Open(SIPRecordFileName,"w");
		if(bReadOpen)
		{
			ReadFileContent(msgDST);
		}
		else
		{
			return 0;
		}
		//process read file data ensure a line is end with \r\n 
		ProcessEnterLine(msgDST,1);
		int srclentest=strlen(msgSRC);
		int dstlentest=strlen(msgDST);
		//change SIP msg
		char * divide="Content-Length:";
		if(! SIPMsgDivideTwoPartDecLine(msgDST,msgDstHeader,msgDstSDP,divide) ) return 0;
		if(! SIPMsgDivideTwoPartDecLine(msgSRC,msgSrcHeader,msgSrcSDP,divide) ) return 0;
		//2 change sdp
		if(nChangeSDP)
		{
			if(! SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"o=","IN IP4","",1) ) return 0;
			if(! SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"c=IN IP4") ) return 0;
			SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=audio","RTP/AVP","",2);
			SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video","RTP/AVP","",2);
			ChangeSIPMSGDualVideoPort(sipStruct,msgDstSDP,msgSrcSDP,"m=video2","RTP/AVP","",2);
			//SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=application","RTP/AVP","",2);
			SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"a=rtcp");
		}
		//3 change header
		if(nChangeHeader)
		{
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"SIP/2.0") ) return 0;
			SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Via: SIP") ;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"From:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"To:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Contact:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Call-ID:") ) return 0;
			if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"CSeq:") ) return 0;
		}
		//4 header add
		if(nHeaderLineADD>0)
		{
			for(int j=0;j<nHeaderLineADD;j++)
			{
				strcat(msgDstHeader,SIPHeaderLineADD[j].Line);
				strcat(msgDstHeader,"\r\n");
			}
		}
		//5 sdp add
		if(nSDPLineADD>0)
		{
			char * divideName="m=audio";
			for(int j=0;j<nSDPLineADD;j++)
			{
				if(SIPSDPLineADD[j].nType==2) divideName="m=video";
				strcat(SIPSDPLineADD[j].Line,"\r\n");
				SIPMsgSDPLineADD(msgDstSDP,divideName,SIPSDPLineADD[j].Line);
			}
		}

		int nTotalLength=SIPMSGBuild(msg,msgDstSDP,msgDstHeader);
		//write msg file log to see
		if(nRecordMsg>0)
		{
			RecordMsgInfo(msg,srclentest,dstlentest,nTotalLength);
		}

		if(nUseFileMSG>0) m_TextFileRead.Close();
		if(nRecordMsg>0) m_TextFileSave.Close();
		return nTotalLength;
	}
}


int ChangeInviteSIPMSG(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgSRC[SIPMSGMaxLen]={0};
	char msgDST[SIPMSGMaxLen]={0};
	char msgDstHeader[SIPMSGMaxLen]={0};
	char msgDstSDP[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	char sipInvReadFileName[256]={0};
	sprintf(sipInvReadFileName,"INV%s",SIPReadFileName);
	bool bReadOpen=false;
	memcpy(msgSRC,msg,len);
	msgSRC[len]='\0';
	if(nUseFileMSG>0) bReadOpen=m_TextFileRead.Open(sipInvReadFileName,"r");
	if(nRecordMsg>0) m_TextFileSave.Open(SIPRecordFileName,"w");
	if(bReadOpen)
	{
		ReadFileContent(msgDST);
	}
	else
	{
		return 0;
	}
	//process read file data ensure a line is end with \r\n 
	ProcessEnterLine(msgDST,1);
	int srclentest=strlen(msgSRC);
	int dstlentest=strlen(msgDST);
	//change SIP msg
	char * divide="Content-Length:";
	if(! SIPMsgDivideTwoPartDecLine(msgDST,msgDstHeader,msgDstSDP,divide) ) return 0;
	if(! SIPMsgDivideTwoPartDecLine(msgSRC,msgSrcHeader,msgSrcSDP,divide) ) return 0;
	//2 change sdp
	if(nChangeSDP)
	{
		if(! SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"o=","IN IP4","",1) ) return 0;
		if(! SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"c=IN IP4") ) return 0;
		SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=audio","RTP/AVP","",2);
		SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=video","RTP/AVP","",2);
		ChangeSIPMSGDualVideoPort(sipStruct,msgDstSDP,msgSrcSDP,"m=video2","RTP/AVP","",2);
		//SIPMsgLinePartUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"m=application","RTP/AVP","",2);
		SIPMsgLineUpdate(msgDstSDP,msgDstSDP,msgSrcSDP,"a=rtcp");
	}
	//3 change header
	if(nChangeHeader)
	{
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"INVITE sip:") ) return 0;
		SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Via: SIP") ;
		SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Max-Forwards:");
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"From:") ) return 0;
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"To:") ) return 0;
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Contact:") ) return 0;
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Call-ID:") ) return 0;
		if(! SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"CSeq:") ) return 0;
		SIPMsgLineUpdate(msgDstHeader,msgDstHeader,msgSrcHeader,"Proxy-Authorization:");
	}
	//4 header add
	if(nHeaderLineADD>0)
	{
		for(int j=0;j<nHeaderLineADD;j++)
		{
			strcat(msgDstHeader,SIPHeaderLineADD[j].Line);
			strcat(msgDstHeader,"\r\n");
		}
	}
	//5 sdp add
	if(nSDPLineADD>0)
	{
		char * divideName="m=audio";
		for(int j=0;j<nSDPLineADD;j++)
		{
			if(SIPSDPLineADD[j].nType==2) divideName="m=video";
			strcat(SIPSDPLineADD[j].Line,"\r\n");
			SIPMsgSDPLineADD(msgDstSDP,divideName,SIPSDPLineADD[j].Line);
		}
	}

	int nTotalLength=SIPMSGBuild(msg,msgDstSDP,msgDstHeader);
	//write msg file log to see
	if(nRecordMsg>0)
	{
		RecordMsgInfo(msg,srclentest,dstlentest,nTotalLength);
	}

	if(nUseFileMSG>0) m_TextFileRead.Close();
	if(nRecordMsg>0) m_TextFileSave.Close();
	return nTotalLength;
}

int ChangeRegisterSIPMSG(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgSRC[SIPMSGMaxLen]={0};
	char msgDST[SIPMSGMaxLen]={0};
	char msgDstHeader[SIPMSGMaxLen]={0};
	char msgDstSDP[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	int regLen=0;
	regLen=SIPMsgPartBuild(msg,msg,"Route:");
	return regLen;
}

int Change200OKSIPMSG(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgBuf[SIPMSGMaxLen]={0};
	char destline[SIPMSGMaxLen/4]={0};
	char part1[SIPMSGMaxLen]={0};
	char part2[SIPMSGMaxLen]={0};
	char newline[SIPMSGMaxLen/4]={0};
	char * msgPn1=NULL;
	char * msgPn2=NULL;
	int nDestLineEndPos=0;
	strcpy(msgBuf,msg);
	if(! SIPGetMsgKeyNameLine(msgBuf,destline,"Via:",nDestLineEndPos) ) return 0;
	SIPMsgDivideTwoPartDecLine(msgBuf,part1,part2,"Via:");
	strcpy(newline,destline);
	msgPn1=strstr(destline,"received=");
	msgPn2=strstr(destline,"branch=");
	if(msgPn1 && msgPn2)
	{
		strncpy(newline,destline,msgPn1-destline);
		newline[msgPn1-destline] = '\0';
		strcat(newline,msgPn2);
	}
	strcpy(msg,part1);
	strcat(msg,newline);
	strcat(msg,part2);
	int totalLength=strlen(msg);
	return totalLength;
}

int SIPMSGRemoveMediaPortZeroLine(char * msg,int len,pjsip_msg * sipStruct)
{
	char msgBuf[SIPMSGMaxLen]={0};
	char msgSrcHeader[SIPMSGMaxLen]={0};
	char msgSrcSDP[SIPMSGMaxLen]={0};
	char *	msgPn=NULL;
	int nTotalLen=0;
	strncpy(msgBuf,msg,len);
	msgPn=strstr(msgBuf,"m=video 0 RTP/AVP");
	if(msgPn)
	{
		nTotalLen=msgPn-msgBuf;
		msgBuf[nTotalLen]='\0';
		char * divide="Content-Length:";
		if(! SIPMsgDivideTwoPartDecLine(msgBuf,msgSrcHeader,msgSrcSDP,divide) ) return len;
		int nTotalLength=SIPMSGBuild(msg,msgSrcSDP,msgSrcHeader);
		return nTotalLength;
	}
	else return len;
}






CFileIO::CFileIO()
: m_fp(NULL)
, m_iLength(0)
, m_nfflusherror(0)
{

}

CFileIO::~CFileIO()
{

}

bool CFileIO::Open(const char* strFileName, const char* strFlag)
{
	if (m_fp)
	{
		return false;
	}
	fopen_s(&m_fp,strFileName, strFlag);
	m_iLength = 0;

	return m_fp ? true : false;
}

void CFileIO::Close()
{
	//CAVAutoLock lock(&m_avDataFrame);
	if (m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}
}

int CFileIO::Write(char* pData, int nLen)
{
	if (m_nfflusherror != 0)
	{
		return 0;
	}
	//CAVAutoLock lock(&m_avDataFrame);
	if (m_fp)
	{
		m_iLength += nLen;
		if (m_iLength >= m_fp->_bufsiz)
		{
			m_nfflusherror = fflush(m_fp);			
			m_iLength = 0;
		}
		if (m_nfflusherror != 0)
		{
			//OutputDebugStringA("fflush error \n");
		}
		int nWriteLen = (int)fwrite(pData, 1, nLen, m_fp);
		return nWriteLen;
	}
	return 0;
}

int CFileIO::Read(char* pData, int nReadLen)
{
	if (m_fp)
	{
		return fread(pData, 1, nReadLen, m_fp);
	}
	return 0;
}

unsigned long CFileIO::GetCurPos()
{
	if (m_fp)
	{
		fpos_t pos;

		if (fgetpos(m_fp, &pos) == 0)
		{
			return pos;
		}
	}

	return 0;
}

int CFileIO::Seek(unsigned long ulPos, int nOrigin)
{
	if (m_fp)
	{
		return fseek(m_fp, ulPos, nOrigin);
	}
	return -1;
}

bool CFileIO::IsFileEnd()
{	
	bool bEnd=false;
	if (m_fp)
	{
		if(feof(m_fp)) bEnd=true;
	}
	return bEnd;
}
