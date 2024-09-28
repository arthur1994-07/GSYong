#include "pch.h"
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/GUInterface/UITextControl.h"
#include "../EngineLib/GUInterface/GameTextControl.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./Land/GLLandManClient.h"
#include "./Pet/GLPetClient.h"
#include "./Pet/GLAnyPet.h"
#include "./Stage/DxGlobalStage.h"
#include "./Effect/GLFactEffect.h"
#include "./GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void GLGaeaClient::MyPetMsgProcess(NET_MSG_GENERIC* pMsg)
{
    if ( m_pPet->IsVALID () )
    {
        m_pPet->MsgProcess (pMsg);
    }
}

void GLGaeaClient::PetMsgProcess(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_BROAD *pNetMsg = (GLMSG::SNETPET_BROAD *) pMsg;

    std::tr1::shared_ptr<GLAnyPet> pPet = m_pLandMClient->GetPet ( pNetMsg->dwGUID );
    if (!pPet)
        CDebugSet::ToListView ( "[PGLCHARCLIENT NULL] ���� PET�� ���� �޼��� �߻�. gaeaid %d", pNetMsg->dwGUID );
    else
        pPet->MsgProcess(pMsg);
}

void GLGaeaClient::MsgPetReqUseCardFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_USEPETCARD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_USEPETCARD_FB* ) pMsg;
    m_pPet->SetReady( false );

    switch ( pNetMsg->emFB )
    {
    case EMPET_USECARD_FB_OK:
        {
            // ���ʻ����� ���� ������������ ����ؼ� Ŭ���̾�Ʈ�� �����ۿ� �ֹ�ȣ ����
            SINVENITEM* pItem = m_pCharacter->m_cInventory.GetItem ( m_pCharacter->m_wInvenPosX1, m_pCharacter->m_wInvenPosY1 );
            if ( !pItem )
                return;
            if ( pItem->sItemCustom.PetDbNum() == 0 )
			{
                pItem->sItemCustom.PetDbNumSet(pNetMsg->m_dwPetID);

				GLWidgetScript::GetInstance().LuaCallFunc< void >(
					NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
					"-n, -n",  m_pCharacter->m_wInvenPosX1, m_pCharacter->m_wInvenPosY1);
			}

            m_pPet->m_sPetSkinPackState.bUsePetSkinPack = pNetMsg->m_sPetSkinPackData.bUsePetSkinPack;
            m_pPet->m_sPetSkinPackState.petSkinMobID	  = pNetMsg->m_sPetSkinPackData.sMobID;
            m_pPet->m_sPetSkinPackState.fPetScale		  = pNetMsg->m_sPetSkinPackData.fScale;
            // ����
            if ( !FAILED( CreatePET ( pMsg ) ) )
            {						
                PrintMsgText(
                    NS_UITEXTCOLOR::PALEGREEN,
                    sc::string::format(
                    ID2GAMEINTEXT("EMPET_USECARD_FB_OK"),
                    m_pPet->GetName()));

                //! �� ������ �̹����� ������ �κ��� Ư�� ��ǥ�� ���� �Ǿ� �־ 
                //! ���������� �̹����� ǥ���Ҽ� �ֵ��� ����
                //m_pInterface->SetPetHungryItem( pItem->GetNativeID() );
				//!! SF_TODO
                //m_pInterface->SetPetHungryItem( m_pCharacter->m_sPetCardNativeID );
                //m_pInterface->SetPetHungry();
                //m_pInterface->UiShowGroupBottom( PET_HUNGRY );
            }

            if ( m_pPet->IsUsePetSkinPack() )
            {
                PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_OK"));
            }
        }
        break;
    case EMPET_USECARD_FB_FAIL:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_FAIL") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;
    case EMPET_USECARD_FB_INVALIDCARD:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_INVALIDCARD") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;
    case EMPET_USECARD_FB_NOITEM:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_NOITEM") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;
    case EMPET_USECARD_FB_NOTENOUGHFULL:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_NOTENOUGHFULL") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;

    case EMPET_USECARD_FB_INVALIDZONE:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_INVALIDZONE") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;

	case EMPET_USECARD_FB_ERR_ACTIVITY:
		PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ERR_ACTIVITY") );
        // ���н� GenNum ó��
		m_pCharacter->m_llPetCardGenNum.Reset();
        break;
    case EMPET_USECARD_FB_ERR_CONTRIBUTION:
        PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ERR_CONTRIBUTION") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;

    case EMPET_USECARD_FB_ACTIONLIMIT:
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ACTIONLIMIT") );
        // ���н� GenNum ó��
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;

    case EMPET_USECARD_FB_NODATA:

        // ��ī�� ���� ����
        SINVENITEM* pItem = m_pCharacter->m_cInventory.GetItem ( m_pCharacter->m_wInvenPosX1, m_pCharacter->m_wInvenPosY1 );
        if (!pItem)
            return;
        pItem->sItemCustom.PetDbNumSet(0);
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_NODATA") );
        m_pCharacter->m_llPetCardGenNum.Reset();
        break;
    }
}

