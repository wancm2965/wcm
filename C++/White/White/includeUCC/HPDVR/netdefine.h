#pragma  once

#define AVCON_NDVR_VERSION		"v6.2"/*大版本*/
#define AVCON_NDVR_SUBVERS		".2"/*子版本*/
#define AVCON_NDVR_BUILDED		".1"/*补订号*/
#define AVCON_NDVR_VERSION_FULL	 AVCON_NDVR_VERSION AVCON_NDVR_SUBVERS AVCON_NDVR_BUILDED

//调用类型
#define NDVR_CLIENT		0
#define NDVR_DEVICE		1
#define AVCON_DEVICE	2

#include <list>
#include <vector>
#include <string>
#include <hash_map>
#include <map>
#pragma warning(disable:4996)	//std::XX被声明为否决的
using namespace std;


#ifndef MIN
#define MIN(a,b) (((a)>(b)) ? (b) : (a))
#endif

#ifndef MAXLEN 
#define MAXLEN(a)	(sizeof(a)-1)
#endif


/************************************************************************/
/* NDVR内部信令                                                         */
/************************************************************************/
// 命令 [5/24/2008]

// 用户管理 
/*
mgr_usr|cmd_mod_user_password|cmd_mod_user_rights|cmd_add_user|cmd_del_user|cmd_get_user|
*/
#define CMD_USER_MODIFY_PSW					"cmd_mod_user_password"/*修改密码*/				
#define CMD_RIGHT_MODIFY					"cmd_mod_user_rights"/*修改权限*/		
#define CMD_ADDUSER							"cmd_add_user"/*添加用户*/		
#define CMD_DELUSER							"cmd_del_user"/*删除用户*/
#define CMD_DOWNLOAD_USER					"cmd_get_user"/*取用户*/


// 通道管理
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
#define CMD_REMOTE_CHANNEL_GET_AV_WIZARD	 "cmd_get_channel_param"/*取通道参数*/
#define CMD_REMOTE_CHANNEL_SET_AV_WIZARD	 "cmd_set_channel_param"/*设通道参数*/
#define CMD_GET_AUDIO_PARAM					 "cmd_get_audio_param"/*取音频参数*/
#define CMD_SET_AUDIO_PARAM					 "cmd_set_audio_param"/*设音频参数*/
#define CMD_GET_VIDEO_PARAM					 "cmd_get_video_param"/*取视频参数*/
#define CMD_SET_VIDEO_PARAM					 "cmd_set_video_param"/*设视频参数*/
#define CMD_START_MOVE_DETECT				 "cmd_start_motion_detect"/*开始移动侦测*/
#define CMD_STOP_MOVE_DETECT				 "cmd_stop_motion_detect"/*停止移动侦测*/
#define CMD_CHANNEL_GET_MOVE_DETECT 		 "cmd_get_motion_detect"/*取动测参数*/
#define CMD_CHANNEL_SET_MOVE_DETECT 		 "cmd_set_motion_detect"/*设动测参数*/
#define CMD_START_VIDEO_MASK				 "cmd_start_video_hide"/*开始视频遮掩*/
#define CMD_STOP_VIDEO_MASK					 "cmd_stop_video_hide"/*停止视频遮掩*/
#define CMD_START_VIDEO_COVER				 "cmd_start_video_cover"/*开始侦测遮挡*/
#define CMD_STOP_VIDEO_COVER				 "cmd_stop_video_cover"/*停止侦测遮挡*/
#define CMD_START_VIDEO_LOST				 "cmd_start_video_lost"/*开始侦测丢失*/
#define CMD_STOP_VIDEO_LOST					 "cmd_stop_video_lost"/*停止侦测丢失*/
#define CMD_CAPTURE_FRAME_TO_JPG			 "cmd_get_frame_to_jpg"/*抓拍*/
#define CMD_VIDEO_INPUT_SETUP				 "cmd_set_video_input"/*视频输入*/
#define CMD_VIDEO_ADJUST_SETUP			     "cmd_set_video_adjust"/*视频调节*/
#define CMD_VIDEO_INVERT					 "cmd_set_video_invert"/*颠倒图象*/
#define CMD_CHANNEL_GETBASEINFO				 "cmd_get_channel_basic"/*取通道基本信息*/
#define CMD_CHANNEL_GETVIDEOINFO			 "cmd_get_channel_video"/*取通道视频信息*/
#define CMD_CHANNEL_GETAUDIOINFO			 "cmd_get_channel_audio"/*取通道音频信息*/
#define CMD_CHANNEL_GETMCUINFO				 "cmd_get_channel_mcu"/*取通道MCU信息*/
#define CMD_CHANNEL_GETPTZINFO			     "cmd_get_channel_ptz"/*取通道云台信息*/
#define CMD_CH_AUDIO_START					 "cmd_start_interphone"/*打开对讲*/
#define CMD_CH_AUDIO_RECEIVE	             "cmd_rcv_interphone"/*接收对讲*/
#define CMD_CH_AUDIO_STOP		             "cmd_stop_interphone"/*停止对讲*/		
#define CMD_CH_VIDEO_START					 "cmd_start_avstream"/*开始音视频*/
#define CMD_CH_VIDEO_RECEIVE				 "cmd_rvc_avstream"/*接收音视频*/
#define CMD_CH_VIDEO_STOP					 "cmd_stop_avstream"/*停止音视频*/
#define CMD_CHANNEL_VIDEOMASK_STATE			 "cmd_get_video_hide_state"/*取遮掩状态*/
#define CMD_CHANNEL_VIDEOMOVE_STATE			 "cmd_get_montion_detect_state"/*取动测状态*/
#define CMD_CHANNEL_RECORDSTATE				 "cmd_get_record_state"/*取录像状态*/
#define CMD_CHANNEL_ALARMSTATE				 "cmd_get_alarm_state"/*取报警状态*/
#define CMD_SWITCH_STREAM_MODE				 "cmd_switch_stream_mode"/*切换流模式*/
#define CMD_CHANNEL_AUTOSCROLL				 "cmd_start_channel_scroll"/*开始或停止轮循*/
#define CMD_DOWNLOAD_CHANNEL_STATUS			 "cmd_dwn_channel_status"/*下载通道状态*/
#define CMD_UPDATE_CHANNEL_NAME				 "cmd_udp_chname"/*更新通道名称*/


