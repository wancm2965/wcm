#ifndef _HPMRS_H__
#define _HPMRS_H__
class LocalServer;
bool CreateHPMRS(LocalServer **ppInstance, const char *cfgfile);
void DestroyHPMRS(LocalServer *pInstance);
void XNETIOUninit();
void XNETIOInit();
#endif
