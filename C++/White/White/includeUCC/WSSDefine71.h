#pragma once

//=======================================================================================
typedef void(*AVCON_WS_Global_SetTimeout)(DWORD dwTimeout);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetOAStatus)(CString csServerHost,unsigned short usServerPort,
                                              CString csProxyServer,unsigned short usProxyPort,
                                              CString csUserID,CString csPassword,
                                              CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetUserAuth)(CString csServerHost,unsigned short usServerPort,
											  CString csProxyServer,unsigned short usProxyPort,
                                              CString csUserID,CString csNodeID,
                                              CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetLDAPAuth)(CString csServerHost,unsigned short usServerPort,
											  CString csProxyServer,unsigned short usProxyPort,
                                              CString csLDAPHost,unsigned short usLDAPPort,
                                              CString csDomain,CString csUserID,CString csPassword,
                                              CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetLDAPAuthEx)(CString csServerHost,unsigned short usServerPort,
											    CString csProxyServer,unsigned short usProxyPort,
                                                CString csLDAPHost,unsigned short usLDAPPort,
                                                CString csDomain,CString csUserID,CString csPassword,
												CString csUserKey,CString csLocalIP,
                                                CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetRadiusAuth)(CString csServerHost,unsigned short usServerPort,
											    CString csProxyServer,unsigned short usProxyPort,
                                                CString csRadiusHost,unsigned short usRadiusPort,
                                                CString csSecret,CString csUserID,CString csPassword,
                                                CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetPkiAuth)(CString csServerHost,unsigned short usServerPort,
											 CString csProxyServer,unsigned short usProxyPort,
                                             CString csLocalIP,CString csUserID,CString csPassword,CString csAuthKey,
                                             CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_LossManager_SetLoss)(CString csServerHost,unsigned short usServerPort,
                                            CString csProxyServer,unsigned short usProxyPort,
                                            CString csUserID,CString csPassword,
                                            CString csLossID,CString csCarID,CString csMemID,CString csFactoryID,double dMoney,unsigned short usStatus,
                                            CString& csResult);

//=======================================================================================
typedef bool (*AVCON_WS_LossManager_GetLoss)(CString csServerHost,unsigned short usServerPort,
                                             CString csProxyServer,unsigned short usProxyPort,
                                             CString csUserID,CString csPassword,
                                             CString csLossID,CString& csResult);

//=======================================================================================
typedef bool (*AVCON_WS_LossManager_GetLossList)(CString csServerHost,unsigned short usServerPort,
                                                 CString csProxyServer,unsigned short usProxyPort,
                                                 CString csUserID,CString csPassword,CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_VodManageer_GetRecFileList)(CString csUrl, CString csProxyServer, unsigned short usProxyPort, 
												   CString csUserID,CString csNodeID, unsigned int uPageSize, unsigned int uPage,
												   CString csStartTime, CString csEndTime, CString csFileName, CString csRecUserName,
												   CString csRoomName, CString csChnName, CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_VodManageer_GetAllRecFileList)(CString csUrl, CString csProxyServer, unsigned short usProxyPort, 
													  CString csUserID,CString csNodeID, unsigned int uPageSize, unsigned int uPage,
													  CString csStartTime, CString csEndTime, CString csFileName, CString csRecUserName,
													  CString csRoomName, CString csChnName, CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_VodManageer_GetRecRemoteList)(CString csUrl, CString csProxyServer, unsigned short usProxyPort, 
													 CString csUserID,CString csNodeID, CString& csResult);

//=======================================================================================
typedef bool(*AVCON_WS_VodManageer_SetRecFile)(CString csUrl, CString csProxyServer, unsigned short usProxyPort, 
											   CString csUserID,CString csNodeID, CString csRoomID, CString csRoomName, 
											   CString csDevID, CString csChnID, CString csChnName, CString csRecUserID, 
											   CString csRecUserName, CString csRecServerID, CString csStartTime, 
											   CString csEndTime, CString csFileAddr, CString csFileNickName,
											   CString csFileStatus, CString csFileSize, CString& csResult);