// 录像管理
/*
mgr_rec|cmd_get_record_file|cmd_start_manual_record|cmd_stop_manual_record|cmd_set_record_param|cmd_get_record_param|
cmd_set_record_schedule|cmd_get_record_schedule|cmd_start_defence|cmd_stop_defence|cmd_issue_defence_state|
cmd_get_fixed_driver|cmd_set_fixed_driver|
*/
#define CMD_GET_RECORD_FILE					"cmd_get_record_file"/*取录像文件*/
#define CMD_MANUAL_RECORD_START				"cmd_start_manual_record"/*开始手动录像*/
#define CMD_MANUAL_RECORD_STOP				"cmd_stop_manual_record"/*停止手动录像*/
#define CMD_ENCODER_RECORD_SET				"cmd_set_record_param"/*设录像参数*/
#define CMD_ENCODER_RECORD_GET				"cmd_get_record_param"/*取录像参数*/
#define CMD_VIDEOPLAN_SET					"cmd_set_record_schedule"/*设计划表*/
#define CMD_VIDEOPLAN_GET					"cmd_get_record_schedule"/*取计划表*/
#define CMD_VIDEOPLAN_START					"cmd_start_defence"/*布防*/
#define CMD_VIDEOPLAN_STOP					"cmd_stop_defence"/*撤防*/
#define CMD_VIDEOPLAN_RUNNING				"cmd_issue_defence_state"/*发布防御状态*/
#define CMD_FIXED_DRIVER_GET				"cmd_get_fixed_driver"/*取磁盘信息*/
#define CMD_FIXED_DRIVER_SET				"cmd_set_fixed_driver"/*设磁盘信息*/



// 云台管理
/*
mgr_ptz|cmd_ptz_control|cmd_ptz_lock|cmd_get_prepoint|cmd_set_prepoint|cmd_set_ptz_param|cmd_get_ptz_param|
*/
#define CMD_PTZ								"cmd_ptz_control"/*云台控制*/
#define CMD_REMOTE_CHANNEL_SEND_YT_CMD		"cmd_ptz_control"/*云台控制*/
#define CMD_CLOUD_PEDESTAL_LOCK				"cmd_ptz_lock"/*云台锁定*/
#define CMD_CHANNEL_GET_PRE_POINT 			"cmd_get_prepoint"/*取预置点*/
#define CMD_CHANNEL_SET_PRE_POINT 			"cmd_set_prepoint"/*设预置点*/
#define CMD_REMOTE_CHANNEL_SET_SERIAL_SET	"cmd_set_ptz_param"/*设云台参数*/
#define CMD_REMOTE_CHANNEL_GET_SERIAL_SET	"cmd_get_ptz_param"/*取云台参数*/



// 设备管理
/*
mgr_dev|cmd_add_encoder|cmd_mod_encoder|cmd_del_encoder|cmd_add_channel|cmd_mod_channel|cmd_del_channel|cmd_get_devinfo|
cmd_get_encoder|cmd_reboot|cmd_clk_remote_key|
*/
#define CMD_ENCODERMGR_EADD					"cmd_add_encoder"/*添加编码器*/
#define CMD_ENCODERMGR_EMOD					"cmd_mod_encoder"/*修改编码器*/
#define CMD_ENCODERMGR_EDEL					"cmd_del_encoder"/*删除编码器*/
#define CMD_ENCODERMGR_CADD					"cmd_add_channel"/*添加通道*/
#define CMD_ENCODERMGR_CMOD					"cmd_mod_channel"/*修改通道*/
#define CMD_ENCODERMGR_CDEL					"cmd_del_channel"/*删除通道*/
#define CMD_GET_REGION_INFO					"cmd_get_devinfo"/*取区域信息*/
#define CMD_CHANNEL_GETALLINFO				"cmd_get_devinfo"/*取设备信息*/
#define CMD_CHANNEL_GETENCODERINFO			"cmd_get_encoder"/*取编码器信息*/
#define CMD_DEV_RECONNECT					"cmd_reboot"/*重启编码器*/
#define CMD_CLICK_SOFT_KEY					"cmd_clk_remote_key"/*远程按键*/
#define CMD_UPDATE_ENCODER_NAME				"cmd_udp_encname"/*更新设备名称*/

// [4/9/2009 lzy]
#define CMD_GET_DEVCFGINFO					"cmd_get_devcfginfo"		//得到设备配置信息
#define CMD_SET_DEVCFGINFO					"cmd_set_devcfginfo"		//保存设备配置信息 
//--------------------------

// 报警管理
/*
mgr_alm|cmd_set_alarm_param|cmd_get_alarm_param|cmd_stop_alert|cmd_issue_start_alarm_state|cmd_issue_stop_alarm_state|
*/
#define CMD_ALARM_PARAM_SET					"cmd_set_alarm_param"/*设报警参数*/
#define CMD_ALARM_PARAM_GET					"cmd_get_alarm_param"/*取报警参数*/
#define CMD_MANUAL_STOP_ALERT				"cmd_stop_alert"/*手动停止报警*/
#define CMD_HOST_DETECTER_ALERT_START		"cmd_issue_start_alarm_state"/*开始报警*/
#define CMD_HOST_DETECTER_ALERT_STOP		"cmd_issue_stop_alarm_state"/*停止报警*/


// 日志管理
/*
mgr_log|cmd_get_operate_log|cmd_get_operate_type|cmd_get_alarm_log|cmd_get_record_log|
cmd_del_operate_log|cmd_del_alarm_log|cmd_del_record_log|cmd_bak_operate_log|cmd_bak_alarm_log|cmd_bak_record_log|
*/
#define CMD_DOWN_OPERLOG					"cmd_get_operate_log"/*取操作日志*/
#define CMD_OPLOGTYPE						"cmd_get_operate_type"/*取操作类型*/
#define CMD_ALARM_LOG						"cmd_get_alarm_log"/*取报警日志*/
#define CMD_GET_RECORD_HISTORY				"cmd_get_record_log"/*取录像日志*/
#define CMD_DEL_OPLOG						"cmd_del_operate_log"/*删除操作日志*/
#define CMD_DEL_ALARMLOG					"cmd_del_alarm_log"/*删除报警日志*/
#define CMD_DEL_RECORDLOG					"cmd_del_record_log"/*删除录像日志*/
#define CMD_SAVE_OPLOG						"cmd_bak_operate_log"/*备份操作日志*/
#define CMD_SAVE_ALARMLOG					"cmd_bak_alarm_log"/*备份报警日志*/
#define CMD_SAVE_RECORDLOG					"cmd_bak_record_log"/*备份录像日志*/


