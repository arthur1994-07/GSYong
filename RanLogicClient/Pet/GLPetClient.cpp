#include "../pch.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/GlobalParam.h"

#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../Land/GLLandManClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Strike/GLStrikeM.h"
#include "../GLGaeaClient.h"
#include "./GLPetClient.h"


#include "../../SigmaCore/DebugInclude.h"

//  ������ �ݱ� �õ� ��û ���� �ð�
static const float fPET_PICKUP_ITEM_TIME = 0.5f;

// Q�ڽ� ��� ����� �ð�;
static const float fPET_GETQBOX_DISABLE_MSG = 300.0f;

GLPetClient::GLPetClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
	, m_pd3dDevice(NULL)
    , m_vDir(D3DXVECTOR3(0,0,-1))
    , m_vDirOrig(D3DXVECTOR3(0,0,-1))
    , m_vPos(D3DXVECTOR3(0,0,0))
    
    ,
	m_vMax(6,20,6),
	m_vMin(-6,0,-6),
	m_fHeight(20.f),

	m_vMaxOrg(6,20,6),
	m_vMinOrg(-6,0,-6),

	m_pOwner(NULL),

	m_bValid(false),
	m_bReadyActive(false),
	m_dwActionFlag(0),
	m_fIdleTimer(0.0f),
	m_fAttackTimer(0.0f),
    m_fPickItemTimer(0.0f),
	m_fQBoxLockNotifyTimer(fPET_GETQBOX_DISABLE_MSG),
	m_fQboxPickableTimer(0.0f),
	m_sPetCrowID(NATIVEID_NULL()),
	m_wColorTEMP(0),
	m_wStyleTEMP(0),
	m_bEnableSytle(FALSE),
	m_bEnableColor(FALSE),
	m_vRandPos(0.0f,0.0f,0.0f)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
	, m_pSkinChar(new DxSkinChar)	// m_pSkinChar �� NULL �� �������� �ʴ´ٴ� ������ ����.
{
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );
	m_vecDroppedItems.reserve(50);
	m_sSkillTARGET.RESET ();
	m_SKILLDELAY.clear();

	m_sPetSkinPackState.Init();

	m_vecActiveSkillInfo.clear();
}

GLPetClient::~GLPetClient(void)
{
	SAFE_DELETE ( m_pSkinChar );
}

HRESULT GLPetClient::Create ( const PGLPET pPetData, D3DXVECTOR3 vPos, D3DXVECTOR3 vDir, boost::shared_ptr<NavigationMesh> spNavi, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !spNavi || !pd3dDevice ) 
		return E_FAIL;

	m_pd3dDevice = pd3dDevice;

	ASSIGN ( *pPetData );

	m_vPos = vPos;
	m_vDir = vDir;
	m_pOwner = m_pGaeaClient->GetCharacter ();

	m_actorMove.Create ( spNavi, m_vPos );

	if ( FAILED ( SkinLoad ( pd3dDevice ) ) )
		return E_FAIL;

	UpdateSuit ( TRUE );

	// Note : 1.AABB Box�� �����´�. 2.���̸� ����� ���´�.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;
	
	m_bValid = true;
	m_bReadyActive = false;

	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	// Note : �ʱ�ȭ
	m_vecActiveSkillInfo.clear();
	//m_vecActiveSkillInfo._Mylast = m_vecActiveSkillInfo._Myfirst;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_OpenWindow );

	return S_OK;
}

void GLPetClient::Delete()
{
	m_actorMove.Stop ();
	m_actorMove.Release ();

	m_pd3dDevice = NULL;
	m_pOwner	 = NULL;
	m_vDir	     = D3DXVECTOR3 (0,0,-1);
	m_vPos	     = D3DXVECTOR3(0,0,0);
	m_vDirOrig   = D3DXVECTOR3(0,0,-1);
	m_bValid	 = false;
	m_bReadyActive	 = false;
	ReSetAllSTATE ();
	SetSTATE ( EM_PETACT_STOP );

	m_sSkillTARGET.RESET ();
	m_fIdleTimer = 0.0f;
	m_fAttackTimer = 0.0f;
	m_fPickItemTimer = 0.0f;
	SecureZeroMemory ( m_wAniSub,sizeof(WORD)*3 );

	m_sPetCrowID = NATIVEID_NULL();

	m_SKILLDELAY.clear();

	m_vecDroppedItems.clear();

	RESET ();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Pet_CloseWindow );
}

HRESULT GLPetClient::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_sPetID == NATIVEID_NULL() )
	{
		GASSERT ( 0&&"m_sPetID==NATIVEID_NULL()" );
		return E_FAIL;
	}

	SNATIVEID loadPetID = m_sPetID;

	if( IsUsePetSkinPack() )
	{
		loadPetID = m_sPetSkinPackState.petSkinMobID;
	}
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( loadPetID );
	if ( !pCrowData )
	{
		GASSERT ( pCrowData );
		return E_FAIL;
	}

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		pd3dDevice );

	if ( !pSkinChar )
	{
		GASSERT ( pSkinChar );
		return E_FAIL;
	}

	SAFE_DELETE(m_pSkinChar);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	

	// ���� �� ���ϸ��̼��߿� �÷��̰� �Ұ����� �ִϸ��̼��� ã�´�.
	if( IsUsePetSkinPack() )
	{
		if( !m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE )  ) m_sPetSkinPackState.bNotFoundGuard = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_RUN, AN_SUB_NONE ) )		 m_sPetSkinPackState.bNotFoundRun = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_WALK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundWalk = TRUE;	
//		m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_NONE ) )
		if( !m_pSkinChar->GETANIAMOUNT( AN_ATTACK, AN_SUB_NONE ) )	 m_sPetSkinPackState.bNotFoundAttack = TRUE;
		if( !m_pSkinChar->GETANIAMOUNT( AN_GESTURE, AN_SUB_04 ) )	 m_sPetSkinPackState.bNotFoundSad	 = TRUE;
	}

	
	m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

	if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, AN_SUB_NONE ) == 0 )
	{
		m_pSkinChar->DEFAULTANI();
	}

	return S_OK;
}

void GLPetClient::DeleteSkin()
{
	// [shhan][2014.12.03] ����� �� �� ��� �ִ°Ŷ� �־� ���´�.
	//						m_pSkinChar �� NULL �� ���ٴ� ������ ����ִ�.
	SAFE_DELETE ( m_pSkinChar );
	m_pSkinChar = new DxSkinChar;
}

// ���� �̵��ϴ� ���̴� �� �������� ���̴� �������� ����.
void GLPetClient::LandShredPtrRefSub()
{
	// ����ϰ� ���� ���鵵�� �Ѵ�.
	m_spWorldLight = std::tr1::shared_ptr<ActorWorldLight>(new ActorWorldLight);
	m_spNameCollision = std::tr1::shared_ptr<ActorNameCollision>(new ActorNameCollision(EMCC_CULL_NONE));
}

HRESULT GLPetClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	return S_OK;
}

