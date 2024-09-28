#include "pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/Quest/GLQuest.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Util/GLogicExServer.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../PK/GLSchoolFreePK.h"
#include "../ClubDeathMatch/GLClubDeathMatch.h"
#include "../Club/GLClubServer.h"
#include "../Party/GLPartyField.h"
#include "../PrivateMarket/PrivateMarketField.h"
#include "../Vehicle/GLVehicleField.h"
#include "./GLChar.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLChar::GetViewAroundLandEvent(GLLandNode* pLandNode) 
{
    // Note : - LandEvent -
    LANDEVENTNODESERVERNODE* pLandEventNode = pLandNode->m_LandEventNodeList.m_pHead;
    for ( ; pLandEventNode; pLandEventNode = pLandEventNode->pNext)
    {
        PLANDEVENTNODESERVER pEvent = pLandEventNode->Data;

        if ( pEvent->m_dwGlobID >= MAXLANDEVENT)
            continue;

        // 클라이언트에 생성 메시지 전송
        GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE MsgFC;
        pEvent->ReqNetMsg_Drop(MsgFC);
        SendToClient(&MsgFC);

        PSFIELDLANDEVENT pFIELDLANDEVENT = m_pGaeaServer->NEW_FIELDLANDEVENT();

        //	리스트에 등록.
        //sc::SGLNODE<SFIELDLANDEVENT*>* pNODE = m_ArrayFIELDLANDEVENTLIST.ADDTAIL ( pFIELDLANDEVENT );

        //	값 설정.
        pFIELDLANDEVENT->dwID    = pEvent->m_dwGlobID;
        //pFIELDLANDEVENT->pNODE   = pNODE;
        pFIELDLANDEVENT->dwFRAME = m_dwViewFrame;

        //	배열에 등록.
        SETFIELDLANDEVENT(pFIELDLANDEVENT);
    }	



}

void GLChar::GetViewAroundMoney(GLLandNode * pLandNode)
{
    //	Note : - Money -
    //
    MONEYDROPNODE* pCurMoney = pLandNode->m_MoneyList.m_pHead;
    for ( ; pCurMoney; pCurMoney = pCurMoney->pNext )
    {
        PMONEYDROP pMoneyDrop = pCurMoney->Data;

        GLMSG::SNETDROP_MONEY SendData;
        SendData.fAge     = pMoneyDrop->fAge;
        SendData.dwGlobID = pMoneyDrop->sDrop.dwGlobID;
        SendData.vPos     = pMoneyDrop->sDrop.vPos;
        SendData.lnAmount = pMoneyDrop->sDrop.lnAmount;
        SendData.bCanGet  = pMoneyDrop->IsTakeItem(GetPartyID(),m_dwGaeaID);

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SendToClient(NET_MSG_DROP_MONEY, SendBuffer);

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        //	값 설정.
        pFIELDCROW->emCROW = CROW_MONEY;
        pFIELDCROW->dwID = pMoneyDrop->dwGlobID;
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        //	리스트에 등록.
        RegisterViewCrow(CROW_MONEY, pMoneyDrop->dwGlobID, pFIELDCROW);
    }
}

void GLChar::GetViewAroundItem(GLLandNode * pLandNode) 
{
    //	Note : - Item -
    //
    ITEMDROPNODE* pCurItem = pLandNode->m_ItemList.m_pHead;
    for ( ; pCurItem; pCurItem = pCurItem->pNext)
    {
        CItemDrop* pItemDrop = pCurItem->Data;

        GLMSG::SNETDROP_ITEM SendData;
        SendData.Data.Assign(pItemDrop->sDrop);
        SendData.Data.fAge = pItemDrop->fAge;
        SendData.Data.bCanGet = pItemDrop->IsTakeItem(GetPartyID(), m_dwGaeaID);

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SendToClient(NET_MSG_DROP_ITEM, SendBuffer);

        SFIELDCROW* pFIELDCROW = NEW_FIELDCROW();

        //	값 설정.
        pFIELDCROW->emCROW = CROW_ITEM;
        pFIELDCROW->dwID = pItemDrop->dwGlobID;
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        //	리스트에 등록.
        RegisterViewCrow(CROW_ITEM, pItemDrop->dwGlobID, pFIELDCROW);
    }
}

void GLChar::GetViewAroundMaterial(GLLandNode * pLandNode)
{
    //	Note : - Material -
    //
    GLMATERIALNODE *pMaterialCur = pLandNode->m_MaterialList.m_pHead;
    for ( ; pMaterialCur; pMaterialCur = pMaterialCur->pNext )
    {
        PGLMATERIAL pGLMaterial = pMaterialCur->Data;
        GLMSG::SNETDROP_MATERIAL MsgFC;
        pGLMaterial->ReqNetMsg_Drop(MsgFC);
        SendToClient(&MsgFC);

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        //	값 설정.
        pFIELDCROW->emCROW = CROW_MATERIAL;	//	몹, NPC 모두 CROW_MOB 로 분류하여 관리.
        pFIELDCROW->dwID = pGLMaterial->GetGaeaID();
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        //	리스트에 등록.
        RegisterViewCrow(CROW_MATERIAL, pGLMaterial->GetGaeaID(), pFIELDCROW);
    }	
}

