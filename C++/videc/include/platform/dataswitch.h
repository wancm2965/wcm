/*****************************************************************************
   ģ����      : DataSwitch
   �ļ���      : dataswitch.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: DataSwitchģ��ӿں�������
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
******************************************************************************/

#ifndef DATASWITCH_H
#define DATASWITCH_H

#define DSVERSION       "Dataswitch A7v0.1"



#include "ipc.h"

//�Ƿ�ʹ��epoll
#define DS_USE_EPOLL 1

/* DataSwitch ��� */
#define DSID    u32

/*DataSwitch ����ֵ*/
#define DSOK    1
#define DSERROR 0

#define INVALID_DSID  0xffffffff   /*��Ч�ľ��ֵ*/

/**
 * @func FilterFunc
 * @brief ���չ��˺���
 *
 * ÿ�����ս����һ�����˺��������˽����յ�UDP���ݰ�ʱִ�д˺�����
 * �����ݺ����ķ���ֵ������̬�ؾ����Ƿ�Դ����ݰ�ת����
 *
 * @param dwRecvIP        - ����IP
 * @param wRecvPort      - ����Port
 * @param dwSrcIP         - ԴIP
 * @param SrcPort       - ԴPort
 * @param pData          - [in, out]���ݰ��������޸ģ�
 * @param uLen           - [in, out]���ݰ����ȣ��޸ĺ�ĳ��Ȳ��ɳ������ֵ
 * @return 0, �����ݰ���Ч�� ����ֵ�������ݰ���Ч��
 */	
typedef u32 (*FilterFunc)(u32 dwRecvIP, u16 wRecvPort, u32 dwSrcIP, u16 wSrcPort, 
  u8* pData, u32 uLen);


#define SENDMEM_MAX_MODLEN        (u8)32        // ����ʱ���������޸ĳ���
    
/**
 * ���ݰ�ת��Ŀ�꣺ָ�������IP, port��
 * DS�����ƵĶ��IP���������ݰ���ת��Ӧ�ò�����Ӱ�졣Ҳ����˵�����
 * ������IP�����Խ�һ�����ݰ���ȷ�ĵ�ת����ȥ����ʹ����һ��IP��û��
 * ��ϵ�ġ����ԣ�ת������ӿ�IP�ǲ���Ҫ�ġ�
 */
typedef struct tagNetSndMember
{
    u32   dwIP;        /* ת��Ŀ��IP��ַ   */
    u16  wPort;       /* ת��Ŀ�Ķ˿ں�   */
    long            lIdx;        /* �ӿ����� */
    u32   errNum;      /* �������*/
    long            errNo;       /* ����� */
    void *          lpuser;      /* user info */
    /* Ϊʵ�ַ���ʱ�޸����ݶ����ӵĲ��� */
    u16  wOffset;        /* �޸Ĳ��ֵ���ʼλ��; Ŀǰδʹ�� */
    u16  wLen;		/* �޸Ĳ��ֵĳ��ȣ�����Ϊ4�ı�����Ϊ���ʾ���޸� */
    char            pNewData[SENDMEM_MAX_MODLEN];
    /* Ҫ�޸ĵ����� */
    void *          pAppData;       /* �û��Զ���Ļص������Ĳ��� */

}TNetSndMember;


typedef void (*SendCallback)(u32 dwRecvIP, u16 wRecvPort,
                             u32 dwSrcIP,  u16 wSrcPort,
                             TNetSndMember *ptSends,            // ת��Ŀ���б�
                             u16* pwSendNum,                    // ת��Ŀ����� 
                             u8* pUdpData, u32 dwUdpLen);


/************************************************************
 * ��������dsCreate
 *
 * ���ܣ�  ����DataSwitchģ��
 *
 * ʵ��˵���� 
 * Dataswitch֧�֡����û�����ÿ�ε���dsCreate�ͻ᷵��һ��
 * �û���ʶ����ӡ�ɾ��ʱ��Ҫƥ���û���־��ת��ʱ�򲻿��ǡ�
 *
 * ����ֵ�� 
 * @return INVALID_DSID: ��ʾʧ�� ; ����ɹ�
 ************************************************************/
