#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffBillboardChar.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicleData.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/GlobalParam.h"

#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../Land/GLLandManClient.h"
#include "../GLGaeaClient.h"
#include "../GLogicExClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Char/GLCharacter.h"
#include "./NSGradeScriptMan.h"
#include "./GLCharClient.h"
#include "SkinCharMulti.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLCharClient::GLCharClient(GLGaeaClient* pGaeaClient) 
	: ClientActor(pGaeaClient, CROW_PC)
	, m_pd3dDevice(NULL)
	, m_pSkinChar(NULL)
	, m_pSkinCharMulti(NULL)
	, m_pTransformSet(NULL)		

	, m_bVisible(TRUE)

	, m_fAge(0.0f)		
	, m_vTarPos(0,0,0)
	, m_fIdleTime(0.0f)
	, m_fMoveDelay(0.0f)

	, m_dwANISUBSELECT(0)
	, m_dwANISUBGESTURE(0)

	, m_vMaxOrg(6,20,6)
	, m_vMinOrg(-6,0,-6)

	, m_vMax(6,20,6)
	, m_vMin(-6,0,-6)
	, m_fHeight(20.f)

	, m_dwCeID(0)

	, m_pLandManClient(NULL)

	, m_emANIMAINSKILL(AN_SKILL)
	, m_emANISUBSKILL(AN_SUB_NONE)
	, m_emANIVehicleSKILL( AN_VEHICLE_STAY )

	, m_emANISUBTYPE(AN_SUB_NONE)
	, m_dwRevData(NULL)

	, m_fITEM_MOVE_R(0.0f)
	, m_fITEM_MOVE (0.0f)
	, m_fITEMATTVELO_R ( 0.0f ) 
	, m_fITEMATTVELO ( 0.0f ) 

	, m_wTARNUM(0)
	, m_vTARPOS(0,0,0)
	, m_wACTIVESKILL_LEVEL(0)

	, m_fattTIMER(0.0f)
	, m_nattSTEP(0)

	, m_bItemShopOpen ( false )

	, m_byFB ( 0 )
	, m_byTW ( 0 )

	, m_eMotionMID( AN_GUARD_N )
	, m_eMotionSID( AN_SUB_NONE )
	, m_fMotionSec( 0.0f )

	, m_dwVehiclePassengerID(0)

	, m_spWorldLight(new ActorWorldLight)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
	, m_spCollisionByObject(new ActorNameCollision(EMCC_CULL_NONE_ONLY_WALL))

	, m_fAnimationMoveRotationY(0.f)
{	
	D3DXMatrixIdentity( &m_matBikeTrans );
	
	DISABLEALLLANDEFF();
	DISABLEALLMAPEFF();

	memset(m_bActionType, false, sizeof( bool ) * SKILL::EMACTION_TYPE_NSIZE);
    memset(m_ClubName, 0, sizeof(char) * CHAR_SZNAME);
}

GLCharClient::~GLCharClient(void)
{
	FactEffectProcessing( EMFACTEFFECTPROCESSING_DESTRUCTOR );
	
	SAFE_DELETE(m_pTransformSet);
	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE(m_sVehicle.m_pSkinChar);

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}

	if ( m_bItemShopOpen )
	{
		//!! SF_TODO
		//m_pInterface->ItemShopDelShopIcon( m_dwGaeaID );
	}
}

void GLCharClient::SetClubName(const std::string& ClubName)
{
    StringCchCopyA(m_ClubName, CHAR_SZNAME, ClubName.c_str());
}

const BOOL GLCharClient::CalcVisibleDetect(const float fTime)
{
	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return FALSE;

// 	if ( IsActState(EM_REQ_VISIBLENONE + EM_REQ_VISIBLEOFF) )
// 		return FALSE; // GM 기능으로 인한 투명 상태이거나 완전 투명 상태면 보이지 않음;

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	const D3DXVECTOR3 vMyPosition(pCharacter->GetPosition() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));
	const D3DXVECTOR3 vPosition(GetPosition() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));

	if ( m_pLandManClient->IsCullByObjectMap() == true )
	{
		// 내 케릭터 시야에 없다면 안보임;
		if ( m_spCollisionByObject->GetCollsion() )
			return FALSE; 

		//if ( m_pGaeaClient->IsCollision(vMyPosition, vPosition, EMCC_CULL_NONE_ONLY_WALL) == true )
		//	return FALSE; // 내 케릭터 시야에 없다면 안보임;
	}

	if ( IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) == FALSE )
		return TRUE; // 은신 중이 아니면 아래 검사들 수행하지 않음;	

	if ( GLCharClient::GetPartyID() == pCharacter->GetPartyID() )
		return TRUE; // 대상과 내가 같은 파티면 보임;	

	const SRECVISIBLE& RECVISIBLE = pCharacter->m_sRECVISIBLE;	

	const D3DXVECTOR3 vDistance(vPosition - vMyPosition);	
	const float fDistance = ::D3DXVec3Length(&vDistance);

	if ( pCharacter->IsCheckedSkillFlagSpecial(EMSPECA_RECVISIBLE) &&
		(m_sINVISIBLE.dwLevelINVISIBLE <= RECVISIBLE.dwLevelRECVISIBLE) &&
		(fDistance < RECVISIBLE.fRadiusRECVISIBLE) )
		return TRUE; // 은신 감지 기능이 활성화 되어 있고 조건을 만족하면 보임;

	return CalcVisibleDetectAuto(fTime, fDistance, vDistance); // 자동 은신 감지;
}

const BOOL GLCharClient::CalcVisibleDetectAuto(const float fTime, const float fDistance, const D3DXVECTOR3& vDistance)
{
	if ( fDistance > GLHIDESET::fInvisiblePerceiveRadius )
		m_sRECVISIBLE.bFLAG = false;
	else if ( (fTime - m_sRECVISIBLE.fLatestCheckTime) > GLHIDESET::fInvisibleUpdateTime )
	{ // 자동 은신 감지 시간이 만료되면 다시 검사, 그렇지 않으면 기존 상태 값 그대로 반환;
		m_sRECVISIBLE.fLatestCheckTime = fTime;

		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();		
		const int nGapLevel = pCharacter->m_wLevel - GetLevel();
		const int nConstPercentage = 
			(nGapLevel > 0 ? int(float(nGapLevel) * GLHIDESET::fInvisiblePerceiveLevel) : 5);		

		const float fPercentage = abs(vDistance.x + vDistance.y + vDistance.z + fDistance);
		const int nPercentage = int((fPercentage - float(int(fPercentage))) * 100.0f);		

		D3DXVECTOR3 vDistanceNormal;
		::D3DXVec3Normalize(&vDistanceNormal, &vDistance);;
		const D3DXVECTOR3& vDirectionNormal = pCharacter->GetDirect();
		const float fDot = ::D3DXVec3Dot(&vDistanceNormal, &vDirectionNormal);

		if ( fDot > GLHIDESET::fInvisiblePerceiveAngle )
			m_sRECVISIBLE.bFLAG = nPercentage < (nConstPercentage * (2.0f - fDistance * GLHIDESET::fInvisiblePerceiveURadius)); // 1.0f/90.0f
		else
			m_sRECVISIBLE.bFLAG = false;
	}

	return m_sRECVISIBLE.bFLAG;
}

HRESULT GLCharClient::Create(GLLandManClient* pLandManClient, std::tr1::shared_ptr<SDROP_CHAR> spCharData, LPDIRECT3DDEVICEQ pd3dDevice)
{
    if (!spCharData)
        return E_FAIL;

	m_pd3dDevice = pd3dDevice;
	m_pLandManClient = pLandManClient;
	if (spCharData)
        m_CharData = *(spCharData.get());

	//m_emVehicle = spCharData->m_emVehicle;
	//m_sVehicle.m_dwGUID = spCharData->m_sVehicle.m_dwGUID;
	//m_sVehicle.m_emTYPE = spCharData->m_sVehicle.m_emTYPE;
	//m_sVehicle.m_sVehicleID = spCharData->m_sVehicle.m_sVehicleID;

    // bjju.sns
    m_byFB = spCharData->m_Base.byFB;
    m_byTW = spCharData->m_Base.byTW;

	m_bItemShopOpen = (spCharData->m_Base.m_bItemShopOpen == 0) ? false : true;

	//for ( int i = 0; i < ACCE_TYPE_SIZE; ++i )
	//{
	//	m_sVehicle.m_PutOnItems[i].Assign( spCharData->m_sVehicle.m_PutOnItems[i] );
	//}

	// 현재 스킬 FACT 업데이트.
	//
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SDROP_SKILLFACT& sDropFact = spCharData->sSKILLFACT[i];
		if (sDropFact.sNATIVEID == NATIVEID_NULL())
            continue;

        RECEIVE_SKILLFACT ( sDropFact.sNATIVEID, static_cast<WORD>(sDropFact.cLEVEL), sDropFact.wSLOT, sDropFact.sTargetID, SSKILLFACT::STIME(sDropFact) );
	}
	
	// 현재 상태이상 업데이트
	for ( int i=0; i<EMBLOW_MULTI; ++i )
	{
		SSTATEBLOW& sDropBlow = spCharData->sSTATEBLOWS[i];
		if (sDropBlow.emBLOW == EMBLOW_NONE)
            continue;

		SSTATEBLOW* pSTATEBLOW = NULL;
		if (sDropBlow.emBLOW <= EMBLOW_SINGLE)
        {
            pSTATEBLOW = &m_sSTATEBLOWS[0];
        }
		else
        {
            DWORD Index = sDropBlow.emBLOW-EMBLOW_SINGLE;
            if (Index < EMBLOW_MULTI)
                pSTATEBLOW = &m_sSTATEBLOWS[Index];
        }

        // assign itself
		//sDropBlow.emBLOW      = sDropBlow.emBLOW;
		//sDropBlow.fAGE        = sDropBlow.fAGE;
		//sDropBlow.fSTATE_VAR1 = sDropBlow.fSTATE_VAR1;
		//sDropBlow.fSTATE_VAR2 = sDropBlow.fSTATE_VAR2;
        if (pSTATEBLOW)
            pSTATEBLOW->SetData(sDropBlow);
	}

	DISABLEALLLANDEFF();
	DISABLEALLMAPEFF();

	BOOL bAmbientUp(TRUE);
	if ( NSLightMapAmbientColor::g_bActiveLightMapAmbientColor )
	{
		bAmbientUp = FALSE;
	}

	// 케릭터의 외형을 설정.
	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS)m_CharData.m_Base.emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[emIndex], pd3dDevice );
	if ( !pSkinChar )
        return E_FAIL;

	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE(m_pTransformSet);
	m_pTransformSet = new GLTransformSet(pd3dDevice, bAmbientUp, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE);
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, bAmbientUp, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );	

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
	m_pSkinCharMulti = new SkinCharMulti(pSkinChar, pd3dDevice, bAmbientUp, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	//m_pSkinCharMulti->SetCharData(pSkinChar, pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

	m_strName = m_CharData.m_Base.szName;
	m_dwGaeaID = m_CharData.m_Base.dwGaeaID;
	m_dwCeID = m_CharData.m_Base.dwCeID; 	
	
	UpdateSuit ();
	UPDATE_ITEM();

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	// 에니메이션 초기화.
	//
	ReSelectAnimation ();

	// 이동 제어 초기화.
	const D3DXVECTOR3& vPosition = m_CharData.m_Base.vPos;
	const D3DXVECTOR3& vDirect = m_CharData.m_Base.vDir;
	
	m_dwSummonGUIDList.clear(); // 소환수 GUID 리스트
	RemoveAllChildCrow();

	m_CHARINDEX = CharClassToIndex((EMCHARCLASS)m_CharData.m_Base.emClass);
	
	if ( pLandManClient )
		ClientActor::InitializeActorMove(vPosition, pLandManClient->GetNaviMesh(), vDirect);
	ClientActor::SetMaxSpeed(GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO);

	if ( spCharData->m_Base.dwActState & EM_ACT_RUN )
        SetActState ( EM_ACT_RUN );
	if ( spCharData->m_Base.dwActState & EM_REQ_VISIBLENONE )
        SetActState ( EM_REQ_VISIBLENONE );
	if ( spCharData->m_Base.dwActState & EM_REQ_VISIBLEOFF )
        SetActState ( EM_REQ_VISIBLEOFF );
	if ( spCharData->m_Base.dwActState & EM_ACT_PEACEMODE )
        SetActState ( EM_ACT_PEACEMODE );

	switch ( spCharData->m_Base.Action )
	{
	case GLAT_MOVE:
		{
			// 이동 시키기 위해 내부 전달 메시지 발생.
			//
			GLMSG::SNETPC_GOTO_BRD NetMsgGoto;
			NetMsgGoto.dwActState = spCharData->m_Base.dwActState;
			NetMsgGoto.fDelay = 0.0f;
			NetMsgGoto.vCurPos = spCharData->m_Base.vPos;
			NetMsgGoto.vTarPos = spCharData->m_Base.vTarPos;
			MsgGoto ( (NET_MSG_GENERIC*) &NetMsgGoto );
		}
		break;

	case GLAT_FALLING:
		TurnAction ( GLAT_FALLING );
		break;
	case GLAT_FEIGN_FALLING :
		TurnAction ( GLAT_FEIGN_FALLING );
		break;
	case GLAT_FEIGN_DIE :
		TurnAction ( GLAT_FEIGN_DIE );
		break;
	case GLAT_DIE:
		TurnAction ( GLAT_DIE );
		break;

	case GLAT_GATHERING:
		m_dwANISUBGESTURE = m_CharData.m_Base.m_dwANISUBTYPE;
		TurnAction( GLAT_GATHERING );
		break;

	case GLAT_TALK:
		m_dwANISUBGESTURE = m_CharData.m_Base.m_dwANISUBTYPE;
		TurnAction ( GLAT_TALK );
		break;

    case GLAT_MOTION:
        SetMotion((EMANI_MAINTYPE)m_CharData.m_Base.m_dwANIMAINTYPE,
                  (EMANI_SUBTYPE)m_CharData.m_Base.m_dwANISUBTYPE,
                  m_CharData.m_Base.m_fAniRemainTime);
        break;
	};

	if ( spCharData->m_Base.dwFLAGS & SDROP_CHAR_BASE::CHAR_GEN )
	{
		if ( !IsActState(EM_REQ_VISIBLENONE) && !IsActState(EM_REQ_VISIBLEOFF) )
		{
			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, vPosition.x, vPosition.y, vPosition.z );

			STARGETID sTargetID(CROW_PC, spCharData->m_Base.dwGaeaID, vPosition);
			DxEffGroupPlayer::GetInstance().NewEffGroup
			(
			GLCONST_CHAR::strREBIRTH_EFFECT.c_str(),
				matEffect,
				&sTargetID,
				FALSE, 
				FALSE);
		}
	}

	if ( !m_pSkinChar->GETCURANIMNODE() )
	{
		DEFAULTANI( m_pSkinChar );
	}

	// 로딩중일수도 있다.
	//if ( !m_pSkinChar->GETCURANIMNODE() )
	//{
	//	
	//	sc::writeLogError(
	//		sc::string::format(
	//		"current animation node null point error [%1%] [M %2% S %3%]",
	//		m_pSkinChar->GetFileName(),
	//		m_pSkinChar->GETCURMTYPE(),
	//		m_pSkinChar->GETCURSTYPE()));
	//}

	// 클럽마크 버전 확인.
	//
	m_pGaeaClient->ReqClubMarkInfo(m_CharData.m_Base.dwGuild, m_CharData.m_Base.dwGuildMarkVer);

	if ( m_bItemShopOpen )
	{
		//!! SF_TODO
		//m_pInterface->ItemShopAddShopIcon( m_dwGaeaID );
	}

	// 국가 세팅;
	m_sCountryInfo.dwID = spCharData->m_Base.dwCountryID;

	return S_OK;
}

