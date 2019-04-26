

#ifndef _HPVOD_H
#define _HPVOD_H


//录像文件扫描
_declspec(dllexport) void HPVOD_ScanFiles(const char* cszServerIPAddress, const char* cszServerName);

//录像文件上传
AFX_EXT_CLASS void HPVOD_UploadFiles(const char* cszServerIPAddress, const char* cszRemotePath);


#endif