void GLChar::GetViewAroundCrow(GLLandNode * pLandNode)
{
    //	Note : - Crow -
    //
    GLCROWNODE *pCrowCur = pLandNode->m_CROWList.m_pHead;
    for ( ; pCrowCur; pCrowCur = pCrowCur->pNext )
    {
        PGLCROW pGLCrow = pCrowCur->Data;

        GLMSG::SNETDROP_CROW MsgFC;
        pGLCrow->ReqNetMsg_Drop(MsgFC);
        SendToClient(&MsgFC);

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        // 값 설정
        pFIELDCROW->emCROW = CROW_MOB;	//	몹, NPC 모두 CROW_MOB 로 분류하여 관리.
        pFIELDCROW->dwID = pGLCrow->GetGaeaID();
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        // 리스트에 등록.
        RegisterViewCrow(CROW_MOB, pGLCrow->GetGaeaID(), pFIELDCROW);
    }
}

void GLChar::GetViewAroundChar(GLLandNode * pLandNode) 
{
    //	Note : - Char -
    //
    GLCHARNODE *pCharCur = pLandNode->m_PCList.m_pHead;
    for ( ; pCharCur; pCharCur = pCharCur->pNext )
    {
		if(pCharCur == NULL)
			break;		

        GLChar* pGLChar = pCharCur->Data;
        if (!pGLChar)
            continue;
        if (pGLChar->m_ClientSlot == m_ClientSlot)
            continue;
		if ( pGLChar->m_dwGaeaID == m_dwGaeaID )
			continue;

        GASSERT(pGLChar->m_dwGaeaID != m_dwGaeaID);

        //	Memo :	주위 캐릭터에 대한 기본 및 외형 정보를 전송한다.
        SendToClient( &pGLChar->ReqNetMsg_Drop_Base() );
        // 상태효과와 걸린 스킬데이터를 전송한다.
        SendToClient( &pGLChar->ReqNetMsg_Drop_StateBlow_SkillFact() );
        // 착용아이템 정보를 전송한다.
        SendToClient( &pGLChar->ReqNetMsg_Drop_PutOnItems() );

        //	지형효과 전송
        for ( size_t i = 0; i < pGLChar->m_vecLandEffect.size(); ++i ) 
        {
            GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD NetMsg;
            NetMsg.bADD = true;
            NetMsg.landEffect = pGLChar->m_vecLandEffect[i];
            NetMsg.dwGaeaID	= pGLChar->m_dwGaeaID;;	

            SendToClient( &NetMsg );
        }

        //	맵효과 전송
        for ( size_t i = 0; i < pGLChar->m_vecMapEffect.size(); ++i ) 
        {
            GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD NetMsg;
            NetMsg.bADD = true;
            NetMsg.sMapEffect = pGLChar->m_vecMapEffect[i];
            NetMsg.dwGaeaID	= pGLChar->m_dwGaeaID;;	

            SendToClient( &NetMsg );
        }


        if ( pGLChar->m_pVehicle->IsActiveValue() )
        {
            GLMSG::SNETPC_VEHICLE_BRD NetMsgVehicle;

            NetMsgVehicle.emVehicle = pGLChar->m_emVehicle;
            NetMsgVehicle.sVehicle.m_DbNum = pGLChar->m_pVehicle->m_DbNum;
            NetMsgVehicle.sVehicle.m_emTYPE = pGLChar->m_pVehicle->m_emTYPE;
            NetMsgVehicle.sVehicle.m_sVehicleID = pGLChar->m_pVehicle->m_sVehicleID;
            NetMsgVehicle.dwGaeaID = pGLChar->m_dwGaeaID;

            for ( int i =0; i < ACCE_TYPE_SIZE; ++i )
            {
                NetMsgVehicle.sVehicle.m_PutOnItems[i].Assign( pGLChar->m_pVehicle->m_PutOnItems[i] );
            }

            for (INT i = 0; i < ACCE_TYPE_SIZE; i++ )
            {
                NetMsgVehicle.sVehicle.m_sColor[ i ].dwMainColor = pGLChar->m_pVehicle->m_sColor[ i ].dwMainColor;
                NetMsgVehicle.sVehicle.m_sColor[ i ].dwSubColor = pGLChar->m_pVehicle->m_sColor[ i ].dwSubColor;
            }

            NetMsgVehicle.sVehicle.m_dwBoosterID = pGLChar->m_pVehicle->m_dwBoosterID;
            NetMsgVehicle.sVehicle.m_bUseBoost = pGLChar->m_pVehicle->m_bUseBoost;
            NetMsgVehicle.sVehicle.m_bUseBoostDelay = pGLChar->m_pVehicle->m_bUseBoostDelay;
            NetMsgVehicle.sVehicle.m_fBoostDelay = pGLChar->m_pVehicle->m_fBoostDelay;
            NetMsgVehicle.sVehicle.m_fBoostTime = pGLChar->m_pVehicle->m_fBoostTime;

            SendToClient(&NetMsgVehicle );
        }

        //if ( m_sVehicle.IsActiveValue() )
        //{
        //	GLMSG::SNETPC_VEHICLE_BRD NetMsgVehicle;
        //	NetMsgVehicle.emVehicle = m_emVehicle;
        //	NetMsgVehicle.sVehicle.m_dwGUID = m_sVehicle.m_dwGUID;
        //	NetMsgVehicle.sVehicle.m_emTYPE = m_sVehicle.m_emTYPE;
        //	NetMsgVehicle.sVehicle.m_sVehicleID = m_sVehicle.m_sVehicleID;
        //	NetMsgVehicle.dwGaeaID = pGLChar->m_dwGaeaID;

        //	for ( int i =0; i < ACCE_TYPE_SIZE; ++i )
        //	{
        //		NetMsgVehicle.sVehicle.m_PutOnItems[i].Assign( m_sVehicle.m_PutOnItems[i] );
        //	}
        //	m_pGLGaeaServer->SENDTOCLIENT ( m_dwClientID, &NetMsgVehicle );
        //}

        if ( pGLChar->m_pPrivateMarket->IsOpen() )
        {
            GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgPMarket;
            NetMsgPMarket.dwGaeaID = pGLChar->m_dwGaeaID;
            NetMsgPMarket.SetTitle(pGLChar->m_pPrivateMarket->GetTitle());
            SendToClient(&NetMsgPMarket );
        }

        // 젤 처음 브로딩 캐스팅
        if( pGLChar->m_emVehicle )
        {
            GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD NetMsgBRD;
            NetMsgBRD.bActive = true;
            NetMsgBRD.dwGaeaID = pGLChar->m_dwGaeaID;
            NetMsgBRD.emVehicle = pGLChar->m_emVehicle;			
            for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
            {
                NetMsgBRD.dwPassengerID[ i ] = pGLChar->m_pVehicle->PassengerId(i);
            }
            SendToClient(&NetMsgBRD );
        }

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        //	리스트에 등록.
        //sc::SGLNODE<SFIELDCROW*>* pNODE = m_arrayPC_FIELDLIST.ADDTAIL ( pFIELDCROW );

        //	값 설정.
        pFIELDCROW->emCROW = CROW_PC;
        pFIELDCROW->dwID = pGLChar->m_dwGaeaID;
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        //	배열에 등록.
        RegisterViewCrow(CROW_PC, pGLChar->m_dwGaeaID, pFIELDCROW);
    }

    //
}