API DSID dsCreate( u32, u32 * );

/************************************************************
 * ��������dsDestroy
 *
 * ���ܣ� ��ֹDataSwitch������
 *
 * ʵ��˵����
 * һ��ɾ��һ���û���ֱ�������û���ɾ����������Դ��ȫ�ͷš�
 *
 * ����˵����
 * @param dsId  - �û���ʶ
 * 
 * ����ֵ��
 ************************************************************/
API void dsDestroy( DSID dsId );
 
/************************************************************
 * �������� dsAdd
 *
 * ���ܣ� ����ת��Ŀ��Ψһ��ת������
 *
 * ʵ��˵����
 * ��ĿǰΨһ���ĺ����ǣ�����Ѿ����ڵĽ���������ת��Ŀ��
 * �뵱ǰ�������ͬ������ɾ����Щ����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwInLocalIP   - �������ݰ�������ӿ�
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 * @param dwOutLocalIP  - ת�����ݰ����ñ���IP
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAdd(DSID dsId,
              u32  dwRecvIP ,
              u16  wRecvPort,
              u32  dwInLocalIP,
              u32  dwSendtoIP,
              u16  wSendtoPort,
              u32  dwOutLocalIP = 0);

/************************************************************
 * ��������dsRemove
 *
 * ���ܣ� ɾ��ת��Ŀ��Ϊָ����ַ��ת������
 * 
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemove(DSID dsId,
                 u32  dwSendtoIP,
                 u16  wSendtoPort);

/************************************************************
 * ��������dsAddDump
 *
 * ���ܣ� ����Dump����
 * DUMP���򣺴�ָ����ַ���յ����ݰ�����ת����
 * ���һ�����յ�ַֻ��DUMP��������յ����ݰ�����ת����
 * �����������ת������������������ת����
 *
 * ʵ��˵����
 * DUMP��������������Σ���Ϊ�������������ǿ��Բ���ģ�
 * ����������
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwInLocalIP   - �������ݰ�������ӿ�
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAddDump(DSID dsId, u32 dwRecvIP, u16 wRecvPort, u32 dwInLocalIP);

/************************************************************
 * ��������dsRemoveDump
 *
 * ���ܣ� ɾ��Dump����
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveDump(DSID dsId, u32 dwRecvIP, u16 wRecvPort);

/************************************************************
 * ��������dsAddManyToOne
 * 
 * ���ܣ� ���Ӷ��һ��ת������
 * ע�⣬����dsAdd�����������෴�����߻��ʹ��ʱҪС�ġ�
 * 
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwInLocalIP   - �������ݰ�������ӿ�
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 * @param dwOutLocalIP  - ת�����ݰ����ñ���IP
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsAddManyToOne(DSID dsId ,
                       u32  dwRecvIP,
                       u16  wRecvPort,
                       u32  dwInLocalIP,
                       u32  dwSendtoIP,
                       u16  wSendtoPort,
                       u32  dwOutLocalIP = 0);

/************************************************************
 * ��������dsRemoveAllManyToOne
 * 
 * ���ܣ� ɾ������ת��Ŀ��Ϊָ����ַ�Ķ��һ����
 * 
 * ע�⣺����������dsRemove��ȫ��ͬ�����Դӽӿڲ�����֤����
 * �ṩ����ӿ���Ϊ�˲������಻ͬ����Ľӿڻ��ʹ�á�һ��
 * �����У�Ӧ��ʼ��ʹ��ĳһ��ӿڡ�
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveAllManyToOne(DSID dsId ,
                             u32  dwSendtoIP,
                             u16  wSendtoPort);

/************************************************************
 * ��������dsRemoveManyToOne
 *
 * ���ܣ� ɾ��ָ���Ķ��һ����
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwInLocalIP   - �������ݰ�������ӿ�
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsRemoveManyToOne(DSID dsId ,
                          u32  dwRecvIP,
                          u16  wRecvPort,
                          u32  dwSendtoIP,
                          u16  wSendtoPort );

/************************************************************
 * ��������dsAddSrcSwitch
 *
 * ���ܣ� ���Ӱ�Դת���Ĺ���
 * 
 * ��Դת����ʹ�ô˹���ʱ���������ݰ���Դ��ַ����������е�Դ
 * ��ַ��ȡ�ÿ��ת��ʱ����Ҫ�ȸ������ݰ��е�Դ��ַ����ת����
 * ������Ҳ�������ʹ��Ĭ��Դ��ַ����0@0����ת������
 * ע�⣺�ýӿ�֧�ֶ�㵽һ�㡣���dwSrcIP��wSrcPort��Ϊ�㣬
 * ������ȫ��ͬ��dsAddManyToOne��
 *
 * ʵ��˵����
 *
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwInLocalIP   - �������ݰ�������ӿ�
 * @param dwSrcIP       - �������ݰ���ԴIP
 * @param wSrcPort      - �������ݰ���ԴPort
 * @param dwSendtoIP    - ת��Ŀ��IP
 * @param wSendtoPort   - ת��Ŀ��Port
 * @param dwOutLocalIP  - ת�����ݰ����ñ���IP
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsAddSrcSwitch(DSID dsId ,
                       u32  dwRecvIP,
                       u16  wRecvPort,
                       u32  dwInLocalIP,
                       u32  dwSrcIP ,
                       u16  wSrcPort,
                       u32  dwSendtoIP,
                       u16  wSendtoPort,
                       u32  dwOutLocalIP = 0);

/************************************************************
 * ��������dsRemoveAllSrcSwitch
 *
 * ���ܣ� ɾ������ָ���İ�Դת������
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwSrcIP       - �������ݰ���ԴIP
 * @param wSrcPort      - �������ݰ���ԴPort
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveAllSrcSwitch(DSID dsId,
                             u32  dwRecvIP,
                             u16  wRecvPort,
                             u32  dwSrcIP,
                             u16  wSrcPort);

/************************************************************
 * ��������dsRemoveSrcSwitch
 * 
 * ���ܣ� ɾ��ָ���İ�Դת���Ĺ���
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param dwSrcIP       - �������ݰ���ԴIP
 * @param wSrcPort      - �������ݰ���ԴPort
 * @param dwSendtoIP    - ת��Ŀ��IP
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveSrcSwitch(DSID dsId,
                          u32  dwRecvIP,
                          u16  wRecvPort,
                          u32  dwSrcIP,
                          u16  wSrcPort,
                          u32  dwSendtoIP,
                          u16  wSendtoPort);

/************************************************************
 * ��������dsSetFilterFunc
 * 
 * ���ܣ� ���ù��˺���
 *
 * ע�⣺����ָ�����Ϊ�գ���ʾ���ԭ�е����ã����⣬����
 * DataSwitch����������߳��е��ô˺����ģ�Ҫ���õĺ�����
 * ����ȫ�ֺ��������õĲ�����Ҳ������ȫ����Ч�ġ�
 *
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 * @param dwRecvIP      - ����IP�����������ݰ���Ŀ��IP
 * @param wRecvPort     - ����Port�����������ݰ���Ŀ��Port
 * @param ptFunc        - ���˺���ָ��
 * @param FuncParam     - Ϊ���˺������ݵĲ���
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/
API u32 dsSetFilterFunc(DSID dsId,
			u32 dwRecvIP, 
                        u16 wRecvPort, 
                        FilterFunc ptFunc);
 
/************************************************************
 * ��������dsRemoveAll
 * 
 * ���ܣ� ɾ�����е�ת������
 * 
 * ʵ��˵����
 * 
 * ����˵����
 * @param dsId          - �û���ʶ
 *
 * ����ֵ��
 * @return DSOK: ��ʾ�ɹ� ; DSERROR: ��ʾʧ�� ;
 ************************************************************/      
