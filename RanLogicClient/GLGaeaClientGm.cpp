#include "pch.h"
#include <shlobj.h>
#include "../SigmaCore/String/StringFormat.h"
#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../RanLogic/Crow/GLCrowData.h"
#include "../RanLogic/Crow/GLCrowDataMan.h"

#include "./Pet/GLPetClient.h"
#include "./Land/GLLandManClient.h"
#include "./Effect/GLFactEffect.h"
#include "./Char/GLCharacter.h"
#include "./DamageRecord/DamageRecord.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLGaeaClient::MsgServerStopFb(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_REQ_SERVERSTOP_FB* pNetMsg = ( GLMSG::SNET_REQ_SERVERSTOP_FB* ) pMsg;

    m_pLandMClient->ResetClientObjects ();

	std::string strCombine( "" );
	switch ( pNetMsg->dwError )
	{
	case SERVER_STOP_NOMAL:
		strCombine = sc::string::format( "%s", ID2GAMEINTEXT( "SERVER_STOP_SOON" ) );
		break;
	case SERVER_STOP_OUTOFMEMORY:
	case SERVER_STOP_JUST_RESTART:
		strCombine = sc::string::format( "%s", ID2GAMEINTEXT( "SERVER_STOP_SOON_JUST_RESTART" ) );
		break;
	}

    PrintMsgTextDlg( NS_UITEXTCOLOR::LIGHTSKYBLUE, strCombine );

    for (WORD i=0; i<MAX_PETSKILLSLOT; ++i)
    {
        m_pCharacter->m_sPETSKILLFACT[i].RESET();
        if (m_pPet->m_sActiveSkillID[i] == NATIVEID_NULL())
            continue;
        FACTEFF::DeleteSkillFactEffect(
            STARGETID(CROW_PC,m_pCharacter->GetGaeaID(),m_pCharacter->GetPosition()),
            m_pCharacter->GetSkinChar(),
            m_pPet->m_sActiveSkillID[i]);
    }
    m_pPet->Delete();
}

void GLGaeaClient::MsgGmCountDownBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_COUNTDOWN_MSG_BRD* pNetMsg = (GLMSG::SNET_GM_COUNTDOWN_MSG_BRD*) pMsg;
	//!! SF_TODO
    //m_pInterface->SET_COUNT_MSG( pNetMsg->nCount );
}

void GLGaeaClient::MsgInstantMapReEntryCheckFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_INSTANT_MAP_REENTRY_CHECK_FB* pNetMsg = (GLMSG::SNET_INSTANT_MAP_REENTRY_CHECK_FB*) pMsg;
    GLMapList::FIELDMAP map_list = GetMapList();
    GLMapList::FIELDMAP_CITER iter = map_list.begin ();
    GLMapList::FIELDMAP_CITER iter_end = map_list.end ();

    for ( ; iter!=iter_end; ++iter )
    {
        if (!iter->second.IsInstantMap())
            continue;
        // 재 입장 시간이 저장되어 있던 경우
        DWORD i = 0;
        for( i = 0; i < pNetMsg->nLimitCount; i++ )
        {
            if( pNetMsg->nRemainMinute[i] == 0 )
                break;
            if( iter->second.m_NativeID.wMainID == pNetMsg->sMapID[i].wMainID )
            {						
                PrintMsgText(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(
                    ID2GAMEINTEXT("REQ_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT"), 
                    GetMapName(iter->second.GetId()),
                    pNetMsg->nRemainMinute[i]));
                break;
            }
        }
        if( i != pNetMsg->nLimitCount ) continue;

        // 재 입장이 가능한 경우				
        PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(
            ID2GAMEINTEXT("REQ_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT"), 
            GetMapName(iter->second.GetId())));															
    }
}

void GLGaeaClient::MsgGmWarningBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WARNING_MSG_BRD* pNetMsg = (GLMSG::SNET_GM_WARNING_MSG_BRD*) pMsg;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ShowWarning,
		"-b", pNetMsg->bOn );
}

