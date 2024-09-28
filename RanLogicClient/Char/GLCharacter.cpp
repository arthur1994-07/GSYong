#include "../pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/NSGUI.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxFogMan.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Msg/ItemRepairMsg.h"
#include "../../RanLogic/Item/ItemDurability.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/stl_Func.h"
#include "../../RanLogic/Quest/GLQUEST.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Skill/GLSkillChangeList.h"
#include "../../RanLogic/Skill/GLSkillReset.h"
#include "../../RanLogic/RandomAddon/RandomAddon.h"

#include "../Club/GLClubClient.h"
#include "../GLGaeaClient.h"
#include "../MiniGame/GLMiniGameManager.h"
#include "../Party/GLPartyClient.h"
#include "../Friend/GLFriendClient.h"
#include "../Post/GLPostClient.h"
#include "../SNS/GLSNSClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../Summon/GLSummonClient.h"
#include "../Char/GLCharClient.h"
#include "../Land/GLLandManClient.h"
#include "../Input/GLInputManager.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../Reservation/GLReservationAction.h"
#include "../TexasHoldem/GLTexasHoldemClient.h"
#include "../StaticHoldItem.h"
#include "./NSGradeScriptMan.h"
#include "./GLCharacter.h"
#include "../RnAttendance/RnAttendanceClient.h"
#include "../Tool/DxParamSet.h"

#include "../TriggerSystem/ClientActionMessage.h"

#include "../Product/GLProductClient.h"

#include "SkinCharMulti.h"

#include "../Chat/GLChatMan.h"
#include "../Minimap/GLMinimapClient.h"
#include "../GfxInterfaceBase.h"
#include "../Macro/MacroManager.h"

#include "../../RanLogic/Prison/LimitPrison.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	class CFINDER
	{
	public:
		enum { BUFFER = 36, };
	protected:
		std::vector<STARGETID*>		m_vecTAR;

	public:
		CFINDER()
		{
			m_vecTAR.reserve(BUFFER);
		}

	public:
		void CLEAR()
		{
			if (!m_vecTAR.empty())
			{
				//m_vecTAR.erase(m_vecTAR.begin(), m_vecTAR.end());
                m_vecTAR.clear();
			}
		}

	public:
		STARGETID FindClosedCrow(const VEC_SK_ITER_RANGE& mapRange, const D3DXVECTOR3& vFromPt)
		{
			GASSERT(mapRange.first!=mapRange.second);

			CLEAR();
			std_afunc::CCompareTargetDist sCOMP(vFromPt);

			for (VEC_SK_ITER pos=mapRange.first; pos!=mapRange.second; ++pos)
			{
				m_vecTAR.push_back(&(*pos));
			}
			std::vector<STARGETID*>::iterator found = std::min_element(m_vecTAR.begin(), m_vecTAR.end(), sCOMP);
			return *(*found);
		}

		static CFINDER& GetInstance()
		{
			static CFINDER Instance;
			return Instance;
		}
	};

	STARGETID* FindCrow(const VEC_SK_ITER_RANGE& mapRange, const STARGETID& _starid)
	{
		static STARGETID sTARID;
		GASSERT(mapRange.first!=mapRange.second);
		VEC_SK_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; ++iter)
		{
			sTARID = (*iter);
			if (sTARID.emCrow==_starid.emCrow && sTARID.GaeaId==_starid.GaeaId)
                return &sTARID;
		}
		return NULL;
	}

	STARGETID* FindCrow(const VEC_SK_ITER_RANGE& mapRange, SCONFTING_CLT::CONFT_MEM& setconftMember, GLGaeaClient* pGaeaClient)
	{
		static STARGETID sTARID;
		GASSERT(mapRange.first!=mapRange.second);
		VEC_SK_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; ++iter)
		{
			sTARID = (*iter);
			if (sTARID.emCrow==CROW_PC)
			{
				SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find(sTARID.GaeaId);
				if (iter!=setconftMember.end())
                    return &sTARID;
			}
			else if (sTARID.emCrow==CROW_SUMMON)
			{
                std::tr1::shared_ptr<GLSummonClient> spSummon = pGaeaClient->GetSummon(sTARID.GaeaId);
				if (spSummon)
				{
					ClientActor* pCOPY = pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
					if (pCOPY)
					{
						SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find(pCOPY->GetGaeaID());
						if (iter!=setconftMember.end())
                            return &sTARID;
					}
				}
			}
		}
		return NULL;
	}

	STARGETID* FindCrowCID(const VEC_SK_ITER_RANGE &mapRange, SCONFTING_CLT::CONFT_MEM& setconftMember, GLGaeaClient* pGaeaClient)
	{
		static STARGETID sTARID;

		GASSERT(mapRange.first!=mapRange.second);

		VEC_SK_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; iter++ )
		{
			sTARID = (*iter);

			if ( sTARID.emCrow==CROW_PC )
			{
				std::tr1::shared_ptr<GLCharClient> pCHAR = pGaeaClient->GetChar(sTARID.GaeaId);
				if (pCHAR)
				{
					SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find ( pCHAR->GetCharData().m_Base.dwCharID );
					if (iter!=setconftMember.end())
                        return &sTARID;
				}
			}
			else if ( sTARID.emCrow==CROW_SUMMON )
			{
                std::tr1::shared_ptr<GLSummonClient> spSummon = pGaeaClient->GetSummon(sTARID.GaeaId);
				if (spSummon)
				{
					ClientActor* pCOPY = pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
					if (pCOPY)
					{
						SCONFTING_CLT::CONFT_MEM_ITER iter = setconftMember.find(pCOPY->GetCharID());
						if (iter!=setconftMember.end())
                            return &sTARID;
					}
				}
			}
		}

		return NULL;
	}

	STARGETID* FindSummonOwner(const VEC_SK_ITER_RANGE& mapRange, const STARGETID& _sownertarid, GLGaeaClient* pGaeaClient)
	{
		static STARGETID sTARID;

		GASSERT(mapRange.first!=mapRange.second);

		VEC_SK_ITER iter = mapRange.first;
		for ( ; iter!=mapRange.second; ++iter )
		{
			sTARID = (*iter);

			if ( sTARID.emCrow!=CROW_SUMMON	)
				continue;

			std::tr1::shared_ptr<GLSummonClient> spSummon = pGaeaClient->GetSummon(sTARID.GaeaId);
			if (spSummon)
			{
				ClientActor* pCOPY = pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
				if (pCOPY)
				{
					if (pCOPY->GetCrow()==_sownertarid.emCrow && pCOPY->GetGaeaID()==_sownertarid.GaeaId)
                        return &sTARID;
				}
			}
		}
		return NULL;
	}
};

const float GLCharacter::m_fELAPS_MOVE = 0.2f;
const float GLCharacter::fLIMIT_TIME_ITEM_SORT = 1.0f;
const float GLCharacter::fMAX_ITEM_SORT_MSG_DELAY_TIME = 30.0f;

GLCharacter::GLCharacter(GLGaeaClient* pGaeaClient, GLClubClient* pMyClub )
	: ClientActor(pGaeaClient, CROW_PC)
	, m_pd3dDevice(NULL)
	, m_pMyClub(pMyClub)
	, m_pTransformSet(NULL)
	, m_pSkinChar(NULL)
	, m_pSkinCharMulti(NULL)
	, m_vServerPos(0,0,0)

	, m_fKeyDownCheckTime(0.0f)
	, m_bAwayGesture(FALSE)    

	, m_fLastMsgMoveSend(0.0f)
	, m_ReservationAction(pGaeaClient)

	, m_bMiniMapMove( false )
	, m_fMiniMapMoveTime( 0.0f )

	, m_fIdleTime(0.0f)
	, m_fattTIMER(0.0f)
	, m_nattSTEP(0)

	, m_dwWAIT(0)

	, m_sRunSkill(NATIVEID_NULL())
	, m_sActiveSkill(NATIVEID_NULL())
	, m_sScopeSkill(SKILLID_NULL)
	, m_dwANISUBCOUNT(0)
	, m_dwANISUBSELECT(0)
	, m_dwANISUBGESTURE(0)

	, m_emOldQuestionType(QUESTION_NONE)

	, m_fQUEST_TIMER(60.0f)

	, m_wInvenPosX1(0)
	, m_wInvenPosY1(0)
	, m_wInvenPosX2(0)
	, m_wInvenPosY2(0)
	, m_wwInvenPos_FaceStyle(0)
	, m_wwInvenPos_HairStyle(0)
	, m_wwInvenPos_HairColor(0)

	, m_bCLUB_CERTIFY(FALSE)
	, m_fCLUB_CERTIFY_TIMER(0)

	, m_wPMPosX(USHRT_MAX)
	, m_wPMPosY(USHRT_MAX)

	, m_vMaxOrg(6,20,6)
	, m_vMinOrg(-6,0,-6)

	, m_vMax(6,20,6)
	, m_vMin(-6,0,-6)
	, m_fHeight(20.f)
	, m_NpcLastClick(0)

	//, m_bEnableHairSytle(FALSE)
	//, m_bEnableHairColor(FALSE)
	//, m_bEnableFaceStyle(FALSE)

	, m_bAttackable(TRUE)
	, m_sPetCardNativeID(NATIVEID_NULL())
	, m_cPetCardGenType(-1)
	, m_bReceivedPetCardInfo(FALSE)
	, m_bIsPetActive(FALSE)
	, m_bMiniGameOpen(FALSE)
	, m_bReqSwapArms(FALSE)
	, m_bIsVehicleActive ( FALSE )
	, m_bRecivedVehicleItemInfo ( FALSE )
	, m_bGarbageOpen ( false )
	, m_bItemShopOpen ( false )
	, m_bReqAttendList( false )
	, m_dwComboAttend(0)
	, m_tAttendLogin(0)
	, m_dwAttendTime(0)
	, m_bRecordChat(FALSE)
	//, m_bUseRemodelCard(false)
	, m_emTypeItemRemodelState( EMTYPE_ITEM_REMODEL_NONE )
	, m_bUseStorageCard(false)
	, m_strRecordChat("")
	, m_fPartyRecruitTime(GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME)
	, m_fLimitTimeItemSort(0.0f)
	, m_fItemSortMsgDelayTime(0.0f)
	, m_bSendItemSortMsg(false)
	, m_fCheckActivityItem(0.0f)
	, m_ServiceProvider(SP_OFFICE_TEST)
	, m_bForcedPKMode(false)

	, m_eMotionMID( AN_GUARD_N )
	, m_eMotionSID( AN_SUB_NONE )
	, m_fMotionSec( 0.0f )
	, m_bMovement(CHAR_STATE_DEFAULT)

	, m_nTitleSelectIndex( 0 )

	, m_dwVehiclePassengerID(0)
	, m_PtoPTargetID(0)

	, m_spWorldLight(new ActorWorldLight)
	, m_bCostumeStats( false )
	, m_emDoActWaitTypePrev(DoActWait_Zero)
	, m_emDoActWaitType(DoActWait_Zero)

	, m_bFirstAnimationMove(FALSE)
	, m_fMacro_RecordUINotifyTime(0.0f)

	, m_nOpenCharSlot ( 0 )
{
    if (m_pGaeaClient)
        m_ServiceProvider = m_pGaeaClient->GetServiceProvider();

	m_pWorkingSkill = new GLWorkingSkill(m_pGaeaClient);
	
	D3DXMatrixIdentity ( &m_matBikeTrans );
	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )		m_dwNumStorageItem[i] = UINT_MAX;

	for( int i = 0; i < 8; i++ )
	{
		m_bOldVisibleTracingUI[i] = FALSE;
		m_bNewVisibleTracingUI[i] = FALSE;
	}

	m_fPrintProcessInfoMsgGap = -10.0f;

	ms_vecTAR_TEMP.reserve(220);

	ClientActor::SetActState(EM_ACT_RUN);

	m_mapPETCardInfo.clear();
	m_mapPETCardInfoTemp.clear();

	m_mapPETReviveInfo.clear();

	m_mapVEHICLEItemInfo.clear();
	m_mapVEHICLEItemInfoTemp.clear();

	m_vecAttend.clear();

	m_mapMarketClick.clear();

    m_vecRebuy.clear();

    SetPriorityItemSort();

    m_TitleManager.Clear();
    memset( m_llActivityPoint, 0, sizeof( LONGLONG ) * ACTIVITY_CLASS_SIZE );

	QuickChangeReset();

    InitPacketFunc();

	m_vQuickNum.reserve( EMSKILLQUICK_SIZE );
}

GLCharacter::~GLCharacter ()
{
	m_vQuickNum.clear();

	ClientActor::Reset();

	SAFE_DELETE(m_pWorkingSkill);

	DeleteSkin();
}

void GLCharacter::SetPriorityItemSort()
{
    // size of FIRST_PRIORITY_ITEMSORT = ITEM_NSIZE;
    for (int iItemType = 0; iItemType<ITEM_NSIZE; ++iItemType)
    {
        switch (iItemType)
        {
		case ITEM_COSTUME_EFFECT_RELEASE :
		case ITEM_EFFECT_RELEASE_CURE :
        case ITEM_CURE:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 0;
            break;

        case ITEM_SUIT:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 1;
            break;

        case ITEM_ARROW:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 2;
            break;

        case ITEM_BULLET:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 3;
            break;

        case ITEM_CHARM:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 4;
            break;

        case ITEM_GRINDING:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 5;
            break;

        default:
            FIRST_PRIORITY_ITEMSORT[iItemType] = 6;
            break;
        };
    }

    for (int iItemLevel=0; iItemLevel<EMITEM_LEVEL_NSIZE; ++iItemLevel)
    {
        switch (iItemLevel)
        {
		case EMITEM_LEVEL_LEGENDARY:
			SECOND_PRIORITY_ITEMSORT[iItemLevel] = 0;
			break;

        case EMITEM_LEVEL_EPIC:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 1;
            break;

        case EMITEM_LEVEL_UNIQUE:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 2;
            break;

        case EMITEM_LEVEL_RARE:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 3;
            break;

        case EMITEM_LEVEL_HIGH:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 4;
            break;

        case EMITEM_LEVEL_NORMAL:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 5;
            break;

        default:
            SECOND_PRIORITY_ITEMSORT[iItemLevel] = 6;
            break;
        };
    }

    for (int iSuit = 0; iSuit < SUIT_NSIZE; ++iSuit)
    {
        switch (iSuit)
        {
        case SUIT_HANDHELD:
            ADDTIONAL_PRIORITY_ITEMSORT[iSuit] = 0;
            break;

        default:
            ADDTIONAL_PRIORITY_ITEMSORT[iSuit] = 1;
            break;
        };
    }
}

float GLCharacter::GetBuyRate ()
{
	float fBUY_RATE = GET_PK_SHOP2BUY();

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand )
	{
		fBUY_RATE += pLand->m_fCommission;
	}

	return fBUY_RATE;
}

float GLCharacter::GetSaleRate ()
{
	float fSALE_RATE = GET_PK_SHOP2SALE ();
	
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand )
	{
		fSALE_RATE -= pLand->m_fCommission;
	}

	return fSALE_RATE;
}

void GLCharacter::ResetData ()
{
	int i = 0;

	GLCHARLOGIC::RESET_DATA();
	ClientActor::Reset();

	DeleteSkin();
	
	SetActState(EM_ACT_RUN);

	m_sActionID = STARGETID(CROW_MOB,EMTARGET_NULL,D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ));
	m_sSelectID = STARGETID(CROW_MOB,EMTARGET_NULL,D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ));

	m_sSkillTargetData.Reset();
	
	m_sRunSkill = SNATIVEID(false);
	m_sActiveSkill = SNATIVEID(false);
	m_sScopeSkill = SKILLID_NULL;
	m_dwANISUBCOUNT = 0;
	m_dwANISUBSELECT = 0;
	m_dwANISUBGESTURE = 0;

	m_fLastMsgMoveSend = 0.0f;
	m_sLastMsgMove = GLMSG::SNETPC_GOTO();
	m_sLastMsgMoveSend = GLMSG::SNETPC_GOTO();

	m_ReservationAction.Clear();

	m_bMiniMapMove = false;
	m_fMiniMapMoveTime = 0.0f;

	m_fIdleTime = 0.0f;
	m_fattTIMER = 0.0f;
	m_nattSTEP = 0;
	m_dwWAIT = 0;

	m_bRecordChat = FALSE;
    //m_bUseRemodelCard = false;
	m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_NONE;
    m_bUseStorageCard = false;

	m_strRecordChat = "";

	for ( DWORD _i = 0; _i < EMSTORAGE_CHANNEL; ++_i )
		m_dwNumStorageItem[_i] = UINT_MAX;

	m_sCONFTING.RESET();

	DoPrivateMarketClose();

	m_bCLUB_CERTIFY = FALSE;
	m_fCLUB_CERTIFY_TIMER = 0;

	m_pMyClub->RESET();

	m_vMaxOrg = D3DXVECTOR3(6,20,6);
	m_vMinOrg = D3DXVECTOR3(-6,0,-6);

	m_vMax = D3DXVECTOR3(6,20,6);
	m_vMin = D3DXVECTOR3(-6,0,-6);

	m_NpcLastClick = 0;

	m_bAttackable = TRUE;

	m_mapPETCardInfo.clear();
	m_mapPETCardInfoTemp.clear();
	m_bReceivedPetCardInfo = FALSE;
	m_bIsPetActive		   = FALSE;

	m_mapPETReviveInfo.clear();

	m_mapVEHICLEItemInfo.clear();
	m_mapVEHICLEItemInfoTemp.clear();

	m_bReqSwapArms			  = FALSE;

	m_bIsVehicleActive		  = FALSE;
	m_bRecivedVehicleItemInfo = FALSE;

	for( i = 0; i < 8; i++ )
	{
		m_bOldVisibleTracingUI[i] = FALSE;
		m_bNewVisibleTracingUI[i] = FALSE;
	}

	m_fPrintProcessInfoMsgGap = -10.0f;

	m_sVehicle.RESET();

    m_bGarbageOpen = false;
	m_bItemShopOpen = false;
	m_bReqAttendList = false;
	m_bCostumeStats = false;
	m_vecAttend.clear();
	m_dwComboAttend = 0;
	m_tAttendLogin = 0;
	m_dwAttendTime = 0;

	m_dwTransformSkill = SNATIVEID::ID_NULL;
	m_IdSkillTransform = NATIVEID_NULL();

	m_mapMarketClick.clear();

	m_mapEntryLimitInfo.clear();

    m_vecRebuy.clear();

	m_fPartyRecruitTime = GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME;

    m_fLimitTimeItemSort = 0.0f;
    m_fItemSortMsgDelayTime = 0.0f;
    m_bSendItemSortMsg = false;

    m_fCheckActivityItem = 0.0f;

    m_TitleManager.Clear();
    memset( m_llActivityPoint, 0, sizeof( LONGLONG ) * ACTIVITY_CLASS_SIZE );

    ActivityBaseData::GetInstance()->ResetData();

	m_eMotionMID = AN_GUARD_N;
	m_eMotionSID = AN_SUB_NONE;
	m_fMotionSec = 0.0f;

    m_bMovement = CHAR_STATE_DEFAULT;
	SetLockMovement(!m_bMovement);

	for( WORD i = 0; i < SKILL_TAB_COUNT; ++i )
		m_vecSkill[ i ].clear();

	m_nOpenCharSlot = 0;
}

HRESULT GLCharacter::SkinLoad ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData(
        GLCONST_CHAR::szCharSkin[emIndex], pd3dDevice);
	if (!pSkinChar)
        return E_FAIL;



	// [shhan][2014.12.08] chf 에 있는 cfg 에 들어있는 bin 파일들을 Thread 로 하든 아니든 파일 로딩을 하도록 한다.
	//						목적 : 자신의 캐릭터가 애니메이션이 없어서 알맞은 동작을 못하는 문제 해결.
	pSkinChar->LoadBIN_HaveToAnimationForMyChar( FALSE );

	// [shhan][2014.12.08] 퀵슬롯에 등록해 놓은 스킬은 미리 애니메이션을 읽도록 한다.
	for ( UINT i=0; i<EMSKILLQUICK_SIZE; ++i )
	{
		if ( m_sSKILLQUICK[i] != NATIVEID_NULL() )
		{
			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( m_sSKILLQUICK[i] );
			if ( pSkill )	//	스킬이 존재할 경우
			{
				pSkinChar->LoadBIN_HaveToAnimationForMyChar( pSkill->m_sEXT_DATA.emANIMTYPE, pSkill->m_sEXT_DATA.emANISTYPE, FALSE );
			}
		}
	}



	BOOL bAmbientUp(TRUE);
	if ( NSLightMapAmbientColor::g_bActiveLightMapAmbientColor )
	{
		bAmbientUp = FALSE;
	}

	DeleteSkin();

	m_pTransformSet = new GLTransformSet(pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	m_pSkinChar = new DxSkinChar;
	m_pSkinChar->SetCharData(pSkinChar, pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );		

	GLTransformSkinManager::GetInstance().LoadFile(pd3dDevice);
	GLDistinguishSkinManager::GetInstance().LoadFile(pd3dDevice );

	m_pSkinCharMulti = new SkinCharMulti( pSkinChar, pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	//m_pSkinCharMulti->SetCharData(pSkinChar, pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
 	
	UpdateSuit( TRUE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;	

	//	Note : 에니메이션 초기화.
	//
	if ( m_pSkinChar )
	{
		SELECTANI ( m_pSkinChar, AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );

		if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE ) == 0 )
		{
			DEFAULTANI( m_pSkinChar );
		}

		// 로딩중일 수 있다.
		//if ( !m_pSkinChar->GETCURANIMNODE() )
		//
		//	sc::writeLogError(
		//		sc::string::format(
		//		"current animation node null point error [%1%] [M %2% S %3%]",
		//		m_pSkinChar->m_szFileName,
		//		AN_GUARD_N,
		//		GLCHARLOGIC::m_emANISUBTYPE));
		//}
	}

	const D3DXVECTOR3& vDirect = GetDirect();
	const D3DXMATRIX& matTrans = GetTransMatrix();

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( 
		STARGETID(CROW_PC, m_dwGaeaID, GetPosition()), 
		m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, matTrans, vDirect );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	// 미리 로딩해 놓으면 좋은 Effect
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strCONFRONT_BOUND.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strLEVELUP_EFFECT.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strREBIRTH_EFFECT.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strVEHICLE_GEN_EFFECT.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strCOMPLATE_ACTIVITY_EFFECT.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strMOB_DIE_BLOOD.c_str(), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( GLCONST_CHAR::strPET_GEN_EFFECT.c_str(), TRUE, TRUE );	
	
	return S_OK;
}

void GLCharacter::DeleteSkin()
{
	SAFE_DELETE(m_pSkinChar);
	SAFE_DELETE(m_pTransformSet);
	SAFE_DELETE(m_sVehicle.m_pSkinChar);

	if ( m_pSkinCharMulti )
	{
		m_pSkinCharMulti->ResetSkinCharMulti();
		SAFE_DELETE(m_pSkinCharMulti);
	}
}

// 맵을 이동하는 것이니 맵 참조값을 줄이는 방향으로 간다.
void GLCharacter::LandShredPtrRefSub()
{
	// 깔끔하게 새로 만들도록 한다.
	m_spWorldLight = std::tr1::shared_ptr<ActorWorldLight>(new ActorWorldLight);
}

HRESULT GLCharacter::SkinLoadForTool( LPDIRECT3DDEVICEQ pd3dDevice, const char* szFile )
{
	DeleteSkin();

	BOOL bAmbientUp(TRUE);
	if ( NSLightMapAmbientColor::g_bActiveLightMapAmbientColor )
	{
		bAmbientUp = FALSE;
	}

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().LoadData( szFile, pd3dDevice );
	if ( !pSkinChar )
        return E_FAIL;

	m_pTransformSet = new GLTransformSet(pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	m_pSkinChar = new DxSkinChar;

	m_pSkinChar->SetCharData ( pSkinChar, pd3dDevice, bAmbientUp, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );

//	UpdateSuit( TRUE );

	// Note : 1.AABB Box를 가져온다. 2.높이를 계산해 놓는다.
	m_pSkinChar->GetAABBBox( m_vMaxOrg, m_vMinOrg );
	m_fHeight = m_vMaxOrg.y - m_vMinOrg.y;

	//	Note : 에니메이션 초기화.
	//
	if ( m_pSkinChar )
	{
		m_pSkinChar->SELECTANI ( AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );

		if ( m_pSkinChar->GETANIAMOUNT( AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE ) == 0 )
		{
			m_pSkinChar->DEFAULTANI();
		}

		// 로딩중일수 있다.
		//if ( !m_pSkinChar->GETCURANIMNODE() )
		//{
		//	//sc::writeLogError(
		//	//	sc::string::format(
		//	//	"current animation node null point error [%1%] [M %2% S %3%]",
		//	//	m_pSkinChar->m_szFileName,
		//	//	AN_GUARD_N,
		//	//	GLCHARLOGIC::m_emANISUBTYPE));
		//}
	}

	const D3DXVECTOR3& vDirect = GetDirect();

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect(
		STARGETID(CROW_PC, m_dwGaeaID, GetPosition()), 
		m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, GetTransMatrix(), vDirect);

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	return S_OK;
}


HRESULT GLCharacter::Create ( boost::shared_ptr<NavigationMesh> spNavi, SCHARDATA2 *pCharData2, D3DXVECTOR3* pvPos, DWORD dwGaeaID, LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	GASSERT(pd3dDevice);
	HRESULT hr;

	m_pd3dDevice = pd3dDevice;
	SetGaeaID(dwGaeaID);

	m_dwTransformSkill = SNATIVEID::ID_NULL;
	m_IdSkillTransform = NATIVEID_NULL();

	//	Note : 케릭터의 외형을 설정.
	if( pCharData2 )
        SCHARDATA2::Assign( *pCharData2 );
	else if( GLCONST_CHAR::VAID_CHAR_DATA2( 0, GLCI_ARMS_M ) )
        SCHARDATA2::Assign( GLCONST_CHAR::GET_CHAR_DATA2( 0, GLCI_ARMS_M ) );

	// 유효한 스킬 검사;
	SET_VALIDATION_EXPSKILL_MAP();

    m_cInventory.SetAddLine( GetOnINVENLINE(), true );
	
	//!! SF_TODO
	//if ( IsCDMSafeTime() )
    //    m_pInterface->VisibleCDMSafeTime( true, m_fCDMSafeTime );

	// 베트남 인벤토리는 세로가 10칸이므로 6칸을 더 추가한다.
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		m_cVietnamInventory.SetAddLine ( 6, true );
	}
//#endif	

	// 초기화
	GLCHARLOGIC::INIT_DATA ( FALSE, TRUE );
	m_sCONFTING.RESET();

	//	초기 런 스킬 지정.
	if ( m_wSKILLQUICK_ACT < EMSKILLQUICK_SIZE )
	{
		m_sRunSkill = m_sSKILLQUICK[m_wSKILLQUICK_ACT];
	}

	hr = SkinLoad ( pd3dDevice, pLandMan );
	if ( FAILED(hr) )	return hr;


	//	Note : 이동 제어 초기화.
	const D3DXVECTOR3& vPosition = pvPos ? *pvPos : m_vStartPos;
	
	ClientActor::InitializeActorMove(vPosition, spNavi, D3DXVECTOR3(0.0f, 0.0f, -1.0f));	
	ClientActor::SetMaxSpeed(GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO);

	DoActWait ( DoActWait_Create );

	GLQuestPlay::MAPQUEST& mapQuest = m_cQuestPlay.GetQuestProc ();
	{
		bool bKNOCK(false);

		GLQuestPlay::MAPQUEST_ITER iter = mapQuest.begin();
		GLQuestPlay::MAPQUEST_ITER iter_end = mapQuest.end();
		for (; iter!=iter_end; ++iter)
		{
			GLQUESTPROG* pPROG = (*iter).second;
			if (!pPROG->IsReqREADING())
                continue;

			bKNOCK = true;

			GLQUEST* pQUEST = GLQuestMan::GetInstance().Find(pPROG->m_sNID.dwID);
			if (pQUEST)
			{				
				PrintConsoleText(
                    sc::string::format(ID2GAMEINTEXT("QUEST_KNOCK"), pQUEST->GetTITLE()));
			}
			else
			{				
				PrintConsoleText(
                    sc::string::format(ID2GAMEINTEXT("QUEST_NOINFO"), pPROG->m_sNID.dwID));
			}
		}

		if ( bKNOCK )
		{
			PrintConsoleText(ID2GAMEINTEXT("QUEST_KNOCK_END"));
		}
	}

	m_pGaeaClient->GetFriendClient()->Clear();
    m_pGaeaClient->NotifyClear();

	m_mapEntryLimitInfo.clear();

	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ NetMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		// 이동 할 때마다 베트남 탐닉 시스템 시간을 요청한다.
		GLMSG::SNETPC_VIETNAM_TIME_REQ NetVietnamMsg;
		NetMsg.dwGaeaID	= m_dwGaeaID;	
		m_pGaeaClient->NETSENDTOFIELD(&NetVietnamMsg);
	}
//#endif

	// Note : 우편 정보 요청
	m_pGaeaClient->GetPostClient()->ReqReceivePostInfo();
	/////////////////////////////////////////

    // Note : SNS 인증 정보 요청
    m_pGaeaClient->GetSNSClient()->GetAuthInfoReq();
    /////////////////////////////////////////


	STARGETID sTargetID(CROW_PC, m_CharDbNum, vPosition);
	D3DXMATRIX matEffect;
	D3DXMatrixTranslation(&matEffect, vPosition.x, vPosition.y, vPosition.z);

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return E_FAIL;

	SNATIVEID mapID = pLandMClient->GetBaseMapID();
	if( pLandMClient->IsInstantMap() )
		mapID.wSubID = 0;

	SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode(mapID);
	if ( pMapNode )
	{
		GLLevelFileClient cLevelFile(m_pGaeaClient);
		if( cLevelFile.LoadFile ( pMapNode->m_LevelFile.c_str(), NULL ) )
		{
			if( cLevelFile.GetUseLevelEtcFunc(EMETCFUNC_CONTROLCAM) )
			{
				const SLEVEL_ETC_FUNC* pEtcFunc = cLevelFile.GetLevelEtcFunc();
				if( !pEtcFunc )	return E_FAIL;

				D3DXVECTOR3 vFromPt, vLookAtPt, vUpPt;
				DxViewPort::GetInstance().GetViewTrans( vFromPt, vLookAtPt, vUpPt );
				vFromPt = pEtcFunc->m_sControlCam.vCamPos + vLookAtPt;

				DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookAtPt, vUpPt );
				DxViewPort::GetInstance().SetCameraControl( TRUE, pEtcFunc->m_sControlCam.vCamPos, pEtcFunc->m_sControlCam.fUp, 
																	pEtcFunc->m_sControlCam.fDown, pEtcFunc->m_sControlCam.fLeft, 
																	pEtcFunc->m_sControlCam.fRight, pEtcFunc->m_sControlCam.fFar, 
																	pEtcFunc->m_sControlCam.fNear );
			}
            else
            {
				DxViewPort::GetInstance().SetCameraControl( FALSE );
			}
		}

		/*
		if ( pMapNode->IsExpeditionMap() && (GetPartyID().isValidExpedition() == false) )
		{
			//m_pInterface->SetMapCheckType( UI::EMMAPCHECK_EXPEDITION );
			//m_pInterface->UiShowGroupFocus ( MAP_REQUIRE_CHECK );
		}
		*/
	}

	// 필요 Effect 및 배운 스킬들의 Effect 들을 미리로딩하도록 한다.
	PreLoadEffectFull();

//#ifndef _RELEASED
	if ( m_dwUserLvl >= USER_USER_GM )
	{
		ReqVisibleOff();
		m_pGaeaClient->turnOnDamageRecord(STARGETID(CROW_PC, m_dwGaeaID));
	}
//#endif

	if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
	{
		m_pGaeaClient->GetAttendanceCompleted();
	}

	if ( m_pGaeaClient->IsEmulator() )
	{
		m_wSex = m_emClass & GLCC_MAN ? 1 : 0;
	}

	GenerateSkillList();
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

	STARGETID sChaseTarID(CROW_PC, m_dwGaeaID, m_vPosition);
	m_pGaeaClient->SetChaseCam(sChaseTarID, true);
	return S_OK;
}

void GLCharacter::PreLoadEffectFull()
{
	SKILL_MAP_CITER iter = m_ExpSkills.begin();
	for ( ; iter!=m_ExpSkills.end(); ++iter )
	{
		const SCHARSKILL &sSkill = (*iter).second;
		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( sSkill.sNativeID.wMainID, sSkill.sNativeID.wSubID );
		if( !pSkill )
			continue;

		PreLoadEffect( pSkill );
		
		if( pSkill->m_sEXT_DATA.idMultiTargetSkill != NATIVEID_NULL() )
		{ //this skill has a second skill.
			pSkill = GLSkillMan::GetInstance().GetData( pSkill->m_sEXT_DATA.idMultiTargetSkill.wMainID, 
														pSkill->m_sEXT_DATA.idMultiTargetSkill.wSubID );
			PreLoadEffect( pSkill );
		} //if
	}
}

