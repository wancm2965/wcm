#ifndef _HP_INTERWORKINGEVENT_H_
#define _HP_INTERWORKINGEVENT_H_

#define IW_PRODUCT_ID     (const char *)"InterworkingGateway"
#define IW_VERSION_ID     (const char *)"AVCON_ALPHA"

#define SIPREG_LISTEN_PORT               6000
#define AID_REGINTERWORK_APP             103
#define AID_GKINTERWORK_APP              227
#define SIP_MAX_ALIAS_LEN		         64
#define SIP_MAX_ALIAS_NUM                128
#define SIP_MAX_NUM_CONTACT_ADDR         5
#define SIP_MAX_NUM_CALLID				 128

#define H323_MAX_ALIAS_NUM				 128
#define H323_MAX_NUM_CONTACT_ADDR        8
#define H323_MAX_NUM_EPID		         256

#define LEN_H323ALIAS					 64


enum  emRegInterEvent
{
	ev_inter_reg_start_con,
	ev_inter_reg_reqallreg,
	ev_inter_reg_con_ack,
	ev_inter_reg_reg,  
	ev_inter_reg_unreg,

	ev_inter_gk_start_con,
	ev_inter_gk_reqallreg,
	ev_inter_gk_con_ack,
	ev_inter_gk_reg,
	ev_inter_gk_unreg,
};

enum emSipRegAliasType
{
	sipReg_Alias_Unknow,
	sipReg_Alias_e164,
	sipReg_Alias_h323Id,
	sipReg_Alias_IP,
};

enum emGkRegAliasType
{
	gkReg_aliastype,
	gkReg_Alias_e164 = 0x1,
	gkReg_Alias_h323ID,
	gkReg_Alias_email,
	gkReg_Alias_epID,
	gkReg_Alias_GKID,
	gkReg_Alias_partyNumber,
	gkReg_Alias_others
};

typedef struct tagTSipRegAlias
{
	emSipRegAliasType	m_byType;
	s8				m_abyAlias[SIP_MAX_ALIAS_LEN + 1];
	tagTSipRegAlias()
	{
		Clear();
	}
	void Clear()
	{
		m_byType = sipReg_Alias_Unknow;
		m_abyAlias[0]	= '\0';
	}

	BOOL32 IsInValid()
	{
		return m_byType == sipReg_Alias_Unknow || strlen(m_abyAlias) == 0;
	}

	void SetAlias(emSipRegAliasType type, const s8* alias)
	{
		m_byType = type;
		strncpy(m_abyAlias, alias, SIP_MAX_ALIAS_LEN);
		m_abyAlias[SIP_MAX_ALIAS_LEN] = '\0';
	}

	void SetAlias(const s8* alias)
	{
		m_byType = sipReg_Alias_h323Id;
		strncpy(m_abyAlias, alias, SIP_MAX_ALIAS_LEN);
		m_abyAlias[SIP_MAX_ALIAS_LEN] = '\0';
	}

	BOOL32 operator == (const tagTSipRegAlias& tRhs)
	{
		return (0 == strcmp(m_abyAlias, tRhs.m_abyAlias));
	}

	tagTSipRegAlias& operator = (const tagTSipRegAlias& tRhs)
	{
		if (this == &tRhs)
		{
			return *this;
		}
		m_byType = tRhs.m_byType;
		strncpy(m_abyAlias, tRhs.m_abyAlias, sizeof(m_abyAlias));
		return *this;
	}

	void HostToNet()
	{
		m_byType = (emSipRegAliasType)(htonl((u32)m_byType));
	}
	void NetToHost()
	{
		m_byType =(emSipRegAliasType)(ntohl((u32)m_byType));
	}

}TSipRegAlias;


