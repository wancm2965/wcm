

#ifndef _HPVOD_H
#define _HPVOD_H


//¼���ļ�ɨ��
_declspec(dllexport) void HPVOD_ScanFiles(const char* cszServerIPAddress, const char* cszServerName);

//¼���ļ��ϴ�
AFX_EXT_CLASS void HPVOD_UploadFiles(const char* cszServerIPAddress, const char* cszRemotePath);


#endif
