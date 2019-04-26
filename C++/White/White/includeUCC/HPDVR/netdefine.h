#pragma  once

#define AVCON_NDVR_VERSION		"v6.2"/*��汾*/
#define AVCON_NDVR_SUBVERS		".2"/*�Ӱ汾*/
#define AVCON_NDVR_BUILDED		".1"/*������*/
#define AVCON_NDVR_VERSION_FULL	 AVCON_NDVR_VERSION AVCON_NDVR_SUBVERS AVCON_NDVR_BUILDED

//��������
#define NDVR_CLIENT		0
#define NDVR_DEVICE		1
#define AVCON_DEVICE	2

#include <list>
#include <vector>
#include <string>
#include <hash_map>
#include <map>
#pragma warning(disable:4996)	//std::XX������Ϊ�����
using namespace std;


#ifndef MIN
#define MIN(a,b) (((a)>(b)) ? (b) : (a))
#endif

#ifndef MAXLEN 
#define MAXLEN(a)	(sizeof(a)-1)
#endif


/************************************************************************/
/* NDVR�ڲ�����                                                         */
/************************************************************************/
// ���� [5/24/2008]

// �û����� 
/*
mgr_usr|cmd_mod_user_password|cmd_mod_user_rights|cmd_add_user|cmd_del_user|cmd_get_user|
*/
#define CMD_USER_MODIFY_PSW					"cmd_mod_user_password"/*�޸�����*/				
#define CMD_RIGHT_MODIFY					"cmd_mod_user_rights"/*�޸�Ȩ��*/		
#define CMD_ADDUSER							"cmd_add_user"/*����û�*/		
#define CMD_DELUSER							"cmd_del_user"/*ɾ���û�*/
#define CMD_DOWNLOAD_USER					"cmd_get_user"/*ȡ�û�*/


// ͨ������
/*
mgr_chn|cmd_get_channel_param|cmd_set_channel_param|cmd_get_audio_param|cmd_set_audio_param|cmd_get_video_param|cmd_set_video_param|
cmd_start_motion_detect|cmd_stop_motion_detect|cmd_get_motion_detect|cmd_set_motion_detect|cmd_start_video_hide|cmd_stop_video_hide|
cmd_start_video_cover|cmd_stop_video_cover|cmd_start_video_lost|cmd_stop_video_lost|
cmd_get_frame_to_jpg|cmd_set_video_input|cmd_set_video_adjust|cmd_set_video_invert|
cmd_get_channel_basic|cmd_get_channel_video|cmd_get_channel_audio|cmd_get_channel_mcu|cmd_get_channel_ptz|
cmd_start_interphone|cmd_rcv_interphone|cmd_stop_interphone|
cmd_start_avstream|cmd_rcv_avstream|cmd_stop_avstream|
cmd_get_video_hide_state|cmd_get_montion_detect_state|cmd_get_record_state|cmd_get_alarm_state|
cmd_switch_stream_mode|cmd_start_channel_scroll|cmd_dwn_channel_status|
*/
#define CMD_REMOTE_CHANNEL_GET_AV_WIZARD	 "cmd_get_channel_param"/*ȡͨ������*/
#define CMD_REMOTE_CHANNEL_SET_AV_WIZARD	 "cmd_set_channel_param"/*��ͨ������*/
#define CMD_GET_AUDIO_PARAM					 "cmd_get_audio_param"/*ȡ��Ƶ����*/
#define CMD_SET_AUDIO_PARAM					 "cmd_set_audio_param"/*����Ƶ����*/
#define CMD_GET_VIDEO_PARAM					 "cmd_get_video_param"/*ȡ��Ƶ����*/
#define CMD_SET_VIDEO_PARAM					 "cmd_set_video_param"/*����Ƶ����*/
#define CMD_START_MOVE_DETECT				 "cmd_start_motion_detect"/*��ʼ�ƶ����*/
#define CMD_STOP_MOVE_DETECT				 "cmd_stop_motion_detect"/*ֹͣ�ƶ����*/
#define CMD_CHANNEL_GET_MOVE_DETECT 		 "cmd_get_motion_detect"/*ȡ�������*/
#define CMD_CHANNEL_SET_MOVE_DETECT 		 "cmd_set_motion_detect"/*�趯�����*/
#define CMD_START_VIDEO_MASK				 "cmd_start_video_hide"/*��ʼ��Ƶ����*/
#define CMD_STOP_VIDEO_MASK					 "cmd_stop_video_hide"/*ֹͣ��Ƶ����*/
#define CMD_START_VIDEO_COVER				 "cmd_start_video_cover"/*��ʼ����ڵ�*/
#define CMD_STOP_VIDEO_COVER				 "cmd_stop_video_cover"/*ֹͣ����ڵ�*/
#define CMD_START_VIDEO_LOST				 "cmd_start_video_lost"/*��ʼ��ⶪʧ*/
#define CMD_STOP_VIDEO_LOST					 "cmd_stop_video_lost"/*ֹͣ��ⶪʧ*/
#define CMD_CAPTURE_FRAME_TO_JPG			 "cmd_get_frame_to_jpg"/*ץ��*/
#define CMD_VIDEO_INPUT_SETUP				 "cmd_set_video_input"/*��Ƶ����*/
#define CMD_VIDEO_ADJUST_SETUP			     "cmd_set_video_adjust"/*��Ƶ����*/
#define CMD_VIDEO_INVERT					 "cmd_set_video_invert"/*�ߵ�ͼ��*/
#define CMD_CHANNEL_GETBASEINFO				 "cmd_get_channel_basic"/*ȡͨ��������Ϣ*/
#define CMD_CHANNEL_GETVIDEOINFO			 "cmd_get_channel_video"/*ȡͨ����Ƶ��Ϣ*/
#define CMD_CHANNEL_GETAUDIOINFO			 "cmd_get_channel_audio"/*ȡͨ����Ƶ��Ϣ*/
#define CMD_CHANNEL_GETMCUINFO				 "cmd_get_channel_mcu"/*ȡͨ��MCU��Ϣ*/
#define CMD_CHANNEL_GETPTZINFO			     "cmd_get_channel_ptz"/*ȡͨ����̨��Ϣ*/
#define CMD_CH_AUDIO_START					 "cmd_start_interphone"/*�򿪶Խ�*/
#define CMD_CH_AUDIO_RECEIVE	             "cmd_rcv_interphone"/*���նԽ�*/
#define CMD_CH_AUDIO_STOP		             "cmd_stop_interphone"/*ֹͣ�Խ�*/		
#define CMD_CH_VIDEO_START					 "cmd_start_avstream"/*��ʼ����Ƶ*/
#define CMD_CH_VIDEO_RECEIVE				 "cmd_rvc_avstream"/*��������Ƶ*/
#define CMD_CH_VIDEO_STOP					 "cmd_stop_avstream"/*ֹͣ����Ƶ*/
#define CMD_CHANNEL_VIDEOMASK_STATE			 "cmd_get_video_hide_state"/*ȡ����״̬*/
#define CMD_CHANNEL_VIDEOMOVE_STATE			 "cmd_get_montion_detect_state"/*ȡ����״̬*/
#define CMD_CHANNEL_RECORDSTATE				 "cmd_get_record_state"/*ȡ¼��״̬*/
#define CMD_CHANNEL_ALARMSTATE				 "cmd_get_alarm_state"/*ȡ����״̬*/
#define CMD_SWITCH_STREAM_MODE				 "cmd_switch_stream_mode"/*�л���ģʽ*/
#define CMD_CHANNEL_AUTOSCROLL				 "cmd_start_channel_scroll"/*��ʼ��ֹͣ��ѭ*/
#define CMD_DOWNLOAD_CHANNEL_STATUS			 "cmd_dwn_channel_status"/*����ͨ��״̬*/
#define CMD_UPDATE_CHANNEL_NAME				 "cmd_udp_chname"/*����ͨ������*/


// ¼�����
/*
mgr_rec|cmd_get_record_file|cmd_start_manual_record|cmd_stop_manual_record|cmd_set_record_param|cmd_get_record_param|
cmd_set_record_schedule|cmd_get_record_schedule|cmd_start_defence|cmd_stop_defence|cmd_issue_defence_state|
cmd_get_fixed_driver|cmd_set_fixed_driver|
*/
#define CMD_GET_RECORD_FILE					"cmd_get_record_file"/*ȡ¼���ļ�*/
#define CMD_MANUAL_RECORD_START				"cmd_start_manual_record"/*��ʼ�ֶ�¼��*/
#define CMD_MANUAL_RECORD_STOP				"cmd_stop_manual_record"/*ֹͣ�ֶ�¼��*/
#define CMD_ENCODER_RECORD_SET				"cmd_set_record_param"/*��¼�����*/
#define CMD_ENCODER_RECORD_GET				"cmd_get_record_param"/*ȡ¼�����*/
#define CMD_VIDEOPLAN_SET					"cmd_set_record_schedule"/*��ƻ���*/
#define CMD_VIDEOPLAN_GET					"cmd_get_record_schedule"/*ȡ�ƻ���*/
#define CMD_VIDEOPLAN_START					"cmd_start_defence"/*����*/
#define CMD_VIDEOPLAN_STOP					"cmd_stop_defence"/*����*/
#define CMD_VIDEOPLAN_RUNNING				"cmd_issue_defence_state"/*��������״̬*/
#define CMD_FIXED_DRIVER_GET				"cmd_get_fixed_driver"/*ȡ������Ϣ*/
#define CMD_FIXED_DRIVER_SET				"cmd_set_fixed_driver"/*�������Ϣ*/



// ��̨����
/*
mgr_ptz|cmd_ptz_control|cmd_ptz_lock|cmd_get_prepoint|cmd_set_prepoint|cmd_set_ptz_param|cmd_get_ptz_param|
*/
#define CMD_PTZ								"cmd_ptz_control"/*��̨����*/
#define CMD_REMOTE_CHANNEL_SEND_YT_CMD		"cmd_ptz_control"/*��̨����*/
#define CMD_CLOUD_PEDESTAL_LOCK				"cmd_ptz_lock"/*��̨����*/
#define CMD_CHANNEL_GET_PRE_POINT 			"cmd_get_prepoint"/*ȡԤ�õ�*/
#define CMD_CHANNEL_SET_PRE_POINT 			"cmd_set_prepoint"/*��Ԥ�õ�*/
#define CMD_REMOTE_CHANNEL_SET_SERIAL_SET	"cmd_set_ptz_param"/*����̨����*/
#define CMD_REMOTE_CHANNEL_GET_SERIAL_SET	"cmd_get_ptz_param"/*ȡ��̨����*/



// �豸����
/*
mgr_dev|cmd_add_encoder|cmd_mod_encoder|cmd_del_encoder|cmd_add_channel|cmd_mod_channel|cmd_del_channel|cmd_get_devinfo|
cmd_get_encoder|cmd_reboot|cmd_clk_remote_key|
*/
#define CMD_ENCODERMGR_EADD					"cmd_add_encoder"/*��ӱ�����*/
#define CMD_ENCODERMGR_EMOD					"cmd_mod_encoder"/*�޸ı�����*/
#define CMD_ENCODERMGR_EDEL					"cmd_del_encoder"/*ɾ��������*/
#define CMD_ENCODERMGR_CADD					"cmd_add_channel"/*���ͨ��*/
#define CMD_ENCODERMGR_CMOD					"cmd_mod_channel"/*�޸�ͨ��*/
#define CMD_ENCODERMGR_CDEL					"cmd_del_channel"/*ɾ��ͨ��*/
#define CMD_GET_REGION_INFO					"cmd_get_devinfo"/*ȡ������Ϣ*/
#define CMD_CHANNEL_GETALLINFO				"cmd_get_devinfo"/*ȡ�豸��Ϣ*/
#define CMD_CHANNEL_GETENCODERINFO			"cmd_get_encoder"/*ȡ��������Ϣ*/
#define CMD_DEV_RECONNECT					"cmd_reboot"/*����������*/
#define CMD_CLICK_SOFT_KEY					"cmd_clk_remote_key"/*Զ�̰���*/
#define CMD_UPDATE_ENCODER_NAME				"cmd_udp_encname"/*�����豸����*/

// [4/9/2009 lzy]
#define CMD_GET_DEVCFGINFO					"cmd_get_devcfginfo"		//�õ��豸������Ϣ
#define CMD_SET_DEVCFGINFO					"cmd_set_devcfginfo"		//�����豸������Ϣ 
//--------------------------

