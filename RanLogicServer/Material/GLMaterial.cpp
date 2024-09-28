#include "pch.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../Material/GLMaterial.h"
#include "../FieldServer/GLGaeaServer.h"

#include "../Util/GLItemLimit.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#define BOX_R (4)

GLMaterial::GLMaterial(GLGaeaServer* pGaeaServer)
    : ServerActor(pGaeaServer, CROW_MATERIAL)	
    , m_pd3dDevice(NULL)
    , m_pSkinChar(NULL)
    
    , m_vMaxOrg(BOX_R,20,BOX_R)
    , m_vMinOrg(-BOX_R,0,-BOX_R)
    
    , m_vMax(BOX_R,20,BOX_R)
   , m_vMin(-BOX_R,0,-BOX_R)    
    
    
    //, m_pGlobNode(NULL)
    , m_pQuadNode(NULL)
    , m_pCellNode(NULL)
    
    , m_fAge(0.0f)
    , m_dwCFlag(NULL)
    , m_vGenPos(0,0,0)
    , m_vStartPos(0,0,0)
    
    , m_fIdleTime(0.0f)
    , m_pMobSchedule(NULL)
    , m_dwGatherCharID(GAEAID_NULL)
    , m_fGatherTime(0.0f)
{	
}

GLMaterial::~GLMaterial(void)
{
	SAFE_DELETE(m_pSkinChar);
}

void GLMaterial::RESET_DATA ()
{
	int i=0;

	m_pd3dDevice = NULL;
	
	m_actorMove.Stop();
	m_actorMove.Release();

	SAFE_DELETE(m_pSkinChar);

	m_vDirect = D3DXVECTOR3(0,0,0);
	
	D3DXMatrixIdentity ( &m_matTrans );

	m_vMaxOrg = D3DXVECTOR3(0,0,0);
	m_vMinOrg = D3DXVECTOR3(0,0,0);

	m_vMax = D3DXVECTOR3(0,0,0);
	m_vMin = D3DXVECTOR3(0,0,0);	

	m_mapID = SNATIVEID(false);
	m_dwCeID = 0;
	
	m_pQuadNode = NULL;
	m_pCellNode = NULL;

	m_fAge = 0.0f;
	m_vPosition = D3DXVECTOR3(0,0,0);
	m_vGenPos = D3DXVECTOR3(0,0,0);
	m_vStartPos = D3DXVECTOR3(0,0,0);
	m_fIdleTime = 0.0f;

	m_dwGatherCharID = GAEAID_NULL;
	m_fGatherTime = 0.0f;

	ServerActor::Reset();
}

HRESULT GLMaterial::CreateMaterial(
    GLMobSchedule* pMobSchedule,
    GLLandMan* pLandMan,
    const SNATIVEID& sCROWID,
    LPDIRECT3DDEVICEQ pd3dDevice,
    LPD3DXVECTOR3 pPos)
{
	GASSERT(pLandMan);

//	m_pLandMan = pLandMan;
	m_pd3dDevice = pd3dDevice;

	// ���� ���� ������ ������.
	m_pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sCROWID );
	if (!m_pCrowData)
	{
        sc::writeLogError(
            sc::string::format(
                "CreateMaterial. CrowID %1%/%2%",
                sCROWID.wMainID, sCROWID.wSubID));
		return E_FAIL;
	}

	m_sNativeID = m_pCrowData->sNativeID;

	// �����췯 �ʱ�ȭ.
	m_pMobSchedule = pMobSchedule;

	//	��ġ �ʱ�ȭ.
	if ( pPos )
		m_vPosition = *pPos;
	else if ( m_pMobSchedule )
		m_vPosition = m_pMobSchedule->m_pAffineParts->vTrans;
	
	m_vGenPos = m_vPosition;

	//	���� �ʱ�ȭ.
	D3DXMATRIX matY;
	D3DXMatrixIdentity ( &matY );
	
	if ( m_pMobSchedule )
		D3DXMatrixRotationY ( &matY, m_pMobSchedule->m_pAffineParts->vRotate.x );

	D3DXVec3TransformCoord ( &m_vDirect, &vDIRORIG, &matY );
	
	if ( PathIsActive() )
		StopActorMove();
	CreateActorMove(pLandMan->GetNavi(), m_vPosition);

	// �ֺ� �ɸ��� �ν�.
	//
	UpdateViewAround ();

	if ( m_pd3dDevice )
	{
		DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_pCrowData->GetSkinObjFile(), m_pd3dDevice );
		if ( !pCharData )
            return E_FAIL;

		SAFE_DELETE(m_pSkinChar);
		m_pSkinChar = new DxSkinChar;
		m_pSkinChar->SetCharData ( pCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD );
	}

	return S_OK;
}