API u32 dsRemoveAll( DSID dsId );

/**********************************************************
 * ����: dsSpecifyFwdSrc                                  *
 * ����: Ϊָ�����յ�ַ����ת�����ݰ�������Դ��ַ       *
 * ����:
 * @param dsId          - DSID
 * @param OrigIP        - ԭʼIP
 * @param OrigPort      - ԭʼPort
 * @param MappedIP      - ӳ��IP
 * @param MappedPort    - ӳ��Port
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort, u32 MappedIP, u16 MappedPort);

/**********************************************************
 * ����: dsResetFwdSrc                                    *
 * ����: �ָ�ָ����ַת�����ݰ���Դ��ַ
 * ����:
 * @param dsId          - DSID
 * @param OrigIP        - ԭʼIP
 * @param OrigPort      - ԭʼPort
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort);

 /**********************************************************
 * ����: dsSetSendCallback                                *
 * ����: ���÷��ͻص�����                                 *
 *       ȡ��ʱ�����ú���ָ��ΪNULL����
 * ����: dsID           - ��������ģ��ʱ�ľ��            *
 *       dwLocalIp      - ���ؽ���IP                      *
 *       wLocalPort     - ���ؽ��ն˿ں�                  *
 *       dwSrcIP        - ת��Ŀ��IP��ַ                  *
 *       wSrcPort       - ת��Ŀ�Ķ˿ں�                  *
 *       pfCallback     - �ص�����
 * ���: ��                                               *
 * ����: ��                                               *
 **********************************************************/    