void GLGaeaClient::MsgPetSkinPackOpenFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_SKINPACKOPEN_FB *pNetMsg = (GLMSG::SNETPET_SKINPACKOPEN_FB *)pMsg;

    if( pNetMsg->emFB == EMPET_PETSKINPACKOPEN_FB_FAIL )
    {
        PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_FAIL") );
        return;
    }
    if( pNetMsg->emFB == EMPET_PETSKINPACKOPEN_FB_PETCARD_FAIL )
    {
        PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPET_PETSKINPACKOPEN_FB_PETCARD_FAIL") );
        return;
    }

    m_pPet->m_sPetSkinPackState.Init();

    m_pPet->m_sPetSkinPackState.bUsePetSkinPack = pNetMsg->sPetSkinPackData.bUsePetSkinPack;
    m_pPet->m_sPetSkinPackState.petSkinMobID	  = pNetMsg->sPetSkinPackData.sMobID;
    m_pPet->m_sPetSkinPackState.fPetScale		  = pNetMsg->sPetSkinPackData.fScale;

    if( pNetMsg->emFB == EMPET_PETSKINPACKOPEN_FB_OK )
    {
        PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_OK") );				
    }
    if( pNetMsg->emFB == EMPET_PETSKINPACKOPEN_FB_END )
    {
        PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_END") );
    }

    PetSkinPackApplyEffect();
}

void GLGaeaClient::MsgPetReqUnUseCardFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_UNUSEPETCARD_FB *pNetMsg = ( GLMSG::SNETPET_REQ_UNUSEPETCARD_FB* ) pMsg;

    // �� �̵��߿� ���� ������� �ϴ� ���
    if ( pNetMsg->bMoveMap ) m_pCharacter->m_bIsPetActive = TRUE;

    for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
    {
        m_pCharacter->m_sPETSKILLFACT[i].RESET ();

        if ( m_pPet->m_sActiveSkillID[i] == NATIVEID_NULL() ) continue;

        FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_pCharacter->GetGaeaID(),m_pCharacter->GetPosition() ), m_pCharacter->GetSkinChar (), m_pPet->m_sActiveSkillID[i] );
    }

    m_pPet->Delete();			
    PrintMsgText(
        NS_UITEXTCOLOR::PALEGREEN,
        sc::string::format(
        ID2GAMEINTEXT("EMPET_UNUSECARD_FB_OK"),
        m_pPet->GetName()));
}

void GLGaeaClient::MsgPetReqGiveFoodFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_GIVEFOOD_FB* pNetMsg = ( GLMSG::SNETPET_REQ_GIVEFOOD_FB* ) pMsg;
    SITEM* pItem = GLogicData::GetInstance().GetItem ( pNetMsg->sNativeID );
    if ( pItem )
    {
        // �������� ����
        if ( m_pPet->DbNum() == pNetMsg->dwPetID )
        {
            if (m_pPet->IsVALID())
                m_pPet->IncreaseFull ( pItem->sDrugOp.dwCureVolume, pItem->sDrugOp.bRatio );
        }

        if (m_pGlobalStage->IsEmulator())
        {					
            PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(
                ID2GAMEINTEXT("EMPET_REQ_GIVEFOOD_FB_OK"),
                m_pPet->GetName(),
                pItem->GetName(),
                pItem->sDrugOp.dwCureVolume));
            return;
        }

        PETCARDINFO_MAP_ITER iter = m_pCharacter->m_mapPETCardInfo.find( pNetMsg->dwPetID );
        if ( iter!=m_pCharacter->m_mapPETCardInfo.end() )
        {
            SPETCARDINFO& sPet = (*iter).second;
            // ��ī���� ���� ����
            sPet.m_nFull = pNetMsg->nFull;

            PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(
                ID2GAMEINTEXT("EMPET_REQ_GIVEFOOD_FB_OK"),
                sPet.m_szName,
                pItem->GetName(),
                pItem->sDrugOp.dwCureVolume));

			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Pet_UpdateUI );
        }
    }
}

void GLGaeaClient::MsgPetReqPetChardInfoFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_PETCARDINFO_FB* pNetMsg = ( GLMSG::SNETPET_REQ_PETCARDINFO_FB* ) pMsg;

    // DB�� ������ �׳� �ʱⰪ���� �Ѿ���� ��찡 �����Ƿ� üũ���ش�.
    if ( pNetMsg->emTYPE == PETTYPE_NONE )
        return;

    SPETCARDINFO sPetCardInfo;
    sPetCardInfo.m_emTYPE        = pNetMsg->emTYPE;
    sPetCardInfo.m_nFull         = pNetMsg->nFull;
    sPetCardInfo.m_wMaxSkillSlot = pNetMsg->wMaxSkillSlot;
    StringCchCopy ( sPetCardInfo.m_szName, PETNAMESIZE+1, pNetMsg->szName );

    for ( WORD i = 0; i < pNetMsg->wSkillNum; ++i )
    {
        PETSKILL sPetSkill = pNetMsg->Skills[i];
        sPetCardInfo.m_ExpSkills.insert ( std::make_pair(sPetSkill.sNativeID.dwID,sPetSkill) );
    }

    for ( WORD i = 0; i < ACCETYPESIZE; ++i )
    {
        sPetCardInfo.m_PutOnItems[i] = pNetMsg->PutOnItems[i];
    }

    for ( WORD i = 0; i < MAX_PETSKILLSLOT; ++i )
    {
        sPetCardInfo.m_sActiveSkillID[i] = pNetMsg->sActiveSkillID[i];
    }

    if ( !pNetMsg->bTrade )
    {
        PETCARDINFO_MAP_ITER iter = m_pCharacter->m_mapPETCardInfo.find ( pNetMsg->dwPetID );
        if ( iter != m_pCharacter->m_mapPETCardInfo.end() )
            m_pCharacter->m_mapPETCardInfo.erase ( iter );

        m_pCharacter->m_mapPETCardInfo.insert ( std::make_pair(pNetMsg->dwPetID, sPetCardInfo) );
    }
    else
    {
        m_pCharacter->m_mapPETCardInfoTemp.insert ( std::make_pair(pNetMsg->dwPetID, sPetCardInfo) );
    }
}