void GLCharacter::PreLoadEffect( const PGLSKILL pSkill )
{
	if ( !pSkill )
		return;

	EMELEMENT emELMT = ( pSkill->m_sAPPLY.emELEMENT==EMELEMENT_ARM ) ? GET_ITEM_ELMT() : (EMELEMENT_SPIRIT);

	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARG(emELMT), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETSELFZONE01(emELMT), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETSELFZONE02(emELMT), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE01(emELMT), TRUE, TRUE );
	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( pSkill->m_sEXT_DATA.GETTARGZONE02(emELMT), TRUE, TRUE );	
}

BOOL GLCharacter::IsVALID_STORAGE ( DWORD dwChannel )
{
	if ( dwChannel >= EMSTORAGE_CHANNEL )	return FALSE;
	return m_bStorage[dwChannel];
}

BOOL GLCharacter::GET_PRETRADE_ITEM ( SITEMCUSTOM& sItemCustom )
{
	SINVEN_POS sPreTradeItem = m_pGaeaClient->GetTradeClient()->GetPreItem();

	if (!sPreTradeItem.VALID())
        return FALSE;

	SINVENITEM* pResistItem = m_cInventory.GetItem(sPreTradeItem.wPosX, sPreTradeItem.wPosY);
	if (!pResistItem)
		return FALSE;
	   
	sItemCustom = pResistItem->sItemCustom;
	return TRUE;
}

BOOL GLCharacter::IsPostBoxWindowOpen()
{
	return false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::CF_PostBox_IsOpen ) ? FALSE : TRUE;
}

BOOL GLCharacter::IsPrivateMarketSerchWindowOpen()
{
	return false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_IsOpen ) ? FALSE : TRUE;
}

BOOL GLCharacter::IsPrivateMarketWindowOpen()
{
	return false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) ? FALSE : TRUE;
}

void GLCharacter::ReSelectAnimation ()
{
	// 제스처중이면 애니메이션 리셋 안함.
	if ( !isAction(GLAT_TALK, GLAT_GATHERING) )
	{
		//	Note : 에니메이션 초기화.
		//
		if (m_emVehicle != EMVEHICLE_OFF)
		{
			int emType = m_sVehicle.m_emTYPE;
			m_emANISUBTYPE = (EMANI_SUBTYPE) ( AN_SUB_HOVERBOARD + emType );
		}

	    if ( m_pSkinChar->GETCURSTYPE() !=GLCHARLOGIC::m_emANISUBTYPE )
			SELECTANI ( m_pSkinChar, m_pSkinChar->GETCURMTYPE(), GLCHARLOGIC::m_emANISUBTYPE );
	}
}

HRESULT GLCharacter::DoActWait ( DoActWait_TYPE emDoActWaitType )
{
	m_dwWAIT = 0;
	SetActState(EM_ACT_WAITING);

	m_emDoActWaitTypePrev = m_emDoActWaitType;
	m_emDoActWaitType = emDoActWaitType;

	return S_OK;
}

HRESULT GLCharacter::Resurrect ( const int nHP, const int nMP, const int nSP, D3DXVECTOR3 vPos, bool bWait )
{
	FactEffectProcessing( EMFACTEFFECTPROCESSING_REBIRTH );

	//	부활시 채력치 회복값.
	INIT_RECOVER ();

	m_sHP.nNow = nHP;
	m_sMP.nNow = nMP;
	m_sSP.nNow = nSP;
	m_sCP.SetData( 0, MAX_CP );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

	//	이동된 멥에 적응.
	MoveActiveMap(m_pGaeaClient->GetActiveMap()->GetNaviMesh(), vPos, bWait);

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

	return S_OK;
}

HRESULT GLCharacter::MoveActiveMap ( boost::shared_ptr<NavigationMesh> spNavi, const D3DXVECTOR3 &vPos, bool bWait )
{
	if ( bWait ) 
		DoActWait ( DoActWait_MoveActiveMap );

	//	Note : 이동 제어 초기화.
	ClientActor::InitializeActorMove(vPos, spNavi);

// 	ClientActor::SetPosition(vPos);	
// 
// 	m_actorMove.Stop ();
// 	if ( pNavi )
// 	{
// 		m_actorMove.Create ( pNavi, vPos, -1 );
// 		GLCharacter::SetPosition ( vPos );
// 	}

	ReSetActState(EM_REQ_GATEOUT);

	// 이동 할 때마다 이벤트 시간을 요청한다.
	GLMSG::SNET_GM_LIMIT_EVENT_TIME_REQ NetMsg;
	NetMsg.dwGaeaID	= m_dwGaeaID;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	GfxInterfaceBase::GetInstance()->ClearNameList();

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		// 이동 할 때마다 베트남 탐닉 시스템 시간을 요청한다.
		GLMSG::SNETPC_VIETNAM_TIME_REQ NetVietnamMsg;
		NetMsg.dwGaeaID	= m_dwGaeaID;	
		m_pGaeaClient->NETSENDTOFIELD(&NetVietnamMsg);
	}
//#endif

	// Note : 우편 정보 요청
	m_pGaeaClient->GetPostClient()->ReqReceivePostInfo();
	/////////////////////////////////////////

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return E_FAIL;

	SNATIVEID mapID = pLandMClient->GetBaseMapID();
	SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(mapID);
	if ( pMapNode )
	{
		GLLevelFileClient cLevelFile(m_pGaeaClient);
		if( cLevelFile.LoadFile ( pMapNode->m_LevelFile.c_str(), NULL ) )

		{
			if( cLevelFile.GetUseLevelEtcFunc(EMETCFUNC_CONTROLCAM) )
			{
				const SLEVEL_ETC_FUNC* pEtcFunc = cLevelFile.GetLevelEtcFunc();
				if( !pEtcFunc )	return E_FAIL;

				D3DXVECTOR3 vFromPt, vLookAtPt, vUpPt;
				DxViewPort::GetInstance().GetViewTrans( vFromPt, vLookAtPt, vUpPt );
				vFromPt = pEtcFunc->m_sControlCam.vCamPos + vLookAtPt;

				DxViewPort::GetInstance().SetViewTrans( vFromPt, vLookAtPt, vUpPt );
				DxViewPort::GetInstance().SetCameraControl( TRUE, pEtcFunc->m_sControlCam.vCamPos, pEtcFunc->m_sControlCam.fUp, 
																	pEtcFunc->m_sControlCam.fDown, pEtcFunc->m_sControlCam.fLeft, 
																	pEtcFunc->m_sControlCam.fRight, pEtcFunc->m_sControlCam.fFar, 
																	pEtcFunc->m_sControlCam.fNear );
			}else
			{
				DxViewPort::GetInstance().SetCameraControl( FALSE );
			}
		}
	}

    PostEnterMap();

	return S_OK;
}

// *****************************************************
// Desc: 장착한 아이템 업데이트
// *****************************************************
HRESULT GLCharacter::UpdateSuit( BOOL bChangeHair, BOOL bChangeFace )
{
	//GASSERT(m_pd3dDevice);
	if( !m_pd3dDevice ) return E_FAIL;

	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() ) 
	{
		UpdateSuit_Weapon();
		return S_OK;
	}

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return E_FAIL;

	for ( int i = 0; i < PIECE_SIZE; i++ )
	{
		if( i == PIECE_HEAD || i == PIECE_HAIR )
			continue;
		ResetPiece( EMPIECECHAR( i ) );
	}

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	if ( bChangeFace )
	{
		// 얼굴
		if ( sCONST.dwHEADNUM > m_wFace )
		{
			std::string strHEAD_CPS = sCONST.strHEAD_CPS[m_wFace];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

			if ( pCharPart  || m_pSkinCharMulti )
			{
				SetPiece ( PIECE_HEAD, pCharPart->GetFileName(), strHEAD_CPS.c_str(),  m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
			}
		}
	}
	// 헤어스타일
	if( bChangeHair )
	{
		if ( sCONST.dwHAIRNUM > m_wHair )
		{
			std::string strHAIR_CPS = sCONST.strHAIR_CPS[m_wHair];
		
			PDXCHARPART pCharPart = NULL;
			pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

			if ( pCharPart  || m_pSkinCharMulti )
			{
				SetPiece ( PIECE_HAIR, pCharPart->GetFileName(), strHAIR_CPS.c_str(), m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
			}
		}
	}

	// 헤어컬러 변경 UI가 있다면 기본 헤어 칼라로 변경하지 말자.
	if( GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_HairColor_IsOpen ) == false )
		SetHairColor( m_wHairColor );

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();

	// 장착한 아이템 업데이트
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) ) continue;

		const SITEMCUSTOM &ItemCustom = m_PutOnItems[i];

		// 복장 아이템
		SNATIVEID nidITEM = ItemCustom.nidDISGUISE;
		if (nidITEM == SNATIVEID(false))
            nidITEM = ItemCustom.GetNativeID();
		
		if (!m_emVehicle && i == SLOT_VEHICLE)
        {
            nidITEM = SNATIVEID(false);
        }
		else if (  m_emVehicle && i == SLOT_VEHICLE )
		{
			nidITEM = m_sVehicle.GetSkinID();
			if ( nidITEM == SNATIVEID(false) )
			{
				m_sVehicle.m_sVehicleID = ItemCustom.GetNativeID();
				nidITEM = ItemCustom.GetNativeID();
			}
		}

		// 유효하지 않으면
		if ( nidITEM == SNATIVEID(false) )
		{
			//	Note : 기본 스킨과 지금 장착된 스킨이 틀릴 경우.
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
				if ( pCharPart && strcmp( pSkinPiece->GetFileName(), pCharPart->GetFileName() ) )
				{
					SetPiece ( pSkinPiece->GetFileName(), m_pd3dDevice, 0X0, 0, EMSCD_ALL_THREAD );
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
		// 유효하면
		else
		{
			const SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
			if ( pItem ) 
			{
				if( i == SLOT_VEHICLE )
				{
					// VEHICLE_TYPE_BOARD ==  *.cps;
					// VEHICLE_TYPE_BIKE, CAR == *.vcf;
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
						SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), EMSCD_ALL_THREAD );
					}
				}
				else
				{
					SetPiece ( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, NULL, pItem->GetSelfBodyEffect() );

                    // Note : 추가적 Grade 효과
                    EMSLOT emSlot = EMSLOT(i);
                    if ( emSlot == SLOT_UPPER || emSlot == SLOT_LOWER || emSlot == SLOT_HAND || emSlot == SLOT_FOOT )
                    {
                        const TSTRING& strGradeCps = NSGradeScriptMan::GetGradeCps( m_wSex, emSlot, ItemCustom.GETGRADE_EFFECT() );
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
					if ( m_PutOnItems[ i ].GetNativeID() != NATIVEID_NULL() )
					{
						if ( m_PutOnItems[ i ].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
							&& m_PutOnItems[ i ].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
						{
							PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
							if ( pCharPart )
							{
								pCharPart->SetHairColor( m_PutOnItems[ i ].dwMainColor );
								pCharPart->SetSubColor( m_PutOnItems[ i ].dwSubColor );
							}
						}
					}

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

// 	// GeneralEffect 갱신;
// 	UpdateGeneralEffect( bSuit, sItemID );

	//과학부 총때문에 EX피스를 읽어온다
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) ) continue;

		const SITEMCUSTOM &ItemCustom = m_PutOnItems[i];

		// 복장 아이템
		SNATIVEID nidITEM = ItemCustom.nidDISGUISE;
		if (nidITEM==SNATIVEID(false))
            nidITEM = ItemCustom.GetNativeID();

		if (!m_emVehicle && i == SLOT_VEHICLE)
        {
            nidITEM = SNATIVEID(false);
        }
		else if (m_emVehicle && i == SLOT_VEHICLE)
		{
			nidITEM = m_sVehicle.GetSkinID();
			if ( nidITEM == SNATIVEID(false) )
			{
				m_sVehicle.m_sVehicleID = ItemCustom.GetNativeID();
				nidITEM = ItemCustom.GetNativeID();
			}
		}

		// 유효하지 않으면
		if ( nidITEM == SNATIVEID(false) )
		{
		}
		// 유효하면
		else
		{
			SITEM* pItem = GLogicData::GetInstance().GetItem ( nidITEM );
			if ( pItem && pItem->GetWearingFileEx(emIndex)[0] ) 
			{
				EMPIECECHAR emPiece = PIECE_NONE;
				SetPiece ( pItem->GetWearingFileEx(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, &emPiece, pItem->GetSelfBodyEffect() );

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

	UpdateGeneralEffect();

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect ( 
		STARGETID(CROW_PC,m_dwGaeaID, vPosition), 
		m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, GetTransMatrix(), vDirect );

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	if (m_dwTransformSkill != SNATIVEID::ID_NULL &&  m_dwTransformSkill < SKILLFACT_SIZE)
	{

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData(m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wMainID, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID.wSubID );
		if( pSkill ) 
		{
			for( BYTE i = 0; i < SKILL::EMTRANSFORM_NSIZE; i++ )
			{
				if( m_wSex == 1 )
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

	// 필요 Effect 및 배운 스킬들의 Effect 들을 미리로딩하도록 한다.
	PreLoadEffectFull();

	//!! SF_TODO
	// 세트아이템 구성 미리보기 UI가 활성화 되어있다면 아이템 교체시 갱신한다;
	/*if ( m_pInterface->UiIsVisibleGroup(SETITEM_PREVIEWWINDOW) )
	{
		m_pInterface->ResetSetitemUIComposition();
	}*/

	return S_OK;
}

HRESULT GLCharacter::UpdateSuit_Weapon()
{
	if( !m_pd3dDevice ) 
		return E_FAIL;

	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);

	SetHairColor( m_wHairColor );

	const D3DXVECTOR3& vPosition = GetPosition();
	const D3DXVECTOR3& vDirect = GetDirect();

	// 장착한 아이템 업데이트
	for ( int i=0; i<SLOT_NSIZE_S_2; i++ )
	{	
		// 현재 장착중인 무기가 아니면 넘어간다.
		if( !IsCurUseArm( EMSLOT(i) ) )
			continue;

		const SITEMCUSTOM &ItemCustom = m_PutOnItems[i];

		// 복장 아이템
		SNATIVEID nidITEM = ItemCustom.nidDISGUISE;
		if (nidITEM == SNATIVEID(false))
            nidITEM = ItemCustom.GetNativeID();
		
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
					SetPiece( pItem->GetWearingFile(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, NULL, pItem->GetSelfBodyEffect() );
					SetPiece( pItem->GetWearingFileEx(emIndex), m_pd3dDevice, NULL, ItemCustom.GETGRADE_EFFECT(), EMSCD_ALL_THREAD, FALSE, NULL, pItem->GetSelfBodyEffect() );
					break;

				default:
					break;
				};	

				// Note : 추가적 Grade 효과
				EMSLOT emSlot = EMSLOT(i);
				if ( emSlot == SLOT_UPPER || emSlot == SLOT_LOWER || emSlot == SLOT_HAND || emSlot == SLOT_FOOT )
				{
					const TSTRING& strGradeCps = NSGradeScriptMan::GetGradeCps( m_wSex, emSlot, ItemCustom.GETGRADE_EFFECT() );
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
				if ( m_PutOnItems[ i ].GetNativeID() != NATIVEID_NULL() )
				{
					if ( m_PutOnItems[ i ].dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 )
						&& m_PutOnItems[ i ].dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
					{
						PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE(EMSLOT(i)) );
						if ( pCharPart )
						{
							pCharPart->SetHairColor( m_PutOnItems[ i ].dwMainColor );
							pCharPart->SetSubColor( m_PutOnItems[ i ].dwSubColor );
						}
					}
				}
			}
		}
	}

	UpdateGeneralEffect();

	//	Note : 스킬 버프, 상태이상 효과 생성.
	FACTEFF::ReNewEffect
	( 
		STARGETID(CROW_PC,m_dwGaeaID, vPosition), 
		m_pSkinChar, 
		m_sSKILLFACT, 
		SKILLFACT_SIZE, 
		m_sSTATEBLOWS, 
		GetTransMatrix(), 
		vDirect 
	);

	if( GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.size() != 0 )
	{
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( m_pSkinChar, GLCONST_CHAR::cCONSTCLASS[emIndex].strCLASS_EFFECT.c_str(), &vDirect );
	}

	// 필요 Effect 및 배운 스킬들의 Effect 들을 미리로딩하도록 한다.
	PreLoadEffectFull();

	return S_OK;
}

void GLCharacter::ResetPiece( int i )
{
	if ( m_pSkinChar )
		m_pSkinChar->ResetPiece( EMPIECECHAR( i ) );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->ResetPiece( EMPIECECHAR( i ) );
}

HRESULT GLCharacter::SetPiece( const TCHAR* szFile, 
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
		hr = m_pSkinChar->SetPiece( szFile, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );
		if ( E_FAIL == hr )
			return hr;
	}

	// 메인의 세팅에 실패하면 분신의 세팅에는 자동으로 실패한다;
	if ( m_pSkinCharMulti )
		hr = m_pSkinCharMulti->SetPiece( szFile, pd3dDevice, dwFlag, nLevel, emFlags, bAttachSub, pOutPiecetype, pSelfBodyEffect );

	return hr;
}

HRESULT GLCharacter::SetPiece(EMPIECECHAR emPieceChar,
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

void GLCharacter::SELECTANI( DxSkinChar* pRenderChar, const TCHAR* szAnim, DWORD dwFlags )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( szAnim, dwFlags );

	if ( m_pSkinCharMulti ) 
		m_pSkinCharMulti->SELECTANI ( szAnim, dwFlags );

}

void GLCharacter::SELECTANI( DxSkinChar* pRenderChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwFlags, DWORD dwSelect )
{
	if ( pRenderChar )
		pRenderChar->SELECTANI ( MType, SType, dwFlags, dwSelect );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTANI ( MType, SType, dwFlags, dwSelect );
}

void GLCharacter::DEFAULTANI( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->DEFAULTANI();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->DEFAULTANI();
}

void GLCharacter::TOSTARTTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOSTARTTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOSTARTTIME();
}


void GLCharacter::TOENDTIME( DxSkinChar* pRenderChar )
{
	if ( pRenderChar )
		pRenderChar->TOENDTIME();

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->TOENDTIME();

}


void GLCharacter::SetPartRend( DxSkinChar* pRenderChar, EMPIECECHAR emPIECE, BOOL bRender )
{
	if ( pRenderChar )
		pRenderChar->SetPartRend( emPIECE, bRender );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPartRend( emPIECE, bRender );
}


void GLCharacter::SetHairColor( WORD wColor )
{
	m_pSkinChar->SetHairColor( wColor );
	m_pSkinCharMulti->SetHairColor( wColor );
}


BOOL GLCharacter::PutPassiveEffect( DxSkinChar *pSkinChar, const char* szEffFile, const D3DXVECTOR3 *pDir, float fTarDir/*=0.0f */ )
{
	DxEffcharDataMan::GetInstance().PutPassiveEffect ( pSkinChar, szEffFile, pDir, fTarDir );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->PutPassiveEffect( szEffFile, pDir, fTarDir  );

	return TRUE;
}


void GLCharacter::OutEffect( DxSkinChar *pSkinChar, std::string strEffFile )
{
	DxEffcharDataMan::GetInstance().OutEffect ( pSkinChar, strEffFile );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->OutEffect( strEffFile );
}
//
//void GLCharacter::CalculateAmbientColor( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan, const D3DXVECTOR3& vPos )
//{
//	m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, vPos );
//
//	if ( m_pSkinCharMulti )
//		m_pSkinCharMulti->CalculateAmbientColor( pd3dDevice, pLandMan, vPos );
//
//}


void GLCharacter::SELECTSKILLANI(  DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE */ )
{
	pSkinChar->SELECTSKILLANI ( MType, SType );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SELECTSKILLANI( MType, SType );
}

BOOL GLCharacter::IsANI_SELECTANI( DxSkinChar *pSkinChar, const TCHAR* szAnim, DWORD dwFlags/*=NULL */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( szAnim, dwFlags );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI ( szAnim, dwFlags );

	return bOK;
}

BOOL GLCharacter::IsANI_SELECTANI( DxSkinChar *pSkinChar, EMANI_MAINTYPE MType, EMANI_SUBTYPE SType/*=AN_SUB_NONE*/, DWORD dwFlags/*=NULL*/, DWORD dwSelect/*=0 */ )
{
	BOOL bOK = FALSE;
	bOK = pSkinChar->IsANI_SELECTANI ( MType, SType, dwFlags, dwSelect );

	//if ( m_pSkinCharMulti )
	//	bOK = m_pSkinCharMulti->IsANI_SELECTANI (  MType, SType, dwFlags, dwSelect  );

	return bOK;
}


void GLCharacter::SetPositionSkin(  DxSkinChar *pSkinChar, const D3DXVECTOR3& vPos )
{
	pSkinChar->SetPosition ( vPos );

	if ( m_pSkinCharMulti )
		m_pSkinCharMulti->SetPosition( vPos );
}

HRESULT GLCharacter::FrameMoveSkin( DxSkinChar *pSkinChar, const float fTime, const float fElapsedTime, BOOL bNoneOverFrame, const D3DXMATRIX &matRot, const BOOL bContinue/*=TRUE*/, BOOL const bFreeze/*=FALSE*/, const BOOL bAttackMode/*=TRUE*/, const BOOL bBoardRender /*= FALSE */ )
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

void GLCharacter::UpdateGeneralEffect()
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

		const SITEMCUSTOM &ItemCustom = m_PutOnItems[i];
		SNATIVEID nidITEM = ItemCustom.nidDISGUISE;
		if (nidITEM == SNATIVEID(false))
		{
			nidITEM = ItemCustom.GetNativeID();
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

DWORD GLCharacter::GetClubID() const
{
    // m_pMyClub == Null일 확률 없음;
    return m_pMyClub->DbNum();
}

bool GLCharacter::IsValidClub() const
{
	return m_pMyClub->IsValid();
}

bool GLCharacter::CheckIsValidClub( int dbClubID )
{
	return ( dbClubID != CLUB_NULL ? true : false );
}


WORD GLCharacter::GetBodyRadius ()
{
	return GETBODYRADIUS();
}

// HRESULT GLCharacter::SetPosition ( D3DXVECTOR3 vPos )
// { // QQQ
// 	ClientActor::SetPosition(vPos);	
// 	m_actorMove.SetPosition ( vPos, -1 );
// 
// 	if ( m_actorMove.PathIsActive() )
// 		m_actorMove.Stop();
// 
// 	if( m_bMiniMapMove && m_pInterface )
// 		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_HideMoveIcon );
// 
// 	return S_OK;
// }

EMCONFT_TYPE GLCharacter::GetConftType(DWORD dwID)
{
	std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar(dwID);
	if (!pChar)
        return EMCONFT_NONE;

	if (IsClubMaster() && pChar->IsClubMaster())
        return EMCONFT_GUILD;
	else if (IsPartyMaster() && pChar->IsPartyMaster())
        return EMCONFT_PARTY;
	else
        return EMCONFT_ONE;

	return EMCONFT_NONE;
}

void GLCharacter::SetMovement( const CharState state )
{
    m_bMovement = state;
    SetLockMovement(!state);
}

BOOL GLCharacter::ActionMoveTo ( float fTime, D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt, BOOL bContinue, BOOL bREACT, BOOL bMOBCOLL )
{
	// 상점이 개설된 상태에서 이동할 수 없다.
	if ( m_sPMarket.IsOpen() )
		return FALSE;

	if ( m_bSTATE_STUN )
		return FALSE;
	
    if ( isAction(GLAT_DIE, GLAT_FALLING) )
		return FALSE;
	

// 	NavigationMesh* pNavi = m_actorMove.GetParentMesh();
// 	if( !pNavi )
// 		return FALSE;
// 
	BOOL bColl=FALSE;
	DWORD dwCollID = 0;
	D3DXVECTOR3 vCollPos = ( vFromPt + vTargetPt ) / 2.0f;
// 	pNavi->IsCollision( vFromPt, vTargetPt, vCollPos, dwCollID, bColl );
	if  ( !ClientActor::IsCollision(vFromPt, vTargetPt, vCollPos, dwCollID, bColl) )
		return FALSE;

	if ( !bColl )
	{
		if ( IsActState(EM_ACT_MACROMODE) )
			GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_RESERVEACTIONMOVE_FAIL);

		return FALSE;
	}

	//	이동하고자 하는 영역이 몹이 있는 영역과 겹칠 경우 이동 불가.
	if ( bMOBCOLL )
	{
		bColl = m_pGaeaClient->IsCollisionMobToPoint( vCollPos, GETBODYRADIUS() );
		if (bColl)
			return FALSE;
	}

	
	const BOOL bGoto = ClientActor::GotoLocation(vCollPos, 10.0f);

	if ( bGoto )
	{
		m_bMiniMapMove = false;
		m_fMiniMapMoveTime = 0.0f;

		TurnAction ( GLAT_MOVE );
		m_sActionID.vPos = vCollPos;

		//	Note : 이동 메시지 정보.
		GLMSG::SNETPC_GOTO NetMsg;
		NetMsg.dwActState = GetActState();
		NetMsg.vCurPos = GetPosition();;
		NetMsg.vTarPos = vCollPos;
		m_sLastMsgMove = NetMsg;

		if( RENDERPARAM::bClickEffect )
		{
			GLInputManager &cInputManager = GLInputManager::GetInstance();

			if( cInputManager.GetKeyState( EMSHORTCUTS_MOVING ) & DXKEY_DOWN )
				DxEffGameHelp::GetInstance().ResetClickEffect();
		}

		//	Note : 메세지 발생. ( 지속적으로 발생하는 이동 콘트롤중 일부만 메시지 발생. )
		//
		if ( bContinue && (m_fLastMsgMoveSend+m_fELAPS_MOVE) > fTime )
            return TRUE;

		if ( ClientActor::PathIsActive() )
		{
			m_fLastMsgMoveSend = fTime;
			m_sLastMsgMoveSend = NetMsg;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}

	return TRUE;
}

BOOL GLCharacter::DominationMoveTo(float fTime, D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt, BOOL bContinue)
{	
	if ( m_sPMarket.IsOpen() )
		return FALSE;

	if ( m_bSTATE_STUN )
		return FALSE;

	if ( isAction(GLAT_DIE, GLAT_FALLING) )
		return FALSE;

	if ( bContinue && (m_fLastMsgMoveSend+m_fELAPS_MOVE) > fTime )
		return TRUE;

	TurnAction(GLAT_IDLE);

	GLMSG::SNETPC_DOMINATE_MOVETO_CF NetMsg(fTime, vFromPt, vTargetPt, m_sDOMINATE.sControlTargetID);
	m_fLastMsgMoveSend = fTime;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return TRUE;
}

BOOL GLCharacter::LargeMapMoveTo( D3DXVECTOR3* pPos, BOOL bContinue )
{
// 	NavigationMesh* pNavi = m_actorMove.GetParentMesh();
// 	if( !pNavi )
// 		return FALSE;
//      
//     pNavi->GetAABB(vMax, vMin);

 	if ( !this->m_bSTATE_CONTROL 
 		|| NSGUI::IsCharMoveAllBlock() )
 	{
	   return FALSE;
 	}	

	D3DXVECTOR3 vMax,vMin;
	if ( !ClientActor::GetAABB(vMax, vMin) )
		return FALSE;

	D3DXVECTOR3 vFrom = *pPos;
	vFrom.y = (vMax.y + 0.5f);

	D3DXVECTOR3 vTarget = *pPos;
	vTarget.y = (vMin.y - 0.5f);

	BOOL bMove = ActionMoveTo( 0.0f, vFrom, vTarget, bContinue, TRUE );
	if( !bMove )
		return FALSE;

	m_bMiniMapMove = true;
	m_fMiniMapMoveTime = 0.0f;

	return TRUE;
}

void GLCharacter::EndAction( EMACTIONTYPE emAction )
{
	switch( GetAction() )
	{
	case GLAT_ATTACK:
        // 공격중 행동을 취한것으로 여겨 공격취소를 통보한다.;
        {
            if( emAction<=GLAT_TALK )
            {
                //	Note : 공격 종료 메세지 발생.
                GLMSG::SNETPC_ATTACK_CANCEL NetMsg;
                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            } //if
        }
		break;

	case GLAT_SKILL_WAIT:
		if ( emAction == GLAT_SKILL || emAction == GLAT_SKILL_WAIT )
		{
			// 정해진 수순.
			break;
		}

		// GLAT_SKILL_WAIT 다음에는 GLAT_SKILL, GLAT_SKILL_WAIT 나와야 하는데 그렇지 못했다.
		// 그래서 case GLAT_SKILL: 를 실행하도록 한다.

	case GLAT_SKILL:
        // 스킬 사용중 행동을 취한것으로 여겨 스킬취소를 통보한다.;
        {
            if( emAction == GLAT_MOVE || 
				emAction == GLAT_ATTACK ||
				emAction == GLAT_PUSHPULL) //|| emAction == GLAT_SHOCK )
            {
				if ( m_ReservationAction.IsExistReaction(REACT_SKILL) == true )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
				}

                GLMSG::SNETPC_SKILL_CANCEL NetMsg;
                m_pGaeaClient->NETSENDTOFIELD ( &NetMsg );
            } //if

            m_idACTIVESKILL = NATIVEID_NULL();
            SetDefenseSkill( false );

            // [4/15/2014 hsshin]
            // 예약스킬을 사용시 간혹 타이밍 문제로 여기로 들어오게 되었을때;
            // 타겟을 초기화 시켜주는 코드때문에 예약스킬의 타겟이 초기화되는 문제가 있다;
            // 예약스킬시스템 구조상의 문제로 보이고 타겟 초기화 코드는 너무 예전부터 있던 코드라서;
            // 예약스킬발동에 한해서 타겟 초기화를 하지 않도록 수정;
            if ( !(emAction == GLAT_SKILL || emAction == GLAT_SKILL_WAIT) )
            {
                m_sSkillTargetData.nTarget = 0;
            }

			ResetHiddenWeaponPiece();
        }
		break;

	case GLAT_FALLING:
	case GLAT_DIE:
		break;
	case GLAT_FEIGN_FALLING :
	case GLAT_FEIGN_DIE		:
		{
			if( emAction == GLAT_FALLING ||
				emAction == GLAT_DIE )
			{
				ReSetActState(EM_ACT_FEIGN_DIE);
			}
		}
		break;
	case GLAT_CONFT_END:
        {
            ReSetActState(EM_ACT_CONFT_WIN);
        }
		break;

	case GLAT_GATHERING:
        {
            ReqCancelGathering();
        }
		break;
	};
} //GLCharacter::EndAction

void GLCharacter::BeginAction( EMACTIONTYPE emAction )
{
	if ( IsActState(EM_ACT_OBSERVE) == true )
	{
		switch ( emAction )
		{
		case GLAT_IDLE:
			m_fIdleTime = 0.0f;
			break;
		case GLAT_MOVE:
			break;
		default:
			emAction = GLAT_IDLE;
			m_fIdleTime = 0.0f;
			break;
		}

		SetAction( emAction );
		return;
	}

	SetAction( emAction );
	switch ( emAction )
	{
	case GLAT_IDLE:
		m_fIdleTime = 0.0f;
		if (m_emVehicle == EMVEHICLE_DRIVER)
		{
			std::tr1::shared_ptr<GLCharClient> pCharClient;
			for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
			{
				pCharClient = m_pGaeaClient->GetChar(m_sVehicle.PassengerId(i));
				if( pCharClient )
					pCharClient->TurnAction( emAction );
			} //for
		} //if
		break;

	case GLAT_MOVE:
		if (m_emVehicle == EMVEHICLE_DRIVER)
		{
			std::tr1::shared_ptr<GLCharClient> pCharClient;
			for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
			{
				pCharClient = m_pGaeaClient->GetChar(m_sVehicle.PassengerId(i));
				if( pCharClient )
					pCharClient->TurnAction( emAction );
			}
		}
		break;

	case GLAT_ATTACK:
		if( m_pSkinChar )			// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
			SELECTANI( m_pSkinChar, AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );
		StartAttackProc ();
		break;

	case GLAT_SKILL_WAIT:
		if( m_pSkinChar )			// 같은 애니메이션으로 공격하면 애니메이션이 중간 중간 안나오는 경우가 생겨서, 리셋을 해준다.
			SELECTANI( m_pSkinChar, AN_GUARD_N, GLCHARLOGIC::m_emANISUBTYPE );

		// StartSkillProc 과 분리되었다.
		// m_nattSTEP, m_fattTIMER, m_emANIMAINSKILL, m_emANISUBSKILL 등.. Data 들이 초기화 및 셋팅이 되어있어야 한다.
		StartSkillWaitProc();
		break;

	case GLAT_SKILL:
		SetHiddenWeaponPiece();
		StartSkillProc();
		break;

	case GLAT_PUSHPULL:
		if( m_emVehicle == EMVEHICLE_DRIVER )
		{
			std::tr1::shared_ptr<GLCharClient> pCharClient;
			for( WORD i = 1; i < MAX_PASSENGER_COUNT; i++ )
			{
				pCharClient = m_pGaeaClient->GetChar(m_sVehicle.PassengerId(i));
				if( pCharClient )
					pCharClient->TurnAction( emAction );
			} //for
		} //if
		break;

	case GLAT_GATHERING:
		break;

	case GLAT_CONFT_END:
		break;

	case GLAT_MOTION:
		break;

	case GLAT_FALLING:
		{
			// 초기화 개념으로 사용;
			PrevUPDATE_DATA(0.0f);
			m_sHP.nNow = 0;
			FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );

			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
		}		
		break;

	case GLAT_DIE:
		//	채력을 0로 초기화.
		//	( 클라이언트에서 회복량 보간으로 서버에서 채력이 0 일때 0가 아닐수 있음 )
		m_sHP.nNow = 0;	

		FactEffectProcessing( EMFACTEFFECTPROCESSING_DEAD );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );
		ResetReservationAction(true);
		break;
	case GLAT_FEIGN_FALLING :
	case GLAT_FEIGN_DIE		:
		break;

	default:
		GASSERT("GLCharacter::TurnAction() 준비되지 않은 ACTION 이 들어왔습니다.");
		break;
	};
} //GLCharacter::BeginAction