// 其他管理
/*
mgr_oth|cmd_login|cmd_logout|cmd_disconnect|cmd_p2p|cmd_transfer|cmd_broadcast|cmd_svr_address|cmd_mcu_address|cmd_mcu_issue_num|
cmd_set_ipfirst|cmd_get_ipfirst|
*/
#define CMD_LOGIN							"cmd_login"/*登陆*/
#define CMD_LOGOUT							"cmd_logout"/*注销*/
#define CMD_DISCONNECT						"cmd_disconnect"/*断开连接*/
#define CMD_P2PURD							"cmd_p2p"/*P2P通信*/
#define CMD_TRANSFER						"cmd_transfer"/*透传信令*/
#define CMD_BROADCAST						"cmd_broadcast"/*广播信令*/
#define CMD_AVCON_IPINFO					"cmd_svr_address"/*中心下发服务器地址*/
#define CMD_CH_MCU							"cmd_mcu_address"/*中心下发MCU地址*/
#define CMD_CH_RELEASE2MCU					"cmd_mcu_issue_num"/*中心下发发布通道数*/
#define CMD_IPP_FIRST_SET					"cmd_set_ipfirst"/*设优先使用的IP协议*/
#define CMD_IPP_FIRST_GET					"cmd_get_ipfirst"/*取优先使用的IP协议*/

// 地图管理
/*
mgr_map|cmd_add_mapnode|cmd_del_mapnode|cmd_mod_mapnode|cmd_add_hotspot|cmd_del_hotspot|cmd_mod_hotspot|cmd_get_mapinfo|
*/
#if 0 //2.0
#define CMD_MAPADD							"cmd_add_mapnode"/*增加地图结点*/
#define CMD_MAPDEL							"cmd_del_mapnode"/*删除地图结点*/
#define CMD_MAPMOD							"cmd_mod_mapnode"/*修改地图结点*/
#define CMD_MAPPOINTADD						"cmd_add_hotspot"/*增加地图热点*/
#define CMD_MAPPOINTDEL						"cmd_del_hotspot"/*删除地图热点*/
#define CMD_MAPPOINTMOD						"cmd_mod_hotspot"/*修改地图热点*/
#define CMD_MAPDOWNLOAD						"cmd_get_mapinfo"/*取地图信息*/

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
//whidbeycai 20070514 地图操作系列命令
#define CMD_MAPADD  "cmd_map_add"    //地图结点增加
#define CMD_MAPDEL  "cmd_map_del"    //地图结点删除
#define CMD_MAPMOD  "cmd_map_mod"    //地图结点修改
#define CMD_MAPPOINTADD "cmd_mappoint_add" //地图热点增加
#define CMD_MAPPOINTDEL "cmd_mappoint_del" //地图热点删除
#define CMD_MAPPOINTMOD "cmd_mappoint_mod" //地图热点修改
#define CMD_MAPDOWNLOAD "cmd_map_download" //地图信息下载
#define CMD_MAPPOINTDOWNLOAD "cmd_mappoint_download" //地图热点信息下载
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
#define ATTRIB_MAP_DOWNTYPE             "downtype" //是否下载同时地图文件
#define ATTRIB_MAP_OBJTYPE             "objtype" //是地图还是地图热点对象
#endif
//  [5/24/2008]

// Added 矩阵控制  [4/10/2009 lgx]
#define ATTRIB_MATRIX_TYPE				"mtx_type"
#define ATTRIB_MATRIX_INPUT				"mtx_in"
#define ATTRIB_MATRIX_OUTPUT			"mtx_out"
#define ATTRIB_MATRIX_CHNNAME			"mtx_chn_name"
#define ATTRIB_MATRIX_TRADENAME        "mtx_trade_name"
//

/************************************************************************/
//不检测的命令
#define CMD_UPDATE_AVCON_STATUS				"cmd_upd_avc_sta"/*更新DVR的AVCON上线状态*/
#define CMD_MATRIX_CTRL						"cmd_matrix_ctrl"/*矩阵控制*/

//免检命令
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
/* NDVR信令字段                                                         */
/************************************************************************/
// 字段 [5/24/2008]
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
#define ATTRIB_AVCON_IPINFO_TYPE			"opt_t" /*0 获取 1设置*/
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
#define ATTRIB_CHANN_COUNT					"chan_count"	//通道个数
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
#define ATTRIB_GAMA							"gama"		//伽马
#define ATTRIB_GAMA_MAX						"gama_max"		//伽马
#define ATTRIB_GAMA_MIN						"gama_min"		//伽马
#define ATTRIB_SHARPEN						"sharpen"
#define ATTRIB_SHARPEN_MAX					"sharpen_max"
#define ATTRIB_SHARPEN_MIN					"sharpen_min"


#define ATTRIB_DEFAULT_VIDEO_PARAM			"def_vid_p"
#define ATTRIB_KEY_VALUE					"key_v"
#define ATTRIB_COPY_TO_EID					"cp_t_e"
#define ATTRIB_COPY_TO_CID					"cp_t_c" 
#define ATTRIB_COPY_TO_WEEK					"cp_t_w"
#define ATTRIB_AUTO_RETURN					"ptz_auto_r"		//自动归位
#define ATTRIB_RETURN_TIME					"ptz_auto_r_t"		//归位时间
#define ATTRIB_CTRL_TYPE					"ptz_ctrl_t"		//控制类型 0:普通 1:转到预置点 2:设置预置点 3:清除预置点 4:取预置点(暂无效)

#define ATTRIB_AUDIO_TRANS_TYPE				"aud_trans_t" //0x01:TCP,0x02:UDP,0x04:RTP
#define ATTRIB_VIDEO_TRANS_TYPE				"vid_trans_t" //0x01:TCP,0x02:UDP,0x04:RTP



// [3/27/2009 lzy]
//增加移动侦测区域的宏定义
#define ATTRIB_MOVE_DETECT_NUM				"JKRectNum"		//一共有几个区域 最大5个 
#define ATTRIB_MOVE_DETECT_BEGIN_HOUR		"JK_Begin_Hour"
#define ATTRIB_MOVE_DETECT_BEGIN_MINUTE		"JK_Begin_Minute"
#define ATTRIB_MOVE_DETECT_END_HOUR			"JK_End_Hour"
#define ATTRIB_MOVE_DETECT_END_MINUTE		"JK_End_Minute"
#define ATTRIB_MOVE_DETECT_GRADE			"JK_Grade"	//不清楚做什么用的 
#define ATTRIB_MOVE_DETECT_RECT				"JK_Rect"	 //保存的是区域 格式为 bottom,left,right,top 空格 bottom,left,right,top 空格
#define ATTRIB_MOVE_DETECT_TYPE				"WhichKindOfDis"	//0:移动侦测 1:遮盖
#define ATTRIB_MOVE_DETECT_START			"JK_BStarted"


