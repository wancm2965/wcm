#ifndef _HP_TYPE_H_
#define _HP_TYPE_H_


typedef char			s8;
typedef unsigned char	u8;
typedef short			s16;
typedef unsigned short	u16;
typedef int				s32,	BOOL32;
typedef unsigned int	u32;

#ifdef _MSC_VER
typedef __int64			s64;
#else 
typedef long long		s64;
#endif 

#ifdef _MSC_VER
typedef unsigned __int64	u64;
#else 
typedef unsigned long long	u64;
#endif

#ifndef _MSC_VER
#ifndef LPSTR
#define LPSTR   char *
#endif
#ifndef LPCSTR
#define LPCSTR  const char *
#endif
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif


#endif // _HP_TYPE_H_