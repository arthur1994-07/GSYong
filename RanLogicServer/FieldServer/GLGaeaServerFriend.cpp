#include "../pch.h"
#include "../Club/GLClubMan.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// *****************************************************
// Desc: 친구이동시 친구위치 반환 (Agent->Field)
// *****************************************************
BOOL GLGaeaServer::Request2FriendCK(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GASSERT(IsField());

    GLMSG::SNETPC_2_FRIEND_CK *pNetMsg = (GLMSG::SNETPC_2_FRIEND_CK *)nmg;
    GLMSG::SNETPC_2_FRIEND_AG NetMsgAG;
    NetMsgAG.dwCOMMAND_CHARID = pNetMsg->dwCOMMAND_CHARID;
    NetMsgAG.dwFRIEND_CHARID = pNetMsg->dwFRIEND_CHARID;
    NetMsgAG.wItemPosX = pNetMsg->wItemPosX;
    NetMsgAG.wItemPosY = pNetMsg->wItemPosY;

    GLChar* pFRIEND = GetCharByDbNum( pNetMsg->dwFRIEND_CHARID );
    if ( !pFRIEND )
    {
        NetMsgAG.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
        SENDTOAGENT(&NetMsgAG);
        return FALSE;
    }

	GLLandMan* const pLand(pFRIEND->GetLandMan());
	if ( pLand == NULL )
		return E_FAIL;

    if ( !pFRIEND->IsValidBody() || pLand==NULL )
    {
        NetMsgAG.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
        SENDTOAGENT(&NetMsgAG);
        return FALSE;
    }

    const SMAPNODE *pMapNode = FindMapNode ( pLand->GetBaseMapID() );
    if ( !pMapNode )
    {
        NetMsgAG.emFB = EM2FRIEND_FB_FAIL;
        SENDTOAGENT(&NetMsgAG);
        return FALSE;
    }	

	InstanceSystem::InstanceField* const pInstance(pLand->GetScriptInstance());
	if ( pInstance == NULL )
	{
		// MapList 에 Move flag 가 설정되어 있지 않으면
		// 친구연결카드로 이동불가능
		if ( !pMapNode->IsFriendMove() )
		{
			NetMsgAG.emFB = EM2FRIEND_FB_IMMOVABLE;
			SENDTOAGENT(&NetMsgAG);
			return FALSE;
		}		
	}
	else
	{ // 신규 인던의 경우 란 기본 시스템 외의 설정을 무시하고,
		// 스크립트에서의 결정을 따른다;
        const SNATIVEID _itemID(pNetMsg->dwItemID);
		if ( pInstance->EventUseItem(pFRIEND->GetCharID(), _itemID.Mid(), _itemID.Sid(), pNetMsg->dwCOMMAND_CHARID) == false )
		{
			NetMsgAG.emFB = EM2FRIEND_FB_FAIL;
			SENDTOAGENT(&NetMsgAG);
			return FALSE;
		}
	}

    BOOL bCoB(FALSE);
    DWORD dwCoB(0);
    D3DXVECTOR3 vCoB(0,0,0);

    D3DXVECTOR3 vec2POS = pFRIEND->GetPosition();
    D3DXMATRIX matRotY;

	boost::shared_ptr<NavigationMesh> spNavi = pLand->GetNavi();
	if ( spNavi )
	{
		for ( float fAngle=0.0f; fAngle<D3DX_PI*2; fAngle += D3DX_PI/4.0f )
		{
			D3DXVECTOR3 vecDIR(1,0,0);
			D3DXMatrixRotationY ( &matRotY, fAngle );

			D3DXVec3TransformCoord ( &vecDIR, &vecDIR, &matRotY );
			D3DXVECTOR3 vPos = pFRIEND->GetPosition() + vecDIR*10.0f;

			spNavi->IsCollision ( D3DXVECTOR3(vPos.x,vPos.y+20.0f,vPos.z), D3DXVECTOR3(vPos.x,vPos.y-30.0f,vPos.z), vCoB, dwCoB, bCoB );
			if ( bCoB )
			{
				vec2POS = vCoB;
				break;
			}
		}
	}

	// 매크로가 켜져있다면 맵이동시 끈다;
	if ( pFRIEND->IsActState(EM_ACT_MACROMODE) )
	{
		pFRIEND->MacroOnOff(false);
		GLMSG::SNET_MACROMODE_CLOSE_FC msg;
		SENDTOCLIENT ( pFRIEND->ClientSlot(), &msg );
	}

    //	Note : 위치 반환.
    NetMsgAG.emFB = EM2FRIEND_FB_OK;
    NetMsgAG.sFriendMapID = pFRIEND->GetCurrentMap();
    NetMsgAG.vFriendPos = vec2POS;
    SENDTOAGENT(&NetMsgAG);

    return TRUE;
}

BOOL GLGaeaServer::Request2FriendREQ(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_2_FRIEND_REQ* pNetMsg = (GLMSG::SNETPC_2_FRIEND_REQ*) nmg;

	
	//if ( pNetMsg->dwGaeaID != dwGaeaID )
	//		return FALSE;

	GLChar* pChar = GetChar(dwGaeaID);
	if ( pChar == NULL )
		return FALSE;

	pNetMsg->dwChaDbNum = pChar->CharDbNum();

	const GLLandMan* const pLand(pChar->GetLandMan());
	if ( pLand == NULL )
		return FALSE;

	SINVENITEM* pInvenItem( pChar->InvenGetItem( pNetMsg->wItemPosX, pNetMsg->wItemPosY ) );
	if ( pInvenItem == NULL )
		return FALSE;

	if ( pInvenItem->Id() != pNetMsg->dwItemID )
		return FALSE;

	SENDTOAGENT(pNetMsg);
	return TRUE;
}

BOOL GLGaeaServer::MsgReqFriendFld(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETPC_2_FRIEND_FLD* pNetMsg = (GLMSG::SNETPC_2_FRIEND_FLD*)nmg;

    if ( dwGaeaID != GAEAID_NULL )
        return FALSE;
    
    GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
    if ( !pChar )
        return FALSE;
    pChar->MsgReqFriendFld( nmg );
    return TRUE;    
}