void GLGaeaClient::MsgPetReqPetRevivedInfoFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_PETREVIVEINFO_FB* pNetMsg = ( GLMSG::SNETPET_REQ_PETREVIVEINFO_FB* ) pMsg;
    m_pCharacter->m_mapPETReviveInfo.clear();
    for ( WORD i = 0; i < pNetMsg->wPetReviveNum; ++i )
    {
        m_pCharacter->m_mapPETReviveInfo.insert ( 
            std::make_pair(pNetMsg->arryPetReviveInfo[i].m_PetDbNum, pNetMsg->arryPetReviveInfo[i] ) );
    }
}

void GLGaeaClient::MsgPetReqReviveFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_REQ_REVIVE_FB* pNetMsg = ( GLMSG::SNETPET_REQ_REVIVE_FB* ) pMsg;

    PETREVIVEINFO sPetRevInfo;
    PETREVIVEINFO_MAP_ITER iter = m_pCharacter->m_mapPETReviveInfo.find ( pNetMsg->dwPetID );
    if ( iter != m_pCharacter->m_mapPETReviveInfo.end() )
        sPetRevInfo = (*iter).second;
    else
        return;

    CString strText;
    switch ( pNetMsg->emFB )
    {
    case EMPET_REQ_REVIVE_FB_OK:
        {					
            PrintMsgText(
                NS_UITEXTCOLOR::PALEGREEN,
                sc::string::format(
                ID2GAMEINTEXT("EMPET_REQ_REVIVE_FB_OK"),
                sPetRevInfo.szPetName));

            SINVENITEM* pINVENITEM = m_pCharacter->m_cInventory.GetItem ( m_pCharacter->m_wInvenPosX1, m_pCharacter->m_wInvenPosY1 );
            if (pINVENITEM)
                pINVENITEM->sItemCustom.PetDbNumSet(sPetRevInfo.m_PetDbNum);

            m_pCharacter->m_mapPETReviveInfo.erase ( iter );

        }
        break;
    case EMPET_REQ_REVIVE_FB_FAIL:				
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            ID2GAMEINTEXT("EMPET_REQ_REVIVE_FB_FAIL"),
            sPetRevInfo.szPetName));
        break;
    case EMPET_REQ_REVIVE_FB_DBFAIL:				
        PrintMsgText(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(
            ID2GAMEINTEXT("EMPET_REQ_REVIVE_FB_DBFAIL"),
            sPetRevInfo.szPetName));
        break;
    }
}

void GLGaeaClient::MsgPetNotEnoughInven(NET_MSG_GENERIC* pMsg)
{
    PrintConsoleTextDlg( ID2GAMEINTEXT("EMPET_NOTENOUGHINVEN") );
}

void GLGaeaClient::MsgCreateAnyPet(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_CREATE_ANYPET* pNetMsg = ( GLMSG::SNETPET_CREATE_ANYPET* ) pMsg;
    SDROPPET& sDropPet = pNetMsg->Data;
   
	if ( m_pLandMClient->CreateAnyPet ( &sDropPet ) != TRUE )
	{
		CDebugSet::ToListView ( "sDropCrow.sMapID != m_pLandMClient->GetMapID()" );
	}
}


void GLGaeaClient::MsgPetDropPet(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPET_DROP_PET* pNetMsg = ( GLMSG::SNETPET_DROP_PET* ) pMsg;
    SDROPPET& sDropPet = pNetMsg->Data;

	if ( m_pLandMClient->DropPet ( &sDropPet ) != TRUE )
    {
        CDebugSet::ToListView ( "sDropCrow.sMapID != m_pLandMClient->GetMapID()" );
    }
}

void GLGaeaClient::ReqPetSkin( DWORD dwItemID, WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY )
{
	GLMSG::SNETPET_SKINPACKOPEN NetMsg( wPosX, wPosY, SNATIVEID( dwItemID ) );
	NetMsg.wHoldPosX = wHoldPosX;
	NetMsg.wHoldPosY = wHoldPosY;
	NETSENDTOFIELD( &NetMsg );
}