void GLCharacter::TurnAction( EMACTIONTYPE toAction )
{
	// FALLING 이나 DIE 액션에서 IDLE 액션으로 이동하는 것은 부활처리중에만 이루어질 수 있다;
	if ( ( GetAction() == GLAT_FALLING || GetAction() == GLAT_DIE ) &&
		toAction == GLAT_IDLE &&
		false == IsActState( EM_ACT_DIE ) )
	{
		return;
	}

    // 현재 액션을 끝냄;
	EndAction( toAction );

    // PVE 인던용 이동 잠금상태일때는 움직이는 행동을 취하려고하면 idle 상태로 바꿈;
    if ( m_bLockMoveMent )
    {
        const EMACTIONTYPE _currentAction = GetAction();
        if ( _currentAction < GLAT_TALK )
            toAction = GLAT_IDLE;
    }

	if ( GLAT_PUSHPULL == GetAction() && 
		( toAction == GLAT_SKILL || toAction == GLAT_SKILL_WAIT ) )
	{
		toAction = GLAT_PUSHPULL;
	}
	
	const EMACTIONTYPE eCurAct = GetAction();

	if( (eCurAct == GLAT_FEIGN_FALLING || eCurAct == GLAT_FEIGN_DIE) && toAction == GLAT_SHOCK )
		return;

    // 다음 액션를 시작함;
    BeginAction( toAction );

	if ( ClientActor::PathIsActive() )
	{
		if( !isAction(GLAT_MOVE, GLAT_PUSHPULL) )
			ClientActor::StopActorMove();
	} //if
}

// void GLCharacter::SetAction( EMACTIONTYPE toAction )
// {
// 	TurnAction( toAction );
// }

void GLCharacter::ResetAction()
{
	if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);

		GLMSG::SNETPC_SKILL_CANCEL NetMsg;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
	else
	if( isAction(GLAT_FEIGN_FALLING, GLAT_FEIGN_DIE) )
	{
		int iSlotIndex = RF_FEIGN_DEATH( m_EffSkillVarSet ).GetSlotIdx();

		if( iSlotIndex >= 0 ) 
		{
			SSKILLFACT& rTemp = GetSkillFact( (size_t)iSlotIndex );
			ReqSkillHoldReset( rTemp.sNATIVEID, (size_t)iSlotIndex );
		}		
	}
	else if (isAction(GLAT_MOVE))
	{
		const D3DXVECTOR3& vPosition = GetPosition();
		GLMSG::SNETPC_GOTO sMSG(m_sLastMsgMoveSend);
		sMSG.dwActState = GetActState();
		sMSG.vCurPos	= vPosition;
		sMSG.vTarPos	= vPosition;
		m_pGaeaClient->NETSENDTOFIELD(&sMSG);
	}else if ( IsProducting() )
	{
		GLProductClient* pProduct = m_pGaeaClient->GetProductClient();
		if( pProduct )
		{
			pProduct->ReqProductCancel();
		}		
	}

	TurnAction(GLAT_IDLE);
}

BOOL GLCharacter::IsCtrlBlockBody ()
{
	if ( m_bSTATE_STUN )
		return TRUE;

	if( isAction( GLAT_SHOCK ) )
		return TRUE;

	if( isAction( GLAT_PUSHPULL ) )
		return TRUE;

	if( isAction( GLAT_CONFT_END ) )
		return TRUE;

	if( isAction( GLAT_GATHERING ) && 
		( m_dwANISUBGESTURE == 0 || m_dwANISUBGESTURE == 1 ) ) 
	{
		return TRUE;
	} //if

	return FALSE;
}

// BOOL GLCharacter::IsValidBody () const
// {
// 	if( IsActState(EM_ACT_WAITING) )
// 		return FALSE;
// 
// 	if( IsActState(EM_REQ_GATEOUT) )
// 		return FALSE;
// 
// 	if( IsActState(EM_ACT_DIE) )
// 		return FALSE;
// 
// 	return ( GetAction() < GLAT_FALLING );
// }

bool GLCharacter::IsProducting()
{
	if( isAction( GLAT_MOTION ) &&
		m_eMotionMID == GLCONST_CHAR::sProductMotionID.wMainID &&
		m_eMotionSID == GLCONST_CHAR::sProductMotionID.wSubID )
	{
		return true;
	}

	return false;
}

BOOL GLCharacter::IsDie ()
{
	if ( !IsActState(EM_ACT_DIE) &&
		 isAction(GLAT_DIE) )
	{
		return TRUE;
	} //if

	return FALSE;
}

const bool GLCharacter::IsIdleCharacter()
{
	return IsIdle();
}

const GLPartyID& GLCharacter::GetPartyID(void) const
{
	return m_pGaeaClient->GetMyPartyClient()->GetPartyID();
}

BOOL GLCharacter::IsPartyMaster()
{
	return m_pGaeaClient->GetMyPartyClient()->isMaster(m_dwGaeaID);
}

BOOL GLCharacter::IsPartyMem ()
{
	return ( m_pGaeaClient->GetMyPartyClient()->isValid() != NULL );
}

bool GLCharacter::IsClubMaster()
{
	return (m_pMyClub->MasterCharDbNum() == m_CharDbNum);
}

BOOL GLCharacter::IsCDCertify ()
{
	return m_pMyClub->IsMemberFlgCDCertify(m_CharDbNum);
}

bool GLCharacter::IsHaveAllianceDisAuthority()	// 클럽 동맹해지 권한을 가지고 있는가.
{
	return m_pMyClub->HaveAllianceDisAuthority(m_CharDbNum);
}

std::string GLCharacter::GetClubName()
{
	return m_pMyClub->Name();
}

void GLCharacter::P2PReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_P2P& sP2PReaction = InOutReaction;

	if ( m_bSTATE_STUN )				
		return;
	
	DWORD dwTargetGUID = sP2PReaction.GetTARID().GaeaId;

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

	if ( !pLandMan )
	{
		sP2PReaction.SETPRESERVATION(false);
		return;
	}

	std::tr1::shared_ptr<GLCharClient> pCharClient = pLandMan->GetChar ( dwTargetGUID );
	if ( !pCharClient )
	{
		sP2PReaction.SETPRESERVATION(false);
		return;
	}

	const D3DXVECTOR3 &vTarPos = pCharClient->GetPosition();
	const D3DXVECTOR3& vPosition = GetPosition();

	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition - vTarPos) );
	if ( isAction(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

	WORD wP2PRange = pCharClient->GetBodyRadius() + GETBODYRADIUS() + 25;
	WORD wP2PAbleDis = wP2PRange + 2;

	if ( fDist>wP2PAbleDis )
	{
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sP2PReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			if ( fMoveDist > wP2PAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vDir = vTarPos - vPosition;
				D3DXVec3Normalize ( &vDir, &vDir );

				D3DXVECTOR3 vMoveTo;
				vMoveTo   = vPosition + vDir*wP2PRange;
				vMoveTo.y = vTarPos.y;

				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction(sReaction);
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction(iterReaction, sP2PReaction);
		return;
	}

	//	Note : PtoP 메뉴.
	//
	//bool bVALID_CLUB = m_sCLUB.m_dwID!=CLUB_NULL;
	//bool bMASTER = m_sCLUB.m_dwMasterID==m_dwCharID;

	if( !GLCONST_CHAR::bBATTLEROYAL && !pCharClient->IsNonTargetable() )
	{
		m_PtoPTargetID = pCharClient->GetGaeaID();

		PreservedReaction(sP2PReaction);
		m_pGaeaClient->ReqChaDbNum( pCharClient->GetName() );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_OpenInteractMenu, "-s", pCharClient->GetName() );
	}
}

LuaTable GLCharacter::GetHPRevoverNum()
{
	std::vector<CString> _vecCStr;
	CString strTemp;	
	LuaTable tbStrList( GLWidgetScript::GetInstance().GetLuaState() );
		
	for ( int i = 0; i < GLCONST_CHAR::EMCONFT_RC_TYPENUM; i++ )
	{
		if ( USHRT_MAX <= GLCONST_CHAR::wCONFT_RC_TYPE[i] )
		{
			strTemp = ID2GAMEWORD( "UNLIMITED" );
			_vecCStr.push_back(strTemp);
		}
		else
		{
			strTemp.Format ( "%d", GLCONST_CHAR::wCONFT_RC_TYPE[i] );
			_vecCStr.push_back(strTemp);
		}		
	}

	unsigned int _Size = _vecCStr.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		tbStrList.set(_index + 1, _vecCStr[_index].GetBuffer());
	}

	return tbStrList;
}

LuaTable GLCharacter::GetHPRate_Mine()
{
	std::vector<CString> _vecCStr;
	CString strTemp;	
	LuaTable tbStrList( GLWidgetScript::GetInstance().GetLuaState() );

	for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; i++ )
	{
		strTemp.Format ( "%1.1f", GLCONST_CHAR::fCONFT_STATE_TYPE[i] );		
		_vecCStr.push_back(strTemp);
	}

	unsigned int _Size = _vecCStr.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		tbStrList.set(_index + 1, _vecCStr[_index].GetBuffer());
	}

	return tbStrList;
}

LuaTable GLCharacter::GetHPRate_Target()
{
	std::vector<CString> _vecCStr;
	CString strTemp;	
	LuaTable tbStrList( GLWidgetScript::GetInstance().GetLuaState() );

	for ( int i = 0; i < GLCONST_CHAR::EMCONFT_STATE_TYPENUM; i++ )
	{
		strTemp.Format ( "%1.1f", GLCONST_CHAR::fCONFT_STATE_TYPE[i] );	
		_vecCStr.push_back(strTemp);
	}

	unsigned int _Size = _vecCStr.size();
	for(unsigned int _index(0); _index < _Size; ++_index)
	{
		tbStrList.set(_index + 1, _vecCStr[_index].GetBuffer());
	}

	return tbStrList;
}

std::string	 GLCharacter::GetConftMessage()
{
	EMCONFT_TYPE emTYPE =GetConftType( m_PtoPTargetID );	

	std::string strCombine;
	switch ( emTYPE )
	{
		case EMCONFT_NONE:
			{
				strCombine = "";		
			}
			break;

		case EMCONFT_ONE:
			{
				strCombine = ID2GAMEINTEXT("CONFLICT_QUESTION");			
			}
			break;

		case EMCONFT_PARTY:
			{
				strCombine = ID2GAMEINTEXT("CONFLICT_QUESTION_PARTY");			
			}
			break;

		case EMCONFT_GUILD:
			{
				strCombine = ID2GAMEINTEXT("CONFLICT_QUESTION_GUILD");			
			}
			break;
	}

	return strCombine;
}

void GLCharacter::PvPReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_PVP& sPVPReaction = InOutReaction;

	if ( m_bSTATE_STUN )	
		return;	

	//! 다른 행동을 취소하지 않는 옵션이 있다면 끝날때까지 대기
	if ( sPVPReaction.IsNONCANCEL() )
	{
		if ( !isAction(GLAT_IDLE) )
		{
			//! 행위 재예약
			m_ReservationAction.ReRegisterReaction( m_ReservationAction.FrontIter(), sPVPReaction );
			return;
		}
	}

	// 공격애니메이션이 종료유무
	if ( !m_bAttackable ) 
	{
		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( m_ReservationAction.FrontIter(), sPVPReaction );
		return;
	}

	if (m_emVehicle)
    {
        if ( sPVPReaction.ISPRINTFAILED() ) 
        {
            PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VEHICLE_CANNOT_PVP_REACTION") ); 
        }

		sPVPReaction.SETPRESERVATION(false);
        return;
    }

    // Note : 모든 NPC관련 창을 닫는다.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseAllNpc );

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
		return;

	if ( pLand->IsPeaceZone() )
	{
		sPVPReaction.SETPRESERVATION(false);
		return;
	}

	const STARGETID& sTargetID = sPVPReaction.GetTARID();
	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();

	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition-vTarPos) );
	if ( isAction(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID );
	if (!pTarget)
	{
		sPVPReaction.SETPRESERVATION(false);
        return;
	}

	//	Note : 공격가능거리에 사정거리 옵션 값을 더해줌.
	//
	int nAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
	nAttackRange += (int) GETSUM_RANGE_PSY_LONG();
	if ( nAttackRange < 0 )
		nAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + 2;

	int nAttackAbleDis = nAttackRange + 2;

	if ( fDist>nAttackAbleDis )
	{
		//! 추적 안함 옵션이있다면 취소
		if ( sPVPReaction.IsNONTRACE() )
		{
			sPVPReaction.SETPRESERVATION(false);
			return;
		}

		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sPVPReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			if ( fMoveDist > nAttackAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo = vTarPos - vPos*nAttackRange;
				vMoveTo.y = vTarPos.y;

				//	Note : 리엑션 등록.
				//
				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sPVPReaction );
		return;
	}

	// 충돌이 되면 공격을 못한다.
	if ( pLand->IsLayerSimpleCollision( vPosition, vTarPos, 5.f ) )
		return;

	//	지형과 충돌하는지 검사.
	//	
	D3DXVECTOR3 vPos1 = vPosition; vPos1.y += 15.0f;
	D3DXVECTOR3 vPos2 = vTarPos; vPos2.y += 15.0f;
	
	BOOL bCollision;
	D3DXVECTOR3 vCollision;
	LPDXFRAME pDxFrame;

	DxLandMan *pLandMan = m_pGaeaClient->GetActiveMap()->GetLandMan();
	pLandMan->IsCollision(vPos1, vPos2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );
	if (bCollision)
	{
        return;
	}

	const PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( AN_ATTACK, GLCHARLOGIC::m_emANISUBTYPE );
	//  [12/21/2012 gbgim]
	const EMACTIONTYPE emAction = GetAction();
	if ( !pAnicont || emAction == GLAT_FALLING || emAction == GLAT_DIE )
	{
		sPVPReaction.SETPRESERVATION(false);
		return;
	}

	WORD wStrikeNum = pAnicont->m_pDxSkinAni->m_wStrikeCount;

	//	Note : 공격수행 가능 검사.
	//    
	EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);
	if ( emBeginFB==EMBEGINA_OK || emBeginFB==EMBEGINA_SP)
	{
		m_sActionID.emCrow = sTargetID.emCrow;
		m_sActionID.GaeaId = sTargetID.GaeaId;
		TurnAction ( GLAT_ATTACK );
	}

	PreservedReaction(sPVPReaction);
	return;
}

void GLCharacter::MobReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_MOB& sMobReaction = InOutReaction;

	if ( m_bSTATE_STUN )
		return;

	//! 다른 행동을 취소하지 않는 옵션이 있다면 끝날때까지 대기
	if ( sMobReaction.IsNONCANCEL() )
	{
		if ( !isAction(GLAT_IDLE) )
		{
			//! 행위 재예약
			m_ReservationAction.ReRegisterReaction( m_ReservationAction.FrontIter(), sMobReaction );
			return;
		}
	}

 	// 공격애니메이션이 종료유무
 	if ( !m_bAttackable ) 
 	{
 		//! 반복일때만 행위 재예약한다, 그러지않으면 공격 시작 후 여기서 한번더 등록하여 두번 공격함;
		if ( InOutReaction.ISPRESERVATION() )
 			m_ReservationAction.ReRegisterReaction( m_ReservationAction.FrontIter(), sMobReaction );

 		return;
 	}

	if ( m_emVehicle )
    {
        if ( sMobReaction.ISPRINTFAILED() )
        {
            PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VEHICLE_CANNOT_MOB_REACTION") ); 
        }

		sMobReaction.SETPRESERVATION(false);
        return;
    }

	// Note : 모든 NPC관련 창을 닫는다.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseAllNpc );

	const STARGETID& sTargetID = sMobReaction.GetTARID();
	m_sOLD_TARMOB = sTargetID;	

	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID );
	
	if ( !pTarget )
	{
		sMobReaction.SETPRESERVATION(false);
		return;
	}

	//	공격 받지 않음 옵션이 체크되어 있는지 검사한다.
	if ( pTarget->IsNonDamaged() )
	{
		if ( sMobReaction.ISPRINTFAILED() )
		{
			PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("CROW_NONDAMAGED") );
		}

		sMobReaction.SETPRESERVATION(false);
		return;
	}		

    // 적대 행위가 가능한지 여부를 검사함. (현재 시스템상 몹과의 상호작용은 적대행위 or 무반응 뿐이다.);
    if ( IsReActionable(sTargetID, EMIMPACT_SIDE_ENEMY) == false )
    {
        sMobReaction.SETPRESERVATION(false);
        return;
    }

	const D3DXVECTOR3& vTarPos = sTargetID.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();
	float fMoveDist = FLT_MAX;
	const float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition-vTarPos) );
	if ( isAction(GLAT_MOVE) )	
		fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

	//	Note : 공격가능거리에 사정거리 옵션 값을 더해줌.
	//
	int nAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + GETATTACKRANGE() + 2;
	nAttackRange += (int) GETSUM_RANGE_PSY_LONG();
	if ( nAttackRange < 0 )
		nAttackRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + 2;

	int nAttackAbleDis = nAttackRange + 2;
 
	if ( fDist > nAttackAbleDis )
	{
		//! 추적 안함 옵션이있다면 취소
		if ( sMobReaction.IsNONTRACE() )
		{
			sMobReaction.SETPRESERVATION(false);
			return;
		}

		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sMobReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			if ( fMoveDist > nAttackAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo = vTarPos - vPos*nAttackRange;
	//			vMoveTo.y = vTarPos.y;

				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sMobReaction );
		return;
	}

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
		return;

	// 충돌이 되면 공격을 못한다.
	if ( pLand->IsLayerSimpleCollision( vPosition, vTarPos, 5.f ) )
		return;

	//	지형과 충돌하는지 검사.
	//	
	D3DXVECTOR3 vPos1 = vPosition; vPos1.y += 15.0f;
	D3DXVECTOR3 vPos2 = vTarPos; vPos2.y += 15.0f;
	
	BOOL bCollision;
	D3DXVECTOR3 vCollision;
	LPDXFRAME pDxFrame;

	DxLandMan *pLandMan = m_pGaeaClient->GetActiveMap()->GetLandMan();
	pLandMan->IsCollision ( vPos1, vPos2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );
	if ( bCollision )	
	{
		sMobReaction.SETPRESERVATION(false);
		return;
	}

	const PANIMCONTNODE pAnicont = m_pSkinChar->GETANI ( AN_ATTACK, GLCHARLOGIC::m_emANISUBTYPE );
	const EMACTIONTYPE emAction = GetAction();
	//if ( !pAnicont )
	if ( !pAnicont || emAction == GLAT_FALLING || emAction == GLAT_DIE )
	{
		sMobReaction.SETPRESERVATION(false);
		return;
	}

	WORD wStrikeNum = pAnicont->m_pDxSkinAni->m_wStrikeCount;

	//	Note : 공격수행 가능 검사.
	//
	EMBEGINATTACK_FB emBeginFB = BEGIN_ATTACK(wStrikeNum);
	if ( emBeginFB==EMBEGINA_OK || emBeginFB==EMBEGINA_SP )
	{
		m_sActionID.emCrow = sTargetID.emCrow;
		m_sActionID.GaeaId = sTargetID.GaeaId;
		TurnAction ( GLAT_ATTACK );
	}

	PreservedReaction(sMobReaction);
	return;
}

// *****************************************************
// Desc: NPC에 관련된 예약된 액션을 처리한다.
// *****************************************************
void GLCharacter::NpcReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_NPC& sNPCReaction = InOutReaction;

	if ( m_bSTATE_STUN )	
		return;

	const STARGETID& sTargetID = sNPCReaction.GetTARID();
	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();

	// 필요없는 코드
	D3DXVECTOR3 vDistance = vPosition - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	// NPC와의 거리
	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition-vTarPos) );
	if ( isAction(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

    std::tr1::shared_ptr<GLCrowClient> spCrow = m_pGaeaClient->GetActiveMap()->GetCrow(sTargetID.GaeaId);
    if (!spCrow)
    {
        sNPCReaction.SETPRESERVATION(false);
        return;
    }

	// 대화가 가능한 범위
	float fTakeRange   = float(spCrow->GetBodyRadius() + GETBODYRADIUS()) + spCrow->GET_INTERACTION_RANGE();
	float fTakeAbleDis = fTakeRange + 2.0f;

	// 지금 위치가 대화가 불가능 하면
	if ( fDist>fTakeAbleDis )
	{
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sNPCReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			// 이동한 목적지가 대화가 불가능 하면 가능한 위치로 이동하는 예약된 액션을 등록
			if ( fMoveDist > fTakeAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo   = vTarPos - vPos*fTakeRange;
				vMoveTo.y = vTarPos.y;

				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sNPCReaction );
		return;
	}

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData(spCrow->m_sNativeID);
	if (!pCrowData)
	{
		sNPCReaction.SETPRESERVATION(false);
        return;
	}

    m_NpcLastClick = 0;

	if ( sNPCReaction.IsONLYTRACE() == false )
	{
		switch ( pCrowData->m_emNPCType )
		{
		default:
		case NPC_TYPE_NORMAL:
			{			
				ReqDialogueWindowOpen(pCrowData->sNativeID, sTargetID.GaeaId);
			}
			break;

		case NPC_TYPE_POSTBOX:
			{
				ReqPostBoxOpen( SNATIVEID(sTargetID.GaeaId) );
			}
			break;

		case NPC_TYPE_ODDEVEN:
			m_pGaeaClient->GetMiniGameManager()->RequestOpen(MiniGame::EMMINIGAME_ODDEVEN, pCrowData->sNativeID, sTargetID.GaeaId);
			break;

		case NPC_TYPE_CLICK_TRIGGER:
			{
				ReqClickTrigger( sTargetID );
			}
			break;
		case NPC_TYPE_TEXASHOLDEM:
			m_pGaeaClient->GetTexasHoldemManager()->OpenMainMenu( sTargetID.GaeaId );
			break;
		}

	}
	
	PreservedReaction(sNPCReaction);
}

/////// 소환수 NPC ///////////////////////////////////////////////////////////////////////////////////////////
void GLCharacter::SummonNpcReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_SUMMON_NPC& sNPCReaction = InOutReaction;

	if ( m_bSTATE_STUN )	
		return;

	const STARGETID& sTargetID = sNPCReaction.GetTARID();
	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();

	// 필요없는 코드
	D3DXVECTOR3 vDistance = vPosition - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	// NPC와의 거리
	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition-vTarPos) );
	if ( isAction(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

	std::tr1::shared_ptr<GLSummonClient> spCrow = m_pGaeaClient->GetActiveMap()->GetSummon(sTargetID.GaeaId);
	if (!spCrow)
	{
		sNPCReaction.SETPRESERVATION(false);
		return;
	}

	// 대화가 가능한 범위
	float fTakeRange   = float(spCrow->GetBodyRadius() + GETBODYRADIUS()) + spCrow->GET_INTERACTION_RANGE();
	float fTakeAbleDis = fTakeRange + 2.0f;

	// 지금 위치가 대화가 불가능 하면
	if ( fDist>fTakeAbleDis )
	{
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sNPCReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			// 이동한 목적지가 대화가 불가능 하면 가능한 위치로 이동하는 예약된 액션을 등록
			if ( fMoveDist > fTakeAbleDis )
			{
				//	Note : 이동지점 지정.
				//
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo   = vTarPos - vPos*fTakeRange;
				vMoveTo.y = vTarPos.y;

				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sNPCReaction );
		return;
	}
	
	const PCROWDATA pCrowData = spCrow->m_pSummonCrowData;//GLCrowDataMan::GetInstance().GetCrowData(spCrow->m_sNativeID);
	if (!pCrowData)
	{
		sNPCReaction.SETPRESERVATION(false);
		return;
	}

	m_NpcLastClick = 0;

	if ( sNPCReaction.IsONLYTRACE() == false )
	{
		switch ( pCrowData->m_emNPCType )
		{
		default:
		case NPC_TYPE_NORMAL:
			{
				if( pCrowData->m_emSummonType == SUMMON_TYPE_NPC )
				ReqDialogueWindowOpen(pCrowData->sNativeID, sTargetID.GaeaId, sTargetID.emCrow );
			}
			break;
		}

	}

	PreservedReaction(sNPCReaction);
}

// *****************************************************
// Desc: 채집NPC에 관련된 예약된 액션을 처리한다.
// *****************************************************
void GLCharacter::GatheringReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_GATHERING& sGatheringReaction = InOutReaction;

	if ( m_bSTATE_STUN )	
		return;

	if ( m_emVehicle )
    {
        if ( sGatheringReaction.ISPRINTFAILED() ) 
        {
            PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VEHICLE_CANNOT_GETHER_REACTION") ); 
        }

		sGatheringReaction.SETPRESERVATION(false);
        return;
    }

    // Note : 모든 NPC관련 창을 닫는다.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseAllNpc );

	// 선택된 NPC 초기화
	m_NpcLastClick = 0;

	const STARGETID& sTargetID = sGatheringReaction.GetTARID();
	const D3DXVECTOR3 &vTarPos = sTargetID.vPos;
	const D3DXVECTOR3& vPosition = GetPosition();
	// 필요없는 코드
	D3DXVECTOR3 vDistance = vPosition - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	// NPC와의 거리
	float fMoveDist = FLT_MAX;
	float fDist = D3DXVec3Length ( &D3DXVECTOR3(vPosition-vTarPos) );
	if ( isAction(GLAT_MOVE) )	fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTargetID );
	if ( !pTarget )
	{
		sGatheringReaction.SETPRESERVATION(false);
		return;
	}

	// 대화가 가능한 범위
	WORD wCollectionRange = pTarget->GetBodyRadius() + GETBODYRADIUS() + 2;
	WORD wCollectionAbleDis = wCollectionRange + 10;

	// 지금 위치가 대화가 불가능 하면
	if ( fDist>wCollectionAbleDis )
	{
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sGatheringReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			{
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo   = vTarPos - vPos*wCollectionRange;
				vMoveTo.y = vTarPos.y;

				sReaction.SetPOS( vMoveTo );
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sGatheringReaction );
		return;
	}

	// 만약 예약된 NPC가 있을경우 NPC의 위치 확인
    std::tr1::shared_ptr<GLMaterialClient> spMaterial = m_pGaeaClient->GetActiveMap()->GetMaterial ( sTargetID.GaeaId );
	if (!spMaterial)
		return;

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( spMaterial->m_sNativeID );
	if ( !pCrowData )
	{
		sGatheringReaction.SETPRESERVATION(false);
		return;
	}

	if ( pCrowData->m_emCrow != CROW_MATERIAL )
	{
		sGatheringReaction.SETPRESERVATION(false);
		return;
	}

	// 채집 시작
	if ( !sGatheringReaction.IsONLYTRACE() )
	{
		ReqGathering( sTargetID );
	}

	PreservedReaction(sGatheringReaction);
}

void GLCharacter::ItemReaction ( IN OUT GLREACTION& InOutReaction )
{
	GLREACTION_ITEM& sItemReaction = InOutReaction;

	if ( m_bSTATE_STUN )
	{
		return;
	}

    // Note : 모든 NPC관련 창을 닫는다.
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseAllNpc );

	const D3DXVECTOR3& vPosition = GetPosition();

	STARGETID sTargetID = sItemReaction.GetTARID();

	D3DXVECTOR3 vTarPos;
	if( true == sItemReaction.GetManualFind() )
	{
		sTargetID.vPos = sItemReaction.GetPOS();
		vTarPos = sTargetID.vPos;
	}
	else
	{
		vTarPos = sTargetID.vPos;
	}
	
	D3DXVECTOR3 vDistance = vPosition - vTarPos;
	float fDistance = D3DXVec3Length ( &vDistance );

	WORD wTarBodyRadius = 4;
	WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
	WORD wTakeAbleDis = wTakeRange + 12;

	if ( fDistance>wTakeAbleDis )
	{		
		GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

		//! 추적지 예약
		{
			GLREACTION_TRACE sReaction;
			sReaction.SetTARID( sItemReaction.GetTARID() );
			sReaction.SetNONMOBCOLL( true );

			{
				D3DXVECTOR3 vPos = vTarPos - vPosition;
				D3DXVec3Normalize ( &vPos, &vPos );

				D3DXVECTOR3 vMoveTo;
				vMoveTo = vTarPos - vPos*wTakeRange;
				sReaction.SetPOS(vMoveTo);
				iterReaction = m_ReservationAction.PriorityRegisterReaction( sReaction );
			}
		}

		//! 행위 재예약
		m_ReservationAction.ReRegisterReaction( iterReaction, sItemReaction );
		return;
	}

	//	줍기 시도.
	if (!sItemReaction.IsONLYTRACE())
        ReqFieldTo (sTargetID);

	PreservedReaction(sItemReaction);
}

void GLCharacter::UseBikeBoost()
{
	SITEM* pItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
	if( !pItem )
        return;

	if( pItem->VehicleType() == VEHICLE_TYPE_BOARD && m_emVehicle != EMVEHICLE_OFF )
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_BIKE") );
		return;
	}

	if( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_DRIVER") );
		return;
	}
	
	if( m_emVehicle == EMVEHICLE_OFF )
	{
		return;
	}

	if( !m_sVehicle.m_dwBoosterID ) 
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_LEARN_BOOSTER") );
		return;
	}

	if( m_sVehicle.m_bUseBoost ) 
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_USING") );
		return;
	}

	if( m_sVehicle.m_bUseBoostDelay )
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_DELAY") );
		return;
	}

	if( m_sVehicle.m_fBoostDelay > 0.0f )
	{
		PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BOOST_NOT_USING_DELAY") );
		return;
	}

	GLMSG::SNETPC_USE_BIKE_BOOST NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLCharacter::PreservedReaction( IN const GLREACTION& InReaction )
{
	if ( InReaction.ISREGISTATION() )
	{
		return;
	}

	if ( !InReaction.ISPRESERVATION() )
	{
		return;
	}

	//! 지속 가능 체크
	if ( !m_ReservationAction.IsAllowedPreservation(InReaction) )
	{	
		return;
	}
	m_ReservationAction.PriorityRegisterReaction( InReaction );
}