void GLMaterial::TurnAction(const EMACTIONTYPE toAction, const bool bMsg)
{
	const EMACTIONTYPE& emAction = GetAction();
	// ���� �׼� ���.
	//
	switch ( emAction )
	{
	case GLAT_IDLE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	default:
		break;
	};

	// �׼� �ʱ�ȭ.
	//	
	SetAction(toAction);

	switch ( toAction )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;
		}
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_FALLING:
		{
			m_fIdleTime = 0.0f;
			if ( !bMsg )
				break;

			// �ֺ� Char���� �������� �˸�.
			GLMSG::SNET_ACTION_BRD NetMsg;
			NetMsg.emCrow	= m_pCrowData->m_emCrow;
			NetMsg.dwID		= m_dwGaeaID;
			NetMsg.emAction	= GetAction();

			SendMsgViewAround ( (NET_MSG_GENERIC*) &NetMsg );
		}
		break;

	default:
		break;
	};
}

void GLMaterial::ReqNetMsg_Drop(GLMSG::SNETDROP_MATERIAL& Msg)
{
	//GLMSG::SNETDROP_MATERIAL NetMsg;
	//NetMsg = GLMSG::SNETDROP_MATERIAL();

	SDROP_MATERIAL &dropCrow	= Msg.Data;
	dropCrow.sNativeID			= m_sNativeID;

	dropCrow.sMapID				= GetCurrentMap().getBaseMapID();
	dropCrow.dwCeID				= m_dwCeID;
	dropCrow.dwGlobID			= m_dwGaeaID;
	dropCrow.vPos				= m_vPosition;
	dropCrow.vDir				= m_vDirect;

	if ( m_fAge < 0.6f )
		dropCrow.dwFLAGS	|= SDROP_CROW::CROW_GEM;

	dropCrow.emAction		= GetAction();
	dropCrow.dwActState		= GetActState();

	//return NetMsg;
}

inline void GLMaterial::INSERT_PC_VIEW ( DWORD dwGAEAID )
{
	if ( m_vecPC_VIEW.size() < m_dwPC_AMOUNT+16 )
		m_vecPC_VIEW.resize(m_dwPC_AMOUNT+16);

	m_vecPC_VIEW[m_dwPC_AMOUNT++] = dwGAEAID;
}

HRESULT GLMaterial::UpdateViewAround ()
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	int nX, nZ;

	m_dwPC_AMOUNT = 0;

	// ���� �þ߿� ������ CELL ��� Ž��.
	//
	nX = int ( m_vPosition.x );
	nZ = int ( m_vPosition.z );
	LANDQUADNODE* pQuadHead = NULL;
	BOUDRECT bRect(nX+MAX_VIEWRANGE,nZ+MAX_VIEWRANGE,nX-MAX_VIEWRANGE,nZ-MAX_VIEWRANGE);
	pLand->GetLandTree()->FindNodes ( bRect, pLand->GetLandTree()->GetRootNode(), &pQuadHead );
	
	LANDQUADNODE* pQuadNode = pQuadHead;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GLLandNode *pLandNode = pQuadNode->pData;
		if( !pLandNode )
			continue;

		if ( pQuadNode->pData->m_PCList.m_dwAmount==0 )
			continue;
		
		// - Char - 
		//
		GLCHARNODE *pCharNode = pQuadNode->pData->m_PCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar *pPChar = pCharNode->Data;
			if( !pPChar ) continue;

			// ĳ������ ���� ������ UpdateViewAround ()���� ����ġ�� ĳ���Ͱ� �������� ���� �� ����.
			if ( !pPChar )													
				continue;

			if ( !m_pGaeaServer->GetChar ( pPChar->GetGaeaID() ) )	
				continue;

			if ( bRect.IsWithIn(nX,nZ) )
			{
				INSERT_PC_VIEW ( pPChar->GetGaeaID() );
			}
		}
	}

	return S_OK;
}