// ��������
/*
mgr_alm|cmd_set_alarm_param|cmd_get_alarm_param|cmd_stop_alert|cmd_issue_start_alarm_state|cmd_issue_stop_alarm_state|
*/
#define CMD_ALARM_PARAM_SET					"cmd_set_alarm_param"/*�豨������*/
#define CMD_ALARM_PARAM_GET					"cmd_get_alarm_param"/*ȡ��������*/
#define CMD_MANUAL_STOP_ALERT				"cmd_stop_alert"/*�ֶ�ֹͣ����*/
#define CMD_HOST_DETECTER_ALERT_START		"cmd_issue_start_alarm_state"/*��ʼ����*/
#define CMD_HOST_DETECTER_ALERT_STOP		"cmd_issue_stop_alarm_state"/*ֹͣ����*/


// ��־����
/*
mgr_log|cmd_get_operate_log|cmd_get_operate_type|cmd_get_alarm_log|cmd_get_record_log|
cmd_del_operate_log|cmd_del_alarm_log|cmd_del_record_log|cmd_bak_operate_log|cmd_bak_alarm_log|cmd_bak_record_log|
*/
#define CMD_DOWN_OPERLOG					"cmd_get_operate_log"/*ȡ������־*/
#define CMD_OPLOGTYPE						"cmd_get_operate_type"/*ȡ��������*/
#define CMD_ALARM_LOG						"cmd_get_alarm_log"/*ȡ������־*/
#define CMD_GET_RECORD_HISTORY				"cmd_get_record_log"/*ȡ¼����־*/
#define CMD_DEL_OPLOG						"cmd_del_operate_log"/*ɾ��������־*/
#define CMD_DEL_ALARMLOG					"cmd_del_alarm_log"/*ɾ��������־*/
#define CMD_DEL_RECORDLOG					"cmd_del_record_log"/*ɾ��¼����־*/
#define CMD_SAVE_OPLOG						"cmd_bak_operate_log"/*���ݲ�����־*/
#define CMD_SAVE_ALARMLOG					"cmd_bak_alarm_log"/*���ݱ�����־*/
#define CMD_SAVE_RECORDLOG					"cmd_bak_record_log"/*����¼����־*/


// ��������
/*
mgr_oth|cmd_login|cmd_logout|cmd_disconnect|cmd_p2p|cmd_transfer|cmd_broadcast|cmd_svr_address|cmd_mcu_address|cmd_mcu_issue_num|
cmd_set_ipfirst|cmd_get_ipfirst|
*/
#define CMD_LOGIN							"cmd_login"/*��½*/
#define CMD_LOGOUT							"cmd_logout"/*ע��*/
#define CMD_DISCONNECT						"cmd_disconnect"/*�Ͽ�����*/
#define CMD_P2PURD							"cmd_p2p"/*P2Pͨ��*/
#define CMD_TRANSFER						"cmd_transfer"/*͸������*/
#define CMD_BROADCAST						"cmd_broadcast"/*�㲥����*/
#define CMD_AVCON_IPINFO					"cmd_svr_address"/*�����·���������ַ*/
#define CMD_CH_MCU							"cmd_mcu_address"/*�����·�MCU��ַ*/
#define CMD_CH_RELEASE2MCU					"cmd_mcu_issue_num"/*�����·�����ͨ����*/
#define CMD_IPP_FIRST_SET					"cmd_set_ipfirst"/*������ʹ�õ�IPЭ��*/
#define CMD_IPP_FIRST_GET					"cmd_get_ipfirst"/*ȡ����ʹ�õ�IPЭ��*/

// ��ͼ����
/*
mgr_map|cmd_add_mapnode|cmd_del_mapnode|cmd_mod_mapnode|cmd_add_hotspot|cmd_del_hotspot|cmd_mod_hotspot|cmd_get_mapinfo|
*/
#if 0 //2.0
#define CMD_MAPADD							"cmd_add_mapnode"/*���ӵ�ͼ���*/
#define CMD_MAPDEL							"cmd_del_mapnode"/*ɾ����ͼ���*/
#define CMD_MAPMOD							"cmd_mod_mapnode"/*�޸ĵ�ͼ���*/
#define CMD_MAPPOINTADD						"cmd_add_hotspot"/*���ӵ�ͼ�ȵ�*/
#define CMD_MAPPOINTDEL						"cmd_del_hotspot"/*ɾ����ͼ�ȵ�*/
#define CMD_MAPPOINTMOD						"cmd_mod_hotspot"/*�޸ĵ�ͼ�ȵ�*/
#define CMD_MAPDOWNLOAD						"cmd_get_mapinfo"/*ȡ��ͼ��Ϣ*/

#define ATTRIB_MAP_ID						"mapid"
#define ATTRIB_MAP_PARENTID					"parid"
#define ATTRIB_MAP_NAME						"name"
#define ATTRIB_MAP_LEVEL					"level"
#define ATTRIB_MAP_FILENAME					"file"
#define ATTRIB_MAPPOINT_ID					"ponid"
#define ATTRIB_MAPPOINT_CID					"chnid"
#define ATTRIB_MAPPOINT_EID					"codid"
#define ATTRIB_MAPPOINT_MAPID				"mapid"
#define ATTRIB_MAPPOINT_TYPE				"type"
#define ATTRIB_MAPPOINT_NAME				"name"
#define ATTRIB_MAPPOINT_X					"x"
#define ATTRIB_MAPPOINT_Y					"y"
#define ATTRIB_MAP_OK						"ok"
#define ATTRIB_MAP_DOWNTYPE					"dwn_t" 


#endif
#if 1//6.1
//whidbeycai 20070514 ��ͼ����ϵ������
#define CMD_MAPADD  "cmd_map_add"    //��ͼ�������
#define CMD_MAPDEL  "cmd_map_del"    //��ͼ���ɾ��
#define CMD_MAPMOD  "cmd_map_mod"    //��ͼ����޸�
#define CMD_MAPPOINTADD "cmd_mappoint_add" //��ͼ�ȵ�����
#define CMD_MAPPOINTDEL "cmd_mappoint_del" //��ͼ�ȵ�ɾ��
#define CMD_MAPPOINTMOD "cmd_mappoint_mod" //��ͼ�ȵ��޸�
#define CMD_MAPDOWNLOAD "cmd_map_download" //��ͼ��Ϣ����
#define CMD_MAPPOINTDOWNLOAD "cmd_mappoint_download" //��ͼ�ȵ���Ϣ����
#define ATTRIB_MAP_ID				"id"
#define ATTRIB_MAP_REGIONID			"regionid"
#define ATTRIB_MAP_FILEHASH			"filehash"
#define ATTRIB_MAP_FILEEXT			"fileext"
#define ATTRIB_MAP_PUBTIME			"pubtime"
#define ATTRIB_MAP_CHNID			"chnid"
#define ATTRIB_MAP_DEVID			"devid"
#define ATTRIB_MAP_UID				"userid"
#define ATTRIB_MAP_LENGHT			"mapfilelen"
#define ATTRIB_MAP_FILECONTENT		"mapfilecontent"

#define ATTRIB_MAP_PARENTID				"parentid"
#define ATTRIB_MAP_NAME				"name"
#define ATTRIB_MAP_LEVEL				"level"
#define ATTRIB_MAP_FILENAME				"filename"
#define ATTRIB_MAPPOINT_ID				"id"
#define ATTRIB_MAPPOINT_CID             "cid"
#define ATTRIB_MAPPOINT_EID				"eid"
#define ATTRIB_MAPPOINT_MAPID				"mapid"
#define ATTRIB_MAPPOINT_TYPE				"type"
#define ATTRIB_MAPPOINT_NAME              "name"
#define ATTRIB_MAPPOINT_X				"x"
#define ATTRIB_MAPPOINT_Y				"y"
#define ATTRIB_MAP_OK                   "ok"
#define ATTRIB_MAP_DOWNTYPE             "downtype" //�Ƿ�����ͬʱ��ͼ�ļ�
#define ATTRIB_MAP_OBJTYPE             "objtype" //�ǵ�ͼ���ǵ�ͼ�ȵ����
#endif
//  [5/24/2008]

// Added �������  [4/10/2009 lgx]
#define ATTRIB_MATRIX_TYPE				"mtx_type"
#define ATTRIB_MATRIX_INPUT				"mtx_in"
#define ATTRIB_MATRIX_OUTPUT			"mtx_out"
#define ATTRIB_MATRIX_CHNNAME			"mtx_chn_name"
#define ATTRIB_MATRIX_TRADENAME        "mtx_trade_name"
//

/************************************************************************/
//����������
#define CMD_UPDATE_AVCON_STATUS				"cmd_upd_avc_sta"/*����DVR��AVCON����״̬*/
#define CMD_MATRIX_CTRL						"cmd_matrix_ctrl"/*�������*/

//�������
static const char* UNCHECK_COMMANDS[] = { 
	CMD_MATRIX_CTRL,
	CMD_UPDATE_AVCON_STATUS,
}; 
#define UNCHECK_COMMAND(cmd) \
{\
	for (int i=0; i<sizeof(UNCHECK_COMMANDS)/sizeof(const char*); i++) \
	{\
		if (strcmp(cmd, UNCHECK_COMMANDS[i]) == 0)\
		{\
			return TRUE;\
		}\
	}\
}
/************************************************************************/


/************************************************************************/
/* NDVR�����ֶ�                                                         */
/************************************************************************/
// �ֶ� [5/24/2008]
#define XNCP_SYS							"XNCP-SYS"
#define ATTRIB_RD							"RD"
#define ATTRIB_CMD_ID						"cmdid"
#define ATTRIB_CMD_TYPE						"cmd_type"
#define ATTRIB_FROMID						"fromid"
#define ATTRIB_BUID							"buid"
#define DATA_ROOT							"dat_r"
#define DATA_ITEM							"dat_i"
#define DATA_ITEM_COUNT						"dat_i_c"
#define ATTRIB_ITEM_COUNT					"item_c"
#define ATTRIB_ENCODER_ID					"codid"
#define ATTRIB_CHANNEL_ID					"chnid"
#define ATTRIB_CHANNEL_INDEX				"chnid"
#define ATTRIB_CHANNEL_NUM					"chn_n"
#define ATTRIB_CHANNEL_NAME					"chnname"
#define ATTRIB_CHANNEL_AUDIOID				"chn_a_i"
#define ATTRIB_CHANNEL_VIDEOID				"chn_v_i"
#define ATTRIB_CHANNEL_SUBVIDEOID			"chn_s_v_i"
#define ATTRIB_CHANNEL_STATE				"chn_s"
#define ATTRIB_CHANNEL_RECORDSTATE			"chn_r_s"
#define ATTRIB_CHANNEL_RECORDSTATE_YPTE		"chn_r_s_t"
#define ATTRIB_CHANNEL_ALARMSTATE			"chn_a_s"
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE		"chn_a_s_t"
#define ATTRIB_CHANNEL_BASIC_STATE			"chn_b_s"
#define ATTRIB_CHANNEL_RECORD_STATE			"chn_r_s"
#define ATTRIB_CHANNEL_ALARM_STATE			"chn_a_s"
#define ATTRIB_CHANNEL_FUNCTION_STATE		"chn_f_s"
#define ATTRIB_PACKET_ID					"pakid"			
#define ATTRIB_PAGE							"pag"	
#define ATTRIB_PAGE_TOTAL					"pag_t"
#define ATTRIB_PAGESIZE						"pag_s"
#define ATTRIB_KEY							"sch_k"	
#define ATTRIB_VALUE						"sch_v"	
#define ATTRIB_ERROR_CODE					"err_c"	
#define ATTRIB_MSG							"msg"
#define ATTRIB_RECORD_TYPE					"rec_t"
#define ATTRIB_ALERT_TYPE					"alm_t"
#define ATTRIB_MASK							"mask"
#define ATTRIB_IP							"ip"
#define ATTRIB_PORT							"port"
#define ATTRIB_NAME							"name"
#define ATTRIB_TYPE							"type"
#define ATTRIB_ALARM_BYPRESETNO				"preid"
#define ATTRIB_DEFENCE_DIS_NAME 			"def_d_n"
#define ATTRIB_PRE_POINT_NUM 				"pre_p_c"
#define ATTRIB_PRE_POINT_NAME 				"pre_p_n"
#define ATTRIB_PRE_POINT_IF_DEFAULT			"pre_p_i_d"
#define ATTRIB_GET_DEF_PRESET 				"pre_d_p"
#define ATTRIB_LOCAL_ADDR					"loc_addr"
#define ATTRIB_TCPPORT						"port_tcp"
#define ATTRIB_UDPPORT						"port_udp"
#define ATTRIB_MCUID						"mcu_i"
#define ATTRIB_MCUADDR						"mcu_a"
#define ATTRIB_MCUPORT						"mcu_p"
#define ATTRIB_MCUTYPE						"mcu_t"
#define ATTRIB_MCURLS_CHCNT					"mcu_r_c"
#define ATTRIB_PTZ_CMD						"ptz_c"
#define ATTRIB_BLANK						"blank"
#define ATTRIB_RESULT						"result"
#define ATTRIB_RESULT_SUCCESS				"success"
#define ATTRIB_RESULT_FAILUER				"failure"
#define ATTRIB_RESULT_FINISH				"finish"