#define ATTRIB_AVIEW_SET					"aview_set"	//增加此字段是为了判断设置NDVR时候是不是从aview传过来的远程配置
#define ATTRIB_VIDEO_DEV_NUMBER				"VideoDeviceNumber" 
#define ATTRIB_RESOLUTION_NUMBER			"ResolutionNumber" 
#define ATTRIB_QUALITY_NUMBER				"QualityNumber" 
#define ATTRIB_IS_PRESET					"iIsPreSet"

/************************************************************************/
/* NDVR类型定义                                                         */
/************************************************************************/
// 类型定义[5/24/2008]

//厂商(编码器类型)
#define	HPNVS_XMC	0/*华平*/
#define HPNVS_HIK	1/*海康*/
#define HPNVS_DHD   2/*大华*/
#define HPNVS_NZI	3/*南自*/

#define IS_HPD(dev_type)			(dev_type == HPNVS_XMC)
#define IS_HIK(dev_type)			(dev_type == HPNVS_HIK)
#define IS_DHD(dev_type)			(dev_type == HPNVS_DHD)
#define IS_NZI(dev_type)			(dev_type == HPNVS_NZI)

//广播
#define BROADCAST ""
#define BROADCAST_LDVR "bc_ldvr"/*设备端界面*/
#define BROADCAST_NDVR "bc_ndvr"/*NDVR客户端*/
#define BROADCAST_DVR  BROADCAST_LDVR BROADCAST_NDVR/*NDVR+LDVR*/
#define BROADCAST_REMOTE "bc_remote"/*avcon客户端*/

//判断客户端类型
#define IS_BROADCAST(from_id)			(from_id.find("$")==-1)/*广播*/
#define IS_BROADCAST_LDVR(from_id)		(from_id == BROADCAST || from_id.find(BROADCAST_LDVR)!=-1)/*广播给设备界面*/
#define IS_BROADCAST_NDVR(from_id)		(from_id == BROADCAST || from_id.find(BROADCAST_NDVR)!=-1)/*广播给NDVR客户*/
#define IS_BROADCAST_REMOTE(from_id)	(from_id == BROADCAST || from_id.find(BROADCAST_REMOTE)!=-1)/*广播给AVCON客户*/
#define IS_LDVR_CLIENT(from_id)			(from_id.find("$0$")!=-1)/*设备端界面*/
#define IS_NDVR_CLIENT(from_id)			(from_id.find("$2$")!=-1)/*NDVR客户端*/
#define IS_REMOTE_CLIENT(from_id)		(from_id.find("$3$")!=-1)/*AVCON客户端*/

//用户类型
#define GROUP_ADMINISTRATOR				0/*管理员*/
#define GROUP_OPERATOR					1/*操作员*/
#define GROUP_VIEWER					2/*监看员*/

//登录结果类型
#define ERR_UID					-1	//用户不匹配
#define ERR_PWD					-2	//密码不匹配
#define ERR_LOCK				-3	//用户已锁定
#define ERR_LOGINED				-5	//账户已登录

#define ERR_TRUE				1/*真值*/
#define ERR_FALSE				0/*假值*/
#define ERR_NULL				-1/*空指针*/
#define ERR_UNREADY				-2/*未准备好的*/
#define ERR_PARAM				-3/*参数错误*/
#define ERR_DB					-4/*数据库错误*/
#define ERR_EXIST				-5/*已存在*/
#define ERR_UNKNOW				-99/*未知错误*/

//服务器类型
#define SERVER_RSS				"RSS"
#define SERVER_MCU				"MCU"


//NDVR设备客户端
#define WM_PLAYER_OSD_TEXT				(WM_USER + 0x331)
#define WM_PLAYER_CAPTURETOFILE			(WM_USER + 0x332)
#define WM_AUDIOVIDEO_BITRATE_CHANGED	(WM_USER + 0x333)

//地图热点类型
#define ATTRIB_MAPHOTTYPE_SEHXIANG 1 //摄像头/通道
#define ATTRIB_MAPHOTTYPE_YUZHI 2 //预置点
#define ATTRIB_MAPHOTTYPE_YUNTAI 3 //云台
#define ATTRIB_MAPHOTTYPE_BIQIAN 4 //标签
#define ATTRIB_MAPHOTTYPE_TISHI 5 //提示


#define PTZ_SPEED 0	//速度
#define PTZ_OTHER 1 //其他参数

#define CMD_UP			101   //向上
#define CMD_STOP_UP		201   
#define CMD_DOWN		102   //向下
#define CMD_STOP_DOWN   202
#define CMD_LEFT		103	  //向左
#define CMD_STOP_LEFT	203	  
#define CMD_RIGHT		104   //向右
#define CMD_STOP_RIGHT  204
#define CMD_DN			105   //变距近
#define CMD_STOP_DN     205
#define CMD_DF			106   //变距远
#define CMD_STOP_DF     206
#define CMD_FS			107   //变焦小
#define CMD_STOP_FS		207
#define CMD_FB			108   //变焦大
#define CMD_STOP_FB     208
#define CMD_HS			109   //光圈小
#define CMD_STOP_HS		209
#define CMD_HB			110   //光圈大
#define CMD_STOP_HB     210
#define CMD_AUTO		111	  //自动	
#define CMD_STOP_AUTO	211

//预置点(新)
#define CMD_PRESET		212		//设置预置点
#define CMD_CLR_PRESET	213		//清除预置点
#define CMD_GOTO_PRESET	214		//调用预置点
#define CMD_AUTO_SCAN   218		//自动巡航 == CMD_PATROL_RUN

//预置点(原来)
#define CMD_PREST_SET	301  
#define CMD_PREST_CALL  401		


//巡航命令:Pattern
#define CMD_PATROL_START 216
#define CMD_PATROL_STOP	217
#define CMD_PATROL_RUN	218 //停止巡航可调用上面任一个控制命令

//设置
#define CMD_SET_SPEED		220
#define CMD_SET_PRESET_NAME 221