HRESULT GLMaterial::SendMsgViewAround(NET_MSG_GENERIC* pMsg)
{
	DWORD dwGAEAID = 0;
	GLChar* pPChar = NULL;

	DWORD wSize = (DWORD)m_vecPC_VIEW.size();

	for ( DWORD i=0; i<m_dwPC_AMOUNT && i<wSize; ++i )
	{
		//	Memo :	m_vecPC_VIEW[i] is GaeaID
		pPChar = m_pGaeaServer->GetChar ( m_vecPC_VIEW[i] );
		if ( pPChar )
		{
			m_pGaeaServer->SENDTOCLIENT ( pPChar->ClientSlot(), pMsg );
		}
	}

	return S_OK;
}

void GLMaterial::SendToClient(NET_MSG_GENERIC* pMsg){
	GLMaterial::SendMsgViewAround(pMsg);
}
void GLMaterial::SendToClient(const DWORD dwClientSlot, NET_MSG_GENERIC* pMsg)
{
	if (m_pGaeaServer)
		m_pGaeaServer->SENDTOCLIENT(dwClientSlot, pMsg);
}


HRESULT GLMaterial::UpateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	float fAnimation = fElapsedTime;

	EMANI_MAINTYPE MType = m_pSkinChar->GETCURMTYPE();
	EMANI_SUBTYPE SType = m_pSkinChar->GETCURSTYPE();
	float fLIFETIME = m_pSkinChar->GETCURANITIME();

	const EMACTIONTYPE& emAction = GetAction();
	switch ( emAction )
	{
	case GLAT_IDLE:
		m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_FALLING:
		m_pSkinChar->SELECTANI ( AN_DIE, AN_SUB_NONE, EMANI_ENDFREEZE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_DIE );
		break;

	case GLAT_DIE:
		fAnimation = 0.0f;
		m_pSkinChar->TOENDTIME();
		break;
	case GLAT_GATHERING:
		m_pSkinChar->SELECTANI ( AN_GATHERING, AN_SUB_NONE );
		if ( m_pSkinChar->ISENDANIM() )			TurnAction ( GLAT_IDLE );
		break;
	};

	// Mob ��Ų ������Ʈ.
	//
	float fSkinAniElap = fElapsedTime;

	switch ( emAction )
	{
	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		break;
	};

	m_pSkinChar->SetPosition ( m_vPosition );
	m_pSkinChar->FrameMove( fTime, fSkinAniElap, FALSE, m_matTrans );

	return S_OK;
}

HRESULT GLMaterial::UpdateGathering( float fElapsedTime )
{
	if ( m_dwGatherCharID == GAEAID_NULL )	return S_OK;

	PGLCHAR pChar = m_pGaeaServer->GetChar( m_dwGatherCharID );
	if ( !pChar || !pChar->isAction( GLAT_GATHERING ) )
	{
		m_dwGatherCharID = GAEAID_NULL;
		return S_OK;
	}

	m_fGatherTime -= fElapsedTime;
			
	// ä�� �Ϸ� ��� ����
	if ( m_fGatherTime < 0.0f )
	{
		GLMSG::SNETPC_REQ_GATHERING_RESULT NetMsg;
        if (sc::Random::getInstance().IsSucceed(m_pCrowData->m_fGatherRate)) 
		{
			bool bDrop = false;

			if ( !GenerateGathering ( pChar->GetGaeaID(), bDrop ) ) 
			{
				NetMsg.emFB = EMREQ_GATHER_RESULT_ITEMFAIL;
			}
			else
			{
				if ( bDrop ) NetMsg.emFB = EMREQ_GATHER_RESULT_SUCCESS_EX;
				else NetMsg.emFB = EMREQ_GATHER_RESULT_SUCCESS;				
			}
		}
		else
		{
			NetMsg.emFB = EMREQ_GATHER_RESULT_FAIL;
		}
        		
		m_pGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );				

		GLMSG::SNETPC_REQ_GATHERING_RESULT_BRD	NetMsgBrd;
		NetMsgBrd.dwGaeaID = pChar->GetGaeaID();
		NetMsgBrd.emFB = NetMsg.emFB;
		pChar->SendMsgViewAround(&NetMsgBrd);

		// ���� ����			
		
		m_dwGatherCharID = GAEAID_NULL;
		// ����( ����� )
		TurnAction ( GLAT_FALLING );
		pChar->ResetAction();
	}

	return S_OK;
}