#define ATTRIB_TREEPID						"tre_pid"	
#define ATTRIB_TREENAME						"tre_name"
#define ATTRIB_TREEID						"tre_id"
#define ATTRIB_TREENODETYPE					"tre_nt"
#define ATTRIB_TREESTATUS					"tre_st"

#define ATTRIB_REGION						"region"
#define ATTRIB_RIGHT						"right"
#define ATTRIB_RIGHTNAME					"rgh_n"
#define ATTRIB_USER							"user"
#define ATTRIB_PSW							"psw"
#define ATTRIB_USERTYPE						"usr_t"
#define ATTRIB_AVCON_IPINFO_TYPE			"opt_t" /*0 ��ȡ 1����*/
#define ATTRIB_FILE_ID						"filid"
#define ATTRIB_FILE_SIZE					"fil_s"	
#define ATTRIB_FILE_URL						"fil_u"
#define ATTRIB_FILE_TYPE					"fil_t"
#define ATTRIB_FILE_PATH					"fil_p"
#define ATTRIB_START_TIME					"bgn_t"
#define ATTRIB_END_TIME						"end_t"
#define ATTRIB_OPERLOG_ID 					"logid"
#define ATTRIB_OPERLOG_DATETIME				"log_t"
#define ATTRIB_OPERLOG_USER					"log_u"
#define ATTRIB_OPERLOG_ACTION				"log_a"
#define ATTRIB_OPERLOG_TAG					"log_d"
#define ATTRIB_NORMAL_PRERECORD				"pre_r"
#define ATTRIB_NORMAL_PRERECORDTIME			"pre_t"
#define ATTRIB_NORMAL_FILECOMMINUTE			"fil_c"
#define ATTRIB_ALARM_PRERECORD				"alm_p_r"
#define ATTRIB_ALARM_PRERECORDTIME			"alm_p_r_t"
#define ATTRIB_ALARM_FILECOMMINUTE			"alm_f_c"
#define ATTRIB_FILECOMMINUTEMODE			"fil_c_m"
#define ATTRIB_FILECOMMINUTETIME			"fil_c_t"
#define ATTRIB_FILECOMMINUTESIZE			"fil_c_s"
#define ATTRIB_CYCLEMODE					"cyl_m"
#define ATTRIB_RECORDCHANNELS				"rec_c_n"
#define ATTRIB_ALERTMODE					"alm_m"
#define ATTRIB_DISKFULLAUTONEXT				"dsk_f_a_n"
#define ATTRIB_RECORDDRIVERS				"rec_d_n"
#define ATTRIB_DISKMINSPACE					"dsk_m_s"
#define ATTRIB_COVERFILEDAYS				"cov_f_d"
#define ATTRIB_DISKFULLALERT				"dsk_f_a"
#define ATTRIB_CURRENTRECORDDRIVER			"rec_c_d"
#define ATTRIB_AUTOUPLOADRECORD				"rec_a_u"
#define ATTRIB_UPLOADRSSIP0					"upl_i_0"
#define ATTRIB_UPLOADRSSIP1					"upl_i_1"
#define ATTRIB_UPLOADRSSIP2					"upl_i_2"
#define ATTRIB_UPLOADRSSPORT				"upl_r_p"
#define ATTRIB_FIXEDDRIVERCOUNT				"fix_dr_c"
#define ATTRIB_FIXEDDRIVER					"fix_d"
#define ATTRIB_FIXEDDRIVERTOTAL				"fix_d_t"
#define ATTRIB_FIXEDDRIVERFREE				"fix_d_f"
#define ATTRIB_ALARM_UPLOAD_CENTER			"alm_u_c"
#define ATTRIB_ALARM_TRIGGR_RECORD			"alm_t_r"
#define ATTRIB_ALARM_LNKAGE_CAMERA			"alm_l_c"
#define ATTRIB_ALARM_OUTPUT_DEVICE			"alm_o_d"
#define ATTRIB_ALARM_VIDEO_LOST				"alm_v_l"
#define ATTRIB_ALARM_VIDEO_HIDE				"alm_v_h"
#define ATTRIB_UART_PORT					"com_p"
#define ATTRIB_BAUD_RATE					"com_b_r"
#define ATTRIB_DATA_BIT						"com_d_b"
#define ATTRIB_STOP_BIT						"com_s_b"
#define ATTRIB_PROV_MODE					"com_p_m"
#define ATTRIB_INFO_INPUT                   "com_i_i"
#define ATTRIB_INFO_OUTPUT                  "com_i_o"
#define ATTRIB_INFO_NAME                    "com_i_n"
#define ATTRIB_INFO_CINDEX                  "com_i_ci"

#define ATTRIB_PLANRUNNING					"pln_r"
#define ATTRIB_CHANNELS						"rec_c_n"
#define ATTRIB_FULLLDAY						"i_f_d"
#define ATTRIB_FULLDAYRECTYPE				"f_d_r_t"
#define ATTRIB_TIMERANGNUMBER				"t_r_n"
#define ATTRIB_WEEKDAY0						"w_d_0"
#define ATTRIB_DAYRCETYPE0					"d_r_t_0"
#define ATTRIB_WEEKDAY1						"w_d_1"
#define ATTRIB_DAYRCETYPE1					"d_r_t_1"
#define ATTRIB_WEEKDAY2						"w_d_2"
#define ATTRIB_DAYRCETYPE2					"d_r_t_2"
#define ATTRIB_WEEKDAY3						"w_d_3"
#define ATTRIB_DAYRCETYPE3					"d_r_t_3"
#define ATTRIB_WEEKDAY4						"w_d_4"
#define ATTRIB_DAYRCETYPE4					"d_r_t_4"
#define ATTRIB_WEEKDAY						"w_d"
#define ATTRIB_BEGIN						"begin"
#define ATTRIB_END							"end"
#define ATTRIB_DESC							"desc"
#define ATTRIB_LOGID						"logid"
#define ATTRIB_STATUS						"status"
#define ATTRIB_FROM							"from"
#define ATTRIB_BEFORE						"before"
#define ATTRIB_ENCODER_NAME					"cod_n"	
#define ATTRIB_ENCODER_TYPE					"cod_t"	
#define ATTRIB_CHANN_COUNT					"chan_count"	//ͨ������
#define ATTRIB_NVS_NAME						"nvs_n"
#define ATTRIB_NVS_IP						"nvsip"
#define ATTRIB_NVS_PORT						"nvs_p"
#define ATTRIB_NVS_UID						"nvs_u_i"
#define ATTRIB_NVS_PWD						"nvs_u_p"
#define ATTRIB_NVS_STATE					"nvs_s"
#define ATTRIB_AVCONCE_NAME					"avc_n"	
#define ATTRIB_AVCONCE_IP					"avcip"
#define ATTRIB_AVCONCE_PORT					"avc_p"
#define ATTRIB_AVCONCE_UID					"avc_u_i"
#define ATTRIB_AVCONCE_PWD					"avc_u_p"
#define ATTRIB_AVCONCE_STATE				"avc_s"	
#define ATTRIB_ENCODER_SERIAL				"cod_s"
#define ATTRIB_ENCODER_VERSION				"cod_v"
#define ATTRIB_TAG							"tag"
#define ATTRIB_CAPTURE_NUMBER				"chn_c_n"
#define ATTRIB_VIDEO_TOMCU					"vid_t_m"
#define ATTRIB_VIDEO_SUBSTREAM				"vid_s_s"
#define ATTRIB_VIDEO_HWND					"vid_h"	
#define ATTRIB_PARENT_HWND					"vid_p_h"
#define ATTRIB_VIDEO_DEVICE					"vid_dev"	
#define ATTRIB_VIDEO_DENOISE				"vid_den"
#define ATTRIB_VIDEO_HDDEVICE				"vid_hdd"
#define ATTRIB_VIDEO_CODEC					"vid_c"
#define ATTRIB_VIDEO_RESOLUTION				"vid_r"
#define ATTRIB_VIDEO_FRAMERATE				"vid_f_r"
#define ATTRIB_VIDEO_FRAMETYPE				"vid_f_t"
#define ATTRIB_VIDEO_QUALITY				"vid_q"
#define ATTRIB_VIDEO_AUTOQUALITY			"vid_a_q"
#define ATTRIB_VIDEO_CODEC_SUB				"vid_c_s"
#define ATTRIB_VIDEO_RESOLUTION_SUB			"vid_r_s"
#define ATTRIB_VIDEO_FRAMERATE_SUB			"vid_f_r_s"
#define ATTRIB_VIDEO_FRAMETYPE_SUB			"vid_f_t_s"
#define ATTRIB_VIDEO_QUALITY_SUB			"vid_q_s"
#define ATTRIB_VIDEO_AUTOQUALITY_SUB		"vid_a_q_s"
#define ATTRIB_VIDEO_QUARTERSIZE_SUB		"vid_q_s_s"
#define ATTRIB_NDCI_BITRATETYPE				"ndc_b"
#define ATTRIB_NDCI_STREAMTYPE				"ndc_s"
#define ATTRIB_NDCI_RESOLUTION				"ndc_r"
#define ATTRIB_NDCI_PICQUALITY				"ndc_p"
#define ATTRIB_NDCI_VIDEOFRAMERATE			"ndc_v_f_r"
#define ATTRIB_NDCI_VIDEOBITRATE			"ndc_v_b"
#define ATTRIB_NDCI_BITRATETYPE_SUB			"ndc_b_t_s"
#define ATTRIB_NDCI_STREAMTYPE_SUB			"ndc_s_t_s"
#define ATTRIB_NDCI_RESOLUTION_SUB			"ndc_r_s"
#define ATTRIB_NDCI_PICQUALITY_SUB			"ndc_p_s"
#define ATTRIB_NDCI_VIDEOFRAMERATE_SUB		"ndc_v_f_r_s"
#define ATTRIB_NDCI_VIDEOBITRATE_SUB		"ndc_v_b_s"
#define ATTRIB_DAHUA_CHANNEL_INFO			"dh_c_i"
#define ATTRIB_DAHUA_IMAGE_SIZE_MASK		"dh_i_s_m"
#define ATTRIB_DAHUA_ENCODE_MODE_MASK		"dh_e_m_m"
#define ATTRIB_AUDIO_DEVICE					"aud_d"
#define ATTRIB_AUDIO_CODEC					"aud_c"
#define	ATTRIB_AUDIO_VAD					"aud_v"
#define ATTRIB_AUDIO_AGC					"aud_ag"
#define ATTRIB_AUDIO_AEC					"aud_ae"
#define ATTRIB_AUDIO_CAPTURE_CARD			"aud_c_c"
#define ATTRIB_AUDIO_CAPTURE_MODE			"aud_c_m"
#define ATTRIB_VIDEOID						"vidid"
#define ATTRIB_SUBVIDEOID					"vidid_s"
#define ATTRIB_AUDIOID						"audid"
#define ATTRIB_PROTOCOLID					"ptz_p_t"
#define ATTRIB_PTZADDR						"ptz_a"
#define ATTRIB_PTZCOMPORT					"ptz_u_p"
#define ATTRIB_DEFPRESETDELAY				"ptz_d_l"
#define ATTRIB_SPEED						"ptz_s"
#define ATTRIB_BAUD							"ptz_b"
#define ATTRIB_PTZ_PROTOCOL_TYPE			"ptz_p_t"
#define ATTRIB_PTZ_UART_PORT				"ptz_u_p"
#define ATTRIB_PTZ_ADDRESS					"ptz_a"
#define ATTRIB_PTZ_SET_TYPE					"ptz_s_t" 
#define ATTRIB_BRIGHTNESS					"brig"
#define ATTRIB_BRIGHTNESS_MAX				"brig_max"
#define ATTRIB_BRIGHTNESS_MIN				"brig_min"
#define ATTRIB_CONTRAST						"cont"
#define ATTRIB_CONTRAST_MAX					"cont_max"
#define ATTRIB_CONTRAST_MIN					"cont_min"
#define ATTRIB_SATURATION					"satu"
#define ATTRIB_SATURATION_MAX				"satu_max"
#define ATTRIB_SATURATION_MIN				"satu_min"
#define ATTRIB_HUE							"hue"
#define ATTRIB_HUE_MAX						"hue_max"
#define ATTRIB_HUE_MIN						"hue_min"
#define ATTRIB_GAMA							"gama"		//٤��
#define ATTRIB_GAMA_MAX						"gama_max"		//٤��
#define ATTRIB_GAMA_MIN						"gama_min"		//٤��
#define ATTRIB_SHARPEN						"sharpen"
#define ATTRIB_SHARPEN_MAX					"sharpen_max"
#define ATTRIB_SHARPEN_MIN					"sharpen_min"