void GLGaeaClient::MsgUserCharInfoFldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_USER_CHAR_INFO_FLD_FB *pNetMsg = (GLMSG::SNET_USER_CHAR_INFO_FLD_FB *) pMsg;			
    PrintConsoleText(sc::string::format("club : %s", pNetMsg->szCLUB));
}

void GLGaeaClient::MsgGmCharInfoFldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_CHAR_INFO_FLD_FB *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_FLD_FB *) pMsg;						
    PrintConsoleText(sc::string::format("charid       : %d", pNetMsg->dwCHARID));			
    PrintConsoleText(sc::string::format("mapid        : %d/%d", pNetMsg->nidMAP.wMainID, pNetMsg->nidMAP.wSubID));			
    PrintConsoleText(sc::string::format("position     : %d/%d", pNetMsg->wPOSX, pNetMsg->wPOSY));			
    PrintConsoleText(sc::string::format("level        : %d", pNetMsg->wLEVEL));			
    PrintConsoleText(sc::string::format("hp           : %d/%d", pNetMsg->sHP.nNow, pNetMsg->sHP.nMax));			
    PrintConsoleText(sc::string::format("mp           : %d/%d", pNetMsg->sMP.nNow, pNetMsg->sMP.nMax));			
    PrintConsoleText(sc::string::format("sp           : %d/%d", pNetMsg->sSP.nNow, pNetMsg->sSP.nMax));			
    //std::strstream strSTREAM;
    //strSTREAM << pNetMsg->sEXP.lnNow << '/' << pNetMsg->sEXP.lnMax << std::ends;			
    PrintConsoleText(sc::string::format("exp          : %1%/%2%", pNetMsg->sEXP.lnNow, pNetMsg->sEXP.lnMax));			
    PrintConsoleText(sc::string::format("club         : %s", pNetMsg->szCLUB));
    //strSTREAM.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
}

void GLGaeaClient::MsgUserCharInfoAgtFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_USER_CHAR_INFO_AGT_FB *pNetMsg = (GLMSG::SNET_USER_CHAR_INFO_AGT_FB *) pMsg;			
    PrintConsoleText(sc::string::format("char name : %s", pNetMsg->szCHARNAME));
}

void GLGaeaClient::MsgGmCharInfoAgtFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_CHAR_INFO_AGT_FB *pNetMsg = (GLMSG::SNET_GM_CHAR_INFO_AGT_FB *) pMsg;			
    PrintConsoleText(sc::string::format("user account : %s", pNetMsg->szUACCOUNT));
    PrintConsoleText(sc::string::format("char name    : %s", pNetMsg->szCHARNAME));			
    PrintConsoleText(sc::string::format("channel      : %d", pNetMsg->dwCHANNEL));			
    PrintConsoleText(sc::string::format("charid       : %d", pNetMsg->dwCHARID));			
    PrintConsoleText(sc::string::format("gaeaid		 : %d", pNetMsg->dwGAEAID));
}

void GLGaeaClient::MsgGmChatBlockFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_CHAT_BLOCK_FB *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_FB *) pMsg;			
    PrintConsoleText(sc::string::format("block        : %s", bool2sz(pNetMsg->bBLOCK)));
    PrintConsoleText(sc::string::format("user account : %s", pNetMsg->szUACCOUNT));			
    PrintConsoleText(sc::string::format("char name    : %s", pNetMsg->szCHARNAME));			
    PrintConsoleText(sc::string::format("block minute : %d", pNetMsg->dwBLOCK_MINUTE));
}

/**
	GM이 유저에게 채팅금지 시킬 경우
	GM이 정한 시간동안 채팅을 금지시키고
	유저에게 채팅차단상태, 시간을 알려준다.
*/
void GLGaeaClient::MsgGmChatBlockNotify(NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY *pNetMsg = (GLMSG::SNET_GM_CHAT_BLOCK_NOTIFY *) pMsg;
	if( m_pCharacter == NULL )
	{
		return;
	}	

	m_pCharacter->m_tCHATBLOCK = pNetMsg->tChatBlock;

	CTime cTime( CTime::GetCurrentTime() );
	CTime cBLOCK( pNetMsg->tChatBlock );
	GetConvertServerTime( cTime );
	
	CTimeSpan cSPAN = cBLOCK - cTime;

	if( m_pCharacter->m_tCHATBLOCK != 0 )
	{
		std::string strNotify = sc::string::format( ID2GAMEINTEXT("NOTIFY_CHAT_BLOCK_STATE"), cSPAN.GetTotalMinutes() );
		PrintAddString(NS_UITEXTCOLOR::ORANGE, strNotify.c_str());

		GLChatMan::GetInstance().SetGMChatBlock( true );
	}
}

