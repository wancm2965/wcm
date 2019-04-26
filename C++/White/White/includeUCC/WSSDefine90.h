#pragma once

//=======================================================================================
typedef void(*AVCON_WS_Global_SetTimeout)(DWORD dwTimeout);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetOAStatus)(const std::string& strServerHost,unsigned short usServerPort,
                                              const std::string& strProxyServer,unsigned short usProxyPort,
                                              const std::string& strUserID,const std::string& strPassword,
											  std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetUserAuth)(const std::string& strServerHost,unsigned short usServerPort,
											  const std::string& strProxyServer,unsigned short usProxyPort,
                                              const std::string& strUserID,const std::string& strNodeID,
                                              std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetLDAPAuth)(const std::string& strServerHost,unsigned short usServerPort,
											  const std::string& strProxyServer,unsigned short usProxyPort,
                                              const std::string& strLDAPHost,unsigned short usLDAPPort,
                                              const std::string& strDomain,const std::string& strUserID,const std::string& strPassword,
                                              std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetLDAPAuthEx)(const std::string& strServerHost,unsigned short usServerPort,
											    const std::string& strProxyServer,unsigned short usProxyPort,
                                                const std::string& strLDAPHost,unsigned short usLDAPPort,
                                                const std::string& strDomain,const std::string& strUserID,const std::string& strPassword,
												const std::string& strUserKey,const std::string& strLocalIP,
                                                std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetRadiusAuth)(const std::string& strServerHost,unsigned short usServerPort,
											    const std::string& strProxyServer,unsigned short usProxyPort,
                                                const std::string& strRadiusHost,unsigned short usRadiusPort,
                                                const std::string& strSecret,const std::string& strUserID,const std::string& strPassword,
                                                std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_OAManager_GetPkiAuth)(const std::string& strServerHost,unsigned short usServerPort,
											 const std::string& strProxyServer,unsigned short usProxyPort,
                                             const std::string& strLocalIP,const std::string& strUserID,const std::string& strPassword,const std::string& strAuthKey,
                                             std::string& strResult);

//=======================================================================================
typedef bool(*AVCON_WS_LossManager_SetLoss)(const std::string& strServerHost,unsigned short usServerPort,
                                            const std::string& strProxyServer,unsigned short usProxyPort,
                                            const std::string& strUserID,const std::string& strPassword,
                                            const std::string& strLossID,const std::string& strCarID,const std::string& strMemID,const std::string& strFactoryID,double dMoney,unsigned short usStatus,
                                            std::string& strResult);

//=======================================================================================
typedef bool (*AVCON_WS_LossManager_GetLoss)(const std::string& strServerHost,unsigned short usServerPort,
                                             const std::string& strProxyServer,unsigned short usProxyPort,
                                             const std::string& strUserID,const std::string& strPassword,
                                             const std::string& strLossID,std::string& strResult);

//=======================================================================================
typedef bool (*AVCON_WS_LossManager_GetLossList)(const std::string& strServerHost,unsigned short usServerPort,
                                                 const std::string& strProxyServer,unsigned short usProxyPort,
                                                 const std::string& strUserID,const std::string& strPassword,std::string& strResult);