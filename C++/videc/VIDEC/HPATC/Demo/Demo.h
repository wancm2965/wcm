#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment(lib,"../lib/Debug/VIDECMP4.lib")
#pragma comment(lib,"../lib/Debug/NETEC.lib")
#pragma comment(lib,"../lib/Debug/HPATC.lib")
#else
#pragma comment(lib,"../lib/Release/VIDECMP4.lib")
#pragma comment(lib,"../lib/Release/NETEC.lib")
#pragma comment(lib,"../lib/Release/HPATC.lib")
# endif

#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#endif