void GLChar::GetViewAroundPet(GLLandNode* pLandNode)
{
    GLPETNODE* pPetNode = pLandNode->m_PETList.m_pHead;
    for ( ; pPetNode; pPetNode = pPetNode->pNext )
    {
        PGLPETFIELD pPet = pPetNode->Data;

        if ( pPet->m_dwGUID >= GetMaxClient() )
            continue;

        // 클라이언트에 생성 메시지 전송
        GLMSG::SNETPET_DROP_PET MsgFC;
        pPet->ReqNetMsg_Drop(MsgFC);
        SendToClient(&MsgFC);

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        //	값 설정.
        pFIELDCROW->emCROW = CROW_PET;
        pFIELDCROW->dwID = pPet->m_dwGUID;
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        // 리스트에 등록
        RegisterViewCrow(CROW_PET, pPet->m_dwGUID, pFIELDCROW);
    }	
}

void GLChar::GetViewAroundSummon(GLLandNode * pLandNode)
{
    GLSUMMONNODE* pSummonNode = pLandNode->m_SummonList.m_pHead;
    for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
    {
        PGLSUMMONFIELD pSummon = pSummonNode->Data;

        if ( pSummon->GetGaeaID() >= GetMaxClient() )
            continue;

        // 클라이언트에 생성 메시지 전송
        GLMSG::SNET_SUMMON_DROP_SUMMON MsgFC;
        pSummon->ReqNetMsg_Drop(MsgFC);
        SendToClient(&MsgFC);

        for ( size_t i = 0; i < pSummon->m_vecLandEffect.size(); ++i ) 
        {
            GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE NetMsg(pSummon->GetGaeaID(),pSummon->m_vecLandEffect[i],true);
            SendToClient( &NetMsg );
        }

        for ( size_t i = 0; i < pSummon->m_vecMapEffect.size(); ++i ) 
        {
            GLMSG::SNET_SUMMON_MAPEFFECT_UPDATE NetMsg(pSummon->GetGaeaID(), pSummon->m_vecMapEffect[i],true);
            SendToClient( &NetMsg );
        }

        PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

        //	값 설정.
        pFIELDCROW->emCROW = CROW_SUMMON;
        pFIELDCROW->dwID = pSummon->GetGaeaID();
        pFIELDCROW->dwFRAME = m_dwViewFrame;
        //pFIELDCROW->pNODE = pNODE;

        //	리스트에 등록.
        RegisterViewCrow(CROW_SUMMON, pSummon->GetGaeaID(), pFIELDCROW);
    }
}

