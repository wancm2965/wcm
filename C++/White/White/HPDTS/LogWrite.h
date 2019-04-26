//========================================================================================
//��Ȩ���У���Ȩ����(C) 2010���Ϻ���ƽ��Ϣ�����ɷ����޹�˾
//ϵͳ���ƣ�AVCON6
//��������: zhu yuyuan
//�������ڣ�2010-12-21
//����˵��������������־������
//��־ÿ����һ���µ��ļ�������������������ʽ****_**_**.log

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <string>
#include "XMemIni.h"
#include "HPDTS/DTSCmdType.h"
#include "XAutoLock.h"
using namespace std;

//��־���ݽṹ��������־���ݺͳ���
typedef struct logNode
{
	char* str;
	//std::string str;
	int len;
}LOGNODE,*PLOGNODE;

//����һ������������־���ݣ��������ڵ���Ŀ����һ��ֵʱ����д���ļ�
typedef std::vector<LOGNODE> LogBuff;
typedef LogBuff::iterator IT_LogBuff;

class LogWrite
{
public:
	LogWrite();
	//�����ֱ�Ϊ����־�ļ�·����һ�λ���������Ŀ���Ƿ����־д��
	LogWrite(std::string logPath, int MaxBuff, bool isOpen);
	virtual ~LogWrite(void);

private:
	bool WriteLog();
	bool InitDirectory();

public:
	//��������õ�д����־����
	int Write(const char* strText, const int len);
	int Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen);
	int Write(const char* strEvent,const int eventLen,const char* strData, const int dataLen, bool isPacket);

	//��ȡ��������־�ļ�·��
	std::string GetLogDirectory();
	void SetLogDirectory(std::string logPath);
	//��ȡ��������󻺳���
	int GetMaxBuffCount();
	void SetMaxBuffCount(int maxBuff);
	//��ȡ�����ÿ���״̬
	bool GetOpenState();
	void SetOpenState(bool isOpen);
	std::string GetSystemDate();
	std::string GetSystemTime();
private:
	fstream f;            //�ļ����������
	std::string m_strLogDrectory;  //������־�ļ�·��
	int m_nMaxBuff;		  //������󻺳���
	int m_nBuffCount;	  //��ǰ�����еĽڵ���
	bool m_bIsOpen;		  //�Ƿ����־����
	LogBuff m_logBuff;	  //��־����

	XCritSec m_csVectorLogBuff;
	//XCritSec m_csWriteBuff;
};