DWORD GLCharClient::GET_PK_COLOR ()
{
	if ( m_CharData.m_Base.nBright >= 0 )
        return GLCONST_CHAR::dwPK_NORMAL_NAME_COLOR;

	int nLEVEL = 0;
	for ( nLEVEL=0; nLEVEL<GLCONST_CHAR::EMPK_STATE_LEVEL; ++nLEVEL )
	{
		if ( GLCONST_CHAR::sPK_STATE[nLEVEL].nPKPOINT <= m_CharData.m_Base.nBright )
            break;
	}

	if ( nLEVEL>=GLCONST_CHAR::EMPK_STATE_LEVEL )
        nLEVEL = GLCONST_CHAR::EMPK_STATE_LEVEL-1;

	return GLCONST_CHAR::sPK_STATE[nLEVEL].dwNAME_COLOR;
}

SITEM* GLCharClient::GET_SLOT_ITEMDATA ( EMSLOT _slot )
{
	if ( VALID_SLOT_ITEM(_slot) )
	{
		const SITEMCLIENT &sITEMCLIENT = GET_SLOT_ITEM ( _slot );

		SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCLIENT.sNativeID );
		return pITEM;
	}

	return NULL;
}

HRESULT GLCharClient::UpdateBoardSuit ()
{
	GASSERT(m_pd3dDevice);

	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS) m_CharData.m_Base.emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )
        return E_FAIL;


	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) )
            continue;

		SITEMCLIENT &ItemClient = m_CharData.m_PutOnItems[i];

		SNATIVEID nidITEM = ItemClient.nidDISGUISE;
		if ( nidITEM==SNATIVEID(false) )
            nidITEM = ItemClient.sNativeID;
		if ( nidITEM==SNATIVEID(false) )
            continue;

		SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
		if ( pItem )
		{
			if( pItem->sSuitOp.emSuit != SUIT_HANDHELD )
                continue;

			SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemClient.GETGRADE_EFFECT(), EMSCD_ALL_THREAD );

			// 코스튬 기본 색 입히기. by luxes.
			PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
			if ( pCharPart )
			{
				DWORD dwMainColor = 0, dwSubColor = 0;
				pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
				pCharPart->SetHairColor( dwMainColor );
				pCharPart->SetSubColor( dwSubColor );
			}

			// 코스튬 변경한 색 입히기. by luxes.
			if ( m_CharData.m_PutOnItems[i].sNativeID != NATIVEID_NULL() )
			{
				if (m_CharData.m_PutOnItems[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
					&& m_CharData.m_PutOnItems[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
				{
					PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
					if ( pCharPart )
					{
						pCharPart->SetHairColor( m_CharData.m_PutOnItems[i].dwMainColor );
						pCharPart->SetSubColor( m_CharData.m_PutOnItems[i].dwSubColor );
					}
				}
			}
		}		
	}

	return S_OK;
}

HRESULT GLCharClient::UpdateBaseSuit ()
{
	GASSERT(m_pd3dDevice);


	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS) m_CharData.m_Base.emClass);


	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( sCONST.dwHEADNUM > m_CharData.m_Base.wFace )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[m_CharData.m_Base.wFace];

		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

		if ( pCharPart && strcmp(strHEAD_CPS.c_str(),pCharPart->GetFileName()) )
		{
			SetPiece ( strHEAD_CPS.c_str(), m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
		}
	}

	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) )
            continue;

		SITEMCLIENT &ItemClient = m_CharData.m_PutOnItems[i];

		SNATIVEID nidITEM = ItemClient.nidDISGUISE;
		if ( nidITEM==SNATIVEID(false) )
            nidITEM = ItemClient.sNativeID;
		if ( nidITEM==SNATIVEID(false) )
            continue;
		
		SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
		if ( pItem )
		{
			if( pItem->sSuitOp.emSuit != SUIT_HANDHELD)
                continue;

			SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemClient.GETGRADE_EFFECT(), EMSCD_ALL_THREAD );
			// 코스튬 기본 색 입히기. by luxes.
			PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
			if ( pCharPart )
			{
				DWORD dwMainColor = 0, dwSubColor = 0;
				pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
				pCharPart->SetHairColor( dwMainColor );
				pCharPart->SetSubColor( dwSubColor );
			}

			// 코스튬 변경한 색 입히기. by luxes.
			if ( m_CharData.m_PutOnItems[i].sNativeID != NATIVEID_NULL() )
			{
				if (m_CharData.m_PutOnItems[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
					&& m_CharData.m_PutOnItems[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
				{
					PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
					if ( pCharPart )
					{
						pCharPart->SetHairColor( m_CharData.m_PutOnItems[i].dwMainColor );
						pCharPart->SetSubColor( m_CharData.m_PutOnItems[i].dwSubColor );
					}
				}
			}
		}
	}
    
    //  탈것 타입은 무조건 Reset시킨다.
    ResetPiece(SLOT_2_PIECE(EMSLOT(SLOT_VEHICLE)));

	for( BYTE i = 0; i < EM_SIMPLESKIN_NUM; i++ )
	{
		if( m_CharData.m_Base.wSex == 1 )
		{				
			SetPiece( GLCONST_CHAR::szManSimpleSkin[i], m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
		}
        else
        {
			SetPiece ( GLCONST_CHAR::szWomenSimpleSkin[i], m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
		}
	}

	return S_OK;
}
HRESULT GLCharClient::UpdateAllSuit ()
{
	GASSERT(m_pd3dDevice);

	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS) m_CharData.m_Base.emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return E_FAIL;

	for ( int i = 0; i < PIECE_SIZE; i++ )
	{
		if( i == PIECE_HEAD || i == PIECE_HAIR )
			continue;
		ResetPiece( EMPIECECHAR( i ) );
	}

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( sCONST.dwHEADNUM > m_CharData.m_Base.wFace )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[m_CharData.m_Base.wFace];

		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

		if ( pCharPart  /*|| m_pSkinCharMulti*/ )
		{
			SetPiece ( PIECE_HEAD, std::string(pCharPart->GetFileName()), strHEAD_CPS,  m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
		}
	}

	if ( sCONST.dwHAIRNUM > m_CharData.m_Base.wHair )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[m_CharData.m_Base.wHair];

		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if ( pCharPart  /*|| m_pSkinCharMulti*/ )
		{
			SetPiece ( PIECE_HAIR, std::string(pCharPart->GetFileName()), strHAIR_CPS, m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
		}
	}

	SetHairColor( m_CharData.m_Base.wHairColor );

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) ) continue;

		SITEMCLIENT &ItemClient = m_CharData.m_PutOnItems[i];

		SNATIVEID nidITEM = ItemClient.nidDISGUISE;
 		if ( nidITEM==SNATIVEID(false) )	nidITEM = ItemClient.sNativeID;

		if ( !m_emVehicle && i == SLOT_VEHICLE  ) nidITEM = SNATIVEID(false);
		else if ( m_emVehicle && i == SLOT_VEHICLE )
		{
			nidITEM = m_sVehicle.GetSkinID();
			if ( nidITEM == SNATIVEID(false) )
			{
				m_sVehicle.m_sVehicleID = ItemClient.sNativeID;
				nidITEM = ItemClient.sNativeID;
			}
		}

		if ( nidITEM == SNATIVEID(false) )
		{
			// 기본 스킨과 지금 장착된 스킨이 틀릴 경우.
			//	SLOT->PIECE.
			DxSkinPieceBase* pSkinPiece = NULL;	//	기본 스킨.
			PDXCHARPART pCharPart = NULL;	//	현재 장착 스킨.

			EMPIECECHAR emPiece = SLOT_2_PIECE(EMSLOT(i));
			if ( emPiece!=PIECE_NONE )
			{
				pSkinPiece = pSkinChar->GetPiece( m_pd3dDevice, emPiece, EMSCD_ALL_THREAD );	//	기본 스킨.
				pCharPart = m_pSkinChar->GetPiece(emPiece);	//	현재 장착 스킨.
			}

			if ( pSkinPiece )
			{
				if ( pCharPart && strcmp(pSkinPiece->GetFileName(),pCharPart->GetFileName()) )
				{
					SetPiece ( pSkinPiece->GetFileName(), m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
				}
			}
			else
			{
				if ( pCharPart )
				{
					ResetPiece(emPiece);
				}
			}

			if( i == SLOT_VEHICLE )
			{
				if( m_sVehicle.m_pSkinChar )
				{
					m_sVehicle.m_pSkinChar->ResetVehicle();
					SAFE_DELETE( m_sVehicle.m_pSkinChar );
				}			
			}
		}
		else
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
			if ( pItem ) 
			{
				if( i == SLOT_VEHICLE )
				{
					if( pItem->VehicleType() != VEHICLE_TYPE_BOARD && m_emVehicle != EMVEHICLE_PASSENGER )
					{
						if( m_sVehicle.m_pSkinChar )
						{
							DxVehicleData* pSkinChar = DxVehicleDataContainer::GetInstance().LoadVehicleData( pItem->GetWearingFile(emIndex), EMSCD_ZERO );
							if ( !pSkinChar )	return E_FAIL;
							m_sVehicle.m_pSkinChar->SetVehiclePartData ( pSkinChar, m_pd3dDevice, TRUE );

							SITEM* pITEM;
							for ( int i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; ++i )
							{
								if ( m_sVehicle.m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() ) continue;
								pITEM = GLogicData::GetInstance().GetItem ( m_sVehicle.m_PutOnItems[i].GetNativeID() );
								if ( !pITEM ) continue;

								m_sVehicle.m_pSkinChar->SetPart ( pITEM->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType), m_pd3dDevice, 0 );

							}

							//오토바이 기본 색깔 입히기
							for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
							{
								PDXCHARPART pSkinPiece = m_sVehicle.m_pSkinChar->GetVehiclePart( i );
								if( pSkinPiece )
								{
									DWORD dwMainColor = 0, dwSubColor = 0;
									pSkinPiece->GetDefaultOverlayColor( dwMainColor , dwSubColor );
									if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
										dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
									{

										pSkinPiece->SetHairColor( dwMainColor );
										pSkinPiece->SetSubColor( dwSubColor );
									}
								}
							}

							//오토바이 색깔 변경한 값 입히기
							for( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
							{
								PDXCHARPART pSkinPiece = m_sVehicle.m_pSkinChar->GetVehiclePart( i );
								if( pSkinPiece )
								{
									if( m_sVehicle.m_sColor[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
										m_sVehicle.m_sColor[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
									{
										pSkinPiece->SetHairColor( m_sVehicle.m_sColor[i].dwMainColor );
										pSkinPiece->SetSubColor( m_sVehicle.m_sColor[i].dwSubColor );
									}									
								}
							}

							m_sVehicle.m_pSkinChar->SetPosition( vPosition );
						}
						else
						{
							DxVehicleData* pSkinChar = DxVehicleDataContainer::GetInstance().LoadVehicleData( pItem->GetWearingFile(emIndex), EMSCD_ZERO );
							if ( !pSkinChar )	return E_FAIL;

							m_sVehicle.m_pSkinChar = new DxVehicle;
							m_sVehicle.m_pSkinChar->SetVehiclePartData ( pSkinChar, m_pd3dDevice, TRUE );

							SITEM* pITEM;
							for ( int i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; ++i )
							{
								if ( m_sVehicle.m_PutOnItems[i].GetNativeID() == NATIVEID_NULL() ) continue;
								pITEM = GLogicData::GetInstance().GetItem ( m_sVehicle.m_PutOnItems[i].GetNativeID() );
								if ( !pITEM ) continue;

								m_sVehicle.m_pSkinChar->SetPart ( pITEM->GetWearingFile( ( EMCHARINDEX ) pItem->sVehicle.emPartsType), m_pd3dDevice, 0 );
							}

							for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++ )
							{
								PDXCHARPART pSkinPiece = m_sVehicle.m_pSkinChar->GetVehiclePart( i );
								if( pSkinPiece )
								{
									DWORD dwMainColor = 0, dwSubColor = 0;
									pSkinPiece->GetDefaultOverlayColor( dwMainColor , dwSubColor );
									if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
										dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
									{

										pSkinPiece->SetHairColor( dwMainColor );
										pSkinPiece->SetSubColor( dwSubColor );
									}
								}
							}

							for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
							{
								PDXCHARPART pSkinPiece = m_sVehicle.m_pSkinChar->GetVehiclePart( i );
								if( pSkinPiece )
								{
									if( m_sVehicle.m_sColor[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
										m_sVehicle.m_sColor[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
									{
										pSkinPiece->SetHairColor( m_sVehicle.m_sColor[i].dwMainColor );
										pSkinPiece->SetSubColor( m_sVehicle.m_sColor[i].dwSubColor );

									}									
								}
							}
							m_sVehicle.m_pSkinChar->SetPosition( vPosition );
						}
					}
					else if( m_emVehicle == EMVEHICLE_PASSENGER )
					{
						continue;
					}
					else
					{
						SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemClient.GETGRADE_EFFECT(), EMSCD_ALL_THREAD );
					}
				}
				else
				{
					SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemClient.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, NULL, pItem->GetSelfBodyEffect() );

                    // Note : 추가적 Grade 효과
                    EMSLOT emSlot = EMSLOT(i);
                    if ( emSlot == SLOT_UPPER || emSlot == SLOT_LOWER || emSlot == SLOT_HAND || emSlot == SLOT_FOOT )
                    {
                        const TSTRING& strGradeCps = NSGradeScriptMan::GetGradeCps( m_CharData.m_Base.wSex, emSlot, ItemClient.GETGRADE_EFFECT() );
                        if ( strGradeCps.empty() )
                        {
                            switch( emSlot )
                            {
                            case SLOT_UPPER:
								{
									ResetPiece( PIECE_GRIDE_UP );
								}
                                break;
                            case SLOT_LOWER:
								{
									ResetPiece( PIECE_GRIDE_DOWN );
								}
                                break;
                            case SLOT_HAND:
								{
									ResetPiece( PIECE_GRIDE_HAND );
								}
                                break;
                            case SLOT_FOOT:
								{
									ResetPiece( PIECE_GRIDE_FOOT );
								}
                                break;
                            };
                        }
                        else
                        {
                            SetPiece( strGradeCps.c_str(), m_pd3dDevice, 0L, 0, EMSCD_ALL_THREAD );
                        }
                    }


					// 코스튬 기본 색 입히기. by luxes.
					PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
					if ( pCharPart )
					{
						DWORD dwMainColor = 0, dwSubColor = 0;
						pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
						pCharPart->SetHairColor( dwMainColor );
						pCharPart->SetSubColor( dwSubColor );
					}

					// 코스튬 변경한 색 입히기. by luxes.
					if ( m_CharData.m_PutOnItems[i].sNativeID != NATIVEID_NULL() )
					{
						if (m_CharData.m_PutOnItems[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
							&& m_CharData.m_PutOnItems[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
						{
							PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
							if ( pCharPart )
							{
								pCharPart->SetHairColor(m_CharData.m_PutOnItems[i].dwMainColor);
								pCharPart->SetSubColor(m_CharData.m_PutOnItems[i].dwSubColor);
							}
						}
					}

					//// Note : 아이템 이펙트
					//if ( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[0] )
					//{
					//	EMPIECECHAR emPiece = SLOT_2_PIECE(EMSLOT(i));					

					//	if ( m_pSkinChar->GetPiece(emPiece) && m_pSkinChar->GetPiece(emPiece)->GetFileName()[0] )
					//	{
					//		DxEffcharDataMan::GetInstance().PutEffect ( m_pSkinChar->GetPiece(emPiece), pItem->GetSelfBodyEffect(), &vDirect );
					//	}
					//	else if ( m_pSkinChar->GetAttachBone(emPiece) && m_pSkinChar->GetAttachBone(emPiece)->GetFileName()[0] )
					//	{
					//		DxEffcharDataMan::GetInstance().PutEffect ( m_pSkinChar->GetAttachBone(emPiece), pItem->GetSelfBodyEffect(), &vDirect );
					//	}
					//}
				}
			}
		}
	}

	//과학부 총때문에 EX피스를 읽어온다

	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) ) continue;

		SITEMCLIENT &ItemClient = m_CharData.m_PutOnItems[i];

		SNATIVEID nidITEM = ItemClient.nidDISGUISE;
		if ( nidITEM==SNATIVEID(false) )	nidITEM = ItemClient.sNativeID;

		if ( !m_emVehicle && i == SLOT_VEHICLE  ) nidITEM = SNATIVEID(false);
		else if ( i == SLOT_VEHICLE )
		{
			nidITEM = m_sVehicle.GetSkinID();
			if ( nidITEM == SNATIVEID(false) )
			{
				m_sVehicle.m_sVehicleID = ItemClient.sNativeID;
				nidITEM = ItemClient.sNativeID;
			}
		}

		if ( nidITEM == SNATIVEID(false) )
		{
		}
		else
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
			if ( pItem && pItem->GetWearingFileEx(emIndex)[0] ) 
			{
				EMPIECECHAR emPiece = PIECE_NONE;
				SetPiece ( pItem->GetWearingFileEx(emIndex), m_pd3dDevice, NULL, ItemClient.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, &emPiece, pItem->GetSelfBodyEffect() );

				//// Note : 아이템 이펙트
				//if ( pItem->GetSelfBodyEffect() && pItem->GetSelfBodyEffect()[0] )
				//{
				//	if ( m_pSkinChar->GetPiece(emPiece) && m_pSkinChar->GetPiece(emPiece)->GetFileName()[0] )
				//	{
				//		DxEffcharDataMan::GetInstance().PutEffect ( m_pSkinChar->GetPiece(emPiece), pItem->GetSelfBodyEffect(), &vDirect );
				//	}
				//	else if ( m_pSkinChar->GetAttachBone(emPiece) && m_pSkinChar->GetAttachBone(emPiece)->GetFileName()[0] )
				//	{
				//		DxEffcharDataMan::GetInstance().PutEffect ( m_pSkinChar->GetAttachBone(emPiece), pItem->GetSelfBodyEffect(), &vDirect );
				//	}
				//}
			}
		}
	}

	if( m_dwTransformSkill != SNATIVEID::ID_NULL )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wSubID );
		if( pSkill ) 
		{
			for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
			{
				if( m_CharData.m_Base.wSex == 1 )
				{
					SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Man[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
				}
                else
                {
					SetPiece ( pSkill->m_sSPECIAL_SKILL.strTransform_Woman[i].c_str(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
				}
			}
		}
	}

	// 전신 이펙트 갱신;
	UpdateGeneralEffect();

	//	스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( 
		STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
		m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, GetTransMatrix(), vDirect );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	return S_OK;
}