HRESULT GLChar::UpdateViewAround()
{
	if ( !IsActState(EM_GETVA_AFTER) && !(IsReserveServerStop ()) )
        return S_FALSE;

	++m_dwViewFrame;
	if (m_dwViewFrame == UINT_MAX)
		m_dwViewFrame = 0;

	// if ( m_dwViewFrame%2 == 1 ) return S_FALSE;
	
	const MapID& mapID(GetCurrentMap());
	const SNATIVEID baseMapID(mapID.getBaseMapID());
	const SNATIVEID gaeaMapID(mapID.getGaeaMapID());
	GLLandMan* const pLand(m_pGaeaServer->GetLand(gaeaMapID));
	if ( pLand == NULL )
	{
		sc::writeLogError(sc::string::format("GetViewAround() : pLand == NULL, dbNum:%1% CharMapID(%2%/%3%, %4%/%5%)", 
			m_CharDbNum, 
			baseMapID.Mid(), baseMapID.Sid(), gaeaMapID.Mid(), gaeaMapID.Sid()));
		return E_FAIL;
	}

	//	Note : 현재 시야에 들어오는 CELL 모두 탐색.
	//
	const D3DXVECTOR3& vPosition = GetViewPosition();// GetPosition();
	const int nPosition[2] = 
	{
		int(vPosition.x), 
		int(vPosition.z)
	};
	LANDQUADNODE* pQuadHead = NULL;

	const int _maxViewRange( MAX_VIEWRANGE );
	BOUDRECT bRect(nPosition[0]+_maxViewRange, nPosition[1]+_maxViewRange, nPosition[0]-_maxViewRange, nPosition[1]-_maxViewRange);

	const int _maxViewRange2( GETVISIBLESIGHT() );
	BOUDRECT sRectOnlySee(nPosition[0]+_maxViewRange2, nPosition[1]+_maxViewRange2, nPosition[0]-_maxViewRange2, nPosition[1]-_maxViewRange2);

	// (GETVISIBLESIGHT()+1) 은 혹시나 float * 가 되면서 값이 내려갈 수 있어서 1을 더함.
	// 1 정도는 애교수준으므로 전과 동일하게 해도 상관없다.
	BOUDRECT* pRectOnlySee(NULL);
	if ( (GETVISIBLESIGHT()+1) < MAX_VIEWRANGE )
	{
		// 값이 작을 경우만 동작되도록 한다.
		// 클 경우는 할 필요없다. 괜히 계산만 많아진다. ( m_bFlyCameraUse 가 이 경우 )
		pRectOnlySee = &sRectOnlySee;
	}

	if(GetViewAroundUsed() || m_bFlyCameraUse)
	{
		const int _maxViewRange(int(float(m_uintViewAround) * GETVISIBLESIGHT_RATE()));
		bRect.nMaxX = nPosition[0]+_maxViewRange;
		bRect.nMaxZ = nPosition[1]+_maxViewRange;
		bRect.nMinX = nPosition[0]-_maxViewRange;
		bRect.nMinZ = nPosition[1]-_maxViewRange;
	}

	pLand->GetLandTree()->FindNodes ( bRect, pLand->GetLandTree()->GetRootNode(), &pQuadHead );

	//	Note : 새로 추가되는 노드와 존속 노드를 표시.
	//
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(pLand->GetLandTree()->GetCellNum()>pQuadNode->dwCID&&"The Index exceed cell Size");

		if (IsReserveServerStop()) 
			continue;
		UpdateViewAroundSummon(pQuadNode, bRect);
		UpdateViewAroundPet(pQuadNode, bRect);
		UpdateViewAroundChar(pQuadNode, bRect, pRectOnlySee);
		UpdateViewAroundCrow(pQuadNode, bRect);		
		UpdateViewAroundMaterial(pQuadNode, bRect);
		UpdateViewAroundItem(pQuadNode, bRect);
		UpdateViewAroundMoney(pQuadNode, bRect);
		UpdateViewAroundLandEvent(pQuadNode, bRect);
	}

	//	Note : 제거될 개체들를 처리한다.
	//
	GLMSG::SNETDROP_OUT NetMsgOut;
	NetMsgOut.RESET_CROW();

	// 제거될 FIELD CROW 처리.
	PROFILE_BEGIN("m_arrayFIELDLIST");    
    for (VIEW_CROW_MAP_ITER iter=m_ViewCrow.begin(); iter!=m_ViewCrow.end(); )
    {
        SFIELDCROW* pFIELD = iter->second;
        if (pFIELD->dwFRAME == m_dwViewFrame)
		{
			++iter;
            continue;
		}

        //	제거될 개체 메시지에 등록,  갯수가 가득할 경우 실패.
        bool bOK = NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
        if ( !bOK )
        {
            //	현재까지 모인 정보를 전송.
            SendToClient(&NetMsgOut);

            //	제거될 개체 메시지에 등록.
            NetMsgOut.RESET_CROW();
            NetMsgOut.ADD_CROW(STARID(pFIELD->emCROW,pFIELD->dwID));
        }
        
        //	리스트에서 삭제.
        iter = m_ViewCrow.erase(iter);
        RELEASE_FIELDCROW(pFIELD);
    }
	PROFILE_END("m_arrayFIELDLIST");

	PROFILE_BEGIN("m_arrayLANDEVENTLIST");	
    
    for (LAND_EVENT_MAP_ITER iter=m_LandEventMap.begin(); iter!=m_LandEventMap.end(); )
    {
        SFIELDLANDEVENT* pFIELD = iter->second;
        if (pFIELD->dwFRAME == m_dwViewFrame)
		{
			++iter;
            continue;
		}
		else
		{
			iter = m_LandEventMap.erase(iter);
			m_pGaeaServer->RELEASE_FIELDLANDEVENT(pFIELD);
		}
    }

	PROFILE_END("m_arrayLANDEVENTLIST");

	if ( NetMsgOut.GETAMOUNT() > 0 )
	{
		SendToClient(&NetMsgOut );
	}

	return S_OK;
}