void GLCharacter::DoReaction( float fTime, IN OUT GLREACTION& sReaction )
{
	if ( sReaction.ISNULL() || (m_bLockMoveMent == true) )
	{
		return;
	}

	//! 피행위자가 정확한지 검사.
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	STARGETID& sTARID = sReaction.GETTARID();

	switch ( sReaction.GETTYPE() )
	{
	case REACT_MOB:
	case REACT_NPC:
	case REACT_P2P:
	case REACT_PVP:
	case REACT_GATHERING:
	case REACT_SUMMON_NPC:
		{
			ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( sTARID );
			if ( !pTarget )
			{
				sReaction.SETPRESERVATION(false);
				return;
			}

			//! 피행위자가 사망시 액션 불가능
			if ( pTarget->isAction(GLAT_FALLING, GLAT_DIE) )
			{
				sReaction.SETPRESERVATION(false);
				return;
			}
		}
		break;

	case REACT_ITEM:
		switch ( sTARID.emCrow )
		{
		case CROW_ITEM:
			if ( !pLand->GetItem(sTARID.GaeaId) )
			{
				sReaction.SETPRESERVATION(false);
				return;
			}
			break;

		case CROW_MONEY:
			if ( !pLand->GetMoney(sTARID.GaeaId) )
			{
				sReaction.SETPRESERVATION(false);
				return;
			}
			break;

		default:
			sReaction.SETPRESERVATION(false);
			return;
		};
		break;
	};

	//!	피행위자의 현제 위치 업데이트
	if ( !sTARID.ISNULL() )
	{
		sTARID.vPos = m_pGaeaClient->GetTargetPos ( sTARID );
	}

	BOOL bMove(FALSE);
	D3DXVECTOR3 vMoveTo(0,0,0);
	bool bCompleted = false;

	switch ( sReaction.GETTYPE() )
	{
	case REACT_TRACE:
		{
			GLREACTION_TRACE& sTrace = sReaction;
			ActionMoveTo(fTime, sTrace.GetPOS()+D3DXVECTOR3(0,+20,0), sTrace.GetPOS()+D3DXVECTOR3(0,-20,0), FALSE, TRUE, 
				sTrace.GetNONMOBCOLL() || IsActState(EM_ACT_MACROMODE) ? FALSE : TRUE );
		}
		break;

	case REACT_P2P:
		{
			P2PReaction(sReaction);										
		}
		break;

	case REACT_PVP:
		{
			PvPReaction(sReaction);
		}		
		break;

	case REACT_MOB:
		{
			MobReaction(sReaction);									
		}
		break;
	case REACT_SUMMON_NPC:
		{
			SummonNpcReaction(sReaction);
		}
		break;
	case REACT_NPC:
		{
			NpcReaction(sReaction);	
		}								
		break;

	case REACT_ITEM:
		{
			ItemReaction(sReaction);													
		}	
		break;

	case REACT_SKILL:
		{
			SkillReaction(sReaction);	
		}	
		break;

	case REACT_GATHERING:
		{
			GatheringReaction(sReaction);												
		}	
		break;

	case REACT_TOGGLE_VEHICLE:
		{
			GLREACTION_TOGGLE_VEHICLE& sToggleVehicle = sReaction;

			sToggleVehicle.SETPRESERVATION(true);

			//! 요청 하지않았다면 요청을 한다.
			if ( !sToggleVehicle.IsREQ() )
			{
				switch( VehicleState() )
				{
				case EMVEHICLE_OFF: 
					{
						//! 탈것에 탑승중이 아닌데 내리기를 요청했다면
						sReaction.SETPRESERVATION( sToggleVehicle.IsACTIVE() );
					}
					break;

				case EMVEHICLE_DRIVER:
				case EMVEHICLE_PASSENGER:
					{
						//! 탈것에 탑승중인데 타기를 요청했다면
						sReaction.SETPRESERVATION( !sToggleVehicle.IsACTIVE() );
					}
					break;
				}

				if ( S_OK == ReqSetVehicle( sToggleVehicle.IsACTIVE() ) )
				{
					sToggleVehicle.SetREQ();
				}
				else
				{
					sToggleVehicle.SETPRESERVATION(false);
				}

				PreservedReaction(sToggleVehicle);
			}
		}
		break;

	case REACT_SWAP_ARMS:
		{
			GLREACTION_SWAP_ARMS& sSwapArms = sReaction;

			sSwapArms.SETPRESERVATION(true);

			//! 요청을 보내지 않았다면
			if ( !IsReqSwapArms() && !sSwapArms.IsREQ() )
			{
				//! 극강부가 아니라면
				if( m_emClass != GLCC_EXTREME_M && m_emClass != GLCC_EXTREME_W ) 
				{
					sSwapArms.SETPRESERVATION(false);
					break;
				}

				//! 스왑할 무기가 없다면
				EMSLOT emRHand_S;
				if ( IsUseArmSub() )
				{
					emRHand_S = SLOT_RHAND;
				}
				else
				{
					emRHand_S = SLOT_RHAND_S;
				}

				SITEM* pItem_S = GET_SLOT_ITEMDATA(emRHand_S);

				if ( !pItem_S )
				{
					sSwapArms.SETPRESERVATION(false);
					break;
				}

				//! 요청을 보낸다
				if ( S_OK == m_pGaeaClient->GetCharacter()->ReqSlotChange() )
				{
					sSwapArms.SetREQ();
				}
				else
				{
					sSwapArms.SETPRESERVATION(false);
				}

				PreservedReaction(sSwapArms);
			}
		}
		break;
	};
}

void GLCharacter::ReservedActionUpdate ( float fTime )
{
	if ( !IsValidBody() )
	{
		m_ReservationAction.Clear();
		return;
	}

	if ( m_bSTATE_STUN )
	{
		m_ReservationAction.Clear();
		return;
	}

	if ( !m_pGaeaClient->GetActiveMap() )
		return;	

	GLREACTION& sReaction = m_ReservationAction.Front();
	if ( sReaction.ISNULL() )
		return;

	GLREACTION_ITER iterReaction = m_ReservationAction.FrontIter();

	DoReaction( fTime, sReaction );

	//! 리액션후 삭제여부를 결정
// 	if ( sReaction.GETTYPE() == REACT_SKILL )
// 	{
// 		m_ReservationAction.ReRegisterReaction()
// 	}
	if ( !sReaction.ISPRESERVATION() )
	{
		m_ReservationAction.EraseReaction( iterReaction );
	}
}

GLREACTION_ITER GLCharacter::BeginReservedSkillReaction()
{
	return m_ReservationAction.Find_First_Of_Reaction(REACT_SKILL);
}

GLREACTION_ITER GLCharacter::NextReservedSkillReaction( GLREACTION_ITER iterWhere )
{
	if ( iterWhere != m_ReservationAction.EndIter() )
	{
		return m_ReservationAction.Find_First_Of_Reaction(++iterWhere, REACT_SKILL);
	}

	return m_ReservationAction.EndIter();
}

GLREACTION_ITER GLCharacter::EndReactionIter()
{
	return m_ReservationAction.EndIter();
}

STARGETID* GLCharacter::GetCONFT_TAR ( VEC_SK_ITER_RANGE &pair )
{
	if ( !m_sCONFTING.IsFIGHTING() )	return NULL;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		{
			//	Note : 대련자가 걸렸을 경우 이를 우선시.
			STARGETID sCONFT_TARID(CROW_PC,m_sCONFTING.dwTAR_ID);
			STARGETID *pTARID = FindCrow ( pair, sCONFT_TARID );
			if ( pTARID )	return pTARID;
		}
		break;

	case EMCONFT_PARTY:
		{
			STARGETID *pTARID = FindCrow(pair, m_sCONFTING.setConftMember, m_pGaeaClient);
			if (pTARID)
                return pTARID;
		}
		break;

	case EMCONFT_GUILD:
		{
			STARGETID *pTARID = FindCrowCID(pair, m_sCONFTING.setConftMember, m_pGaeaClient);
			if (pTARID)
                return pTARID;
		}
		break;
	};

	return NULL;
}

STARGETID* GLCharacter::GetCONFT_SUMMON ( VEC_SK_ITER_RANGE &pair )
{
	if ( !m_sCONFTING.IsFIGHTING() )	return NULL;

	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		{
			//	Note : 소환수의 주인을 추적한후 대련자가 걸렸을 경우 이를 우선시.
			STARGETID sCONFT_OWNERID(CROW_PC,m_sCONFTING.dwTAR_ID);
			STARGETID* pTARID = FindSummonOwner(pair, sCONFT_OWNERID, m_pGaeaClient);
			if (pTARID)
                return pTARID;
		}
		break;

	case EMCONFT_PARTY:
		{
			STARGETID *pTARID = FindCrow(pair, m_sCONFTING.setConftMember, m_pGaeaClient);
			if (pTARID)
                return pTARID;
		}
		break;

	case EMCONFT_GUILD:
		{
			STARGETID *pTARID = FindCrowCID(pair, m_sCONFTING.setConftMember, m_pGaeaClient);
			if (pTARID)
                return pTARID;
		}
		break;
	};

	return NULL;
}

bool GLCharacter::IsClubBattle_TAR ( std::tr1::shared_ptr<GLCharClient> pCHAR )
{
    if (!pCHAR)
        return false;

	//	Note : PK 의 경우.
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if (!pLAND)
        return false;

	// 클럽 배틀중일경우
	if ((m_pMyClub->IsBattle(pCHAR->GETCLUBID()) || m_pMyClub->IsBattleAlliance(pCHAR->GETALLIANCEID()))
		&& pLAND->IsClubBattleZone() )
		return true;
    else
	    return false;
}

int GLCharacter::GetClubColorIndex(std::tr1::shared_ptr<GLCharClient> pCHAR )
{
	int nIndex = 0;
	
	if (!pCHAR)
        return nIndex;

	CLUB_BATTLE_ITER pos = m_pMyClub->m_mapBattle.begin();
	CLUB_BATTLE_ITER pos_end = m_pMyClub->m_mapBattle.end();
	for ( ; pos != pos_end; ++pos)
	{
		if (pCHAR->GETCLUBID() == (*pos).first)
            break;
		if (pCHAR->GETALLIANCEID() == (*pos).first)
            break;
		nIndex++;
	}

	nIndex = nIndex % NS_UITEXTCOLOR::MAX_CLUB_BATTLE_COLOR;

	return nIndex;
}

SACTIONTAR GLCharacter::FindActionTarget ( const D3DXVECTOR3 &vTargetPt, const D3DXVECTOR3 &vFromPt, const DWORD dwFindOption, const EMFIND_TAR emFINDTAR )
{
	SACTIONTAR sACTAR;
	if ( IsActState(EM_ACT_CONTINUEMOVE) )		return sACTAR;

	////	Note : 사용자 입력값.
	////
	//DxInputDevice &dxInputDev = DxInputDevice::GetInstance();
	
	//DWORD dwKeyR = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_RUN] );
	//DWORD dwKeyLAlt = dxInputDev.GetKeyState ( DIK_LMENU );
	//DWORD dwKeyLCtrl = dxInputDev.GetKeyState ( DIK_LCONTROL );
	//DWORD dwKeyLShift = dxInputDev.GetKeyState ( DIK_LSHIFT );
	//DWORD dwKeySPACE = dxInputDev.GetKeyState ( DIK_SPACE );
	//DWORD dwKeyZ = dxInputDev.GetKeyState ( RANPARAM::MenuShotcut[SHOTCUT_PKMODE] );

	bool bONLY_ITEM = (dwFindOption&EMFIND_OPTION_ONLYITEM) ? true : false;
	bool bODER_P2P  = (dwFindOption&EMFIND_OPTION_P2PMENU) ? true : false;

	bool bALL(false), bANY(false), bENEMY(false), bOUR(false), bDIE(false);
	switch ( emFINDTAR )
	{
	case ENFIND_TAR_NULL:
		bALL = true;
		bDIE = true;
		bENEMY = true;
		bANY = true;
		bOUR = true;
		break;
	case EMFIND_TAR_ENEMY:
		bENEMY = true;
		break;
	case EMFIND_TAR_ANY:
		bANY = true;
		break;
	case EMFIND_TAR_OUR:
		bOUR = true;
		break;

	case EMFIND_TAR_ANY_DIE:
		bANY = true;
		bDIE = true;
		break;
	case EMFIND_TAR_OUR_DIE:
		bOUR = true;
		bDIE = true;
		break;

	case ENFIND_TAR_LAND:
		sACTAR.emACTAR = EMACTAR_NULL;
		sACTAR.sTARID  = STARGETID(CROW_MOB, EMTARGET_NULL, D3DXVECTOR3(0.0f,0.0f,0.0f));
		return sACTAR;
	};

	STARGETID sTID_FOUND;
	VEC_SK_ITER_RANGE pairRange;

	//	Crow 검색 ( 단, 연속 이동일 때는 검색하지 않는다. )
	VEC_SK_TAR *pDetectMap = NULL;

	DWORD emCrow = ( CROW_EX_PC | CROW_EX_NPC | CROW_EX_MOB | CROW_EX_ITEM | 
					 CROW_EX_MONEY | CROW_EX_SUMMON | CROW_EX_MATERIAL );

	if ( emFINDTAR != ENFIND_TAR_NULL && m_pGaeaClient->IsCTFPlayer() )
		pDetectMap = m_pGaeaClient->DetectReactionableCrow ( emCrow, vFromPt, vTargetPt, bENEMY );
	else
		pDetectMap = m_pGaeaClient->DetectCrow ( emCrow, vFromPt, vTargetPt );

	const D3DXVECTOR3& vPosition = GetPosition();
	if ( !pDetectMap )
	{
		//	Note : 스킬의 '모두에게' 혹은 '우리편에게' 속성일 경우 선택된 케릭이 없을때,
		//			자신이 선택되는지 점검.

		//	ENFIND_TAR_OUR
		//	오타인것 같은데 일단 이렇게 사용하기로 한다.
		//	타겟 등록하는 부분도 있다..

		// 2012.8.14 추가;
		// EMFIND_TAR_OUR가 EMFIND_TAR_ENEMY로 되있었는데, 
		// 적을 대상으로 하는 스킬을 나한테 시전 할수 없는데 왜 ENEMY로 되있었는지??
//		if ( emFINDTAR==EMFIND_TAR_ANY || emFINDTAR==EMFIND_TAR_OUR )
		if( emFINDTAR == EMFIND_TAR_ANY || emFINDTAR == EMFIND_TAR_OUR )
		{
			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, m_vMax, m_vMin );
			if( bCol )
			{
				//	우리편이면 OUR 아니면 ANY
				EMACTIONTAR		emACTAR = EMACTAR_PC_ANY;
				if( bOUR )	
					emACTAR = EMACTAR_PC_OUR;

				sACTAR.emACTAR	= emACTAR;
				sACTAR.sTARID	= STARGETID(CROW_PC,m_dwGaeaID, vPosition);
			} //if
		} //if

		return sACTAR;
	}

	//	Note : ALT 키를 누른 상태에서는 검출 제외 ==> 아이템, 돈에 우선권을 줌.
	//
	if ( !bONLY_ITEM )
	{
		//	PVP.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			// P2P 용도.
			sACTAR.sTARID.RESET();
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			
			if ( bODER_P2P )
			{
				sACTAR.emACTAR = EMACTAR_PC_P2P;
				sACTAR.sTARID = sTID_FOUND;
			}
			else if ( bENEMY )
			{
				STARGETID *pPVP(NULL);

				//	Note : 대련자가 있는지 검사.	( 대련자 우선 )
				//
				pPVP = GetCONFT_TAR(pairRange);
				if ( pPVP )
				{
					sACTAR.emACTAR = EMACTAR_PC_PVP;
					sACTAR.sTARID = *pPVP;
				}
				else
				{
					STARGETID &sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
					std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( sTID_FOUND.GaeaId );
					if (pCHAR)
					{
						// 강제 공격키를 누르지 않아도 되는 경우( 성향 이벤트 중일경우 )
						bool bFORCED_PK = m_bForcedPKMode;
						bFORCED_PK = true == bFORCED_PK && false == GfxInterfaceBase::GetInstance()->GetFocusInTextInput();

						if ( IsReActionable(sTID_FOUND, EMIMPACT_SIDE_ENEMY, bFORCED_PK) )
						{
							sACTAR.emACTAR = EMACTAR_PC_PVP;
							sACTAR.sTARID = sTID_FOUND;
						}
					}
				} //if..else
			}

			if( !sACTAR.sTARID.ISNULL() )
				return sACTAR;
// 			{
// 				STARGETID	selectId = GetSelectForcedTargetAround();
// 				if( !selectId.ISNULL() )		// if there is a forced target.
// 					sACTAR.sTARID = selectId;
// 				return sACTAR;
// 			} //if
		} //if

		//	SUMMON
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_SUMMON, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			bool bNonTarget = false;
			//	Note : P2P 용도.
			//
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTID_FOUND.GaeaId);
			if ( spSummon )
			{
				bNonTarget = spSummon->IsNonTargetable();
					
			}

			if ( !bNonTarget )
			{
				if ( bALL )
				{
					sACTAR.emACTAR = EMACTAR_PC_ANY;
					sACTAR.sTARID  = sTID_FOUND;

					if( spSummon->GetSummonType() == SUMMON_TYPE_NPC )
					{
						sACTAR.emACTAR = EMACTAR_SUMMON_NPC;
						return sACTAR;
					}
				}
				else if ( bENEMY )
				{
					STARGETID *pPVP(NULL);

					//	Note : 대련자가 있는지 검사.( 대련자 우선 )
					//
					pPVP = GetCONFT_SUMMON(pairRange);
					if ( pPVP )
					{
						sACTAR.emACTAR = EMACTAR_SUMMON_ATTACK;
						sACTAR.sTARID = *pPVP;
						return sACTAR;
					}

					//STARGETID& sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
					//std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTID_FOUND.GaeaId);
					if (spSummon)
					{
						std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar( spSummon->m_dwOwnerGaeaID );
						if (pCHAR)
						{
							// 강제 공격키를 누르지 않아도 되는 경우( 성향 이벤트 중일경우 )
							bool bFORCED_PK = m_bForcedPKMode;
							bFORCED_PK = true == bFORCED_PK && false == GfxInterfaceBase::GetInstance()->GetFocusInTextInput();
							if (IsReActionable(sTID_FOUND, EMIMPACT_SIDE_ENEMY, bFORCED_PK))
							{
								sACTAR.emACTAR = EMACTAR_SUMMON_ATTACK;
								sACTAR.sTARID  = sTID_FOUND;
								return sACTAR;
							}
						}
					}
				}
			}
		}	

		if ( bENEMY )
		{
			//	MOB.
			pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
			if ( pairRange.first != pairRange.second )
			{
				STARGETID *pTARID = FindCrow ( pairRange, m_sOLD_TARMOB );
				if ( pTARID )
				{
					/*
					//	Note : 공격 받지 않음 옵션이 체크되어 있는지 검사한다.
					//

					if ( pTarget->IsNonDamaged() ) return SACTIONTAR();

					*/

					sACTAR.emACTAR = EMACTAR_MOB;
					sACTAR.sTARID = *pTARID;
					return sACTAR;
				}

				sACTAR.emACTAR = EMACTAR_MOB;
				sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );


				/*
				//	Note : 공격 받지 않음 옵션이 체크되어 있는지 검사한다.
				//


				if ( pTarget->IsNonDamaged() ) return SACTIONTAR();
				}
				*/

				return sACTAR;
			}
		}


		//	NPC.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_NPC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sACTAR.emACTAR = EMACTAR_NPC;
			sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			return sACTAR;
		}

		//	METERIAL
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MATERIAL, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sACTAR.emACTAR = EMACTAR_MATERIAL;
			sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
			return sACTAR;
		}

		//	PC, OUR, ANY.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );			
			const bool bParty = m_pGaeaClient->GetMyPartyClient()->isMember(sTID_FOUND.GaeaId);

			//	우리편이면 OUR 아니면 ANY
			if ( bParty && bOUR )
			{
				sACTAR.emACTAR = EMACTAR_PC_OUR;
				sACTAR.sTARID = sTID_FOUND;
				return sACTAR;
			}
			else
			{
				sACTAR.emACTAR	= EMACTAR_PC_ANY;
				sACTAR.sTARID	= sTID_FOUND;
				bool bFORCED_PK = m_bForcedPKMode;
				bFORCED_PK = true == bFORCED_PK && false == GfxInterfaceBase::GetInstance()->GetFocusInTextInput();

				std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( sTID_FOUND.GaeaId );
				if( IsReActionable(sTID_FOUND, EMIMPACT_SIDE_ENEMY, bFORCED_PK) )
				{
					STARGETID	selectId = GetSelectForcedTargetAround();
					if( !selectId.ISNULL() )		// if there is a forced target.
						sACTAR.sTARID = selectId;
				} //if

				return sACTAR;
			}
		}

		//	Note : 스킬의 '모두에게' 혹은 '우리편에게' 속성일 경우 선택된 케릭이 없을때,
		//			자신이 선택되는지 점검.
		if ( emFINDTAR==EMFIND_TAR_ANY || emFINDTAR==EMFIND_TAR_ENEMY )
		{
			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, m_vMax, m_vMin );
			if ( bCol )
			{
				//	우리편이면 OUR 아니면 ANY
				EMACTIONTAR		emACTAR = EMACTAR_PC_ANY;
				if ( bOUR )
					emACTAR = EMACTAR_PC_OUR;

				sACTAR.emACTAR = emACTAR;
				sACTAR.sTARID = STARGETID(CROW_PC,m_dwGaeaID, vPosition);
				return sACTAR;
			}
		}
	}

	//	ITEM.
	pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
	if ( pairRange.first != pairRange.second )
	{
		sACTAR.emACTAR = EMACTAR_ITEM;
		sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
		return sACTAR;
	}

	//	MONEY.
	pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
	if ( pairRange.first != pairRange.second )
	{
		sACTAR.emACTAR = EMACTAR_ITEM;
		sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );
		return sACTAR;
	}

	if ( bDIE )
	{
		if (  emFINDTAR != ENFIND_TAR_NULL && m_pGaeaClient->IsCTFPlayer() )
		{
			pDetectMap = m_pGaeaClient->DetectReactionableCrowDie ( vFromPt, vTargetPt, bENEMY );
		}
		else
		{
			pDetectMap = m_pGaeaClient->DetectCrowDie ( vFromPt, vTargetPt );
		}

		if ( pDetectMap )
		{
			//	Note : ALT 키를 누른 상태에서는 검출 제외 ==> 아이템, 돈에 우선권을 줌.
			//
			if ( !bONLY_ITEM )
			{
				if ( bENEMY )
				{
					//	MOB.
					pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MOB, STAR_ORDER() );
					if ( pairRange.first != pairRange.second )
					{
						sACTAR.emACTAR = EMACTAR_MOB;
						sACTAR.sTARID = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );

						return sACTAR;
					}
				}
				else
				{
					//	PC ANY.
					pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_PC, STAR_ORDER() );
					if ( pairRange.first != pairRange.second )
					{
						sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vFromPt );

						sACTAR.emACTAR = EMACTAR_PC_ANY;
						sACTAR.sTARID = sTID_FOUND;
						return sACTAR;
					}
				}
			}
		}

		return sACTAR;
	}


	return sACTAR;
}

SACTIONTAR GLCharacter::FindNearItem ()
{
	SACTIONTAR sACTAR_ITEM;
	SACTIONTAR sACTAR_MONEY;

	//	Crow 검색 ( 단, 연속 이동일 때는 검색하지 않는다. )
	VEC_SK_TAR *pDetectMap = NULL;
	VEC_SK_ITER_RANGE pairRange;
	STARGETID sTID_FOUND;

	DWORD emCrow = ( CROW_EX_ITEM | CROW_EX_MONEY );
	const D3DXVECTOR3& vPosition = GetPosition();	
	pDetectMap = m_pGaeaClient->DetectCrow ( emCrow, vPosition, GETVISIBLESIGHT() * 0.3333333333f );
	if ( pDetectMap )
	{
		//	Item.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_ITEM, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vPosition );

			sACTAR_ITEM.emACTAR = EMACTAR_ITEM;
			sACTAR_ITEM.sTARID = sTID_FOUND;
		}

		//	Money.
		pairRange = std::equal_range ( pDetectMap->begin(), pDetectMap->end(), CROW_MONEY, STAR_ORDER() );
		if ( pairRange.first != pairRange.second )
		{
			sTID_FOUND = CFINDER::GetInstance().FindClosedCrow ( pairRange, vPosition );

			sACTAR_MONEY.emACTAR = EMACTAR_ITEM;
			sACTAR_MONEY.sTARID = sTID_FOUND;
		}
	}

	if ( sACTAR_MONEY.emACTAR!=EMACTAR_NULL && sACTAR_ITEM.emACTAR!=EMACTAR_NULL )
	{
		std_afunc::CCompareTargetDist sDist(vPosition);

		bool bMore = sDist.operator () ( sACTAR_MONEY.sTARID, sACTAR_ITEM.sTARID );

		if ( bMore )	return sACTAR_MONEY;
		else			return sACTAR_ITEM;
	}
	else if ( sACTAR_MONEY.emACTAR!=EMACTAR_NULL )
	{
		return sACTAR_MONEY;
	}
	else if ( sACTAR_ITEM.emACTAR!=EMACTAR_NULL )
	{
		return sACTAR_ITEM;
	}

	SACTIONTAR sACTAR;
	return sACTAR;
}

const RebuyItem::REBUY_VEC& GLCharacter::RebuyList() const
{
    return m_vecRebuy;
}

unsigned int GLCharacter::GetCountItemInInven( EMITEM_TYPE emType  )
{
    return static_cast<unsigned int>( m_cInventory.CountTurnItem( emType ) );
}
unsigned int GLCharacter::GetCountItemInInven( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value )
{
	return static_cast<unsigned int>( m_cInventory.CountTurnItem( emType, typeValue, _value ) );
}

void GLCharacter::GetItemPosInInven( EMITEM_TYPE emType, WORD& wOutPosX, WORD& wOutPosY )
{
    SINVENITEM* pInvenItem = m_cInventory.FindItem( emType );

    if ( pInvenItem )
    {
        wOutPosX = pInvenItem->wPosX;
        wOutPosY = pInvenItem->wPosY;
    }
    else
    {
        wOutPosX = USHRT_MAX;
        wOutPosY = USHRT_MAX;
    }
}
void GLCharacter::GetItemPosInInven( EMITEM_TYPE emType, ITEM_VALUE typeValue, int _value, WORD& wOutPosX, WORD& wOutPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindItem( emType, typeValue, _value );

	if ( pInvenItem )
	{
		wOutPosX = pInvenItem->wPosX;
		wOutPosY = pInvenItem->wPosY;
	}
	else
	{
		wOutPosX = USHRT_MAX;
		wOutPosY = USHRT_MAX;
	}
}

LuaTable GLCharacter::GetItemPosInInvenByTable( DWORD dwType )
{
	EMITEM_TYPE emType = (EMITEM_TYPE)dwType;
	SINVENITEM* pInvenItem = m_cInventory.FindItem( emType );

	LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	if( NULL == pInvenItem )
	{
		tbItem.set( 1, false );
		tbItem.set( 2, USHRT_MAX );
		tbItem.set( 3, USHRT_MAX );
	}
	else
	{
		tbItem.set( 1, true );
		tbItem.set( 2, pInvenItem->wPosX );
		tbItem.set( 3, pInvenItem->wPosY );
	}

	return tbItem;
}

LuaTable GLCharacter::FindInvenItemPos( WORD wMID, WORD wSID, int nCountIndex )
{
	LuaTable tbItemIndex( GLWidgetScript::GetInstance().GetLuaState() );

	SINVENITEM* pIvenItem = m_cInventory.FindItem( SNATIVEID( wMID, wSID ), nCountIndex );
	if( NULL == pIvenItem )
		return tbItemIndex;

	tbItemIndex.set( 1, pIvenItem->wPosX );
	tbItemIndex.set( 2, pIvenItem->wPosY );

	return tbItemIndex;
}

bool GLCharacter::IsExistReturnSchoolCard(void)
{
	const SINVENITEM* const pInvenItem = m_cInventory.FindDrugItem(ITEM_DRUG_CALL_SCHOOL);
	return (pInvenItem == NULL ? false : true);
}

void GLCharacter::SetMotion( EMANI_MAINTYPE eMID, EMANI_SUBTYPE eSID, FLOAT fSec )
{
	TurnAction( GLAT_MOTION );

	m_eMotionMID = eMID;
	m_eMotionSID = eSID;
	m_fMotionSec = fSec;
}

void GLCharacter::PostEnterMap()
{
    if (m_pGaeaClient == 0)
    {
        return;
    }

    m_pGaeaClient->ReqMapTable();
}

void GLCharacter::SetAction( const EMACTIONTYPE emAction )
{
	if (m_pGaeaClient)
	{
		TriggerSystem::SClientActionMessage msg;
		msg.dwCharDbNum = GetCharID();
		msg.ePreAction = GetAction();
		msg.eNextAction = emAction;
		m_pGaeaClient->SendTriggerMessage(&msg);
	}

	ClientActor::SetAction(emAction);
}

void GLCharacter::DoPrivateMarketClose()
{
	m_sPMarket.DoMarketClose();

	// 마켓이 종료되면 시야를 정상으로 되돌린다.
	DxFogMan::GetInstance().SetFogRange ( static_cast<FOGRANGE>( DXPARAMSET::GetInstance().m_dwFogRange ) );

	// 개인상점 디스플레이 삭제
	GfxInterfaceBase::GetInstance()->DeletePrivateMarketDisplay( m_dwGaeaID );
}

void GLCharacter::GenerateSkillList()
{
	for( WORD i = 0; i < SKILL_TAB_COUNT; ++i )
	{
		int nBegin = 0;
		int nEnd = 0;
		GetSkillIndexRange( nBegin, nEnd );

		int nSelectPage = nBegin + i;
		if( nSelectPage > nEnd )
			continue;

		WORD wClass = 0;
		WORD wIndex = 0;
		GLSkillMan::GetInstance().GetMaxSkill( wClass, wIndex );

		for( WORD j = 0; j < wIndex; ++j )
		{
			GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( nSelectPage, j );
			if( NULL != pSkill )
			{
				SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );
				if( true == pSkill->m_sBASIC.bLearnView && NULL == pCharSkill )
					continue;

				m_vecSkill[ i ].push_back( pSkill );
			}
		}

		if( true == m_vecSkill[ i ].empty() )
			continue;

		class CCompareSkill
		{
		public :
			bool operator()( const GLSKILL* pLValue, const GLSKILL* pRValue )
			{
				if( pLValue->m_sBASIC.dwGRADE < pRValue->m_sBASIC.dwGRADE )
				{
					return true;
				}
				else if( pLValue->m_sBASIC.dwGRADE == pRValue->m_sBASIC.dwGRADE )
				{
					if( pLValue->m_sBASIC.sNATIVEID.wSubID == pRValue->m_sBASIC.sNATIVEID.wSubID )
						return true;
				}

				return false;
			}
		};

		std::sort( m_vecSkill[ i ].begin(), m_vecSkill[ i ].end(), CCompareSkill() );
	}
}

LuaTable GLCharacter::GetTitle()
{
	std::vector< TITLEINFO > vecTitle;

	int nTitleCount = 1;
	LuaTable tbTitle( GLWidgetScript::GetInstance().GetLuaState() );

	{
		//  뺏지없음
		TITLEINFO sTitle;
		sTitle.nIndex = TITLE_INDEX_NONE;
		sTitle.nClass = ACTIVITY_CLASS_SIZE;
		sTitle.strTitle = "CAHRWINDOW_TITLE";
		vecTitle.push_back( sTitle );
	}

	MAPTITLE& mapTitle = m_TitleManager.GetData();

	ITER_MAPTITLE iter = mapTitle.begin();
	for ( ; iter != mapTitle.end(); ++iter )
	{
		TITLEINFO& title = iter->second;
		vecTitle.push_back( title );
	}

	std::sort( vecTitle.begin(), vecTitle.end() );

	for( size_t i = 0; i < vecTitle.size(); ++i ) 
	{
		const TITLEINFO& title = vecTitle[ i ];

		if( title.bSelected )
			m_nTitleSelectIndex = static_cast< int >( i );

		LuaTable tbTitleObj( GLWidgetScript::GetInstance().GetLuaState() );
		tbTitleObj.set( 1, title.strTitle.c_str() );
		tbTitleObj.set( 2, NS_UITEXTCONTROL::GetActivityColor( title.nClass ) );
		//  [5/13/2015 gbgim];
		// 뱃지를 선택안할 시 -1값이 기입이되는데 여기서 unsigned로 되어있다;
		// 뱃지 변경시 로직에서 해당 값을 signed-1로 처리해주는 구간도있기때문에;
		// 스케일폼 전달시 부호를 붙여서 전달하자;
		int nSignIndex = static_cast<int>(title.nIndex);
		tbTitleObj.set( 3, nSignIndex );

		tbTitle.set( nTitleCount++, tbTitleObj );
	}

	return tbTitle;
}

int GLCharacter::GetSumPowerAttribute( int nType )
{
	if( SKILL::EMAPPLY_NSIZE <= nType )
		return 0;

	return m_sumPowerAttribute[ nType ];
}

LuaTable GLCharacter::GetChangeUpAbillity( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta )
{
	LuaTable tbStat( GLWidgetScript::GetInstance().GetLuaState() );

	GLCHARLOGIC* pCharOrigData = &GetCharLogic();
	GLCHARLOGIC* pVirtualCharData = new GLCHARLOGIC;
	if ( pVirtualCharData )
	{
		SAFE_DELETE( pVirtualCharData );
		return tbStat;
	}

	pVirtualCharData->Assign( *pCharOrigData );
	//*pVirtualCharData = *pCharOrigData;

	pVirtualCharData->m_sStats.wPow += wPow;
	pVirtualCharData->m_sStats.wDex += wDex;
	pVirtualCharData->m_sStats.wSpi += wSpi;
	pVirtualCharData->m_sStats.wStr += wStr;
	pVirtualCharData->m_sStats.wSta += wSta;

	pVirtualCharData->SET_VALIDATION_EXPSKILL_MAP();
	pVirtualCharData->INIT_DATA( FALSE, FALSE );
	pVirtualCharData->UPDATE_DATA( 1.0f, 1.0f );
	
	tbStat.set( 1, pVirtualCharData->GETFORCE_LOW() - pCharOrigData->GETFORCE_LOW() );
	tbStat.set( 2, pVirtualCharData->GETFORCE_HIGH() - pCharOrigData->GETFORCE_HIGH() );
	tbStat.set( 3, pVirtualCharData->GETDEFENSE() - pCharOrigData->GETDEFENSE() );
	tbStat.set( 4, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_MELEE ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_MELEE ] );
	tbStat.set( 5, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_RANGE ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_RANGE ] );
	tbStat.set( 6, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_MAGIC ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_MAGIC ] );
	tbStat.set( 7, pVirtualCharData->GETHIT() - pCharOrigData->GETHIT() );
	tbStat.set( 8, pVirtualCharData->GETAVOID() - pCharOrigData->GETAVOID() );
	tbStat.set( 9, pVirtualCharData->GETMAXHP() - pCharOrigData->GETMAXHP() );
	tbStat.set( 10, pVirtualCharData->GETMAXMP() - pCharOrigData->GETMAXMP() );
	tbStat.set( 11, pVirtualCharData->GETMAXSP() - pCharOrigData->GETMAXSP() );

	SAFE_DELETE( pVirtualCharData );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

	return tbStat;	
}

