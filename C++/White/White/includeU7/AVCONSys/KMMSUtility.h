#pragma once

#include "KMMSDefine.h"
#include "KGeneralFunc.h"
/************************************************************************/
/* 打包，解包数据结构                                                   */
/* 打包时，可能使用 KCmdPacket，也可能使用 KCmdItem，所以用模板         */
/* 要针对不同的数据类型打包，所以要重载相应数据类型的模板               */
/************************************************************************/

//------------------------------------------------------------------------
inline void CMDPacket( KCmdItem& t, PMMS_WND_PRO ptMMS_WND_PRO)
{
    t.SetAttribUL( "WNDID", ptMMS_WND_PRO->nWndid);
    t.SetAttribUL( "RESOLUTION", ptMMS_WND_PRO->resolution);
    t.SetAttribUL( "IMAQUALITY", ptMMS_WND_PRO->imaquality);
    t.SetAttribUL( "STREAM", ptMMS_WND_PRO->stream);
    t.SetAttribUL( "FRAMERATE", ptMMS_WND_PRO->framerate);
}

//------------------------------------------------------------------------
inline void CMDUnPacket( KCmdItem& t, PMMS_WND_PRO ptMMS_WND_PRO)
{
    ptMMS_WND_PRO->nWndid       = t.GetAttrib("WNDID").AsUnsignedInt();
    ptMMS_WND_PRO->resolution   = t.GetAttrib("RESOLUTION").AsUnsignedInt();
    ptMMS_WND_PRO->imaquality   = t.GetAttrib("IMAQUALITY").AsUnsignedInt();
    ptMMS_WND_PRO->stream       = t.GetAttrib("STREAM").AsUnsignedInt();
    ptMMS_WND_PRO->framerate    = t.GetAttrib("FRAMERATE").AsUnsignedInt();
}

//------------------------------------------------------------------------
inline void CMDPacket( KCmdItem& t, PMMS_SCREEN_PRO ptMMS_SCREEN_PRO)
{
    t.SetAttribUL( "SCREENID", ptMMS_SCREEN_PRO->nScreenID);
    t.SetAttribUL( "TMID", ptMMS_SCREEN_PRO->nTMID);
}


//------------------------------------------------------------------------
inline void CMDUnPacket( KCmdItem& t, PMMS_SCREEN_PRO ptMMS_SCREEN_PRO)
{
    ptMMS_SCREEN_PRO->nScreenID = t.GetAttrib("SCREENID").AsUnsignedInt();
    ptMMS_SCREEN_PRO->nTMID = t.GetAttrib("TMID").AsUnsignedInt();
}


//------------------------------------------------------------------------
inline void CMDPacket( KCmdItem& t, PMMS_ROOMTYPE ptMMS_ROOMTYPE)
{
    t.SetAttribUL( "ROOMTYPEID", ptMMS_ROOMTYPE->nRoomTypeID);
    t.SetAttribUL( "SCREENCOUNT", ptMMS_ROOMTYPE->nScreenCount);
    t.SetAttribUL( "PREEMPTYPE", ptMMS_ROOMTYPE->nPreempType);
    t.SetAttribUL( "EXSPOKENWIN", ptMMS_ROOMTYPE->nExspokeswin);

    t.SetAttrib( "ROOMTYPENAME", ptMMS_ROOMTYPE->strRoomTypeName);
    t.SetAttrib( "FUNCLIST", ptMMS_ROOMTYPE->strFucList);
}


//------------------------------------------------------------------------
inline void CMDUnPacket( KCmdItem& t, PMMS_ROOMTYPE ptMMS_ROOMTYPE)
{
    ptMMS_ROOMTYPE->nRoomTypeID = t.GetAttrib("ROOMTYPEID").AsUnsignedInt();
    ptMMS_ROOMTYPE->nScreenCount = t.GetAttrib("SCREENCOUNT").AsUnsignedInt();
    ptMMS_ROOMTYPE->nPreempType    = t.GetAttrib("PREEMPTYPE").AsUnsignedInt();
    ptMMS_ROOMTYPE->nExspokeswin    = t.GetAttrib("EXSPOKENWIN").AsUnsignedInt();

    ptMMS_ROOMTYPE->strRoomTypeName = t.GetAttrib("ROOMTYPENAME").AsString();
    ptMMS_ROOMTYPE->strFucList = t.GetAttrib("FUNCLIST").AsString();
}

//------------------------------------------------------------------------
inline void CMDPacket( KCmdItem& t, PMMS_PHY_ROOM ptMMS_PHY_ROOM)
{
    t.SetAttribUL( "PHYROOMID", ptMMS_PHY_ROOM->nPhyRoomID);
    t.SetAttribUL( "ROOMTYPEID", ptMMS_PHY_ROOM->nRoomTypeID);
    t.SetAttrib( "ROOMNAME", ptMMS_PHY_ROOM->strRoomName);
	t.SetAttrib("ORDERID",ptMMS_PHY_ROOM->strOrderID);
}