void GLChar::UpdateViewAroundSummon(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    PROFILE_BEGIN("pQuadNode->pData->m_SummonList");
    
	GLSUMMONNODE* pSummonNode = pQuadNode->pData->m_SummonList.m_pHead;
    for ( ; pSummonNode; pSummonNode = pSummonNode->pNext )
    {
        PGLSUMMONFIELD pSummon = pSummonNode->Data;
		const D3DXVECTOR3& vPosition = pSummon->GetPosition();

		const int nPosition[2] = 
		{
			int(vPosition.x), 
			int(vPosition.z)
		};

        if ( bRect.IsWithIn(nPosition[0], nPosition[1] ) )
        {
            if ( pSummon->GetGaeaID() >= MAXCROW )
                continue;

            // 있으면 프레임 갱신
            SFIELDCROW* pCrowSummon = GetViewCrow(CROW_SUMMON, pSummon->GetGaeaID());
            if (pCrowSummon) // if ( m_arrayFieldSummon[pSummon->m_dwGlobID] )
            {
                //m_arrayFieldSummon[pSummon->m_dwGlobID]->dwFRAME = m_dwViewFrame;
                pCrowSummon->dwFRAME = m_dwViewFrame;
            }
            else
            {
                // 클라이언트에 생성 메시지 전송
                GLMSG::SNET_SUMMON_DROP_SUMMON MsgFC;
                pSummon->ReqNetMsg_Drop(MsgFC);
                SendToClient(&MsgFC);

                for ( size_t i = 0; i < pSummon->m_vecLandEffect.size(); ++i ) 
                {
                    GLMSG::SNET_SUMMON_LANDEFFECT_UPDATE NetMsg(pSummon->GetGaeaID(),pSummon->m_vecLandEffect[i],true);
                    SendToClient((NET_MSG_GENERIC*) &NetMsg );
                }

                for ( size_t i = 0; i < pSummon->m_vecMapEffect.size(); ++i ) 
                {
                    GLMSG::SNET_SUMMON_MAPEFFECT_UPDATE NetMsg(pSummon->GetGaeaID(),pSummon->m_vecMapEffect[i],true);
                    SendToClient((NET_MSG_GENERIC*) &NetMsg );
                }

                PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

                //	값 설정.
                pFIELDCROW->emCROW  = CROW_SUMMON;
                pFIELDCROW->dwID    = pSummon->GetGaeaID();
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE   = pNODE;

                // 리스트에 등록.
                RegisterViewCrow(CROW_SUMMON, pSummon->GetGaeaID(), pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_SummonList");
}

void GLChar::UpdateViewAroundPet(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    PROFILE_BEGIN("pQuadNode->pData->m_PETList");
    int nX = 0;
    int nZ = 0;
    GLPETNODE* pPetNode = pQuadNode->pData->m_PETList.m_pHead;
    for ( ; pPetNode; pPetNode = pPetNode->pNext )
    {
        PGLPETFIELD pPet = pPetNode->Data;
        nX = int(pPet->m_vPos.x); nZ = int(pPet->m_vPos.z);

        if (pPet->m_dwGUID >= GetMaxClient())
            continue;

        // 내팻 빼고
        if ( pPet->m_dwGUID != m_dwPetGUID && bRect.IsWithIn ( nX, nZ ) )
        {
            SFIELDCROW* pPetCrow = GetViewCrow(CROW_PET, pPet->m_dwGUID);
            // 있으면 프레임 갱신
            if (pPetCrow)
            {
                pPetCrow->dwFRAME = m_dwViewFrame;
            }
            else
            {
                // 클라이언트에 생성 메시지 전송
                GLMSG::SNETPET_DROP_PET MsgFC;
                pPet->ReqNetMsg_Drop(MsgFC);
                SendToClient(&MsgFC);

                PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

                //	값 설정.
                pFIELDCROW->emCROW = CROW_PET;
                pFIELDCROW->dwID = pPet->m_dwGUID;
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE = pNODE;

                // 리스트에 등록.
                RegisterViewCrow(CROW_PET, pPet->m_dwGUID, pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_PETList");
}

// pRectOnlySee - 버프, 디버프로 인한 시야 변경.
void GLChar::UpdateViewAroundChar(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect, const BOUDRECT* pRectOnlySee)
{
    PROFILE_BEGIN("pQuadNode->pData->m_PCList");
    //	Note : - Char - 
    //
    GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
    for ( ; pCharNode; pCharNode = pCharNode->pNext )
    {
        GLChar* pTargetChar = pCharNode->Data;
        if ( pTargetChar == NULL )
			continue;

		const D3DXVECTOR3& vPosition = pTargetChar->GetPosition();
		const int nPosition[2] = 
		{
			int(vPosition.x), 
			int(vPosition.z)
		};
		
        if ( pTargetChar->m_ClientSlot != m_ClientSlot && (bRect.IsWithIn(nPosition[0], nPosition[1]) ) )
        {
            DWORD dwID = pTargetChar->m_dwGaeaID;
            if (!m_pGaeaServer->IsValidGaeaID(dwID))
                continue;

			bool _bVisibleOff(pTargetChar->IsActState(EM_REQ_VISIBLENONE + EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE));

			// [shhan][2015.09.01] rm #1631 - [스킬]시야제한 효과 수정 요청
			//
			// [기존 문제점]
			// 내 시야가 좁아졌다면, m_ViewCrow 값에서 상대방을 없애버려 내 행동을 상대방이 알지 못한다.
			//
			// [해결방향]
			// 내 시야가 좁아졌다면, 상대방을 SNETDROP_OUT 시켜주도록 한다. ( 내 Client 상만 상대방이 안보임 )
			// 하지만 m_ViewCrow 에는 상대방 정보가 있어야 한다. ( 내 행동을 알려줘야한다 )
			//
			// [추가로 생각해볼 점] ( 이건 기존과 동일 )
			// 상대방이 자신의 위치나 행동은 나에게 보내지만(Server), 난 캐릭터를 DROP_OUT 해서 볼수 없다. (Client)
			// 이렇게 작업되어 있는데 해킹한다면 표시는 할 수 있을 것 같다.
			//
			if ( pRectOnlySee )
			{
				// 내 시야가 변경되었고, 내 시야에 PC 가 안 들어오면 Off 한다.
				if ( !pRectOnlySee->IsWithIn( nPosition[0], nPosition[1] ) )
				{
					_bVisibleOff = true;
				}
			}

			
            SFIELDCROW* pCharCrow = GetViewCrow(CROW_PC, dwID);

			// FlyCamera을 알아야 FlyCam에게 정보를 주기 때문에 알려준다;
			// 내가 FlyCamera상태라면 상대 유저에게 날 ViewAround에 등록시켜야함;
			if ( m_bFlyCameraUse )
			{
				pTargetChar->RegisterViewFlyCamChar(CROW_PC, m_dwGaeaID);
			}

			if ( pCharCrow == NULL )
			{
				PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();                
				pFIELDCROW->emCROW = CROW_PC;
				pFIELDCROW->dwID = dwID;
				pFIELDCROW->invisible = _bVisibleOff == true ? false : true;
				RegisterViewCrow(CROW_PC, dwID, pFIELDCROW);

				pCharCrow = pFIELDCROW;
			}

			pCharCrow->dwFRAME = m_dwViewFrame;

			
			if ( _bVisibleOff != bool(pCharCrow->invisible) )
			{
				pCharCrow->invisible = _bVisibleOff;
				if ( _bVisibleOff == true )
				{
					//dropOut;
					GLMSG::SNETDROP_OUT NetMsgOut;
					NetMsgOut.RESET_CROW();
					NetMsgOut.ADD_CROW(STARID(CROW_PC,dwID));

					SendToClient(&NetMsgOut);
				}
				else
				{
					//	Memo :	주위 캐릭터에 대한 기본 및 외형 정보를 전송한다.;
					SendToClient( &pTargetChar->ReqNetMsg_Drop_Base() );
					// 상태효과와 걸린 스킬데이터를 전송한다.;
					SendToClient( &pTargetChar->ReqNetMsg_Drop_StateBlow_SkillFact() );
					// 착용아이템 정보를 전송한다.;
					SendToClient( &pTargetChar->ReqNetMsg_Drop_PutOnItems() );

					for (size_t i = 0; i < pTargetChar->m_vecLandEffect.size(); ++i) 
					{
						GLMSG::SNETPC_LANDEFFECT_UPDATE_BRD NetMsg;
						NetMsg.bADD = true;
						NetMsg.landEffect = pTargetChar->m_vecLandEffect[i];
						NetMsg.dwGaeaID	= pTargetChar->m_dwGaeaID;
						SendToClient( &NetMsg );
					}

					for ( size_t i = 0; i < pTargetChar->m_vecMapEffect.size(); ++i ) 
					{
						GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD NetMsg;
						NetMsg.bADD = true;
						NetMsg.sMapEffect = pTargetChar->m_vecMapEffect[i];
						NetMsg.dwGaeaID	= pTargetChar->m_dwGaeaID;;	

						SendToClient((NET_MSG_GENERIC*) &NetMsg );
					}

					if ( pTargetChar->m_pVehicle->IsActiveValue() )
					{
						GLMSG::SNETPC_VEHICLE_BRD NetMsgVehicle;

						NetMsgVehicle.emVehicle = pTargetChar->m_emVehicle;
						NetMsgVehicle.sVehicle.m_DbNum = pTargetChar->m_pVehicle->m_DbNum;
						NetMsgVehicle.sVehicle.m_emTYPE = pTargetChar->m_pVehicle->m_emTYPE;
						NetMsgVehicle.sVehicle.m_sVehicleID = pTargetChar->m_pVehicle->m_sVehicleID;
						NetMsgVehicle.dwGaeaID = pTargetChar->m_dwGaeaID;

						for ( int i =0; i < ACCE_TYPE_SIZE; ++i )
						{
							NetMsgVehicle.sVehicle.m_PutOnItems[i].Assign( pTargetChar->m_pVehicle->m_PutOnItems[i] );
						}
						for (INT i = 0; i < ACCE_TYPE_SIZE; i++ )
						{
							NetMsgVehicle.sVehicle.m_sColor[ i ].dwMainColor = pTargetChar->m_pVehicle->m_sColor[ i ].dwMainColor;
							NetMsgVehicle.sVehicle.m_sColor[ i ].dwSubColor = pTargetChar->m_pVehicle->m_sColor[ i ].dwSubColor;
						}

						NetMsgVehicle.sVehicle.m_dwBoosterID = pTargetChar->m_pVehicle->m_dwBoosterID;
						NetMsgVehicle.sVehicle.m_bUseBoost = pTargetChar->m_pVehicle->m_bUseBoost;
						NetMsgVehicle.sVehicle.m_bUseBoostDelay = pTargetChar->m_pVehicle->m_bUseBoostDelay;
						NetMsgVehicle.sVehicle.m_fBoostDelay = pTargetChar->m_pVehicle->m_fBoostDelay;
						NetMsgVehicle.sVehicle.m_fBoostTime = pTargetChar->m_pVehicle->m_fBoostTime;

						SendToClient(&NetMsgVehicle );
					}

					if ( pTargetChar->m_pPrivateMarket->IsOpen() )
					{
						GLMSG::SNETPC_PMARKET_OPEN_BRD NetMsgPMarket;
						NetMsgPMarket.dwGaeaID = pTargetChar->m_dwGaeaID;
						NetMsgPMarket.SetTitle(pTargetChar->m_pPrivateMarket->GetTitle());
						SendToClient(&NetMsgPMarket );
					}

					if( pTargetChar->m_emVehicle )
					{
						GLMSG::SNETPC_ACTIVE_WITH_VEHICLE_BRD NetMsgBRD;
						NetMsgBRD.bActive = true;
						NetMsgBRD.dwGaeaID = pTargetChar->m_dwGaeaID;
						NetMsgBRD.emVehicle = pTargetChar->m_emVehicle;
						for( INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
						{
							NetMsgBRD.dwPassengerID[ i ] = pTargetChar->m_pVehicle->PassengerId(i);
						}
						SendToClient(&NetMsgBRD );
					}

					//PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();                
					//pFIELDCROW->emCROW = CROW_PC;
					//pFIELDCROW->dwID = dwID;
					//pFIELDCROW->dwFRAME = m_dwViewFrame;
					RegisterViewCrow(CROW_PC, dwID, pCharCrow);
				}
			}
        }
    }
    PROFILE_END("pQuadNode->pData->m_PCList");
}

void GLChar::UpdateViewAroundCrow(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    PROFILE_BEGIN("pQuadNode->pData->m_CROWList");
    //	Note : - Crow - 
    //
    if (IsReserveServerStop()) 
        return;

	const MapID& mapID(GetCurrentMap());
	const SNATIVEID baseMapID(mapID.getBaseMapID());
	const SNATIVEID gaeaMapID(mapID.getGaeaMapID());
	GLLandMan* const pLand(m_pGaeaServer->GetLand(gaeaMapID));
	if ( pLand == NULL )
		return;

    int nX = 0;
    int nZ = 0;
    GLCROWNODE *pCrowNode = pQuadNode->pData->m_CROWList.m_pHead;
    for ( ; pCrowNode; pCrowNode = pCrowNode->pNext)
    {
        GLCrow* pGLCrow = pCrowNode->Data;
        nX = int(pGLCrow->GetPosition().x);
        nZ = int(pGLCrow->GetPosition().z);

		GLCrow* pGLCrowTemp = pLand->GetCrow( pGLCrow->GetGaeaID() );

        if ( bRect.IsWithIn(nX,nZ) )
        {
            DWORD dwID = pGLCrow->GetGaeaID();
            if (dwID >= MAXCROW)
                continue;
            SFIELDCROW* pCrowNpc = GetViewCrow(CROW_MOB, dwID);
            if (pCrowNpc) // if (m_arrayFieldCROW[dwID])
            {       
                //m_arrayFieldCROW[dwID]->dwFRAME = m_dwViewFrame;
                pCrowNpc->dwFRAME = m_dwViewFrame;
            }
            else
            {
                GLMSG::SNETDROP_CROW MsgFC;
                pGLCrow->ReqNetMsg_Drop(MsgFC);
                SendToClient(&MsgFC);

                PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

                //	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
                pFIELDCROW->emCROW = CROW_MOB;
                pFIELDCROW->dwID = dwID;
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE = pNODE;

                //	리스트에 등록.
                RegisterViewCrow(CROW_MOB, dwID, pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_CROWList");
}


void GLChar::UpdateViewAroundMaterial(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    if (IsReserveServerStop()) 
        return;

    PROFILE_BEGIN("pQuadNode->pData->m_MaterailList");
    //	Note : - Material - 
    //
    int nX = 0;
    int nZ = 0;
    GLMATERIALNODE *pMaterialNode = pQuadNode->pData->m_MaterialList.m_pHead;
    for ( ; pMaterialNode; pMaterialNode = pMaterialNode->pNext )
    {
        PGLMATERIAL pGLMaterial = pMaterialNode->Data;
        nX = int(pGLMaterial->GetPosition().x); nZ = int(pGLMaterial->GetPosition().z);

        if ( bRect.IsWithIn(nX,nZ) )
        {
            DWORD dwID = pGLMaterial->GetGaeaID();
            if (dwID >= MAXCROW)
                continue;

            SFIELDCROW* pCrowMaterial = GetViewCrow(CROW_MATERIAL, dwID);
            if (pCrowMaterial) //if ( m_arrayFieldMATERIAL[dwID] )
            {
                //m_arrayFieldMATERIAL[dwID]->dwFRAME = m_dwViewFrame;
                pCrowMaterial->dwFRAME = m_dwViewFrame;
            }
            else
            {
                GLMSG::SNETDROP_MATERIAL MsgFC;
                pGLMaterial->ReqNetMsg_Drop(MsgFC);
                SendToClient(&MsgFC);

                PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

                pFIELDCROW->emCROW = CROW_MATERIAL;
                pFIELDCROW->dwID = dwID;
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE = pNODE;

                //	리스트에 등록.
                RegisterViewCrow(CROW_MATERIAL, dwID, pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_MaterailList");
}


void GLChar::UpdateViewAroundItem(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    //	Note : - Item - 
    //
    if (IsReserveServerStop()) 
        return;

    int nX = 0;
    int nZ = 0;
    PROFILE_BEGIN("pQuadNode->pData->m_ItemList");
    ITEMDROPNODE *pItemNode = pQuadNode->pData->m_ItemList.m_pHead;
    for ( ; pItemNode; pItemNode = pItemNode->pNext )
    {
        PITEMDROP pItemDrop = pItemNode->Data;
        nX = int(pItemDrop->vPos.x); nZ = int(pItemDrop->vPos.z);

        if ( bRect.IsWithIn(nX,nZ) )
        {
            DWORD dwID = pItemDrop->dwGlobID;
            if ( dwID>=MAXITEM )
                continue;

            SFIELDCROW* pCrowItem = GetViewCrow(CROW_ITEM, dwID);
            if (pCrowItem) //if ( m_arrayFieldITEM[dwID] )
            {
                //m_arrayFieldITEM[dwID]->dwFRAME = m_dwViewFrame;
                pCrowItem->dwFRAME = m_dwViewFrame;
            }
            else
            {
                GLMSG::SNETDROP_ITEM SendData;
                SendData.Data.Assign(pItemDrop->sDrop);
                SendData.Data.fAge = pItemDrop->fAge;
                SendData.Data.bCanGet = pItemDrop->IsTakeItem(GetPartyID(), m_dwGaeaID);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
                SendToClient(NET_MSG_DROP_ITEM, SendBuffer);

                SFIELDCROW* pFIELDCROW = NEW_FIELDCROW();

                //	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
                pFIELDCROW->emCROW = CROW_ITEM;
                pFIELDCROW->dwID = dwID;
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE = pNODE;

                //	리스트에 등록.
                RegisterViewCrow(CROW_ITEM, dwID, pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_ItemList");
}


void GLChar::UpdateViewAroundMoney(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    //	Note : - Money - 
    //
    if (IsReserveServerStop()) 
        return;

    int nX = 0;
    int nZ = 0;
    PROFILE_BEGIN("pQuadNode->pData->m_MoneyList");
    MONEYDROPNODE *pMoneyNode = pQuadNode->pData->m_MoneyList.m_pHead;
    for ( ; pMoneyNode; pMoneyNode = pMoneyNode->pNext )
    {
        PMONEYDROP pMoneyDrop = pMoneyNode->Data;
        nX = int(pMoneyDrop->vPos.x); nZ = int(pMoneyDrop->vPos.z);

        if ( bRect.IsWithIn(nX,nZ) )
        {
            DWORD dwID = pMoneyDrop->dwGlobID;
            if ( dwID>=MAXMONEY )
                continue;

            SFIELDCROW* pCrowMoney = GetViewCrow(CROW_MONEY, dwID);
            if (pCrowMoney) //if ( m_arrayFieldMONEY[dwID] )
            {
                //m_arrayFieldMONEY[dwID]->dwFRAME = m_dwViewFrame;
                pCrowMoney->dwFRAME = m_dwViewFrame;
            }
            else
            {
                GLMSG::SNETDROP_MONEY SendData;
                SendData.fAge     = pMoneyDrop->fAge;
                SendData.dwGlobID = pMoneyDrop->sDrop.dwGlobID;
                SendData.vPos     = pMoneyDrop->sDrop.vPos;
                SendData.lnAmount = pMoneyDrop->sDrop.lnAmount;
                SendData.bCanGet  = pMoneyDrop->IsTakeItem(GetPartyID(), m_dwGaeaID);

                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
                SendToClient(NET_MSG_DROP_MONEY, SendBuffer);

                PSFIELDCROW pFIELDCROW = NEW_FIELDCROW();

                //	편의상 MOB, NPC 모두 CROW_MOB으로 분류.
                pFIELDCROW->emCROW = CROW_MONEY;
                pFIELDCROW->dwID = dwID;
                pFIELDCROW->dwFRAME = m_dwViewFrame;
                //pFIELDCROW->pNODE = pNODE;

                //	리스트에 등록.
                RegisterViewCrow(CROW_MONEY, dwID, pFIELDCROW);
            }
        }
    }
    PROFILE_END("pQuadNode->pData->m_MoneyList");
}


void GLChar::UpdateViewAroundLandEvent(LANDQUADNODE* pQuadNode, const BOUDRECT& bRect)
{
    PROFILE_BEGIN("pQuadNode->pData->m_LandEventNodeList");
    int nX = 0;
    int nZ = 0;
    LANDEVENTNODESERVERNODE* pLandEventNode = pQuadNode->pData->m_LandEventNodeList.m_pHead;
    for ( ; pLandEventNode; pLandEventNode = pLandEventNode->pNext )
    {
        PLANDEVENTNODESERVER pEvent = pLandEventNode->Data;
        nX = int(pEvent->m_vPosition.x); nZ = int(pEvent->m_vPosition.z);

        if ( bRect.IsWithIn(nX,nZ) )
        {
            DWORD dwID = pEvent->m_dwGlobID;
            if ( pEvent->m_dwGlobID >= MAXLANDEVENT )
                continue;

            SFIELDLANDEVENT* pLandEvent = GetLandEvent(dwID);
            if (pLandEvent) //if ( m_arrayFieldLandEvent[dwID] )
            {
                //m_arrayFieldLandEvent[dwID]->dwFRAME = m_dwViewFrame;
                pLandEvent->dwFRAME = m_dwViewFrame;
            }
            else
            {
                // 클라이언트에 생성 메시지 전송
                GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE MsgFC;
                pEvent->ReqNetMsg_Drop(MsgFC);
                SendToClient(&MsgFC);

                PSFIELDLANDEVENT pFIELDLANDEVENT = m_pGaeaServer->NEW_FIELDLANDEVENT();

                // 리스트에 등록.
                //sc::SGLNODE<SFIELDLANDEVENT*>* pNODE = m_ArrayFIELDLANDEVENTLIST.ADDTAIL ( pFIELDLANDEVENT );

                //	값 설정.
                pFIELDLANDEVENT->dwID    = pEvent->m_dwGlobID;
                //pFIELDLANDEVENT->pNODE   = pNODE;
                pFIELDLANDEVENT->dwFRAME = m_dwViewFrame;

                //	배열에 등록.
                SETFIELDLANDEVENT(pFIELDLANDEVENT);
            }
        }
    }	
    PROFILE_END("pQuadNode->pData->m_LandEventNodeList");
}