HRESULT GLCharClient::UpdateSuit ()
{
	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() ) 
	{
		if ( E_FAIL == UpdateSuit_Weapon() )
		{
			if ( m_pTransformSet )
				m_pTransformSet->CancelDistinguish();

			return E_FAIL;
		}

		return S_OK;
	}

	if( m_emVehicle )
	{
		UpdateAllSuit();
		return S_OK;
	}

	if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME  )
		return S_OK;

	HRESULT hr(S_OK);
	if( RANPARAM::emAroundDetail == EM_AROUND_BOARD )
	{
		hr = UpdateBoardSuit();
	}
	else if( RANPARAM::emAroundDetail == EM_AROUND_BASE )
	{
		hr = UpdateBaseSuit();
	}
	else
	{
		hr = UpdateAllSuit();
	}
	return hr;
}

HRESULT GLCharClient::UpdateSuit_Weapon()
{
	if( !m_pd3dDevice ) 
		return E_FAIL;

	//EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS) m_CharData.m_Base.emClass);

	SetHairColor( m_CharData.m_Base.wHairColor );

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();

	// 장착한 아이템 업데이트
	bool bSuccess = false;
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) )
			continue;

		SITEMCLIENT &ItemClient = m_CharData.m_PutOnItems[i];

		// 복장 아이템
		SNATIVEID nidITEM = ItemClient.nidDISGUISE;
		if (nidITEM == SNATIVEID(false))
			nidITEM = ItemClient.sNativeID;

		if ( nidITEM != SNATIVEID(false) )
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
			if ( pItem ) 
			{
				switch( i )
				{
					// 무기만 셋팅.
				case SLOT_RHAND:
				case SLOT_LHAND:
				case SLOT_RHAND_S:
				case SLOT_LHAND_S:
					{
						if ( S_OK == SetPiece(
							pItem->GetWearingFile(emIndex),
							m_pd3dDevice,
							NULL,
							ItemClient.GETGRADE_EFFECT(),
							EMSCD_ALL_THREAD,
							FALSE,
							NULL,
							pItem->GetSelfBodyEffect() ) )
						{
							bSuccess = true;
						}

						if ( S_OK == SetPiece(
							pItem->GetWearingFileEx(emIndex),
							m_pd3dDevice,
							NULL,
							ItemClient.GETGRADE_EFFECT(),
							EMSCD_ALL_THREAD,
							FALSE,
							NULL,
							pItem->GetSelfBodyEffect() ) )
						{
							bSuccess = true;
						}
					}
					break;

				default:
					break;
				};	

				// Note : 추가적 Grade 효과
				EMSLOT emSlot = EMSLOT(i);
				if ( emSlot == SLOT_UPPER || emSlot == SLOT_LOWER || emSlot == SLOT_HAND || emSlot == SLOT_FOOT )
				{
					const TSTRING& strGradeCps = NSGradeScriptMan::GetGradeCps( m_CharData.m_Base.wSex, emSlot, ItemClient.GETGRADE_EFFECT() );
					if ( strGradeCps.empty() )
					{
						switch( emSlot )
						{
						case SLOT_UPPER:
							ResetPiece( PIECE_GRIDE_UP );
							break;
						case SLOT_LOWER:
							ResetPiece( PIECE_GRIDE_DOWN );
							break;
						case SLOT_HAND:
							ResetPiece( PIECE_GRIDE_HAND );
							break;
						case SLOT_FOOT:
							ResetPiece( PIECE_GRIDE_FOOT );
							break;
						};
					}
					else
					{
						SetPiece( strGradeCps.c_str(), m_pd3dDevice, 0L, 0, EMSCD_ALL_THREAD );
					}
				}

				// 코스튬 기본 색 입히기. by luxes.
				PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
				if ( pCharPart )
				{
					DWORD dwMainColor = 0, dwSubColor = 0;
					pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );
					pCharPart->SetHairColor( dwMainColor );
					pCharPart->SetSubColor( dwSubColor );
				}

				// 코스튬 변경한 색 입히기. by luxes.
				if ( m_CharData.m_PutOnItems[i].sNativeID != NATIVEID_NULL() )
				{
					if (m_CharData.m_PutOnItems[i].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
						&& m_CharData.m_PutOnItems[i].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
					{
						PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
						if ( pCharPart )
						{
							pCharPart->SetHairColor(m_CharData.m_PutOnItems[i].dwMainColor);
							pCharPart->SetSubColor(m_CharData.m_PutOnItems[i].dwSubColor);
						}
					}
				}
			}
		}
	}

	if ( false == bSuccess )
		return E_FAIL;

	// 전신 이펙트 갱신;
	UpdateGeneralEffect();

	//	스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect
	( 
		STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
		m_pSkinChar, 
		m_sSKILLFACT, 
		SKILLFACT_SIZE, 
		m_sSTATEBLOWS, 
		GetTransMatrix(), 
		vDirect 
	);

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	return S_OK;
}

void GLCharClient::ResetPiece( int i )
{
	if ( m_pSkinChar )
		m_pSkinChar->ResetPiece(i);

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->ResetPiece(i);
}


HRESULT GLCharClient::SetPiece( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFlag, int nLevel, EMSCD_FLAGS emFlags, BOOL bAttachSub/*=FALSE*/, EMPIECECHAR* pOutPiecetype/*=NULL*/, const char* pSelfBodyEffect/*=NULL */ )
{
	HRESULT hr = S_FALSE;
	if ( m_pSkinChar )
	{
		hr = m_pSkinChar->SetPiece ( szFile, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
		if ( E_FAIL == hr )
			return hr;
	}

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->SetPiece ( szFile, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect  );

	return hr;
}

HRESULT GLCharClient::SetPiece(EMPIECECHAR emPieceChar,
							   const std::string& strFileOrigin,
							   const std::string& strFileChange, 
							   LPDIRECT3DDEVICEQ pd3dDevice, 
							   DWORD dwFlag, 
							   int nLevel, 
							   EMSCD_FLAGS emFlags, 
							   BOOL bAttachSub/*=FALSE*/, 
							   EMPIECECHAR* pOutPiecetype/*=NULL*/, 
							   const char* pSelfBodyEffect/*=NULL */ )
{
	HRESULT hr = FALSE;

	if ( m_pSkinChar )
	{
		PDXCHARPART pCharPart = m_pSkinChar->GetPiece(emPieceChar);
		if (pCharPart && strcmp( strFileOrigin.c_str(), strFileChange.c_str() ))
		{
			hr = m_pSkinChar->SetPiece( strFileChange, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
			if ( E_FAIL == hr )
				return hr;
		}
	}

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->SetPiece( emPieceChar, strFileChange, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );

	return hr;
}

void GLCharClient::SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( szAnim, dwFlags );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTANI ( szAnim, dwFlags );
}

void GLCharClient::SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0*/ )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( MType, SType, dwFlags, dwSelect );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTANI ( MType, SType, dwFlags, dwSelect );
}

void GLCharClient::DEFAULTANI( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->DEFAULTANI();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->DEFAULTANI();
}


void GLCharClient::TOSTARTTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOSTARTTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOSTARTTIME();
}


void GLCharClient::TOENDTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOENDTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOENDTIME();
}


void GLCharClient::SetPartRend( DxSkinChar* pRenderChar, EMPIECECHAR emPIECE, BOOL bRender )
{
	if ( pRenderChar )
		pRenderChar->SetPartRend ( emPIECE, bRender );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPartRend ( emPIECE, bRender );
}


void GLCharClient::SetHairColor( WORD wColor )
{
	if ( m_pSkinChar )
		m_pSkinChar->SetHairColor( wColor );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetHairColor( wColor );
}


BOOL GLCharClient::PutPassiveEffect( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir/*=0.0f */ )
{
	BOOL bOk = FALSE;
	bOk = DxEffcharDataMan::GetInstance().PutPassiveEffect ( pSkinChar, szEffFile, pDir, fTarDir );
	if ( FALSE == bOk )
		return bOk;

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		bOk = m_pSkinCharMulti->PutPassiveEffect( szEffFile, pDir, fTarDir  );
	return bOk;
}

//
//void GLCharClient::CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos )
//{
//	if ( m_pSkinChar )
//		m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, vPos );
//
//	if ( m_pSkinCharMulti )
//		m_pSkinCharMulti->CalculateAmbientColor( pd3dDevice, pLandMan, vPos );
//}


BOOL GLCharClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( szAnim, dwFlags );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI ( szAnim, dwFlags );

	return bOK;
}

BOOL GLCharClient::IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( MType, SType, dwFlags, dwSelect );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI (  MType, SType, dwFlags, dwSelect  );

	return bOK;
}


void GLCharClient::SetPositionSkin( DxSkinChar *pSkinChar, const D3DXVECTOR3& vPos )
{
	if ( pSkinChar )
		pSkinChar->SetPosition ( vPos );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPosition ( vPos );
}


HRESULT GLCharClient::FrameMoveSkin( DxSkinChar *pSkinChar, const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/*=TRUE*/, BOOL const bFreeze/*=FALSE*/, const BOOL bAttackMode/*=TRUE*/, const BOOL bBoardRender /*= FALSE */ )
{
	HRESULT hr = S_FALSE;
	hr = pSkinChar->FrameMove ( fTime, fElapsedTime, bNoneOverFrame, matRot, bContinue, bFreeze, bAttackMode, bBoardRender );
	if ( E_FAIL == hr )
		return hr;

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->FrameMove( fTime, fElapsedTime, bNoneOverFrame, matRot , bContinue, bFreeze, bAttackMode, bBoardRender );

	return hr;
}

void GLCharClient::ClearReserveActionForSkinChar( DxSkinChar *pSkinChar )
{
	pSkinChar->ClearReserveAction();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->ClearReserveAction();
}