#define CHANNEL_MAX_NUM			32
#define DEFAULTBRIGHTNESS 6
#define DEFAULTCONTRAST 6
#define DEFAULTSATURATION  6
#define DEFAULTHUE 6
#define DEFAULTVOLUME 50

// + 软键盘 for 华南中石油监控系统整合网关开发软件交办事件处理单 by lgx [4/21/2008]
#define SK_NUM1				1 	//数字1
#define SK_NUM2				2 	//数字2
#define SK_NUM3				3 	//数字3
#define SK_NUM4				4 	//数字4
#define SK_NUM5				5 	//数字5
#define SK_NUM6				6 	//数字6
#define SK_NUM7				7 	//数字7
#define SK_NUM8				8 	//数字8
#define SK_NUM9				9 	//数字9
#define SK_NUM0				10	//数字0

#define SK_POWER			12	//电源
#define SK_MENU				13	//菜单
#define SK_ENTER			14	//确定/进入
#define SK_ESC				15	//取消/退出
#define SK_EDIT				16	//编辑

#define SK_PLAY				18	//回放/预览
#define SK_REC				19  //录像
#define SK_PAN				20	//PAN
#define SK_MULTI			21  //多画面
#define SK_INPUT			22	//输入法
#define SK_INTERPHONE 		23	//对讲
#define SK_SYSINFO			24	//系统信息
#define SK_FORWARD			25  //快进
#define SK_BACKWARD 		26	//快退

#define SK_UP				28	//上
#define SK_UP_END			29	//上结束
#define SK_DOWN				30	//下
#define SK_DOWN_END 		31	//下结束
#define SK_LEFT				32	//左
#define SK_LEFT_END 		33	//左结束
#define SK_RIGHT			34	//右
#define SK_RIGHT_END 		35	//右结束

#define SK_ADD_APERTURE		37	//光圈+
#define SK_ADD_APERTURE_END 38	//光圈+结束
#define SK_SUB_APERTURE		39	//光圈-
#define SK_SUB_APERTURE_END 40	//光圈-结束

#define SK_ADD_FOCUS		42	//聚集+
#define SK_ADD_FOCUS_END	43	//聚集+结束
#define SK_SUB_FOCUS		44	//聚集-
#define SK_SUB_FOCUS_END	45	//聚集-结束

#define SK_ADD_MULTIPLE		47	//变倍+
#define SK_ADD_MULTIPLE_END	48	//变倍+结束
#define SK_SUB_MULTIPLE		49	//变倍-
#define SK_SUB_MULTIPLE_END	50	//变倍-结束

#define CHANNEL_STATE_OFFLINE		0 //离线
#define CHANNEL_STATE_ONLINE		1 //在线
#define CHANNEL_STATE_PREVIEW		2 //预览
#define CHANNEL_STATE_ALERT			3 //报警
#define CHANNEL_STATE_SELECT		4 //选择

// 连接状态
#define DEV_DISCONNECTED			0 // 未连接
#define DEV_CONNECTED				1 // 连接

//AVCON status type
#define AVCON_LOGIN_STATUS		1/*登录*/
#define AVCON_LOGOUT_STATUS		2/*注销*/
#define AVCON_CONNECT_STATUS	3/*连接*/

//1：序列号已其他帐号绑定
//2：帐号不存在
//3：密码错
//4：序列号不正确
//5：帐号已登录
//其他：未知错误
//AVCON login status
#define LOGIN_SUCCESSED			0/*登录成功*/
#define LOGIN_THE_SAME_ACCOUNT	1/*序列号已其他帐号绑定*/
#define LOGIN_ACCOUNT_ERROR		2/*帐号不存在*/
#define LOGIN_PASSWORD_ERROR	3/*密码错*/
#define LOGIN_SERIAL_ERROR		4/*序列号不正确*/
#define LOGIN_ACCOUNT_LOGINED	5/*帐号已登录*/
#define LOGIN_SYSTEM_ERROR		99/*系统错误*/

//AVCON logout status
#define LOGOUT_SUCCESSED		0/*注销成功*/
#define LOGOUT_DISCONNECTED		1/*系统断开*/
#define LOGOUT_SYSTEM_ERROR		99/*系统错误*/

//AVCON connect status
#define HPMON_CONNECTING		10/*正在连接...*/
#define HPMON_RECONNECTED		11/*重连成功*/
#define HPMON_BUSY				12/*服务正忙...*/
#define HPMON_FAILED			13/*连接失败*/	
#define HPMON_CONNECTED			14/*连接成功*/
#define HPMON_DISCONNECTED		15/*断开连接*/

//AVCON device online
#define ONLINE_SUCCESSED		0
#define ONLINE_THE_SAME_ACCOUNT	1/*序列号已其他帐号绑定*/
#define ONLINE_ACCOUNT_ERROR	2/*帐号不存在*/
#define ONLINE_PASSWORD_ERROR	3/*密码错*/
#define ONLINE_SERIAL_ERROR		4/*序列号不正确*/
#define ONLINE_ACCOUNT_LOGINED  5/*帐号已登录*/
#define ONLINE_SYSTEM_ERROR		99/*系统错误*/

#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_ADD		1
#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_SET		2
#define ATTRIB_CHANNEL_RECORDSTATE_YPTE_DEL		3
#define CHANNEL_RECSTATE_NONE					0	// 无录像
#define CHANNEL_RECSTATE_SCHEME_TIME			1	// 计划定时录像
#define CHANNEL_RECSTATE_SCHEME_MOVE			2   // 计划动测录像
#define CHANNEL_RECSTATE_SCHEME_ALARM			3   // 计划报警录像
#define CHANNEL_RECSTATE_SCHEME_MOVEOALARM		4   // 计划动测|报警录像
#define CHANNEL_RECSTATE_SCHEME_MOVEAALARM		5	// 计划动测&报警录像
#define CHANNEL_RECSTATE_NORMAL_MANUAL			6	// 常规手动录像
#define CHANNEL_RECSTATE_NORMAL_MOVE			7   // 常规动测录像
#define CHANNEL_RECSTATE_NORMAL_ALARM			8   // 常规报警录像
#define CHANNEL_RECSTATE_NORMAL_PRE				9   // 常规预录像
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_ADD		1
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_SET		2
#define ATTRIB_CHANNEL_ALARMSTATE_YPTE_DEL		3
#define CHANNEL_ALARMSTATE_NONE					0	// 无报警
#define CHANNEL_ALARMSTATE_DISKFULL				1	// 磁盘满
#define CHANNEL_ALARMSTATE_VIDEOLOST			2   // 视频丢失
#define CHANNEL_ALARMSTATE_MOVEDETECT			3   // 移动侦测
#define CHANNEL_ALARMSTATE_HOSTALARM			4   // 主机IO
#define CHANNEL_ALARMSTATE_MOVEOALARM			5   // 动测|IO
#define CHANNEL_ALARMSTATE_MOVEAALARM			6	// 动测&IO
#define CHANNEL_ALARMSTATE_VIDEOHIDE			7   // 视频遮挡

