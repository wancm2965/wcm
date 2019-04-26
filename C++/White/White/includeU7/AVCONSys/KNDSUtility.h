/*
	KNDSDefine.h
	author: lhp
*/
#include "KNDSDefine.h"
#pragma once

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

	//诊断信息的参数
	t.SetAttrib("DIAGNOSEUSERID",ptNDSFileInfo->_strUserID);
	t.SetAttrib("DEVID",ptNDSFileInfo->_strDevID);
	t.SetAttrib("DEVNAME",ptNDSFileInfo->_strDevName);
	t.SetAttrib("CHANNELID",ptNDSFileInfo->_strChannelID);
	t.SetAttrib("CHANNELNAME",ptNDSFileInfo->_strChannelName);
	t.SetAttrib("DIAGNOSEDATE",ptNDSFileInfo->_strDiagnoseDate);
	t.SetAttrib("DIAGNOSETYPE",ptNDSFileInfo->_strDiagnoseType);
	t.SetAttrib("FILETYPE",ptNDSFileInfo->_strFileType);
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

	//诊断信息的参数
	ptNDSFileInfo->_strUserID = t.GetAttrib("DIAGNOSEUSERID").AsString();
	ptNDSFileInfo->_strDevID = t.GetAttrib("DEVID").AsString();
	ptNDSFileInfo->_strDevName = t.GetAttrib("DEVNAME").AsString();
	ptNDSFileInfo->_strChannelID = t.GetAttrib("CHANNELID").AsString();
	ptNDSFileInfo->_strChannelName = t.GetAttrib("CHANNELNAME").AsString();
	ptNDSFileInfo->_strDiagnoseDate = t.GetAttrib("DIAGNOSEDATE").AsString();
	ptNDSFileInfo->_strDiagnoseType = t.GetAttrib("DIAGNOSETYPE").AsString();
	ptNDSFileInfo->_strFileType = t.GetAttrib("FILETYPE").AsString();
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

