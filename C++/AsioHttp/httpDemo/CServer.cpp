#include "stdafx.h"
#include "CServer.h"
#include <string>
#include "jsonconfig.h"

//#include "ServerSession.h"
#include "Task.h"

using namespace std;
void Base64Decode(const std::string& InBuffer, std::string& OutBuffer)
{
	if (InBuffer.length() % 4)
	{
		return;
	}

	const char DECODETAB[] =
	{
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  62,   0,   0,   0,  63,
		52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,   0,   0,   0,
		0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
		15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,   0,   0,   0,
		0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
		41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   0,   0,   0,   0,   0
	};

	size_t i = 0;
	size_t l = InBuffer.length();

	OutBuffer = "";
	while (i < l)
	{
		while (i < l && (InBuffer[i] == 13 || InBuffer[i] == 10))
		{
			i++;
		}
		if (i < l)
		{
			char b1 = (char)((DECODETAB[(int)InBuffer[i]] << 2 & 0xfc) + (DECODETAB[(int)InBuffer[i + 1]] >> 4 & 0x03));
			OutBuffer += b1;
			if (InBuffer[i + 2] != '=')
			{
				char b2 = (char)((DECODETAB[(int)InBuffer[i + 1]] << 4 & 0xf0) + (DECODETAB[(int)InBuffer[i + 2]] >> 2 & 0x0f));
				OutBuffer += b2;
			}
			if (InBuffer[i + 3] != '=')
			{
				char b3 = (char)((DECODETAB[(int)InBuffer[i + 2]] << 6 & 0xc0) + DECODETAB[(int)InBuffer[i + 3]]);
				OutBuffer += b3;
			}
			i += 4;
		}
	}
}

void Base64DecodeToFile(const std::string& encodedstr, const std::string& filename)
{
	std::string decodedstr = "";
	Base64Decode(encodedstr, decodedstr);
	int decodedlen = decodedstr.length();
	const char* pData = decodedstr.c_str();
	FILE* fp = ::fopen(filename.c_str(), "wb");
	if (fp == NULL)
		return;

	for (int i = 0; i < decodedlen; i++)
	{
		::fwrite((char*)pData + i, sizeof(char), 1, fp);
	}
	::fclose(fp);
}
CServer::CServer()
{
	m_pHttpServer = NULL;
}


CServer::~CServer()
{
}

bool CServer::OnAPItrigger(void*handler, const char*api, const char* param, char**pReply)
{
	int n = 0;
	printf("OnAPItrigger is %s handler is %d \n",api,handler);
	std::string strParam(param);
	if (strParam.empty())
	{		
		return false;
	}
	std::string strApiKey = CJsonConfig::Ins().GetValue(strParam, "apikey", false);
	std::string strrecnum = CJsonConfig::Ins().GetValue(strParam, "recnum", false);
	std::string strthreshold = CJsonConfig::Ins().GetValue(strParam, "threshold", false);
	std::string strdatatype = CJsonConfig::Ins().GetValue(strParam, "datatype", false);
	std::string strdataparam = CJsonConfig::Ins().GetValue(strParam, "dataparam", true);
	if (strdatatype.compare("pic") == 0)
	{
		std::string strrecdata = CJsonConfig::Ins().GetValue(strdataparam, "recdata", false);
		std::string strdatalen = CJsonConfig::Ins().GetValue(strdataparam, "datalen", false);
		std::string strwidth = CJsonConfig::Ins().GetValue(strdataparam, "width", false);
		std::string strheight = CJsonConfig::Ins().GetValue(strdataparam, "height", false);
		std::string strFilePath = "E:\\123.jpg";
		Base64DecodeToFile(strrecdata, strFilePath);
	}

	int nCount;
	nCount = rand() % 100;
	CServerSession serversession;
	CTask task;
	task.SaveSession(&serversession);
	task.SaveCount(nCount);
	task.SaveServer(this, handler);
	task.StartTask();
	std::string strRel = "now processing";
	AsyncReply(handler, strRel.c_str());
	//serversession.StartWait();
	/*for (int n = 0; n< 10; n++)
	{	
		std::string strRel = "now processing"+std::to_string(n);
		AsyncReply(handler, strRel.c_str());
	}*/
	
	char bufReply[256] = { 0 };
	sprintf(bufReply, "I Get param and I do work over count is %d", nCount);
	std::string strReply(bufReply);

	printf("handler %d ,Reply is %s \n", handler, strReply.c_str());

	char* strTemp = new char[strReply.length() + 1];
	strcpy_s(strTemp, strReply.length() + 1, strReply.c_str());
	*pReply = strTemp;
	return true;
}

bool CServer::StartServer()
{
	m_pHttpServer = IHttpServer::Create(this);
	if (m_pHttpServer)
	{
		std::string strIp = "0.0.0.0";

		std::string strPath = "/ai/startrecogntion";
		m_pHttpServer->RegAPIpath(strPath.c_str());
		m_pHttpServer->StartHttpServer(strIp.c_str(), 5667);
	}
	return true;
}

bool CServer::StopServer()
{
	if (m_pHttpServer)
	{
		m_pHttpServer->StopHttpServer();
		delete m_pHttpServer;
		m_pHttpServer = NULL;
	}
	return true;
}

void CServer::AsyncReply(void* handler,const char* reply)
{
	if (m_pHttpServer)
	{
		m_pHttpServer->AsyncReply(handler, reply);
	}
}