void GLCharacter::ApplyStatPoint( WORD wPow, WORD wDex, WORD wSpi, WORD wStr, WORD wSta )
{
	WORD wTotal = wPow + wDex + wSpi + wStr + wSta;
	if( m_wStatsPoint < wTotal )
		return;

	WORD emSTATS[ EMSIZE ] = { 0, };
	emSTATS[ EMPOW ] = wPow;
	emSTATS[ EMDEX ] = wDex;
	emSTATS[ EMSPI ] = wSpi;
	emSTATS[ EMSTR ] = wStr;
	emSTATS[ EMSTA ] = wSta;
	ReqStatsUp( emSTATS );
}

LuaTable GLCharacter::GetPutOnItem( EMSLOT emSlot )
{
	LuaTable tbItem( GLWidgetScript::GetInstance().GetLuaState() );
	tbItem.set( 1, m_PutOnItems[ emSlot ].Mid() );
	tbItem.set( 2, m_PutOnItems[ emSlot ].Sid() );
	tbItem.set( 3, m_PutOnItems[ emSlot ].GetNativeID().IsValidNativeID() );

	return tbItem;
}

LuaTable GLCharacter::GetPutOnItems()
{
	LuaTable tbItems( GLWidgetScript::GetInstance().GetLuaState() );
	
	for( int i = 0; i < SLOT_NSIZE_S_2; ++i )
		tbItems.set( i + 1, GetPutOnItem( EMSLOT( i ) ) );

	return tbItems;
}

//const SITEMCUSTOM* GLCharacter::GetPutOnItemCustom( SNATIVEID nativId )
//{
//	const SITEM* pItem = GLogicData::GetInstance().GetItem( nativId );
//
//	EMSLOT emSlot = SLOT_TSIZE;
//
//	if( pItem )
//	{		
//		switch ( pItem->sBasicOp.emItemType )
//		{
//		case ITEM_SUIT:
//		case ITEM_INC_GRINDINGRATE:
//		case ITEM_ANTI_RESET:
//		case ITEM_ANTI_DISAPPEAR:
//		case ITEM_REVIVE:
//			{
//				switch ( pItem->sSuitOp.emSuit )
//				{
//				case SUIT_HEADGEAR:
//					emSlot = SLOT_HEADGEAR;
//					break;
//				case SUIT_UPPER:
//					emSlot = SLOT_UPPER;
//					break;
//				case SUIT_LOWER:
//					emSlot = SLOT_LOWER;
//					break;
//				case SUIT_HAND:
//					emSlot = SLOT_HAND;
//					break;
//				case SUIT_FOOT:
//					emSlot = SLOT_FOOT;
//					break;
//				case SUIT_NECK:
//					emSlot = SLOT_NECK;
//					break;
//				case SUIT_WRIST:
//					emSlot = SLOT_WRIST;
//					break;
//				case SUIT_FINGER:
//					{
//						if ( VALID_SLOT_ITEM(SLOT_LFINGER) && !VALID_SLOT_ITEM(SLOT_RFINGER) )
//						{ // 오른손가락만 비어있다면 오른손가락에 착용;
//							emSlot = SLOT_RFINGER; 
//						}
//						else
//						{ // 그외의 경우에는 모두 왼손가락 착용;
//							emSlot = SLOT_LFINGER;
//						}
//					}
//					break;
//				case SUIT_WAISTBAND:
//					emSlot = SLOT_WAISTBAND;
//					break;
//				case SUIT_EARRING:
//					emSlot = SLOT_EARRINGS;
//					break;
//				case SUIT_DECORATION:
//					emSlot = SLOT_DECORATION;
//					break;
//				case SUIT_ACCESSORIES:
//					{
//						if ( VALID_SLOT_ITEM(SLOT_LACCESSORIES) && !VALID_SLOT_ITEM(SLOT_RACCESSORIES) )
//						{ // 오른쪽만 비어있다면 오른쪽에 착용;
//							emSlot = SLOT_RACCESSORIES;                     
//						}
//						else
//						{ // 그외의 경우에는 모두 왼쪽 착용;
//							emSlot = SLOT_LACCESSORIES;
//						}
//					}
//					break;
//
//				case SUIT_HANDHELD:
//					{
//						if ( pItem->sSuitOp.IsBOTHHAND() )
//						{
//							emSlot = GetCurRHand();
//						}
//						else if ( pItem->sSuitOp.emHand == HAND_BOTH )
//						{
//							emSlot = GetCurRHand();
//
//							if ( VALID_SLOT_ITEM(GetCurRHand()) )
//							{ 
//								if ( !VALID_SLOT_ITEM(GetCurLHand()) )
//								{
//									emSlot = GetCurLHand();
//								}
//							}
//						}
//						else if ( pItem->sSuitOp.emHand&HAND_LEFT )
//						{
//							emSlot = GetCurLHand();
//						}
//						else if ( pItem->sSuitOp.emHand&HAND_RIGHT )
//						{
//							emSlot = GetCurRHand();
//						}
//					}
//					break;
//				}
//			}
//			break;
//
//		case ITEM_ARROW:
//		case ITEM_CHARM:
//		case ITEM_BULLET:
//			{
//				emSlot = GetCurLHand();
//			}
//			break;
//
//		case ITEM_VEHICLE:
//			{
//				emSlot = SLOT_VEHICLE;
//			}
//			break;
//
//		default: return NULL;
//		}
//	}
//
//	return &m_PutOnItems[emSlot];
//
//}

WORD GLCharacter::GetPutOnItemCount( EMSLOT emSlot )
{
	return m_PutOnItems[ emSlot ].wTurnNum;
}

int GLCharacter::GetPutOnItemType( EMSLOT emSlot )
{
	SITEM* pItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emSlot ].GetNativeID() );
	if( NULL == pItem )
		return -1;

	return pItem->BasicType();
}

int GLCharacter::GetWeaponType(void)
{
	//	Note : 아이템 착용 조건 검사.
	EMSLOT emRHand, emRHand_S;
	if( TRUE == IsUseArmSub() )
	{
		emRHand = SLOT_RHAND_S;
		emRHand_S = SLOT_RHAND;
	}
	else
	{
		emRHand = SLOT_RHAND;
		emRHand_S = SLOT_RHAND_S;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emRHand ].GetNativeID() );
	if( NULL == pItem )
		return 0;

	return pItem->sSuitOp.emAttack;
}

int GLCharacter::CheckTrashItem()
{
	if( FALSE == m_sGarbageItem.VALID() )
		return NSWIDGET_SCRIPT::EMINVEN_TRASH_NOREGIST;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sGarbageItem.wPosX, m_sGarbageItem.wPosY );
	if ( NULL == pResistItem )
		return NSWIDGET_SCRIPT::EMINVEN_TRASH_NOINVEN;

	BYTE nType = 0;
	LONGLONG nCost = 0;
	bool bOk = m_pGaeaClient->GetCharacter()->IsValidGarbageUse( pResistItem->CustomItem().GetNativeID(), nType, nCost );
	if( false == bOk )
	{
		if( GLCHARLOGIC::EGarbageMoney == nType ||
			GLCHARLOGIC::EGarbagePoint == nType )
			return NSWIDGET_SCRIPT::EMINVEN_TRASH_MONEY;
		else
			return NSWIDGET_SCRIPT::EMINVEN_TRASH_NOITEM;
	}

	return NSWIDGET_SCRIPT::EMINVEN_TRASH_OK;
}

ESlotHold GLCharacter::UseInvenItem( WORD wPosX, WORD wPosY )
{
	bool bHold = holditem::HaveHoldItem();

	const SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return ESlotHold::Release;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if( NULL == pItem )
		return ESlotHold::Release;

	// 아이템 조건 확인
	EMINVEN_USE_ITEM emItemMsg = EMINVEN_USE_ITEM_OK;
	if( FALSE == SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom, emItemMsg ) && false == bHold )
	{
		// 에러 메시지 출력
		switch( emItemMsg )
		{
		case EMINVEN_USE_ITEM_FAIL :
			break;

		case EMINVEN_USE_ITEM_NOITEM :
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "EMCHARGED_ITEM_GET_FB_NOITEM" ) );
			break;

		case EMINVEN_USE_ITEM_BADITEM :
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "EMINVEN_RENAME_FB_BADITEM" ) );
			break;

		case EMINVEN_USE_ITEM_BADCLASS :
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "EMINVEN_GENDER_CHANGE_FB_BADCLASS" ) );
			break;

		case EMINVEN_USE_ITEM_BADLEVEL :
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "EMINVEN_ITEM_WRONG_LEVEL" ) );
			break;
		}

		return ESlotHold::Release;
	}

	// 메시지박스 필요한 아이템일 경우 선택 팝업을 띄운다.( ex: 선물상자, 포인트카드 )
	EMITEM_TYPE emItemType = GetInvenItemType( wPosX, wPosY );
	switch( emItemType )
	{
	case ITEM_LOCK_BOX :
		{
			// 잠겨있다면, 열겟다는 메세지박스 오픈하지않음
			if( true == pInvenItem->sItemCustom.bVietnamGainItem )
			{
				break;
			}
			else
			{
				if( true == holditem::IsHoldPlace( holditem::INVENTORY) )
					holditem::ReleaseHoldItem();
			}
		}
	case ITEM_BOX :
	case ITEM_WRAPPED_BOX :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_UseItemBox,
				"-n, -n, -s", 
				wPosX, wPosY, pInvenItem->sItemCustom.GETNAME().c_str() );

		}
		return ESlotHold::Release;

	case ITEM_POINT_CARD_REFUND :
	case ITEM_POINT_CARD_NOT_REFUND :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_UseItemPointCard,
				"-n, -n, -s",
				wPosX, wPosY, pInvenItem->sItemCustom.GETNAME().c_str() );
		}
		return ESlotHold::Release;

	case ITEM_RANMOBILE_CONNECTION :
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFRunc_RanMobile_UseRecognizeItem );
		}
		break;
	}

	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsOpen ) )
	{
		if( ITEM_PRIVATEMARKET == emItemType )
			return ESlotHold::Release;

		/*if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
			NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_IsBlock,
			"-n, -n", wPosX, wPosY ) )
			return ESlotHold::Release;*/
	}

	return ReqInvenDrug( wPosX, wPosY );
}

bool GLCharacter::TryRequestBankItemInfo()
{
	return S_OK == ReqItemBankInfo( false ) ? true : false;
}

DWORD GLCharacter::GetSkillQuick( WORD wIndex )
{
	if( EMSKILLQUICK_SIZE <= wIndex )
		return NATIVEID_NULL().dwID;

	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		if( m_sChangeQUICK[wIndex].IsValidNativeID() )
			return m_sChangeQUICK[wIndex].dwID;
	}

	return m_sSKILLQUICK[ wIndex ].dwID;	
}

DWORD GLCharacter::GetItemQuick( WORD wIndex )
{
	if( EMACTIONQUICK_SIZE <= wIndex )
		return NATIVEID_NULL().dwID;

	return m_sACTIONQUICK[ wIndex ].GetNativeID().dwID;
}

void GLCharacter::SendChatMsg( DWORD dwType, const char* szName, const char* szMsg, LuaTable tbData )
{
	if( true == m_pGaeaClient->ExecuteCommand( szMsg ) )
		return;

	// 채팅 블럭이더라도 귓말, 파티, 클럽, 동맹, 진영 채팅이라면 허용한다;
	if( true == GLChatMan::GetInstance().CheckBlock( szMsg ) 
		&& ( 0 == (dwType & (GLCHAT_DEF::EMCHAT_PRIVATE|GLCHAT_DEF::EMCHAT_PARTY|GLCHAT_DEF::EMCHAT_GUILD|GLCHAT_DEF::EMCHAT_ALLIANCE|GLCHAT_DEF::EMCHAT_FACTION)) ) )
		return;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	GLCharacter* pChar     = m_pGaeaClient->GetCharacter();

	bool bPrison = false;
	bool bEnable = true;

	if( pLand && pChar ) {
		const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLand->GetBaseMapID() );

		if( pMapNode && PRISONLIMIT.IsLimitStatus( pMapNode, pChar->UserLevel(), PML_CHATTING ) ) {
			bPrison = true;	
		}
	}

	if( bPrison ) {
		bEnable = (dwType != GLCHAT_DEF::EMCHAT_NORMAL) ? false : true;
	}	

	if( !bEnable ) {
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::RED, ID2GAMEINTEXT( "CHAT_PRISION_LIMIT" ) );
		return;
	}

	VEC_LINK_TEXT_DATA_EX vecTextLinkData;
	VEC_LINK_DATA_BASIC vecLinkBasic;
	for( int i = 1; i <= tbData.TableLen(); ++i )
	{
		LuaTable tbLinkData = tbData.get< LuaTable >( i );

		SLINK_TEXT_DATA_EX sTextLinkData;
		sTextLinkData.sLinkDataBasic.sLinkType.wMType = tbLinkData.get< WORD >( "wMType" );
		sTextLinkData.sLinkDataBasic.sLinkType.wSType = tbLinkData.get< WORD >( "wSType" );
		sTextLinkData.sLinkDataBasic.sLinkPos.nStart = tbLinkData.get< int >( "nStart" );
		sTextLinkData.sLinkDataBasic.sLinkPos.nEnd = tbLinkData.get< int >( "nEnd" );
		sTextLinkData.sLinkDataBasic.dwData1 = tbLinkData.get< DWORD >( "dwData1" );
		sTextLinkData.sLinkDataBasic.dwData2 = tbLinkData.get< DWORD >( "dwData2" );
		sTextLinkData.dwTextColor = tbLinkData.get< DWORD >( "dwColor" );

		vecLinkBasic.push_back( sTextLinkData.sLinkDataBasic );
		vecTextLinkData.push_back( sTextLinkData );
	}

	if ( S_OK == ReqSendChatMsg( dwType, szName, szMsg, vecLinkBasic ) )
	{
		switch ( dwType )
		{
		case GLCHAT_DEF::EMCHAT_SYSTEM:
		case GLCHAT_DEF::EMCHAT_TOALL:
			break;
		default:
			VEC_LINK_TEXT_DATA vecLinkData;
			GLChatMan::GetInstance().ConvertLinkData( vecTextLinkData, vecLinkData );
			GLChatMan::GetInstance().OnNotifyReceiveMsg( dwType, szName, szMsg, true, 
				USER_GM3 <= UserLevel(), &vecLinkData );
			break;
		}
	}
//	 ReqSendChatMsg( dwType, szName, szMsg, vecLinkBasic );
}

void GLCharacter::GetSkillIndexRange( int& nBegin, int& nEnd )
{
	switch( m_emClass )
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
		{
			nBegin = EMSKILL_FIGHTER_01;
			nEnd = EMSKILL_FIGHTER_04;
		}
		break;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
		{
			nBegin = EMSKILL_ARMS_01;
			nEnd = EMSKILL_ARMS_04;
		}
		break;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
		{
			nBegin = EMSKILL_ARCHER_01;
			nEnd = EMSKILL_ARCHER_04;
		}
		break;

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
		{
			nBegin = EMSKILL_SPIRIT_01;
			nEnd = EMSKILL_SPIRIT_04;
		}
		break;	

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
		{
			nBegin = EMSKILL_EXTREME_01;
			nEnd = EMSKILL_EXTREME_04;
		}
		break;
	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
		{
			nBegin = EMSKILL_SCIENTIST_01;
			nEnd = EMSKILL_SCIENTIST_04;			
		}
		break;
	case GLCC_ASSASSIN_M:
	case GLCC_ASSASSIN_W:
		{
			nBegin = EMSKILL_ASSASSIN_01;
			nEnd = EMSKILL_ASSASSIN_04;			
		}
		break;
	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
		{
			nBegin = EMSKILL_TRICKER_01;
			nEnd = EMSKILL_TRICKER_04;			
		}
		break;
	case GLCC_ETC_M:
	case GLCC_ETC_W:
		{
			nBegin = EMSKILL_ETC_01;
			nEnd = EMSKILL_ETC_04;			
		}
		break;
	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
		{
			nBegin = EMSKILL_ACTOR_01;
			nEnd = EMSKILL_ACTOR_08;	
		}
		break;
	}
}

std::string GLCharacter::GetLearnedSkillCost()
{
	std::string strCost;

	strCost = GLWidgetScript::MakeMoney(  GLSkillResetDefine::Instance()->GetLearnedSkillCost(m_ExpSkills) );

	return strCost;
}

LuaTable GLCharacter::GetSkillList( int nPage )
{
	LuaTable tbSkillList( GLWidgetScript::GetInstance().GetLuaState() );

	if( SKILL_TAB_COUNT <= nPage )
		return tbSkillList;

	BOOL  bChange   = RF_DISGUISE( m_EffSkillVarSet).IsOn();
	int   dwTableID = RF_DISGUISE( m_EffSkillVarSet ).GetTableID() - 1;

	bChange = (dwTableID < 0) ? FALSE : bChange;
	
	if( bChange ) {
		bChange = !GSCL_INST.IsChangeTable((UINT)dwTableID) ? FALSE : bChange;
	}

	VEC_GLSKILL_ITER iter = m_vecSkill[ nPage ].begin();
	for( int i = 1; iter != m_vecSkill[ nPage ].end(); ++iter, ++i )
	{		
		GLSKILL* pSkill = (*iter);

		SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );

		if( bChange )
		{
			GLSKILL* pOrgSkill = pSkill;

			SCHARSKILL pTempCharSkill;

			pSkill = GSCL_INST.GetChangeSkill( (UINT)dwTableID, pSkill, pTempCharSkill );
			
			//  [9/19/2016 gbgim];
			// * before now 
			//   사용될 스킬은 변신전까지 초기화가 안되어있고;
			//   변신시 여기서(UI skill ui page 갱신시 초기화되고있었음)시 초기화가된다;
			// * after
			//   로비에서 게임 스테이지로 전환시 변신 스킬이 함께 초기화되도록 수정함;
			//   changeSet확인;
			// * after after
			//	 세팅이 제대로 안되어 있을 경우를 대비하여 주석 해제;
			
			if( pSkill && pOrgSkill != pSkill && pTempCharSkill.sNativeID != NATIVEID_NULL() )
			{
				if ( pCharSkill )
					LEARN_CHANGE_SKILL( pOrgSkill, pCharSkill->wLevel );
			}

			if( pSkill == NULL ) 
				pSkill = pOrgSkill;
		}

		WORD wNextLevel = 0;
		if( NULL != pCharSkill )
		{
			wNextLevel = pCharSkill->wLevel + 1;
			if( SKILL::MAX_LEVEL <= wNextLevel )
				wNextLevel = SKILL::MAX_LEVEL - 1;
		}

		SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
		SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[ wNextLevel ];

		// 선행 스킬 있는지 확인
		GLSKILL* pNeedSkill = NULL;
		for( int j = 0; j < SKILL_TAB_COUNT; ++j )
		{
			for( VEC_GLSKILL_ITER it = m_vecSkill[ j ].begin(); it != m_vecSkill[ j ].end(); ++it )
			{
				GLSKILL* pTempSkill = *it;
				GLSKILL* pOrgSkill = pTempSkill;

				// 변장 후의 스킬로 변경한다;
				SCHARSKILL pTempCharSkill;
				pTempSkill = GSCL_INST.GetChangeSkill( (UINT)dwTableID, pTempSkill, pTempCharSkill );

				if( pTempSkill == NULL ) 
					pTempSkill = pOrgSkill;

				// 선행스킬인지 확인한다;
				if( pTempSkill->GetId() != sLEARN.sSKILL )
					continue;

				// 선행스킬이라면 저장한다;
				pNeedSkill = pTempSkill;
				
				break;
			}
		}

		LuaTable tbSkill( GLWidgetScript::GetInstance().GetLuaState() );
		// 기본 정보 -----------------------------------------------------------
		tbSkill.set( "dwID", pSkill->GetId().dwID );								// ID
		tbSkill.set( "strName", pSkill->GetName() );								// 스킬 이름
		tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );							// 등급
		tbSkill.set( "wLevel", NULL == pCharSkill ? 0 : pCharSkill->wLevel + 1 );	// 현재 레벨
		tbSkill.set( "dwMaxLevel", pSkill->m_sBASIC.dwMAXLEVEL );					// 최대 레벨
		tbSkill.set( "bPassive", pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? true : false );

		// 요구 정보 -----------------------------------------------------------
		tbSkill.set( "dwNeedLevel", sLVL.dwLEVEL );				// 캐릭터 레벨
		tbSkill.set( "dwNeedPoint", sLVL.dwSKP );				// 스킬 포인트
		tbSkill.set( "dwNeedSkillLevel", sLVL.dwSKILL_LVL + 1 );// 요구 보유 스킬 레벨
		tbSkill.set( "bOnlyOneStat", sLEARN.bOnlyOneStats );	// 요구 능력치 중 하나만 충족해도 되는지 여부
		tbSkill.set( "wPow", sLVL.sSTATS.wPow );				// 힘
		tbSkill.set( "wStr", sLVL.sSTATS.wStr );				// 체력
		tbSkill.set( "wSpi", sLVL.sSTATS.wSpi );				// 정신
		tbSkill.set( "wDex", sLVL.sSTATS.wDex );				// 민첩
		tbSkill.set( "wSta", sLVL.sSTATS.wSta );				// 근력
		if( NULL == pNeedSkill )								// 선행 스킬 ID, 이름, 레벨 충족?
		{
			tbSkill.set( "dwNeedSkillID", 0 );
			tbSkill.set( "strNeedSkillName", "" );
			tbSkill.set( "bNeedSkill", true );
		}
		else
		{
			tbSkill.set( "dwNeedSkillID", pNeedSkill->GetId().dwID );
			tbSkill.set( "strNeedSkillName", pNeedSkill->GetName() );

			SCHARSKILL* pCharNeedSkill = GETLEARNED_SKILL( pNeedSkill->GetId() );
			if( NULL == pCharNeedSkill )
				tbSkill.set( "bNeedSkill", false );
			else
				tbSkill.set( "bNeedSkill", ( sLVL.dwSKILL_LVL <= pCharNeedSkill->wLevel ) );
		}

		LuaTable tbSkillMoeny( GLWidgetScript::GetInstance().GetLuaState() );
		for( int iLevel = 0; iLevel < SKILL::MAX_LEVEL; ++iLevel )
		{
			SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[ iLevel ];
			if( sLEARN.sLVL_STEP[iLevel].dwReqMoney <= 0 )
				break ;

			tbSkillMoeny.set( iLevel, sLEARN.sLVL_STEP[iLevel].dwReqMoney);
		}

		// 금액
		tbSkill.set( "dwMoney", sLVL.dwReqMoney );
		tbSkill.set( "dwMoneyLevelList", tbSkillMoeny );
		// 스크롤 사용 여부
		tbSkill.set( "bUseScroll", GLogicData::GetInstance().GetRnSkillUseScrool( pSkill->GetId() ) );

		tbSkillList.set( i, tbSkill );
	}

	return tbSkillList;
}

DWORD GLCharacter::GetUsingSkillPoint( LuaTable tbSkillList )
{
	DWORD dwUsingSkillPoint = 0;
	for( int i = 1; i <= tbSkillList.TableLen(); ++i )
	{
		LuaTable tbSkill = tbSkillList.get< LuaTable >( i );

		GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( SNATIVEID( tbSkill.get< DWORD >( "dwID" ) ) );
		if( NULL == pSkill )
			continue;

		SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );
		if( NULL == pCharSkill )
			continue;

		for( WORD i = 0; i < pCharSkill->wLevel + 1; ++i )
		{
			if( i < SKILL::MAX_LEVEL )
				dwUsingSkillPoint += pSkill->m_sLEARN.sLVL_STEP[ i ].dwSKP;
		}
	}

	return dwUsingSkillPoint;
}

LuaTable GLCharacter::GetSkillInvest( DWORD dwID, WORD wLevel )
{
	LuaTable tbSkill( GLWidgetScript::GetInstance().GetLuaState() );

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( SNATIVEID( dwID ) );
	if( NULL == pSkill )
		return tbSkill;

	SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );
	if( NULL != pCharSkill )
		wLevel += pCharSkill->wLevel;

	WORD wDisplayLevel = wLevel;
	if( SKILL::MAX_LEVEL <= wLevel )
		wLevel = SKILL::MAX_LEVEL;

	if( SKILL::MAX_LEVEL <= wLevel )
		wLevel = SKILL::MAX_LEVEL - 1;

	SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;
	SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[ wLevel ];

	// 기본 정보 -----------------------------------------------------------
	tbSkill.set( "dwID", pSkill->GetId().dwID );					// ID
	tbSkill.set( "strName", pSkill->GetName() );					// 스킬 이름
	tbSkill.set( "dwGrade", pSkill->m_sBASIC.dwGRADE );				// 등급
	tbSkill.set( "wLevel", wDisplayLevel );							// 포인트 투자 후 레벨
	tbSkill.set( "dwMaxLevel", pSkill->m_sBASIC.dwMAXLEVEL );		// 최대 레벨
	tbSkill.set( "bPassive", pSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ? true : false );

	// 요구 정보 -----------------------------------------------------------
	tbSkill.set( "dwNeedLevel", sLVL.dwLEVEL );				// 캐릭터 레벨
	tbSkill.set( "dwNeedPoint", sLVL.dwSKP );				// 스킬 포인트
	tbSkill.set( "dwNeedSkillLevel", sLVL.dwSKILL_LVL + 1 );// 요구 보유 스킬 레벨
	tbSkill.set( "bOnlyOneStat", sLEARN.bOnlyOneStats );	// 요구 능력치 중 하나만 충족해도 되는지 여부
	tbSkill.set( "wPow", sLVL.sSTATS.wPow );				// 힘
	tbSkill.set( "wStr", sLVL.sSTATS.wStr );				// 체력
	tbSkill.set( "wSpi", sLVL.sSTATS.wSpi );				// 정신
	tbSkill.set( "wDex", sLVL.sSTATS.wDex );				// 민첩
	tbSkill.set( "wSta", sLVL.sSTATS.wSta );				// 근력
	// 선행 스킬 ID, 이름, 레벨 충족?
	// 스킬 포인트를 투자 안한 상태를 제외하고
	// 선행 스킬 정보는 스크립트 내에서만 알 수 있으니 더미 정보만 넘겨준다.
	tbSkill.set( "dwNeedSkillID", 0 );
	tbSkill.set( "strNeedSkillName", "" );
	tbSkill.set( "bNeedSkill", false );

	return tbSkill;
}

LuaTable GLCharacter::GetLearnSkillMaxLevel( DWORD dwID, DWORD dwCurrnetLevel )
{
	LuaTable tbLearnCost( GLWidgetScript::GetInstance().GetLuaState() );

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( SNATIVEID( dwID ) );
	if( pSkill == NULL	)
		return NULL;

	int nLevel = dwCurrnetLevel;

	int nCharLearnLevl = 0;

	SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );
	if( pCharSkill != NULL)
	{
		nCharLearnLevl = pCharSkill->wLevel + 1;
		nLevel = nCharLearnLevl + nLevel;

		// 배운 스킬일 경우에는 현재 레벨값이 다르게 나온다.
		if( dwCurrnetLevel != 0 )
			nLevel -= 1;
	}

	LONGLONG LONGLONGReserveMoney = GLWidgetScript::GetInstance().LuaCallFunc< LONGLONG >( NSWIDGET_SCRIPT::g_strFunc_SkillUpMoney  );
	DWORD dwUsingSkillPoint = GLWidgetScript::GetInstance().LuaCallFunc< DWORD >( NSWIDGET_SCRIPT::g_strFunc_SkillUsingPoint  );

	//DWORD dwOrignalMoney =dwReserveMoney;
	DWORD dwOrignalSkillPoint = dwUsingSkillPoint;

	LONGLONG longCurrnetMoney = this->GetInvenMoney();
	DWORD dwCurrnetSkillPoint = this->m_dwSkillPoint;

	SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;

	for ( nLevel; nLevel <= SKILL::MAX_LEVEL; ++nLevel)
	{
		if( nLevel >= pSkill->m_sBASIC.dwMAXLEVEL )
		{
			nLevel = (int)pSkill->m_sBASIC.dwMAXLEVEL;
			if( nCharLearnLevl == 0 )
				nLevel -= 1;

			break ;
		}

		SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[nLevel];

		if( sLVL.dwLEVEL == 1 &&
			sLVL.dwSKILL_LVL == 0 &&
			sLVL.dwReqMoney == 0 )
		{
			nLevel -= 1;
			break ;
		}

		if( GLCHARLOGIC::CHECKLEARNABLE_SKILL(  SNATIVEID( dwID ) , (WORD)nLevel ) != EMSKILL_LEARN_OK )
		{
			if( nCharLearnLevl == 0 )
				nLevel -= 1;
			
			break ;
		}

		if( (LONGLONG)(( LONGLONGReserveMoney + sLVL.dwReqMoney )) > longCurrnetMoney)
		{
			nLevel -= 1;
			break ;
		}

		dwUsingSkillPoint + sLVL.dwSKP;
		if( dwUsingSkillPoint + sLVL.dwSKP > dwCurrnetSkillPoint)
		{
			nLevel -= 1;
			break ;
		}

		dwUsingSkillPoint += sLVL.dwSKP;
		LONGLONGReserveMoney += sLVL.dwReqMoney;
	}

	std::stringstream strsteram;
	strsteram << LONGLONGReserveMoney;

	tbLearnCost.set( "ReserveMoney", strsteram.str().c_str() );
	tbLearnCost.set( "UsingSkillPoint", dwUsingSkillPoint - dwOrignalSkillPoint );
	tbLearnCost.set( "Level", nLevel - nCharLearnLevl );

	return tbLearnCost;
}

LuaTable GLCharacter::GetLearnSkillMinLevel( DWORD dwID, DWORD dwCurrnetLevel )
{
	LuaTable tbLearnCost( GLWidgetScript::GetInstance().GetLuaState() );

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( SNATIVEID( dwID ) );
	if( pSkill == NULL	)
		return tbLearnCost;

	DWORD minLevel = 0;
	int nLevel = dwCurrnetLevel - 1;

	SCHARSKILL* pCharSkill = GETLEARNED_SKILL( pSkill->GetId() );
	if( pCharSkill != NULL)
	{
		minLevel = pCharSkill->wLevel + 1;
		nLevel += pCharSkill->wLevel;
	}

	DWORD dwReserveMoney = 0;
	DWORD dwUsingSkillPoint = 0;

	SKILL::SLEARN& sLEARN = pSkill->m_sLEARN;

	for ( nLevel; nLevel >= (int)minLevel ; --nLevel)
	{
		SKILL::SLEARN_LVL& sLVL = sLEARN.sLVL_STEP[nLevel];

		dwUsingSkillPoint += sLVL.dwSKP;
		dwReserveMoney += sLVL.dwReqMoney;
	}


	tbLearnCost.set( "ReserveMoney", dwReserveMoney );
	tbLearnCost.set( "UsingSkillPoint", dwUsingSkillPoint );
	tbLearnCost.set( "Level", minLevel );

	return tbLearnCost;
}

DWORD GLCharacter::GetNeedSkillPoint( DWORD dwID, WORD wLevel )
{
	if( SKILL::MAX_LEVEL <= wLevel )
		return 0;

	GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( SNATIVEID( dwID ) );
	if( NULL == pSkill )
		return 0;

	return pSkill->m_sLEARN.sLVL_STEP[ wLevel ].dwSKP;
}

void GLCharacter::SkillLevelUp( LuaTable tbSkillUpData )
{
	std::vector< SNATIVEID > vecSkillID;

	for( int i = 1; i <= tbSkillUpData.TableLen(); ++i )
	{
		SNATIVEID sID( tbSkillUpData.get< DWORD >( i ) );
		vecSkillID.push_back( sID );
	}

	if( true == vecSkillID.empty() )
		return;

	ReqRnSkillUp( vecSkillID );
}

WORD GLCharacter::GetEmptySkillQuick()
{
	WORD wTab = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	for( WORD i =  wTab * wSlotCount; i < EMSKILLQUICK_SIZE; ++i )
	{
		if( false == m_sSKILLQUICK[ i ].IsValidNativeID() )
			return i;
	}

	return EMSKILLQUICK_SIZE;
}

LuaTable GLCharacter::GetHaveVehicleAll()
{
	LuaTable tbItems( GLWidgetScript::GetInstance().GetLuaState() );

	const GLInventory& ref_Inventory = m_cInventory;	
	const GLInventory::CELL_MAP* pItemList = ref_Inventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pItemList->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemList->end();
	int i = 0;
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM *pINVENITEM = (*iter).second;

		const SITEM* pItemData = GLogicData::GetInstance().GetItem ( pINVENITEM->GetNativeID() );
		if( pItemData->BasicType() == ITEM_VEHICLE )
		{
			tbItems.set( i + 1, pINVENITEM->GetNativeID().Id() );
			i++;
		}
	}

	return tbItems;
}

