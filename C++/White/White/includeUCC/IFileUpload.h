//========================================================================================
//��Ȩ���У���Ȩ����(C) 2009���Ϻ���ƽ��������޹�˾
//ϵͳ���ƣ�fud
//�ļ�������IFileUpload
//�������ߣ�lhp
//�������ڣ�2010-03-19
//����˵���������˱���¼���ļ��ϴ����������Ľӿ�
//          �ṩ��֪ͨ������޸��ļ�״̬�Ľӿ�
//
//---------------------------------------------------------------------------------------
//�޸ļ�¼��01
//�޸��ˣ�  lhp
//�޸�ʱ�䣺2010-03-19
//�޸����ݣ�����
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
    string          _strFileName;       // �ļ����ƣ�����ȫ·������������������+ʱ�䣻��������+�û�����+ʱ�䣩
	string          _strDesc;           // �ļ��������������ƣ�
    string          _strStartTime;      // ��ʼʱ��
	string			_strEndTime;		// ����ʱ��
    unsigned short  _usDuration;        // �ļ�ʱ��
    unsigned long   _usSize;            // �ļ���С

    string          _strDisplay1;       // �ֱ���1
    unsigned short  _usFps1;            // ֡��1
    unsigned short  _usBps1;            // ����1

    string          _strDisplay2;       // �ֱ���2
    unsigned short  _usFps2;            // ֡��2
    unsigned short  _usBps2;            // ����2
}TSendFileInfo, *PTSendFileInfo;


/************************************************************************/
/* �ӿڶ���                                                             */
/************************************************************************/
class IFileUploadInterface
{
public:
    IFileUploadInterface(){}
    virtual ~IFileUploadInterface(){}
public:
    //�����ļ��洢��������ַ
    virtual void SetFSvrInfo( const string& sSvrIp, int nPort) = 0;

    //�����ļ������ô˽ӿڶ�Σ��ɴ������ļ�
    virtual void SendFile( PTSendFileInfo ptSendFileInfo) =0;

    // ֹͣ�ϴ��ļ����� ��ʾֹͣ���е��ϴ�
    virtual void StopSendFile(const string& strFileName="") =0;

    // ֪ͨ������޸�¼���ļ�״̬
};

class IFileUploadCallback
{
public:
	enum ESendErr{ 
        E_FILEISSENDING,	    // �ļ������ϴ���
        E_NETERROR,			    // �������
        E_DISKFULL,             // ������������
        E_LOCALFILEOPRERR,      // �����ļ�����ʧ��(��/��/д/seek)
        E_SERVERFILEOPRERR,     // �������ļ�����ʧ��(��/��/д/seek)
        E_MD5CHECKERR,          // �ļ� MD5 ��֤ʧ��
    };

public:
    IFileUploadCallback(){}
    virtual ~IFileUploadCallback(){}

public:
    // �ļ��ϴ��ɹ�
    virtual void OnFileUploadEnd(const string& strFileName) = 0;

    // �ļ��ϴ�ʧ��
    virtual void OnFileUploadError(const string& strFileName, ESendErr nErrorID) = 0;
};

/************************************************************************/
/* API                                                                  */
/************************************************************************/
AFX_EXT_CLASS bool InitFileUploadInstance( IFileUploadCallback* pFileUploadCallback);
AFX_EXT_CLASS IFileUploadInterface * GetFileUploadInstance();
AFX_EXT_CLASS void ReleaseFileUploadInstance();