#define ATTRIB_DEFAULT_VIDEO_PARAM			"def_vid_p"
#define ATTRIB_KEY_VALUE					"key_v"
#define ATTRIB_COPY_TO_EID					"cp_t_e"
#define ATTRIB_COPY_TO_CID					"cp_t_c" 
#define ATTRIB_COPY_TO_WEEK					"cp_t_w"
#define ATTRIB_AUTO_RETURN					"ptz_auto_r"		//�Զ���λ
#define ATTRIB_RETURN_TIME					"ptz_auto_r_t"		//��λʱ��
#define ATTRIB_CTRL_TYPE					"ptz_ctrl_t"		//�������� 0:��ͨ 1:ת��Ԥ�õ� 2:����Ԥ�õ� 3:���Ԥ�õ� 4:ȡԤ�õ�(����Ч)

#define ATTRIB_AUDIO_TRANS_TYPE				"aud_trans_t" //0x01:TCP,0x02:UDP,0x04:RTP
#define ATTRIB_VIDEO_TRANS_TYPE				"vid_trans_t" //0x01:TCP,0x02:UDP,0x04:RTP



// [3/27/2009 lzy]
//�����ƶ��������ĺ궨��
#define ATTRIB_MOVE_DETECT_NUM				"JKRectNum"		//һ���м������� ���5�� 
#define ATTRIB_MOVE_DETECT_BEGIN_HOUR		"JK_Begin_Hour"
#define ATTRIB_MOVE_DETECT_BEGIN_MINUTE		"JK_Begin_Minute"
#define ATTRIB_MOVE_DETECT_END_HOUR			"JK_End_Hour"
#define ATTRIB_MOVE_DETECT_END_MINUTE		"JK_End_Minute"
#define ATTRIB_MOVE_DETECT_GRADE			"JK_Grade"	//�������ʲô�õ� 
#define ATTRIB_MOVE_DETECT_RECT				"JK_Rect"	 //����������� ��ʽΪ bottom,left,right,top �ո� bottom,left,right,top �ո�
#define ATTRIB_MOVE_DETECT_TYPE				"WhichKindOfDis"	//0:�ƶ���� 1:�ڸ�
#define ATTRIB_MOVE_DETECT_START			"JK_BStarted"


#define ATTRIB_AVIEW_SET					"aview_set"	//���Ӵ��ֶ���Ϊ���ж�����NDVRʱ���ǲ��Ǵ�aview��������Զ������
#define ATTRIB_VIDEO_DEV_NUMBER				"VideoDeviceNumber" 
#define ATTRIB_RESOLUTION_NUMBER			"ResolutionNumber" 
#define ATTRIB_QUALITY_NUMBER				"QualityNumber" 
#define ATTRIB_IS_PRESET					"iIsPreSet"

/************************************************************************/
/* NDVR���Ͷ���                                                         */
/************************************************************************/
// ���Ͷ���[5/24/2008]

//����(����������)
#define	HPNVS_XMC	0/*��ƽ*/
#define HPNVS_HIK	1/*����*/
#define HPNVS_DHD   2/*��*/
#define HPNVS_NZI	3/*����*/

#define IS_HPD(dev_type)			(dev_type == HPNVS_XMC)
#define IS_HIK(dev_type)			(dev_type == HPNVS_HIK)
#define IS_DHD(dev_type)			(dev_type == HPNVS_DHD)
#define IS_NZI(dev_type)			(dev_type == HPNVS_NZI)

//�㲥
#define BROADCAST ""
#define BROADCAST_LDVR "bc_ldvr"/*�豸�˽���*/
#define BROADCAST_NDVR "bc_ndvr"/*NDVR�ͻ���*/
#define BROADCAST_DVR  BROADCAST_LDVR BROADCAST_NDVR/*NDVR+LDVR*/
#define BROADCAST_REMOTE "bc_remote"/*avcon�ͻ���*/

//�жϿͻ�������
#define IS_BROADCAST(from_id)			(from_id.find("$")==-1)/*�㲥*/
#define IS_BROADCAST_LDVR(from_id)		(from_id == BROADCAST || from_id.find(BROADCAST_LDVR)!=-1)/*�㲥���豸����*/
#define IS_BROADCAST_NDVR(from_id)		(from_id == BROADCAST || from_id.find(BROADCAST_NDVR)!=-1)/*�㲥��NDVR�ͻ�*/
#define IS_BROADCAST_REMOTE(from_id)	(from_id == BROADCAST || from_id.find(BROADCAST_REMOTE)!=-1)/*�㲥��AVCON�ͻ�*/
#define IS_LDVR_CLIENT(from_id)			(from_id.find("$0$")!=-1)/*�豸�˽���*/
#define IS_NDVR_CLIENT(from_id)			(from_id.find("$2$")!=-1)/*NDVR�ͻ���*/
#define IS_REMOTE_CLIENT(from_id)		(from_id.find("$3$")!=-1)/*AVCON�ͻ���*/

//�û�����
#define GROUP_ADMINISTRATOR				0/*����Ա*/
#define GROUP_OPERATOR					1/*����Ա*/
#define GROUP_VIEWER					2/*�࿴Ա*/

//��¼�������
#define ERR_UID					-1	//�û���ƥ��
#define ERR_PWD					-2	//���벻ƥ��
#define ERR_LOCK				-3	//�û�������
#define ERR_LOGINED				-5	//�˻��ѵ�¼

#define ERR_TRUE				1/*��ֵ*/
#define ERR_FALSE				0/*��ֵ*/
#define ERR_NULL				-1/*��ָ��*/
#define ERR_UNREADY				-2/*δ׼���õ�*/
#define ERR_PARAM				-3/*��������*/
#define ERR_DB					-4/*���ݿ����*/
#define ERR_EXIST				-5/*�Ѵ���*/
#define ERR_UNKNOW				-99/*δ֪����*/

//����������
#define SERVER_RSS				"RSS"
#define SERVER_MCU				"MCU"


//NDVR�豸�ͻ���
#define WM_PLAYER_OSD_TEXT				(WM_USER + 0x331)
#define WM_PLAYER_CAPTURETOFILE			(WM_USER + 0x332)
#define WM_AUDIOVIDEO_BITRATE_CHANGED	(WM_USER + 0x333)

//��ͼ�ȵ�����
#define ATTRIB_MAPHOTTYPE_SEHXIANG 1 //����ͷ/ͨ��
#define ATTRIB_MAPHOTTYPE_YUZHI 2 //Ԥ�õ�
#define ATTRIB_MAPHOTTYPE_YUNTAI 3 //��̨
#define ATTRIB_MAPHOTTYPE_BIQIAN 4 //��ǩ
#define ATTRIB_MAPHOTTYPE_TISHI 5 //��ʾ


#define PTZ_SPEED 0	//�ٶ�
#define PTZ_OTHER 1 //��������

#define CMD_UP			101   //����
#define CMD_STOP_UP		201   
#define CMD_DOWN		102   //����
#define CMD_STOP_DOWN   202
#define CMD_LEFT		103	  //����
#define CMD_STOP_LEFT	203	  
#define CMD_RIGHT		104   //����
#define CMD_STOP_RIGHT  204
#define CMD_DN			105   //����
#define CMD_STOP_DN     205
#define CMD_DF			106   //���Զ
#define CMD_STOP_DF     206
#define CMD_FS			107   //�佹С
#define CMD_STOP_FS		207
#define CMD_FB			108   //�佹��
#define CMD_STOP_FB     208
#define CMD_HS			109   //��ȦС
#define CMD_STOP_HS		209
#define CMD_HB			110   //��Ȧ��
#define CMD_STOP_HB     210
#define CMD_AUTO		111	  //�Զ�	
#define CMD_STOP_AUTO	211

//Ԥ�õ�(��)
#define CMD_PRESET		212		//����Ԥ�õ�
#define CMD_CLR_PRESET	213		//���Ԥ�õ�
#define CMD_GOTO_PRESET	214		//����Ԥ�õ�
#define CMD_AUTO_SCAN   218		//�Զ�Ѳ�� == CMD_PATROL_RUN

//Ԥ�õ�(ԭ��)
#define CMD_PREST_SET	301  
#define CMD_PREST_CALL  401		


//Ѳ������:Pattern
#define CMD_PATROL_START 216
#define CMD_PATROL_STOP	217
#define CMD_PATROL_RUN	218 //ֹͣѲ���ɵ���������һ����������

//����
#define CMD_SET_SPEED		220
#define CMD_SET_PRESET_NAME 221

#define CHANNEL_MAX_NUM			32
#define DEFAULTBRIGHTNESS 6
#define DEFAULTCONTRAST 6
#define DEFAULTSATURATION  6
#define DEFAULTHUE 6
#define DEFAULTVOLUME 50

// + ����� for ������ʯ�ͼ��ϵͳ�������ؿ�����������¼����� by lgx [4/21/2008]
#define SK_NUM1				1 	//����1
#define SK_NUM2				2 	//����2
#define SK_NUM3				3 	//����3
#define SK_NUM4				4 	//����4
#define SK_NUM5				5 	//����5
#define SK_NUM6				6 	//����6
#define SK_NUM7				7 	//����7
#define SK_NUM8				8 	//����8
#define SK_NUM9				9 	//����9
#define SK_NUM0				10	//����0

#define SK_POWER			12	//��Դ
#define SK_MENU				13	//�˵�
#define SK_ENTER			14	//ȷ��/����
#define SK_ESC				15	//ȡ��/�˳�
#define SK_EDIT				16	//�༭

#define SK_PLAY				18	//�ط�/Ԥ��
#define SK_REC				19  //¼��
#define SK_PAN				20	//PAN
#define SK_MULTI			21  //�໭��
#define SK_INPUT			22	//���뷨
#define SK_INTERPHONE 		23	//�Խ�
#define SK_SYSINFO			24	//ϵͳ��Ϣ
#define SK_FORWARD			25  //���
#define SK_BACKWARD 		26	//����

#define SK_UP				28	//��
#define SK_UP_END			29	//�Ͻ���
#define SK_DOWN				30	//��
#define SK_DOWN_END 		31	//�½���
#define SK_LEFT				32	//��
#define SK_LEFT_END 		33	//�����
#define SK_RIGHT			34	//��
#define SK_RIGHT_END 		35	//�ҽ���

#define SK_ADD_APERTURE		37	//��Ȧ+
#define SK_ADD_APERTURE_END 38	//��Ȧ+����
#define SK_SUB_APERTURE		39	//��Ȧ-
#define SK_SUB_APERTURE_END 40	//��Ȧ-����

#define SK_ADD_FOCUS		42	//�ۼ�+
#define SK_ADD_FOCUS_END	43	//�ۼ�+����
#define SK_SUB_FOCUS		44	//�ۼ�-
#define SK_SUB_FOCUS_END	45	//�ۼ�-����

#define SK_ADD_MULTIPLE		47	//�䱶+
#define SK_ADD_MULTIPLE_END	48	//�䱶+����
#define SK_SUB_MULTIPLE		49	//�䱶-
#define SK_SUB_MULTIPLE_END	50	//�䱶-����

#define CHANNEL_STATE_OFFLINE		0 //����
#define CHANNEL_STATE_ONLINE		1 //����
#define CHANNEL_STATE_PREVIEW		2 //Ԥ��
#define CHANNEL_STATE_ALERT			3 //����
#define CHANNEL_STATE_SELECT		4 //ѡ��

// ����״̬
#define DEV_DISCONNECTED			0 // δ����
#define DEV_CONNECTED				1 // ����

//AVCON status type
#define AVCON_LOGIN_STATUS		1/*��¼*/
#define AVCON_LOGOUT_STATUS		2/*ע��*/
#define AVCON_CONNECT_STATUS	3/*����*/