void GLCharClient::UpdateGeneralEffect()
{
	const STARGETID sTargetID(CROW_PC,m_dwGaeaID, GetPosition());
	SITEM* pCHECKITEM = NULL; 
	// 기존 전신 이펙트 삭제;
	if ( m_sGeneralEffItemID != NATIVEID_NULL() && (pCHECKITEM = GLogicData::GetInstance().GetItem ( m_sGeneralEffItemID )) != NULL)
	{
		if ( DxEffGroupPlayer::GetInstance().FindPassiveEffect(pCHECKITEM->GetGeneralEffect(), sTargetID) )
		{
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect(pCHECKITEM->GetGeneralEffect(), sTargetID);
		}
	}

	char* szInputGeneralEff = NULL;	
	EMSUIT emHighSuitSlot = SUIT_NSIZE;

	// 장착한 아이템 중 검사;
	INT i;
	for ( i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기만 검사한다; 
		if( !IsCurUseArm( EMSLOT(i) ) ) 
			continue;

		SNATIVEID nidITEM = m_CharData.m_PutOnItems[i].sNativeID;
		if (nidITEM == SNATIVEID(false))
		{
			nidITEM = nidITEM;
			if ( nidITEM == SNATIVEID(false) )
				continue;
		}

		pCHECKITEM = GLogicData::GetInstance().GetItem ( nidITEM );
		if ( pCHECKITEM )
		{
			char* szGeneralEff = NULL;
			if ( (szGeneralEff = const_cast<char*>(pCHECKITEM->GetGeneralEffect())) != NULL && strlen(szGeneralEff) > 0)
			{
				m_sGeneralEffItemID = nidITEM;
				DxEffGroupPlayer::GetInstance().PassiveEffect(szGeneralEff
					, GetTransMatrix()
					, sTargetID );
				break;
			}
		}
	}

	if ( static_cast<INT>(i) == SUIT_NSIZE )
	{
		m_sGeneralEffItemID = NATIVEID_NULL();
	}
}

WORD GLCharClient::GetBodyRadius ()
{
	return GETBODYRADIUS();
}


void GLCharClient::DisableDebuffSkillFact()
{
	const D3DXVECTOR3& _vPosition = GetPosition();
	for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		// 적이 건 스킬 효과만 제거
		if ( pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY  )
        {
            FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,_vPosition), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

			m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, m_sSKILLFACT[ i ].sNATIVEID.dwID, i );
			RESETSKEFF( i );
        }
    }
}

void GLCharClient::DisableSkillFact()
{
	EMSLOT emRHand = GetCurRHand();	
	SITEM* pRightItem = GET_SLOT_ITEMDATA(emRHand);	
	const D3DXVECTOR3& _vPosition = GetPosition();
	
	for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
	{
		if ( m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL() ) continue;
	
        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
		if ( !pSkill ) continue;

		// 스킬 자신 버프
		if ( pSkill->m_sBASIC.emIMPACT_TAR != TAR_SELF || pSkill->m_sBASIC.emIMPACT_REALM != REALM_SELF ) continue;

		const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
		SKILL::GLSKILL_ATT emSKILL_RITEM = pSkill->m_sBASIC.emUSE_RITEM;

		// 스킬 도구 종속 없음
		if ( emSKILL_RITEM == SKILL::SKILLATT_NOCARE )	continue;

		// 스킬 과 무기가 불일치
		if( !pRightItem || !CHECHSKILL_ITEM(emSKILL_RITEM,CONVERT_ITEMATT( pRightItem->sSuitOp.emAttack ),bHiddenWeapon) )
		{
			FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC, m_dwGaeaID, _vPosition), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

			m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, m_sSKILLFACT[ i ].sNATIVEID.dwID, i );
			RESETSKEFF( i );
		}
	}
}


void GLCharClient::ReSelectAnimation ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHAND = NULL;
	SITEM* pLHAND = NULL;

	if ( m_CharData.m_PutOnItems[emRHand].sNativeID!=NATIVEID_NULL() )
		pRHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emRHand].sNativeID );
	
	if ( m_CharData.m_PutOnItems[emLHand].sNativeID!=NATIVEID_NULL() )
		pLHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emLHand].sNativeID );

	m_emANISUBTYPE = CHECK_ANISUB ( pRHAND, pLHAND, CharClassToIndex( GetClass() ) );

	// 제스처중이면 애니메이션 리셋 안함.
	if ( isAction(GLAT_TALK, GLAT_GATHERING) )
	{
		// 에니메이션 초기화.
		//
		SELECTANI ( m_pSkinChar, m_pSkinChar->GETCURMTYPE(), m_emANISUBTYPE );
	}
}

// void GLCharClient::SetPosition ( const D3DXVECTOR3 &vPos )
// {
// 
// 	ClientActor::SetPosition(vPos);	
// 	m_actorMove.SetPosition( vPos, -1 );
// 	if ( m_actorMove.PathIsActive() )
// 		m_actorMove.Stop();
// }

void GLCharClient::TurnAction ( EMACTIONTYPE toAction )
{
	// 이전 액션 취소.
	//
	switch ( GetAction() )
	{
	case GLAT_IDLE:
		{
			if( toAction == GLAT_FEIGN_DIE )
			{
				SetActState( EM_ACT_FEIGN_DIE );
			}
		}
		break;

	case GLAT_FALLING:
		{
			if ( toAction==GLAT_FALLING )	return;
		}		
		break;
		
	case GLAT_DIE:
		{	
			if ( toAction==GLAT_FALLING || toAction==GLAT_DIE )	return;
		}
		break;

	case GLAT_CONFT_END:
		ReSetActState(EM_ACT_CONFT_WIN);
		break;

	case GLAT_SKILL:
		ResetHiddenWeaponPiece();
		break;
	case GLAT_FEIGN_FALLING :
	case GLAT_FEIGN_DIE		:
		{
			if( toAction == GLAT_DIE ||
				toAction == GLAT_FALLING )
			{
				ReSetActState(EM_ACT_FEIGN_DIE);
			}
			else
			if( toAction == GLAT_SHOCK )
			{
				return;
			}
		}
		break;
	default:
		break;
	};

	// 액션 초기화.
	//
	//SetAction(toAction);	
	SetAction( toAction );

	switch ( toAction )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;

			std::tr1::shared_ptr<GLCharClient> pCharClient;
			if( m_emVehicle == EMVEHICLE_DRIVER )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
					if( pCharClient )
					{
						pCharClient->TurnAction( GLAT_IDLE );
					}
					else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(i) )
					{
						pCharacter->TurnAction( GLAT_IDLE );
					}
				}
			}
		}
		break;

	case GLAT_MOVE:
		{
			std::tr1::shared_ptr<GLCharClient> pCharClient;
			if( m_emVehicle == EMVEHICLE_DRIVER )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
					if( pCharClient )
					{
						pCharClient->TurnAction( GLAT_MOVE );
					}
					else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(i) )
					{
						pCharacter->TurnAction( GLAT_MOVE );
					}
				}
			}

		}
		break;

	case GLAT_PUSHPULL:
		{
			std::tr1::shared_ptr<GLCharClient> pCharClient;
			if( m_emVehicle == EMVEHICLE_DRIVER )
			{
				GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
				for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
					if( pCharClient )
					{
						pCharClient->TurnAction( GLAT_PUSHPULL );
					}
					else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(i) )
					{
						pCharacter->TurnAction( GLAT_PUSHPULL );
					}
				}
			}
		}
		break;

	case GLAT_ATTACK:

		// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
		if ( m_pSkinChar )
		{
			SELECTANI( m_pSkinChar, AN_GUARD_N, m_emANISUBTYPE );
		}

		StartAttackProc ();
		break;

	case GLAT_SKILL_WAIT:
		{
			// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
			if ( m_pSkinChar )
			{
				SELECTANI( m_pSkinChar, AN_GUARD_N, m_emANISUBTYPE );
			}

			// 스킬 정보 가져옴.
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_idACTIVESKILL.wMainID, m_idACTIVESKILL.wSubID );
			if ( !pSkill )								return;

			if( m_sVehicle.PassengerId(0) == m_dwGaeaID )
			{
				if( m_idACTIVESKILL.wMainID == EMSKILL_BIKE )
				{
					SITEM* pItem = NULL;
					pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
					m_emANIMAINSKILL = EMANI_MAINTYPE ( AN_BIKE_A +  pItem->sVehicle.emPartsType );
					m_emANISUBSKILL = pSkill->m_sEXT_DATA.emANISTYPE;
					m_emANIVehicleSKILL = pSkill->m_sEXT_DATA.emANIVehicleMTYPE;
				}
				else
				{
					m_emANIMAINSKILL = pSkill->m_sEXT_DATA.emANIMTYPE;
					m_emANISUBSKILL = pSkill->m_sEXT_DATA.emANISTYPE;
					m_emANIVehicleSKILL = AN_VEHICLE_STAY;
				}
			}
			else
			{
				m_emANIMAINSKILL = pSkill->m_sEXT_DATA.emANIMTYPE;
				m_emANISUBSKILL = pSkill->m_sEXT_DATA.emANISTYPE;
			}

			// 방향을 전환시킨다;
			const D3DXVECTOR3 vReDirect = UpdateSkillDirection(
				m_pGaeaClient,
				GetPosition(),
				GetDirect(),
				m_idACTIVESKILL,
				m_vTARPOS,
				m_sTARIDS );
			ClientActor::SetDirect( vReDirect );
		}
		break;

	case GLAT_SKILL:
		{
			SetHiddenWeaponPiece();
			StartSkillProc ();
		}
		break;	
	case GLAT_CONFT_END:
		break;
	case GLAT_FALLING:
		{
			FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );
		}		
		break;
	case GLAT_DIE:
		{
			FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );
		}		
		break;

	case GLAT_GATHERING:		
		break;

	case GLAT_MOTION:
		break;
	case GLAT_FEIGN_FALLING :
	case GLAT_FEIGN_DIE		:
		break;
	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};
	
	if ( ClientActor::PathIsActive() )
	{
		if ( !isAction(GLAT_MOVE, GLAT_PUSHPULL) )
			ClientActor::StopActorMove();
	}
}
// 
// void GLCharClient::SetAction( EMACTIONTYPE toAction )
// {
// 	TurnAction( toAction );
// }