void GLGaeaClient::MsgGmCheckInstantNumFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_GM_CHECKINSTANTNUM_FB *pNetMsg = (GLMSG::SNETPC_GM_CHECKINSTANTNUM_FB *) pMsg;            
    PrintConsoleText(
        sc::string::format(
        "The number %d is generated instant map",
        pNetMsg->dwInstantmapCnt));
}

void GLGaeaClient::MsgGmMoveToCharFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_GM_MOVE2CHAR_FB *pNetMsg = (GLMSG::SNETPC_GM_MOVE2CHAR_FB *) pMsg;

    switch ( pNetMsg->emFB )
    {
    case EMGM_MOVE2CHAR_FB_FAIL:
        PrintConsoleText ( ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_FAIL") );
        break;
    case EMGM_MOVE2CHAR_FB_OK:
        PrintConsoleText ( ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_OK") );
        break;
    case EMGM_MOVE2CHAR_FB_TO_CONDITION:
        PrintConsoleText ( ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_TO_CONDITION") );
        break;
    case EMGM_MOVE2CHAR_FB_TO_INSTANTMAP:
        PrintConsoleText ( ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_TO_INSTANTMAP") );
        break;
    case EMGM_MOVE2CHAR_FB_MY_CONDITION:
        PrintConsoleText ( ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_MY_CONDITION") );
        break;
    case EMGM_MOVE2CHAR_FB_CHANNEL:
        {
            PrintConsoleText(
                sc::string::format(
                ID2GAMEINTEXT("EMGM_MOVE2CHAR_FB_CHANNEL"),
                pNetMsg->nChannel));
        }
        break;
    };
}

void GLGaeaClient::MsgGmWherePcPosFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WHERE_PC_POS_FB *pNetMsg = (GLMSG::SNET_GM_WHERE_PC_POS_FB *) pMsg;

    if (pNetMsg->bFOUND)
    {                
        PrintConsoleText(
            sc::string::format(
            "pc_pos : %d, %d",
            pNetMsg->nPosX,
            pNetMsg->nPosY));
    }
    else
    {
        PrintConsoleText("pc_pos : not found");
    }
}

void GLGaeaClient::MsgGmWherePcMapFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WHERE_PC_MAP_FB* pNetMsg = (GLMSG::SNET_GM_WHERE_PC_MAP_FB*) pMsg;

    const char* pMapName = GetMapName(pNetMsg->nidMAP);
    if (pMapName)
    {   
        PrintConsoleText(
            sc::string::format(
            "pc_map : %s, channel %d, mapid[%d/%d]",
            pMapName,
            pNetMsg->dwChannel,
            pNetMsg->nidMAP.wMainID,
            pNetMsg->nidMAP.wSubID));
    }
}


void GLGaeaClient::MsgGmPrintCrowListFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_PRINT_CROWLIST_FB *pNetMsg = (GLMSG::SNET_GM_PRINT_CROWLIST_FB *) pMsg;

    const PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( pNetMsg->mID, pNetMsg->sID );
    if (pCrow)
    {			    
        if (pNetMsg->emCrow == CROW_NPC)
        {                    
            PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(
                "1234 [%s]",
                "type : npc, mid : %d, SID : %d, num : %d, name : %s", 
                pNetMsg->mID,
                pNetMsg->sID,
                pNetMsg->wNum,
                pCrow->GetName()));
        }
        else
        {                    
            PrintMsgText(
                NS_UITEXTCOLOR::RED,
                sc::string::format(
                "type : monster, mid : %d, SID : %d, num : %d, name : %s", 
                pNetMsg->mID,
                pNetMsg->sID,
                pNetMsg->wNum,
                pCrow->GetName()));
        }			
    }
}


