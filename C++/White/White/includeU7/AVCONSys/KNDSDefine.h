/*
	KNDSDefine.h
	author: lhp
*/

#pragma once

// ndsagent �洢���ļ���Ϣ
typedef struct tagNDSFileInfo
{
    tagNDSFileInfo()
    {
        _ulSize = 0;
		_usTeam = 0;
        _ulFileID = 0;

		_strUserID			= "";
		_strDevID			= ""; 
		_strDevName			= "";
		_strChannelID		= ""; 
		_strChannelName		= "";
		_strDiagnoseDate	= "";
		_strDiagnoseType	= "";
		_strFileType		= ""; //���ͼƬ������Ϊ"DIAGNOSE"
    }

	unsigned short	_usTeam;			// �Ƿ�ΪȺ��ʹ��
	unsigned long   _ulSize;            // �ļ���С(KBytes)
	unsigned long   _ulFileID;          // NETEC_FileNodeTX ���ɵ� file id
	std::string     _strMCUID;          // MCUID(�ļ���ŵ�MCU�������շ���)
	std::string     _strMCUNodeID;      // MCUNodeID
	std::string     _strFileName;       // �ļ�����(�ϴ�ʱȫ·��������ʱ�ǻص����ģ�����������)
	std::string     _strSenderID;       // ������ID(������)
	std::string     _strReceiverID;     // ������ID(������)
	std::string     _strSenderName;     // ����������
	std::string     _strDesc;           // �ļ�����
	std::string     _strFileMD5;        // �ļ�MD5
	std::string	    _strUpLoadDate;		// �ļ��ϴ�����

	//�����Ϣ�Ĳ���
	std::string		_strUserID;
	std::string		_strDevID; 
	std::string		_strDevName;
	std::string		_strChannelID; 
	std::string		_strChannelName;
	std::string		_strDiagnoseDate;
	std::string		_strDiagnoseType;
	std::string		_strFileType; //���ͼƬ������Ϊ"DIAGNOSE"
	
}TNDSFileInfo, *PTNDSFileInfo;

// NETEC ������ļ��������Ϣ
typedef struct tagNETECFileRXInfo
{
    tagNETECFileRXInfo()
    {
        _ulLength = 0;
        _ulFileID = 0;
    }

    unsigned long   _ulFileID;          // NETEC_FileNodeTX ���ɵ� file id
    unsigned long   _ulLength;          // �ļ���С(Bytes)��ע�⣬Ҫ�� Bytes
    std::string     _strNodeID;         // ������NodeID
    std::string     _strMCUID;          // MCUID(�Լ���MCUID�������ͷ���)
    std::string     _strMCUIP;          // MCUIP(�Լ���MCUID�������ͷ���)
    std::string     _strMCUType;        // MCUType(�Լ���MCUID�������ͷ���)
    unsigned short  _wMCUPort;          // MCUPort(�Լ���MCUID�������ͷ���)
}TNETECFileRXInfo, *PTNETECFileRXInfo;

