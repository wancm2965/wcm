/*
	KNDSDefine.h
	author: lhp
*/

#pragma once

#include "KBASE.h"

// ndsagent 存储的文件信息
typedef struct tagNDSFileInfo
{
    tagNDSFileInfo()
    {
        _ulSize = 0;
		_usTeam = 0;
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
}TNDSFileInfo, *PTNDSFileInfo;

// NETEC 库接收文件所需的信息
typedef struct tagNETECFileRXInfo
{
    tagNETECFileRXInfo()
    {
        _ulLength = 0;
    }

    unsigned long   _ulFileID;          // NETEC_FileNodeTX 生成的 file id
    unsigned long   _ulLength;          // 文件大小(Bytes)，注意，要填 Bytes
    std::string     _strNodeID;         // 发送者NodeID
    std::string     _strMCUID;          // MCUID(自己的MCUID，即发送方的)
    std::string     _strMCUIP;          // MCUIP(自己的MCUID，即发送方的)
    std::string     _strMCUType;        // MCUType(自己的MCUID，即发送方的)
    unsigned short  _wMCUPort;          // MCUPort(自己的MCUID，即发送方的)
}TNETECFileRXInfo, *PTNETECFileRXInfo;

//---------------------------------------------------------------------------------------
inline std::string ansi2utf8( string strAnsi);
inline std::string utf82ansi( string strUtf8);

//////////////////////////////////////////////////////////////////////////
// 实用内联函数
inline void CMDPacket( KCmdPacket& t, PTNDSFileInfo ptNDSFileInfo, bool bTrans2utf8 = false)
{
    string strFileName = bTrans2utf8 ? ansi2utf8(ptNDSFileInfo->_strFileName) : ptNDSFileInfo->_strFileName;
    t.SetAttribUL( "SIZE", ptNDSFileInfo->_ulSize);
    t.SetAttribUL( "NETECFILEID", ptNDSFileInfo->_ulFileID);

    t.SetAttrib( "MCUID", ptNDSFileInfo->_strMCUID);
    t.SetAttrib( "MCUNODEID", ptNDSFileInfo->_strMCUNodeID);
    t.SetAttrib( "FILENAME", strFileName);
    t.SetAttrib( "SENDERID", ptNDSFileInfo->_strSenderID);
    t.SetAttrib( "RECEIVERID", ptNDSFileInfo->_strReceiverID);
    t.SetAttrib( "SENDERNAME", ptNDSFileInfo->_strSenderName);
    t.SetAttrib( "DESC", ptNDSFileInfo->_strDesc);
    t.SetAttrib( "FILEMD5", ptNDSFileInfo->_strFileMD5);
	t.SetAttrib( "UPLOADDATE", ptNDSFileInfo->_strUpLoadDate);
	t.SetAttribUS( "TEAM", ptNDSFileInfo->_usTeam);
}

inline void CMDUnPacket( KCmdPacket& t, PTNDSFileInfo ptNDSFileInfo, bool bTrans2Ansi = false)
{
    ptNDSFileInfo->_ulSize = t.GetAttrib("SIZE").AsUnsignedLong();
    ptNDSFileInfo->_ulFileID = t.GetAttrib("NETECFILEID").AsUnsignedLong();

    ptNDSFileInfo->_strMCUID = t.GetAttrib( "MCUID").AsString();
    ptNDSFileInfo->_strMCUNodeID = t.GetAttrib( "MCUNODEID").AsString();
    ptNDSFileInfo->_strFileName = t.GetAttrib( "FILENAME").AsString(); 
    ptNDSFileInfo->_strSenderID = t.GetAttrib( "SENDERID").AsString(); 
    ptNDSFileInfo->_strReceiverID = t.GetAttrib( "RECEIVERID").AsString(); 
    ptNDSFileInfo->_strSenderName = t.GetAttrib( "SENDERNAME").AsString(); 
    ptNDSFileInfo->_strDesc = t.GetAttrib( "DESC").AsString(); 
    ptNDSFileInfo->_strFileMD5 = t.GetAttrib( "FILEMD5").AsString();
	 ptNDSFileInfo->_strUpLoadDate = t.GetAttrib( "UPLOADDATE").AsString();
    if ( bTrans2Ansi )
    {
        ptNDSFileInfo->_strFileName = utf82ansi(ptNDSFileInfo->_strFileName);
    }
	ptNDSFileInfo->_usTeam = t.GetAttrib("TEAM").AsUnsignedShort();
}

inline void CMDPacket( KCmdPacket& t, PTNETECFileRXInfo ptNETECFileRXInfo)
{
    t.SetAttribUL( "NETECFILEID", ptNETECFileRXInfo->_ulFileID);
    t.SetAttribUL( "FILELENGTH", ptNETECFileRXInfo->_ulLength);

    t.SetAttrib( "NODEID", ptNETECFileRXInfo->_strNodeID);
    t.SetAttrib( "MCUID", ptNETECFileRXInfo->_strMCUID);
    t.SetAttrib( "MCUIP", ptNETECFileRXInfo->_strMCUIP);
    t.SetAttrib( "MCUTYPE", ptNETECFileRXInfo->_strMCUType);

    t.SetAttribUS( "MCUPORT", ptNETECFileRXInfo->_wMCUPort);
}

inline void CMDUnPacket( KCmdPacket& t, PTNETECFileRXInfo ptNETECFileRXInfo)
{
    ptNETECFileRXInfo->_ulFileID = t.GetAttrib( "NETECFILEID").AsUnsignedLong();
    ptNETECFileRXInfo->_ulLength = t.GetAttrib( "FILELENGTH").AsUnsignedLong();

    ptNETECFileRXInfo->_strNodeID = t.GetAttrib( "NODEID").AsString();
    ptNETECFileRXInfo->_strMCUID = t.GetAttrib( "MCUID").AsString();
    ptNETECFileRXInfo->_strMCUIP = t.GetAttrib( "MCUIP").AsString();
    ptNETECFileRXInfo->_strMCUType = t.GetAttrib( "MCUTYPE").AsString();

    ptNETECFileRXInfo->_wMCUPort = t.GetAttrib( "MCUPORT").AsUnsignedShort();
}

#ifdef WIN32                    //----------------------------------------------------
//---------------------------------------------------------------------------------------
inline std::string CSTR2STR(CString csString)
{
    USES_CONVERSION;
    LPSTR lpszString = W2A(csString.LockBuffer());
    csString.UnlockBuffer();

    return lpszString;
}

//---------------------------------------------------------------------------------------
inline CString STR2CSTR(const std::string& strString)
{
    USES_CONVERSION;
    CString csString;
    csString = A2T(strString.c_str());
    return csString;
}

//---------------------------------------------------------------------------------------
inline std::string ansi2utf8( string strAnsi)
{
    return U2A( STR2CSTR(strAnsi));
}

//---------------------------------------------------------------------------------------
inline std::string utf82ansi( string strUtf8)
{
    return CSTR2STR( A2U(strUtf8));
}

#else                           //----------------------------------------------------
inline std::string ansi2utf8( string strAnsi)
{
    return strAnsi;
}

inline std::string utf82ansi( string strUtf8)
{
    return strUtf8;
}

#endif                          //----------------------------------------------------

