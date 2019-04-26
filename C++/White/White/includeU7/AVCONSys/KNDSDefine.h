/*
	KNDSDefine.h
	author: lhp
*/

#pragma once

// ndsagent 存储的文件信息
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
		_strFileType		= ""; //诊断图片的类型为"DIAGNOSE"
    }

	unsigned short	_usTeam;			// 是否为群组使用
	unsigned long   _ulSize;            // 文件大小(KBytes)
	unsigned long   _ulFileID;          // NETEC_FileNodeTX 生成的 file id
	std::string     _strMCUID;          // MCUID(文件存放的MCU，即接收方的)
	std::string     _strMCUNodeID;      // MCUNodeID
	std::string     _strFileName;       // 文件名称(上传时全路径，下载时是回调给的，不必再自填)
	std::string     _strSenderID;       // 发送者ID(带域名)
	std::string     _strReceiverID;     // 接收者ID(带域名)
	std::string     _strSenderName;     // 发送者名称
	std::string     _strDesc;           // 文件描述
	std::string     _strFileMD5;        // 文件MD5
	std::string	    _strUpLoadDate;		// 文件上传日期

	//诊断信息的参数
	std::string		_strUserID;
	std::string		_strDevID; 
	std::string		_strDevName;
	std::string		_strChannelID; 
	std::string		_strChannelName;
	std::string		_strDiagnoseDate;
	std::string		_strDiagnoseType;
	std::string		_strFileType; //诊断图片的类型为"DIAGNOSE"
	
}TNDSFileInfo, *PTNDSFileInfo;

// NETEC 库接收文件所需的信息
typedef struct tagNETECFileRXInfo
{
    tagNETECFileRXInfo()
    {
        _ulLength = 0;
        _ulFileID = 0;
    }

    unsigned long   _ulFileID;          // NETEC_FileNodeTX 生成的 file id
    unsigned long   _ulLength;          // 文件大小(Bytes)，注意，要填 Bytes
    std::string     _strNodeID;         // 发送者NodeID
    std::string     _strMCUID;          // MCUID(自己的MCUID，即发送方的)
    std::string     _strMCUIP;          // MCUIP(自己的MCUID，即发送方的)
    std::string     _strMCUType;        // MCUType(自己的MCUID，即发送方的)
    unsigned short  _wMCUPort;          // MCUPort(自己的MCUID，即发送方的)
}TNETECFileRXInfo, *PTNETECFileRXInfo;