//1�����к��������ʺŰ�
//2���ʺŲ�����
//3�������
//4�����кŲ���ȷ
//5���ʺ��ѵ�¼
//������δ֪����
//AVCON login status
#define LOGIN_SUCCESSED			0/*��¼�ɹ�*/
#define LOGIN_THE_SAME_ACCOUNT	1/*���к��������ʺŰ�*/
#define LOGIN_ACCOUNT_ERROR		2/*�ʺŲ�����*/
#define LOGIN_PASSWORD_ERROR	3/*�����*/
#define LOGIN_SERIAL_ERROR		4/*���кŲ���ȷ*/
#define LOGIN_ACCOUNT_LOGINED	5/*�ʺ��ѵ�¼*/
#define LOGIN_SYSTEM_ERROR		99/*ϵͳ����*/

//AVCON logout status
#define LOGOUT_SUCCESSED		0/*ע���ɹ�*/
#define LOGOUT_DISCONNECTED		1/*ϵͳ�Ͽ�*/
#define LOGOUT_SYSTEM_ERROR		99/*ϵͳ����*/

//AVCON connect status
#define HPMON_CONNECTING		10/*��������...*/
#define HPMON_RECONNECTED		11/*�����ɹ�*/
#define HPMON_BUSY				12/*������æ...*/
#define HPMON_FAILED			13/*����ʧ��*/	
#define HPMON_CONNECTED			14/*���ӳɹ�*/
#define HPMON_DISCONNECTED		15/*�Ͽ�����*/

//AVCON device online
#define ONLINE_SUCCESSED		0
#define ONLINE_THE_SAME_ACCOUNT	1/*���к��������ʺŰ�*/
#define ONLINE_ACCOUNT_ERROR	2/*�ʺŲ�����*/
#define ONLINE_PASSWORD_ERROR	3/*�����*/
#define ONLINE_SERIAL_ERROR		4/*���кŲ���ȷ*/
#define ONLINE_ACCOUNT_LOGINED  5/*�ʺ��ѵ�¼*/
#define ONLINE_SYSTEM_ERROR		99/*ϵͳ����*/

#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_ADD		1
#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_SET		2
#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_DEL		3
#define CHANNEL_RECSTATE_NONE					0	// ��¼��
#define CHANNEL_RECSTATE_SCHEME_TIME			1	// �ƻ���ʱ¼��
#define CHANNEL_RECSTATE_SCHEME_MOVE			2   // �ƻ�����¼��
#define CHANNEL_RECSTATE_SCHEME_ALARM			3   // �ƻ�����¼��
#define CHANNEL_RECSTATE_SCHEME_MOVEOALARM		4   // �ƻ�����|����¼��
#define CHANNEL_RECSTATE_SCHEME_MOVEAALARM		5	// �ƻ�����&����¼��
#define CHANNEL_RECSTATE_NORMAL_MANUAL			6	// �����ֶ�¼��
#define CHANNEL_RECSTATE_NORMAL_MOVE			7   // ���涯��¼��
#define CHANNEL_RECSTATE_NORMAL_ALARM			8   // ���汨��¼��
#define CHANNEL_RECSTATE_NORMAL_PRE				9   // ����Ԥ¼��
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_ADD		1
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_SET		2
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_DEL		3
#define CHANNEL_ALARMSTATE_NONE					0	// �ޱ���
#define CHANNEL_ALARMSTATE_DISKFULL				1	// ������
#define CHANNEL_ALARMSTATE_VIDEOLOST			2   // ��Ƶ��ʧ
#define CHANNEL_ALARMSTATE_MOVEDETECT			3   // �ƶ����
#define CHANNEL_ALARMSTATE_HOSTALARM			4   // ����IO
#define CHANNEL_ALARMSTATE_MOVEOALARM			5   // ����|IO
#define CHANNEL_ALARMSTATE_MOVEAALARM			6	// ����&IO
#define CHANNEL_ALARMSTATE_VIDEOHIDE			7   // ��Ƶ�ڵ�

enum
{
	KIND_OF_DIS_MOVE_DETECT,	// �ƶ�֡��
	KIND_OF_DIS_MASK_ALERT		// ��Ƶ�ڵ�
};

// ¼������
typedef enum 
{
	RT_ALL			=-1,// ȫ��
	RT_SCHEDULE		=0,	// ��ʱ¼��
	RT_MANUAL		=1,	// �ֶ�¼��
	RT_MOVEDETECT	=2,	// �ƶ�֡��
	RT_ALARM		=3,	// ����¼��
	RT_MOVEORALARM	=4,	// ����|����
	RT_MOVEANDALARM	=5 // ����&����
}RECORD_TYPE;
#define RT_MAXID RT_ALARM+1

// ��������
typedef enum 
{
	AT_ALL			=-1,// ȫ��
	AT_DISKFULL		=0,	// ���̿ռ���
	AT_VIDEOLOST	=1,	// ��Ƶ��ʧ
	AT_MOVEDETECT	=2,	// �ƶ�֡��
	AT_ALARM		=3,	// ��������
	AT_MOVEORALARM	=4,	// ֡��|����
	AT_MOVEANDALARM	=5,	// ֡��&����
	AT_VIDEOHIDE	=6	// ��Ƶ�ڵ�
}ALERT_TYPE;
#define AT_MAXID  AT_ALARM+1

//���̲��㱨������
typedef enum
{
	AT_DISKFULL_ACYLIC = 0,					// ��ѭ��¼����̿ռ�������
	AT_DISKFULL_CYCLE_NORMAL_COVER_LOST,	// ѭ��¼����̿ռ������渲��ʧ��
	AT_DISKFULL_CYCLE_COMPEL_COVER_LOST	// ѭ��¼����̿ռ���ǿ�Ƹ���ʧ��
}ALERT_DISKFULL_TYPE;


//ͨ������״̬
typedef enum
{
	CBST_OFFLINE	= 0x00,/*����*/
	CBST_ONLINE		= 0x01,/*����*/
	CBST_PREVIEW	= 0x02,/*Ԥ��*/
	CBST_RECORD		= 0x04,/*¼��*/
	CBST_ALARM		= 0x08/*����*/
}CHANNEL_BASIC_STATE_TYPE;

//ͨ��¼��״̬
typedef enum
{
	CRST_SCHEDULE		= 0x01,/*��ʱ¼��*/
	CRST_MANUAL			= 0x02,/*�ֶ�¼��*/
	CRST_MOTION_DETECT	= 0x04,/*�ƶ����*/
	CRST_DEFENCE_IO		= 0x08/*����IO����*/
}CHANNEL_RECORD_STATE_TYPE;

//ͨ������״̬
typedef enum
{
	CAST_DISK_FULL		= 0x01,/*��������*/
	CAST_VIDEO_LOST		= 0x02,/*�źŶ�ʧ*/
	CAST_MOTION_DETECT	= 0x04,/*�ƶ����*/
	CAST_DEFENCE_IO		= 0x08,/*����IO*/
	CAST_VIDEO_COVER	= 0x10/*��Ƶ�ڵ�*/
}CHANNEL_ALARM_STATE_TYPE;

//��Ƶ����״̬
typedef enum
{
	VFST_MOTION_DETECT	= 0x01,/*�ƶ�֡��*/
	VFST_VIDEO_HIDE		= 0x02,/*��Ƶ����*/
	VFST_VIDEO_LOST		= 0x04,/*��Ƶ��ʧ*/
	VFST_VIDEO_COVER	= 0x08/*��Ƶ�ڵ�*/
}VIDEO_FUNCTION_STATE_TYPE;

//ͨ��״̬����
typedef enum
{
	CST_BASIC		= 0x01,/*����״̬*/
	CST_RECORD		= 0x02,/*¼��״̬*/
	CST_ALARM		= 0x04,/*����״̬*/	
	CST_FUNCTION	= 0x08/*����״̬*/
}CHANNEL_STATE_TYPE;


//  [5/24/2008]
/************************************************************************/
/* NDVR�ṹ��                                                           */
/************************************************************************/



typedef struct _Group
{
	int nId;
	char name[128];
	int nType ;
	int nParentId ;
}GROUPINFO,FAR * LPGROUPONFO;



typedef struct tagWEEKDAYVIDEOPLAN_INFO
{
public:
	tagWEEKDAYVIDEOPLAN_INFO(){
		nFullDay     = 0;
		nRecType     = 0;
		nTimeNum     = 0;
		cWeekDay0    = _T("");
		nDayRecType0 = 0;
		cWeekDay1    = _T("");
		nDayRecType1 = 0;
		cWeekDay2    = _T("");
		nDayRecType2 = 0;
		cWeekDay3    = _T("");
		nDayRecType3 = 0;}

	tagWEEKDAYVIDEOPLAN_INFO& operator=(const tagWEEKDAYVIDEOPLAN_INFO& wdvpi){
		nFullDay     = wdvpi.nFullDay;    
		nRecType     = wdvpi.nRecType;    
		nTimeNum     = wdvpi.nTimeNum;    
		cWeekDay0    = wdvpi.cWeekDay0;   
		nDayRecType0 = wdvpi.nDayRecType0;
		cWeekDay1    = wdvpi.cWeekDay1;   
		nDayRecType1 = wdvpi.nDayRecType1;
		cWeekDay2    = wdvpi.cWeekDay2;   
		nDayRecType2 = wdvpi.nDayRecType2;
		cWeekDay3    = wdvpi.cWeekDay3;   
		nDayRecType3 = wdvpi.nDayRecType3;
		return *this; }

public:
	int nFullDay;
	int nRecType;
	int nTimeNum;
	CString cWeekDay0 ;
	int nDayRecType0 ;
	CString cWeekDay1;
	int nDayRecType1 ;
	CString cWeekDay2 ;
	int nDayRecType2 ;
	CString cWeekDay3;
	int nDayRecType3 ;
}WEEKDAYVIDEOPLAN_INFO ;


// �ƶ���HPRecordManager.h
// ¼���ļ���Ϣ lgx
typedef struct tagRECORDFILEINFO
{
public:
	tagRECORDFILEINFO(){
		filename  = "";
		eid		  = "";
		cid		  = 0;
		type      = 0;
		filesize  = 0;
		filepath  = "";
		starttime = "";
		stoptime  = "";
		url       = "";}

	tagRECORDFILEINFO& operator=(const tagRECORDFILEINFO& rfi){
		filename  = rfi.filename; 
		eid		  = rfi.eid;
		cid		  = rfi.cid;
		type      = rfi.type;     
		filesize  = rfi.filesize; 
		filepath  = rfi.filepath; 
		starttime = rfi.starttime;
		stoptime  = rfi.stoptime; 
		url		  = rfi.url;
		return *this; }

public:
	string	filename;			// �ļ���.
	string	eid;				// ��������
	ULONG	cid;				// ͨ����
	int		type;				// ¼������
	long	filesize;			// �ļ���С
	string	filepath;			// �ļ�·��
	string	starttime;			// ��ʼʱ��
	string	stoptime;			// ����ʱ��
	string  url;				// ���粥��
}RECORDFILEINFO, *LPRECORDFILEINFO;
typedef vector<RECORDFILEINFO> RECORDFILEINFO_LIST; //lgx
//
typedef map<string, string> STRINGTOSTRING_MAP;
typedef STRINGTOSTRING_MAP::iterator STRINGTOSTRING_MAP_IT;

// ������־
typedef struct tagOPERATELOGINFO
{
public:
	tagOPERATELOGINFO(){
		id          = "";
		eid			= "";
		cid			= "";
		channelname = "";
		user        = "";
		action      = "";
		status      = "";
		group       = "";
		datetime    = "";
		description = "";}

	tagOPERATELOGINFO& operator=(const tagOPERATELOGINFO& oli){
		id          = oli.id;         
		eid			= oli.eid;    
		cid			= oli.cid;  
		channelname = oli.channelname;
		user        = oli.user;       
		action      = oli.action;     
		status      = oli.status;     
		group       = oli.group;      
		datetime    = oli.datetime;   
		description = oli.description;
		return *this; }

public:
	string  id;
	string  eid;	// ��������δ��
	string  cid;	// ͨ��
	string	channelname;
	string	user;	//�����û�
	string	action;	//����
	string	status;	//״̬:�ɹ���ʧ�ܡ�ԽȨ
	string  group;	//��������
	string	datetime;//����ʱ��
	string	description;//��������
}OPERATELOGINFO, *LPOPERATELOGINFO;
typedef vector<OPERATELOGINFO>			OPERATELOGINFO_LIST;
typedef OPERATELOGINFO_LIST::iterator	OPERATELOGINFO_LIST_IT;