HRESULT GLMaterial::FrameMove ( float fTime, float fElapsedTime )
{
	GASSERT(m_pCrowData);


	UpdateGathering( fElapsedTime );
	// ���忡�� ��ϵ� ���� ������Ʈ �ϱ� ���ؼ� ȣ����.
	//
	UpdateViewAround ();

	//	���� ������ �ɸ��Ͱ� ����, skinchar�� ��ȿ�϶�(���ķ�����,level edit)�� �ƴҶ�. & �������� �ƴҶ�
	if ( m_dwPC_AMOUNT==0 && !m_pSkinChar ) return S_OK;

	m_fAge += fElapsedTime;

	// AABB ���.
	//
	m_vMax = m_vPosition + m_vMaxOrg;
	m_vMin = m_vPosition + m_vMinOrg;

	// ���� ��ǥ�� ��ȿ�� �˻�.
	//		(����) Ÿ������ ����� �ݵ�� ȣ���Ͽ� ��ȿ�� �˻� �ʿ�.
	//



	// �׼Ǻ� ó���۾�.
	//
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		m_fIdleTime += fElapsedTime;
		break;

	case GLAT_TALK:
		break;

	case GLAT_MOVE:
		break;

	case GLAT_ATTACK:
		break;

	case GLAT_SKILL:
		break;

	case GLAT_SHOCK:
		break;

	case GLAT_FALLING:
		m_fIdleTime += fElapsedTime;
		if ( m_fIdleTime > 4.0f )
			TurnAction ( GLAT_DIE );
		break;

	case GLAT_PUSHPULL:
		break;

	case GLAT_DIE:
		break;
	case GLAT_GATHERING:
		break;
	};

	// Mob�� ���� ��ġ ������Ʈ.
	//
	m_vPosition = m_actorMove.Position();

	// m_pd3dDevice �� ��ȿ�� ��츸 �Ʒ� �ڵ尡 ����ȴ�. ( Level Editor ���� �½�Ʈ��. )
	//
	if ( !m_pd3dDevice )	return S_OK;

	// ���ϸ��̼�, ��Ų ������Ʈ.
	//
	UpateAnimation ( fTime, fElapsedTime );

	// ���� ��ġ�� �������� Transform ��Ʈ���� ���.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPosition.x, m_vPosition.y, m_vPosition.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDirect, vDIRORIG );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	return S_OK;
}

HRESULT GLMaterial::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{


	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	if ( bRendAABB ) EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	�׽�Ʈ��.
	//HRESULT hr;	
	//hr = m_pSkinChar->Render ( pd3dDevice, TRUE, 1.f, m_matTrans );
	//if ( FAILED(hr) )	return hr;

	//////////////////////////////////////////////////////////////////////////
	//
	//	�̰��� �´ٴ°� Emulater�� �����ٴ� �Ű� �װ� ���������� �۾��Ѵٴ°���.
	//	������� ���� ���ص� ��.
	//
	//if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//{
	//	m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matTrans );
	//}
	//else
	{
		m_pSkinChar->SetPhysX_Cloth( TRUE );
		m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
		m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}

	return S_OK;
}