HRESULT GLCharClient::UpdateAnimation ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	BOOL bPeaceZone = m_pGaeaClient->GetActiveMap()->IsPeaceZone();
	if ( !bPeaceZone && IsActState(EM_ACT_PEACEMODE) )		bPeaceZone = TRUE;

	BOOL bLowSP = FALSE;
	BOOL bFreeze = FALSE;

	EMANI_MAINTYPE emMType;
	EMANI_SUBTYPE emSType;

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	if ( pRenderChar == NULL )
		return E_FAIL;

	if ( m_emVehicle && m_sVehicle.m_emTYPE == VEHICLE_TYPE_BOARD )
	{
		emSType = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + m_sVehicle.m_emTYPE);
		//emSType = AN_SUB_HOVERBOARD;
	}
	else
	{
		emSType = AN_SUB_NONE;
	}

	switch ( GetAction() )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime += fElapsedTime;

			BOOL bUseBoost = FALSE;
			BOOL* pOnceBoost = NULL;
			if ( bPeaceZone && pRenderChar->GETANI ( AN_PLACID, AN_SUB_NONE ) )
			{
				emMType = AN_PLACID;
			}
			else
			{
				BOOL bPANT(FALSE); // 헐떡거림.

				if ( pRenderChar->GETANI ( AN_GUARD_L, m_emANISUBTYPE ) )
				{
					bPANT = bLowSP || m_bSTATE_PANT;
					for ( int i=0; i<EMBLOW_MULTI; ++i )
					{
						if ( m_sSTATEBLOWS[i].emBLOW == EMBLOW_STUN || m_sSTATEBLOWS[i].emBLOW == EMBLOW_MAD )		bPANT = TRUE;
					}
				}

				if ( bPANT )
				{
					emMType = AN_GUARD_L;
					emSType = m_emANISUBTYPE;
				}
				else
				{
					emMType = AN_GUARD_N;
					emSType = m_emANISUBTYPE;
				}
			}

			std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			SITEM* pItem = NULL;

			if( pCharClient )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharClient->GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
				bUseBoost = pCharClient->VehicleUseBoost();
				pOnceBoost = pCharClient->VehicleOnceBoostRef();
			}
			else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(0) )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharacter->GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
				bUseBoost = pCharacter->VehicleUseBoost();
				pOnceBoost = pCharacter->VehicleOnceBoostRef();
			}
			if ( pItem && pItem->VehicleType() != VEHICLE_TYPE_BOARD ) 
			{
				switch ( pItem->VehicleType() )
				{
				case VEHICLE_TYPE_BIKE:
					emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_CAR:
					emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_REARCAR:
					emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
					break;
				}
				
				emSType = (m_emVehicle == EMVEHICLE_DRIVER) ? AN_SUB_DRIVER_STOP : AN_SUB_PASSENGER_STOP;
				if( bUseBoost )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						if( pOnceBoost && (*pOnceBoost) == TRUE )
						{
							emSType = AN_SUB_DRIVER_BOOST;		
						}							
					}
					else if( m_emVehicle == EMVEHICLE_PASSENGER )
					{
						emSType = AN_SUB_PASSENGER_WALK;
					}
				}
			}

			if ( m_sHALLUCINATE.bEmoticon )
			{
				if ( !IsANI_SELECTANI( pRenderChar, AN_GESTURE, m_sHALLUCINATE.emAni_SubType) )
				{
					SELECTANI ( pRenderChar, emMType, emSType );
				}
			}
			else
			{
				SELECTANI ( pRenderChar, emMType, emSType );
			}

			if( m_sVehicle.m_pSkinChar )
			{
				if( bUseBoost && pOnceBoost )
				{
					if( (*pOnceBoost) == TRUE )
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_BOOSTER );			
						if( m_sVehicle.m_pSkinChar->IsANI_ISENDANIM( AN_VEHICLE_BOOSTER ) )
						{
							(*pOnceBoost) = FALSE;
						}
					}
					else
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_STAY );
					}
				}
				else
				{
					m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_STAY );			
				}
			}
		}
		break;

	case GLAT_MOVE:
		{
			emMType = IsActState(EM_ACT_RUN) ? AN_RUN : AN_WALK;
			if ( !m_emVehicle )
			{
				emSType = bPeaceZone ? AN_SUB_NONE : m_emANISUBTYPE;
			}

			std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
			SITEM* pItem = NULL;
			BOOL bUseBoost = FALSE;
			BOOL* pOnceBoost = NULL;

			if( pCharClient )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharClient->GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
				bUseBoost = pCharClient->VehicleUseBoost();
				pOnceBoost = pCharClient->VehicleOnceBoostRef();
			}
			else if( pCharacter && pCharacter->GetGaeaID() == m_sVehicle.PassengerId(0) )
			{
				pItem = GLogicData::GetInstance().GetItem ( pCharacter->GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
				bUseBoost = pCharacter->VehicleUseBoost();
				pOnceBoost = pCharacter->VehicleOnceBoostRef();
			}
			if ( pItem && pItem->VehicleType() != VEHICLE_TYPE_BOARD ) 
			{				
				switch ( pItem->VehicleType() )
				{
				case VEHICLE_TYPE_BIKE:
					emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_CAR:
					emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
					break;
				case VEHICLE_TYPE_REARCAR:
					emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
					break;
				}
				if ( m_emVehicle == EMVEHICLE_DRIVER )
					emSType = IsActState(EM_ACT_RUN) ? AN_SUB_DRIVER_RUN : AN_SUB_DRIVER_WALK;
				else // else if(EMVEHICLE_PASSENGER)
					emSType = IsActState(EM_ACT_RUN) ? AN_SUB_PASSENGER_RUN : AN_SUB_PASSENGER_WALK;

				if( bUseBoost )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						if( pOnceBoost && (*pOnceBoost) == TRUE )
						{
							emSType = AN_SUB_DRIVER_BOOST;			
						}							
					}
					else if( m_emVehicle == EMVEHICLE_PASSENGER )
					{
						emSType = AN_SUB_PASSENGER_WALK;
					}
				}								
			}

			SELECTANI ( pRenderChar, emMType, emSType );

			if( m_sVehicle.m_pSkinChar )
			{
				if( bUseBoost && pOnceBoost )
				{
					if( (*pOnceBoost) == TRUE )
					{
						m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_BOOSTER );			
						if( m_sVehicle.m_pSkinChar->IsANI_ISENDANIM( AN_VEHICLE_BOOSTER ) )
						{
							(*pOnceBoost) = FALSE;
						}
					}
					else
					{
						m_sVehicle.m_pSkinChar->SELECTANI( IsActState(EM_ACT_RUN) ? AN_VEHICLE_RUN : AN_VEHICLE_WALK );
					}
				}
				else 
				{
					m_sVehicle.m_pSkinChar->SELECTANI( IsActState(EM_ACT_RUN) ? AN_VEHICLE_RUN : AN_VEHICLE_WALK );
				}					
			}
		}
		break;

	case GLAT_ATTACK:
		{
			EMSLOT emRHand = GetCurRHand();
			EMSLOT emLHand = GetCurLHand();

			SITEM* pRHAND = NULL;
			SITEM* pLHAND = NULL;

			if ( m_CharData.m_PutOnItems[emRHand].sNativeID!=NATIVEID_NULL() )
				pRHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emRHand].sNativeID );
			
			if ( m_CharData.m_PutOnItems[emLHand].sNativeID!=NATIVEID_NULL() )
				pLHAND = GLogicData::GetInstance().GetItem ( m_CharData.m_PutOnItems[emLHand].sNativeID );

			EMANI_SUBTYPE emANISUBTYPE = CHECK_ATTACK_ANISUB ( pRHAND, pLHAND, CharClassToIndex( GetClass() ) );
			SELECTANI ( pRenderChar, AN_ATTACK, emANISUBTYPE, NULL, m_dwANISUBSELECT );
			if ( pRenderChar->IsANI_ISENDANIM ( AN_ATTACK, emANISUBTYPE, m_dwANISUBSELECT ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_SKILL_WAIT:
		{
			SELECTANI ( pRenderChar, m_emANIMAINSKILL, m_emANISUBSKILL );
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->SELECTANI( m_emANIVehicleSKILL );
			}

			// 애니메이션이 로드 되어서 작동이 된다.
			if ( pRenderChar->GETCURMTYPE() == m_emANIMAINSKILL && pRenderChar->GETCURSTYPE() == m_emANISUBSKILL )
			{
				// 스킬을 실행한다;
				TurnAction ( GLAT_SKILL );
			}
		}
		break;

	case GLAT_SKILL:
		{
			SELECTANI ( pRenderChar, m_emANIMAINSKILL, m_emANISUBSKILL );
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->SELECTANI( m_emANIVehicleSKILL );
			}

			if ( m_pSkinChar->IsANI_ISENDANIM ( m_emANIMAINSKILL, m_emANISUBSKILL ) )
			{
				TurnAction ( GLAT_IDLE );
			}
		}
		break;

	case GLAT_TALK:
		{
			BOOL bOK = IsANI_SELECTANI ( pRenderChar, AN_GESTURE, EMANI_SUBTYPE(m_dwANISUBGESTURE) );
			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );
				break;
			}
			
			if ( !m_pSkinChar->IsCurAniConInfo(ACF_LOOP) && pRenderChar->IsANI_ISENDANIM ( AN_GESTURE, EMANI_SUBTYPE(m_dwANISUBGESTURE) ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_SHOCK:
	
		if ( !m_emVehicle )
		{
			emSType = m_emANISUBTYPE;
		}

		SELECTANI ( pRenderChar, AN_SHOCK, emSType );
//		if ( m_pSkinChar->IsANI_ISENDANIM () )	TurnAction ( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		if ( m_emVehicle || !IsANI_SELECTANI ( pRenderChar, AN_SPEC, (EMANI_SUBTYPE)m_dwANISUBSELECT ) )
		{
			SELECTANI ( pRenderChar, AN_GUARD_N, (EMANI_SUBTYPE)emSType );
		}
		break;

	case GLAT_FALLING:
		emMType = AN_DIE;
		emSType = AN_SUB_NONE;
		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );
			
			SITEM* pItem = NULL;
			pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
			if( pItem )
			{
				if( pItem->VehicleType() != VEHICLE_TYPE_BIKE )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						switch ( pItem->VehicleType() )
						{
						case VEHICLE_TYPE_BIKE:
							emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
							break;
						case VEHICLE_TYPE_CAR:
							emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
							break;
						case VEHICLE_TYPE_REARCAR:
							emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
							break;
						}
						emSType = AN_SUB_DRIVER_DIE; 
					}
				}
			}
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}
		else
		{
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );			
		}

		if ( pRenderChar->IsANI_ISENDANIM ( emMType, emSType ) )
		{
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->TOENDTIME();
			}
			TurnAction ( GLAT_DIE );		
		}
		break;
	case GLAT_FEIGN_FALLING:
		emMType = AN_DIE;
		emSType = AN_SUB_NONE;
		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );

			SITEM* pItem = NULL;
			pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).sNativeID );
			if( pItem )
			{
				if( pItem->VehicleType() != VEHICLE_TYPE_BIKE )
				{
					if( m_emVehicle == EMVEHICLE_DRIVER )
					{
						switch ( pItem->VehicleType() )
						{
						case VEHICLE_TYPE_BIKE:
							emMType = EMANI_MAINTYPE( AN_BIKE_A +  pItem->sVehicle.emPartsType );
							break;
						case VEHICLE_TYPE_CAR:
							emMType = EMANI_MAINTYPE( AN_CAR_A + pItem->sVehicle.emPartsType );
							break;
						case VEHICLE_TYPE_REARCAR:
							emMType = EMANI_MAINTYPE( AN_REARCAR_A + pItem->sVehicle.emPartsType );
							break;
						}
						emSType = AN_SUB_DRIVER_DIE; 
					}
				}
			}
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );
		}
		else
		{
			SELECTANI ( pRenderChar, emMType, emSType, EMANI_ENDFREEZE );			
		}

		if ( pRenderChar->IsANI_ISENDANIM ( emMType, emSType ) )
		{
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->TOENDTIME();
			}
			TurnAction ( GLAT_FEIGN_DIE );
		}
		break;
	case GLAT_FEIGN_DIE :
		{
			bFreeze = TRUE;
			TOENDTIME( pRenderChar );
			if( m_sVehicle.m_pSkinChar )
			{
				m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );
				m_sVehicle.m_pSkinChar->TOENDTIME();
			}
			else
			{
				SELECTANI ( pRenderChar, AN_DIE, emSType, EMANI_ENDFREEZE );
				TOENDTIME(pRenderChar);
			}			
		}
		break;
	case GLAT_DIE:
		bFreeze = TRUE;
		TOENDTIME( pRenderChar );
		if( m_sVehicle.m_pSkinChar )
		{
			m_sVehicle.m_pSkinChar->SELECTANI( AN_VEHICLE_DOWN );
			m_sVehicle.m_pSkinChar->TOENDTIME();
		}
		else
		{
			SELECTANI ( pRenderChar, AN_DIE, emSType, EMANI_ENDFREEZE );
			TOENDTIME(pRenderChar);
		}
		break;

	case GLAT_CONFT_END:
		{
			EMANI_MAINTYPE emMTYPE(AN_CONFT_LOSS);
			if ( IsActState(EM_ACT_CONFT_WIN) )
				emMTYPE = AN_CONFT_WIN;

			SELECTANI ( pRenderChar, emMTYPE, AN_SUB_NONE );

			if ( pRenderChar->IsANI_ISENDANIM ( emMTYPE, AN_SUB_NONE ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_GATHERING:
		{
			BOOL bOK = IsANI_SELECTANI ( pRenderChar, AN_GATHERING, EMANI_SUBTYPE(m_dwANISUBGESTURE) );

			if ( !bOK )
			{
				TurnAction ( GLAT_IDLE );
				break;
			}
			
			if ( !m_pSkinChar->IsCurAniConInfo(ACF_LOOP) && pRenderChar->IsANI_ISENDANIM ( AN_GATHERING, EMANI_SUBTYPE(m_dwANISUBGESTURE) ) )
				TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_MOTION:
		{
			SELECTANI( pRenderChar, m_eMotionMID, m_eMotionSID );

			if( m_fMotionSec == FLT_MAX )
			{
				if( pRenderChar->IsANI_ISENDANIM( m_eMotionMID, m_eMotionSID ) )
				{
					TOSTARTTIME( pRenderChar );
				}
			}
			else
			{
				m_fMotionSec -= fElapsedTime;
				if( m_fMotionSec <= 0.0f )
				{
					TurnAction( GLAT_IDLE );
					m_fMotionSec = FLT_MAX;
				}
				else
				{
					if( pRenderChar->IsANI_ISENDANIM( m_eMotionMID, m_eMotionSID ) )
					{
						TOSTARTTIME( pRenderChar );
					}
				}
			}
		}
		break;
	};

	// 스킨 업데이트.
	//
	SetPositionSkin( pRenderChar, GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );

	BOOL bContinue = isAction(GLAT_ATTACK, GLAT_SKILL);

	// 상태 이상에 따라 모션 속도를 조정한다.
	//
	float fSkinAniElap = fElapsedTime;
	switch ( GetAction() )
	{
	case GLAT_MOVE:
		{
			if ( VehicleState() == EMVEHICLE_OFF )
			{
				fSkinAniElap *= GETMOVE_TOTAL();
			}
		}
		break;

	case GLAT_ATTACK:
	case GLAT_SKILL:
		fSkinAniElap *= (GETATTVELO() + GETATT_ITEM());
		break;
	};

	CalculateMatrix( GetPosition() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f) );


	// CalculateMatrix 에서 Matrix 값은 다 얻었다.
	// 여기는 그 계산된 Matrix 값을 넣는 장소
	if ( !IsActState(EM_REQ_VISIBLENONE) )
	{
		// 바이크 타면 언제가 되었든 계산은 되야 한다.
		if ( m_emVehicle == EMVEHICLE_DRIVER )
		{
			DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;

			// 드라이버면 나와 내가 태운 캐릭터의 FrameMove 를 해야 한다.
			// Vehicle을 탔지만 드라이버가 아닐경우는 계산하지 않는다. 다른곳에서 하기 때문.
			if( pVehicle )
			{
				// 내 캐릭터가 타고 있는가~?
				// 내가 타고 있다면 bNoneOverFrame 을 TRUE 로 줘서 떨림이 없도록 한다.
				BOOL bInGLCharacter(FALSE);

				// Vehicle 에 탄 Char FrameMove
				for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
				{
					DWORD dwPassengerID = m_sVehicle.PassengerId(i);
					if ( m_pGaeaClient->GETMYGAEAID() == dwPassengerID )
					{
						bInGLCharacter = TRUE;
						break;
					}
				}

				// Vehicle FrameMove
				pVehicle->FrameMove ( fTime, fSkinAniElap, bInGLCharacter, m_matBikeTrans, bContinue, bFreeze );

				// Vehicle 에 탄 Char FrameMove
				for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
				{
					DWORD dwPassengerID = m_sVehicle.PassengerId(i);
					if ( m_pGaeaClient->GETMYGAEAID() == dwPassengerID )
					{
						GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
						pCharacter->GetSkinChar()->FrameMoveCharOnVehicle ( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone, pVehicle, i, bInGLCharacter );
					}
					else
					{
						std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( dwPassengerID );
						if( pCharClient )
						{
							pCharClient->GetSkinChar()->FrameMoveCharOnVehicle ( fTime, fSkinAniElap, bContinue, bFreeze, !bPeaceZone, pVehicle, i, bInGLCharacter );
						}
					}
				}
			}
			else
			{
				// pVehicle가 NULL 이라면, 보드를 타고 있을 경우임.

				if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME || RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
				{
					// [shhan][2014.12.19] 이걸 하지 않으면 m_vecReserveFunction 에 값이 계속 누적되어서 메모리 부족현상이 나타난다.
					//
					// 렌더하지 않는다.
					//
					ClearReserveActionForSkinChar( pRenderChar );
				}
				else
				{
					FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, FALSE, GetTransMatrix(), bContinue, bFreeze, !bPeaceZone, FALSE );
				}
			}
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )	// 탑승자일 경우 바이크 주인(GLCharacter,GLCharClient) 에서 FrameMove 를 해준다.
		{
			// 아무작업도 안함.
		}
		else	// 아무것도 탑승하지 않는 상태임.
		{
			if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME || RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
			{
				// [shhan][2014.12.19] 이걸 하지 않으면 m_vecReserveFunction 에 값이 계속 누적되어서 메모리 부족현상이 나타난다.
				//
				// 렌더하지 않는다.
				//
				ClearReserveActionForSkinChar( pRenderChar );
			}
			else
			{
				FrameMoveSkin ( pRenderChar, fTime, fSkinAniElap, FALSE, GetTransMatrix(), bContinue, bFreeze, !bPeaceZone, FALSE );
			}
		}
	}

	// 비 전투 지역일때 표시 안되야 하는 아이템(단검,투척) 인지 검사후 랜더 끄기.
	//
	if ( bPeaceZone )
	{
		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();

		SITEM *pItemR(NULL);
		SITEM *pItemL(NULL);

		if ( VALID_SLOT_ITEM(emRHand) )
		{
			pItemR = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM(emRHand).sNativeID );
		}

		if ( VALID_SLOT_ITEM(emLHand) )
		{
			pItemL = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM(emLHand).sNativeID );
		}

		if ( pItemR )
		{
			BOOL bRend = !( pItemR->sSuitOp.emAttack==ITEMATT_DAGGER || pItemR->sSuitOp.emAttack==ITEMATT_THROW );
			SetPartRend ( pRenderChar, PIECE_RHAND, bRend );
		}

		if ( pItemL )
		{
			BOOL bRend = !( pItemL->sSuitOp.emAttack==ITEMATT_DAGGER || pItemL->sSuitOp.emAttack==ITEMATT_THROW );
			SetPartRend ( pRenderChar, PIECE_LHAND, bRend );
		}
	}
	else
	{
		SetPartRend ( pRenderChar, PIECE_RHAND, TRUE );
		SetPartRend ( pRenderChar, PIECE_LHAND, TRUE );
	}

	return S_OK;
}

