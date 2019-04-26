// Independent_WBDlg.cpp : 实现文件

#include "stdafx.h"
#include "Independent_WBDlg.h"
#include "..\include\WB\WBDefine.h"					  
#include "..\includeUCC\NETEC\NETEC_Node.h"
#include "WBMainDlg.h"
#include "AVDataPacket.h"

#define TIMER_WB_CNN_NET 10119

// CIndependent_WBDlg 对话框

IMPLEMENT_DYNAMIC(CIndependent_WBDlg, CDialog)

CIndependent_WBDlg::CIndependent_WBDlg(unsigned long ulWBChannelID)
: AVMTDataChannel(ulWBChannelID, true, false)
, m_ulWBChannelID(ulWBChannelID)
, m_pIAVMTDataSessionClient(NULL)
{
}

CIndependent_WBDlg::~CIndependent_WBDlg(void)
{
}

void CIndependent_WBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIndependent_WBDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_NCLBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CIndependent_WBDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    WB_SetTypeOfWB(WB_SINGLERVER);//这条语句的调用需放在显示白板之前
	WB_ShowWhiteBoardDlg(this, (IHPNetNotify*)this, &CWnd::wndTop, CRect(0, 0, 0, 0), 0);
	std::string strTemp = _W2A(GetExePath() + _T("//HPDTS//Local//"));
	WB_SetWBTempDir(strTemp.c_str());
  
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CIndependent_WBDlg::DestroyWindow()
{
	WB_Close();
	ReleaseConnection();

	return CDialog::DestroyWindow();
}

BOOL CIndependent_WBDlg::PreTranslateMessage( MSG* pMsg )
{
	//屏蔽键盘Enter消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		return FALSE;
	}
	//屏蔽键盘Esc消息
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) 
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CIndependent_WBDlg::OnSize(UINT nType, int cx, int cy)
{
	WB_MoveBorad(NULL, CRect(0, 0, cx, cy), SWP_NOACTIVATE);
}

void CIndependent_WBDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_WB_CNN_NET == nIDEvent)
	{
		KillTimer(TIMER_WB_CNN_NET);
		//WB_ReqAllObj();
	}

	__super::OnTimer(nIDEvent);
}

bool CIndependent_WBDlg::Connect(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype)
{
	if (AVMTDataChannel::Connect() == false)
		return false;

	SetWBChannelChanged(ulWBChannelID, mcuid, mcuaddr, mcuport, mcutype);

	KillTimer(TIMER_WB_CNN_NET);
	SetTimer(TIMER_WB_CNN_NET, 100, NULL);

	return true;
}

void CIndependent_WBDlg::ReleaseConnection(void)
{
	KillTimer(TIMER_WB_CNN_NET);

	if (m_pIAVMTDataSessionClient)
	{
		m_pIAVMTDataSessionClient->ReleaseConnections();
		delete m_pIAVMTDataSessionClient;
		m_pIAVMTDataSessionClient = NULL;
	}
	m_bConnected = false;

	AVMTDataChannel::RemoveAVMTDataChannel(GetDataChannelID());

	AVMTDataChannel::ReleaseConnections();
}