void GLGaeaClient::MsgGmWhereNpcAllFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WHERE_NPC_ALL_FB *pNetMsg = (GLMSG::SNET_GM_WHERE_NPC_ALL_FB *) pMsg;

    const PCROWDATA pCrow = GLCrowDataMan::GetInstance().GetCrowData( pNetMsg->nidNPC.wMainID, pNetMsg->nidNPC.wSubID );
    if (pCrow)
    {
        PrintConsoleText(
            sc::string::format(
            "%s, id : %d %d, pos : %d %d",
            pCrow->GetName(),
            pNetMsg->nidNPC.wMainID, 
            pNetMsg->nidNPC.wSubID,
            pNetMsg->nPosX,
            pNetMsg->nPosY));
    }
}


void GLGaeaClient::MsgGmWhereNpcFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WHERE_NPC_FB *pNetMsg = (GLMSG::SNET_GM_WHERE_NPC_FB *) pMsg;
    PrintConsoleText(
        sc::string::format("npc_pos : %d, %d", pNetMsg->nPosX, pNetMsg->nPosY));
}

void GLGaeaClient::MsgGmEventExpEnd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_EVENT_EXP_END_FB *pNetMsg = (GLMSG::SNET_GM_EVENT_EXP_END_FB *) pMsg;			            
    PrintConsoleText(
        sc::string::format(
        "Experience Event End, Field Server [%d]",
        pNetMsg->wFIELDSVR));
}

void GLGaeaClient::MsgGmEventItemGenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_EVEN_ITEM_GEN_FB *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN_FB *) pMsg;
    float fSCALE = pNetMsg->fRATE;
    PrintConsoleText(
        sc::string::format("item Gen Event Begin, scale : %3.4f", fSCALE));
}


void GLGaeaClient::MsgGmWhisperStateFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_WHISPER_STATE_FB *pNetMsg = (GLMSG::SNET_GM_WHISPER_STATE_FB*) pMsg;

    if ( pNetMsg->bOn )
    {
        PrintConsoleText( "Whisper State : on" );
    }
    else
    {
        PrintConsoleText( "Whisper State : off" );
    }
}

void GLGaeaClient::MsgGmNonConfrontMode(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_NONCONFRONT_MODE *pNetMsg = (GLMSG::SNET_GM_NONCONFRONT_MODE*) pMsg;

    if ( pNetMsg->bMode )
    {
        PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("NONCONFRONT_MODE_ON_MSG") );
    }
    else
    {
        PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("NONCONFRONT_MODE_OFF_MSG") );
    }
}

void GLGaeaClient::MsgGmNonConfrontModeViewFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_FB *pNetMsg = (GLMSG::SNET_GM_NONCONFRONT_MODE_VIEW_FB*) pMsg;

    PrintConsoleText("Nonconfront Maps Info : ");
    PrintConsoleText("[MapMID] [MapSID] [Channel] [ApplyTime(min)] [RemainedTime(min)]");

    for (WORD i=0; i<pNetMsg->wSize; ++i)
    {
        if (pNetMsg->sInfo[i].dwAPPLYTIME == -1)
        {
            PrintConsoleText(
                sc::string::format(
                "[%3d] [%3d] [%d] [UNLIMITED] [UNLIMITED]",
                pNetMsg->sInfo[i].sMAPID.wMainID,
                pNetMsg->sInfo[i].sMAPID.wSubID,
                pNetMsg->sInfo[i].nCHANNEL));

        }
        else
        {
            PrintConsoleText(
                sc::string::format(
                "[%3d] [%3d] [%d] [%9d] [%9d]",
                pNetMsg->sInfo[i].sMAPID.wMainID,
                pNetMsg->sInfo[i].sMAPID.wSubID,
                pNetMsg->sInfo[i].nCHANNEL,
                (DWORD)(pNetMsg->sInfo[i].dwAPPLYTIME/60000),
                (DWORD)(pNetMsg->sInfo[i].dwREMAINEDTIME/60000)));
        }
    }
}