HRESULT GLPetClient::UpdateSuit( BOOL bChangeStyle )
{

	if( IsUsePetSkinPack() ) return S_OK;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return E_FAIL;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return E_FAIL;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// ��Ÿ�� ����
	if ( bChangeStyle )
	{
		if ( sSTYLE.wSTYLENum > m_wStyle )
		{
			std::string strSTYLE_CPS = sSTYLE.strSTYLE_CPS[m_wStyle];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	���� ���� ��Ų.

			if ( pCharPart && strcmp( strSTYLE_CPS.c_str(), pCharPart->GetFileName() ) )
			{
				m_pSkinChar->SetPiece( strSTYLE_CPS.c_str(), m_pd3dDevice, 0L, 0, EMSCD_ALL_THREAD );
			}
		}
	}

	// �÷� ���� (�÷�����ī�� ������̸� �������� �ʴ´�)
	if ( !m_bEnableColor )
        m_pSkinChar->SetHairColor( m_wColor );

	// �Ǽ����� ��� ����
	for ( WORD i = 0; i < ACCETYPESIZE; ++i )
	{
		SITEM* pSlotItem = GLogicData::GetInstance().GetItem( m_PutOnItems[i].GetNativeID() );
		if ( pSlotItem )
            m_pSkinChar->SetPiece( pSlotItem->GetPetWearingFile(), m_pd3dDevice, 0, 0, EMSCD_ALL_THREAD );
	}

	return S_OK;
}

void GLPetClient::SetMoveState ( BOOL bRun = TRUE )
{
	if ( bRun ) 
	{
		m_actorMove.SetMaxSpeed ( m_fRunSpeed );
		SetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			if( IsUsePetSkinPack() && m_sPetSkinPackState.bNotFoundRun )
			{
				m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );				
			}else{
				m_pSkinChar->SELECTANI ( AN_RUN, AN_SUB_NONE );
			}
		}
	}
	else
	{
		m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
		ReSetSTATE ( EM_PETACT_RUN );
		if ( IsSTATE ( EM_PETACT_MOVE ) )
		{
			m_pSkinChar->SELECTANI ( AN_WALK, AN_SUB_NONE );
		}
	}

	// �̵����¸� �����ϴ� �޽��� �߼ۿ��
	GLMSG::SNETPET_REQ_UPDATE_MOVESTATE NetMsg(m_dwActionFlag);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLPetClient::ReSetAllSTATE (void)
{
	BOOL bIsQboxWait(IsSTATE(EM_PETACT_QBOXPICK_WAIT));
	m_dwActionFlag = 0;
	if ( bIsQboxWait )
		SetSTATE(EM_PETACT_QBOXPICK_WAIT);
}

void GLPetClient::SetAttackState( const D3DXVECTOR3& vTargetPos )
{
	// ���� ��ȯ
	D3DXVECTOR3 vDirection = vTargetPos - m_vPos;
	D3DXVec3Normalize( &m_vDir, &vDirection );
	m_vDir = vDirection;

	if( FALSE == IsSTATE( EM_PETACT_ATTACK ) )
	{
		ReSetAllSTATE();
		SetSTATE( EM_PETACT_ATTACK );
		m_pSkinChar->SELECTANI( AN_ATTACK, AN_SUB_NONE );
	}
	else
	{
		m_actorMove.Stop();
		m_pSkinChar->SELECTANI( AN_GUARD_N, AN_SUB_NONE );
	}

	m_fAttackTimer = 0.0f;
}

HRESULT	GLPetClient::UpdatePetState ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vOwnerPos, vRandPos, vDist;
	float fDist;

	static float fTimer(0.0f);
	static WORD wSubAni(0);
	static float fIdleTimer(0.0f);

    m_fPickItemTimer += fElapsedTime;
	if( GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock ) == false &&
		this->IsActiveSkill(SKILL::EMFOR_PET_GETQBOX) )
	{
		m_fQBoxLockNotifyTimer += fElapsedTime;
	}

	GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
	if ( IsSTATE(EM_PETACT_QBOXPICK_WAIT) )
	{
		m_fQboxPickableTimer += fElapsedTime;
		if ( m_fQboxPickableTimer >= GLCONST_CHAR::fQBoxPickableTime )
		{
			m_fQboxPickableTimer = 0.0f;
			ReSetSTATE(EM_PETACT_QBOXPICK_WAIT);
		}
	}


	// ���� ���ݸ�� ���̸�
	if ( IsSTATE ( EM_PETACT_ATTACK ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			if( true == m_pSkinChar->IsAni( AN_ATTACK ) )
			{
				m_fAttackTimer += fElapsedTime;
				if ( m_pSkinChar->GETCURANITIME() < m_fAttackTimer )
					ReqStop ( true );
			}
		}
	}

	// �����Ҷ�
	if ( IsSTATE ( EM_PETACT_SAD ) ) return S_OK;

	// ����������
	if ( IsSTATE ( EM_PETACT_STOP ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
		}
		else
		{
			m_vRandPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

			m_fIdleTimer += fElapsedTime;
			if ( m_fIdleTimer > 2.0f )
			{
				ReSetSTATE ( EM_PETACT_STOP );
				SetSTATE ( EM_PETACT_FUNNY );
				MakeAniSubKey ( fTime );

				// Funny Action
				m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

				GLMSG::SNETPET_REQ_FUNNY NetMsg(m_wAniSub[wSubAni]);
    			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

				m_fIdleTimer = 0.0f;
			}
		}

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	//	[shhan][2016.01.19] ���� Funny Action �� ���ý� Sub ���� AN_SUB_00, AN_SUB_01, AN_SUB_02 �� �ƴ� 
	//						AN_SUB_01, AN_SUB_02, AN_SUB_03 ���� �ؾ� ���� �۵��մϴ�.
	//						
	//						3 �� �ϳ��� �����ְ� AN_SUB_00 �� ������ �Ǹ� ����Ÿ��� ������ �߻��մϴ�.
	//						SELECTANI ���� AN_SUB_00 �� ���� �����ؼ� ������ ���ְ�,
	//						���� Frame ���� ���ǹ� �˻�� ���� Idle �� ���õ�.
	//////////////////////////////////////////////////////////////////////////
	// FUNNY �׼����̸�
	if ( IsSTATE ( EM_PETACT_FUNNY ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			ReqGoto ( GetRandomOwnerTarget (), false );
			wSubAni = 0;
		}
		else
		{
			const DxSkinAnimation* pANIMCON = m_pSkinChar->GETCURANIM ();

            if ( !pANIMCON )
            {
                //GASSERT( 0 && "HRESULT GLPetClient::UpdatePetState���� m_pSkinChar->GETCURANIM()�� NULL�Դϴ�." );

				// �����ϰ� ����� ���� Idle �� �����Ѵ�.
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

                return E_FAIL;
            }

			// Funny �ִϸ��̼��� �������� Idle �ִϸ��̼� ����
			if ( pANIMCON->m_MainType == AN_GESTURE && pANIMCON->m_SubType == m_wAniSub[wSubAni] )
			{
				if ( m_pSkinChar->ISENDANIM () )
				{
					m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );

					GLMSG::SNETPET_REQ_STOP NetMsg(m_dwGUID, EM_PETACT_STOP, m_vPos, true );
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

					if ( ++wSubAni > 2 ) 
					{
						// Funny �ִϸ��̼� ���� �缳��
						wSubAni = 0;
						MakeAniSubKey ( fTime );
					}

					fIdleTimer = 0.0f;
				}
			}
			// Idle �ִϸ��̼��� ������ Funny �ִϸ��̼� ����
			else if ( pANIMCON->m_MainType == AN_GUARD_N && pANIMCON->m_SubType == AN_SUB_NONE )
			{
				fIdleTimer += fElapsedTime;
				// �ִϸ��̼� ������ ������ �����Ǿ� �����Ƿ� ���� ������ �˼� �����Ƿ� �ð����� ������.
				if ( fIdleTimer > 3.0f )
				{
					// Funny Action
					m_pSkinChar->SELECTANI ( AN_GESTURE, (EMANI_SUBTYPE) m_wAniSub[wSubAni] );

					GLMSG::SNETPET_REQ_FUNNY NetMsg(m_wAniSub[wSubAni]);
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

					fIdleTimer = 0.0f;
				}
			}
			else	
			{
				// �����ϰ� ����� ���� Idle �� �����Ѵ�.
				m_pSkinChar->SELECTANI ( AN_GUARD_N, AN_SUB_NONE );
			}
		}
	}