BOOL GLCharClient::IsCollisionVolume ()
{
	const CLIPVOLUME& CV = DxViewPort::GetInstance().GetClipVolume();
	if (COLLISION::IsCollisionVolume ( CV, m_vMax, m_vMin ))
        return TRUE;
    else
        return FALSE;
}

float GLCharClient::GETATTVELO()
{
    return GLOGICEX::GetAttackVelocity(m_fATTVELO, m_fITEMATTVELO_R);
	//float fATTVELO = m_fATTVELO + m_fITEMATTVELO_R;
	//return fATTVELO<0.0f?0.0f:fATTVELO;
}

float GLCharClient::GETMOVEVELO()
{
    return GLOGICEX::GetMoveBaseVelocity(m_fSTATE_MOVE_RATE, m_fSKILL_MOVE, m_fITEM_MOVE_R, m_fOPTION_MOVE);
	//float fMOVE = m_fSTATE_MOVE_RATE + m_fSKILL_MOVE + m_fITEM_MOVE_R; 
	//return fMOVE<0.0f?0.0f:fMOVE;
}

float GLCharClient::GETATT_ITEM ()
{
	float fATTVELO = m_fITEMATTVELO / 100;
	return fATTVELO;
}

float GLCharClient::GETMOVE_ITEM ()
{
	float fMOVE = m_fITEM_MOVE / GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO;
	return fMOVE<0.0f?0.0f:fMOVE;
}

float GLCharClient::GetMoveVelo()
{
    return GLOGICEX::GetMoveVelocity(
        IsActState(EM_ACT_RUN),
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO,
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO,
        m_sVehicle.m_bUseBoost ? true : false,
        GETMOVEVELO(),
        GETMOVE_ITEM(),
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max,
        IsCDMSafeTime(),
        GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED);

    /*
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = 0.0f;

	if (m_sVehicle.m_bUseBoost)
	{
		fMoveVelo = fDefaultVelo * ((GETMOVEVELO() + GETMOVE_ITEM()) * 1.5f);
	}
	else
	{
		fMoveVelo = fDefaultVelo * (GETMOVEVELO() + GETMOVE_ITEM());
	}
	
	if (IsCDMSafeTime())
        fMoveVelo = fMoveVelo * GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED;
	return fMoveVelo;
    */
}

void GLCharClient::UpdateSpecialSkill()
{
	// 변신 스킬 체크
	if( m_dwTransformSkill != SNATIVEID::ID_NULL )
	{
		if( m_sSKILLFACT[m_dwTransformSkill].IsSpecialSkill( SKILL::EMSSTYPE_TRANSFORM ) )
		{
			const D3DXVECTOR3& _vPosition = GetPositionActor();
			// 쓰러지는 모션이면 변신해제
			STARGETID sTargetID(CROW_PC,m_dwGaeaID, _vPosition);
			if( isAction( GLAT_FALLING ) )
			{
				FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID, _vPosition), m_pSkinChar, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID );
			}

			if( m_sSKILLFACT[m_dwTransformSkill].sNATIVEID != NATIVEID_NULL() ) 
			{			
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wSubID );
				if( pSkill && pSkill->m_sSPECIAL_SKILL.emSSTYPE == SKILL::EMSSTYPE_TRANSFORM )
				{
					if( pSkill->m_sSPECIAL_SKILL.strEffectName.size() != 0 )
					{
						if( m_sSKILLFACT[m_dwTransformSkill].fAGE <= pSkill->m_sSPECIAL_SKILL.dwRemainSecond && 
							!m_sSKILLFACT[m_dwTransformSkill].bRanderSpecialEffect )
						{
							D3DXMATRIX matTrans;							
							D3DXMatrixTranslation ( &matTrans, _vPosition.x, _vPosition.y, _vPosition.z );
							DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sSPECIAL_SKILL.strEffectName.c_str(), matTrans, &sTargetID, TRUE, FALSE );
							m_sSKILLFACT[m_dwTransformSkill].bRanderSpecialEffect = TRUE;
						}
					}
				}
			}
			else
			{
				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
					m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID.dwID, m_dwTransformSkill );

				RESETSKEFF(m_dwTransformSkill);
				UpdateSuit();				
				m_dwTransformSkill = SNATIVEID::ID_NULL;
			}


		}
		else
		{
			if( m_IdSkillTransform != NATIVEID_NULL() )
			{
				//m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
				//m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID.dwID, m_dwTransformSkill );
				m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
					m_IdSkillTransform.dwID, m_dwTransformSkill );

				if( m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID == m_IdSkillTransform )
					RESETSKEFF(m_dwTransformSkill);

				UpdateSuit();
				m_dwTransformSkill = SNATIVEID::ID_NULL;
				m_IdSkillTransform = NATIVEID_NULL();
			}

			/*
			m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, 
				m_sSKILLFACT[ m_dwTransformSkill ].sNATIVEID.dwID, m_dwTransformSkill );

			RESETSKEFF(m_dwTransformSkill);
			UpdateSuit();
			m_dwTransformSkill = SNATIVEID::ID_NULL;
			*/
		}
	}
}


/**
 * 지속 효과의 시간이 만료된 경우 호출 되는 함수; 
 * @ Param dwSkillfactIndex		[in] 만료된 지속효과의 인덱스;
**/
void GLCharClient::UPDATE_DATA_END(const DWORD dwSkillfactIndex, BOOL bTimeDel, BOOL bForceEnd )
{
	if ( dwSkillfactIndex >= SKILLFACT_SIZE )
		return;

	SSKILLFACT& sSKILLFACT = m_sSKILLFACT[dwSkillfactIndex];
	if ( sSKILLFACT.sNATIVEID==NATIVEID_NULL() )
		return;
	if ( !sSKILLFACT.bEnable )
		return;

	const size_t nSize = sSKILLFACT.vecSPEC.size();
	for ( unsigned int _i = 0; _i < nSize; ++_i )
	{
		const SKILL::SSPEC_ADDON_LEVEL& spec = sSKILLFACT.vecSPEC[_i];
		switch ( spec.emSPEC )
		{
		case EMSPECA_COUNTATTACK :
			{	
				m_SkillCAIndex.AllReset();
			}
			break;
		case EMSPECA_INVISIBLE:
			if ( m_sINVISIBLE.bPrevStateRun )
				SetActState(EM_ACT_RUN);
			else
				ReSetActState(EM_ACT_RUN);
			break;
		case EMSPECA_HALLUCINATE:
            {
                DxSkinChar* pRenderChar = NULL;
			    if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) != NULL )
			    {
				    pRenderChar->DEFAULTANI();
				    pRenderChar->StopLoopSound();
			    }
            }
			break;
        case EMSPECA_TAUNT:
			// [shhan][2015.03.30] 타겟과 모션은 초기화 할 필요가 있는가~?
			//						rm #1029 - 괜히 EMSPECA_TAUNT 종료시 다른유저가 보았을 때 움직임이 멈춰보일 수 있다.
            ////도발이 끝나면 타겟을 푼다.
            //TurnAction(GLAT_IDLE);
            //m_sTargetID = TARGETID_NULL;
            break;
		case EMSPECA_FEIGN_DEATH :
			RF_FEIGN_DEATH( m_EffSkillVarSet ).End( dynamic_cast<GLCHARLOGIC*>(this), bForceEnd );
			break;
		case EMSPECA_SCREEN_THROW :
			RF_SCREEN_THROW( m_EffSkillVarSet ).ResetValue(spec.sSPEC.fVAR1);
			break;
		case EMSPECA_CAMERA_HIT :
			RF_CAMERA_HIT( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_DISGUISE :
			RF_DISGUISE( m_EffSkillVarSet ).ResetValue();

			Stop_Disguise( );
			break;
		case EMSPECA_PROVOKE :
			RF_PROVOKE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_SPECIAL_IMMUNE :
			RF_IMMUNE( m_EffSkillVarSet ).ResetValue();
			break;
		case EMSPECA_ACTIVATE_DAMAGE :
			RF_ACTIVE( m_EffSkillVarSet ).ResetValue();
			break;
		}		
	}

	FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_PC,m_dwGaeaID, GetPosition()), m_pSkinChar, sSKILLFACT.sNATIVEID);

	m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, sSKILLFACT.sNATIVEID.dwID, dwSkillfactIndex );

	sSKILLFACT.RESET();
}


HRESULT GLCharClient::FrameMove ( const boost::shared_ptr<DxLandMan>& spLandMan, float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;
	
	// 지형 효과 업데이트
	UpdateLandEffect();

	//	맵 효과 업데이트
	UpdateMapEffect();

	// Note : 도시락 사용 가능한 지역이면 모든 LUNCHBOX를 Enable 한다
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
		return S_OK;

	BOOL bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
	if ( IsValidBody() && !bLunchBoxForbid )
	{
		SETENABLEALLLUNCHBOX ( TRUE );
	}
	else
	{
		// Note : 모든 LUNCHBOX를 Disable 한다
		SETENABLEALLLUNCHBOX ( FALSE );
	}

    // 스턴중이라도 밀고당기기 액션 중이라면 움직임을 멈추지 않는다
    if ( m_bSTATE_STUN && !isAction( GLAT_PUSHPULL ) )
    {
        if ( ClientActor::PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
        {
            ClientActor::StopActorMove();
            TurnAction( GLAT_SHOCK );
        }
    }

	// 상태 갱신.
	//
	m_sAirborne.update(fElapsedTime);
	UPDATE_DATA ( fTime, fElapsedTime, TRUE );

	if( RF_FEIGN_DEATH( m_EffSkillVarSet).IsOn() )	
	{
		if( !IsActState( EM_ACT_FEIGN_DIE) ){
			SetActState( EM_ACT_FEIGN_DIE );
			TurnAction( GLAT_FEIGN_FALLING );
		}
	}
	/*
	else
	{
		if( IsActState( EM_ACT_FEIGN_DIE) ){
			ReSetActState(EM_ACT_FEIGN_DIE);
			TurnAction( GLAT_IDLE );
		}
	}*/

	//// 케릭 현재 위치 업대이트.
	////
	//const D3DXVECTOR3& vPosition = ClientActor::RefreshPosition(0.0f);

	if ( IsValidBody() ) 
	{
		if ( IsCDMSafeTime() )
            m_CharData.m_Base.m_fCDMSafeTime -= fElapsedTime;
	}

	// 공격 목표의 유효성 검사.
	//		(주의) 타갯정보 사용전 반드시 호출하여 유효성 검사 필요.
	//
	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sTargetID );
	if ( !pTarget )
	{
		m_sTargetID.GaeaId = EMTARGET_NULL;
		if ( isAction(GLAT_ATTACK) )
			TurnAction ( GLAT_IDLE );
	}

	// 에니메이션, 스킨 업데이트.
	//
	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	if( m_pSkinCharMulti )
	{
		BOOL bUpdateSuit = FALSE;
		BOOL bIsCloneSkill = IsCheckedSkillFlagSpecial(EMSPECA_CLONE);
		bUpdateSuit = m_pSkinCharMulti->SetUseSkinCharMulti( bIsCloneSkill, m_sClone.GetClonNum());

		if ( m_pSkinCharMulti->IsUpdateSuit() && bUpdateSuit )
			UpdateSuit();

		if ( !bIsCloneSkill )
			m_sClone.Reset();
	}

	switch ( GetAction() )
	{
	case GLAT_MOVE:
		{
			// 케릭의 이동 업데이트.
			//
			ClientActor::SetMaxSpeed(GetMoveVelo());
			ClientActor::Update( pLand->GetvecLayerCollision(), fElapsedTime);

			if (m_pLandManClient->IsObstacle(m_actorMove.Position(), m_actorMove.GetPreviousPosition()))
			{
				
				//ClientActor::SetPositionActor(vPosNew);
				//ClientActor::SetPosition(m_actorMove.GetPreviousPosition());
				
				m_actorMove.Stop ();
				ClientActor::SetPositionActor(m_actorMove.GetPreviousPosition());

				
			}
			else
			{

				if ( !ClientActor::PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
				{
					ClientActor::StopActorMove();
					TurnAction ( GLAT_IDLE );
				}
			}



			// 케릭의 현재 위치 업데이트.
			//
			//const D3DXVECTOR3& vRePosition = ClientActor::RefreshPosition(0.0f);

			// 현재 방향 업데이트.
			//
			const D3DXVECTOR3& vMovement = ClientActor::GetNextPosition();
			if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
			{
				D3DXVECTOR3 vDirection = vMovement - ClientActor::GetPositionActor();	//vRePosition;
				if ( !DxIsMinVector(vDirection,0.2f) )
				{
					D3DXVec3Normalize ( &vDirection, &vDirection );
					ClientActor::SetDirect(vDirection);
				}
			}
		}
		break;

	case GLAT_ATTACK:
		{
			AttackProc ( fElapsedTime );

			// 공격 방향으로 회전.
			//
			ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sTargetID );
			if ( pTarget )
			{
				D3DXVECTOR3 vDirection = pTarget->GetPosition() - ClientActor::GetPositionActor();
				D3DXVec3Normalize ( &vDirection, &vDirection );
				ClientActor::SetDirect(vDirection);
			}
		}
		break;

	case GLAT_SKILL:
		{
			SkillProc ( fElapsedTime );

			// 공격 방향으로 회전.
			//
			const D3DXVECTOR3 vReDirect = UpdateSkillDirection(m_pGaeaClient, ClientActor::GetPositionActor(), GetDirect(), m_idACTIVESKILL, m_vTARPOS, m_sTARIDS);
			ClientActor::SetDirect(vReDirect);
		}
		break;

	case GLAT_SHOCK:
		TurnAction( GLAT_IDLE );
		break;

	case GLAT_PUSHPULL:
		{
			ClientActor::Update( pLand->GetvecLayerCollision(), fElapsedTime );			
			if ( !ClientActor::PathIsActive() )
			{				
				if ( m_pSkinChar->IsCurAniConInfo(ACF_LOOP) || pRenderChar->ISENDANIM () )
				{
					ClientActor::StopActorMove();
					TurnAction ( GLAT_IDLE );
				}
			}
		}
		break;
	};	

	{
		const BOOL bPrevState = m_bVisible;
		m_bVisible = CalcVisibleDetect(fTime);	
		if ( m_bVisible == FALSE )
			DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(CROW_PC, m_dwGaeaID), false);
		else if ( m_bVisible != bPrevState )
			DxEffGroupPlayer::GetInstance().RenderEnable(STARGETID(CROW_PC, m_dwGaeaID), true);
	}

	// [AnimationMove]
	SkinAniControl_AnimationMove();

	// [shhan][2015.01.27] RefreshPosition 를 안해주니 PUSHPULL 될 시 캐릭터 형상과 이름, 타켓팅 되는 위치가 따로 분리되는 문제가 생겼다.
	//						m_vPosition 것을 없앨 경우는 PUSHPULL Test 도 같이 해봐야 할것 같음.
	ClientActor::RefreshPosition(0.f);

	// 스킬 이펙트 업데이트.
	//
	FACTEFF::UpdateSkillEffect(
		m_pGaeaClient,
		STARGETID(CROW_PC,m_dwGaeaID, GetPositionActor()),
		m_pSkinChar,
		m_sSKILLFACT,
		SKILLFACT_SIZE,
		m_sSTATEBLOWS);

	if ( RANPARAM::emAroundDetail == EM_AROUND_ORIG )
	{
		UpdateSkillEff();
		UpdateSpecialSkill();
	}

	// 여기서 DxSkinChar 의 최종 FrameMove 가 이루어 진다.
	// 이 시점 부터 렌더링 끝날 때 까지 Part 를 변경하는 작업이 이루어지면 문제가 발생한다.
	UpdateAnimation ( fTime, fElapsedTime );
	


	//////////////////////////////////////////////////////////////////////////
	// Skin 관련 위치를 사용한다. ( vPositionOfSkin ) ( airbone 적용된 값 )
	//////////////////////////////////////////////////////////////////////////

	const D3DXVECTOR3& vPositionOfSkin = ClientActor::GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);

	// AABB 계산.
	//
	m_vMax = vPositionOfSkin + m_vMaxOrg;
	m_vMin = vPositionOfSkin + m_vMinOrg;


	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPositionOfSkin + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );

	if ( m_pLandManClient->IsCullByObjectMap() == true )
	{
		const D3DXVECTOR3 vUserPosition(m_pGaeaClient->GetCharacter()->GetPositionActor() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));
		const D3DXVECTOR3 vCharPosition(GetPositionActor() + D3DXVECTOR3(0.0f, 12.0f, 0.0f));

		//////////////////////////////////////////////////////////////////////////
		//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
		//				airbone 값이 적용된 것을 사용하지는 않는다.
		// Thread 상으로 넘긴다.
		m_spCollisionByObject->InitData( spLandMan );
		NSSkinAniThread::AddCollisionByObject( m_spCollisionByObject, vUserPosition, vCharPosition );
	}


	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spWorldLight->InitData( spLandMan );
	NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPositionOfSkin );

	return S_OK;
}