// �̺�Ʈ�� Ž�� ���� �ý��ۿ� ���� ����� ������ ������� ���Ѵ�.
float GLMaterial::CalculateItemGenRate( float fGenRate, PGLCHAR pChar, bool bParty )
{
	float fEVENT_RATE = 1.0f; 
	
	float fRate = 1.0f; 

	if( !pChar ) 
	{
		fRate = fGenRate * fEVENT_RATE;
		return fRate;
	}

	fEVENT_RATE = m_pGaeaServer->GetItemGainRate(pChar->m_CHARINDEX,pChar->m_wLevel);
    fRate = fGenRate * fEVENT_RATE;

#ifdef CH_PARAM_USEGAIN //chinaTest%%%
	if( pChar->m_ChinaGainType == GAINTYPE_HALF )
	{
		fRate /= 2.0f;
	}else if( pChar->m_ChinaGainType == GAINTYPE_EMPTY )
	{
		fRate = 0.0f;
	}
#endif
#ifndef CH_PARAM_USEGAIN //** Add EventTime
	if( pChar->m_bEventStart && pChar->m_bEventApply)
	{
		SEventState sEventState = m_pGaeaServer->m_sEventState;
		if( sEventState.bEventStart )
			fRate *= sEventState.fItemGainRate;
	}
#endif

	return fRate;
}

DWORD GLMaterial::GenerateGathering( DWORD dwGaeaID, bool& bDrop )
{
#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
    return GenerateGathering_20060417( dwGaeaID, bDrop );
#else
    return GenerateGathering_20110722( dwGaeaID, bDrop );
#endif
}

#if defined( _ITEM_GEN_110722_H_ )
DWORD GLMaterial::GenerateGathering_20110722( DWORD dwGaeaID, bool& bDrop )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

    EMGROUP emItemHoldGrp(EMGROUP_ONE);
    DWORD dwItemHoldID(GAEAID_NULL);

    // ȹ�� ���� �ɼ� ����.
    //

    if ( dwGaeaID == GAEAID_NULL ) return FALSE;

    PGLCHAR pChar = m_pGaeaServer->GetChar(dwGaeaID);
    if (!pChar)
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on GenerateGathering_20110722 ", dwGaeaID));
        return FALSE;
	}

    float fGEN_RATE = 1.0f;
    bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
    if (bgenitemhold == false)
    {
        dwItemHoldID = GAEAID_NULL;
    }
    else 
    {
        emItemHoldGrp = EMGROUP_ONE;
        dwItemHoldID = dwGaeaID;			
    }

    float fGenRate = 1.0f; 

    fGenRate = CalculateItemGenRate( fGenRate, pChar, false );

    //CGENITEM &cGenItem = m_pCrowData->m_cGenItem;

     // Note : ������ �߻�    
	Generate(pChar);

	VEC_RESULT_ITER iter = m_vecResult.begin();

    for (; iter != m_vecResult.end(); ++iter)
    {
        SGENRESULT& sResult = (*iter);

        if ( sResult.IsNull() )
        {
            continue;
        } 

        D3DXVECTOR3 vGenPos = D3DXVECTOR3(m_vPosition) + D3DXVECTOR3(sc::Random::RANDOM_NUM()*4.0f,0.0f,sc::Random::RANDOM_NUM()*4.0f);

        BOOL bCollision;
        D3DXVECTOR3 vCollision;
        bCollision = pLand->IsCollisionNavi
            (
            D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,+5,0),
            D3DXVECTOR3(vGenPos)+D3DXVECTOR3(0,-5,0),
            vCollision
            );

        GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();

        switch ( sResult.emType )
        {
        case EMGENRESULTTYPE_ITEM:
            {
                SITEMCUSTOM sITEM_NEW(sResult.Data.sItemID);
                sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
                sITEM_NEW.cGenType = EMGEN_GATHERING;
                // sITEM_NEW.cChnID = (BYTE) m_pGaeaServer->GetServerChannel();
                //sITEM_NEW.cFieldID = (BYTE) m_pGaeaServer->GetFieldSvrID();
                sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );

				sITEM_NEW.GenerateBasicStat( false );
				sITEM_NEW.GenerateLinkSkill();
				sITEM_NEW.GenerateAddOption();

                CItemDrop cDropItem;
                cDropItem.sItemCustom = sITEM_NEW;
                if ( pChar->IsInsertToInven ( &cDropItem ) )
                {
                    pChar->InsertToInven ( &cDropItem, true, true, false, false, true );

					GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
					// ��� �� �α� ����
                    pItemLimit->LogItemExchange(
						sITEM_NEW,
						gamelog::ID_MOB,
						m_pCrowData->sNativeID.dwID,
						gamelog::ID_CHAR,
						pChar->m_CharDbNum, 
                        ITEM_ROUTE_GATHERING,
						sITEM_NEW.wTurnNum,
						( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_GATHERING,
						false );
                }
                else
                {
                    if ( bCollision == FALSE )
						break;
					
					bDrop = true;
                    pLand->DropItem ( m_vPosition, &(cDropItem.sItemCustom), emItemHoldGrp, dwItemHoldID );
                }
            }
            break;

        case EMGENRESULTTYPE_MONEY:
            {
                if ( bCollision == FALSE )
					break;

                pLand->DropMoney ( vGenPos, sResult.Data.dwMoney, TRUE, emItemHoldGrp, dwItemHoldID );
            }
            break;
        }
    }

    return TRUE;	
}
#endif