/*
	// ������ ����/��ų ���϶�
	if ( m_pOwner->IsACTION ( GLAT_ATTACK ) || m_pOwner->IsACTION ( GLAT_SKILL ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();
		vDist = m_vPos - vOwnerPos;
		fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
		}
	}
*/

	// �̵��� ������ ��ٸ���
	if ( IsSTATE ( EM_PETACT_WAIT ) )
	{
		vOwnerPos = m_pOwner->GetPosition ();

		if ( m_pOwner->isAction ( GLAT_IDLE ) )
		{
			D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
			vOwnerPos = m_pOwner->GetPosition ();
			vOwnerPos += vRandPos;
			ReqGoto ( vOwnerPos, false );
			return S_OK;
		}

		vDist = vOwnerPos - m_vPos;
		float fDist = D3DXVec3Length(&vDist);
		if ( fDist > GLCONST_PET::fWalkArea )
		{
			D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
			D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
			vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

			ReqGoto ( vOwnerPos, false );
		}

		m_fIdleTimer = 0.0f;
	}

	if ( IsSTATE ( EM_PETACT_MOVE ) )
	{
		// �������� �ֿ췯 �޸���
		if ( IsSTATE ( EM_PETACT_RUN_PICKUP_ITEM ) )
		{
			vDist = m_vPos - m_sSkillTARGET.vPos;
			fDist = D3DXVec3Length(&vDist);
			if ( !m_actorMove.PathIsActive() )
			{
                if ( m_fPickItemTimer > fPET_PICKUP_ITEM_TIME ) 
                {
				    BOOL bPickup = FALSE;
                    m_fPickItemTimer = 0.0f;
					if ( m_sSkillTARGET.emCrow==CROW_ITEM )
					{
						std::tr1::shared_ptr<CItemClientDrop> spItemDrop;
						//  [2/12/2015 gbgim];
						// ����ó���� ������ ��ȣ�� ������;; �Ʒ����� ó����;
						if ( pLAND && (spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId)) )
						{
							SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
							if ( pItem && pItem->sBasicOp.emItemType==ITEM_QITEM )
							{
								if ( this->IsQBoxPickableTime() )
								{
									SetSTATE(EM_PETACT_QBOXPICK_WAIT);
									bPickup = TRUE;
								}
							}
							else
							{
								bPickup = TRUE;
							}
						}
					}
					else if ( m_sSkillTARGET.emCrow==CROW_MONEY )
					{
						bPickup = TRUE;
					}

					// �������� �ݴ´�.
					if ( bPickup == TRUE )
						m_pOwner->ReqFieldTo ( m_sSkillTARGET, true );  

					ReqStop ();

				    return S_OK;
                }
			}
		}
		else
		{
			vOwnerPos = m_pOwner->GetPosition ();
			vDist = m_vPos - vOwnerPos;
			fDist = D3DXVec3Length(&vDist);

			STARGETID sTarget = m_pOwner->GetActionID ();

			if ( !m_actorMove.PathIsActive() )
			{
				ReqStop ();
				return S_OK;
			}

			// 2�ʿ� �ѹ��� �̵��޽����� �����Ѵ�. (�̺κ��� �����غ��� �ҵ��ϴ�)
			fTimer += fElapsedTime;
			if ( fTimer > 2.0f && m_pOwner->isAction ( GLAT_MOVE ) )
			{
				// D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
				vOwnerPos = m_pOwner->GetPosition ();

				D3DXVECTOR3 vOwnerBack = m_vPos - vOwnerPos;
				D3DXVec3Normalize ( &vOwnerBack, &vOwnerBack );
				vOwnerPos += (vOwnerBack*GLCONST_PET::fOwnerDistance);

				ReqGoto ( vOwnerPos, false );
				fTimer = 0.0f;
			}
			
			// ������ ������ �Ÿ��� ���� �ٰų� �ȴ´�
			// [����] ���� �ڵ忡�� �̹� ���� ������ ���¸� ���������Ƿ� 
			//		   ������ �ȴ� ���¿� ���� ó���� ���ָ� �ȴ�.
			if ( !m_pOwner->IsActState ( EM_ACT_RUN ) )
			{
				vDist = m_vPos - vOwnerPos;
				float fDist = D3DXVec3Length(&vDist);

				// ���ΰ� �Ÿ��� �ʹ� �־����� �����Ÿ��� �����ϱ� ���� �޸���
				if ( fDist > GLCONST_PET::fRunArea && !IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					m_actorMove.SetMaxSpeed ( m_fRunSpeed );
					SetSTATE ( EM_PETACT_RUN_CLOSELY );
					SetMoveState ();
				}
				else if ( fDist <= GLCONST_PET::fWalkArea && IsSTATE ( EM_PETACT_RUN_CLOSELY ) )
				{
					m_actorMove.SetMaxSpeed ( m_fWalkSpeed );
					ReSetSTATE ( EM_PETACT_RUN_CLOSELY );
					SetMoveState ( FALSE );
				}
			}
		}
		m_fIdleTimer = 0.0f;
	}

	return S_OK;
}

EM_FIELDITEM_STATE GLPetClient::IsInsertableInven ( STARGETID sTar )
{
	GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
	if (!pLAND)
        return EM_FIELDITEM_STATE_GENERALFAIL;

	if ( m_sSkillTARGET.emCrow==CROW_ITEM )
	{
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId);
		if (!spItemDrop)
            return EM_FIELDITEM_STATE_REMOVED;

		SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
		if (!pItem)
            return EM_FIELDITEM_STATE_GENERALFAIL;

		BOOL bOk = m_pOwner->IsInsertToInven(spItemDrop);
		if (!bOk)
            return EM_FIELDITEM_STATE_INSERTFAIL;
	}
	else if ( m_sSkillTARGET.emCrow==CROW_MONEY )
	{
        std::tr1::shared_ptr<CMoneyClientDrop> spMoney = pLAND->GetMoney(m_sSkillTARGET.GaeaId);
		if (!spMoney)
            return EM_FIELDITEM_STATE_REMOVED;
	}

	return EM_FIELDITEM_STATE_INSERTOK;
}