float GLCharClient::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

void GLCharClient::UpdateCharPos_RenderChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	if ( m_spWorldLight->IsNotCalculateColor() )
		return;

	// 보드일 경우 m_sVehicle.m_pSkinChar 가 NULL 일 수도 있다.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyChar( pd3dDevice, m_sVehicle.m_pSkinChar, TRUE, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );
	m_pSkinCharMulti->RenderMyChar( pd3dDevice, TRUE, bHALF_VISIBLE, m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult, FALSE );
}

void GLCharClient::UpdateCharPos_RenderChar_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	// 보드일 경우 m_sVehicle.m_pSkinChar 가 NULL 일 수도 있다.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyChar( pd3dDevice, m_sVehicle.m_pSkinChar, FALSE, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	RenderMyChar( pd3dDevice, pRenderChar, FALSE, bHALF_VISIBLE );
}

void GLCharClient::UpdateCharPos_RenderChar_MaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	// 보드일 경우 m_sVehicle.m_pSkinChar 가 NULL 일 수도 있다.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyCharMaterialEffect( pd3dDevice, m_sVehicle.m_pSkinChar, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	RenderMyCharMaterialEffect( pd3dDevice, pRenderChar, bHALF_VISIBLE );
}

void GLCharClient::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( !pRenderChar )
		return;

	if ( m_spWorldLight->IsNotCalculateColor() )
		return;

	NSMaterialSkinManager::SetMob( FALSE );
	pRenderChar->SetWorldLight( m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult );

	if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
	{
		if ( bOpaqueState )
		{
			// 아무것도 하지 않는다.
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 0.5f );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 0.5f );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 0.5f );
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 0.5f );
			}
		}		
	}
	else		// 정상렌더
	{
		if ( bOpaqueState )
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_ALL  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f );
				pRenderChar->RenderMaterialHardAlpha ( pd3dDevice, 1.f );
			}
		}
		else
		{
			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
			{
			}
			else
			{
				pRenderChar->RenderMaterialSoftAlpha ( pd3dDevice, 1.f );
			}
		}
	}
}

void GLCharClient::RenderMyCharMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bHALF_VISIBLE )
{	
	if ( !pRenderChar )
		return;

	NSMaterialSkinManager::SetMob( FALSE );

	if ( bHALF_VISIBLE )	// 캐릭터 반투명 렌더
	{
		pRenderChar->RenderMaterialEffect ( pd3dDevice, 0.5f );
	}
	else		// 정상렌더
	{
		pRenderChar->RenderMaterialEffect ( pd3dDevice, 1.f );
	}
}

HRESULT GLCharClient::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	DxLandMan* pLandMan(NULL);
	if ( m_pLandManClient )
	{
		pLandMan = m_pLandManClient->GetLandMan();
	}

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();
	const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
	if ( !IsVisibleDetect() )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

        return S_FALSE;
	}

	// Occlusion Culling 
	if ( !NSOcclusionCulling::VisibleOcclusionCharZone( m_vMax, m_vMin ) )
		return S_FALSE;
	
	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		if ( bHALF_VISIBLE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &vDirect );
		}
		else
		{
			DxEffcharDataMan::GetInstance().OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
	}


	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
	{
		D3DXMATRIX matYRot, matTrans;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

        return S_OK;
	}

	//if ( pLandMan )
	//{
	//	CalculateAmbientColor( pd3dDevice, pLandMan, GetPosition() );
	//}

	// 바이크 타면 무조건 다 보여주도록 한다.
	// 바이크 타면 언제가 되었든 계산은 되야 한다.
	if ( m_emVehicle == EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		// 운전자는 RANPARAM::emAroundDetail 상관없이 보여준다.
		UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
	}
	else if ( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		// 탑승자는 RANPARAM::emAroundDetail 상관없이 보여준다.
		UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
	}
	else
	{
		// 평상시 모습이거나 혹은 보드를 타고 있을 경우는 RANPARAM::emAroundDetail 에 따른다.
		if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
		{
			// 렌더하지 않는다.
		}
		else if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
		{
			DxEffBillboardChar::GetInstance().AddBillboardChar( (EMBOARD_TYPE)m_CHARINDEX, vPosition );
		}
		else
		{
			UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
		}
	}



	//if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
	//{
	//	if( m_emVehicle )
	//	{
	//		if ( m_sVehicle.m_pSkinChar )
	//		{
	//			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//			{
	//				m_sVehicle.m_pSkinChar->RenderFixed( pd3dDevice, 1.f, m_matBikeTrans );
	//			}
	//			else
	//			{
	//				m_sVehicle.m_pSkinChar->RenderMaterialOpaque( pd3dDevice, 1.f, m_matBikeTrans, FALSE, TRUE, FALSE );
	//				m_sVehicle.m_pSkinChar->RenderMaterialHardAlpha( pd3dDevice, 1.f );
	//			}

	//			m_sVehicle.m_pSkinChar->UpdatePassengerPos();

	//			if ( pLandMan )
	//			{
	//				m_sVehicle.m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matBikeTrans._41,m_matBikeTrans._42,m_matBikeTrans._43) );
	//			}

	//		}
	//		UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
	//	}
	//	else
	//	{
	//		return S_OK;
	//	}
	//}

	//if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD && m_emVehicle )
	//{
	//	if ( m_sVehicle.m_pSkinChar )
	//	{
	//		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//		{
	//			m_sVehicle.m_pSkinChar->RenderFixed( pd3dDevice, 1.f, m_matBikeTrans );
	//		}
	//		else
	//		{
	//			m_sVehicle.m_pSkinChar->RenderMaterialOpaque( pd3dDevice, 1.f, m_matBikeTrans, FALSE, TRUE, FALSE );
	//			m_sVehicle.m_pSkinChar->RenderMaterialHardAlpha( pd3dDevice, 1.f );
	//		}

	//		m_sVehicle.m_pSkinChar->UpdatePassengerPos();

	//		if ( pLandMan )
	//		{
	//			m_sVehicle.m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matBikeTrans._41,m_matBikeTrans._42,m_matBikeTrans._43) );
	//		}
	//	}
	//	UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
	//}
	//else if( RANPARAM::emAroundDetail == EM_AROUND_BOARD && !m_emVehicle )
	//{
	//	DxEffBillboardChar::GetInstance().AddBillboardChar( (EMBOARD_TYPE)m_CHARINDEX, vPosition );		
	//}
	//else
	//{
	//	if ( m_sVehicle.m_pSkinChar && m_emVehicle == EMVEHICLE_DRIVER )
	//	{
	//		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//		{
	//			m_sVehicle.m_pSkinChar->RenderFixed( pd3dDevice, 1.f, m_matBikeTrans );
	//		}
	//		else
	//		{
	//			m_sVehicle.m_pSkinChar->RenderMaterialOpaque( pd3dDevice, 1.f, m_matBikeTrans, FALSE, TRUE, FALSE );
	//			m_sVehicle.m_pSkinChar->RenderMaterialHardAlpha( pd3dDevice, 1.f );
	//		}

	//		m_sVehicle.m_pSkinChar->UpdatePassengerPos();

	//		if ( pLandMan )
	//		{
	//			m_sVehicle.m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matBikeTrans._41,m_matBikeTrans._42,m_matBikeTrans._43) );
	//		}
	//	}
	//	UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );
	//}

	return S_OK;
}

HRESULT GLCharClient::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);

	if ( m_emVehicle == EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		// 운전자는 RANPARAM::emAroundDetail 상관없이 보여준다.
		UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
	}
	else if ( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		// 탑승자는 RANPARAM::emAroundDetail 상관없이 보여준다.
		UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
	}
	else
	{
		// 평상시 모습이거나 혹은 보드를 타고 있을 경우는 RANPARAM::emAroundDetail 에 따른다.
		if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
		{
			// 렌더하지 않는다.
		}
		else if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
		{
			// 렌더하지 않는다.
		}
		else
		{
			UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
		}
	}

	//if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
	//{
	//	if( m_emVehicle )
	//	{
	//		if ( m_sVehicle.m_pSkinChar )
	//		{
	//			if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//			{
	//			}
	//			else
	//			{
	//				m_sVehicle.m_pSkinChar->RenderMaterialSoftAlpha( pd3dDevice, 1.f );
	//			}
	//		}
	//		UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
	//	}
	//	else
	//	{
	//		return S_OK;
	//	}
	//}

	//if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD && m_emVehicle )
	//{
	//	if ( m_sVehicle.m_pSkinChar )
	//	{
	//		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//		{
	//		}
	//		else
	//		{
	//			m_sVehicle.m_pSkinChar->RenderMaterialSoftAlpha( pd3dDevice, 1.f );
	//		}
	//	}
	//	UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
	//}
	//else if( RANPARAM::emAroundDetail == EM_AROUND_BOARD && !m_emVehicle )
	//{
	//}
	//else
	//{
	//	if ( m_sVehicle.m_pSkinChar && m_emVehicle == EMVEHICLE_DRIVER )
	//	{
	//		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//		{
	//		}
	//		else
	//		{
	//			m_sVehicle.m_pSkinChar->RenderMaterialSoftAlpha( pd3dDevice, 1.f );
	//		}
	//	}
	//	UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );
	//}

	return S_OK;
}

HRESULT GLCharClient::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE);


	if ( (m_emVehicle == EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar) ||
		m_emVehicle == EMVEHICLE_PASSENGER )
	{
		// 탑승자는 RANPARAM::emAroundDetail 상관없이 보여준다.
		UpdateCharPos_RenderChar_MaterialEffect( pd3dDevice, bHALF_VISIBLE );
	}
	else
	{
		if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME )
		{
			// 렌더하지 않는다.
		}
		else if ( RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
		{
		}
		else
		{
			UpdateCharPos_RenderChar_MaterialEffect( pd3dDevice, bHALF_VISIBLE );
		}
	}

	return S_OK;
}

HRESULT GLCharClient::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv, BOOL bWorldSystemOLD )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )
        return S_FALSE;

	if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME || RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
        return S_OK;
	
	if ( IsDie() )
        return S_FALSE;

    if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
        return S_OK;

	// 그림자 랜더링.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bWorldSystemOLD );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
		hr = DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
	
	if ( FAILED(hr) )
        return hr;

	return S_OK;
}

HRESULT GLCharClient::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &cv, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	HRESULT hr;
	if ( !IsVisibleDetect() )
		return S_FALSE;

	if ( RANPARAM::emAroundDetail == EM_AROUND_ONLYNAME || RANPARAM::emAroundDetail == EM_AROUND_BOARD || RENDERPARAM::g_bForceLowHardwareMode )
		return S_OK;

	if ( IsDie() )
		return S_FALSE;

	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )
		return S_OK;

	// 그림자 랜더링.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
		hr = DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight ( pRenderChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );

	if ( FAILED(hr) )
		return hr;

	return S_OK;
}

HRESULT GLCharClient::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	m_pTransformSet->RestoreDeviceObjects( pd3dDevice );

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->RestoreDeviceObjects ( m_pd3dDevice );
	}

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->RestoreDeviceObjects( m_pd3dDevice );

	return S_OK;
}

HRESULT GLCharClient::InvalidateDeviceObjects ()
{
	m_pSkinChar->InvalidateDeviceObjects ();
	m_pTransformSet->InvalidateDeviceObjects();

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->InvalidateDeviceObjects();
	}

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->InvalidateDeviceObjects();

	return S_OK;
}

EMELEMENT GLCharClient::GET_ITEM_ELMT ()
{
	EMSLOT emRHand = GetCurRHand();
	EMSLOT emLHand = GetCurLHand();

	SITEM* pRHandItem = GET_SLOT_ITEMDATA(emRHand);
	SITEM* pLHandItem = GET_SLOT_ITEMDATA(emLHand);

	EMELEMENT emELEMENT(EMELEMENT_SPIRIT);
	if ( pRHandItem && pRHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pRHandItem->sSuitOp.sBLOW.emTYPE );
	}
	else if ( pLHandItem && pLHandItem->sSuitOp.sBLOW.emTYPE!=EMBLOW_NONE )
	{
		emELEMENT = STATE_TO_ELEMENT ( pLHandItem->sSuitOp.sBLOW.emTYPE );
	}

	return emELEMENT;
}