/*
Callback_CalculateGenProb
Callback_CalculateGenItemRate
Callback_CalculateGenResult
Callback_GenItemLog
*/
void GLMaterial::Generate(GLChar* pChar, bool bParty)
{
    m_vecResult.clear();

	CGENITEM::VEC_SGENINFO &vecGenInfo = m_pCrowData->m_cGenItem.m_vecGenInfo;

    CGENITEM::VEC_SGENINFO_ITER InfoIter = vecGenInfo.begin();
    for (; InfoIter != vecGenInfo.end(); ++InfoIter)
    {
        SGENINFO& sInfo = (*InfoIter);

        if ( sInfo.fProbability == 0.0f )
        {
            continue;
        }

        // �������� �߻�Ȯ�� üũ
        if ( sInfo.fProbability < 100.0f )
        {
            float fProb = sInfo.fProbability;            

            if ( !sc::Random::getInstance().IsSucceed(fProb) )
            {
                continue;
            }
        }

        // �߻����� Ȯ�� üũ
        WORD wNum = m_pCrowData->m_cGenItem.GenerateNum(sInfo.vecGenNum);

        if ( wNum == 0 )
        {
            continue;
        }

        // ������ �߻�
        for ( WORD i=0; i<wNum; ++i )
        {
            SGENRESULT sResult;
            GenerateItem(pChar, sInfo.vecGenItem, sResult);

            if(sResult.IsNull())
            {
                continue;
            }

            m_vecResult.push_back(sResult);
        }
    }
}


/*
Callback_CalculateGenProb
Callback_CalculateGenItemRate
Callback_CalculateGenResult
Callback_GenItemLog
*/
void GLMaterial::GenerateItem(GLChar* pChar, SGENINFO::VEC_SPGENITEM& vecGenItem, OUT SGENRESULT& sResult, bool bParty)
{   
    float fRate    = sc::Random::getInstance().GetFloatPercent();
    float fNowRate = 0.0f;

    SGENINFO::VEC_SPGENITEM_ITER iter = vecGenItem.begin();
    for (; iter!=vecGenItem.end(); ++iter)
    {
        SGENITEMBASE* pItem = (*iter).get();

        if (!pItem)
        {
            continue;
        }

        if (!pItem->IsGen(static_cast<GLCHARLOGIC *>(pChar)))
        {
            continue;
        }

        float fItemRate = pItem->fRate;
        
        if ( (fNowRate < fRate) && (fRate < fNowRate+fItemRate) )
        {
            pItem->GetResult(sResult);

            break;
        }

        fNowRate += fItemRate;
    }
}