void GLPetClient::ProceedSkill ()
{
	PETACTIVESKILLLIST_VEC& vecActiveSkillList = UpdateActiveSkillInfo();
	DWORD				    dwGetItemFlag      = GetSkillGetItemFlag();

	//	��ų ���� ������.
	for ( WORD i=0; i<vecActiveSkillList.size(); ++i )
	{
		SPETACTIVESKILLINFO& sPetActiveSkillInfo = vecActiveSkillList[i];

		SKILL::EMTYPES emType = sPetActiveSkillInfo.emTypes;
		WORD		   wCount = sPetActiveSkillInfo.wCount;  // Note : ��ø ��
		
		switch ( emType )
		{

		// ţ���Կ� ������ �Ծ��ش�. (�������ӿ� �ѹ��� ���� �Ծ��ش�!)
		case SKILL::EMFOR_PET_AUTOMEDISON:
			{
//	������ ���μ��� �̵� ���״�.
//	���� ������ Ȯ�εǸ� ���߿� ��������~~
/*
				DWORD dwMaxOwnersHP = m_pOwner->GETMAXHP ();
				DWORD dwMaxOwnersMP = m_pOwner->GETMAXMP ();
				DWORD dwMaxOwnersSP = m_pOwner->GETMAXSP ();
				DWORD dwHPVar = dwMaxOwnersHP - m_pOwner->GETHP ();
				int   nMPVar  = dwMaxOwnersMP - m_pOwner->GETMP ();
				int   nSPVar  = dwMaxOwnersSP - m_pOwner->GETSP ();


				// Note : ��ȯ���� �Ǹ� üũ�Ͽ� ���� ���� ��ȯ���� �Ǹ� ���󰣴�
				//
				if ( !m_pOwner->m_dwSummonGUIDList.empty() )
				{
					FLOAT fOwnerHpRatio = (((FLOAT)dwMaxOwnersHP-dwHPVar)/(FLOAT)dwMaxOwnersHP) * 100.0f;

					SUMMONID_ITER SummonIter    = m_pOwner->m_dwSummonGUIDList.begin();
					SUMMONID_ITER SummonIterEnd = m_pOwner->m_dwSummonGUIDList.end();

					for ( ; SummonIter!=SummonIterEnd; ++SummonIter )
					{
						PGLSUMMONCLIENT pSummon = m_pGaeaClient->GetSummon((*SummonIter));

						if ( !pSummon || !pSummon->IsValidBody() )
							continue;

						DWORD dwMaxSummonHP  = pSummon->GETMAXHP();
						DWORD dwSummonHP     = pSummon->GETHP();
						FLOAT fSummonHpRatio = ((FLOAT)dwSummonHP/(FLOAT)dwMaxSummonHP) * 100.0f;

						if ( fSummonHpRatio < fOwnerHpRatio )
						{
							// Note : ������ HP�� ���� �����
							dwMaxOwnersHP = dwMaxSummonHP;
							dwHPVar       = (dwMaxSummonHP - dwSummonHP);
						}
					}
				}

				// Q, W, E, A, S, D
				for ( WORD wSLOT = 0; wSLOT < EMACTIONQUICK_SIZE; ++wSLOT )
				{
					const SACTION_SLOT &sACTION = m_pOwner->m_sACTIONQUICK[wSLOT];
					if ( !sACTION.VALID() )	continue;

					switch ( sACTION.wACT )
					{
					case EMACT_SLOT_DRUG:
						{
							SINVENITEM* pInvenItem = m_pOwner->m_cInventory.FindItem ( sACTION.sNID );
							if ( !pInvenItem )	continue;

							SITEM* pITEM = GLogicData::GetInstance().GetItem ( sACTION.sNID );
							if ( !pITEM ) continue;

							if ( pITEM->sBasicOp.emItemType!=ITEM_CURE ) continue;

							if ( m_pOwner->IsCoolTime( pInvenItem->sItemCustom.sNativeID ) ) continue;
							
							// HP �����̸�
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_HP:
							case ITEM_DRUG_HP_MP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_CURE:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersHP <= (float)dwHPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= dwHPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// HP�� 20% ������ �������� ������ �Դ´�.
									if ( (float)(dwMaxOwnersHP-dwHPVar)/dwMaxOwnersHP*100.0f < GLCONST_PET::fMaginotLineHP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};

							// MP �����̸�
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_MP:
							case ITEM_DRUG_HP_MP:
							case ITEM_DRUG_MP_SP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersMP <= float(nMPVar) )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= nMPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// MP�� 20% ������ ��������
									if ( (float)(dwMaxOwnersMP-nMPVar)/dwMaxOwnersMP*100.0f < GLCONST_PET::fMaginotLineMP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};

							// SP �����̸�
							switch ( pITEM->sDrugOp.emDrug )
							{
							case ITEM_DRUG_SP:
							case ITEM_DRUG_MP_SP:
							case ITEM_DRUG_HP_MP_SP:
							case ITEM_DRUG_HP_MP_SP_CURE:
								{
									if ( pITEM->sDrugOp.bRatio )
									{
										if ( pITEM->sDrugOp.wCureVolume/100.0f*dwMaxOwnersSP <= (float)nSPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}
									else
									{
										if ( pITEM->sDrugOp.wCureVolume <= nSPVar )
										{
											m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
											return;
										}
									}

									// SP�� 20% ������ ��������
									if ( (float)(dwMaxOwnersSP-nSPVar)/dwMaxOwnersSP*100.0f < GLCONST_PET::fMaginotLineSP )
									{
										m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
										return;
									}
								}
							};
							
							// �̻�ġ��
							if ( pITEM->sDrugOp.emDrug == ITEM_DRUG_CURE )
							{
								if ( m_pOwner->ISSTATEBLOW() )
								{
									m_pOwner->ReqInvenDrug ( pInvenItem->wPosX, pInvenItem->wPosY, true );
									return;
								}
							}
						}
					};
				} // end of for
			*/
			}
			break;

			// �������� �ݴ� ��ų
		case SKILL::EMFOR_PET_GETALLITEMS:
		case SKILL::EMFOR_PET_GETRAREITEMS:
		case SKILL::EMFOR_PET_GETPOTIONS:
		case SKILL::EMFOR_PET_GETSTONE:
		case SKILL::EMFOR_PET_GETMONEY:
		case SKILL::EMFOR_PET_GETQBOX:
		case SKILL::EMFOR_PET_GETMATERIALS:
			{
			}			
			break;

			// ������ ��ų���� ������ ���¸� �������ش�.
		case SKILL::EMFOR_PET_HEAL:
		case SKILL::EMFOR_PET_SUPPROT:
		case SKILL::EMFOR_PET_BACKUP_ATK:
		case SKILL::EMFOR_PET_BACKUP_DEF:
			{
			}
			break;

		case SKILL::EMFOR_PET_PROTECTITEMS:
			break;

		default: break;
		};
	}

	// Note : ������ �ݱ⿡ ���� ���μ���
	//		  ��û ���� �ƴϰų� ��û�� ������.
    if ( dwGetItemFlag != EMPETSKILL_NONE && m_pOwner->IsValidBody() )
	{
		GLLandManClient* pLAND = m_pGaeaClient->GetActiveMap();
		if ( pLAND == NULL )
			return;

		// �ֺ��� �������� ������ �޷�����.
		if ( !IsSTATE(EM_PETACT_RUN_PICKUP_ITEM) )
		{
			if ( !m_vecDroppedItems.empty() )
			{
				// �ϳ� ������
				m_sSkillTARGET = m_vecDroppedItems.back();
				m_vecDroppedItems.pop_back ();

				// �������� �κ��� ���� �� �ִ��� �˻�
				EM_FIELDITEM_STATE emResult = IsInsertableInven ( m_sSkillTARGET );

				// ť�ڽ��� ������ ȹ���� �ƴϱ⶧���� ť�ڽ��� ȹ���Ϸ����Ѵٸ�;
				// �κ��丮 �˻�� �����Ѵ�;
				if ( emResult == EM_FIELDITEM_STATE_INSERTFAIL && (dwGetItemFlag & EMPETSKILL_GETQBOX))
				{
					std::tr1::shared_ptr<CItemClientDrop> spItemDrop;
					if ( (spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId)) )
					{
						SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
						if ( pItem && pItem->sBasicOp.emItemType == ITEM_QITEM )
						{
							emResult = EM_FIELDITEM_STATE_INSERTOK;
						}
					}
				}

				switch ( emResult )
				{
					// ��Ʈ��ũ �����̷� �������� �ʴ� �������� ��� �õ��Ұ��
				case EM_FIELDITEM_STATE_REMOVED:
					break;

					// �κ��丮 ���� ����
				case EM_FIELDITEM_STATE_INSERTFAIL:
					{
                        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLAND->GetItem(m_sSkillTARGET.GaeaId);
						if (!spItemDrop)
                            break;

						SITEM* pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
						if (!pItem)
                            break;

						// Note : �ش�Ǵ� ���� ��ų�� ����Ѵ�.
						for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
						{
							PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
							if ( !pSkill )	continue;

							SKILL::EMTYPES emSkillType = pSkill->m_sAPPLY.emBASIC_TYPE;

							bool bCancel = false;

							switch ( emSkillType )
							{
							case SKILL::EMFOR_PET_GETALLITEMS:																												   bCancel = true; break;
							case SKILL::EMFOR_PET_GETRAREITEMS: if (pItem->sBasicOp.emLevel    == EMITEM_LEVEL_HIGH )																    bCancel = true; break;
							case SKILL::EMFOR_PET_GETPOTIONS:	if (pItem->sBasicOp.emItemType == ITEM_CURE	|| pItem->sBasicOp.emItemType == ITEM_EFFECT_RELEASE_CURE )			bCancel = true; break;
							case SKILL::EMFOR_PET_GETSTONE:		if (pItem->sBasicOp.emItemType == ITEM_GRINDING || pItem->sBasicOp.emItemType == ITEM_MATERIALS )	bCancel = true; break;
							case SKILL::EMFOR_PET_GETQBOX:		if (pItem->sBasicOp.emItemType == ITEM_QITEM )												bCancel = true; break;
							case SKILL::EMFOR_PET_GETMATERIALS:	if (pItem->sBasicOp.emItemType == ITEM_MATERIALS )												bCancel = true; break;
							}
	
							if ( bCancel )
							{
								// Note : ��ų�� ����ع�����.
								ReqChangeActiveSkill ( NATIVEID_NULL(), i );

								m_sSkillTARGET.RESET ();
								m_vecDroppedItems.clear();
								m_pGaeaClient->PrintConsoleTextDlg( ID2GAMEINTEXT("EMPET_NOTENOUGHINVEN") );
							}
						}
					}
					break;;
					// �κ��丮 ���� ����
				case EM_FIELDITEM_STATE_INSERTOK:
					{
						D3DXVECTOR3 vDist, vDir, vTar;
						vDist = m_sSkillTARGET.vPos - m_vPos;
						D3DXVec3Normalize ( &vDir, &vDist );
						float fDist = D3DXVec3Length ( &vDist ) - 10.0f; // �ݱ� ������ �Ÿ���ŭ �Ÿ� ����
						vTar = vDir * fDist;
						vTar = m_vPos + vTar;
						ReqGoto ( vTar, true );
					}
					break;
					// �Ϲ� ������ ���
				case EM_FIELDITEM_STATE_GENERALFAIL:
					{
					}
					break;
				}
			}
			// ���� ������ ������ ������ ��û�Ѵ�.
			else 
			{
				ReqGetItem ( dwGetItemFlag ); 
			}
		}
	}
	else
	{
		if ( !m_vecDroppedItems.empty() )
		{
			m_vecDroppedItems.clear();
		}
	}
}