void GLCharClient::ReceiveDamage(const int nDamage, DWORD dwDamageFlag, const WORD wSchool, const STARGETID& sACTOR)
{
	if (dwDamageFlag & DAMAGE_TYPE_SHOCK)
        TurnAction ( GLAT_SHOCK );

	D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nDamage, dwDamageFlag, UI_ATTACK, sACTOR, m_sTargetID );

	if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
	{
		const D3DXVECTOR3& vPosition = GetPosition();
		// 강한타격 이펙트
		D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

		D3DXVECTOR3 vDIR_ORG(1,0,0);
		float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

		D3DXMATRIX matTrans;
		D3DXMatrixRotationY ( &matTrans, fdir_y );
		matTrans._41 = vPosition.x;
		matTrans._42 = vPosition.y + 10.0f;
		matTrans._43 = vPosition.z;

		// 자기 위치 이펙트 발생시킴.
		DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, FALSE );
	}

	m_CharData.m_Base.sHP.DECREASE(nDamage);	

	m_pGaeaClient->logDamageRecord(sACTOR, nDamage);

	RECEIVE_DAMAGE_AFTER();	
	GLCHARLOGIC::RELEASE_ENDURE_PROC();

	// 방어스킬의 이펙트가 있을때 발동시킴.
	SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );
}

void GLCharClient::ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR)
{
	D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nHeal, dwHealFlag, UI_UNDERATTACK, sACTOR, m_sTargetID );

	m_CharData.m_Base.sHP.INCREASE(nHeal);
}

void GLCharClient::ReceiveBlocking(const STARGETID& sACTOR)
{
	BOOL bCollision = FALSE;

	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK, sACTOR, m_sTargetID );
		bCollision = TRUE;
	}
/*
	// DROP CHAR의 사이즈문제로 취소, 따로 스킬 사라졌다는 메세지를 서버에서 보낸다.

	// Note : 데미지 무효 활성화 여부
	for ( int i=0; i<SKILLFACT_SIZE; ++i )
	{
		SSKILLFACT &sSKEFF = m_sSKILLFACT[i];

		if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )
			continue;

		for ( DWORD j=0; j<sSKEFF.vecSPEC.size(); ++j )
		{
			if ( sSKEFF.vecSPEC[j].emSPEC != EMSPECA_IGNORED_DAMAGE ) continue;

			// Note : 값 감소
			if ( sSKEFF.dwCount )
			{
				--sSKEFF.dwCount;
			}

			if ( sSKEFF.dwCount == 0 )
			{
				// Note : 삭제후 탈출

				if ( bCollision)
				{
					FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,m_vPos), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );
				}

				RESETSKEFF(i);
				i = SKILLFACT_SIZE;
				break;
			}
		}
	}
*/
}

void GLCharClient::ReceiveAVoid ( const STARGETID& sACTOR )
{
	if ( IsCollisionVolume() )
	{
		D3DXVECTOR3 vPos = GetPosBodyHeight();
		GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, UI_ATTACK, sACTOR, m_sTargetID );
	}
}

void GLCharClient::ReceiveSwing ()
{
	m_pSkinChar->DOSHOCKMIX();
	m_pSkinCharMulti->DOSHOCKMIX();
}


// Note : bjju. 상황에 따라 Effect를 처리하는 함수
void GLCharClient::FactEffectProcessing ( EMFACTEFFPROCESSING emFactEffCase )
{
	switch ( emFactEffCase )
    {
    case EMFACTEFFECTPROCESSING_DESTRUCTOR:
    case EMFACTEFFECTPROCESSING_MOVEMAP:
        {
            FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS );

            for ( int i=0; i<EMBLOW_MULTI;   ++i )
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

            for ( int i=0; i<SKILLFACT_SIZE; ++i )
                m_sSKILLFACT [i].RESET();

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );
        }
        break;

    case EMFACTEFFECTPROCESSING_DEAD:
    case EMFACTEFFECTPROCESSING_REBIRTH:
        {
			RESET_SKILL_DATA();

            FACTEFF::DeleteEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS );

            for ( int i=0; i<EMBLOW_MULTI; ++i )												
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

            for( int i=SKILLFACT_INDEX_NORMAL_START; i < SKILLFACT_INDEX_NORMAL_END; ++i )
				m_sSKILLFACT [i].RESET();

            for ( int i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i )	m_sSKILLFACT [i].bEnable = FALSE;
            for ( int i=SKILLFACT_INDEX_SYSTEM_START;   i<SKILLFACT_INDEX_SYSTEM_END;   ++i ) 	m_sSKILLFACT [i].bEnable = FALSE;

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );
        }
        break;
    };
}

void GLCharClient::SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec )
{
	TurnAction( GLAT_MOTION );

	m_eMotionMID = eMID;
	m_eMotionSID = eSID;
	m_fMotionSec = fSec;
}

void GLCharClient::Resurrect(const int nHP, const int nMP, const int nSP, D3DXVECTOR3 vPos)
{
	INIT_RECOVER ();

	m_sHP.nNow = nHP;
	m_sMP.nNow = nMP;
	m_sSP.nNow = nSP;
	m_sCP.SetData( 0, MAX_CP );

	TurnAction ( GLAT_IDLE );

	D3DXMATRIX matEffect;
	const D3DXVECTOR3& vPosition = GetPosition();
	D3DXMatrixTranslation ( &matEffect, vPosition.x, vPosition.y, vPosition.z );

	STARGETID sTargetID(CROW_PC,m_dwGaeaID, vPosition);
	DxEffGroupPlayer::GetInstance().NewEffGroup
		(
		GLCONST_CHAR::strREBIRTH_EFFECT.c_str(),
		matEffect,
		&sTargetID,
		FALSE, 
		TRUE
		);

	ReSetActState(EM_ACT_DIE);
}

BOOL GLCharClient::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

void GLCharClient::StartDealyAction( const SCHARSKILL& sSkill, std::tr1::shared_ptr<GLCharClient> pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	GASSERT( pOwnerChar );

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
	if ( !pSkill )
		return;

	EMSLOT				emRHand = pOwnerChar->GetCurRHand();
	STARGETID			sTARG( pOwnerChar->GETCROW(), pOwnerChar->GetGaeaID() );
	EMELEMENT			emELMT = ( pSkill->m_sAPPLY.emELEMENT == EMELEMENT_ARM ) ? pOwnerChar->GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);	// 스킬 시작과 동시에 나타나는 이팩트. 발동.
	if( pSkill->m_sEXT_DATA.emSELFBODY == SKILL::EMTIME_FIRST )
		DxEffGroupPlayer::GetInstance().NewEffBody( pSkill->m_sEXT_DATA.GETSELFBODY(emELMT), &sTARG, &pOwnerChar->GetDirect() );

	SANIMSTRIKE			sStrike;
	sStrike.m_emPiece	= pOwnerChar->VALID_SLOT_ITEM( emRHand ) ? PIECE_RHAND : PIECE_GLOVE;
	sStrike.m_emEffect	= EMSF_TARGET;
	sStrike.m_dwFrame	= 0;

	if( pSkill->m_sEXT_DATA.emSELFZONE01 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE01_POS, pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), &sTARG );

	if( pSkill->m_sEXT_DATA.emSELFZONE02 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE02_POS, pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), &sTARG );

	if( pSkill->m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_FIRST )
		pOwnerChar->SK_EFF_SELFZONE( sStrike, pSkill->m_sEXT_DATA.emSELFZONE03_POS, pSkill->m_sEXT_DATA.GETSELFZONE03(emELMT), &sTARG );

	WORD				wNumTarIds	= pTargetData->nTarget;
	STARID*				pTarIds		= (STARID*)pTargetData->TargetIDs;
	if( pSkill->m_sBASIC.emIMPACT_TAR == TAR_SPEC )
	{
		for( WORD i = 0; i < wNumTarIds; ++i )
		{
			STARGETID		sTARGET;
			sTARGET.emCrow = pTarIds[i].GETCROW();
			sTARGET.GaeaId = pTarIds[i].GETID();

			if( !m_pGaeaClient->IsVisibleCV(sTARGET) )
				continue;

			sTARGET.vPos = m_pGaeaClient->GetTargetPos( sTARGET );
			if( pSkill->m_sEXT_DATA.emTARGZONE01==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE01_POS, pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT) );

			if( pSkill->m_sEXT_DATA.emTARGZONE02==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE02_POS, pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT) );

			if( pSkill->m_sEXT_DATA.emTARGZONE03==SKILL::EMTIME_FIRST )
				pOwnerChar->SK_EFF_TARZONE( sTARGET, pSkill->m_sEXT_DATA.emTARGZONE03_POS, pSkill->m_sEXT_DATA.GETTARGZONE03(emELMT) );
		} //for
	} //if

}

void GLCharClient::NewBlowEffect(SSTATEBLOW* const pStateBlows)
{
	for (int i=0; i<EMBLOW_MULTI; ++i)
	{
		SSTATEBLOW& sNewBlow = pStateBlows[i];
		if ( sNewBlow.emBLOW == EMBLOW_NONE )
			continue;

		const D3DXVECTOR3& vPosition = GetPosition();
		if( sNewBlow.emBLOW <= EMBLOW_SINGLE )
		{		
			FACTEFF::DeleteBlowSingleEffect( STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, m_sSTATEBLOWS );

			for( int i=0; i<EMBLOW_MULTI; ++i )
				m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
		}

		SSTATEBLOW *pSTATEBLOW = NULL;
		if ( sNewBlow.emBLOW <= EMBLOW_SINGLE )
			pSTATEBLOW = &m_sSTATEBLOWS[0];
		else
			pSTATEBLOW = &m_sSTATEBLOWS[sNewBlow.emBLOW-EMBLOW_SINGLE];

		pSTATEBLOW->emBLOW		= sNewBlow.emBLOW;
		pSTATEBLOW->fAGE		= sNewBlow.fAGE;
		pSTATEBLOW->fSTATE_VAR1 = sNewBlow.fSTATE_VAR1;
		pSTATEBLOW->fSTATE_VAR2 = sNewBlow.fSTATE_VAR2;
		pSTATEBLOW->wSchool		= sNewBlow.wSchool;

		FACTEFF::NewBlowEffect (
			STARGETID(CROW_PC, m_dwGaeaID, vPosition),
			m_pSkinChar, sNewBlow.emBLOW, GetTransMatrix(), GetDirect() );	// 효과 생성
	}
}

const BOOL GLCharClient::IsFeignDie() const
{
	if ( IsCheckedSkillFlagSpecial(EMSPECA_FEIGN_DEATH) == false )
		return TRUE;

	return FALSE;
}

void GLCharClient::End_FeignDeath( BOOL bForce )
{
	ReSetActState(EM_ACT_FEIGN_DIE);

	EMACTIONTYPE eAction = GetAction();

	if( eAction != GLAT_SKILL && eAction != GLAT_SKILL_WAIT )
		TurnAction(GLAT_IDLE);
}

void GLCharClient::Play_Disguise( int iIdx, BOOL bOn )
{
	if( !bOn || iIdx == 0 ) return;

	EMCHARCLASS emClass = (EMCHARCLASS)m_CharData.m_Base.emClass;
	EMCHARINDEX emIndex = CharClassToIndex(emClass);
	
	DxSkinCharData* pCharData = m_pTransformSet->GetDistinguishCharData( (emClass & GLCC_MAN) ? 1 : 0, iIdx, true );

	if( pCharData )
	{
		BOOL bAmbientUp = NSLightMapAmbientColor::g_bActiveLightMapAmbientColor ? FALSE : TRUE;

		m_pSkinChar->SetCharData( pCharData, m_pd3dDevice, bAmbientUp, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		m_pSkinChar->DEFAULTANI();
		m_pSkinChar->StopLoopSound();

		// 무기를 위한 UpdateSuit;
		if ( E_FAIL == UpdateSuit_Weapon() )
		{
			if ( m_pTransformSet )
				m_pTransformSet->CancelDistinguish();

			Stop_Disguise( );

			return;
		}

		const D3DXVECTOR3& vPosition = GetPosition();

		for ( int i = 0; i < SKILLFACT_SIZE; ++i )
		{
			SSKILLFACT &sSKEFF = m_sSKILLFACT[i];
			if ( sSKEFF.sNATIVEID==NATIVEID_NULL() )	continue;

			FACTEFF::NewSkillFactEffect ( 
				STARGETID(CROW_PC, m_dwGaeaID, vPosition), 
				m_pSkinChar, sSKEFF.sNATIVEID, GetTransMatrix(), GetDirect() );

		}

		TurnAction(GLAT_IDLE);
	}
}

void GLCharClient::Stop_Disguise()
{
	EMCHARINDEX emIndex = CharClassToIndex((EMCHARCLASS)m_CharData.m_Base.emClass);

	DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().FindData(GLCONST_CHAR::szCharSkin[emIndex]);

	if( pCharData )
	{
		BOOL bAmbientUp = NSLightMapAmbientColor::g_bActiveLightMapAmbientColor ? FALSE : TRUE;

		m_pSkinChar->SetCharData( pCharData, m_pd3dDevice, bAmbientUp, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

		UpdateSuit();	
	}
}

void GLCharClient::UpdateSkillEff()
{
	/// 변장 /////////////////////
	BOOL bOn   = RF_DISGUISE( m_EffSkillVarSet ).IsOn();
	UINT Index = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

	SITEMCLIENT sLHandItem = m_CharData.m_PutOnItems[ SLOT_LHAND ];
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.sNativeID.dwID );
	if ( pItem )
	{
		// Table ID 가 같을 경우에만 Index를 변경한다;
		// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
		if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
			Index = pItem->sPet.sPetID.Sid();
	}

	if( m_pTransformSet->AutoResetDisting( Index, bOn ) ) {
		Stop_Disguise( );
	}
	else {
		Play_Disguise( Index, bOn );
	}
}

void GLCharClient::SkinAniControl_AnimationMove()
{
	// 애니메이션에서 캐릭터를 이동시켜줬을 수도 있으니 확인하자.
	if( m_pSkinChar->IsAnimationMove() )
	{
		D3DXVECTOR3 vPos;
		D3DXMATRIX matRotate;
		D3DXMatrixRotationY( &matRotate, m_fAnimationMoveRotationY );
		D3DXVec3TransformCoord( &vPos, &m_pSkinChar->GetAnimationMovePos(), &matRotate );

		vPos += GetPositionActor();
		SetPositionActor( vPos );
	}
}