enum
{
	KIND_OF_DIS_MOVE_DETECT,	// 移动帧测
	KIND_OF_DIS_MASK_ALERT		// 视频遮挡
};

// 录像类型
typedef enum 
{
	RT_ALL			=-1,// 全部
	RT_SCHEDULE		=0,	// 定时录像
	RT_MANUAL		=1,	// 手动录像
	RT_MOVEDETECT	=2,	// 移动帧测
	RT_ALARM		=3,	// 报警录像
	RT_MOVEORALARM	=4,	// 动测|报警
	RT_MOVEANDALARM	=5 // 动测&报警
}RECORD_TYPE;
#define RT_MAXID RT_ALARM+1

// 报警类型
typedef enum 
{
	AT_ALL			=-1,// 全部
	AT_DISKFULL		=0,	// 磁盘空间满
	AT_VIDEOLOST	=1,	// 视频丢失
	AT_MOVEDETECT	=2,	// 移动帧测
	AT_ALARM		=3,	// 主机报警
	AT_MOVEORALARM	=4,	// 帧测|主机
	AT_MOVEANDALARM	=5,	// 帧测&主机
	AT_VIDEOHIDE	=6	// 视频遮挡
}ALERT_TYPE;
#define AT_MAXID  AT_ALARM+1

//磁盘不足报警类型
typedef enum
{
	AT_DISKFULL_ACYLIC = 0,					// 非循环录像磁盘空间满报警
	AT_DISKFULL_CYCLE_NORMAL_COVER_LOST,	// 循环录像磁盘空间满常规覆盖失败
	AT_DISKFULL_CYCLE_COMPEL_COVER_LOST	// 循环录像磁盘空间满强制覆盖失败
}ALERT_DISKFULL_TYPE;


//通道基本状态
typedef enum
{
	CBST_OFFLINE	= 0x00,/*离线*/
	CBST_ONLINE		= 0x01,/*在线*/
	CBST_PREVIEW	= 0x02,/*预览*/
	CBST_RECORD		= 0x04,/*录像*/
	CBST_ALARM		= 0x08/*报警*/
}CHANNEL_BASIC_STATE_TYPE;

//通道录像状态
typedef enum
{
	CRST_SCHEDULE		= 0x01,/*定时录像*/
	CRST_MANUAL			= 0x02,/*手动录像*/
	CRST_MOTION_DETECT	= 0x04,/*移动侦测*/
	CRST_DEFENCE_IO		= 0x08/*防区IO报警*/
}CHANNEL_RECORD_STATE_TYPE;

//通道报警状态
typedef enum
{
	CAST_DISK_FULL		= 0x01,/*磁盘已满*/
	CAST_VIDEO_LOST		= 0x02,/*信号丢失*/
	CAST_MOTION_DETECT	= 0x04,/*移动侦测*/
	CAST_DEFENCE_IO		= 0x08,/*防区IO*/
	CAST_VIDEO_COVER	= 0x10/*视频遮挡*/
}CHANNEL_ALARM_STATE_TYPE;

//视频功能状态
typedef enum
{
	VFST_MOTION_DETECT	= 0x01,/*移动帧测*/
	VFST_VIDEO_HIDE		= 0x02,/*视频遮掩*/
	VFST_VIDEO_LOST		= 0x04,/*视频丢失*/
	VFST_VIDEO_COVER	= 0x08/*视频遮档*/
}VIDEO_FUNCTION_STATE_TYPE;

//通道状态类型
typedef enum
{
	CST_BASIC		= 0x01,/*基本状态*/
	CST_RECORD		= 0x02,/*录像状态*/
	CST_ALARM		= 0x04,/*报警状态*/	
	CST_FUNCTION	= 0x08/*功能状态*/
}CHANNEL_STATE_TYPE;


//  [5/24/2008]
/************************************************************************/
/* NDVR结构体                                                           */
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


// 移动自HPRecordManager.h
// 录像文件信息 lgx
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
	string	filename;			// 文件名.
	string	eid;				// 编码器号
	ULONG	cid;				// 通道号
	int		type;				// 录像类型
	long	filesize;			// 文件大小
	string	filepath;			// 文件路径
	string	starttime;			// 开始时间
	string	stoptime;			// 结束时间
	string  url;				// 网络播放
}RECORDFILEINFO, *LPRECORDFILEINFO;
typedef vector<RECORDFILEINFO> RECORDFILEINFO_LIST; //lgx
//
typedef map<string, string> STRINGTOSTRING_MAP;
typedef STRINGTOSTRING_MAP::iterator STRINGTOSTRING_MAP_IT;

// 操作日志
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
	string  eid;	// 编码器，未用
	string  cid;	// 通道
	string	channelname;
	string	user;	//操作用户
	string	action;	//动作
	string	status;	//状态:成功、失败、越权
	string  group;	//动作类型
	string	datetime;//发生时间
	string	description;//附加描述
}OPERATELOGINFO, *LPOPERATELOGINFO;
typedef vector<OPERATELOGINFO>			OPERATELOGINFO_LIST;
typedef OPERATELOGINFO_LIST::iterator	OPERATELOGINFO_LIST_IT;

// 报警日志
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
	string  eid;		// 编码器，未用
	string  cid;		// 通道
	string	channelname;
	string	from;		// 报警源(探测器)
	string	to;			// 报警设备(警报器)
	string	level;		// 警报级别(内定)
	string	start;		// 触发时间
	string  stop;		// 解除时间
	string	user;		// 解除人
	string  description;
}ALERTLOGINFO, *LPALERTLOGINFO;
typedef vector<ALERTLOGINFO>		ALERTLOGINFO_LIST;
typedef ALERTLOGINFO_LIST::iterator ALERTLOGINFO_LIST_IT;

/*区域结点*/
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
	string	mcuaddr;/*MCU地址*/
	UINT	mcuport;/*MCU端口*/
	string	mcutype;/*MCU类型*/
}REGION_NODE, *LPREGION_NODE;
typedef map<ULONG, REGION_NODE> MAP_RGNINFO;
typedef map<ULONG, REGION_NODE>::iterator MAP_RGNINFO_IT;