bool GLPetClient::CheckSkill ( SNATIVEID sSkillID, WORD wSlot )
{
	PETSKILL_MAP_CITER learniter = m_ExpSkills.find ( sSkillID.dwID );
	if ( learniter==m_ExpSkills.end() )
	{
		// ����� ���� ��ų
		return false;
	}

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter!=m_SKILLDELAY.end() )
	{
		// ��ų������
		return false;
	}

	if ( wSlot >= m_wMaxSkillSlot || wSlot >= MAX_PETSKILLSLOT )
	{
		// �߸��� ��ų ����
		return false;
	}

	return true;
}

float GLPetClient::GetSkillDelayPercent ( SNATIVEID sSkillID )
{
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					     return 0.0f;

	PETDELAY_MAP_ITER delayiter = m_SKILLDELAY.find ( sSkillID.dwID );
	if ( delayiter==m_SKILLDELAY.end() ) return 0.0f;

	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[0];
	float fSkillDelay = sSKILL_DATA.fDELAYTIME;
	float fCurDelay = (*delayiter).second;

	if ( fSkillDelay == 0.0f ) fSkillDelay = 1.0f;

	return fCurDelay / fSkillDelay;
}

void GLPetClient::AccountSkill ( SNATIVEID sSkillID )
{
	PETSKILL_MAP_ITER iter = m_ExpSkills.find ( sSkillID.dwID );
	if ( iter == m_ExpSkills.end() ) return;
	PETSKILL sPetSkill = (*iter).second;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID );
	if ( !pSkill )					 return;
	SKILL::CDATA_LVL &sSKILL_DATA = pSkill->m_sAPPLY.sDATA_LVL[sPetSkill.wLevel];

	m_SKILLDELAY.insert ( std::make_pair(sSkillID.dwID, sSKILL_DATA.fDELAYTIME) );
}

void GLPetClient::UpdateSkillDelay ( float fElapsedTime )
{
	PETDELAY_MAP_ITER iter_del;

	PETDELAY_MAP_ITER iter = m_SKILLDELAY.begin ();
	PETDELAY_MAP_ITER iter_end = m_SKILLDELAY.end ();

	for ( ; iter!=iter_end; )
	{
		float &fDelay = (*iter).second;
		iter_del = iter++;

		fDelay -= fElapsedTime;
		if ( fDelay < 0.0f )	m_SKILLDELAY.erase ( iter_del );
	}
}