API u32 dsSetSendCallback( DSID dsId, 
                u32 dwRecvIP, 
                u16 wRecvPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                SendCallback pfCallback);

 /**********************************************************
 * ����: dsSetAppDataForSend                              *
 * ����: Ϊ����Ŀ������һ���Զ����ָ��                   *
 *       ȡ��ʱ�����ú���ָ��ΪNULL����
 * ����: dsID           - ��������ģ��ʱ�ľ��            *
 *       dwLocalIp      - ���ؽ���IP                      *
 *       wLocalPort     - ���ؽ��ն˿ں�                  *
 *       dwSrcIP        - ԴIP��ַ                  *
 *       wSrcPort       - Դ�˿ں�                  *
 *       dwDstIP        - ת��Ŀ��IP��ַ                  *
 *       wDstPort       - ת��Ŀ�Ķ˿ں�                  *
 *       pAppData       - �Զ���ָ��
 * ���: ��                                               *
 * ����: 
 *     DSOK:�ɹ� DSERROR:ʧ��                             *
 **********************************************************/    
API u32 dsSetAppDataForSend( DSID dsId, 
                u32 dwRecvIP, 
                u16 wRecvPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                u32 dwDstIP,
                u16 wDstPort,
                void * pAppData);
                
/**********************************************************
 * ����: dsGetRecvPktCount                                *
 * ����: ��ѯ�����ܰ���                                   *
 * ����: dsID        - ��������ģ��ʱ�ľ��               *
 *       dwRecvIP   - ���ؽ���IP                         *
 *       wRecvPort  - ���ؽ��ն˿ں�                     *
 *       dwSrcIP     - ԴIP                               *
 *       wSrcPort    - Դ�˿ں�                           *
 *       dwRecvPktCount  - �����ܰ���                     *
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/    
API u32 dsGetRecvPktCount( DSID dsId , u32 dwRecvIP , u16 wRecvPort ,
						  u32 dwSrcIP , u16 wSrcPort , u32& dwRecvPktCount );

/**********************************************************
 * ����: dsGetSendPktCount                                *
 * ����: ��ѯ�����ܰ���                                   *
 * ����: dsID        - ��������ģ��ʱ�ľ��               *
 *       dwRecvIP   - ���ؽ���IP                         *
 *       wRecvPort  - ���ؽ��ն˿ں�                     *
 *       dwSrcIP     - ԴIP                               *
 *       wSrcPort    - Դ�˿ں�                           *
 *       dwSendtoIP     - ת��Ŀ��IP��ַ                     *
 *       wSendtoPort    - ת��Ŀ�Ķ˿ں�                     *
 *       dwSendPktCount  - �����ܰ���                     *
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/    
API u32 dsGetSendPktCount( DSID dsId , u32 dwRecvIP , u16 wRecvPort ,
						  u32 dwSrcIP , u16 wSrcPort ,
						  u32 dwSendtoIP, u16 wSendtoPort, u32& dwSendPktCount );
						  
/**********************************************************
 * ����: dsGetRecvBytesCount	                          *
 * ����: ��ѯ�������ֽ���                                 *
 * ����:												  *
 * ���: ��                                               *
 * ����: ���ؼ�ʱ�����ֽ���								  *
 **********************************************************/