void GLGaeaClient::MsgGmEventGradeFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_EVENT_GRADE_FB *pNetMsg = (GLMSG::SNET_GM_EVENT_GRADE_FB *) pMsg;

    float fSCALE = pNetMsg->fRATE;            
    PrintConsoleText(
        sc::string::format("Grade Event Begin, scale : %3.4f", fSCALE));
}


void GLGaeaClient::MsgGmEventItemGenEndFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_EVEN_ITEM_GEN_END_FB *pNetMsg = (GLMSG::SNET_GM_EVEN_ITEM_GEN_END_FB *) pMsg;

    PrintConsoleText ( "item Gen Event End" );
}


void GLGaeaClient::MsgGmEventGradeEndFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_EVENT_GRADE_END_FB *pNetMsg = (GLMSG::SNET_GM_EVENT_GRADE_END_FB *) pMsg;

    PrintConsoleText ( "Grade Event End" );
}


void GLGaeaClient::MsgGmBigHeadBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_BIGHEAD *pNetMsg = (GLMSG::SNET_GM_BIGHEAD *) pMsg;
    DxSkinAniControl::m_bBIGHEAD = pNetMsg->bBIGHEAD;
	DxSkinAniControl::m_fBIGHEAD = pNetMsg->fSize;
}

void GLGaeaClient::MsgGmBigHandBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_BIGHAND *pNetMsg = (GLMSG::SNET_GM_BIGHAND *) pMsg;
    DxSkinAniControl::m_bBIGHAND = pNetMsg->bBIGHAND;
	DxSkinAniControl::m_fBIGHAND = pNetMsg->fSize;
}


void GLGaeaClient::MsgGmFreePkBrd(NET_MSG_GENERIC* pMsg)
{
    // 클라이언트가 뭔가 할일이 있다면 여기에 코딩
    GLMSG::SNET_GM_FREEPK_BRD *pNetMsg = (GLMSG::SNET_GM_FREEPK_BRD *) pMsg;
    if ( pNetMsg->bSTATEUPDATE == false )
    {
        if ( pNetMsg->dwPKTIME != 0 )
        {
            m_bBRIGHTEVENT = true;
            PrintConsoleText ( ID2GAMEINTEXT("BRIGHT_EVENT_START") );
        }
        else
        {
            m_bBRIGHTEVENT = false;
            PrintConsoleText ( ID2GAMEINTEXT("BRIGHT_EVENT_END") );

            // 적대관계 모두 삭제
            GetCharacter()->DEL_PLAYHOSTILE_ALL();
        }
    }
}


void GLGaeaClient::MsgGmShopInfoFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_SHOP_INFO_FB *pNetMsg = (GLMSG::SNET_GM_SHOP_INFO_FB *)pMsg;

    if (pNetMsg->bSTATE == GLMSG::SNET_GM_SHOP_INFO_FB::END)
    {   
        PrintConsoleText(
            sc::string::format(
            "Shop Info Search End! [%s][%s]",
            m_szShopInfoFile,
            pNetMsg->szBasicInfo));
        return;
    }

    if( pNetMsg->bSTATE == GLMSG::SNET_GM_SHOP_INFO_FB::FIRST )
    {
        PrintConsoleText("Shop Info Search Start!");

        TCHAR szPROFILE[MAX_PATH] = {0};
        memset(m_szShopInfoFile, 0, sizeof(char) * (MAX_PATH));
        SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

        StringCchCopy(m_szShopInfoFile, MAX_PATH, szPROFILE);
        StringCchCat (m_szShopInfoFile, MAX_PATH, m_pPath->SaveRoot().c_str()); //SUBPATH::SAVE_ROOT );
        CreateDirectory( m_szShopInfoFile, NULL );

        const CTime& curTime = GetCurrentTime();

        CHAR  szFileName[MAX_PATH] = {0};
        sprintf_s( szFileName, MAX_PATH, "ShopInfo_[%d%d%d%d%d%d].csv", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 
            curTime.GetHour(), curTime.GetMinute(), curTime.GetSecond() );

        StringCchCat ( m_szShopInfoFile, MAX_PATH, szFileName );
    }

    FILE* fp = NULL;
    fp = _fsopen( m_szShopInfoFile, "a+", _SH_DENYNO );
    if( fp )
    {
        if( pNetMsg->bSTATE == GLMSG::SNET_GM_SHOP_INFO_FB::FIRST )
        {
            if( pNetMsg->bBasicInfo )
            {
                char szHeader[] = "UserNum,CharNum,ItemMid,ItemSid,Price\n";
                fprintf( fp, szHeader );
            }else{
                char szHeader[] = "UserNum,CharNum,ItemMid,ItemSid,Price,RanOptType1,RandValue1,RanOptType2,RandValue2,RanOptType3,RandValue3,RanOptType4,RandValue4,DAMAGE,DEFENSE,RESIST_FIRE,RESIST_ICE,RESIST_ELEC,RESIST_POISON,RESIST_SPIRIT\n";
                fprintf( fp, szHeader );																			  
            }																										  
        }																											  
        fprintf( fp, pNetMsg->szBasicInfo );																		  
        fclose( fp );
    }
}