HRESULT GLPetClient::FrameMove( GLLandManClient* pLandManClient, float fTime, float fElapsedTime )
{
	if ( !m_bValid ) return S_OK;
	
	HRESULT hr(S_OK);

	UpdatePetState ( fTime, fElapsedTime );
	ProceedSkill ();
	// UpdateSkillDelay ( fElapsedTime ); ��ȹ�� �躴�� ��û���� ���� [06.10.09]

	// ============== Update about Actor (Begin) ============== /

	hr = m_actorMove.Update ( pLandManClient->GetvecLayerCollision(), fElapsedTime );
	if ( FAILED(hr) )	return E_FAIL;

	// ============== Update about Actor (end)   ============== /

	// ============== Update about Skin (Begin) ============== /

	D3DXMATRIX matTrans, matYRot, matScale;

	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	if( IsUsePetSkinPack() )
	{
		D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
		m_matTrans = matScale * matYRot * matTrans;
	}
	else
	{
		m_matTrans = matYRot * matTrans;
	}

	m_pSkinChar->SetPosition ( m_vPos );

	if( IsUsePetSkinPack() && IsSTATE ( EM_PETACT_MOVE ) )
	{
		float fSpeedGap1, fSpeedGap2, fTempTime, fTempElapsedTime;

		fSpeedGap1 = 1.0f / m_sPetSkinPackState.fPetScale;
		if( IsSTATE( EM_PETACT_RUN ) && m_sPetSkinPackState.bNotFoundRun ) fSpeedGap2 = m_fRunSpeed / m_fWalkSpeed;
		else															  fSpeedGap2 = 1.0f;

		fTempTime		 = fTime * fSpeedGap1 * fSpeedGap2;
		fTempElapsedTime = fElapsedTime * fSpeedGap1 * fSpeedGap2;

		m_pSkinChar->FrameMove ( fTempTime, fTempElapsedTime, FALSE, m_matTrans );
   	}
	else
	{
		m_pSkinChar->FrameMove ( fTime, fElapsedTime, FALSE, m_matTrans );
	}

	// ============== Update about Skin (End)   ============== /

	// ���� ��ġ ������Ʈ
	m_vPos = m_actorMove.Position();

	// ���� ������Ʈ
	D3DXVECTOR3 vMovement = m_actorMove.NextPosition();
	if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
	{
		D3DXVECTOR3 vDirection = vMovement - m_vPos;
		if ( !DxIsMinVector(vDirection,0.2f) )
		{
			D3DXVec3Normalize ( &vDirection, &vDirection );
			m_vDir = vDirection;
		}
	}

	//	Note : AABB ���.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;


	//////////////////////////////////////////////////////////////////////////
	//			Name Collision �� Thread �󿡼� Ȯ���ϵ��� �����Ѵ�.
	if ( pLandManClient )
	{
		// Thread ������ �ѱ��.
		m_spNameCollision->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), m_vPos + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );



		//////////////////////////////////////////////////////////////////////////
		//			ActorWorldLight �� Thread �󿡼� Ȯ���ϵ��� �����Ѵ�.
		// Thread ������ �ѱ��.
		m_spWorldLight->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddActorWorldLight( m_spWorldLight, m_vPos );
	}


	return S_OK;
}

HRESULT GLPetClient::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	

	// ���� �ǽ� ������ �Ųٷ� �׸���.
	// �ɸ��Ͱ� ���� ���̸� �굵 �������ϰ� ����;
	if ( m_pSkinChar )
	{
		D3DXMATRIX matScale, matRender;
		if( IsUsePetSkinPack() )
		{				
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;			
		}
		else
			matRender = m_matTrans;

		//m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE, FALSE, m_pOwner->m_bINVISIBLE );

		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // ���� ���ΰ�;

		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			if ( bHALF_VISIBLE )
			{
				DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
			}
			else
			{
				DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
			}
		}

		//if ( m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) )
		//{
		//	DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &m_vDir );
		//}
		//else
		//{
		//	DxEffcharDataMan::GetInstance().OutEffect ( m_pSkinChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		//}

		
		//m_pSkinChar->Render( pd3dDevice, TRUE, 1.f, matRender, FALSE, TRUE, TRUE, FALSE);

		RenderMyChar( pd3dDevice, matRender, TRUE, bHALF_VISIBLE );

		DxLandMan* pLandMan(NULL);
		if ( m_pGaeaClient )
		{
			GLLandManClient* pLandManClient = m_pGaeaClient->GetLandManClient();
			if ( pLandManClient )
			{
				pLandMan = pLandManClient->GetLandMan();
			}
		}

		//if ( pLandMan )
		//{
		//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(matRender._41,matRender._42,matRender._43) );
		//}
	}
	
	return S_OK;
}

HRESULT GLPetClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;



	// ���� �ǽ� ������ �Ųٷ� �׸���.
	// �ɸ��Ͱ� ���� ���̸� �굵 �������ϰ� ����;
	if ( m_pSkinChar )
	{
		D3DXMATRIX matScale, matRender;
		if( IsUsePetSkinPack() )
		{				
			D3DXMatrixScaling( &matScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale, m_sPetSkinPackState.fPetScale );
			matRender = matScale * m_matTrans;			
		}
		else
			matRender = m_matTrans;

		//m_pSkinChar->Render( pd3dDevice, matRender, FALSE, TRUE, TRUE, FALSE, m_pOwner->m_bINVISIBLE );

		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // ���� ���ΰ�;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		RenderMyChar( pd3dDevice, matRender, FALSE, bHALF_VISIBLE );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	NSMaterialSkinManager::SetMob( FALSE );

	if ( m_pSkinChar )
	{
		bool bHALF_VISIBLE = m_pOwner->IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE); // ���� ���ΰ�;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		if ( bHALF_VISIBLE )	// ĳ���� ������ ����
		{
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
		}
		else		// ���󷻴�
		{
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, 1.f );
		}
	}

	return S_OK;
}

void GLPetClient::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( m_spWorldLight->IsNotCalculateColor() )
		return;

	NSMaterialSkinManager::SetMob( FALSE );
	m_pSkinChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

	if ( bHALF_VISIBLE )	// ĳ���� ������ ����
	{
		if ( bOpaqueState )
		{
			// �ƹ��͵� ���� �ʴ´�.
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
			}
			else
			{
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? TRUE : FALSE );
				m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 0.5f );
				m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
				m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
			}
		}		
	}
	else		// ���󷻴�
	{
		if ( bOpaqueState )
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f );
			}
			else
			{
				m_pSkinChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY ? TRUE : FALSE );
				m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
				m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			}
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
			}
			else
			{
				m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			}
		}
	}
}

HRESULT GLPetClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : �׸��� ������.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob ( m_pSkinChar, m_matTrans, pd3dDevice, bWorldSystemOLD );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( !m_bValid )
		return S_OK;

	if ( !pd3dDevice )
		return E_FAIL;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : �׸��� ������.
	//
	if ( m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( m_pSkinChar, m_matTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	}

	return S_OK;
}

HRESULT GLPetClient::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight )
{
	if ( !m_bValid )
        return S_OK;

    if ( !pd3dDevice )
        return E_FAIL;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	//	Note : �ݻ� ������.
	//
	if ( m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_pSkinChar, pd3dDevice, fWaterHeight );
	}

	return S_OK;
}

HRESULT GLPetClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}
	
	return S_OK;
}

HRESULT GLPetClient::InvalidateDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}
	
	return S_OK;
}

// Device �� ���� �ִ� �͵鸸 ������ �ϱ� �ٶ�.
HRESULT GLPetClient::DeleteDeviceObjects ()
{
	//if ( !m_bValid ) return S_OK;

	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
		//SAFE_DELETE( m_pSkinChar );
	}

	Delete();

	return S_OK;
}