typedef struct tagENCODER_INFO// 编码器信息
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
	string	eid;			// 编码器号
	string	encodername;		// 编码器名称
	int		encodertype;		// 编码器类型
	int		channelnum;			// 通道个数
	string	nvsname;			// NVS服务器名称
	string	nvsip;				// NVS服务器IP
	int		nvsport;			// NVS服务器端口
	string	nvsuid;				// NVS登录用户
	string	nvspwd;				// NVS登录密码
	int		nvsstate;			// NVS连接状态
	string  avconcename;		// AVCON中心服务器名
	string  avconceip;			// AVCON中心服务器IP
	int     avconceport;		// AVCON中心服务器端口
	string  avconceuid;			// AVCON中心用户
	string  avconcepwd;			// AVCON中心密码
	int		avconcestate;		// AVCON中心连接状态
	int		tag;				// 标记
	string  desc;				// 描述
	string  serial;             // 设备序列号
	string  version;			// 设备版本号
}ENCODER_INFO, *LPENCODER_INFO;
typedef vector<ENCODER_INFO>			ENCODER_INFO_LIST;
typedef ENCODER_INFO_LIST::iterator		ENCODER_INFO_LIST_IT;
typedef map<string, ENCODER_INFO>		ENCODER_INFO_MAP;
typedef ENCODER_INFO_MAP::iterator		ENCODER_INFO_MAP_IT;


typedef struct tagCHANNEL_INFO// 通道信息
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
	string		eid;/*设备ID*/
	ULONG		cid;/*通道ID*/
	string		encodername;/*设备名称*/		
	string		channelname;/*通道名称*/
	string		locaddr;/*局域网内IP*/
	UINT		tcpport;/*局域网内用动态TCP端口*/	
	UINT        udpport;/*局域网内用动态TCP端口*/
	ULONG		videoid;/*主流ID*/
	ULONG		subvideoid;/*子流ID*/
	ULONG		audioid;/*音频ID*/
	string		mcuid;/*MCUID*/
	string		mcuaddr;/*MCU地址*/
	UINT		mcuport;/*MCU端口*/
	string		mcutype;/*MCU类型*/
	int			state;/*通道状态*/ 
	int			tag;/*标记,保留*/
	string		desc;/*描述*/
	DWORD_PTR	hwnd;/*窗口句柄*/
	DWORD_PTR	parent;/*父窗口句柄*/
}CHANNEL_INFO, *LPCHANNEL_INFO;
typedef vector<CHANNEL_INFO>			CHANNEL_INFO_LIST;
typedef CHANNEL_INFO_LIST::iterator		CHANNEL_INFO_LIST_IT;

typedef map<string, CHANNEL_INFO_LIST>	CINFO_LIST_MAP;
typedef CINFO_LIST_MAP::iterator		CINFO_LIST_MAP_IT;


typedef struct tagVIDEO_INFO// 视频信息
{
	string	eid;			// 编码器
	ULONG	cid;			// 通道
	string	encodername;	
	string	channelname;	
	int     hddevice;		// 0为标清，1为高清
	int		substream;		// 是否启用子流
	int     tomcu;			// 发布到MCU

	//dvr
	int		device;			// 设备
	int     denoise;		// 图像去噪

	int		codec;			// 编码方式
	int		resolution;		// 分辨率
	int		framerate;		// 视频帧率
	int		frametype;		// 视频模式
	int		quality;		// 视频码流
	int		autoquality;	// 图像质量自动控制

	int		codec_sub;		// 子流编码方式
	int		resolution_sub;	// 子分辨率
	int		framerate_sub;	// 子流帧率
	int		frametype_sub;	// 子流模式
	int		quality_sub;	// 子流码流
	int		autoquality_sub;// 图像质量自动控制

	int     quartersize_sub;// 1/4尺寸

	//海康
	NET_DVR_COMPRESSION_INFO ndci;
	NET_DVR_COMPRESSION_INFO ndci_sub;

	//大华
	DH_VIDEOENC_OPT dvo[6];/*0，1，2（主流） （3，4，5）子流*/
	DWORD dwImageSizeMask; /*分辨率掩码，按位表示设备能够支持的分辨率*/
	DWORD dwEncodeModeMask;/*编码模式掩码，按位表示设备能够支持的编码模式*/

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

typedef struct tagAUDIO_INFO// 音频信息
{
	string	eid;			// 编码器
	ULONG	cid;			// 通道
	string	encodername;		
	string	channelname;		

	int		device;			// 设备
	int		codec;			// 编码方式
	int		agc;			// 自动增益
	int		aec;			// 回音消除
	int		vad;			// 静音检测
	int		capture_card;	// 采集音频卡
	int		capture_mode;	// 采集模式：0:MIC，1:LINE_IN，2:WAVE_OUT_MIXER

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


typedef struct tagMCU_INFO// MCU信息
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
	string	eid;/*设备ID*/
	ULONG	cid;/*通道ID*/
	string	encodername;/*设备名称*/		
	string	channelname;/*通道名称*/		
	string	locaddr;/*局域网IP*/	
	UINT	tcpport;/*局域网内用动态TCP端口*/	
	UINT    udpport;/*局域网内用动态TCP端口*/
	ULONG	videoid;/*主流ID*/
	ULONG	subvideoid;/*子流ID*/
	ULONG	audioid;/*音频ID*/
	string	mcuid;/*MCUID*/
	string	mcuaddr;/*MCU地址*/
	UINT	mcuport;/*MCU端口*/
	string	mcutype;/*MCU类型*/
}MCU_INFO, *LPMCU_INFO;
typedef vector<MCU_INFO>				MCU_INFO_LIST;
typedef MCU_INFO_LIST::iterator			MCU_INFO_LIST_IT;




typedef struct tagPTZ_INFO// 云台
{
	string	eid;			
	ULONG	cid;			
	string	encodername;	
	string	channelname;
	int		protocolid;/*云台协议*/
	int		ptzaddr;/*云台地址*/
	int		ptzcomport;/*云台串口*/
	int		defpresetdelay;/*返回缺省预置位的时长(秒)*/
	int		presetidx;/*预置点索引号*/
	int		ctltype;/*控制类型 0:普通 1:转到预置点 2:设置预置点 3:清除预置点 4:取预置点(暂无效)*/
	int     speed;/*控制速度*/
	int     baud;/*波特率*/

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

typedef struct tagVIDEO_PARAM// 视频参数
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
	string	eid;			// 编码器号
	string	encodername;		// 编码器名称
	ULONG	cid;			// 通道号
	string	channelname;		// 通道名称
	int		brightness;			// 光亮度
	int		contrast;			// 对比度
	int		saturation;			// 饱和度
	int		hue;				// 色彩
	int		gamma;				//伽马
	int		sharpen;			//锐化

}VIDEO_PARAM, *LPVIDEO_PARAM;
typedef vector<VIDEO_PARAM>				VIDEO_PARAM_LIST;
typedef VIDEO_PARAM_LIST::iterator		VIDEO_PARAM_LIST_IT;


typedef struct tagRECFILE_INFO// 录像文件信息 
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
	string	filename;			// 文件名
	string	eid;				// 编码器号
	ULONG	cid;				// 通道号
	int		nvstype;			// 厂商类型
	bool	loc_file;			// 是否DVR设备本地录像
	int		rectype;			// 录像类型
	int		scheme;				// 计划录像
	int		upload;				// 上传标识
	LONG	filesize;			// 文件大小
	string	starttime;			// 开始时间
	string	stoptime;			// 结束时间
	string	filepath;			// 文件全路径
	string	prevfile;			// 连续的上一个文件
	string	nextfile;			// 连续的下一个文件
	string	url;				// 远程播放
}RECFILE_INFO, *LPRECFILE_INFO;
typedef vector<RECFILE_INFO>			RECFILE_INFO_LIST; 
typedef RECFILE_INFO_LIST::iterator		RECFILE_INFO_LIST_IT;

