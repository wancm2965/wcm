#ifndef _AVFDLGCHAT_H
#define _AVFDLGCHAT_H

class IDlgChatNotify
{
public:
	virtual void OnMessageSend(CString SendUserID,CString RecvUserID,CString text,CString fontface,CString fontsize,CString fontcolor,CString fontEffects)=0;        
																						//������Ϣ
	//virtual void OnGetConfUserList(list<string > & userlist)=0; //�õ�������Ա�б�
    virtual CString OnGetUserName(CString userid)=0;            //�õ��û�����                                      
	virtual CString GetLocalUserID()=0;			 			 //�õ������û�ID
	virtual CString GetLocalUserName()=0;          			 //�õ������û���
	virtual bool IsInAVConference()=0;            				 //�жϵ�ǰ�Ƿ�������	
	virtual bool IsMyAuditor()=0;						//������   
    virtual void DlgChatNotify_OnLButtonDown(UINT nFlags, CPoint point) = 0;    //�߼������϶�֧��
	virtual IAVCRecordFactory* GetConferenceRecordFactory( void ) = 0;	//�õ�����¼��ָ��
	virtual bool GetUserIsSubMeeting(CString strUserID) = 0;	//�õ��û��Ƿ��������
	virtual void SaveRemoteConfChatStream(char* outPacket, int nLen, bool bKeyFrame) = 0;

};

class IDlgChat
{
public:
	virtual void ReleaseConnection()=0;    //�ͷ���Դ
	virtual void SetShowWindow(bool bShow)=0;    //��ʾ����
	virtual void SetMyWindowPos(CWnd * pWndInsertAfter,int x,int y,int cx,int cy,UINT nFlag)=0;//���ô���λ��					
	virtual void AddUserNameToList(CString UserName,CString UserID,bool bShowMsg = true)=0;  //���ӻ�����Ա
	virtual void DelUserNamefromList(CString UserName)=0; //ɾ��������Ա

	virtual CWnd * GetWnd()=0;					//�õ����ھ��
	virtual void MySetParent(CWnd * pWnd)=0;		//���ø�����
	virtual bool GetWindowVisible()=0;		//��ⴰ�ھ���Ƿ���Ч
	virtual void OnGetMessage(CString SendUserName,CString RecvUserName,CString text,CString fontface,CString fontsize,CString fontcolor,CString fontEffects)=0; 
	                                             //�����û�������Ϣ
	virtual void OnSystemMessage(CString title,CString text)=0;
										//����ϵͳ��Ϣ

    virtual void EnableWindow(bool bEnable = true) = 0; //���ڿ���/������

	virtual BOOL StartConferenceRecord(void) = 0;				// ��ʼ����¼��
	virtual void StopConferenceRecord(void) = 0;				// ֹͣ����¼��

	virtual void SetConferencePlayer(void) = 0;				// ���鲥����
	virtual void SetRecordContent(const std::string& strText) = 0;
	virtual void ReSetFocus()= 0;

};

AFX_EXT_CLASS IDlgChat * CreateDlgChat(IDlgChatNotify& rNotify,CWnd * pCtrlWnd);

#endif