//------------------------------------------------------------------------
inline void CMDUnPacket( KCmdItem& t, PMMS_PHY_ROOM ptMMS_PHY_ROOM)
{
    ptMMS_PHY_ROOM->nPhyRoomID  = t.GetAttrib("PHYROOMID").AsUnsignedInt();
    ptMMS_PHY_ROOM->nRoomTypeID = t.GetAttrib("ROOMTYPEID").AsUnsignedInt();
	ptMMS_PHY_ROOM->strRoomName = t.GetAttrib("ROOMNAME").AsString();
    ptMMS_PHY_ROOM->strOrderID = t.GetAttrib("ORDERID").AsString();
}

//------------------------------------------------------------------------
inline void CMDPacket( KCmdPacket& t, PMMS_TALK_SET ptMMS_TALK_SET)
{
    t.SetAttribUL( "RESOLUTION", ptMMS_TALK_SET->resolution);
    t.SetAttribUL( "STREAM", ptMMS_TALK_SET->stream);
    t.SetAttribUL( "FRAMERATE", ptMMS_TALK_SET->framerate);

    t.SetAttrib( "FUNCLIST", ptMMS_TALK_SET->strFucList);
}


//------------------------------------------------------------------------
inline void CMDUnPacket( KCmdPacket& t, PMMS_TALK_SET ptMMS_TALK_SET)
{
    ptMMS_TALK_SET->resolution  = t.GetAttrib("RESOLUTION").AsUnsignedInt();
    ptMMS_TALK_SET->stream      = t.GetAttrib("STREAM").AsUnsignedInt();
    ptMMS_TALK_SET->framerate   = t.GetAttrib("FRAMERATE").AsUnsignedInt();

    ptMMS_TALK_SET->strFucList  = t.GetAttrib("FUNCLIST").AsString();
}

/************************************************************************/
/* 向 MMS_ROOMTYPE_LST 中添加元素                                       */
/************************************************************************/
inline PMMS_SCREEN_PRO InserMMS_SCREEN_PRO( MMS_ROOMTYPE_LST& lstRoomType, int nRoomTypeID, int nScreenID )
{
    PMMS_SCREEN_PRO ptMMS_SCREEN_PRO = NULL;
    MMS_ROOMTYPE tMMS_ROOMTYPE;
    tMMS_ROOMTYPE.nRoomTypeID = nRoomTypeID;
    MMS_ROOMTYPE_LST::iterator itRoomType = find_if( lstRoomType.begin(), lstRoomType.end(), bind1st(TComparePointer<MMS_ROOMTYPE>(), &tMMS_ROOMTYPE));
    if ( itRoomType != lstRoomType.end() )
    {
        PMMS_ROOMTYPE ptMMS_ROOMTYPE = *itRoomType;
        ptMMS_SCREEN_PRO = new MMS_SCREEN_PRO;
        ptMMS_ROOMTYPE->mapScreenPro.insert( MMS_SCREEN_PRO_MAP::value_type( nScreenID, ptMMS_SCREEN_PRO));
    }

    return ptMMS_SCREEN_PRO;
}


inline PMMS_WND_PRO InserMMS_WND_PRO( MMS_ROOMTYPE_LST& lstRoomType, int nRoomTypeID, int nScreenID, int nWndid)
{
    PMMS_WND_PRO ptMMS_WND_PRO = NULL;
    MMS_ROOMTYPE tMMS_ROOMTYPE;
    tMMS_ROOMTYPE.nRoomTypeID = nRoomTypeID;

    // 找会议室
    MMS_ROOMTYPE_LST::iterator itRoomType= find_if( lstRoomType.begin(), lstRoomType.end(), bind1st(TComparePointer<MMS_ROOMTYPE>(), &tMMS_ROOMTYPE));
    if ( itRoomType != lstRoomType.end() )
    {
        PMMS_ROOMTYPE ptMMS_ROOMTYPE = *itRoomType;
        MMS_SCREEN_PRO_MAP::iterator itScreenPro = ptMMS_ROOMTYPE->mapScreenPro.find(nScreenID);
        if ( itScreenPro != ptMMS_ROOMTYPE->mapScreenPro.end())
        {
            PMMS_SCREEN_PRO ptMMS_SCREEN_PRO = itScreenPro->second;
            ptMMS_WND_PRO = new MMS_WND_PRO;
            ptMMS_SCREEN_PRO->mapWndPro.insert( MMS_WND_PRO_MAP::value_type( nWndid, ptMMS_WND_PRO));
        }
    }
    return ptMMS_WND_PRO;
}

