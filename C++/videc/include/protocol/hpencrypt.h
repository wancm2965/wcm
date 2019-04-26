#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_
#include "hptype.h"
//#include "osp.h"

#ifdef WIN32
#include <winsock2.h>
#endif

#define HPENCRYPTVERSION		(const char*)"hpencrypt 40.01.02 071203"

void hpencryptver(); 
void HpGetEncryptVer(char** ppEncryptVer);

typedef long*  verylong;



/**************************��������*******************************************/

/*=============================================================================
������		��StrToNumber
����		�����ִ�ת��Ϊ������
�㷨ʵ��	������ѡ�
�������˵���� 
            pszText   ��ʮ����������ʾ���ַ���("user"->"75736572")  
            pvlNum    Ҫ����Ĵ�����
����ֵ˵����bool
=============================================================================*/
bool StrToNumber(char *pszText, verylong *pvlNum);

/*=============================================================================
������		��NumberToStr
����		����������ת��Ϊ�ִ�
�㷨ʵ��	������ѡ�
�������˵���� 
            pszText   ��ʮ����������ʾ���ַ���("user"->"75736572")  
            pvlNum    ����Ĵ�����
����ֵ˵����bool
=============================================================================*/
bool NumberToStr(char *pszText, verylong vlNum);

bool HpVerylongNumFree();

bool HpFreeKarMem();

/*=============================================================================
������		��HpExpMod
����		��bb = (a^e) mod n
����ֵ˵����bool
=============================================================================*/
bool HpExpMod(verylong a,  verylong e,   verylong n, verylong *bb); 

bool HpRandomPrime(long lBits, verylong *bb);

void vlFree(verylong *a);


inline void hpstrrev(char *s)
{
	if(*s==0) return;
	char *p=s;
	char c;
	while(*(++p)); p--;
	while(p>s)
	{
		c =*p;
		*p--=*s;
		*s++=c;
	}
}
/*****************************************************************************/

/**********************des****************************************************/
/* what */
typedef enum QFDES_what {qfDES_encrypt, qfDES_decrypt} QFDES_what;

/* mode */
typedef enum QFDES_mode {qfDES_ecb, qfDES_cbc, qfDES_cfb, qfDES_ofb} QFDES_mode;

void HpDES(u8	*key, u8 *data,  u32 size,  
		  const QFDES_what what,
		  const QFDES_mode mode,
		  u8 *initVec);

bool IsWeakDesKey(u8 *key);
/****************************************************************************/

/**********************AES****************************************************/

/*  Generic Defines  */
#define     DIR_ENCRYPT           0 /*  Are we encrpyting?  */
#define     DIR_DECRYPT           1 /*  Are we decrpyting?  */
#define     MODE_ECB              1 /*  Are we ciphering in ECB mode?   */
#define     MODE_CBC              2 /*  Are we ciphering in CBC mode?   */
#define     MODE_CFB1             3 /*  Are we ciphering in 1-bit CFB mode? */

/*  Algorithm-specific Defines  */
#define     MAX_KEY_SIZE         /*32*/64 /* # of ASCII char's needed to represent a key */
#define     MAX_IV_SIZE          16 /* # bytes needed to represent an IV  */

int HpAES(s8 *keyMaterial, s32 keyLen, u8 mode,u8 direction,s8 *IV, 
           u8 *input, s32 inputLen, u8 *outBuffer);

#endif
