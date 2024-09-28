#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../Enginelib/DxTools/DxClubMan.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/*
void GLCharacter::MsgProcessClub( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
	//case NET_MSG_GCTRL_CLUB_INFO_2CLT:
    //    MsgClubInfo(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_DEL_2CLT:
    //    MsgClubDel();
	//	break;

	//case NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION:
    //    MsgClubInfoDissolution(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
    //    MsgClubMember(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_NEW_FB:
    //    MsgClubNewFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK:
    //    MsgClubMemberReqAsk(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB:
    //    MsgClubMemberReqFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT:
    //    MsgClubMarkChange(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_RANK_2CLT:
    //    MsgClubRank(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_RANK_FB:
    //    MsgClubRankFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB:
    //    MsgClubMemberKickFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_STATE:
    //    MsgClubMemberState(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_POS:
    //    MsgClubMemberPos(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_DISSOLUTION_FB:
    //    MsgClubDissolutionFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT:
    //    MsgClubMemberDel(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB:
    //    MsgClubMemberDelFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB:
    //    MsgClubMemberSecedeFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK:
    //    MsgClubAuthorityReqAsk();
	//	break;

	//case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB:
    //    MsgClubAuthorityReqFB(nmg);
	//	break;
	
	//case NET_MSG_GCTRL_CLUB_AUTHORITY_CLT:
    //    MsgClubAutority(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB:
    //    MsgClubCD_CertifyFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_COMMISSION_FB:
    //    MsgClubCommissionFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_RESET:
    //    MsgClubStorageReset(nmg);
	//	break;

    //case NET_MSG_CLUB_STORAGE_SEND_END:
    //    MsgClubStorageSendEnd(nmg);
    //    break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY:
    //    MsgClubStorageUpdateMoney(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM:
    //    MsgClubGetStorageItem(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_INSERT:
    //    MsgClubStorageInsert(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_DELETE:
    //    MsgClubStorageDelete(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS:
    //    MsgClubStorageDelIns(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM:
    //    MsgClubStorageUpdateItem(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_NOTICE_FB:
    //    MsgClubNoticeFB(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_NOTICE_CLT:
    //    MsgClubNotice(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT:
    //    MsgClubMemberRename(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_SUBMASTER_FB:
    //    MsgClubSubMasterFb(nmg);
	//	break;

	//case NET_MSG_GCTRL_CLUB_SUBMASTER_BRD:
    //    MsgClubSubMasterBrd(nmg);
	//	break;

	default:
		CDebugSet::ToListView ( "GLCharacter::MsgProcess unknown message type %d", nmg->nType );
		break;
	};
}
*/
void GLCharacter::MsgUpdateCdmSafeTimeBrd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD* pNetMsg = ( GLMSG::SNETPC_UPDATE_CDM_SAFETIME_BRD* ) pMsg;
    m_fCDMSafeTime = pNetMsg->fCDMSafeTime;
	//!! SF_TODO
    //m_pInterface->VisibleCDMSafeTime( true, pNetMsg->fCDMSafeTime );
}
/*
void GLCharacter::MsgClubInfo( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_INFO_2CLT *pNetMsg = (GLMSG::SNET_CLUB_INFO_2CLT *) nmg;

    m_ClubDbNum = pNetMsg->dwClubID;
    m_pMyClub->m_DbNum = pNetMsg->dwClubID;
    m_pMyClub->SetName(pNetMsg->szClubName);

    m_pMyClub->SetMasterCharDbNum(pNetMsg->dwMasterID);
    m_pMyClub->SetMasterName(pNetMsg->szMasterName);
    m_pMyClub->m_dwCDCertifior = pNetMsg->dwCDCertifior;

    m_pMyClub->m_dwMarkVER = pNetMsg->dwMarkVER;
    m_pMyClub->m_dwRank = pNetMsg->dwRank;

    m_pMyClub->m_tOrganize = pNetMsg->tOrganize;
    m_pMyClub->m_tDissolution = pNetMsg->tDissolution;

    m_pMyClub->SetNotice(pNetMsg->szNotice);

    StringCchCopy ( m_szNick, CHAR_SZNAME, pNetMsg->szNickName );

    //	Note : 클럽마크 버전 확인.
    //
    ReqClubMarkInfo ( m_pMyClub->DbNum(), m_pMyClub->m_dwMarkVER );

    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubDel() 
{
    m_ClubDbNum = CLUB_NULL;
    m_pMyClub->RESET();

    m_pInterface->REFRESH_CLUB_STATE ();
    m_pInterface->REFRESH_CLUB_LIST ();
}
*/
/*
void GLCharacter::MsgClubInfoDissolution( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_INFO_DISSOLUTION *pNetMsg = (GLMSG::SNET_CLUB_INFO_DISSOLUTION *) nmg;

    m_pMyClub->m_tDissolution = pNetMsg->tDissolution;
    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubMember( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_2CLT *) nmg;

    for ( DWORD i=0; i<pNetMsg->dwMemberNum; ++i )
    {
        GLCLUBMEMBER cMEMBER(
            pNetMsg->sMEMBER[i].dwID,
            pNetMsg->sMEMBER[i].szMember,
            pNetMsg->sMEMBER[i].dwFlag,
            pNetMsg->sMEMBER[i].m_ChaLevel);
        m_pMyClub->SetMember(cMEMBER);
    }

    m_pInterface->REFRESH_CLUB_LIST ();
}
*/
/*
void GLCharacter::MsgClubNewFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::NET_CLUB_NEW_FB_FC *pNetMsg = (GLMSG::NET_CLUB_NEW_FB_FC *) nmg;

    std::string strText;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_NEW_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_FAIL") );
        break;

    case EMCLUB_NEW_FB_OK:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_OK"), pNetMsg->szClubName );
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, strText );
        break;

    case EMCLUB_NEW_FB_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_ALREADY") );
        break;

    case EMCLUB_NEW_FB_NOTMASTER:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMASTER") );
        break;

    case EMCLUB_NEW_FB_NOTMEMBER:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMEMBER"), GLCONST_CHAR::dwCLUB_PARTYNUM );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );				
        break;

    case EMCLUB_NEW_FB_NOTEXIST_MEM:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTEXIST_MEM"), GLCONST_CHAR::dwCLUB_PARTYNUM );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_MEM_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_MEM_ALREADY") );
        break;

    case EMCLUB_NEW_FB_MASTER_LVL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_MASTER_LVL"), GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_AVER_LIVING:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_AVER_LIVING"), GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_NOTMONEY:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMONEY"), GLogicData::GetInstance().GetClubRankData(0).m_dwPay );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_NAME:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_NAME"), pNetMsg->szClubName );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_BADNAME:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_NEW_FB_BADNAME"), pNetMsg->szClubName );
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_NEW_FB_OTHERSCHOOL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_OTHERSCHOOL") );
        break;
        //#ifdef TH_PARAM
    case EMCLUB_NEW_FB_THAICHAR_ERROR:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_THAICHAR_ERROR") );
        break;
        //#endif
        //#ifdef VN_PARAM
    case EMCLUB_NEW_FB_VNCHAR_ERROR:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_VNCHAR_ERROR") );
        break;
        //#endif
    };
}
*/
/*
void GLCharacter::MsgClubMemberReqAsk(NET_MSG_GENERIC* nmg) 
{
    GLMSG::SNET_CLUB_MEMBER_REQ_ASK *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_REQ_ASK *) nmg;
    m_pInterface->DOMODAL_CLUB_JOIN_ASK(
        pNetMsg->dwMaster, pNetMsg->szClubName, pNetMsg->szMasterName);
}
*/
/*
void GLCharacter::MsgClubMemberReqFB(NET_MSG_GENERIC* nmg) 
{
    GLMSG::SNET_CLUB_MEMBER_REQ_FB* pPacket = (GLMSG::SNET_CLUB_MEMBER_REQ_FB *) nmg;

    std::string strText;

    switch (pPacket->emFB)
    {
    case EMCLUB_MEMBER_REQ_FB_FAIL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_FAIL"), pPacket->szReqName ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_OK:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OK"), pPacket->szReqName ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_REFUSE:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_REFUSE"), pPacket->szReqName ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_ALREADY:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_ALREADY"), pPacket->szReqName ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_MAXNUM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_MAXNUM") );
        break;

    case EMCLUB_MEMBER_REQ_FB_SECEDED:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_SECEDED"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_OTHERSCHOOL"), pPacket->szReqName ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;

    case EMCLUB_MEMBER_REQ_FB_CLUBBATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_CLUBBATTLE") );
        break;

    case EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY:
        m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_NOT_SAME_COUNTRY"));
        break;

    default:
        m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_REQ_FB_FAIL"));
        break;
    };
}
*/
/*
void GLCharacter::MsgClubSubMasterBrd( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_SUBMASTER_BRD *pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_BRD *)nmg;
    m_pMyClub->SetMemberFlag ( pNetMsg->dwCharID, pNetMsg->dwFlags );
    m_pMyClub->m_dwCDCertifior = pNetMsg->dwCDCertifior;

    m_pInterface->REFRESH_CLUB_LIST ();
    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubSubMasterFb( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_SUBMASTER_FB *pNetMsg = (GLMSG::SNET_CLUB_SUBMASTER_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUBSUBMASTER_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_FAIL") );
        }
        break;
    case EMCLUBSUBMASTER_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_OK") );
        }
        break;

    case EMCLUBSUBMASTER_FB_RESET:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_RESET") );
        }
        break;

    case EMCLUBSUBMASTER_FB_NOTMASTER:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_NOTMASTER") );
        }
        break;
    case EMCLUBSUBMASTER_FB_BATTLE:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_BATTLE") );
        }
        break;
    case EMCLUBSUBMASTER_FB_CDM:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_CDM") );
        }
        break;
    case EMCLUBSUBMASTER_FB_CDM_MEM:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUBSUBMASTER_FB_CDM_MEM"), GLCONST_CHAR::dwCLUB_DEATHMATCH_MEM ).c_str();
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    };
}
*/
/*
void GLCharacter::MsgClubMemberRename( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_RENAME_CLT *)nmg;

    GLCLUBMEMBER* pCLUBMember = m_pMyClub->GetMember( pNetMsg->dwCharID );
    GASSERT ( pCLUBMember );
    if ( pCLUBMember )
    {
        pCLUBMember->SetName(pNetMsg->szName);
    }
    if ( m_pMyClub->IsMaster ( pNetMsg->dwCharID ) )
    {
        m_pMyClub->SetMasterName(pNetMsg->szName);
    }

    // 인터페이스 갱신
    m_pInterface->REFRESH_CLUB_LIST ();
    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubNotice(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_NOTICE_CLT *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_CLT *)nmg;
    m_pMyClub->SetNotice(pNetMsg->szNotice);

    //	Note : 채팅창에 공지 출력.
    m_pInterface->PrintMsgDlg ( NS_UITEXTCOLOR::GUILD, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK") );			
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::GUILD, sc::string::format( ID2GAMEINTEXT("CLUB_NOTICE"), m_pMyClub->Name() ) );
    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::GUILD, sc::string::format("%s", m_pMyClub->m_szNotice) );

    //	Note : 클럽창 갱신.
    m_pInterface->REFRESH_CLUB_NOTICE();
}
*/
/*
void GLCharacter::MsgClubNoticeFB(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_NOTICE_FB *pNetMsg = (GLMSG::SNET_CLUB_NOTICE_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_NOTICE_FB_FAIL:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_FAIL") );
        }
        break;
    case EMCLUB_NOTICE_FB_OK:
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_OK") );
        }
        break;
    case EMCLUB_NOTICE_FB_NOTMATER:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NOTICE_FB_NOTMATER") );
        }
        break;
    };
}
*/
/*
void GLCharacter::MsgClubStorageUpdateItem( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_ITEM *)nmg;

    DWORD dwChannel = pNetMsg->dwChannel;
    SINVENITEM* pInvenItem = m_pMyClub->GetItem(dwChannel, pNetMsg->wPosX, pNetMsg->wPosY);
    if (pInvenItem)
        pInvenItem->sItemCustom.wTurnNum = pNetMsg->wTurnNum;
}
*/
/*
void GLCharacter::MsgClubStorageDelIns( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DEL_AND_INS *)nmg;

    DWORD dwChannel = pNetMsg->dwChannel;
    
    // PET
    // 팻카드 정보를 제거해준다.
    //SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem ( pNetMsg->wDelX, pNetMsg->wDelX );
    //if ( pInvenItem )
    //{
    //    PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    //    if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    //}
    m_pMyClub->DeleteItem(dwChannel, pNetMsg->wDelX, pNetMsg->wDelY);
    m_pMyClub->InsertItem(pNetMsg->sInsert.sItemCustom, dwChannel, pNetMsg->sInsert.wPosX, pNetMsg->sInsert.wPosY);

    // PET
    // 팻카드 정보를 요청한다.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sInsert.sItemCustom.sNativeID );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pNetMsg->sInsert.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(pNetMsg->sInsert.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
}
*/
/*
void GLCharacter::MsgClubStorageDelete( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_DELETE *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_DELETE *)nmg;

    const DWORD dwChannel = pNetMsg->dwChannel;    
    // PET
    // 팻카드 정보를 제거해준다.
    //SINVENITEM* pInvenItem = m_sCLUB.m_cStorage[dwChannel].GetItem( pNetMsg->wPosX, pNetMsg->wPosY );
    //if ( pInvenItem )
    //{
    //PETCARDINFO_MAP_ITER iter = m_mapPETCardInfo.find ( pInvenItem->sItemCustom.dwPetID );
    //if ( iter != m_mapPETCardInfo.end() ) m_mapPETCardInfo.erase ( iter );
    //}    
    m_pMyClub->DeleteItem(dwChannel, pNetMsg->wPosX, pNetMsg->wPosY);
}
*/
/*
void GLCharacter::MsgClubStorageInsert( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_INSERT *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_INSERT *)nmg;

    const SINVENITEM &sInvenItem = pNetMsg->Data;
    const DWORD dwChannel = pNetMsg->dwChannel;

    m_pMyClub->InsertItem(sInvenItem.sItemCustom, dwChannel, sInvenItem.wPosX, sInvenItem.wPosY);

    // PET
    // 팻카드 정보를 요청한다.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( sInvenItem.sItemCustom.sNativeID );
    if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && sInvenItem.sItemCustom.PetDbNum() != 0 )
    {
        GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(sInvenItem.sItemCustom.PetDbNum());
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
}
*/
/*
void GLCharacter::MsgClubGetStorageItem( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_INSERT* pNetMsg = (GLMSG::SNET_CLUB_STORAGE_INSERT*) nmg;
    m_pMyClub->InsertItem(pNetMsg->Data.sItemCustom, pNetMsg->dwChannel, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY);
}
*/
/*
void GLCharacter::MsgClubStorageUpdateMoney( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY* pNetMsg = (GLMSG::SNET_CLUB_STORAGE_UPDATE_MONEY*) nmg;
    m_pMyClub->SetStorageMoney(pNetMsg->lnMoney);
    m_pMyClub->SetIncomeMoney(pNetMsg->IncomeMoney);
}
*/
/*
void GLCharacter::MsgClubStorageSendEnd(NET_MSG_GENERIC* nmg) 
{
    GLMSG::NET_CLUB_STORAGE_SEND_END* pPacket = (GLMSG::NET_CLUB_STORAGE_SEND_END*) nmg;
    m_pMyClub->SetStorageState(true);
}
*/
/*
void GLCharacter::MsgClubStorageReset( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_STORAGE_RESET *pNetMsg = (GLMSG::SNET_CLUB_STORAGE_RESET *)nmg;
    m_pMyClub->RESET_STORAGE();
}
*/
/*
void GLCharacter::MsgClubCommissionFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_GUID_COMMISSION_FB *pNetMsg = (GLMSG::SNET_CLUB_GUID_COMMISSION_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMGUIDCOMMISSION_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_FAIL") );
        break;
    case EMGUIDCOMMISSION_FB_OK:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_OK"), pNetMsg->fCommission ).c_str();
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
        break;
    case EMGUIDCOMMISSION_FB_RANGE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_RANGE") );
        break;
    case EMGUIDCOMMISSION_FB_NOTCLUB:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_NOTCLUB") );
        break;
    case EMGUIDCOMMISSION_FB_ALREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_ALREADY") );
        break;
    case EMGUIDCOMMISSION_FB_BATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_BATTLE") );
        break;
    };
}
*/
/*
void GLCharacter::MsgClubCD_CertifyFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_CD_CERTIFY_FB *pNetMsg = (GLMSG::SNET_CLUB_CD_CERTIFY_FB *)nmg;

    switch ( pNetMsg->emFB )
    {
    case EMCDCERTIFY_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_FAIL") );
        break;
    case EMCDCERTIFY_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_OK") );

        m_bCLUB_CERTIFY = TRUE;
        m_fCLUB_CERTIFY_TIMER = 0.0f;
        m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( true );
        break;
    case EMCDCERTIFY_BADNPC:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_BADNPC") );
        break;
    case EMCDCERTIFY_NOTMASTER:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTMASTER") );
        break;
    case EMCDCERTIFY_AREADYCERTIFY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_AREADYCERTIFY") );
        break;
    case EMCDCERTIFY_HAVEGUIDCLUB:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_HAVEGUIDCLUB") );
        break;
    case EMCDCERTIFY_OTHERCERTIFING:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_OTHERCERTIFING") );
        break;
    case EMCDCERTIFY_AREADYGUID:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_AREADYGUID") );
        break;
    case EMCDCERTIFY_NOTTIME:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTTIME") );
        break;
    case EMCDCERTIFY_DISTANCE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_DISTANCE") );
        break;
    case EMCDCERTIFY_NOTITEM:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTITEM") );
        break;
    case EMCDCERTIFY_NOTCONDITION:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_NOTCONDITION") );
        break;

    case EMCDCERTIFY_AREADY:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_AREADY") );
        break;

    case EMCDCERTIFY_ING_TIMEOUT:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_TIMEOUT") );

        m_bCLUB_CERTIFY = FALSE;
        m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;
    case EMCDCERTIFY_ING_DISTANCE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_DISTANCE") );

        m_bCLUB_CERTIFY = FALSE;
        m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;
    case EMCDCERTIFY_ING_DIE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCDCERTIFY_ING_DIE") );

        m_bCLUB_CERTIFY = FALSE;
        m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;

    case EMCDCERTIFY_COMPLETE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCDCERTIFY_COMPLETE") );

        m_bCLUB_CERTIFY = FALSE;
        m_pInterface->SET_CONFT_CONFIRM_VISIBLE ( false );
        break;
    };
}
*/
/*
void GLCharacter::MsgClubAutority( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_AUTHORITY_CLT *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_CLT *)nmg;

    GLCLUBMEMBER* pMember = m_pMyClub->GetMember( pNetMsg->dwCharID );
    if (!pMember)
        return;

    m_pMyClub->SetMasterCharDbNum(pNetMsg->dwCharID);
    m_pMyClub->SetMasterName(pMember->m_szName);

    m_pInterface->REFRESH_CLUB_STATE ();
    m_pInterface->REFRESH_CLUB_LIST ();

    std::string strText = sc::string::format(ID2GAMEINTEXT("EMCLUB_AUTHORITY_CLT"), pMember->m_szName);
    m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::PALEGREEN, strText);
}
*/
/*
void GLCharacter::MsgClubAuthorityReqFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_AUTHORITY_REQ_FB *pNetMsg = (GLMSG::SNET_CLUB_AUTHORITY_REQ_FB *)nmg;

    GLCLUBMEMBER* pMember = m_pMyClub->GetMember(pNetMsg->dwCharID);
    if (!pMember)
        return;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_AUTHORITY_REQ_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_FAIL") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_OK:
        {
            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_OK"), pMember->m_szName, m_pMyClub->Name() ).c_str();
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::PALEGREEN, strText );
        }
        break;
    case EMCLUB_AUTHORITY_REQ_FB_REFUSE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_REFUSE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_NOMASTER:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOMASTER") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_NOONLINE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_NOONLINE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_ALLIANCE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_ALLIANCE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CLUBBATTLE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CONFING:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CONFING") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_TIME:
        {
            CTime cTime(pNetMsg->tAuthority);

            //	서버 타임으로 변환( TimeZone 계산 )
            m_pGaeaClient->GetConvertServerTime( cTime );

            CString strAuthorityDate;

            strAuthorityDate = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
                cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() ).c_str();

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_TIME"), strAuthorityDate ).c_str();
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        }
        break;
    case EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_GUIDNBATTLE") );
        break;
    case EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_AUTHORITY_REQ_FB_CDMBATTLE") );
        break;
    }
}
*/
/*
void GLCharacter::MsgClubAuthorityReqAsk() 
{
    m_pInterface->DOMODAL_CLUB_AUTHORITY_ASK(m_pMyClub->Name());
}
*/
/*
void GLCharacter::MsgClubMemberSecedeFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_SECEDE_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_SECEDE_FB *) nmg;
    switch ( pNetMsg->emFB )
    {
    case EMCLUB_MEMBER_SECEDE_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_FAIL") );
        break;
    case EMCLUB_MEMBER_SECEDE_FB_OK:
        {
            m_ClubDbNum = CLUB_NULL;
            m_pMyClub->RESET();

            m_pInterface->REFRESH_CLUB_STATE ();
            m_pInterface->REFRESH_CLUB_LIST ();

            std::string strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_MEMBER_SECEDE_FB_OK"), GLCONST_CHAR::dwCLUB_JOINBLOCK_DAY ).c_str();
            m_pGaeaClient->PrintConsoleTextDlg ( strText );
        }
        break;
    };
}
*/
/*
void GLCharacter::MsgClubMemberDelFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_DEL_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_DEL_FB *) nmg;
    switch ( pNetMsg->emFB )
    {
    case EMCLUB_MEMBER_DEL_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_FAIL") );
        break;
    case EMCLUB_MEMBER_DEL_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_DEL_FB_OK") );
        break;
    };
}
*/
/*
void GLCharacter::MsgClubMemberDel(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNET_CLUB_MEMBER_DEL_2CLT* pNetMsg = (GLMSG::SNET_CLUB_MEMBER_DEL_2CLT*) nmg;
    if (m_pMyClub->DbNum() == pNetMsg->dwClub)
        m_pMyClub->DelMember(pNetMsg->dwMember);
    m_pInterface->REFRESH_CLUB_LIST();
}
*/
/*
void GLCharacter::MsgClubDissolutionFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_DISSOLUTION_FB *pNetMsg = (GLMSG::SNET_CLUB_DISSOLUTION_FB *) nmg;

    std::string strText; 

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_DIS_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_FAIL") );
        break;
    case EMCLUB_DIS_FB_OK:
        m_pMyClub->DoDissolution ( pNetMsg->tDissolution );
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_DIS_FB_OK"), GLCONST_CHAR::dwCLUB_DISSOLUTION_DAY ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );		
        break;
    case EMCLUB_DIS_FB_CANCEL:
        m_pMyClub->DoDissolutionCancel ();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_CANCEL") );
        break;
    case EMCLUB_DIS_FB_CLUBBATTLE:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_DIS_FB_CLUBBATTLE") );
        break;
    };

    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubMemberPos( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_POS *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_POS *) nmg;

    for ( DWORD i=0; i<pNetMsg->dwNumber; ++i )
    {
        GLMSG::SMEMPOS &sPOS = pNetMsg->sPOS[i];
        m_pMyClub->SetMemberPos ( sPOS.dwCHARID, D3DXVECTOR3(sPOS.vecPOS.x,0,sPOS.vecPOS.y) );
    }

    m_pInterface->REFRESH_CLUB_LIST ();
}
*/
/*
void GLCharacter::MsgClubMarkChange( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *pNetMsg = (GLMSG::SNET_CLUB_MARK_CHANGE_2CLT *) nmg;

    //	Note : 클럽 마크 변경.
    m_pMyClub->m_dwMarkVER = pNetMsg->dwMarkVER;
    memcpy ( m_pMyClub->m_aryMark, pNetMsg->aryMark, sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY );

    //	Note : 이미지에 적용.
    DWORD dwServerID = m_pGaeaClient->GetCharacter()->m_dwServerID;
    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
    DxClubMan::GetInstance().SetClubData(
        pGlobalStage->GetD3dDevice(),
        dwServerID,
        m_ClubDbNum,
        pNetMsg->dwMarkVER,
        m_pMyClub->m_aryMark);
    m_pInterface->REFRESH_CLUB_STATE();
}
*/
/*
void GLCharacter::MsgClubRank( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_RANK_2CLT *pNetMsg = (GLMSG::SNET_CLUB_RANK_2CLT *) nmg;

    m_pMyClub->m_dwRank = pNetMsg->dwRank;

    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubRankFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_RANK_FB *pNetMsg = (GLMSG::SNET_CLUB_RANK_FB *) nmg;

    std::string strText;

    switch ( pNetMsg->emFB )
    {
    case EMCLUB_RANK_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_FAIL") );
        break;
    case EMCLUB_RANK_FB_OK:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_OK") );
        break;
    case EMCLUB_RANK_FB_MAX:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_RANK_FB_MAX") );
        break;
    case EMCLUB_RANK_FB_MASTER_LVL:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_RANK_FB_MASTER_LVL"), pNetMsg->dwValue ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;
    case EMCLUB_RANK_FB_AVER_LIVING:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_RANK_FB_AVER_LIVING"), pNetMsg->dwValue ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;
    case EMCLUB_RANK_FB_NOTMONEY:
        strText = sc::string::format( ID2GAMEINTEXT("EMCLUB_RANK_FB_NOTMONEY"), pNetMsg->dwValue ).c_str();
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, strText );
        break;
    };
}
*/
/*
void GLCharacter::MsgClubMemberKickFB( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_NICK_FB *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_NICK_FB *) nmg;
    switch ( pNetMsg->emFB )
    {
    case EMCLUB_MEMBER_NICK_FB_FAIL:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_FAIL") );
        break;

    case EMCLUB_MEMBER_NICK_FB_OK:
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_OK") );
            StringCchCopy ( m_szNick, CHAR_SZNAME, pNetMsg->szNick );
        }
        break;

    case EMCLUB_MEMBER_NICK_FB_BADNAME:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_BADNAME") );
        break;

        //#ifdef TH_PARAM
    case EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR:
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_THAICHAR_ERROR") );
        break;
        //#endif

        //#ifdef VN_PARAM
    case EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR:
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_MEMBER_NICK_FB_VNCHAR_ERROR"));
        break;
        //#endif
    };

    m_pInterface->REFRESH_CLUB_STATE ();
}
*/
/*
void GLCharacter::MsgClubMemberState( NET_MSG_GENERIC* nmg ) 
{
    GLMSG::SNET_CLUB_MEMBER_STATE *pNetMsg = (GLMSG::SNET_CLUB_MEMBER_STATE *) nmg;

    for ( DWORD i=0; i<pNetMsg->dwNumber; ++i )
    {
        GLMSG::SMEMSTATE &sSTATE = pNetMsg->sSTATE[i];
        m_pMyClub->SetMemberState ( sSTATE.dwCHARID, sSTATE.bONLINE, sSTATE.nCHANNEL, sSTATE.nidMAP );
    }

    m_pInterface->REFRESH_CLUB_LIST ();
}
*/