typedef struct tagGkRegAlias
{
	emGkRegAliasType  m_emType;
	s8		   m_achAlias[LEN_H323ALIAS+1];

	tagGkRegAlias()
	{
		Clear();
	}

	void Clear()
	{
		m_emType = gkReg_aliastype;
		memset( m_achAlias, 0, sizeof(m_achAlias) );
	}

	BOOL SetAliasAddr( emGkRegAliasType aliasType, const char* pAlias )
	{
		if ( aliasType <= gkReg_aliastype || aliasType >= gkReg_Alias_others ||
			pAlias == NULL ) 
			return FALSE;

		memset( m_achAlias, 0, sizeof(m_achAlias) );

		m_emType = aliasType;

		u16 nLen = strlen(pAlias) < LEN_H323ALIAS ? strlen(pAlias) : LEN_H323ALIAS;
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	

	emGkRegAliasType GetAliasType()
	{
		return m_emType;
	}

	s8* GetAliasName()  
	{
		return m_achAlias;	
	}

	void HostToNet()
	{
		m_emType = (emGkRegAliasType)(htonl((u32)m_emType));
	}
	void NetToHost()
	{
		m_emType =(emGkRegAliasType)(ntohl((u32)m_emType));
	}

}TGkRegAlias;


typedef struct tagInterNetAddr
{
	u32 m_dwIP;
	u16 m_wPort;

	void SetNetAddr(u32 dwIP, u16 wPort)
	{
		m_dwIP = dwIP;
		m_wPort = wPort;
	}

	void SetIPAddr(u32 dwIP)
	{
		m_dwIP = dwIP;
	}
	u32 GetIPAddr()
	{
		return m_dwIP;
	}

	void SetIPPort(u16 wPort)
	{
		m_wPort = wPort;
	}
	u16 GetIPPort()
	{
		return m_wPort;
	}

	BOOL operator==(tagInterNetAddr& tAddr)
	{
		if ( GetIPAddr() == tAddr.GetIPAddr()
			&& GetIPPort() == tAddr.GetIPPort() )
		{
			return TRUE;
		}

		return FALSE;
	}

	tagInterNetAddr()
	{
		m_dwIP  = 0;
		m_wPort = 0;
	}

	void HostToNet()
	{
		m_wPort = htons( m_wPort );
		m_dwIP = htonl( m_dwIP );
	}
	void NetToHost()
	{
		m_wPort = ntohs( m_wPort );
		m_dwIP = ntohl( m_dwIP );
	}

}TInterNetAddr;


typedef struct tagTSipRegInterwork 
{

	u16				m_wAliasNum;
	TSipRegAlias	m_atAlias[SIP_MAX_ALIAS_NUM];
	u16             m_wAddrNum;
	TInterNetAddr	m_atCalledAddr[SIP_MAX_NUM_CONTACT_ADDR];

	s8				m_aSipCallId[SIP_MAX_NUM_CALLID];
	u32				m_dwExpires;

	tagTSipRegInterwork()
	{
		memset(this, 0, sizeof(tagTSipRegInterwork));
	}
	void HostToNet()	
	{
		u16 wIndex=0;
		for( wIndex=0; wIndex <m_wAliasNum; wIndex++)
		{
			m_atAlias[wIndex].HostToNet();
		}
		for( wIndex=0;wIndex< m_wAddrNum; wIndex++)
		{
			m_atCalledAddr[wIndex].HostToNet();
		}
		m_wAliasNum = htons(m_wAliasNum) ;
		m_wAddrNum = htons(m_wAddrNum);
		m_dwExpires = htonl(m_dwExpires);
	}
	void NetToHost()
	{
		u16 wIndex =0;

		m_wAliasNum = ntohs(m_wAliasNum);
		m_wAddrNum = ntohs(m_wAddrNum);
		m_dwExpires = ntohl(m_dwExpires);

		for(wIndex=0; wIndex <m_wAliasNum; wIndex++)
		{
			m_atAlias[wIndex].NetToHost();
		}
		for(wIndex=0;wIndex< m_wAddrNum; wIndex++)
		{
			m_atCalledAddr[wIndex].NetToHost();
		}

	}
}TSipRegInterwork;


typedef struct tagGKRegInterwork 
{	
	u16				m_wAliasNum;
	s8				m_aEPId[H323_MAX_NUM_EPID];
	u32				m_dwExpires;

	tagGKRegInterwork()
	{
		memset(this, 0, sizeof(tagGKRegInterwork));
	}	


	void HostToNet()
	{
		m_wAliasNum = htons( m_wAliasNum );
		m_dwExpires = htonl( m_dwExpires );
	}

	void NetToHost()
	{
		m_wAliasNum = ntohs( m_wAliasNum );
		m_dwExpires = ntohl( m_dwExpires );
	}
}TGKRegInterwork;


#endif 
