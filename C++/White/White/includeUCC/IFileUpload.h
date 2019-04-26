//========================================================================================
//版权所有：版权所有(C) 2009，上海华平计算机有限公司
//系统名称：fud
//文件名：IFileUpload
//作　　者：lhp
//创建日期：2010-03-19
//功能说明：定义了本地录像文件上传到服务器的接口
//          提供了通知服务端修改文件状态的接口
//
//---------------------------------------------------------------------------------------
//修改记录：01
//修改人：  lhp
//修改时间：2010-03-19
//修改内容：创建
//---------------------------------------------------------------------------------------

#pragma once

typedef struct tagSendFileInfo
{
    tagSendFileInfo()
    {
        _usSize = 0;
        _usFps1 = 0;
        _usBps1 = 0;
        _usFps2 = 0;
        _usBps2 = 0;
    }
    string          _strFileName;       // 文件名称（本地全路径，命名规则：议名称+时间；会议名称+用户名称+时间）
	string          _strDesc;           // 文件描述（会议名称）
    string          _strStartTime;      // 开始时间
	string			_strEndTime;		// 结束时间
    unsigned short  _usDuration;        // 文件时长
    unsigned long   _usSize;            // 文件大小

    string          _strDisplay1;       // 分辨率1
    unsigned short  _usFps1;            // 帧率1
    unsigned short  _usBps1;            // 码率1

    string          _strDisplay2;       // 分辨率2
    unsigned short  _usFps2;            // 帧率2
    unsigned short  _usBps2;            // 码率2
}TSendFileInfo, *PTSendFileInfo;


/************************************************************************/
/* 接口定义                                                             */
/************************************************************************/
class IFileUploadInterface
{
public:
    IFileUploadInterface(){}
    virtual ~IFileUploadInterface(){}
public:
    //设置文件存储服务器地址
    virtual void SetFSvrInfo( const string& sSvrIp, int nPort) = 0;

    //传输文件，调用此接口多次，可传输多个文件
    virtual void SendFile( PTSendFileInfo ptSendFileInfo) =0;

    // 停止上传文件，空 表示停止所有的上传
    virtual void StopSendFile(const string& strFileName="") =0;

    // 通知服务端修改录像文件状态
};

class IFileUploadCallback
{
public:
	enum ESendErr{ 
        E_FILEISSENDING,	    // 文件正在上传中
        E_NETERROR,			    // 网络故障
        E_DISKFULL,             // 服务器磁盘满
        E_LOCALFILEOPRERR,      // 本地文件操作失败(打开/读/写/seek)
        E_SERVERFILEOPRERR,     // 服务器文件操作失败(打开/读/写/seek)
        E_MD5CHECKERR,          // 文件 MD5 验证失败
    };

public:
    IFileUploadCallback(){}
    virtual ~IFileUploadCallback(){}

public:
    // 文件上传成功
    virtual void OnFileUploadEnd(const string& strFileName) = 0;

    // 文件上传失败
    virtual void OnFileUploadError(const string& strFileName, ESendErr nErrorID) = 0;
};

/************************************************************************/
/* API                                                                  */
/************************************************************************/
AFX_EXT_CLASS bool InitFileUploadInstance( IFileUploadCallback* pFileUploadCallback);
AFX_EXT_CLASS IFileUploadInterface * GetFileUploadInstance();
AFX_EXT_CLASS void ReleaseFileUploadInstance();