void GLPetClient::SK_EFF_SELFZONE ( const SANIMSTRIKE &sStrikeEff, const SKILL::EMEFFECTPOS emPOS, const char* const szZONE_EFF, STARGETID *pTarget )
{
	D3DXMATRIX matTrans;

	//	��Ʈ����ũ ��������.
	BOOL bTRANS(FALSE);
	if ( emPOS == SKILL::EMPOS_STRIKE )
	{
		//	Ÿ���� ��ġ.
		D3DXVECTOR3 vTARPOS = m_vPos + m_vDir * 60.0f;

		//	Ÿ�� ��ġ�� �˾Ƴ�.
		STRIKE::SSTRIKE sStrike;

		bool bAttBone = false;
		if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
		{
			bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
		}

		BOOL bOk = FALSE;

		if ( bAttBone )
		{		
			if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
				return;

			PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
			if ( !pDxAttBone ) 
				return;

			STRIKE::CSELECTOR Selector ( vTARPOS, pDxAttBone->m_AttBoneArray );
			bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		}
		else
		{
			STRIKE::CSELECTOR Selector ( vTARPOS, m_pSkinChar->m_PartArray );
			bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		}

		if ( bOk )
		{
			bTRANS = TRUE;

			D3DXMatrixTranslation ( &matTrans, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );
		}
	}

	//	������� ��ǥ���� ȸ�� �Ӽ� �־.
	if ( !bTRANS )
	{
		bTRANS = TRUE;

		D3DXMATRIX matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * m_matTrans;
	}

	//	Note : �ڱ� ��ġ ����Ʈ �߻���Ŵ.
	DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, matTrans, pTarget, TRUE, FALSE );
}

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
void GLPetClient::StyleInitData()
{
	m_bEnableSytle = TRUE;
	
	m_wStyleTEMP = m_wStyle;

	// �÷� ��ȭ ���� ��� �ǵ��� �ʴ´�.
	if( !m_bEnableColor )
	{
		m_wColorTEMP = m_wColor;
	}
}

void GLPetClient::ColorInitData()
{
	m_bEnableColor = TRUE;

	m_wColorTEMP = m_wColor;

	// ��Ÿ�� ��ȭ ���� ��� �ǵ��� �ʴ´�.
	if( !m_bEnableSytle )
	{
		m_wStyleTEMP = m_wStyle;
	}
}

void GLPetClient::StyleChange( WORD wStyle )
{
	if( 0 > m_emTYPE )
		return;

	m_wStyleTEMP = wStyle; 

	if ( m_wColor == GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyle] && !m_bEnableColor )
	{
		m_wColorTEMP = GLCONST_PET::sPETSTYLE[m_emTYPE].wSTYLE_COLOR[m_wStyleTEMP];
	}

	StyleUpdate();
	ColorUpdate();
}

void GLPetClient::ColorChange( WORD wColor )
{ 
	m_wColorTEMP = wColor; 

	ColorUpdate();
}

void GLPetClient::StyleUpdate()
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return;

	const PETSTYLE &sSTYLE = GLCONST_PET::GetPetStyle ( m_emTYPE );

	// ��Ÿ��.
	if ( sSTYLE.wSTYLENum > m_wStyleTEMP )
	{
		std::string strHAIR_CPS = sSTYLE.strSTYLE_CPS[m_wStyleTEMP];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	���� ���� ��Ų.

		if( pCharPart && strcmp( strHAIR_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL, EMSCD_ALL_THREAD );
		}
	}

	// Note : �̰��� ȣ���ϸ鼭.. �Ӹ����� �����·� �ٲ�� ������.
	//		�׷��� FALSE ȣ��� �Ӹ������ �ȹٲ�� �Ѵ�.
	//UpdateSuit( FALSE );
}

void GLPetClient::ColorUpdate()
{
	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sPetID );
	if ( !pCrowData )					   return;

	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( 
		pCrowData->GetSkinObjFile(),
		m_pd3dDevice );

	if ( !pSkinChar )					  return;

	m_pSkinChar->SetHairColor( m_wColorTEMP );
}

void GLPetClient::MakeAniSubKey ( float fTime )
{
	static bool bPlus(true);

	//srand ( unsigned int(fTime) );
	m_wAniSub[0] = rand()%3+1;
	m_wAniSub[1] = rand()%3+1;
	if ( m_wAniSub[0] == m_wAniSub[1] )
	{
		if ( bPlus )
		{
			m_wAniSub[1] += 1;
			if ( m_wAniSub[1] > 3 ) m_wAniSub[1] = 1;
		}
		else
		{
			m_wAniSub[1] -= 1;
			if ( m_wAniSub[1] < 1 ) m_wAniSub[1] = 3;
		}
		bPlus = !bPlus;
	}
	for ( int i = 1; i < 4; ++i )
	{
		if ( m_wAniSub[0] != i && m_wAniSub[1] != i )
		{
			m_wAniSub[2] = i;
			break;
		}
	}
}

// Note : ���� Ȱ��ȭ�� ��ų�� �����Ѵ�
PETACTIVESKILLLIST_VEC& GLPetClient::UpdateActiveSkillInfo ()
{
	// Note : �ʱ�ȭ
	m_vecActiveSkillInfo.clear();
	//m_vecActiveSkillInfo._Mylast = m_vecActiveSkillInfo._Myfirst;

	for ( WORD i=0; i<m_wMaxSkillSlot; ++i )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sActiveSkillID[i] );
		if ( !pSkill )	continue;

		SPETACTIVESKILLINFO sInfo;
		sInfo.emTypes = pSkill->m_sAPPLY.emBASIC_TYPE;
		sInfo.wLevel  = 0;

		PETACTIVESKILLLIST_VEC_ITER iterFind = std::find(m_vecActiveSkillInfo.begin(), m_vecActiveSkillInfo.end(), sInfo);

		if ( iterFind != m_vecActiveSkillInfo.end() )
		{
			(*iterFind).wCount++;
		}
		else
		{
			sInfo.wCount = 0;
			m_vecActiveSkillInfo.push_back( sInfo );
		}
	}

	return m_vecActiveSkillInfo;
}

D3DXVECTOR3 GLPetClient::GetRandomOwnerTarget ()
{
	D3DXVECTOR3 vRandPos = GLPETDEFINE::GetRandomPosition ();
	m_vRandPos = vRandPos;

    //  �̵��߿� Shock���¿� ������ ������ �̵��� �����.
    //  �̶� ������ ��ġ�� Ÿ���� ��ġ�� Ʋ�����鼭
    //  ���� ��� Ÿ���ֺ��� ��ġ�� �̵��ҷ��� �ϸ鼭 �޼����� ��� �����Եȴ�.
    
    D3DXVECTOR3 vPos;
    if ( m_pOwner->isAction( GLAT_MOVE ) )
    {
        STARGETID sTargetID = m_pOwner->GetActionID ();
        vPos = sTargetID.vPos;
	    vPos += vRandPos;
    }
    else
    {
        vPos = m_pOwner->GetPosition ();
        vPos += vRandPos;
    }

	return vPos;

}

BOOL GLPetClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

bool GLPetClient::IsActiveSkill(SKILL::EMTYPES emSkillType)
{
	const size_t dwSize(m_vecActiveSkillInfo.size());
	for ( size_t i = 0; i < dwSize; ++i )
	{
		if ( m_vecActiveSkillInfo[i].emTypes == emSkillType )
			return true;
	}
	return false;
}

