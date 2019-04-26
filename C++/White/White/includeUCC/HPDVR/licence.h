#ifndef __LICENCE_H__
#define __LICENCE_H__

#define MAX_LICENCE_STR_LEN			17
#define MAX_MAC_LEN					6

//返回值  0: 检测通过
//返回值  1: 系列号无效
//返回值  2: 日期过期
//返回值  3: 没有软件狗
//返回值  4: 系列号空或者系列号长度不对(16位)
//返回值  5: MAC地址类型不对
extern "C" __declspec(dllexport) int CheckLicence(
			unsigned char licence_str[MAX_LICENCE_STR_LEN],
			unsigned char mac_addr[MAX_MAC_LEN]);

//返回值 -1: 没有插软件狗
//返回值  N: 通道数N
extern "C" __declspec(dllexport) int GetDVRCHCount(void);

#endif