void CIndependent_WBDlg::SetWBChannelChanged(unsigned long ulWBChannelID,const std::string& mcuid,const std::string& mcuaddr,unsigned short mcuport,const std::string& mcutype)
{
	if (mcuid == "" || mcuaddr == "")
	{
		return;
	}

	bool bChanged = false;
	if (m_mcuid != mcuid || m_ulWBChannelID != ulWBChannelID)
	{
		m_mcuid		= mcuid;
		m_mcuaddr	= mcuaddr;
		m_mcuport	= mcuport;
		m_mcutype	= mcutype;

		m_ulWBChannelID = ulWBChannelID;

		bChanged = true;
	}

	if (bChanged || m_pIAVMTDataSessionClient == NULL)
	{
		if (m_pIAVMTDataSessionClient)
		{
			m_pIAVMTDataSessionClient->ReleaseConnections();
			delete m_pIAVMTDataSessionClient;
			m_pIAVMTDataSessionClient = NULL;
		}

		AVMTDataChannel::RemoveAVMTDataChannel(GetDataChannelID());
		AVMTDataChannel::SetDataChannelID(ulWBChannelID);
		AVMTDataChannel::AddAVMTDataChannel(this);

		m_pIAVMTDataSessionClient = CreateIAVMTDataSessionClient(*this, m_ulWBChannelID,
			m_mcuid.c_str(),m_mcuaddr.c_str(),m_mcuport,m_mcutype.c_str(),
			NETEC_Node::GetMCUID(),NETEC_Node::GetMCUIP(),NETEC_Node::GetServerPort(),""/*NETEC_Node::GetMCUType()*/);
	}
}

bool CIndependent_WBDlg::IsConnect(void)
{
	return m_bConnected;
}

/*处理白板内部自定义数据结构。参数：a）白板数据包 b）数据包长度*/
bool CIndependent_WBDlg::processWBData(char * pData,long nLen)
{
	WB_ProcessDataIn(pData, nLen);
	return true;
}

void CIndependent_WBDlg::SetLocalUserID(std::string strUserID)
{
	WB_SetLocalUserID(strUserID.c_str());
}

//////////////////////////////////////////////////////////////////////////
// 继承AVMTDataChannel接口
void CIndependent_WBDlg::OnDataToUI(const char*pData,int nLen)
{
	WB_ProcessDataIn(pData, nLen);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 继承IHPNetNotify接口
void CIndependent_WBDlg::SendWbData(const char* pData,int iLen,bool bToNet)
{
	if (bToNet)
	{
			AVMTDataChannel::ProcessUIData(pData, iLen);
	}
	else
	{
		UINT iCmd = 0;
		
	
		AVDataInPacket inx((char*)pData, iLen);
		inx >> iCmd;
		if (WB_NEW_COMMAND_DATA == iCmd)
		{
			UINT uLen = 0;
			inx>>uLen;
			unsigned char* pBuff = inx.ReadData((int&)uLen);
			char* pSendToMainDlg = new char[uLen];
			if (NULL != pSendToMainDlg)
			{
				memset(pSendToMainDlg, 0, uLen);
				memcpy(pSendToMainDlg, pBuff, uLen);
				CWBMainDlg* pParent = (CWBMainDlg*)GetParent();
				if ( NULL != pParent)
				{
					pParent->ProcessWBToLocalData(pSendToMainDlg);
					delete[] pSendToMainDlg;
					pSendToMainDlg = NULL;
				}
			}
			
		}

	/*	if (CMD_SINGLE_QUIT == iCmd)
		{
		
			BOOL bl = TRUE;
			WB_SetMediaSinToIntnet(bl);
			pParent->CloseLoginDlg();
			WB_MediaSingelToNet();
		}
		*/

		//UINT uCmd = 0;
		//AVDataInPacket in((char*)pData,iLen);
		//in >>uCmd;
		//if (uCmd == WB_NEW_COMMAND_DATA)
		//{
		//	UINT uLen = 0;
		//	in >> uLen;
		//	unsigned char* pBuff = in.ReadData((int&)uLen);
		//	char* pNotifyData = new char[uLen];
		//	memset(pNotifyData,0,uLen);
		//	memcpy(pNotifyData,pBuff,uLen);
		//	PostMessage(WM_DATA_NOTIFY,(WPARAM)pNotifyData,NULL);
		//}
	}
}
//////////////////////////////////////////////////////////////////////////

void CIndependent_WBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if( (nID & 0xFFF0) == SC_CLOSE )
	{
		PostQuitMessage(0);
	}
	else if( (nID & 0xFFF0) == SC_MOVE )
	{
		return;
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CIndependent_WBDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	return;
}