// ������־
typedef struct tagALERTLOGINFO
{
public:
	tagALERTLOGINFO(){
		id			= "";
		eid			= "";
		cid			= "";
		channelname = "";
		from        = "";
		to			= "";
		level       = "";
		start       = "";
		stop        = "";
		user        = "";
		description = "";}

	tagALERTLOGINFO& operator=(const tagALERTLOGINFO& ali){
		id			= ali.id;         
		eid		    = ali.eid;    
		cid			= ali.cid;  
		channelname = ali.channelname;
		from        = ali.from;       
		to			= ali.to;         
		level       = ali.level;      
		start       = ali.start;      
		stop        = ali.stop;       
		user        = ali.user;       
		description = ali.description;
		return *this; }

public:
	string 	id;
	string  eid;		// ��������δ��
	string  cid;		// ͨ��
	string	channelname;
	string	from;		// ����Դ(̽����)
	string	to;			// �����豸(������)
	string	level;		// ��������(�ڶ�)
	string	start;		// ����ʱ��
	string  stop;		// ���ʱ��
	string	user;		// �����
	string  description;
}ALERTLOGINFO, *LPALERTLOGINFO;
typedef vector<ALERTLOGINFO>		ALERTLOGINFO_LIST;
typedef ALERTLOGINFO_LIST::iterator ALERTLOGINFO_LIST_IT;

/*������*/
typedef struct _REGION_NODE
{
	ULONG   nodeid;
	ULONG   parentid;
	string  nodename;
	ULONG	nodetype;
	ULONG   nodestate;
	string	devid;
	ULONG	chnid;
	string  locaddr;
	UINT    tcpport;
	UINT    udpport;
	ULONG	audioid;
	ULONG	videoid;
	ULONG   subvideoid;
	string	mcuid;/*MCUID*/
	string	mcuaddr;/*MCU��ַ*/
	UINT	mcuport;/*MCU�˿�*/
	string	mcutype;/*MCU����*/
}REGION_NODE, *LPREGION_NODE;
typedef map<ULONG, REGION_NODE> MAP_RGNINFO;
typedef map<ULONG, REGION_NODE>::iterator MAP_RGNINFO_IT;

typedef struct tagENCODER_INFO// ��������Ϣ
{
public:
	tagENCODER_INFO()
	{
		eid    = "";
		encodername  = "";
		encodertype  = 0;
		channelnum   = 0;
		nvsname      = "";
		nvsip        = "";
		nvsport      = 0;
		nvsuid       = "";
		nvspwd       = "";
		nvsstate     = 0;
		avconcename  = "";
		avconceip    = "";
		avconceport  = 0;
		avconceuid   = "";
		avconcepwd   = "";
		avconcestate = 0;
		tag          = 0;
		desc         = "";
		serial       = "";
		version      = "";
	}

	tagENCODER_INFO& operator=(const tagENCODER_INFO& ei)
	{
		eid			 = ei.eid;
		encodername  = ei.encodername;
		encodertype  = ei.encodertype;
		channelnum   = ei.channelnum;
		nvsname      = ei.nvsname;
		nvsip        = ei.nvsip;
		nvsport      = ei.nvsport;
		nvsuid       = ei.nvsuid;
		nvspwd       = ei.nvspwd;
		nvsstate     = ei.nvsstate;
		avconcename  = ei.avconcename;
		avconceip    = ei.avconceip;
		avconceport  = ei.avconceport;
		avconceuid   = ei.avconceuid;
		avconcepwd   = ei.avconcepwd;
		avconcestate = ei.avconcestate;
		tag          = ei.tag;
		desc         = ei.desc;
		serial		 = ei.serial;
		version      = ei.version;
		return *this; 
	}
	BOOL operator==(const tagENCODER_INFO& ei)
	{
		return ((eid==ei.eid) && (encodername==ei.encodername) && (encodertype==ei.encodertype) &&
			(channelnum==ei.channelnum) && (nvsname==ei.nvsname) && (nvsip==ei.nvsip) &&
			(nvsport==ei.nvsport) && (nvsuid==ei.nvsuid) && (nvspwd==ei.nvspwd) &&
			(nvsstate==ei.nvsstate) && (avconcename ==ei.avconcename)&& (avconceip==ei.avconceip) &&
			( avconceport==ei.avconceport) && (avconceuid==ei.avconceuid) && (avconcepwd==ei.avconcepwd) &&
			(avconcestate==ei.avconcestate) && (tag==ei.tag) && (desc==ei.desc) &&
			(serial==ei.serial) && (version==ei.version)); 
	}

public:
	string	eid;			// ��������
	string	encodername;		// ����������
	int		encodertype;		// ����������
	int		channelnum;			// ͨ������
	string	nvsname;			// NVS����������
	string	nvsip;				// NVS������IP
	int		nvsport;			// NVS�������˿�
	string	nvsuid;				// NVS��¼�û�
	string	nvspwd;				// NVS��¼����
	int		nvsstate;			// NVS����״̬
	string  avconcename;		// AVCON���ķ�������
	string  avconceip;			// AVCON���ķ�����IP
	int     avconceport;		// AVCON���ķ������˿�
	string  avconceuid;			// AVCON�����û�
	string  avconcepwd;			// AVCON��������
	int		avconcestate;		// AVCON��������״̬
	int		tag;				// ���
	string  desc;				// ����
	string  serial;             // �豸���к�
	string  version;			// �豸�汾��
}ENCODER_INFO, *LPENCODER_INFO;
typedef vector<ENCODER_INFO>			ENCODER_INFO_LIST;
typedef ENCODER_INFO_LIST::iterator		ENCODER_INFO_LIST_IT;
typedef map<string, ENCODER_INFO>		ENCODER_INFO_MAP;
typedef ENCODER_INFO_MAP::iterator		ENCODER_INFO_MAP_IT;


typedef struct tagCHANNEL_INFO// ͨ����Ϣ
{
	tagCHANNEL_INFO()
	{
		eid = "";
		cid = 0;
		encodername = "";
		channelname = "";
		locaddr = "";
		tcpport = 0;
		udpport = 0;
		videoid	= 0;
		subvideoid = 0;
		audioid	= 0;
		mcuid = "";
		mcuaddr = "";
		mcuport = 0;
		mcutype = "";
		state = 0;
		tag = 0;
		desc = "";
		hwnd = 0;
		parent = 0;
	}

	tagCHANNEL_INFO& operator=(const tagCHANNEL_INFO& ci)
	{
		eid = ci.eid; 	
		cid = ci.cid;	
		encodername = ci.encodername;	
		channelname = ci.channelname;	
		locaddr = ci.locaddr;
		tcpport = ci.tcpport;
		udpport = ci.udpport;
		videoid	= ci.videoid;	
		subvideoid = ci.subvideoid;	
		audioid	= ci.audioid;       
		mcuid = ci.mcuid;
		mcuaddr = ci.mcuaddr;
		mcuport = ci.mcuport;
		mcutype = ci.mcutype;
		state = ci.state;         
		tag = ci.tag;           
		desc = ci.desc;	
		hwnd = ci.hwnd;
		parent = ci.parent;
		return *this; 
	}

public:
	string		eid;/*�豸ID*/
	ULONG		cid;/*ͨ��ID*/
	string		encodername;/*�豸����*/		
	string		channelname;/*ͨ������*/
	string		locaddr;/*��������IP*/
	UINT		tcpport;/*���������ö�̬TCP�˿�*/	
	UINT        udpport;/*���������ö�̬TCP�˿�*/
	ULONG		videoid;/*����ID*/
	ULONG		subvideoid;/*����ID*/
	ULONG		audioid;/*��ƵID*/
	string		mcuid;/*MCUID*/
	string		mcuaddr;/*MCU��ַ*/
	UINT		mcuport;/*MCU�˿�*/
	string		mcutype;/*MCU����*/
	int			state;/*ͨ��״̬*/ 
	int			tag;/*���,����*/
	string		desc;/*����*/
	DWORD_PTR	hwnd;/*���ھ��*/
	DWORD_PTR	parent;/*�����ھ��*/
}CHANNEL_INFO, *LPCHANNEL_INFO;
typedef vector<CHANNEL_INFO>			CHANNEL_INFO_LIST;
typedef CHANNEL_INFO_LIST::iterator		CHANNEL_INFO_LIST_IT;

typedef map<string, CHANNEL_INFO_LIST>	CINFO_LIST_MAP;
typedef CINFO_LIST_MAP::iterator		CINFO_LIST_MAP_IT;


typedef struct tagVIDEO_INFO// ��Ƶ��Ϣ
{
	string	eid;			// ������
	ULONG	cid;			// ͨ��
	string	encodername;	
	string	channelname;	
	int     hddevice;		// 0Ϊ���壬1Ϊ����
	int		substream;		// �Ƿ���������
	int     tomcu;			// ������MCU

	//dvr
	int		device;			// �豸
	int     denoise;		// ͼ��ȥ��

	int		codec;			// ���뷽ʽ
	int		resolution;		// �ֱ���
	int		framerate;		// ��Ƶ֡��
	int		frametype;		// ��Ƶģʽ
	int		quality;		// ��Ƶ����
	int		autoquality;	// ͼ�������Զ�����

	int		codec_sub;		// �������뷽ʽ
	int		resolution_sub;	// �ӷֱ���
	int		framerate_sub;	// ����֡��
	int		frametype_sub;	// ����ģʽ
	int		quality_sub;	// ��������
	int		autoquality_sub;// ͼ�������Զ�����

	int     quartersize_sub;// 1/4�ߴ�

	//����
	NET_DVR_COMPRESSION_INFO ndci;
	NET_DVR_COMPRESSION_INFO ndci_sub;

	//��
	DH_VIDEOENC_OPT dvo[6];/*0��1��2�������� ��3��4��5������*/
	DWORD dwImageSizeMask; /*�ֱ������룬��λ��ʾ�豸�ܹ�֧�ֵķֱ���*/
	DWORD dwEncodeModeMask;/*����ģʽ���룬��λ��ʾ�豸�ܹ�֧�ֵı���ģʽ*/

	tagVIDEO_INFO()
	{
		eid = "";
		cid = 0; 
		encodername = "";
		channelname = "";
		hddevice = 0;
		substream = 0;
		tomcu = 0;
		device = 0;
		denoise = 0;
		autoquality = 0; 
		codec = 0; 
		resolution = 0; 
		framerate = 0; 
		quality = 0; 
		frametype = 0; 
		codec_sub = 0; 
		resolution_sub = 0; 
		framerate_sub = 0; 
		quality_sub = 0; 
		frametype_sub = 0; 
		autoquality_sub = 0;
		quartersize_sub = 0;

		ndci.byBitrateType = 0;
		ndci.byPicQuality = 2;
		ndci.byResolution = 1;
		ndci.byStreamType = 1;
		ndci.dwVideoBitrate = 11;
		ndci.dwVideoFrameRate = 10;
		ndci_sub.byBitrateType = 0;
		ndci_sub.byPicQuality = 2;
		ndci_sub.byResolution = 1;
		ndci_sub.byStreamType = 1;
		ndci_sub.dwVideoBitrate = 11;
		ndci_sub.dwVideoFrameRate = 10;
	}

	tagVIDEO_INFO& operator=(const tagVIDEO_INFO& vi)
	{
		eid = vi.eid;     
		cid = vi.cid;     
		encodername = vi.encodername;   
		channelname = vi.channelname;
		hddevice = vi.hddevice;
		substream = vi.substream;	
		tomcu = vi.tomcu;
		device = vi.device;
		denoise = vi.denoise;
		codec = vi.codec;    
		resolution = vi.resolution;     
		autoquality = vi.autoquality;   
		framerate = vi.framerate;     
		quality = vi.quality;     
		frametype = vi.frametype;    
		codec_sub = vi.codec_sub; 
		resolution_sub = vi.resolution_sub;  
		framerate_sub = vi.framerate_sub;  
		quality_sub = vi.quality_sub;  
		frametype_sub = vi.frametype_sub; 
		autoquality_sub = vi.autoquality_sub;
		quartersize_sub = vi.quartersize_sub;

		ndci.byBitrateType = vi.ndci.byBitrateType;
		ndci.byPicQuality = vi.ndci.byPicQuality;
		ndci.byResolution = vi.ndci.byResolution;
		ndci.byStreamType = vi.ndci.byStreamType;
		ndci.dwVideoBitrate = vi.ndci.dwVideoBitrate;
		ndci.dwVideoFrameRate = vi.ndci.dwVideoFrameRate;
		ndci_sub.byBitrateType = vi.ndci_sub.byBitrateType;
		ndci_sub.byPicQuality = vi.ndci_sub.byPicQuality;
		ndci_sub.byResolution = vi.ndci_sub.byResolution;
		ndci_sub.byStreamType = vi.ndci_sub.byStreamType;
		ndci_sub.dwVideoBitrate = vi.ndci_sub.dwVideoBitrate;
		ndci_sub.dwVideoFrameRate = vi.ndci_sub.dwVideoFrameRate;
		return *this; 
	}
}VIDEO_INFO, *LPVIDEO_INFO;
typedef vector<VIDEO_INFO>				VIDEO_INFO_LIST;
typedef VIDEO_INFO_LIST::iterator		VIDEO_INFO_LIST_IT;