API s64 dsGetRecvBytesCount( );

/**********************************************************
 * ����: dsGetSendBytesCount	                          *
 * ����: ��ѯ�������ֽ���                                 *
 * ����:												  *
 * ���: ��                                               *
 * ����: ���ؼ�ʱ�ֽ���									  *
 **********************************************************/
API s64 dsGetSendBytesCount( );

/************************************************************
 * ������ dsinfo
 * ���ܣ� ��ʾ���е�ת�����򣬼��������ڼ����Ķ˿� 
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dsinfo();

/************************************************************
 * ������ dsver
 * ���ܣ� ��ʾDataswitch�İ汾��Ϣ
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dsver();

/************************************************************
 * ������ dshelp
 * ���ܣ� ��ʾDataswitch���ṩ������İ�����Ϣ
 * ���룺
 * �����
 * ���أ�
 ************************************************************/      
API void dshelp();




/************************************************************
 * ������ dedebug
 * ���ܣ� ��/�رյ�����Ϣ
 * 
 * ���룺
 * @param op            - ָ�������û��������set, clear
 *                        (����WIN32��Ч)
 * �����
 * ���أ�
 ************************************************************/      
API void dsdebug(const char* op = NULL);

/************************************************************
 * ������ dedebug
 * ���ܣ� ��/�رո���һ��������Ϣ
 * �������ô˺�������Ϊ������Ϣ��Ӱ���������� 
 * 
 * ���룺
 * @param op            - ָ�������û��������set, clear
 *                        (����WIN32��Ч)
 * �����
 * ���أ�
 ************************************************************/      
API void dsdebug2();

 /**********************************************************
 * ����: dsSpecifyDstFwdSrc                               *
 * ����: Ϊָ��Ŀ�ĵ�ַ����ת�����ݰ�������Դ��ַ       *
 * ����:
 * @param dsId          - DSID
 * @param dwLocalIP     - ����IP
 * @param wLocalPort    - ����Port
 * @param dwDstIP		- Ŀ��IP
 * @param wDstPort		- Ŀ�Ķ˿�
 * @param dwMappedIP    - ӳ��IP
 * @param wMappedPort   - ӳ��Port
 * @param dwSrcIP		- ԴIP
 * @param wSrcPort		- Դ�˿�
 * ���: ��                                               *
 * ����: �ɹ�����DSOK ����DSERROR                         *
 **********************************************************/
API u32	dsSpecifyDstFwdSrc(DSID dsId, u32 dwLocalIP, u16 wLocalPort, 
						   u32 dwDstIP , u16 wDstPort ,
						   u32 dwMappedIP, u16 wMappedPort,
						   u32 dwSrcIP = 0 , u16 wSrcPort = 0 );


#endif