bool GLCharacter::HaveClubJoinAutority()
{
	return m_pMyClub->HaveMemberJoinAuthority(m_CharDbNum);
}

bool GLCharacter::HaveClubAllianceAuthority()
{
	return m_pMyClub->HaveAllianceAuthority(m_CharDbNum);
}

bool GLCharacter::HaveClubBattleAuthority()
{
	return GLCONST_CHAR::bCLUB_BATTLE && m_pMyClub->HaveClubBattleAuthority(m_CharDbNum);
}

bool GLCharacter::HaveClubStorageAuthority()
{
	return m_pMyClub->HaveStorageAuthority(m_CharDbNum);
}

bool GLCharacter::IsAllianceMaster()
{
	return m_pMyClub->IsChief();
}

bool GLCharacter::IsRideAVehicleNow()
{ 
	return (VehicleState() == EMVEHICLE_DRIVER);
}

DWORD GLCharacter::GetCurrentMapID()
{
	GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
	if( NULL == pLandClient )
		return -1;

	return pLandClient->GetBaseMapID().dwID;
}

LuaTable GLCharacter::GetPositionForLua()
{
	LuaTable tbPosition( GLWidgetScript::GetInstance().GetLuaState() );
	tbPosition.set( "x", m_vPosition.x );
	tbPosition.set( "y", m_vPosition.y );
	tbPosition.set( "z", m_vPosition.z );

	return tbPosition;
}

LuaTable GLCharacter::GetMapPosition()
{
	LuaTable tbPosition( GLWidgetScript::GetInstance().GetLuaState() );

	GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
	if( NULL == pLandClient )
		return tbPosition;

	int nPosX = 0;
	int nPosY = 0;

	const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();
	sMapAxisInfo.Convert2MapPos( m_vPosition.x, m_vPosition.z, nPosX, nPosY );

	tbPosition.set( "x", nPosX );
	tbPosition.set( "y", nPosY );

	return tbPosition;
}

LuaTable GLCharacter::GetMapPositionOffset()
{
	LuaTable tbPosition( GLWidgetScript::GetInstance().GetLuaState() );

	GLLandManClient* pLandClient = m_pGaeaClient->GetActiveMap();
	if( NULL == pLandClient )
		return tbPosition;

	const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();

	int nPosX = 0;
	int nPosY = 0;
	GLMinimapClient::GetInstance().ConvertWorldToMap( sMapAxisInfo, m_vPosition, nPosX, nPosY );

	tbPosition.set( "x", nPosX );
	tbPosition.set( "y", nPosY );

	return tbPosition;
}

void GLCharacter::ReqBikePutOnWith()
{
	if( m_pGaeaClient->GetCharacter()->VehicleState() != EMVEHICLE_DRIVER )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_BOARDING") );
		return;
	}

	if( m_pGaeaClient->GetCharacter()->IsProducting() )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_DONT_T_INVEN" ) ); 
		return;
	}

	INT nIndex = 0;
	for (INT i = 0; i < MAX_PASSENGER_COUNT; i++ )
	{
		if (m_pGaeaClient->GetCharacter()->VehiclePassenger(i) == GAEAID_NULL)
		{
			continue;
		}
		nIndex++;						
	}
	if( nIndex >= PASSENGER_COUNT )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_PASSENGER_FULL") );
		return;
	}

	const SITEM* pVehicleItem = GLogicData::GetInstance().GetItem(m_pGaeaClient->GetCharacter()->VehicleId());
	if( !pVehicleItem )
	{
		return;
	}
	if ( pVehicleItem->sVehicle.bNotUseParts == TRUE )
	{
		if ( pVehicleItem->sVehicle.emVehicleType == VEHICLE_TYPE_BOARD )
		{			
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_SUBSEAT") );
			return;
		}
	}
	else
	{
		if (m_pGaeaClient->GetCharacter()->VehiclePutOnItem(ACCE_TYPE_PARTS_E) == NATIVEID_NULL())
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_NOT_SUBSEAT") );
			return;
		}
	}

	GLMSG::SNET_WITH_VEHICLE NetMsg;
	NetMsg.dwTargetID = m_PtoPTargetID;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

bool GLCharacter::MoveToMap( DWORD dwMapID, int nPosX, int nPosY, bool bVehicle, bool bGMMove )
{
	// GM 맵 이동
	if( true == bGMMove && USER_USER_GM <= UserLevel() )
	{
		GLLevelFileClient* pLevelFile = GLMinimapClient::GetInstance().GetLevelClient( dwMapID );
		if( NULL == pLevelFile || true == pLevelFile->IsInstantMap() )
			return false;

		int nMapPosX = 0;
		int nMapPosY = 0;
		GLMinimapClient::GetInstance().ConvertImgToMapPos( pLevelFile->GetMapAxisInfo(), nPosX, nPosY, nMapPosX, nMapPosY );

		const SNATIVEID mapID = pLevelFile->GetBaseMapID();

		if ( m_pGaeaClient->IsEmulator() )
		{
			GLMSG::SNET_GM_MOVE2MAPPOS_FLD NetMsgFld;
			NetMsgFld.nidMAP = mapID;
			NetMsgFld.dwPOSX = nMapPosX;
			NetMsgFld.dwPOSY = nMapPosY;
			NetMsgFld.dwGaeaID = 0;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsgFld);
		}
		else
		{
			GLMSG::SNET_GM_MOVE2MAPPOS NetMsg( mapID, nMapPosX, nMapPosY );
			m_pGaeaClient->NETSENDTOAGENT( &NetMsg );
		}

		return false;
	}

	// 일반 맵 이동
	GLLevelFileClient* pLevelFile = m_pGaeaClient->GetActiveMap();
	if( NULL == pLevelFile )
		return false;

	if( dwMapID != pLevelFile->GetBaseMapID().dwID )
		return false;

	D3DXVECTOR3 vWorldPos( 0.0f, 0.0f, 0.0f );
	GLMinimapClient::GetInstance().ConvertImgMapToWorld( pLevelFile->GetMapAxisInfo(), nPosX, nPosY, vWorldPos );

	// 맵 이동시 자동 탈것 타기
	if( true == bVehicle && EMVEHICLE_OFF != VehicleState() )
		ReqSetVehicle( true );

	return TRUE == LargeMapMoveTo( &vWorldPos, FALSE ) ? true : false;
}

void GLCharacter::SetVehicleColor( LuaTable tbColorList )
{
	DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
	if( NULL == pVehicle )
		return;

	DWORD dwMainColor = 0;
	DWORD dwSubColor = 0;

	for( int i = 1; i <= tbColorList.TableLen(); ++i )
	{
		if( ACCE_TYPE_SIZE <= i )
			break;

		LuaTable tbColor = tbColorList.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbColor ) )
			continue;

		if( false == tbColor.get< bool >( "bEnable" ) )
			continue;

		dwMainColor = tbColor.get< DWORD >( "dwMainColor" );
		dwSubColor = tbColor.get< DWORD >( "dwSubColor" );

		PDXCHARPART pSkinPiece = pVehicle->GetVehiclePart( i );
		if( NULL != pSkinPiece )
		{
			if( EMVEHICLE_DRIVER == m_emVehicle )
			{
				pSkinPiece->SetHairColor( dwMainColor, FALSE );
				pSkinPiece->SetSubColor( dwSubColor, TRUE );
			}
		}
	}
}

LuaTable GLCharacter::GetVehicleColor()
{
	LuaTable tbColorList( GLWidgetScript::GetInstance().GetLuaState() );

	DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
	if( NULL == pVehicle )
		return tbColorList;

	DWORD dwMainColor = 0;
	DWORD dwSubColor = 0;
	bool bEnable = false;

	int nTableIndex = 1;
	for( int i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; ++i )
	{
		PDXCHARPART pSkinPiece = pVehicle->GetVehiclePart( i );
		if( NULL != pSkinPiece )
		{
			// [shhan][2014.09.29] Overlay 색을 변경하다가 취소를 할 경우, 기존 셋팅값이 아닌, Piece 셋팅값이 나오는 문제 수정.
			dwMainColor = pSkinPiece->GetHairColor();
			dwSubColor = pSkinPiece->GetSubColor();

			if( D3DCOLOR_RGBA( 0, 0, 0, 0 ) == dwMainColor &&
				D3DCOLOR_RGBA( 0, 0, 0, 0 ) == dwSubColor )
				pSkinPiece->GetDefaultOverlayColor( dwMainColor, dwSubColor );

			// 값이 잘 들어가 있으면 셋팅.
			if( D3DCOLOR_RGBA( 0, 0, 0, 0 ) != dwMainColor &&
				D3DCOLOR_RGBA( 0, 0, 0, 0 ) != dwSubColor )
			{
				dwMainColor = 0xFF000000 | dwMainColor;
				dwSubColor = 0xFF000000 | dwSubColor;
				bEnable = true;
			}
			else
			{
				bEnable = false;
			}
		}
		else
		{
			dwMainColor = 0;
			dwSubColor = 0;
			bEnable = false;
		}

		LuaTable tbColor( GLWidgetScript::GetInstance().GetLuaState() );
		tbColor.set( "dwMainColor", dwMainColor );
		tbColor.set( "dwSubColor", dwSubColor );
		tbColor.set( "bEnable", bEnable );
		tbColorList.set( nTableIndex++, tbColor );
	}

	return tbColorList;
}

void GLCharacter::CommitVehicleColor( LuaTable tbColorList )
{
	DWORD dwMainColor[ ACCE_TYPE_SIZE ];
	DWORD dwSubColor[ ACCE_TYPE_SIZE ];

	memset( dwMainColor, 0, sizeof( dwMainColor ) );
	memset( dwSubColor, 0, sizeof( dwSubColor ) );

	for( int i = 1; i <= tbColorList.TableLen(); ++i )
	{
		if( ACCE_TYPE_SIZE <= i )
			break;

		LuaTable tbColor = tbColorList.get< LuaTable >( i );
		if( false == IS_LUATABLE( tbColor ) )
			continue;

		if( true == tbColor.get< bool >( "bEnable" ) )
		{
			dwMainColor[ i ] = tbColor.get< DWORD >( "dwMainColor" );
			dwSubColor[ i ] = tbColor.get< DWORD >( "dwSubColor" );
		}
	}

	ReqInvenVehicleColorChange( dwMainColor, dwSubColor );
}

void GLCharacter::SetCostumeColor( LuaTable tbColor )
{
	EMSLOT emCostumSelectSlot = m_pGaeaClient->GetCharacter()->GetCostumSelectSlot();
	PDXCHARPART pCharPart = m_pGaeaClient->GetCharacter()->GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );
	if( NULL != pCharPart )
	{
		DWORD dwMainColor = 0;
		DWORD dwSubColor = 0;
		pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

		// overlay 효과가 안들어가있으면 염색 할 수 없다.
		if( 0 != dwMainColor )
			pCharPart->SetHairColor( tbColor.get< DWORD >( "dwMainColor" ) );

		if( 0 != dwSubColor )
			pCharPart->SetSubColor( tbColor.get< DWORD >( "dwSubColor" ) );
	}
}

LuaTable GLCharacter::GetCostumeColor()
{
	DWORD dwMainColor = 0;
	DWORD dwSubColor = 0;

	bool bMainEnable = true;
	bool bSubEnable = true;

	EMSLOT emCostumSelectSlot = GetCostumSelectSlot();

	PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );
	if( NULL != pCharPart )
		pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

	// 염색 불가능
	if( 0 == dwMainColor )
	{
		bMainEnable = false;
	}
	else
	{
		// 이미 염색 했었다면 염색한 색 가져오기
		if( 0 != m_PutOnItems[ emCostumSelectSlot ].dwMainColor )
			dwMainColor = m_PutOnItems[ emCostumSelectSlot ].dwMainColor;
	}

	// 염색 불가능
	if( 0 == dwSubColor )
	{
		bSubEnable = false;
	}
	else
	{
		// 이미 염색 했었다면 염색한 색 가져오기
		if( 0 != m_PutOnItems[ emCostumSelectSlot ].dwSubColor )
			dwSubColor = m_PutOnItems[ emCostumSelectSlot ].dwSubColor;
	}

	LuaTable tbColor( GLWidgetScript::GetInstance().GetLuaState() );
	tbColor.set( "dwMainColor", dwMainColor );
	tbColor.set( "bMainEnable", bMainEnable );
	tbColor.set( "dwSubColor", dwSubColor );
	tbColor.set( "bSubEnable", bSubEnable );

	return tbColor;
}

void GLCharacter::CommitCostumeColor( LuaTable tbColor )
{
	ReqInvenCostumColorChange( tbColor.get< DWORD >( "dwMainColor" ), tbColor.get< DWORD >( "dwSubColor" ) );
}

DWORD GLCharacter::GetStartMapID()
{
	return m_sStartMapID.dwID;
}

LuaTable GLCharacter::GetResurrectionCondition()
{
	bool bItemResurrect = NULL != FindRevive() ? true : false;
	bool bAutoResurrect = false;
	bool bCantUseResurrect = false;
	bool bGuildBattleMap = false;
	bool bCanRecoveryExp = m_pGaeaClient->CanRecoveryExp();

	//	귀혼주 쿨 타임 체크
	if( true == bItemResurrect ) 
	{
		SITEM* pItem = GET_SLOT_ITEMDATA( SLOT_DECORATION );
		if( NULL != pItem && true == IsCoolTime( pItem->sBasicOp.sNativeID ) )
			bItemResurrect = false;
	}

	if( false == IsRestartPossible() )
	{
		bItemResurrect = false;
		bCantUseResurrect = true;
	}

	if( true == bItemResurrect )
	{
		// 소모성 귀혼주이면 자동 귀혼주 사용
		SITEM* pItem = GET_SLOT_ITEMDATA( SLOT_DECORATION );
		if( NULL != pItem && false == pItem->ISINSTANCE() )
			bAutoResurrect = true;
	}

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if( NULL != pLand )
		bGuildBattleMap = pLand->m_bClubBattle || pLand->m_bClubDeathMatch;

	LuaTable tbCondition( GLWidgetScript::GetInstance().GetLuaState() );
	tbCondition.set( "bItemResurrect", bItemResurrect );
	tbCondition.set( "bAutoResurrect", bAutoResurrect );
	tbCondition.set( "bCantUseResurrect", bCantUseResurrect );
	tbCondition.set( "bGuildBattleMap", bGuildBattleMap );
	tbCondition.set( "bCanRecoveryExp", bCanRecoveryExp );
	
	return tbCondition;
}

LuaTable GLCharacter::GetRsurrectionExpRecovery()
{
	WORD wCharLev = GETLEVEL();
	LONGLONG llLevUpExp = GET_LEVELUP_EXP();

	LONGLONG llReExp = 0;
	LONGLONG llDecMoney = 0;
	LONGLONG llDecExp = LONGLONG( GLOGICEX::GLDIE_DECEXP( wCharLev ) * 0.01f * llLevUpExp );

	LONGLONG longExpLow = m_sExperience.lnLow;
	if( llDecExp > longExpLow )
		llDecExp = longExpLow;

	// 회복되는 경험치.
	if( llDecExp <= 0 )
		llReExp = 0;

	llReExp = (LONGLONG)( llDecExp * GLOGICEX::GLDIE_RECOVERYEXP( wCharLev ) );
	if( 0 > llReExp )
		llReExp = 0;

	if( llDecExp < llReExp )
		llReExp = llDecExp;

	LuaTable tbRecovery( GLWidgetScript::GetInstance().GetLuaState() );
	tbRecovery.set( "nDecExp", llDecExp );
	tbRecovery.set( "nReExp", llReExp );
	tbRecovery.set( "nMoney", LONGLONG( llReExp * GLOGICEX::GLDIE_EXPMONEY( m_wLevel ) ) );
	tbRecovery.set( "nRecoveryPercent",  (WORD)( GLOGICEX::GLDIE_RECOVERYEXP( wCharLev ) * 100 ) );

	return tbRecovery;
}

void GLCharacter::ResurrectCheckPoint()
{
	if( false == IsDie() )
		return;

	ReqResurrect( EM_RESURRECTION_NOMAL );
}

void GLCharacter::ResurrectExp()
{
	if( false == IsDie() )
		return;

	ReqResurrect( EM_RESURRECTION_SPECIAL );
}

void GLCharacter::ResurrectCurPos()
{
	if( false == IsDie() )
		return;

	ReqResurrect( EM_RESURRECTION_SPECIAL_ITEM );
}

void GLCharacter::ResurrectSkill()
{
	if( false == IsDie() )
		return;

	ReqResurrect( EM_RESURRECTION_SKILL );
}

LuaTable GLCharacter::GetQuestListProc()
{
	LuaTable tbQuestListData( GLWidgetScript::GetInstance().GetLuaState() ); //반환 되는 테이블
	LuaTable tbQuestAreaList( GLWidgetScript::GetInstance().GetLuaState() ); //영역 리스트			

	std::vector<std::string> _vecAreaList; //영역 비교 검사용(지역별)

	GLQuestPlay::MAPQUEST QuestProcMap = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = QuestProcMap.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestProcMap.end();

	for (; iter != iter_end; ++iter )
	{
		GLQUESTPROG* pQuestProg = (*iter).second;
		DWORD dwNID = pQuestProg->m_sNID.dwID;
		GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwNID );

		if ( !pQuest ) continue;			

		int Size = _vecAreaList.size();
		bool NoDuplication = true;

		for(int index = 0; index < Size; ++index)
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if(_vecAreaList[index] == CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", pQuest->m_dwQuestArea).GetString() )
					NoDuplication = false;
			}
			else
			{
				if(_vecAreaList[index] == COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea))
					NoDuplication = false;
			}
		}

		if(NoDuplication)
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strArea = CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", pQuest->m_dwQuestArea).GetString();
				_vecAreaList.push_back(strArea.GetBuffer());
			}
			else
				_vecAreaList.push_back(COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea).c_str());
		}

	}//퀘스트 영역들을 담음(중복검사까지 해서 중복 없이 벡터에 넣음)

	int Size = _vecAreaList.size();
	for(int index = 0; index < Size; ++index)
	{
		tbQuestAreaList.set(index + 1, _vecAreaList[index].c_str()); //영역
		tbQuestListData.set(_vecAreaList[index].c_str(), GetQuestDetailListProc(_vecAreaList[index])); //영역 안의 퀘스트
	}

	tbQuestListData.set("Area", tbQuestAreaList);
	return tbQuestListData;
}

LuaTable GLCharacter::GetQuestDetailListProc(std::string _AreaName)
{
	LuaTable tbQuest( GLWidgetScript::GetInstance().GetLuaState() ); 

	GLQuestPlay::MAPQUEST QuestProcMap = m_cQuestPlay.GetQuestProc();
	GLQuestPlay::MAPQUEST_ITER iter = QuestProcMap.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestProcMap.end();

	for (int i = 0 ; iter != iter_end; ++iter )
	{
		LuaTable tbQuestAreaDetailList( GLWidgetScript::GetInstance().GetLuaState() ); //영역 안의 퀘스트 리스트

		GLQUESTPROG* pQuestProg = (*iter).second;
		DWORD dwNID = pQuestProg->m_sNID.dwID;
		GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwNID );

		if ( !pQuest ) continue;			

		CString strCompare = GLUseFeatures::GetInstance().IsUsingDataIntegrate() ?
			CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", pQuest->m_dwQuestArea) : COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea).c_str();
		if( _AreaName == strCompare.GetString() )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);			
				CString strTitle = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
				tbQuestAreaDetailList.set(1, strTitle.GetBuffer());
			}
			else
				tbQuestAreaDetailList.set(1, pQuest->GetTITLE());

			bool bCheck(false);
			for ( DWORD i=0; i<RANPARAM::MAX_QUEST_DISPLAY; ++i )
			{
				if ( pQuest->m_sNID.dwID == RANPARAM::QuestDisplayID[ i ] )
				{
					bCheck = true;
				}
			}
			tbQuestAreaDetailList.set(2, pQuest->m_sNID.dwID);
			tbQuestAreaDetailList.set(3, bCheck);
		}

		tbQuest.set(i + 1, tbQuestAreaDetailList);
		++i;
	}

	return tbQuest;
}

LuaTable GLCharacter::GettbQuestProc( DWORD dwQuestID)
{
	LuaTable tbQuest( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbQuestData( GLWidgetScript::GetInstance().GetLuaState() );		
	LuaTable tbQuestCondition( GLWidgetScript::GetInstance().GetLuaState() );
	LuaTable tbQuestItem( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbQuestSkill( GLWidgetScript::GetInstance().GetLuaState() );	

	const DWORD dwMINITE_BY_HOUR = 60;
	
	GLQUESTPROG* pQuestProg = m_cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg )
		return tbQuest;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest )
		return tbQuest;

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );
	if( !pQuestStep )
		return tbQuest;

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;

	DWORD dwCount = pQuestProg->m_dwCOUNT;	

	if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		CString strContent;
		strKey.Format("QD_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		strContent =CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		tbQuestData.set( "Content", strContent.GetBuffer() );
	}
	else
	{
		tbQuestData.set( "Content", pQuest->m_strCOMMENT.c_str() );
	}//내용

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		CString strDetail;
		strKey.Format("QD_%03d_%03d_D_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
		strDetail = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		tbQuestData.set("Detail", strDetail.GetBuffer());
	}
	else
	{
		tbQuestData.set("Detail", pQuestStep->GetCOMMENT () );
	}//자세히

	GLQUEST::VECQSTEP& vecProgStep = pQuest->m_vecProgStep;
	for ( int i = 0; i < (int)vecProgStep.size(); ++i )
	{
		LuaTable tbQuestDetailData( GLWidgetScript::GetInstance().GetLuaState() );
		const GLQUEST_STEP& sQuestStep = vecProgStep[i];

		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			CString strStepTitle;
			strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
			strStepTitle = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
			tbQuestDetailData.set("StepTitle", strStepTitle.GetBuffer() );
		}
		else
		{
			tbQuestDetailData.set("StepTitle", sQuestStep.GetTITLE() );
		}

		if(i == (int)dwSTEP)
		{
			CString strCombine;		
			CString strConditionLine1, strConditionLine2;

			if ( pQuest->m_dwLimitTime )
			{
				const DWORD dwLimitTime = pQuest->m_dwLimitTime;
				const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
				const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

				const float fLapsTime = pQuestProg->m_fLAPSTIME;
				const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// 초->분단위
				const DWORD dwLapsHour = dwLapsTime / dwMINITE_BY_HOUR;
				const DWORD dwLapsMinute = dwLapsTime % dwMINITE_BY_HOUR;

				CString strLimitTime;
				if ( dwLimitHour )
				{
					strLimitTime.Format ( "%d%s %d%s",
						dwLimitHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
						dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
				}
				else
				{
					strLimitTime.Format ( "%d%s",
						dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
				}

				CString strLapsTime;
				if ( dwLapsHour )
				{
					strLapsTime.Format ( "%d%s %d%s",
						dwLapsHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
						dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
				}
				else
				{
					strLapsTime.Format ( "%d%s",
						dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
				}

				strConditionLine1.Format ( "%s : [%s/%s]", 
					ID2GAMEWORD("QUEST_MAIN_CONDITION",0), strLapsTime, strLimitTime );	
			}

			if ( pQuest->m_dwLimitPartyMemNum )
			{
				if ( strConditionLine1.GetLength() )
					strCombine.Format ( ", %s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
				else
					strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
				strConditionLine1 += strCombine;			
			}

			if ( strConditionLine1.GetLength() )
			{
				strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine1 );
				tbQuestDetailData.set("Condition1", strCombine.GetBuffer() );					
			}

			if ( pQuest->m_bNonDie )
			{
				strConditionLine2 = ID2GAMEWORD("QUEST_MAIN_CONDITION",2);
				if ( strConditionLine1.GetLength () )	//	윗 라인이 있을 경우
				{
					std::string strCondition = ID2GAMEWORD("QUEST_CONDITION");
					std::string strBlank ( strCondition.size(), ' ' );

					strCombine.Format ( "%s - %s", strBlank.c_str(), strConditionLine2 );				
				}
				else
				{
					strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine2 );				
				}

				tbQuestDetailData.set("Condition2", strCombine.GetBuffer() );					
			}

			if ( pQuestStep->IsNEED_NPCTALK() )
			{
				CString strCombine;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					SNATIVEID sNPCID = pQuestStep->m_dwNID_NPCTALK;
// 					CString strKey;
// 					strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
// 					CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
// 					CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 					CString strText;				
// 					strCombine.Format("%s \'%s\'%s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCrowName, strCommentText);
// 				}
// 				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_NPCTALK() );
				}

				tbQuestDetailData.set("NPCtalk", strCombine.GetBuffer());
			}

			if ( pQuestStep->IsNEED_QITEM() )
			{
				CString strCombine;
				LuaTable tbQuestItemIDList( GLWidgetScript::GetInstance().GetLuaState() );
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strCombine.Format( "%s %s"
// 						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 						, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBGEN_ITEM, CGameTextMan::EM_COMMENT_TEXT).GetString() );
// 				}
// 				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_MOBGEN_QITEM() );
				}
				
				int QItemSize(pQuestStep->m_vecMOBGEN_QITEM.size());
				for(int QItemIdx = 0; QItemIdx < QItemSize; ++QItemIdx)
				{
					LuaTable tbQuestItemID( GLWidgetScript::GetInstance().GetLuaState() );

					const GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList();
					GLInventory::CELL_MAP_CITER iter = ItemList->begin();
					GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

					DWORD dwCurNum(0);
					SNATIVEID dwItemID(pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID);
					for (; iter!=iter_end; ++iter)
					{
						SINVENITEM* pInvenItem = (*iter).second;				
						SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;

						if (ref_ItemCustom.GetNativeID() == dwItemID)
						{
							dwCurNum += ref_ItemCustom.wTurnNum;
						}
					}

					tbQuestItemID.set(1, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID.wMainID);
					tbQuestItemID.set(2, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID.wSubID);
					tbQuestItemID.set(3, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].wNUM);
					tbQuestItemID.set(4, dwCurNum);
					tbQuestItemIDList.set(QItemIdx + 1, tbQuestItemID);
				}				

				tbQuestDetailData.set("QItem", strCombine.GetBuffer());
				tbQuestDetailData.set("QItemID", tbQuestItemIDList);
			}			

			if ( pQuestStep->IsNEED_MOBKILL()|| pQuestStep->IsNEED_PLAYERKILL() )
			{
				CString strCombine;
				CString strCommentText;
// 				if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
// 				{
// 					strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				}
// 				else
				{
					strCommentText = pQuestStep->GetOBJ_MOBKILL();
				}
				if ( sSTEP_PROG.m_dwNUM_MOBKILL >= 0 )
				{
					strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString(),
						sSTEP_PROG.m_dwNUM_MOBKILL,
						pQuestStep->m_dwNUM_MOBKILL );
				}
				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString() );
				}

				tbQuestDetailData.set("Kill", strCombine.GetBuffer());
			}

			if ( pQuestStep->IsNEED_ITEMUSE() )
			{
				CString strCombine;
				CString strComment;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				}