// 权限表
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
	string cmd;			//命令字
	string action;		//命令名
	string desc;		//命令描述
	BOOL log;			//是否记录日志
	BOOL allow;			//是否允许执行，根据用户权限配置决定值
	//struct tagCommandsTable *subcmd;	//子命令，
}COMMAND_TABLE, *LPCOMMAND_TABLE;
typedef hash_map<string, COMMAND_TABLE> COMMAND_TABLE_MAP;
typedef COMMAND_TABLE_MAP::iterator		COMMAND_TABLE_MAP_IT;

typedef struct tagHPUSER_INFO// 用户信息 
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
	string	name;				// 用户
	int		type;				// 类型
	string	regions;			// 区域
	string  rights;				// 权限
}HPUSER_INFO, *LPHPUSER_INFO;
typedef vector<HPUSER_INFO>			USER_INFO_LIST; 
typedef USER_INFO_LIST::iterator	USER_INFO_LIST_IT;

typedef struct tagMOVE_DETECT_INFO// 移动帧测信息
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
	string	eid;	// 编码器
	ULONG   cid;	// 通道
	RECT	rect[5];	// 区域
	int		number;     // 区域个数
	int		grade;		// 敏感度
	bool	started;    // 启动
}MOVE_DETECT_INFO;
typedef vector<MOVE_DETECT_INFO>		MOVE_DETECT_INFO_LIST; 
typedef MOVE_DETECT_INFO_LIST::iterator	MOVE_DETECT_INFO_LIST_IT;

typedef struct tagVIDEO_MASK_INFO// 视频遮掩信息
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
	string	eid;	// 编码器
	ULONG   cid;	// 通道
	RECT	rect[5];	// 区域
	int		number;     // 区域个数
	bool	started;    // 启动
}VIDEO_MASK_INFO;
typedef vector<VIDEO_MASK_INFO>			VIDEO_MASK_INFO_LIST; 
typedef VIDEO_MASK_INFO_LIST::iterator	VIDEO_MASK_INFO_LIST_IT;

typedef struct tagPREPOINT_INFO // 预置点信息
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
	string	eid;	// 编码器
	ULONG   cid;	// 通道
	ULONG	prepointid; // 预置点
	string  name;		// 预置点名称
	int     defenceid;  // 防区
	BOOL bIfDefaultPrePoint;
	bool bAutoReturn;		//是否自动归位
	int  iReturnTime;	//归位时间 秒
}PREPOINT_INFO,*LPPREPOINT_INFO;
typedef vector<PREPOINT_INFO>			PREPOINT_INFO_LIST; 
typedef PREPOINT_INFO_LIST::iterator	PREPOINT_INFO_LIST_IT;


// 报警联动
#define WM_USER_DETECTER_ALERT_MSG  WM_USER + 21
enum { 

	DETECTER_ALERT_WITH_BONDED_TO_PRESET,
	DETECTER_ALERT_WITHOUT_BONDED_TO_PRESET,
	DETECTER_ALERT_CANCEL
};

typedef struct tagALERT_INFO// 报警信息
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
	string	eid;	// 编码器
	ULONG   cid;	// 通道
	int		alerttype;  // 报警类型
	int     alertid;	// 报警ID
	int		nvstype;	// NVS类型
	int		prepoint;	// 预置点
	bool	scheme;		// 是否计划
}ALERT_INFO;
typedef vector<ALERT_INFO>			ALERT_INFO_LIST; 
typedef ALERT_INFO_LIST::iterator	ALERT_INFO_LIST_IT;

typedef struct tagUPLOAD_INFO// 上传信息
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
	string  server;		// 服务器
	string	ip0;		// 录像服务器IP0
	string	ip1;		// 录像服务器IP1
	string	ip2;		// 录像服务器IP2
	int		port;		// 上传端口号
	int		type;		// 上传类型 
	bool	autoupload;	// 自动上传
	string	desc;		// 描述
}UPLOAD_INFO;
typedef vector<UPLOAD_INFO>			UPLOAD_INFO_LIST; 
typedef UPLOAD_INFO_LIST::iterator	UPLOAD_INFO_LIST_IT;


typedef struct tagDevCfgCopyInfo //设备复制信息
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
	bool   copy;		// 已复制
	string fromeid;		// 源编码器
	ULONG  fromcid;		// 源通道
	string toeid;		// 目的编码器
	ULONG  tocid;		// 目的通道
}DevCfgCopyInfo;

//音视频波特率
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
	ULONG width;	// + 分辨率 宽 by lgx [1/24/2008]
	ULONG height;   // + 分辨率 高 by lgx [1/24/2008]
}AVBITRATE;



//报警联动
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

//报警日志
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


//操作日志
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

//操作输入端口
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

//报警参数
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



//视频参数的相关最大值和最小值
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