void GLGaeaClient::MsgGmViewAllPlayerFldFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB *pNetMsg = (GLMSG::SNET_GM_VIEWALLPLAYER_FLD_FB *)pMsg;

    if (pNetMsg->dwPlayerNum != 0)
    {				
        PrintConsoleText(
            sc::string::format(
            "Player Count : %d",
            pNetMsg->dwPlayerNum + 1));
    }
    else
    {                
        PrintConsoleText(
            sc::string::format(
            "%s : charid = %d",
            pNetMsg->szCHARNAME,
            pNetMsg->dwCHARID));
    }
}


void GLGaeaClient::MsgGmViewWorkEventFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_GM_VIEWWORKEVENT_FB *pNetMsg = (GLMSG::SNET_GM_VIEWWORKEVENT_FB *)pMsg;
    PrintMsgText(
        NS_UITEXTCOLOR::NEGATIVE,
        sc::string::format(
        "%s",
        pNetMsg->szWorkEventInfo));
}

void GLGaeaClient::LogItemGmReload()
{
    GLMSG::NET_GM_LOG_ITEM_RELOAD_CA Msg;
    NETSENDTOAGENT(&Msg);
}

void GLGaeaClient::ServerGameDataUpdate( int nType )
{
    GLMSG::NET_GM_GAME_DATA_UPDATE_CA Msg;
	Msg.nDataType = nType;
    NETSENDTOAGENT(&Msg);
}

void GLGaeaClient::turnOnDamageRecord(const STARGETID& ActorID)
{
	m_pDamageRecord->turnOn();
	addActorDamageRecord(ActorID);
}

void GLGaeaClient::logDamageRecord(const STARGETID& ActorID, const DWORD nDamage, const SNATIVEID& SkillID /* = SNATIVEID(false) */, const WORD wSkillLevel /* = 0 */, const DWORD dwDamageFlag /* = 0 */)
{
	if ( m_pDamageRecord->isOn() == false )
		return;

	ClientActor* const pActor = GetCopyActor(ActorID);
	if ( pActor == NULL )
		return;

	bool bUpdate = false;
	if ( ActorID.emCrow == CROW_PC )
	{
		bUpdate = m_pDamageRecord->doDamage( pActor->GetCharID(), pActor->GetName(),
			nDamage, SkillID, wSkillLevel, dwDamageFlag);
	}
	else
	{
		const SNATIVEID& CrowID = pActor->GetCrowID();
		bUpdate = m_pDamageRecord->doDamage( CrowID.Mid(), CrowID.Sid(), pActor->GetName(),
			nDamage, SkillID, wSkillLevel, dwDamageFlag);
	}

	if( true == bUpdate )
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_DPS_Update );
}