// 				else
				{
					strComment = pQuestStep->GetOBJ_ITEMUSE();
				}
				if ( sSTEP_PROG.m_dwNUM_ITEMUSE >= 0 )
				{
					strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment,
						sSTEP_PROG.m_dwNUM_ITEMUSE,
						pQuestStep->m_dwNUM_ITEMUSE );
				}
				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment );
				}

				tbQuestDetailData.set("ItemUse", strCombine.GetBuffer());
			}

			if ( pQuestStep->IsNEED_ITEMGRINDING() )
			{
				CString strCombine;
				if ( sSTEP_PROG.m_dwNUM_ITEMGRINDING >= 0 )
				{
					strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str (),
						sSTEP_PROG.m_dwNUM_ITEMGRINDING,
						pQuestStep->m_dwNUM_ITEMGRINDING );
				}
				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str () );
				}

				tbQuestDetailData.set("ItemGrinding", strCombine.GetBuffer());
			}

			if ( pQuestStep->IsNEED_LEVELUP() )
			{
				CString strCombine;
				CString strComment;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString();
// 				}
// 				else
				{
					strComment = pQuestStep->GetOBJ_LEVEL_UP();
				}
				if ( sSTEP_PROG.m_bLEVELUP_ALL )
				{
					strCombine.Format( "%s %s [%d/%d]"
						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
						, strComment.GetString()
						, sSTEP_PROG.m_dwNUM_LEVELUP
						, pQuestStep->m_dwNUM_LEVELUP );
				}
				else
				{
					strCombine.Format( "%s %s"
						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
						, strComment.GetString() );
				}

				tbQuestDetailData.set("LevelUp", strCombine.GetBuffer());
			}

			if ( pQuestStep->IsNEED_CONFTING() )
			{
				CString strCombine;
				if ( sSTEP_PROG.m_dwNUM_CONFTING >= 0 )
				{
					strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str (),
						sSTEP_PROG.m_dwNUM_CONFTING,
						pQuestStep->m_dwNUM_CONFTING );
				}
				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str () );
				}

				tbQuestDetailData.set("Conft", strCombine.GetBuffer());				
			}

			if ( pQuestStep->IsNEED_NPCGUARD() )
			{
				CString strCombine;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strCombine.Format( "%s %s"
// 						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 						, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCQUARD, CGameTextMan::EM_COMMENT_TEXT).GetString() );
// 				}
// 				else
				{
					strCombine.Format( "%s %s"
						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
						, pQuestStep->GetOBJ_NPCGUARD() );
				}

				tbQuestDetailData.set("NpcGuard", strCombine.GetBuffer());		
			}

			if ( pQuestStep->IsNEED_REACHZONE() )
			{
				CString strCombine;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), 
// 						CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 				}
// 				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_REACH_ZONE() );
				}

				tbQuestDetailData.set("ReachZone", strCombine.GetBuffer());	
			}

			if ( pQuestStep->IsNEED_DEFENSEZONE() )
			{
				CString strCombine;
// 				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 				{
// 					strCombine.Format( "%s %s"
// 						, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
// 						, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_GUARD_MAP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString());
// 				}
// 				else
				{
					strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_DEFENSE_ZONE() );
				}

				tbQuestDetailData.set("Defence", strCombine.GetBuffer());
			}
		}

		tbQuestCondition.set(i + 1, tbQuestDetailData);
	}//진행사항

	tbQuestData.set("Exp", pQuest->m_lnGiftEXP);
	tbQuestData.set("Money", pQuest->m_dwGiftMONEY);
	tbQuestData.set("Element", pQuest->m_dwGiftELEMENT);
	tbQuestData.set("LifePoint", pQuest->m_dwGiftLIFEPOINT);
	tbQuestData.set("SkillPoint", pQuest->m_dwGiftSKILLPOINT);
	tbQuestData.set("StatPoint", pQuest->m_dwGiftSTATSPOINT);

	if ( pQuest->m_vecGiftITEM.size () || pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
	{
		if ( pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
		{
			int nday = m_pGaeaClient->GetRnAttendanceClient()->m_nDayCount;
			DWORD dwItemId = GLQuestMan::GetInstance().GetRnattendanceRewardNDay( nday ) ;

			if (dwItemId != UINT_MAX)
			{
				SNATIVEID sItemId( dwItemId );
				SITEMCUSTOM sItemCustom( sItemId );
				pQuest->m_vecGiftITEM.clear();
				pQuest->m_vecGiftITEM.push_back(sItemCustom);
			}
		}

		INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
		INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();			

		for (int i = 0 ; iter != iter_end; iter++ )
		{
			LuaTable tbItemID( GLWidgetScript::GetInstance().GetLuaState() );	
			SITEMCUSTOM& sItemCustom = (*iter);
			tbItemID.set(1, sItemCustom.GetNativeID().wMainID);
			tbItemID.set(2, sItemCustom.GetNativeID().wSubID);
			tbItemID.set(3, sItemCustom.TurnNum() );
			tbQuestItem.set(i + 1, tbItemID);		
			++i;
		}		
	}

	if ( pQuest->m_vecGiftSKILL.size () )
	{
		DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin ();
		DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end ();
		for (int i = 0 ; iter != iter_end; )
		{
			DWORD dwID = (*iter);			
			tbQuestSkill.set(i + 1, dwID);
			++i;
		}
	}//퀘스트 보상

	tbQuest.set("Item", tbQuestItem);
	tbQuest.set("Skill", tbQuestSkill);
	tbQuest.set("Cond", tbQuestCondition);
	tbQuest.set("Data", tbQuestData);

	return tbQuest;
}


LuaTable GLCharacter::GettbQuestProcInfo( DWORD dwQuestID)
{
	LuaTable tbQuest( GLWidgetScript::GetInstance().GetLuaState() );		
	LuaTable tbQuestItemIDList( GLWidgetScript::GetInstance().GetLuaState() );
	const DWORD dwMINITE_BY_HOUR = 60;

	GLQUESTPROG* pQuestProg = m_cQuestPlay.FindProc ( dwQuestID );
	if ( !pQuestProg )
		return tbQuest;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest )
		return tbQuest;

	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );
	if( !pQuestStep )
		return tbQuest;

	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;

	LuaTable tbQuestDetailData( GLWidgetScript::GetInstance().GetLuaState() );
	//const GLQUEST_STEP& sQuestStep = pQuestStep;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		CString strStepTitle;
		strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
		strStepTitle = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		tbQuestDetailData.set("StepTitle", strStepTitle.GetBuffer() );
	}
	else
	{
		tbQuestDetailData.set("StepTitle", pQuestStep->GetTITLE() );
	}	

	CString strCombine;		
	CString strConditionLine1, strConditionLine2;

	if ( pQuest->m_dwLimitTime )
	{
		const DWORD dwLimitTime = pQuest->m_dwLimitTime;
		const DWORD dwLimitHour = dwLimitTime / dwMINITE_BY_HOUR;
		const DWORD dwLimitMinute = dwLimitTime % dwMINITE_BY_HOUR;

		const float fLapsTime = pQuestProg->m_fLAPSTIME;
		const DWORD dwLapsTime = ((DWORD)fLapsTime) / dwMINITE_BY_HOUR;	// 초->분단위
		const DWORD dwLapsHour = dwLapsTime / dwMINITE_BY_HOUR;
		const DWORD dwLapsMinute = dwLapsTime % dwMINITE_BY_HOUR;

		CString strLimitTime;
		if ( dwLimitHour )
		{
			strLimitTime.Format ( "%d%s %d%s",
				dwLimitHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
				dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
		}
		else
		{
			strLimitTime.Format ( "%d%s",
				dwLimitMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
		}

		CString strLapsTime;
		if ( dwLapsHour )
		{
			strLapsTime.Format ( "%d%s %d%s",
				dwLapsHour, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",0),	// 2, 3
				dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
		}
		else
		{
			strLapsTime.Format ( "%d%s",
				dwLapsMinute, ID2GAMEWORD("QUEST_CONDITION_TIME_UNIT",1) );
		}

		strConditionLine1.Format ( "%s : [%s/%s]", 
			ID2GAMEWORD("QUEST_MAIN_CONDITION",0), strLapsTime, strLimitTime );	
	}

	if ( pQuest->m_dwLimitPartyMemNum )
	{
		if ( strConditionLine1.GetLength() )
			strCombine.Format ( ", %s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
		else
			strCombine.Format ( "%s : %d", ID2GAMEWORD("QUEST_MAIN_CONDITION",1), pQuest->m_dwLimitPartyMemNum );
		strConditionLine1 += strCombine;			
	}

	if ( strConditionLine1.GetLength() )
	{
		strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine1 );
		tbQuestDetailData.set("Condition1", strCombine.GetBuffer() );					
	}

	if ( pQuest->m_bNonDie )
	{
		strConditionLine2 = ID2GAMEWORD("QUEST_MAIN_CONDITION",2);
		if ( strConditionLine1.GetLength () )	//	윗 라인이 있을 경우
		{
			std::string strCondition = ID2GAMEWORD("QUEST_CONDITION");
			std::string strBlank ( strCondition.size(), ' ' );

			strCombine.Format ( "%s - %s", strBlank.c_str(), strConditionLine2 );				
		}
		else
		{
			strCombine.Format ( "%s - %s", ID2GAMEWORD("QUEST_CONDITION"), strConditionLine2 );				
		}

		tbQuestDetailData.set("Condition2", strCombine.GetBuffer() );					
	}

	if ( pQuestStep->IsNEED_NPCTALK() )
	{
		CString strCombine;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			SNATIVEID sNPCID = pQuestStep->m_dwNID_NPCTALK;
			CString strKey;
			strKey.Format("CN_%03d_%03d", sNPCID.Mid(), sNPCID.Sid());
			CString strCrowName		= CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_CROW_TEXT).GetString();
			CString strCommentText	= CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCTALK, CGameTextMan::EM_COMMENT_TEXT).GetString();
			CString strText;				
			strCombine.Format("%s \'%s\'%s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCrowName, strCommentText);
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_NPCTALK() );
		}

		tbQuestDetailData.set("NPCtalk", strCombine.GetBuffer());
	}

	if ( pQuestStep->IsNEED_QITEM() )
	{
		CString strCombine;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strCombine.Format( "%s %s"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBGEN_ITEM, CGameTextMan::EM_COMMENT_TEXT).GetString() );
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_MOBGEN_QITEM() );
		}

		int QItemSize(pQuestStep->m_vecMOBGEN_QITEM.size());
		for(int QItemIdx = 0; QItemIdx < QItemSize; ++QItemIdx)
		{			
			LuaTable tbQuestItemID( GLWidgetScript::GetInstance().GetLuaState() );

			const GLInventory::CELL_MAP *ItemList = pQuestProg->m_sINVENTORY.GetItemList();
			GLInventory::CELL_MAP_CITER iter = ItemList->begin();
			GLInventory::CELL_MAP_CITER iter_end = ItemList->end();

			DWORD dwCurNum(0);
			SNATIVEID dwItemID(pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID);
			for (; iter!=iter_end; ++iter)
			{
				SINVENITEM* pInvenItem = (*iter).second;				
				SITEMCUSTOM &ref_ItemCustom = pInvenItem->sItemCustom;

				if (ref_ItemCustom.GetNativeID() == dwItemID)
				{
					dwCurNum += ref_ItemCustom.wTurnNum;
				}
			}

			tbQuestItemID.set(1, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID.wMainID);
			tbQuestItemID.set(2, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].sNID.wSubID);
			tbQuestItemID.set(3, pQuestStep->m_vecMOBGEN_QITEM[QItemIdx].wNUM);		
			tbQuestItemID.set(4, dwCurNum);		
			
			tbQuestItemIDList.set(QItemIdx + 1, tbQuestItemID);
		}
		tbQuestDetailData.set("QItem", strCombine.GetBuffer());
		tbQuestDetailData.set("QItemID", tbQuestItemIDList);
	}



	if ( pQuestStep->IsNEED_MOBKILL()|| pQuestStep->IsNEED_PLAYERKILL() )
	{
		CString strCombine;
		CString strCommentText;
		if (GLUseFeatures::GetInstance().IsUsingDataIntegrate())
		{
			strCommentText = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_MOBKILL, CGameTextMan::EM_COMMENT_TEXT).GetString();
		}
		else
		{
			strCommentText = pQuestStep->GetOBJ_MOBKILL();
		}
		if ( sSTEP_PROG.m_dwNUM_MOBKILL >= 0 )
		{
			strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString(),
				sSTEP_PROG.m_dwNUM_MOBKILL,
				pQuestStep->m_dwNUM_MOBKILL );
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strCommentText.GetString() );
		}

		tbQuestDetailData.set("Kill", strCombine.GetBuffer());
	}

	if ( pQuestStep->IsNEED_ITEMUSE() )
	{
		CString strCombine;
		CString strComment;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_ITEM_USE, CGameTextMan::EM_COMMENT_TEXT).GetString();
		}
		else
		{
			strComment = pQuestStep->GetOBJ_ITEMUSE();
		}
		if ( sSTEP_PROG.m_dwNUM_ITEMUSE >= 0 )
		{
			strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment,
				sSTEP_PROG.m_dwNUM_ITEMUSE,
				pQuestStep->m_dwNUM_ITEMUSE );
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), strComment );
		}

		tbQuestDetailData.set("ItemUse", strCombine.GetBuffer());
	}

	if ( pQuestStep->IsNEED_ITEMGRINDING() )
	{
		CString strCombine;
		if ( sSTEP_PROG.m_dwNUM_ITEMGRINDING >= 0 )
		{
			strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str (),
				sSTEP_PROG.m_dwNUM_ITEMGRINDING,
				pQuestStep->m_dwNUM_ITEMGRINDING );
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_ITEMGRINDING.c_str () );
		}

		tbQuestDetailData.set("ItemGrinding", strCombine.GetBuffer());
	}

	if ( pQuestStep->IsNEED_LEVELUP() )
	{
		CString strCombine;
		CString strComment;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strComment = CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_LEVEL_UP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString();
		}
		else
		{
			strComment = pQuestStep->GetOBJ_LEVEL_UP();
		}
		if ( sSTEP_PROG.m_bLEVELUP_ALL )
		{
			strCombine.Format( "%s %s [%d/%d]"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, strComment.GetString()
				, sSTEP_PROG.m_dwNUM_LEVELUP
				, pQuestStep->m_dwNUM_LEVELUP );
		}
		else
		{
			strCombine.Format( "%s %s"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, strComment.GetString() );
		}

		tbQuestDetailData.set("LevelUp", strCombine.GetBuffer());
	}

	if ( pQuestStep->IsNEED_CONFTING() )
	{
		CString strCombine;
		if ( sSTEP_PROG.m_dwNUM_CONFTING >= 0 )
		{
			strCombine.Format( "%s %s [%d/%d]", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str (),
				sSTEP_PROG.m_dwNUM_CONFTING,
				pQuestStep->m_dwNUM_CONFTING );
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->m_strOBJ_CONFTING.c_str () );
		}

		tbQuestDetailData.set("Conft", strCombine.GetBuffer());				
	}

	if ( pQuestStep->IsNEED_NPCGUARD() )
	{
		CString strCombine;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strCombine.Format( "%s %s"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_NPCQUARD, CGameTextMan::EM_COMMENT_TEXT).GetString() );
		}
		else
		{
			strCombine.Format( "%s %s"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, pQuestStep->GetOBJ_NPCGUARD() );
		}

		tbQuestDetailData.set("NpcGuard", strCombine.GetBuffer());		
	}

	if ( pQuestStep->IsNEED_REACHZONE() )
	{
		CString strCombine;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), 
				CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_REACH_ZONE, CGameTextMan::EM_COMMENT_TEXT).GetString());
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_REACH_ZONE() );
		}

		tbQuestDetailData.set("ReachZone", strCombine.GetBuffer());	
	}

	if ( pQuestStep->IsNEED_DEFENSEZONE() )
	{
		CString strCombine;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			strCombine.Format( "%s %s"
				, ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 )
				, CGameTextMan::GetInstance().GetText("QUEST_STEP_COMMENT_TEXT", EMQSCT_GUARD_MAP_COMMENT, CGameTextMan::EM_COMMENT_TEXT).GetString());
		}
		else
		{
			strCombine.Format( "%s %s", ID2GAMEWORD( "QUEST_LIST_STEP_PLUS", 0 ), pQuestStep->GetOBJ_DEFENSE_ZONE() );
		}

		tbQuestDetailData.set("Defence", strCombine.GetBuffer());
	}


	tbQuest.set("Cond", tbQuestDetailData); //진행사항

	return tbQuest;
}


LuaTable GLCharacter::GetQuestListEnd()
{
	LuaTable tbQuestListData( GLWidgetScript::GetInstance().GetLuaState() ); //반환 되는 테이블
	LuaTable tbQuestAreaList( GLWidgetScript::GetInstance().GetLuaState() ); //영역 리스트			

	std::vector<std::string> _vecAreaList; //영역 비교 검사용(지역별)

	GLQuestPlay::MAPQUEST QuestEndMap = m_cQuestPlay.GetQuestEnd();
	GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end();

	for (; iter != iter_end; ++iter )
	{
		GLQUESTPROG* pQuestProg = (*iter).second;
		DWORD dwNID = pQuestProg->m_sNID.dwID;
		GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwNID );

		if ( !pQuest ) continue;			

		int Size = _vecAreaList.size();
		bool NoDuplication = true;

		for(int index = 0; index < Size; ++index)
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				if(_vecAreaList[index] == CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", pQuest->m_dwQuestArea).GetString() )
					NoDuplication = false;
			}
			else
			{
				if(_vecAreaList[index] == COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea))
					NoDuplication = false;
			}
		}

		if(NoDuplication)
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strArea = CGameTextMan::GetInstance().GetCommentText("QUEST_AREA",pQuest->m_dwQuestArea).GetString();
				_vecAreaList.push_back(strArea.GetBuffer());
			}			
			else
				_vecAreaList.push_back(COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea).c_str());
		}

	}//퀘스트 영역들을 담음(중복검사까지 해서 중복 없이 벡터에 넣음)

	int Size = _vecAreaList.size();
	for(int index = 0; index < Size; ++index)
	{
		tbQuestAreaList.set(index + 1, _vecAreaList[index].c_str()); //영역
		tbQuestListData.set(_vecAreaList[index].c_str(), GetQuestDetailListEnd(_vecAreaList[index])); //영역 안의 퀘스트
	}

	tbQuestListData.set("Area", tbQuestAreaList);

	return tbQuestListData;
}

LuaTable GLCharacter::GetQuestDetailListEnd(std::string _AreaName)
{
	LuaTable tbQuest( GLWidgetScript::GetInstance().GetLuaState() ); 	

	GLQuestPlay::MAPQUEST QuestEndMap = m_cQuestPlay.GetQuestEnd();
	GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end();

	for(int i = 0 ; iter != iter_end; ++iter )
	{
		LuaTable tbQuestAreaDetailList( GLWidgetScript::GetInstance().GetLuaState() ); //영역 안의 퀘스트 리스트

		GLQUESTPROG* pQuestProg = (*iter).second;
		DWORD dwNID = pQuestProg->m_sNID.dwID;
		GLQUEST* pQuest = GLQuestMan::GetInstance().Find( dwNID );

		if ( !pQuest ) continue;			

		CString strCompare = GLUseFeatures::GetInstance().IsUsingDataIntegrate() ?
			CGameTextMan::GetInstance().GetCommentText("QUEST_AREA", pQuest->m_dwQuestArea) : COMMENT::QUEST_AREA_NAME.at(pQuest->m_dwQuestArea).c_str();
		if ( _AreaName == strCompare.GetString() )
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);			

				CString strArea = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
				tbQuestAreaDetailList.set(1, strArea.GetBuffer());
			}
			else
				tbQuestAreaDetailList.set(1, pQuest->GetTITLE());
				
			tbQuestAreaDetailList.set(2, pQuest->m_sNID.dwID);
		}

		tbQuest.set(i + 1, tbQuestAreaDetailList);
		++i;
	}

	return tbQuest;
}

LuaTable GLCharacter::GettbQuestEnd( DWORD dwQuestID)
{
	LuaTable tbQuest( GLWidgetScript::GetInstance().GetLuaState() );		
	LuaTable tbQuestItem( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbQuestSkill( GLWidgetScript::GetInstance().GetLuaState() );	
	LuaTable tbQuestData( GLWidgetScript::GetInstance().GetLuaState() );		
	LuaTable tbQuestStepText( GLWidgetScript::GetInstance().GetLuaState() );		

	GLQuestPlay::MAPQUEST QuestEndMap = m_cQuestPlay.GetQuestEnd ();
	GLQuestPlay::MAPQUEST_ITER iter = QuestEndMap.find( dwQuestID );
	GLQuestPlay::MAPQUEST_ITER iter_end = QuestEndMap.end ();
	GLQUESTPROG* pQuestProg = (*iter).second;

	if ( !pQuestProg ) 
		return tbQuest;
	
	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( dwQuestID );
	if ( !pQuest ) 
		return tbQuest;	
	
	const DWORD dwSTEP = pQuestProg->m_dwSTEP;
	GLQUEST_STEP* pQuestStep = pQuest->GetSTEP ( dwSTEP );
	GLQUESTPROG_STEP& sSTEP_PROG = pQuestProg->m_sSTEP_PROG;	

	bool bComplete = pQuestProg->m_bCOMPLETE;
	DWORD dwCount = pQuestProg->m_dwCOUNT;
	if ( bComplete )
	{
		CString strCombine;
		strCombine.Format( "%s / %s : %d", ID2GAMEWORD( "QUEST_LIST_RESULT", 1 )
			, ID2GAMEWORD( "QUEST_LIST_RESULT", 3 ), dwCount );

		tbQuestData.set("Result", strCombine.GetBuffer() );
	}
	else
	{
		CString strCombine;
		strCombine.Format( "%s / %s : %d", ID2GAMEWORD( "QUEST_LIST_RESULT", 2 )
			, ID2GAMEWORD( "QUEST_LIST_RESULT", 3 ), dwCount );

		tbQuestData.set("Result", strCombine.GetBuffer() );
	}//수행결과

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		CString strContent;
		strKey.Format("QD_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
		strContent = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		tbQuestData.set("Content" , strContent.GetBuffer() );
	}
	else
	{
		tbQuestData.set( "Content", pQuest->m_strCOMMENT.c_str() );
	}//내용

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CString strKey;
		CString strDetail;
		strKey.Format("QD_%03d_%03d_D_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, dwSTEP);
		strDetail = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
		tbQuestData.set( "Detail", strDetail.GetBuffer());
	}
	else
	{
		if ( pQuestStep )
			tbQuestData.set( "Detail", pQuestStep->GetCOMMENT () );
		else
			tbQuestData.set( "Detail", "" );
	}//자세히

	GLQUEST::VECQSTEP& vecProgStep = pQuest->m_vecProgStep;
	for ( int i = 0; i < (int)vecProgStep.size(); ++i )
	{
		const GLQUEST_STEP& sQuestStep = vecProgStep[i];

		if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			CString strStepText;
			strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, i);
			strStepText = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
			tbQuestStepText.set(i + 1, strStepText.GetBuffer());
		}
		else
			tbQuestStepText.set(i + 1, sQuestStep.GetTITLE() );
	}

	tbQuestData.set("StepTitle", tbQuestStepText);
	tbQuestData.set("Exp", pQuest->m_lnGiftEXP);
	tbQuestData.set("Money", pQuest->m_dwGiftMONEY);
	tbQuestData.set("Element", pQuest->m_dwGiftELEMENT);
	tbQuestData.set("LifePoint", pQuest->m_dwGiftLIFEPOINT);
	tbQuestData.set("SkillPoint", pQuest->m_dwGiftSKILLPOINT);
	tbQuestData.set("StatPoint", pQuest->m_dwGiftSTATSPOINT);

	if ( pQuest->m_vecGiftITEM.size () || pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
	{
		if ( pQuest->m_sSTARTOPT.nJOIN == EMQJ_ATTENDANCE )
		{
			int nday = m_pGaeaClient->GetRnAttendanceClient()->m_nDayCount; // 임시 코드 수정 예정 2014.7.19 bjkim
			DWORD dwItemId = GLQuestMan::GetInstance().GetRnattendanceRewardNDay( nday ) ;

			if (dwItemId != UINT_MAX)
			{
				SNATIVEID sItemId( dwItemId );
				SITEMCUSTOM sItemCustom( sItemId );
				pQuest->m_vecGiftITEM.clear();
				pQuest->m_vecGiftITEM.push_back(sItemCustom);
			}
		}

		INVENQARRAY_ITER iter = pQuest->m_vecGiftITEM.begin ();
		INVENQARRAY_ITER iter_end = pQuest->m_vecGiftITEM.end ();			
		
		for (int i = 0 ; iter != iter_end; iter++ )
		{
			LuaTable tbItemID( GLWidgetScript::GetInstance().GetLuaState() );	
			SITEMCUSTOM& sItemCustom = (*iter);
			tbItemID.set(1, sItemCustom.GetNativeID().wMainID);
			tbItemID.set(2, sItemCustom.GetNativeID().wSubID);
			tbItemID.set(3, sItemCustom.TurnNum());
			tbQuestItem.set(i + 1, tbItemID);		
			++i;
		}		
	}

	if ( pQuest->m_vecGiftSKILL.size () )
	{
		DWQARRAY_ITER iter = pQuest->m_vecGiftSKILL.begin ();
		DWQARRAY_ITER iter_end = pQuest->m_vecGiftSKILL.end ();
		for (int i = 0 ; iter != iter_end; )
		{
			DWORD dwID = (*iter);		
			tbQuestSkill.set(i + 1, dwID);
			++i;
		}
	}

	tbQuest.set("Data", tbQuestData);
	tbQuest.set("Item", tbQuestItem);
	tbQuest.set("Skill", tbQuestSkill);		
	
	return tbQuest;
}

bool GLCharacter::GetQuestComplete(DWORD _dwID, bool ProcOrEnd)
{
	if(ProcOrEnd)
	{
		GLQUESTPROG* pQuest = m_cQuestPlay.FindProc(_dwID);
		return pQuest->m_bCOMPLETE;
	}
	else
	{
		GLQUESTPROG* pQuest = m_cQuestPlay.FindEnd(_dwID);
		return pQuest->m_bCOMPLETE;
	}
}

DWORD GLCharacter::GetQuestStep(DWORD _dwID)
{
	if(_dwID <= 0)
		return 0;

	GLQUESTPROG* pQuest = m_cQuestPlay.FindProc(_dwID);
	return pQuest->m_dwSTEP;
}

bool GLCharacter::GetQuestCompleteCheck(DWORD _dwID)
{
	if(_dwID <= 0)
		return false;

	GLQUESTPROG* pQuestProg = m_cQuestPlay.FindProc(_dwID);
	return pQuestProg->CheckCOMPLETE();
}

bool GLCharacter::GetQuestGiveUp(DWORD _dwID)
{
	if(_dwID <= 0)
		return false;

	GLQUEST* pQuest = GLQuestMan::GetInstance().Find ( _dwID );
	return pQuest->IsGIVEUP();
}

void GLCharacter::SetQuestHelpCheck(DWORD _dwID, bool _bCheck)
{
	if(_bCheck)
	{
		for ( DWORD i=0; i<RANPARAM::MAX_QUEST_DISPLAY; ++i )
		{
			if ( NATIVEID_NULL().dwID == RANPARAM::QuestDisplayID[ i ] )
			{
				RANPARAM::QuestDisplayID[ i ] = _dwID;
				return;
			}
		}
	}
	else
	{
		for ( DWORD i=0; i<RANPARAM::MAX_QUEST_DISPLAY; ++i )
		{
			if ( _dwID == RANPARAM::QuestDisplayID[ i ] )
			{
				RANPARAM::QuestDisplayID[ i ] = -1;
				return;
			}
		}
	}
}

void GLCharacter::AutoArmSwap()
{
	if( true == IsReqSwapArms() )
		return;

	if( GLCC_EXTREME_M != m_emClass && GLCC_EXTREME_W != m_emClass )
		return;

	SNATIVEID sSkillID = GetskillRunSlot();
	if( sSkillID == NATIVEID_NULL() )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSkillID.wMainID, sSkillID.wSubID );
	if( NULL == pSkill )
		return;
	if( SKILL::EMROLE_PASSIVE == pSkill->m_sBASIC.emROLE )
		return;

	//	Note : 아이템 착용 조건 검사.
	EMSLOT emRHand, emRHand_S;
	if( TRUE == IsUseArmSub() )
	{
		emRHand = SLOT_RHAND_S;
		emRHand_S = SLOT_RHAND;
	}
	else
	{
		emRHand = SLOT_RHAND;
		emRHand_S = SLOT_RHAND_S;
	}

	const bool bHiddenWeapon = pSkill->m_sLEARN.bHiddenWeapon;
	GLITEM_ATT emITEM_RIGHT = ITEMATT_NOTHING;
	SKILL::GLSKILL_ATT emSKILL_RIGHT = pSkill->m_sBASIC.emUSE_RITEM;

	if( SKILL::SKILLATT_NOCARE != emSKILL_RIGHT )
	{
		SITEM* pItem = GET_SLOT_ITEMDATA( emRHand );

		// 아이템이 없거나 스킬 구동 조건 아이템이 아닐때 
		if( NULL == pItem || 
			false == CHECHSKILL_ITEM( emSKILL_RIGHT, CONVERT_ITEMATT( pItem->sSuitOp.emAttack ), bHiddenWeapon ) )
		{
			// Sub무기를 체크해서 맞을경우 스왑한다.
			SITEM* pItem_S = GET_SLOT_ITEMDATA( emRHand_S );
			if( NULL != pItem_S && 
				true == CHECHSKILL_ITEM( emSKILL_RIGHT, CONVERT_ITEMATT( pItem_S->sSuitOp.emAttack ), bHiddenWeapon ) )
				ReqSlotChange();
		}
	}
}

LuaTable GLCharacter::TempPutOnStatInvenItem( WORD wPosX, WORD wPosY, WORD wVirtualPow, WORD wVirtualDex,
											  WORD wVirtualSpi, WORD wVirtualStr, WORD wVirtualSta )
{
	LuaTable tbStat( GLWidgetScript::GetInstance().GetLuaState() );

	const SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem || 
		NATIVEID_NULL() == pInvenItem->GetNativeID() || 
		true == pInvenItem->CustomItem().IsBoxWrapped() )
		return tbStat;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItem || false == pItem->IsSuitable() )
		return tbStat;

	if( false == (pItem->sBasicOp.dwReqCharClass & m_emClass) )
		return tbStat;

	GLCHARLOGIC* pCharOrigData = &GetCharLogic();

	GLCHARLOGIC* pVirtualCharData = new GLCHARLOGIC;
	if ( NULL == pVirtualCharData )
	{
		SAFE_DELETE( pVirtualCharData );
		return tbStat;
	}

	pVirtualCharData->Assign( *pCharOrigData );
	//*pVirtualCharData = *pCharOrigData;

	pVirtualCharData->m_sStats.wPow += wVirtualPow;
	pVirtualCharData->m_sStats.wDex += wVirtualDex;
	pVirtualCharData->m_sStats.wSpi += wVirtualSpi;
	pVirtualCharData->m_sStats.wStr += wVirtualStr;
	pVirtualCharData->m_sStats.wSta += wVirtualSta;

	for( int i = 0; i < SLOT_TSIZE; ++i )
	{
		if( EMSLOTCHECK_OK == CHECKSLOT_ITEM( pInvenItem->GetNativeID(), EMSLOT( i ) ) )
		{
			// 반대쪽 슬롯 비어있으면 반대쪽에 넣자.
			switch( i )
			{
			case SLOT_RFINGER :
				{
					if( false == pVirtualCharData->m_PutOnItems[ SLOT_LFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_LFINGER;
					// 희한한게 비교는 오른쪽에다 하고 착용은 왼쪽에다 한다.
					// 오른쪽 슬롯도 착용중이면 왼쪽에 넣자.
					else if( true == pVirtualCharData->m_PutOnItems[ SLOT_RFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_LFINGER;
				}
				break;

			case SLOT_LFINGER :
				{
					if( false == pVirtualCharData->m_PutOnItems[ SLOT_RFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_RFINGER;
				}
				break;

			case SLOT_RACCESSORIES :
				{
					if( false == pVirtualCharData->m_PutOnItems[ SLOT_LACCESSORIES ].GetNativeID().IsValidNativeID() )
						i = SLOT_LACCESSORIES;
				}
				break;

			case SLOT_LACCESSORIES :
				{
					if( false == pVirtualCharData->m_PutOnItems[ SLOT_RACCESSORIES ].GetNativeID().IsValidNativeID() )
						i = SLOT_RACCESSORIES;
				}
				break;
			}

			pVirtualCharData->m_PutOnItems[ i ] = pInvenItem->sItemCustom;
			break;
		}
	}

	//BYTE   nDurabilityApply[DURABILITY_APPLY_COUNT] = {  1, 1, 1, 1, 1, 1  };

	// 백업
	//for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
	//{
	//	nDurabilityApply[i] = GLCONST_CHAR::nDurabilityApply[i];
	//
	//	GLCONST_CHAR::nDurabilityApply[i] = 0;
	//}

	pVirtualCharData->SET_VALIDATION_EXPSKILL_MAP();
	pVirtualCharData->INIT_DATA( FALSE, FALSE );
	pVirtualCharData->UPDATE_DATA( 1, 1 );

	// 복구
	//for( int i = 0; i < DURABILITY_APPLY_COUNT; ++i )
	//{
	//	GLCONST_CHAR::nDurabilityApply[i] = nDurabilityApply[i];
	//}

	tbStat.set( 1, pVirtualCharData->GETFORCE_LOW() - pCharOrigData->GETFORCE_LOW() );
	tbStat.set( 2, pVirtualCharData->GETFORCE_HIGH() - pCharOrigData->GETFORCE_HIGH() );
	tbStat.set( 3, pVirtualCharData->GETDEFENSE() - pCharOrigData->GETDEFENSE() );
	tbStat.set( 4, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_MELEE ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_MELEE ] );
	tbStat.set( 5, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_RANGE ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_RANGE ] );
	tbStat.set( 6, pVirtualCharData->m_sumPowerAttribute[ SKILL::EMAPPLY_MAGIC ] - 
		pCharOrigData->m_sumPowerAttribute[ SKILL::EMAPPLY_MAGIC ] );
	tbStat.set( 7, pVirtualCharData->GETHIT() - pCharOrigData->GETHIT() );
	tbStat.set( 8, pVirtualCharData->GETAVOID() - pCharOrigData->GETAVOID() );
	tbStat.set( 9, pVirtualCharData->GETMAXHP() - pCharOrigData->GETMAXHP() );
	tbStat.set( 10, pVirtualCharData->GETMAXMP() - pCharOrigData->GETMAXMP() );
	tbStat.set( 11, pVirtualCharData->GETMAXSP() - pCharOrigData->GETMAXSP() );
/*
	tbStat.set( 12, apCharChangeData->GETCRITICAL_TOTAL() - pCharOrigData->GETCRITICAL_TOTAL() );
	tbStat.set( 13, apCharChangeData->m_sSUMITEM.fIncR_CrushingBlow - pCharOrigData->m_sSUMITEM.fIncR_CrushingBlow );
	tbStat.set( 14, apCharChangeData->GETMOVE_TOTAL() - pCharOrigData->GETMOVE_TOTAL() );
	tbStat.set( 15, apCharChangeData->GETATT_TOTAL() - pCharOrigData->GETATT_TOTAL() );
	tbStat.set( 16, apCharChangeData->GETEXPVELO() - pCharOrigData->GETEXPVELO() );
	tbStat.set( 17, apCharChangeData->m_fVariationRateHP - pCharOrigData->m_fVariationRateHP );
	tbStat.set( 18, apCharChangeData->m_fVariationRateMP - pCharOrigData->m_fVariationRateMP );
	tbStat.set( 19, apCharChangeData->m_fVariationRateSP - pCharOrigData->m_fVariationRateSP );
	tbStat.set( 20, apCharChangeData->m_sSUMITEM.nReceive_CP_Inc - pCharOrigData->m_sSUMITEM.nReceive_CP_Inc );
	tbStat.set( 21, (apCharChangeData->m_sSUMITEM.nPotion_HP_Inc + apCharChangeData->m_nPotion_HP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_HP_Inc + pCharOrigData->m_nPotion_HP_Inc_SKILL) );
	tbStat.set( 22, (apCharChangeData->m_sSUMITEM.nPotion_MP_Inc + apCharChangeData->m_nPotion_MP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_MP_Inc + pCharOrigData->m_nPotion_MP_Inc_SKILL) );
	tbStat.set( 23, (apCharChangeData->m_sSUMITEM.nPotion_SP_Inc + apCharChangeData->m_nPotion_SP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_SP_Inc + pCharOrigData->m_nPotion_SP_Inc_SKILL) );
*/
	tbStat.set( 24, (pVirtualCharData->m_sStats.wPow + pVirtualCharData->m_sSUMITEM.sStats.wPow) - 
		(pCharOrigData->m_sStats.wPow + pCharOrigData->m_sSUMITEM.sStats.wPow) );
	tbStat.set( 25, (pVirtualCharData->m_sStats.wDex + pVirtualCharData->m_sSUMITEM.sStats.wDex) - 
		(pCharOrigData->m_sStats.wDex + pCharOrigData->m_sSUMITEM.sStats.wDex) );
	tbStat.set( 26, (pVirtualCharData->m_sStats.wSpi + pVirtualCharData->m_sSUMITEM.sStats.wSpi) - 
		(pCharOrigData->m_sStats.wSpi + pCharOrigData->m_sSUMITEM.sStats.wSpi) );
	tbStat.set( 27, (pVirtualCharData->m_sStats.wStr + pVirtualCharData->m_sSUMITEM.sStats.wStr) - 
		(pCharOrigData->m_sStats.wStr + pCharOrigData->m_sSUMITEM.sStats.wStr) );
	tbStat.set( 28, (pVirtualCharData->m_sStats.wSta + pVirtualCharData->m_sSUMITEM.sStats.wSta) - 
		(pCharOrigData->m_sStats.wSta + pCharOrigData->m_sSUMITEM.sStats.wSta) );
/*
	tbStat.set( 29, apCharChangeData->m_sSUMRESIST_SKILL.nFire - pCharOrigData->m_sSUMRESIST_SKILL.nFire );
	tbStat.set( 30, apCharChangeData->m_sSUMRESIST_SKILL.nIce - pCharOrigData->m_sSUMRESIST_SKILL.nIce );
	tbStat.set( 31, apCharChangeData->m_sSUMRESIST_SKILL.nElectric - pCharOrigData->m_sSUMRESIST_SKILL.nElectric );
	tbStat.set( 32, apCharChangeData->m_sSUMRESIST_SKILL.nPoison - pCharOrigData->m_sSUMRESIST_SKILL.nPoison );
	tbStat.set( 33, apCharChangeData->m_sSUMRESIST_SKILL.nSpirit - pCharOrigData->m_sSUMRESIST_SKILL.nSpirit );
*/

	SAFE_DELETE( pVirtualCharData );

	return tbStat;
}

LuaTable GLCharacter::GetPremium()
{
	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;
	int nHour = 0;
	int nMin = 0;
	
	if( true == m_bPREMIUM )
	{
		CTime cTime( m_tPREMIUM );

		// 서버 타임으로 변환( TimeZone 계산 )
		m_pGaeaClient->GetConvertServerTime( cTime );

		nYear = cTime.GetYear();
		nMonth = cTime.GetMonth();
		nDay = cTime.GetDay();
		nHour = cTime.GetHour();
		nMin = cTime.GetMinute();
	}

	tb.set( 1, m_bPREMIUM );
	tb.set( 2, nYear );
	tb.set( 3, nMonth );
	tb.set( 4, nDay );
	tb.set( 5, nHour );
	tb.set( 6, nMin );

	return tb;
}

void GLCharacter::ReqResetCountry( WORD wPosX, WORD wPosY )
{
	GLMSG::NET_USE_ITEM_INITIALIZE_COUNTRY_CF sNetMsg;
	sNetMsg.m_wPosX = wPosX;
	sNetMsg.m_wPosY = wPosY;

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

void GLCharacter::ReqUseExpCapsuleItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	GLMSG::SNETPC_EXP_CAPSULE_REQ sNetMsg;
	sNetMsg.wPosX = wInvenPositionX;
	sNetMsg.wPosY = wInvenPositionY;

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

void GLCharacter::ReqUseExpCompressorItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	GLMSG::SNETPC_EXP_COMPRESSOR_REQ sNetMsg;
	sNetMsg.wPosX = wInvenPositionX;
	sNetMsg.wPosY = wInvenPositionY;	

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

void GLCharacter::ReqUseUnlockCharacterSlotItem ( WORD wInvenPositionX, WORD wInvenPositionY )
{
	GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF sNetMsg;
	sNetMsg.wPosX = wInvenPositionX;
	sNetMsg.wPosY = wInvenPositionY;

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

bool GLCharacter::CheckInvenItemDecompose( WORD wPosX, WORD wPosY )
{
	const SINVENITEM* const pSlotItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL != pSlotItem && true == pSlotItem->sItemCustom.IsDecompose() )
		return true;

	return false;
}

LuaTable GLCharacter::GetEquipConsume()
{
	EMSLOT emSlot = SLOT_LHAND;

	// 극강부일 경우엔 현재 활성화된 무기의 소모품으로
	if( GLCC_EXTREME_M == m_emClass || GLCC_EXTREME_W == m_emClass )
	{
		if( true == IsUseArmSub() )
			emSlot = SLOT_LHAND_S;
	}

	LuaTable tb( GLWidgetScript::GetInstance().GetLuaState() );

	const SITEMCUSTOM& sItem = m_PutOnItems[ emSlot ];
	const SITEM* pItem = GLogicData::GetInstance().GetItem( sItem.GetNativeID() );
	if( NULL == pItem )
	{
		tb.set( 1, -1 );
		tb.set( 2, -1 );
	}
	else
	{
		tb.set( 1, sItem.wTurnNum );
		tb.set( 2, (int) pItem->sBasicOp.emItemType );
	}

	return tb;
}

LuaTable GLCharacter::GetRepairableItem()
{
	LuaTable tbList( GLWidgetScript::GetInstance().GetLuaState() );

	int nTableIndex = 1;
	for( WORD i = SLOT_HEADGEAR; i < SLOT_TSIZE; ++i )
	{
		const SITEMCUSTOM& sItemCustom = GET_SLOT_ITEM( EMSLOT( i ) );
		if( sItemCustom.GetNativeID() == NATIVEID_NULL() )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if( NULL == pItem )
			continue;

		if( !ItemDurability::GetInstance().IsUse( sItemCustom.GetNativeID() ) )
			continue;

		WORD wPercent = ItemDurability::GetInstance().GetPercent( sItemCustom );

		if( wPercent < 100 )
		{
			LuaTable tbRepairItem( GLWidgetScript::GetInstance().GetLuaState() );
			tbRepairItem.set( 1, sItemCustom.GetNativeID().Mid() );
			tbRepairItem.set( 2, sItemCustom.GetNativeID().Sid() );
			tbRepairItem.set( 3, sItemCustom.GETNAME() );
			tbRepairItem.set( 4, sItemCustom.GETGRADE( EMGRINDING_NUNE ) );
			tbRepairItem.set( 5, ItemDurability::GetInstance().Get( sItemCustom ) );
			tbRepairItem.set( 6, ItemDurability::GetInstance().GetMax( sItemCustom ) );
			tbRepairItem.set( 7, wPercent );
			tbRepairItem.set( 8, ItemDurability::GetInstance().GetRepairPriceAllPoint( sItemCustom ) );
			tbRepairItem.set( 9, i );
			tbRepairItem.set( 10, ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ] );
			tbRepairItem.set( 11, ItemDurability::GetInstance().GetTextColor( sItemCustom, NS_UITEXTCOLOR::WHITE ) );
			tbList.set( nTableIndex++, tbRepairItem );
		}
	}

	return tbList;
}

void GLCharacter::ReqItemRepair( LuaTable tbSlot )
{
	GLMSG::NET_ITEM_REPAIR_REQ SendData;
	SendData.m_dwChaDbNum = CharDbNum();

	for( int i = 1; i <= tbSlot.TableLen(); ++i )
	{
		EMSLOT emSlot = EMSLOT( tbSlot.get< WORD >( i ) );
		SendData.m_vecSlot.push_back( emSlot );
	}

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );
	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_ITEM_REPAIR_REQ, PackBuffer );
}

void GLCharacter::ReqItemRepairCardUse( WORD wPosX, WORD wPosY )
{
	GLMSG::NET_ITEM_REPAIR_CARD_USE_REQ SendData;
	{
		SendData.m_dwChaDbNum = CharDbNum();
		SendData.m_nInvecPosX = wPosX;
		SendData.m_nInvecPosY = wPosY;
	}

	msgpack::sbuffer PackBuffer;
	msgpack::pack( PackBuffer, SendData );
	m_pGaeaClient->NETSENDTOFIELD( NET_MSG_ITEM_REPAIR_CARD_REQ, PackBuffer );
}

DWORD GLCharacter::GetInvenItemCount( WORD wMID, WORD wSID )
{
	return m_cInventory.CountTurnItem( SNATIVEID( wMID, wSID ) );
}

WORD GLCharacter::GetInvenItemSuitType( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return SUIT_NSIZE;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItem )
		return SUIT_NSIZE;

	if( false == pItem->IsSuitable() )
		return SUIT_NSIZE;

	return WORD( pItem->sSuitOp.emSuit );
}