bool GLPetClient::IsQBoxLockMessageNotifyTime(void)
{
	if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock ) && 
		fPET_GETQBOX_DISABLE_MSG < m_fQBoxLockNotifyTimer)
	{
		m_fQBoxLockNotifyTimer = 0.0f;
		return true;
	}
	return false;
}

bool GLPetClient::IsQBoxPickableTime()
{
	return !IsSTATE(EM_PETACT_QBOXPICK_WAIT);
}

int GLPetClient::GetType() 
{
	return (int) m_emTYPE;
}

std::string GLPetClient::GetTypeString()
{
	if( -1 < m_emTYPE && m_emTYPE < COMMENT::PET_TYPE.size() )
		return COMMENT::PET_TYPE[ m_emTYPE ].c_str();
	else 
		return "";
}

const WORD  GLPetClient::GetMaxSkillSlot() const
{
	return m_wMaxSkillSlot;
}

lua_tinker::table GLPetClient::GetPetCardID()
{
	GLCharacter* pChar = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	if( pChar->GetGaeaID() == m_dwOwner )
	{
		tbItem.set("dwID", pChar->m_sPetCardNativeID.dwID );
		tbItem.set("wMID", pChar->m_sPetCardNativeID.wMainID );
		tbItem.set("wSID", pChar->m_sPetCardNativeID.wSubID );
	}

	return tbItem;
}

lua_tinker::table GLPetClient::GetPetItemList()
{
	lua_tinker::table tbItemList( GLWidgetScript::GetInstance().GetLuaState() );
	int nDisplayCnt = 1; //Lua
	for( int i = 0; i < ACCETYPESIZE; ++i )
	{
		lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		
		tbItem.set("dwID", m_PutOnItems[i].GetNativeID().dwID );
		tbItem.set("wMID", m_PutOnItems[i].GetNativeID().wMainID );
		tbItem.set("wSID", m_PutOnItems[i].GetNativeID().wSubID );

		tbItemList.set( nDisplayCnt++, tbItem );

	}

	return tbItemList;
}

lua_tinker::table GLPetClient::GetPetSkillList( int nIndex )
{
	lua_tinker::table tbSkill( GLWidgetScript::GetInstance().GetLuaState() );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( EMSKILL_PET, nIndex );
	if ( pSkill )
	{
		// �⺻ ���� -----------------------------------------------------------
		tbSkill.set( "dwID", pSkill->GetId().dwID );			// ID
		tbSkill.set( "wMID", pSkill->GetId().wMainID );			// wMID
		tbSkill.set( "wSID", pSkill->GetId().wSubID );			// wSID
		tbSkill.set( "strName", pSkill->GetName() );			// ��ų �̸�
		tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );		// ���
		tbSkill.set( "bPassive", (pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? 1:0) ); //�нú� ��ų ���� Ȯ��

		//	Note : ���� ��ų���� �˻�.
		PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( pSkill->GetId().dwID );
		if ( learniter==m_ExpSkills.end() )
			tbSkill.set( "bLearn", FALSE );	
		else
			tbSkill.set( "bLearn", TRUE );		

		//��ų Ȱ��ȭ Ȯ��
		//Ȱ��ȭ ���̶�� ������� ���� ��ȣ�� ��ȯ
		int nActivatedIndex( 0 );
		for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
		{
			if ( m_sActiveSkillID[i] == pSkill->GetId() )
				nActivatedIndex = i + 1;	
		}
		tbSkill.set( "nActivated", nActivatedIndex );	
		
	}

	return tbSkill;
}

lua_tinker::table GLPetClient::GetPetSkillListAll()
{
	int nDisplayCnt = 1; //Lua
	lua_tinker::table tbResult( GLWidgetScript::GetInstance().GetLuaState() );
	
	for( int i = 0; i < MAX_PETSKILL; ++i )
	{
		lua_tinker::table tbSkill( GLWidgetScript::GetInstance().GetLuaState() );
		
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( EMSKILL_PET, i );
		if ( pSkill )
		{
			// �⺻ ���� -----------------------------------------------------------
			tbSkill.set( "dwID", pSkill->GetId().dwID );			// ID
			tbSkill.set( "wMID", pSkill->GetId().wMainID );			// wMID
			tbSkill.set( "wSID", pSkill->GetId().wSubID );			// wSID
			tbSkill.set( "strName", pSkill->GetName() );			// ��ų �̸�
			tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );		// ���
			tbSkill.set( "bPassive", (pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? 1:0) ); //�нú� ��ų ���� Ȯ��

			//	Note : ���� ��ų���� �˻�.
			PETSKILL_MAP_ITER learniter = m_ExpSkills.find ( pSkill->GetId().dwID );
			if ( learniter==m_ExpSkills.end() )
				tbSkill.set( "bLearn", FALSE );	
			else
				tbSkill.set( "bLearn", TRUE );		

			//��ų Ȱ��ȭ Ȯ��
			//Ȱ��ȭ ���̶�� ������� ���� ��ȣ�� ��ȯ
			int nActivatedIndex( 0 );
			for ( WORD i=0; i<MAX_PETSKILLSLOT; ++i )
			{
				if ( m_sActiveSkillID[i] == pSkill->GetId() )
					nActivatedIndex = i + 1;	
			}
			tbSkill.set( "nActivated", nActivatedIndex );	
			
			//��� �Է�
			tbResult.set( nDisplayCnt++, tbSkill );
		}
	}

	return tbResult;
}


void GLPetClient::SetActivatedSkill( lua_tinker::table tbActivatedSkill )
{
	for( int i = 1; i <= tbActivatedSkill.TableLen(); ++i )
	{
		if( i > m_wMaxSkillSlot ) continue; //���ų ī�带 ����Ͽ����� 2 �ƴϸ� 1

		lua_tinker::table tbSkill = tbActivatedSkill.get< lua_tinker::table >( i );

		SNATIVEID sID;

		if( tbSkill.get< DWORD >( "dwID" ) == -1 )
			sID = NATIVEID_NULL() ;
		else
			sID = tbSkill.get< DWORD >( "dwID" );
		
		if( m_sActiveSkillID[i - 1] != sID )
		{
			ReqChangeActiveSkill(sID, i - 1);
		}	
	}
}

WORD GLPetClient::ColorDWORDtoWORD( DWORD dwColor )
{
	WORD wR, wG, wB;
	wR = (WORD)((dwColor&0xff0000)>>16);
	wG = (WORD)((dwColor&0xff00)>>8);
	wB = (WORD)(dwColor&0xff);

	// �����
	wR = wR >> 3;
	wG = wG >> 3;
	wB = wB >> 3;
	return (wR<<10) + (wG<<5) + wB;
}

DWORD GLPetClient::GetColor()
{
	int nR = int( float( (m_wColor & 0x7c00) >> 10 ) * 255.0f / 31.0f );
	int nG = int( float( (m_wColor & 0x3e0) >> 5 ) * 255.0f / 31.0f );
	int nB = int( float( (m_wColor & 0x1f) ) * 255.0f / 31.0f );

	return ( 0xFF000000 | (nR << 16) | (nG << 8) | nB );
}

void GLPetClient::SetColor( DWORD dwColor )
{
	ColorChange( ColorDWORDtoWORD( dwColor ) );
}

void GLPetClient::CommitColor( DWORD dwColor )
{
	if( NULL == m_pOwner )
		return;

	ReqChangeColor( ColorDWORDtoWORD( dwColor ) );
}