#if defined( _ITEM_GEN_060417_H_ ) || defined( _QUESTION_ITEM_GEN_060417_H_ )
DWORD GLMaterial::GenerateGathering_20060417( DWORD dwGaeaID, bool& bDrop )
{
	GLLandMan* const pLand( GetLandMan() );
	if ( pLand == NULL )
		return E_FAIL;

	EMGROUP emItemHoldGrp(EMGROUP_ONE);
	DWORD dwItemHoldID(GAEAID_NULL);

	// ȹ�� ���� �ɼ� ����.
	//

	if ( dwGaeaID == GAEAID_NULL ) return FALSE;

	PGLCHAR pChar = m_pGaeaServer->GetChar(dwGaeaID);
	if (!pChar)
	{
		// ��� �� �α� ����
		// ĳ���Ͱ� �������� ����. ������ return �ؾ������� �α׸� ���ؼ� �߰��� ���̹Ƿ� �ϴ� �α׸� ����� ���� �߽߰� ó��
		sc::writeLogError( sc::string::format( "Can not find GetCharByDbNum : %1% on GenerateGathering_20060417 ", dwGaeaID));
        return FALSE;
	}

	float fGEN_RATE = 1.0f;
	bool bgenitemhold = m_pGaeaServer->IsGenItemHold();
	if (bgenitemhold == false)
	{
		dwItemHoldID = GAEAID_NULL;
	}
	else 
	{
		emItemHoldGrp = EMGROUP_ONE;
		dwItemHoldID = dwGaeaID;			
	}

	float fGenRate = 1.0f; 

	fGenRate = CalculateItemGenRate( fGenRate, pChar, false );
	
	SGENITEM_060417 &sGenItem = m_pCrowData->m_sGenItem;

	if ( sGenItem.ISGEN(fGenRate) )
	{
		WORD wGenNum = sGenItem.SELECTNUM();
		
		for ( WORD i=0; i<wGenNum; ++i )
		{
			bool bSEPCID = false;
			DWORD dwSpecID = sGenItem.SELECTSPECID(fGEN_RATE, bSEPCID);
			SNATIVEID sNID;
			
			if (dwSpecID==SGENITEM_060417::EMSPEC_NULL)
                return FALSE;
			
			if (bSEPCID)
                sNID = GLItemMan::GetInstance().RendomGenSpecID ( dwSpecID );
			else
                sNID = dwSpecID;
			
			if (sNID==NATIVEID_NULL())
                return FALSE;
		
			const SITEM* pItemGather = GLogicData::GetInstance().GetItem( sNID );
			if (!pItemGather)
                return FALSE;

            GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();

            SITEMCUSTOM sITEM_NEW;
            sITEM_NEW.sNativeID = sNID;
            sITEM_NEW.tBORNTIME = CTime::GetCurrentTime().GetTime();
            sITEM_NEW.cGenType = EMGEN_GATHERING;
            //sITEM_NEW.cChnID = (BYTE) m_pGaeaServer->GetServerChannel();
            //sITEM_NEW.cFieldID = (BYTE) m_pGaeaServer->GetFieldSvrID();
            sc::SeqUniqueGUID::Instance()->getGUID( sITEM_NEW.guid );

			CItemDrop cDropItem;
			cDropItem.sItemCustom = sITEM_NEW;
			if ( pChar->IsInsertToInven ( &cDropItem ) )
			{
				pChar->InsertToInven ( &cDropItem, true, false, true, false, true );
				GLItemLimit* pItemLimit = m_pGaeaServer->GetItemLimit();
				// ��� �� �α� ����
				pItemLimit->LogItemExchange ( sITEM_NEW, gamelog::ID_MOB, m_pCrowData->sNativeID.dwID, gamelog::ID_CHAR, pChar->m_CharDbNum, 
					ITEM_ROUTE_GATHERING, sITEM_NEW.wTurnNum, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_ADD_ITEM_BY_GATHERING, false );
			}
			else
			{
				bDrop = true;
				pLand->DropItem ( m_vPosition, &(cDropItem.sItemCustom), emItemHoldGrp, dwItemHoldID );
			}
		}
	}
	
	return TRUE;	
}

#endif