bool GLCharacter::WearableCheck( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return false;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( pItem == NULL
		|| pItem->IsSuitable() == false
		|| SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) == FALSE )
		return false;

	return true;
}

DWORD GLCharacter::GetSlotTurnNumCountItem( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return 0;

	return pInvenItem->sItemCustom.wTurnNum;
}

bool GLCharacter::IsEnableLunchBox()
{
	GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
	return NULL != pLandMan ? !pLandMan->IsLunchBoxForbid() : false;
}

bool GLCharacter::IsDisguiseInvenItem( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return false;

	if( pInvenItem->sItemCustom.IsDisguise() == true )
		return true;

	return false;
}

const bool GLCharacter::IsRebuildItem ( const DWORD _dwNPCGlobalID ) const
{
	// 개조 할 아이템 유효성 검사한다;
	if ( FALSE == m_sRebuildItem.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTITEM" ) );

		return false;
	}

	// 개조 비용을 확인한다;
	if ( m_i64RebuildInput != m_i64RebuildCost || GetInvenMoney() < m_i64RebuildCost )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::NEGATIVE,
			ID2GAMEINTEXT( "EMREBUILD_RESULT_MONEY" ) );

		return false;
	}

	// 개조요청을 보내기전에 개조 아이템에 대한 유효성 검사;
	const SINVENITEM* pInvenItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if ( NULL == pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTITEM" ) );

		return false;
	}

	// 포장된 아이템은 개조할 수 없다;
	// http://172.16.2.87/redmine/issues/832
	if ( pInvenItem->sItemCustom.IsBoxWrapped() )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_WRAPPED" ) );

		return false;
	}

	// 장착 아이템만 개조가 가능하다.;
	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if( pItem && pItem->sBasicOp.emItemType != ITEM_SUIT )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOTSUIT" ) );

		return false;
	}

	// 개조 아이템 타입별로 검사한다;
	switch ( m_emTypeItemRemodelState )
	{
	case EMTYPE_ITEM_REMODEL_NONE:
		{
			// NPC 유효성 체크한다;
			PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();
			if ( NULL == pLandMan )
				return false;

			std::tr1::shared_ptr< GLCrowClient > spCrow = pLandMan->GetCrow( _dwNPCGlobalID );
			if ( NULL == spCrow )
				return false;

			if ( false == spCrow->IsNpcActionable( GetPosition(), GETBODYRADIUS() ) )
				return false;
		}
		break;

	case EMTYPE_ITEM_REMODEL_RANDOMOPTION:
		{
			// 랜덤옵션 파일이 지정되어 있어야 가능하다.;
			if ( strlen( pItem->sRandomOpt.szNAME ) <= 3 )
			{
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::DISABLE,
					ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );

				return false;
			}
		}
		break;

	case EMTYPE_ITEM_REMODEL_BASICSTATS:
		{
			// 아이템이 기본능력치 수정 가능한 아이템인지 확인한다;
			if ( ( false == pItem->IsEditableBasicAttackDamage() ) &&
				( false == pItem->IsEditableBasicDefence() ) )
			{
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::DISABLE,
					ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );

				return false;
			}
		}
		break;

	case EMTYPE_ITEM_REMODEL_ADDSTATS:
		{
			RandomAddon::Entity sEntity;
			if ( !pItem->sSuitOp.szRandomAddonScript ||
				E_FAIL == RandomAddon::Manager::GetInstance().Find( pItem->sSuitOp.szRandomAddonScript, sEntity ) )
			{
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::DISABLE,
					ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );

				return false;
			}
		}
		break;

	case EMTYPE_ITEM_REMODEL_ITEMSKILL:
		{
			// 아이템이 아이템스킬 수정 가능한 아이템인지 확인한다;
			if ( -1 == pItem->GetLinkSkillIndex() )
			{
				m_pGaeaClient->PrintMsgTextDlg(
					NS_UITEXTCOLOR::DISABLE,
					ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_DONTCHANGE" ) );

				return false;
			}
		}
		break;
	}

	return true;
}



void GLCharacter::UseDisguiseInvenItem( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem )
		return;

	SINVENITEM* pDisguiseInvenItem = m_cInventory.GetItem( wHoldPosX, wHoldPosY );
	if( NULL == pDisguiseInvenItem )
		return;

	holditem::HoldItem( holditem::INVENTORY, pDisguiseInvenItem->CustomItem(), wHoldPosX, wHoldPosY );
	UseInvenItem( wPosX, wPosY );
}

void GLCharacter::UpdateMacroState( float fElapsedTime )
{
	if ( GLMacroManager::GetInstance().IsActive() == false )
		return;

	if ( GLUseFeatures::GetInstance().IsUsingMacro_LimitTime() == TRUE )
	{
		m_fRemainMacroTime -= fElapsedTime;
		if ( m_fRemainMacroTime < 0.0f )
			m_fRemainMacroTime = 0.0f;

		// 분단위가 바뀔때마다 갱신한다;
		if ( (int)m_fRemainMacroTime/60 != (int)m_fMacro_RecordUINotifyTime/60 )
		{
			m_fMacro_RecordUINotifyTime = m_fRemainMacroTime;
			GLWidgetScript::GetInstance().LuaCallFunc<void>( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetMacroTime, "-f", m_fRemainMacroTime);
		}
	}
}

void GLCharacter::FillMacroTime(float fTime)
{
	m_fRemainMacroTime += fTime;
	GLWidgetScript::GetInstance().LuaCallFunc<void>( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_MacroStateUpdate);
}

bool GLCharacter::PlayerGotoLocation(D3DXVECTOR3& vPosition)
{
	const BOOL bGoto = ClientActor::GotoLocation(vPosition, 10.0f);

	if ( bGoto )
	{
		m_bMiniMapMove = false;
		m_fMiniMapMoveTime = 0.0f;

		TurnAction ( GLAT_MOVE );
		m_sActionID.vPos = vPosition;

		//	Note : 이동 메시지 정보;
		GLMSG::SNETPC_GOTO NetMsg;
		NetMsg.dwActState = GetActState();
		NetMsg.vCurPos = GetPosition();;
		NetMsg.vTarPos = vPosition;
		m_sLastMsgMove = NetMsg;

		if( RENDERPARAM::bClickEffect )
		{
			DxEffGameHelp::GetInstance().ResetClickEffect();
		}

		if ( ClientActor::PathIsActive() )
		{
			m_fLastMsgMoveSend = 0.0f;
			m_sLastMsgMoveSend = NetMsg;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
	return bGoto;
}

const BOOL GLCharacter::IsFeignDie() const
{
	if ( IsCheckedSkillFlagSpecial(EMSPECA_FEIGN_DEATH) == false )
		return TRUE;

	return FALSE;
}

const SNATIVEID& GLCharacter::GetChangeQuick( int iNum )
{
	return m_sChangeQUICK[iNum];
}

BOOL GLCharacter::GetSetitemEquipInfo(const DWORD dwSetID, OUT DWORD& dwEffectMinTermCount, OUT DWORD& dwEquipCount)
{
	const SSETITEM *sSetItem = GLItemSet::GetInstance().GetInfoSetItem(dwSetID);

	if(NULL == sSetItem) return FALSE;	//셋트아이템 검색안됨 -> 출력 안함;

	DWORD dwCheckEffectMinTermCount(100), dwCheckEquipCount(0);
	for( INT i = 0; i < sSetItem->vecBonus.size(); ++i )
		dwCheckEffectMinTermCount = min(sSetItem->vecBonus[i].byTerms, dwCheckEffectMinTermCount);

	for( INT i = 0; i < m_vInfoPutOnSetItem.size(); ++i )
	{
		if( sSetItem->dwSetID == m_vInfoPutOnSetItem[i].dwSetID )
		{
			dwCheckEquipCount = m_vInfoPutOnSetItem[i].vItemID.size();
			break;
		}
	}
	dwEffectMinTermCount = dwCheckEffectMinTermCount;
	dwEquipCount = dwCheckEquipCount;
	return TRUE;
}

LuaTable GLCharacter::GetCharInfoDetailAddonInfo()
{
	// 캐릭터 정보 디테일 가산 옵션 정보를 묶어줌;
	LuaTable tbDetailInfo( GLWidgetScript::GetInstance().GetLuaState() );
	tbDetailInfo.set("CriticalRate",	GetCriticalRate());
	tbDetailInfo.set("CrushingBlow",	GetCrushingBlow());
	tbDetailInfo.set("MoveSpeed",		GetMoveSpeed());
	tbDetailInfo.set("AtkSpeed",		GetAtkSpeed());
	tbDetailInfo.set("ExpRate",			GetExpRate());
	tbDetailInfo.set("IncrHpTotal",		GetIncrHpTotal());
	tbDetailInfo.set("IncrMpTotal",		GetIncrMpTotal());
	tbDetailInfo.set("IncrSpTotal",		GetIncrSpTotal());
	tbDetailInfo.set("ReceiveCP",		GetReceiveCP());
	tbDetailInfo.set("PotionHpInc",		GetPotionHpInc());
	tbDetailInfo.set("PotionMpInc",		GetPotionMpInc());
	tbDetailInfo.set("PotionSpInc",		GetPotionSpInc());

	tbDetailInfo.set("ResistFire",	GetResistFire());
	tbDetailInfo.set("ResistCold",	GetResistIce());
	tbDetailInfo.set("ResistElec",	GetResistElectric());
	tbDetailInfo.set("ResistPisn",	GetResistPoison());
	tbDetailInfo.set("ResistSprt",	GetResistSpirit());

	tbDetailInfo.set("IncR_Ignore_BossDamage",			GetIncR_Ignore_BossDamage());
	tbDetailInfo.set("Inc_BossFixedDamage",				GetInc_BossFixedDamage());
	tbDetailInfo.set("IncR_Critical_EmergencyMob",		GetIncR_Critical_EmergencyMob());
	tbDetailInfo.set("IncR_Critical_EmergencyBoss",		GetIncR_Critical_EmergencyBoss());
	tbDetailInfo.set("Dec_Damage_EmergencyState",		GetDec_Damage_EmergencyState());
	tbDetailInfo.set("IncR_ExpAddEffect",				GetIncR_ExpAddEffect());
	tbDetailInfo.set("IncR_DropGenMoney",				GetIncR_DropGenMoney());
	tbDetailInfo.set("DecR_Damage_MeleeSkill",			GetDecR_Damage_MeleeSkill());
	tbDetailInfo.set("DecR_Damage_RangeSkill",			GetDecR_Damage_RangeSkill());
	tbDetailInfo.set("DecR_Damage_MagicSkill",			GetDecR_Damage_MagicSkill());
	tbDetailInfo.set("Inc_HP_SoloPlay",					GetInc_HP_SoloPlay());
	tbDetailInfo.set("Inc_HP_PartyPlay",				GetInc_HP_PartyPlay());
	tbDetailInfo.set("IncR_AtkSpeed_SoloPlay",			GetIncR_AtkSpeed_SoloPlay());
	tbDetailInfo.set("IncR_AtkSpeed_PartyPlay",			GetIncR_AtkSpeed_PartyPlay());

	tbDetailInfo.set("GetIncR_CriticalDamage",			GetIncR_CriticalDamage());
	tbDetailInfo.set("GetIncR_CrushingBlow",			GetIncR_CrushingBlow());
	tbDetailInfo.set("GetIncR_LifePerKill",				GetIncR_LifePerKill());
	tbDetailInfo.set("GetIncR_MPPerKill",				GetIncR_MPPerKill());
	tbDetailInfo.set("GetIncR_SPPerKill",				GetIncR_SPPerKill());
	tbDetailInfo.set("GetIncR_HPMPSPPerKill",			GetIncR_HPMPSPPerKill());
	tbDetailInfo.set("GetIncR_AllMPRatioToAttack",		GetIncR_AllMPRatioToAttack());
	tbDetailInfo.set("GetIncR_AllSPRatioToHp",			GetIncR_AllSPRatioToHp());
	tbDetailInfo.set("GetIncR_AllHitRatioToMelee",		GetIncR_AllHitRatioToMelee());
	tbDetailInfo.set("GetIncR_AllHitRatioToRange",		GetIncR_AllHitRatioToRange());
	tbDetailInfo.set("GetIncR_AllHitRatioToMagic",		GetIncR_AllHitRatioToMagic());
					 
	return tbDetailInfo;
}

LuaTable GLCharacter::GetCharInfoDetailAddonLimitInfo()
{
	LuaTable tbAddonLmtInfo( GLWidgetScript::GetInstance().GetLuaState() );
	tbAddonLmtInfo.set( 1, 0 );
	tbAddonLmtInfo.set( 2, 0 );
	tbAddonLmtInfo.set( 3, 0 );
	tbAddonLmtInfo.set( 4, 0 );
	tbAddonLmtInfo.set( 5, 0 );
	tbAddonLmtInfo.set( 6, 0 );
	tbAddonLmtInfo.set( 7, 0 );
	tbAddonLmtInfo.set( 8, 0 );
	tbAddonLmtInfo.set( 9, 0 );
	tbAddonLmtInfo.set( 10, 0 );
	tbAddonLmtInfo.set( 11, 0 );
	tbAddonLmtInfo.set( 12, 0 );
	tbAddonLmtInfo.set( 13, 0 );
	tbAddonLmtInfo.set( 14, 0 );
	tbAddonLmtInfo.set( 15, 0 );
	tbAddonLmtInfo.set( 16, 0 );
	tbAddonLmtInfo.set( 17, 0 );
	tbAddonLmtInfo.set( 18, GLCONST_CHAR::fIgnore_BossDamageMax );
	tbAddonLmtInfo.set( 19, GLCONST_CHAR::nBossFixedDamageMax );
	tbAddonLmtInfo.set( 20, GLCONST_CHAR::fIncRCriticalMax_EmergencyMob );
	tbAddonLmtInfo.set( 21, GLCONST_CHAR::fIncRCriticalMax_EmergencyBoss );
	tbAddonLmtInfo.set( 22, GLCONST_CHAR::fDecRDamageMax_EmergencyState );
	tbAddonLmtInfo.set( 23, GLCONST_CHAR::fIncRDropGenMoneyMax );
	tbAddonLmtInfo.set( 24, GLCONST_CHAR::fDecRDamageMax_MeleeSkill );
	tbAddonLmtInfo.set( 25, GLCONST_CHAR::fDecRDamageMax_RangeSkill );
	tbAddonLmtInfo.set( 26, GLCONST_CHAR::fDecRDamageMax_MagicSkill );
	tbAddonLmtInfo.set( 27, GLCONST_CHAR::nInc_HPMax_SoloPlay );
	tbAddonLmtInfo.set( 28, GLCONST_CHAR::nInc_HPMax_PartyPlay );
	tbAddonLmtInfo.set( 29, GLCONST_CHAR::fIncR_AtkSpeedMax_SoloPlay );
	tbAddonLmtInfo.set( 30, GLCONST_CHAR::fIncR_AtkSpeedMax_PartyPlay );
	tbAddonLmtInfo.set( 31, GLCONST_CHAR::fIncR_CriticalDamageMax );
	tbAddonLmtInfo.set( 32, GLCONST_CHAR::fIncR_CrushingBlowMax );
	tbAddonLmtInfo.set( 33, GLCONST_CHAR::fIncR_LifePerKillMax );
	tbAddonLmtInfo.set( 34, GLCONST_CHAR::fIncR_MPPerKillMax );
	tbAddonLmtInfo.set( 35, GLCONST_CHAR::fIncR_SPPerKillMax );
	tbAddonLmtInfo.set( 36, GLCONST_CHAR::fIncR_HPMPSPPerKillMax );
	tbAddonLmtInfo.set( 37, GLCONST_CHAR::fIncR_AllMPRatioToAttackMax );
	tbAddonLmtInfo.set( 38, GLCONST_CHAR::fIncR_AllSPRatioToHpMax );
	tbAddonLmtInfo.set( 39, GLCONST_CHAR::fIncR_AllHitRatioToMeleeMax );
	tbAddonLmtInfo.set( 40, GLCONST_CHAR::fIncR_AllHitRatioToRangeMax );
	tbAddonLmtInfo.set( 41, GLCONST_CHAR::fIncR_AllHitRatioToMagicMax );
	return tbAddonLmtInfo;
}

LuaTable GLCharacter::GetCharTempPuonInfoDetailAddonInfo(WORD wPosX, WORD wPosY, WORD wVirtualPow, WORD wVirtualDex,
														 WORD wVirtualSpi, WORD wVirtualStr, WORD wVirtualSta)
{
	LuaTable tbDetailInfo( GLWidgetScript::GetInstance().GetLuaState() );

	const SINVENITEM* pInvenItem = m_cInventory.GetItem( wPosX, wPosY );
	if( NULL == pInvenItem || 
		NATIVEID_NULL() == pInvenItem->GetNativeID() || 
		true == pInvenItem->CustomItem().IsBoxWrapped() )
		return tbDetailInfo;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->GetNativeID() );
	if( NULL == pItem || false == pItem->IsSuitable() )
		return tbDetailInfo;

	if( false == (pItem->sBasicOp.dwReqCharClass & m_emClass) )
		return tbDetailInfo;

	GLCHARLOGIC* pCharOrigData = &GetCharLogic();

	std::auto_ptr< GLCHARLOGIC > apCharChangeData;
	apCharChangeData.reset( new GLCHARLOGIC );

	//*apCharChangeData = *pCharOrigData;
	apCharChangeData->Assign( *pCharOrigData );

	apCharChangeData->m_sStats.wPow += wVirtualPow;
	apCharChangeData->m_sStats.wDex += wVirtualDex;
	apCharChangeData->m_sStats.wSpi += wVirtualSpi;
	apCharChangeData->m_sStats.wStr += wVirtualStr;
	apCharChangeData->m_sStats.wSta += wVirtualSta;

	for( int i = 0; i < SLOT_TSIZE; ++i )
	{
		if( EMSLOTCHECK_OK == CHECKSLOT_ITEM( pInvenItem->GetNativeID(), EMSLOT( i ) ) )
		{
			// 반대쪽 슬롯 비어있으면 반대쪽에 넣자.
			switch( i )
			{
			case SLOT_RFINGER :
				{
					if( false == apCharChangeData->m_PutOnItems[ SLOT_LFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_LFINGER;
					// 희한한게 비교는 오른쪽에다 하고 착용은 왼쪽에다 한다.
					// 오른쪽 슬롯도 착용중이면 왼쪽에 넣자.
					else if( true == apCharChangeData->m_PutOnItems[ SLOT_RFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_LFINGER;
				}
				break;

			case SLOT_LFINGER :
				{
					if( false == apCharChangeData->m_PutOnItems[ SLOT_RFINGER ].GetNativeID().IsValidNativeID() )
						i = SLOT_RFINGER;
				}
				break;

			case SLOT_RACCESSORIES :
				{
					if( false == apCharChangeData->m_PutOnItems[ SLOT_LACCESSORIES ].GetNativeID().IsValidNativeID() )
						i = SLOT_LACCESSORIES;
				}
				break;

			case SLOT_LACCESSORIES :
				{
					if( false == apCharChangeData->m_PutOnItems[ SLOT_RACCESSORIES ].GetNativeID().IsValidNativeID() )
						i = SLOT_RACCESSORIES;
				}
				break;
			}

			apCharChangeData->m_PutOnItems[ i ] = pInvenItem->sItemCustom;
			break;
		}
	}

	apCharChangeData->INIT_DATA( FALSE, FALSE );
	apCharChangeData->UPDATE_DATA( 1, 1 );

	tbDetailInfo.set( 1, apCharChangeData->GETCRITICAL_TOTAL() - pCharOrigData->GETCRITICAL_TOTAL() );
	tbDetailInfo.set( 2, apCharChangeData->m_sSUMITEM.fIncR_CrushingBlow - pCharOrigData->m_sSUMITEM.fIncR_CrushingBlow );
	tbDetailInfo.set( 3, apCharChangeData->GETMOVE_TOTAL() - pCharOrigData->GETMOVE_TOTAL() );
	tbDetailInfo.set( 4, apCharChangeData->GETATT_TOTAL() - pCharOrigData->GETATT_TOTAL() );
	tbDetailInfo.set( 5, apCharChangeData->GETEXPVELO() - pCharOrigData->GETEXPVELO() );
	tbDetailInfo.set( 6, apCharChangeData->m_fVariationRateHP - pCharOrigData->m_fVariationRateHP );
	tbDetailInfo.set( 7, apCharChangeData->m_fVariationRateMP - pCharOrigData->m_fVariationRateMP );
	tbDetailInfo.set( 8, apCharChangeData->m_fVariationRateSP - pCharOrigData->m_fVariationRateSP );
	tbDetailInfo.set( 9, apCharChangeData->m_sSUMITEM.nReceive_CP_Inc - pCharOrigData->m_sSUMITEM.nReceive_CP_Inc );
	tbDetailInfo.set(10, (apCharChangeData->m_sSUMITEM.nPotion_HP_Inc + apCharChangeData->m_nPotion_HP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_HP_Inc + pCharOrigData->m_nPotion_HP_Inc_SKILL) );
	tbDetailInfo.set(11, (apCharChangeData->m_sSUMITEM.nPotion_MP_Inc + apCharChangeData->m_nPotion_MP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_MP_Inc + pCharOrigData->m_nPotion_MP_Inc_SKILL) );
	tbDetailInfo.set(12, (apCharChangeData->m_sSUMITEM.nPotion_SP_Inc + apCharChangeData->m_nPotion_SP_Inc_SKILL) - 
		(pCharOrigData->m_sSUMITEM.nPotion_SP_Inc + pCharOrigData->m_nPotion_SP_Inc_SKILL) );

	tbDetailInfo.set(13,	apCharChangeData->m_sSUMRESIST_SKILL.nFire - pCharOrigData->m_sSUMRESIST_SKILL.nFire );
	tbDetailInfo.set(14,	apCharChangeData->m_sSUMRESIST_SKILL.nIce - pCharOrigData->m_sSUMRESIST_SKILL.nIce );
	tbDetailInfo.set(15,	apCharChangeData->m_sSUMRESIST_SKILL.nElectric - pCharOrigData->m_sSUMRESIST_SKILL.nElectric );
	tbDetailInfo.set(16,	apCharChangeData->m_sSUMRESIST_SKILL.nPoison - pCharOrigData->m_sSUMRESIST_SKILL.nPoison );
	tbDetailInfo.set(17,	apCharChangeData->m_sSUMRESIST_SKILL.nSpirit - pCharOrigData->m_sSUMRESIST_SKILL.nSpirit );

	tbDetailInfo.set(18,	apCharChangeData->m_sSUMITEM.fIncR_Ignore_BossDamage			- GetIncR_Ignore_BossDamage());
	tbDetailInfo.set(19,	apCharChangeData->m_sSUMITEM.nInc_BossFixedDamage				- GetInc_BossFixedDamage());
	tbDetailInfo.set(20,	apCharChangeData->m_sSUMITEM.fIncR_Critical_EmergencyMob		- GetIncR_Critical_EmergencyMob());
	tbDetailInfo.set(21,	apCharChangeData->m_sSUMITEM.fIncR_Critical_EmergencyBoss		- GetIncR_Critical_EmergencyBoss());
	tbDetailInfo.set(22,	apCharChangeData->m_sSUMITEM.fDecR_Damage_EmergencyState			- GetDec_Damage_EmergencyState());
//	tbDetailInfo.set(23,	apCharChangeData->m_sSUMITEM.fIncR_ExpAddEffect					- GetIncR_ExpAddEffect());
	tbDetailInfo.set(23,	apCharChangeData->m_sSUMITEM.fIncR_DropGenMoney					- GetIncR_DropGenMoney());
	tbDetailInfo.set(24,	apCharChangeData->m_sSUMITEM.fDecR_Damage_MeleeSkill			- GetDecR_Damage_MeleeSkill());
	tbDetailInfo.set(25,	apCharChangeData->m_sSUMITEM.fDecR_Damage_RangeSkill			- GetDecR_Damage_RangeSkill());
	tbDetailInfo.set(26,	apCharChangeData->m_sSUMITEM.fDecR_Damage_MagicSkill			- GetDecR_Damage_MagicSkill());
	tbDetailInfo.set(27,	apCharChangeData->m_sSUMITEM.nInc_HP_SoloPlay					- GetInc_HP_SoloPlay());
	tbDetailInfo.set(28,	apCharChangeData->m_sSUMITEM.nInc_HP_PartyPlay					- GetInc_HP_PartyPlay());
	tbDetailInfo.set(29,	apCharChangeData->m_sSUMITEM.fIncR_AtkSpeed_SoloPlay			- GetIncR_AtkSpeed_SoloPlay());
	tbDetailInfo.set(30,	apCharChangeData->m_sSUMITEM.fIncR_AtkSpeed_PartyPlay			- GetIncR_AtkSpeed_PartyPlay());

	tbDetailInfo.set(31,	apCharChangeData->m_sSUMITEM.fIncR_CriticalDamage		- GetIncR_CriticalDamage());
	tbDetailInfo.set(32,	apCharChangeData->m_sSUMITEM.fIncR_CrushingBlowDamage	- GetIncR_CrushingBlow());
	tbDetailInfo.set(33,	apCharChangeData->m_sSUMITEM.fIncR_LifePerKill			- GetIncR_LifePerKill());
	tbDetailInfo.set(34,	apCharChangeData->m_sSUMITEM.fIncR_MPPerKill			- GetIncR_MPPerKill());
	tbDetailInfo.set(35,	apCharChangeData->m_sSUMITEM.fIncR_SPPerKill			- GetIncR_SPPerKill());
	tbDetailInfo.set(36,	apCharChangeData->m_sSUMITEM.fIncR_HPMPSPPerKill		- GetIncR_HPMPSPPerKill());
	tbDetailInfo.set(37,	apCharChangeData->m_sSUMITEM.fIncR_AllMPRatioToAttack	- GetIncR_AllMPRatioToAttack());
	tbDetailInfo.set(38,	apCharChangeData->m_sSUMITEM.fIncR_AllSPRatioToHp		- GetIncR_AllSPRatioToHp());
	tbDetailInfo.set(39,	apCharChangeData->m_sSUMITEM.fIncR_AllHitRatioToMelee	- GetIncR_AllHitRatioToMelee());
	tbDetailInfo.set(40,	apCharChangeData->m_sSUMITEM.fIncR_AllHitRatioToRange	- GetIncR_AllHitRatioToRange());
	tbDetailInfo.set(41,	apCharChangeData->m_sSUMITEM.fIncR_AllHitRatioToMagic	- GetIncR_AllHitRatioToMagic());
	return tbDetailInfo;
}