typedef struct tagAUDIO_INFO// ��Ƶ��Ϣ
{
	string	eid;			// ������
	ULONG	cid;			// ͨ��
	string	encodername;		
	string	channelname;		

	int		device;			// �豸
	int		codec;			// ���뷽ʽ
	int		agc;			// �Զ�����
	int		aec;			// ��������
	int		vad;			// �������
	int		capture_card;	// �ɼ���Ƶ��
	int		capture_mode;	// �ɼ�ģʽ��0:MIC��1:LINE_IN��2:WAVE_OUT_MIXER

	tagAUDIO_INFO()
	{
		eid = "";
		cid = 0; 
		encodername = "";
		channelname = "";
		device = 0;
		codec = 0; 
		agc = 0; 
		aec = 0; 
		vad = 0; 
		capture_card = 0; 
		capture_mode = 0; 
	}

	tagAUDIO_INFO& operator=(const tagAUDIO_INFO& ai)
	{
		eid	= ai.eid;	 
		cid = ai.cid;    
		encodername = ai.encodername;  
		channelname = ai.channelname;  
		device = ai.device;
		codec = ai.codec;   
		agc = ai.agc;          
		aec = ai.aec;          
		vad = ai.vad;          
		capture_card = ai.capture_card; 
		capture_mode = ai.capture_mode;          
		return *this; 
	}

public:
}AUDIO_INFO, *LPAUDIO_INFO;
typedef vector<AUDIO_INFO>				AUDIO_INFO_LIST;
typedef AUDIO_INFO_LIST::iterator		AUDIO_INFO_LIST_IT;


typedef struct tagMCU_INFO// MCU��Ϣ
{
	tagMCU_INFO()
	{
		eid = "";
		cid = 0;
		encodername = "";
		channelname = "";
		locaddr = "";
		tcpport = 0;
		udpport = 0;
		videoid	= 0;
		subvideoid = 0;
		audioid	= 0;
		mcuid = "";
		mcuaddr = "";
		mcuport = 0;
		mcutype = "";
	}

	tagMCU_INFO& operator=(const tagMCU_INFO& mi)
	{
		eid = mi.eid; 	
		cid = mi.cid;	
		encodername = mi.encodername;	
		channelname = mi.channelname;	
		locaddr = mi.locaddr;
		tcpport = mi.tcpport;
		udpport = mi.udpport;
		videoid	= mi.videoid;	
		subvideoid = mi.subvideoid;	
		audioid	= mi.audioid;       
		mcuid = mi.mcuid;
		mcuaddr = mi.mcuaddr;
		mcuport = mi.mcuport;
		mcutype = mi.mcutype;
		return *this; 
	}

public:
	string	eid;/*�豸ID*/
	ULONG	cid;/*ͨ��ID*/
	string	encodername;/*�豸����*/		
	string	channelname;/*ͨ������*/		
	string	locaddr;/*������IP*/	
	UINT	tcpport;/*���������ö�̬TCP�˿�*/	
	UINT    udpport;/*���������ö�̬TCP�˿�*/
	ULONG	videoid;/*����ID*/
	ULONG	subvideoid;/*����ID*/
	ULONG	audioid;/*��ƵID*/
	string	mcuid;/*MCUID*/
	string	mcuaddr;/*MCU��ַ*/
	UINT	mcuport;/*MCU�˿�*/
	string	mcutype;/*MCU����*/
}MCU_INFO, *LPMCU_INFO;
typedef vector<MCU_INFO>				MCU_INFO_LIST;
typedef MCU_INFO_LIST::iterator			MCU_INFO_LIST_IT;




typedef struct tagPTZ_INFO// ��̨
{
	string	eid;			
	ULONG	cid;			
	string	encodername;	
	string	channelname;
	int		protocolid;/*��̨Э��*/
	int		ptzaddr;/*��̨��ַ*/
	int		ptzcomport;/*��̨����*/
	int		defpresetdelay;/*����ȱʡԤ��λ��ʱ��(��)*/
	int		presetidx;/*Ԥ�õ�������*/
	int		ctltype;/*�������� 0:��ͨ 1:ת��Ԥ�õ� 2:����Ԥ�õ� 3:���Ԥ�õ� 4:ȡԤ�õ�(����Ч)*/
	int     speed;/*�����ٶ�*/
	int     baud;/*������*/

	tagPTZ_INFO()
	{
		eid	= "";
		cid	= 0; 
		encodername	= "";
		channelname	= "";
		protocolid = 0; 
		ptzaddr = 0; 
		ptzcomport = 0; 
		defpresetdelay = 0; 
		presetidx = 0;
		ctltype = 0;
		speed = 0;
		baud = 0;
	}

	tagPTZ_INFO& operator=(const tagPTZ_INFO& pi)
	{
		eid	= pi.eid;	 
		cid = pi.cid;    
		encodername	= pi.encodername;  
		channelname	= pi.channelname;  
		protocolid = pi.protocolid; 
		ptzaddr = pi.ptzaddr;   
		ptzcomport = pi.ptzcomport;          
		defpresetdelay = pi.defpresetdelay;        
		presetidx = pi.presetidx;
		ctltype	= pi.ctltype;
		speed = pi.speed;
		baud = pi.baud;
		return *this; 
	}
}PTZ_INFO, *LPPTZ_INFO;
typedef vector<PTZ_INFO>				PTZ_INFO_LIST;
typedef PTZ_INFO_LIST::iterator			PTZ_INFO_LIST_IT;

typedef struct tagVIDEO_PARAM// ��Ƶ����
{
public:
	tagVIDEO_PARAM(){
		eid      = "";
		encodername    = "";
		cid      = 0; 
		channelname    = "";
		brightness	   = 0; 
		contrast       = 0; 
		saturation     = 0; 
		hue            = 0; }

	tagVIDEO_PARAM& operator=(const tagVIDEO_PARAM& vp){
		eid      = vp.eid;	 
		encodername    = vp.encodername;  
		cid      = vp.cid;    
		channelname    = vp.channelname;  
		brightness	   = vp.brightness; 
		contrast       = vp.contrast;   
		saturation     = vp.saturation;          
		hue			   = vp.hue;  
		gamma		   = vp.gamma;
		sharpen		   = vp.sharpen;
		return *this; }

public:
	string	eid;			// ��������
	string	encodername;		// ����������
	ULONG	cid;			// ͨ����
	string	channelname;		// ͨ������
	int		brightness;			// ������
	int		contrast;			// �Աȶ�
	int		saturation;			// ���Ͷ�
	int		hue;				// ɫ��
	int		gamma;				//٤��
	int		sharpen;			//��

}VIDEO_PARAM, *LPVIDEO_PARAM;
typedef vector<VIDEO_PARAM>				VIDEO_PARAM_LIST;
typedef VIDEO_PARAM_LIST::iterator		VIDEO_PARAM_LIST_IT;


typedef struct tagRECFILE_INFO// ¼���ļ���Ϣ 
{
public:
	tagRECFILE_INFO(){
		filename	= "";
		eid			= "";
		cid			= 0;
		nvstype		= 0;
		rectype		= 0;
		scheme		= 0;
		upload		= 0;
		filesize	= 0;
		starttime	= "";
		stoptime	= "";
		filepath	= "";
		prevfile	= "";
		nextfile	= ""; 
		url			= "";
		loc_file    = true;}

	tagRECFILE_INFO& operator=(const tagRECFILE_INFO& ri){
		filename	= ri.filename;
		eid			= ri.eid;
		cid			= ri.cid;
		nvstype		= ri.nvstype;
		rectype		= ri.rectype;
		scheme		= ri.scheme;
		upload		= ri.upload;
		filesize	= ri.filesize;
		starttime	= ri.starttime;
		stoptime	= ri.stoptime;
		filepath	= ri.filepath;
		prevfile	= ri.prevfile;
		nextfile	= ri.nextfile;
		url			= ri.url;
		loc_file	= ri.loc_file;
		return *this; }

public:
	string	filename;			// �ļ���
	string	eid;				// ��������
	ULONG	cid;				// ͨ����
	int		nvstype;			// ��������
	bool	loc_file;			// �Ƿ�DVR�豸����¼��
	int		rectype;			// ¼������
	int		scheme;				// �ƻ�¼��
	int		upload;				// �ϴ���ʶ
	LONG	filesize;			// �ļ���С
	string	starttime;			// ��ʼʱ��
	string	stoptime;			// ����ʱ��
	string	filepath;			// �ļ�ȫ·��
	string	prevfile;			// ��������һ���ļ�
	string	nextfile;			// ��������һ���ļ�
	string	url;				// Զ�̲���
}RECFILE_INFO, *LPRECFILE_INFO;
typedef vector<RECFILE_INFO>			RECFILE_INFO_LIST; 
typedef RECFILE_INFO_LIST::iterator		RECFILE_INFO_LIST_IT;

// Ȩ�ޱ�
typedef struct tagCOMMAND_TABLE
{
public:
	tagCOMMAND_TABLE(){
		cmd		= "";
		action	= "";
		desc	= "";
		log		= FALSE;
		allow	= FALSE;}

	tagCOMMAND_TABLE& operator=(const tagCOMMAND_TABLE& ct){
		cmd		= ct.cmd;
		action	= ct.action;
		desc	= ct.desc;
		log		= ct.log;
		allow	= ct.allow;
		return *this; }

public:
	string cmd;			//������
	string action;		//������
	string desc;		//��������
	BOOL log;			//�Ƿ��¼��־
	BOOL allow;			//�Ƿ�����ִ�У������û�Ȩ�����þ���ֵ
	//struct tagCommandsTable *subcmd;	//�����
}COMMAND_TABLE, *LPCOMMAND_TABLE;
typedef hash_map<string, COMMAND_TABLE> COMMAND_TABLE_MAP;
typedef COMMAND_TABLE_MAP::iterator		COMMAND_TABLE_MAP_IT;

typedef struct tagHPUSER_INFO// �û���Ϣ 
{
public:
	tagHPUSER_INFO(){
		name	= "";
		type	= 0;
		regions	= "";
		rights	= ""; }

	tagHPUSER_INFO& operator=(const tagHPUSER_INFO& ui){
		name	= ui.name;
		type	= ui.type;
		regions	= ui.regions;
		rights	= ui.rights;
		return *this; }

public:
	string	name;				// �û�
	int		type;				// ����
	string	regions;			// ����
	string  rights;				// Ȩ��
}HPUSER_INFO, *LPHPUSER_INFO;
typedef vector<HPUSER_INFO>			USER_INFO_LIST; 
typedef USER_INFO_LIST::iterator	USER_INFO_LIST_IT;

typedef struct tagMOVE_DETECT_INFO// �ƶ�֡����Ϣ
{
public:
	tagMOVE_DETECT_INFO(){
		eid	= "";
		cid	= 0;
		number		= 0; 
		grade		= 0;
		started		= false;
		memset(&rect, 0, sizeof(RECT)*5); }

	tagMOVE_DETECT_INFO& operator=(const tagMOVE_DETECT_INFO& mdi){
		eid	= mdi.eid;
		cid	= mdi.cid;
		number		= mdi.number; 
		grade		= mdi.grade;
		started		= mdi.started;
		memcpy(&rect, mdi.rect, sizeof(RECT)*5);
		return *this; }

public:
	string	eid;	// ������
	ULONG   cid;	// ͨ��
	RECT	rect[5];	// ����
	int		number;     // �������
	int		grade;		// ���ж�
	bool	started;    // ����
}MOVE_DETECT_INFO;
typedef vector<MOVE_DETECT_INFO>		MOVE_DETECT_INFO_LIST; 
typedef MOVE_DETECT_INFO_LIST::iterator	MOVE_DETECT_INFO_LIST_IT;

