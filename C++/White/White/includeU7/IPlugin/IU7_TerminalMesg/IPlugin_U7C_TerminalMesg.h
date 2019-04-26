// UCC7-C�����
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7C_UCMesg_Observer
	{
	public:
		virtual ~IPlugin_U7C_UCMesg_Observer(){};

	public:
		//�յ����ڷֱ��ʵ���Ϣ
		virtual void OnCodeStream(int nLevel) = 0;
		virtual void OnCodeFrame(int nLevel) = 0;
		virtual void OnDecodStream(int nLevel) = 0;
		virtual void OnDecodFrame(int nLevel) = 0;
		virtual void OnDecodResolution(int nLevel) = 0;	
	};

	class IPlugin_U7C_TerminalMesg_Observer
	{
	public:
		virtual ~IPlugin_U7C_TerminalMesg_Observer(){};

	public:

		//�յ��ն˴���������Ϣ
		virtual void OnTerminalinfo(const char* memberid, std::string GroupID, int Cpu, int Up, int Down) = 0; 
		virtual void OnTerminalAVLostAblity(const char* memberid, std::string GroupID, int Ablity, double ALost, double Vlost) = 0;

		virtual void OnTTerminalAinfo(const char* memberid, int CpuTp, int Cpu, int Memory, double TotalSpace, double FreeSpace
			, std::string Mac, std::string IP) = 0;
		virtual void OnMicSpeakerValue(const char* memberid,  int AVNum, int MicValue, int AudioValue) = 0;
		//ģ����Ϣ
		virtual void OnTTerminalTPinfo(int ScreenID, int WindowID, std::string MemberID) = 0;
		virtual void OnUCTTerminalinfo(const char* memberid, std::string WindowID, std::string ScreenID, int WinUp, int WinDown, double windvs
			, double windas, std::string winr, double winAlost, double winVlost, int Winp) = 0;

		//����Ӳ����Ϣ�����ȡ����ӵ��ն���Ϣ
		
		virtual void OnLocalinfo(const char* strFromID, std::string ProcessName, std::string ProcessType, int ProcessNum, int MaxcLockSpeed, std::string Os,
			std::string ComPatiBility, std::string DACType, std::string Modedescipion, std::string Caption, std::string VideoProcess) = 0;
		virtual void OnNet(const char* strFromID, int DevID1, std::string DevName1, int DevID2, std::string DevName2, int DevID3, std::string DevName3) = 0;
		virtual void OnAudioin(const char* strFromID, int DevID1, std::string DevName1, int DevID2, std::string DevName2, int DevID3, std::string DevName3) = 0;
		virtual void OnAudioout(const char* strFromID, int DevID1, std::string DevName1, int DevID2, std::string DevName2, int DevID3, std::string DevName3) = 0;
		virtual void OnTpinfoEnd(const char* strFromID) = 0;
	};

	class IPlugin_U7C_TerminalMesg : public IPlugin
	{
	public:
		virtual ~IPlugin_U7C_TerminalMesg(){};

	public:

		virtual void AddObserver(IPlugin_U7C_TerminalMesg_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7C_TerminalMesg_Observer* pObserver) = 0;

		/*"CODE_STREAM" //��������
		"CODE_FRAME" //����֡��
		"DECOD_STREAM" // ��������
		"DECOD_FRAME" //����֡��
		"DECOD_RESOLUTION" //����ֱ���*/


		//������ն˷���
		//////////�������//////////////////////////////////////////////
		virtual void SendMMUserData(const char* strToID, const char* strUERCMD, int nLevel) = 0;
		/////////////�����ȡ״̬
		virtual void SendMMUserData(const char* strToID, const char* strUERCMD, std::string GruopID) = 0;
		/////////////Զ�̿��Ʒ�������
		virtual void SendContralData(const char* strToID, const char* strUERCMD, int nCardIndex = 0, const char* memberid = "" , int AVType = 100) = 0;
		//���ͱ��ش����������Ϣ
		virtual void SendMMUserData(const char* strFromID, const char* strUERCMD, int ScreenID, int WindowID) = 0;
		//�����
		virtual void SendMMUserData(const char* strToID, const char* strType, const char* Cmd) = 0;

		virtual int GetCpuUsage() = 0; //��ʱ��ȡcpuʹ����
	};
}