void GLGaeaClient::addActorDamageRecord(const STARGETID& ActorID)
{
	if ( m_pDamageRecord->isOn() == false )
		return;

	ClientActor* const pActor = GetCopyActor(ActorID);
	if ( pActor == NULL )
		return;

	switch ( ActorID.emCrow )
	{
	case CROW_PC:
		m_pDamageRecord->addActor(pActor->GetCharID(), pActor->GetName());
		break;
	case CROW_MOB:
	case CROW_SUMMON:
		const SNATIVEID& ActorID = pActor->GetCrowID();
		m_pDamageRecord->addActor(ActorID.Mid(), ActorID.Sid(), pActor->GetName());
		break;
	}
}
void GLGaeaClient::dropActorDamageRecord(const DWORD nIndex)
{
	if ( m_pDamageRecord->isOn() == false )
		return;

	m_pDamageRecord->dropActorByIndex(nIndex);
}

void GLGaeaClient::resetActorDamageRecord(const DWORD nIndex)
{
	if ( m_pDamageRecord->isOn() == false )
		return;

	m_pDamageRecord->resetActorByIndex(nIndex);
}

void GLGaeaClient::saveCSVActorDamageRecord(void)
{
	if ( m_pDamageRecord->isOn() == false )
		return;

	m_pDamageRecord->saveCSV();
}

void GLGaeaClient::ReqSetNotifyMoney( const LONGLONG& amount )
{
	if ( amount < 0 )
	{
		return;
	}

	GLMSG::NET_GM_NOTIFY_MONEY_CAF Msg;
	Msg.money = amount;
	NETSENDTOAGENT( &Msg );
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, 등록 요청에 대한 응답
 * modified : 
 */
void GLGaeaClient::MsgGmSetRandomboxChanceReqAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC *pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC *)pMsg;

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_SUCESS)
    {   
        PrintConsoleText("SUCCESS");

        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_FAIL)
    {   
        PrintConsoleText("FAIL");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REQ_FLAG_GMGRADE_ERR)
    {   
        PrintConsoleText("GM_GRADE_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_BOXID_ERR)
    {   
        PrintConsoleText("BOXID_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_ITEM_ERR)
    {   
        PrintConsoleText("ITEM_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_COUNT_ERR)
    {   
        PrintConsoleText("COUNT_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_CHANUM_ERR)
    {   
        PrintConsoleText("CHANUM_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC::REP_FLAG_DUPLICATE_ERR)
    {   
        PrintConsoleText("DATA_DUPLICATE_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 조회 요청에 대한 응답
 * modified : 
 */
void GLGaeaClient::MsgGmGetRandomboxChanceListAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC *pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC *)pMsg;

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_SUCESS)
    {   
        //PrintConsoleText("SUCCESS");
        PrintConsoleText(sc::string::format("RCCD [%d][%d/%d][%d/%d][%d][%d][%d][%d]", 
                                            pNetMsg->nIndex, 
                                            pNetMsg->sBoxID.wMainID,
                                            pNetMsg->sBoxID.wSubID,
                                            pNetMsg->sItemID.wMainID,
                                            pNetMsg->sItemID.wSubID,
                                            pNetMsg->nReqCount,
                                            pNetMsg->dwTargetChaNum,
                                            pNetMsg->dwGMNum,
                                            pNetMsg->nOpenCount
                                           ));

        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_FAIL)
    {   
        PrintConsoleText("FAIL");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REQ_FLAG_GMGRADE_ERR)
    {   
        PrintConsoleText("GM_GRADE_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC::REP_FLAG_NOTFOUND_ERR)
    {   
        PrintConsoleText("DATA NOT FOUND ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.17, 삭제 요청에 대한 응답
 * modified : 
 */
void GLGaeaClient::MsgGmDelRandomboxChanceAC(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC *pNetMsg = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC *)pMsg;

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_SUCESS)
    {   
        PrintConsoleText("SUCCESS");

        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_FAIL)
    {   
        PrintConsoleText("FAIL");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REQ_FLAG_GMGRADE_ERR)
    {   
        PrintConsoleText("GM_GRADE_ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }

    if (pNetMsg->wRepFlag == GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC::REP_FLAG_NOTFOUND_ERR)
    {   
        PrintConsoleText("DATA NOT FOUND ERROR");
        //PrintConsoleText(sc::string::format("Shop Info Search End! [%s][%s]", m_szShopInfoFile, pNetMsg->szBasicInfo));
        return;
    }
}