typedef struct tagVIDEO_MASK_INFO// ��Ƶ������Ϣ
{
public:
	tagVIDEO_MASK_INFO(){
		eid	= "";
		cid	= 0;
		number		= 0; 
		started		= false;
		memset(&rect, 0, sizeof(RECT)*5); }

	tagVIDEO_MASK_INFO& operator=(const tagVIDEO_MASK_INFO& vmi){
		eid	= vmi.eid;
		cid	= vmi.cid;
		number		= vmi.number; 
		started		= vmi.started;
		memcpy(&rect, vmi.rect, sizeof(RECT)*5);
		return *this; }

public:
	string	eid;	// ������
	ULONG   cid;	// ͨ��
	RECT	rect[5];	// ����
	int		number;     // �������
	bool	started;    // ����
}VIDEO_MASK_INFO;
typedef vector<VIDEO_MASK_INFO>			VIDEO_MASK_INFO_LIST; 
typedef VIDEO_MASK_INFO_LIST::iterator	VIDEO_MASK_INFO_LIST_IT;

typedef struct tagPREPOINT_INFO // Ԥ�õ���Ϣ
{
public:
	tagPREPOINT_INFO(){
		eid	= "";
		cid	= 0;
		prepointid	= 0; 
		name		= ""; 
		defenceid	= 0;
		bIfDefaultPrePoint=0;
		bAutoReturn		= false;
		iReturnTime	= 60; 
	}

	tagPREPOINT_INFO& operator=(const tagPREPOINT_INFO& pi){
		eid	= pi.eid;
		cid	= pi.cid;
		prepointid	= pi.prepointid; 
		name		= pi.name;
		defenceid	= pi.defenceid;
		bIfDefaultPrePoint=pi.bIfDefaultPrePoint;
		bAutoReturn		= pi.bAutoReturn; 
		iReturnTime	= pi.iReturnTime;

		return *this; }

public:
	string	eid;	// ������
	ULONG   cid;	// ͨ��
	ULONG	prepointid; // Ԥ�õ�
	string  name;		// Ԥ�õ�����
	int     defenceid;  // ����
	BOOL bIfDefaultPrePoint;
	bool bAutoReturn;		//�Ƿ��Զ���λ
	int  iReturnTime;	//��λʱ�� ��
}PREPOINT_INFO,*LPPREPOINT_INFO;
typedef vector<PREPOINT_INFO>			PREPOINT_INFO_LIST; 
typedef PREPOINT_INFO_LIST::iterator	PREPOINT_INFO_LIST_IT;


// ��������
#define WM_USER_DETECTER_ALERT_MSG  WM_USER + 21
enum { 

	DETECTER_ALERT_WITH_BONDED_TO_PRESET,
	DETECTER_ALERT_WITHOUT_BONDED_TO_PRESET,
	DETECTER_ALERT_CANCEL
};

typedef struct tagALERT_INFO// ������Ϣ
{
public:
	tagALERT_INFO(){
		eid	= "";
		cid	= 0;
		alerttype	= 0; 
		alertid		= 0; 
		nvstype		= 0;
		prepoint	= 0;
		scheme		= false; }

	tagALERT_INFO& operator=(const tagALERT_INFO& ai){
		eid	= ai.eid;
		cid	= ai.cid;
		alerttype	= ai.alerttype; 
		alertid		= ai.alertid;
		nvstype	 	= ai.nvstype;
		prepoint	= ai.prepoint;
		scheme		= ai.scheme;
		return *this; }

public:
	string	eid;	// ������
	ULONG   cid;	// ͨ��
	int		alerttype;  // ��������
	int     alertid;	// ����ID
	int		nvstype;	// NVS����
	int		prepoint;	// Ԥ�õ�
	bool	scheme;		// �Ƿ�ƻ�
}ALERT_INFO;
typedef vector<ALERT_INFO>			ALERT_INFO_LIST; 
typedef ALERT_INFO_LIST::iterator	ALERT_INFO_LIST_IT;

typedef struct tagUPLOAD_INFO// �ϴ���Ϣ
{
public:
	tagUPLOAD_INFO(){
		server		= "";
		ip0			= "";
		ip1			= "";
		ip2			= "";
		port		= 0; 
		type		= 0; 
		autoupload	= false;
		desc		= ""; }

	tagUPLOAD_INFO& operator=(const tagUPLOAD_INFO& ui){
		server		= ui.server;
		ip0			= ui.ip0;
		ip1			= ui.ip1;
		ip2			= ui.ip2;
		port		= ui.port; 
		type		= ui.type;
		autoupload	= ui.autoupload;
		desc		= ui.desc;
		return *this; }

public:
	string  server;		// ������
	string	ip0;		// ¼�������IP0
	string	ip1;		// ¼�������IP1
	string	ip2;		// ¼�������IP2
	int		port;		// �ϴ��˿ں�
	int		type;		// �ϴ����� 
	bool	autoupload;	// �Զ��ϴ�
	string	desc;		// ����
}UPLOAD_INFO;
typedef vector<UPLOAD_INFO>			UPLOAD_INFO_LIST; 
typedef UPLOAD_INFO_LIST::iterator	UPLOAD_INFO_LIST_IT;


typedef struct tagDevCfgCopyInfo //�豸������Ϣ
{
public:
	tagDevCfgCopyInfo(){
		copy	= false;
		fromeid = "";
		fromcid	= 0; 
		toeid	= "";
		tocid	= 0;  }

	tagDevCfgCopyInfo& operator=(const tagDevCfgCopyInfo& dcci){
		copy	= dcci.copy;
		fromeid	= dcci.fromeid;
		fromcid	= dcci.fromcid; 
		toeid	= dcci.toeid;
		tocid	= dcci.tocid;
		return *this; }

public:
	bool   copy;		// �Ѹ���
	string fromeid;		// Դ������
	ULONG  fromcid;		// Դͨ��
	string toeid;		// Ŀ�ı�����
	ULONG  tocid;		// Ŀ��ͨ��
}DevCfgCopyInfo;

//����Ƶ������
typedef struct tagAVBITRATE
{
	tagAVBITRATE(){
		abitrate	= 0;
		alostcount  = 0;
		vbitrate	= 0; 
		vlostcount	= 0; 
		width		= 0;
		height      = 0;}

	tagAVBITRATE& operator=(const tagAVBITRATE& avb){
		abitrate	= avb.abitrate;
		alostcount	= avb.alostcount;
		vbitrate	= avb.vbitrate;
		vlostcount	= avb.vlostcount;
		width		= avb.width;
		height		= avb.height;
		return *this; }

	ULONG abitrate;
	ULONG alostcount;
	ULONG vbitrate;
	ULONG vlostcount;
	ULONG width;	// + �ֱ��� �� by lgx [1/24/2008]
	ULONG height;   // + �ֱ��� �� by lgx [1/24/2008]
}AVBITRATE;



//��������
typedef struct tagALARM_LINK_INFO
{
	string eid;
	ULONG  cid;
	int    iotype;
	int	   linkid;
	string linkdesc;
	int	   presetid;
	int    cruiseid;

	tagALARM_LINK_INFO(){
		eid			= "";
		cid			= 0;
		iotype		= 0;
		linkid		= 0;
		linkdesc	= "";
		presetid	= 0; 
		cruiseid	= 0;  }

	tagALARM_LINK_INFO& operator=(const tagALARM_LINK_INFO& ali){
		eid			= ali.eid;
		cid			= ali.cid;
		iotype		= ali.iotype;
		linkid		= ali.linkid;
		linkdesc	= ali.linkdesc;
		presetid	= ali.presetid; 
		cruiseid	= ali.cruiseid;  
		return *this; }
}ALARM_LINK_INFO, *LPALARM_LINK_INFO;
typedef vector<ALARM_LINK_INFO>				ALARM_LINK_INFO_LIST; 
typedef vector<ALARM_LINK_INFO>::iterator	ALARM_LINK_INFO_LIST_IT;

//������־
typedef struct tagALARM_INFO
{
	string eid;
	ULONG  cid;
	int	   type;
	int	   subtype;
	int    nvstype;
	string userid;
	string datetime;

	tagALARM_INFO(){
		eid		 = "";
		cid		 = 0;
		type	 = 0;
		subtype  = 0; 
		nvstype  = 0;  
		userid   = "";
		datetime = ""; }

	tagALARM_INFO& operator=(const tagALARM_INFO& ai){
		eid		 = ai.eid;
		cid		 = ai.cid;
		type	 = ai.type;
		subtype  = ai.subtype; 
		nvstype  = ai.nvstype;  
		userid   = ai.userid;
		datetime = ai.datetime;
		return *this; }
}ALARM_INFO, *LPALARM_INFO;
typedef vector<ALARM_INFO>				ALARM_INFO_LIST; 
typedef vector<ALARM_INFO>::iterator	ALARM_INFO_LIST_IT;


//������־
typedef struct tagOPLOG_INFO
{
	string userid;
	string command;
	string datetime;
	string logtype;
	string logtext;
	int	   status;

	tagOPLOG_INFO(){
		userid	 = "";
		command	 = "";
		datetime = "";
		logtype  = "";
		logtext  = ""; 
		status   = 0; }

	tagOPLOG_INFO& operator=(const tagOPLOG_INFO& oi){
		userid	 = oi.userid;
		command	 = oi.command;
		datetime = oi.datetime;
		logtype  = oi.logtype;
		logtext  = oi.logtext; 
		status   = oi.status;
		return *this; }
}OPLOG_INFO, *LPOPLOG_INFO;
typedef vector<OPLOG_INFO>				OPLOG_INFO_LIST; 
typedef vector<OPLOG_INFO>::iterator	OPLOG_INFO_LIST_IT;

//��������˿�
typedef struct tagALARM_PORT_INFO
{
	string eid;
	int    uartport;
	int    inputnum;
	int    outputnum;
	int    baudrate;
	int    databit;
	int    stopbit;
	int    parity;

	tagALARM_PORT_INFO(){
		eid			= "";
		uartport	= 0;
		inputnum	= 0;
		outputnum	= 0;
		baudrate	= 0;
		databit		= 0;
		stopbit		= 0; 
		parity		= 0; }

	tagALARM_PORT_INFO& operator=(const tagALARM_PORT_INFO& aii){
		eid			= aii.eid;
		uartport	= aii.uartport;
		inputnum	= aii.inputnum;
		outputnum	= aii.outputnum;
		baudrate	= aii.baudrate;
		databit		= aii.databit;
		stopbit		= aii.stopbit; 
		parity		= aii.parity;
		return *this; }
}ALARM_PORT_INFO, *LPALARM_PORT_INFO;
typedef vector<ALARM_PORT_INFO>				ALARM_PORT_INFO_LIST; 
typedef vector<ALARM_PORT_INFO>::iterator	ALARM_PORT_INFO_LIST_IT;

//��������
typedef struct tagALARM_PARAM
{
	string eid;
	long	cid;
	int    center;
	int    record;
	int    linkage;
	int    output;
	int    videolost;
	int    videohide;

	tagALARM_PARAM(){
		eid		  = "";
		cid		  =0;
		center	  = 0;
		record	  = 0;
		linkage	  = 0;
		output	  = 0;
		videolost = 0;
		videohide = 0; }

	tagALARM_PARAM& operator=(const tagALARM_PARAM& ap){
		eid		  = "";
		cid		 =ap.cid;
		center	  = ap.center;
		record	  = ap.record;
		linkage	  = ap.linkage;
		output	  = ap.output;
		videolost = ap.videolost;
		videohide = ap.videohide;
		return *this; }
}ALARM_PARAM, *LPALARM_PARAM;
typedef vector<ALARM_PARAM>				ALARM_PARAM_LIST; 
typedef vector<ALARM_PARAM>::iterator	ALARM_PARAM_LIST_IT;



//��Ƶ������������ֵ����Сֵ
typedef struct tagVIDEO_PARAM_RANGE
{
	int BrightnessMax;
	int BrightnessMin;
	int ContrastMax;
	int ContrastMin;
	int SaturationMax;
	int SaturationMin;
	int HueMax;
	int HueMin;
	int GammaMax;
	int GammaMin;
	int SharpenMax;
	int SharpenMin;

	tagVIDEO_PARAM_RANGE& operator=(const tagVIDEO_PARAM_RANGE& vpr)
	{
		 BrightnessMax = vpr.BrightnessMax;
		 BrightnessMin = vpr.BrightnessMin;
		 ContrastMax = vpr.ContrastMax;
		 ContrastMin = vpr.ContrastMin;
		 SaturationMax = vpr.SaturationMax;
		 SaturationMin = vpr.SaturationMin;
		 HueMax = vpr.HueMax;
		 HueMin= vpr.HueMin;
		 GammaMax = vpr.GammaMax;
		 GammaMin = vpr.GammaMin;
		 SharpenMax = vpr.SharpenMax;
		 SharpenMin = vpr.SharpenMin;
		 return *this; 

	}
}VIDEO_PARAM_RANGE, *LPVIDEO_PARAM_RANGE;









