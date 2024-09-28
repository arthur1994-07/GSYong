#include "../pch.h"
#include <shlobj.h>
#include <tlhelp32.h>
#include <Psapi.h>

#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/GUInterface/Cursor.h"
#include "../../EngineLib/GUInterface/NSGUI.h"
#include "../../EngineLib/DxEffect/EffProj/DxEffProj.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffFanwise.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffPlane.h"
#include "../../EngineLib/DxEffect/Single/Util/DxEffCircle.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxEffect/DxPostEffect.h" // by luxes.
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimation.h"
#include "../../EngineLib/DxMeshs/SkinMesh/SANIMCONINFO.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxFootColl.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"
#include "../../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxSound/BgmSound.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../EngineLib/Common/CommonWeb.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../EngineLib/GlobalParam.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLuaPlay.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
//#include "../../RanLogic/Item/GLItemMixMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Activity/ActivityBase.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/TransformSet/GLTransformSet.h"
#include "../../RanLogic/stl_Func.h"

#include "../../RanLogicClient/Product/GLProductClient.h"
// 
#include "../../RanUI/OldUI/Interface/SkillTrayTab.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../Strike/GLStrikeM.h"
#include "../Party/GLPartyClient.h"
#include "../Friend/GLFriendClient.h"
#include "../Post/GLPostClient.h"
#include "../SNS/GLSNSClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../Attendance/GLAttendanceClient.h"
#include "../CapturedTheField/GLCaptureTheFieldClient.h"
#include "../Command/dxincommand.h"
#include "../GLogicExClient.h"
#include "../Land/GLLandManClient.h"
#include "../Input/GLInputManager.h"
#include "./GLCharacter.h"

#include "../GfxInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bEFFECT_TOOL;

HRESULT GLCharacter::PlayerUpdate ( float fTime, float fElapsedTime )
{
	//	Note : 사용자 입력값 적용.
	//
	DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

	DWORD dwKeyR(NULL);
	DWORD dwKeyX(NULL);
	DWORD dwKeyZ(NULL);
	DWORD dwKeyLAlt(NULL);
	DWORD dwKeyLCtrl(NULL);
	DWORD dwKeyLShift(NULL);
	DWORD dwkeySPACE(NULL);
	DWORD dwML(NULL), dwMM(NULL), dwMR(NULL);

	// 채팅윈도우 활성화시 키보드 가져 오지 않음.
    EngineDeviceMan* pDevice = m_pGaeaClient->GetEngineDevice();
    DXInputString* pInputString = pDevice->GetInputString();
	//if ( !pInputString->IsOn() && COMMON_WEB::GetTopWindowID() < 0 )
	//{
	//	dwKeyR = dxInputDev.GetKeyState(RANPARAM::MenuShotcut[SHOTCUT_RUN]);
	//	dwKeyZ = dxInputDev.GetKeyState(DIK_Z);
	//	dwkeySPACE = dxInputDev.GetKeyState(DIK_SPACE);		
	//	dwKeyX = dxInputDev.GetKeyState(RANPARAM::MenuShotcut[SHOTCUT_ATTACKMODE]);
	//}

    // TODO : 불법 프로그램 감지 테스트..
	if ( !m_sPMarket.IsOpen() ) 
	{
		if( dxInputDev.IsUpdatInputState() )
		{
			m_fKeyDownCheckTime = 0.0f;
            m_bAwayGesture = false;
		}
        else
        {
			m_fKeyDownCheckTime += fElapsedTime;


            if( m_fKeyDownCheckTime >= GLCONST_CHAR::fBECOME_AWAY_TIME )
			{
                if ( !m_bAwayGesture )
                {
			        m_bAwayGesture = true;

                    ReqGESTURE( GLCONST_CHAR::nAWAY_GESTURE_NUMBER );		
                }
			}

			if(m_pGaeaClient->IsCTFPlayer() &&  GLUseFeatures::GetInstance().IsUsingafkBanCTF() )
			{
				if( m_fKeyDownCheckTime >= GLCONST_CHAR::fCTF_BECOME_AWAY_TIME )
				{
					PrintMsgText(NS_UITEXTCOLOR::CHAT_SYSTEM,ID2GAMEINTEXT("COMPETITIONNOTIFY_IN_AFK_WARNNING_1") );
					//팅구기
					m_pGaeaClient->ReqCaptureTheFieldQuit();
				}
				else
				{
					float fleftime = GLCONST_CHAR::fCTF_BECOME_AWAY_TIME  -m_fKeyDownCheckTime;
					if(fleftime > 0)
					{
						int vsize = GLCONST_CHAR::vecCTFNotify.size();
						DWORD temppos = -1;

						for(int i = vsize-1 ; i >-1 ; --i  )
						{
							if( fleftime <GLCONST_CHAR::vecCTFNotify[i] )
							{
								temppos = i;
							}
						}

						if(temppos != -1 && temppos != m_nAwayGestureLevel)
						{
							//채팅 뿌리기						
							PrintMsgText(NS_UITEXTCOLOR::CHAT_SYSTEM, sc::string::format(ID2GAMEINTEXT("COMPETITIONNOTIFY_IN_AFK_WARNNING_0") , GLCONST_CHAR::vecCTFNotify[temppos]) );
							m_nAwayGestureLevel = temppos;
						}
					}
				}
			}
			
		}
	}
    else
    {
		m_fKeyDownCheckTime = 0.0f;
	}

	CDebugSet::ToView ( 1, "KeyDownCheckTime %f", m_fKeyDownCheckTime );

	dwKeyLAlt = dxInputDev.GetKeyState ( DIK_LMENU );
	dwKeyLCtrl = dxInputDev.GetKeyState ( DIK_LCONTROL );
	dwKeyLShift = dxInputDev.GetKeyState ( DIK_LSHIFT );


	if( false == GfxInterfaceBase::GetInstance()->IsMouseHitUI() && 
		false == GfxInterfaceBase::GetInstance()->IsDragging() )
	{
		dwML = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
		dwMM = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );
		dwMR = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );
	}
	else
	{
		dwML = NULL;
		dwMM = NULL;
		dwMR = NULL;
	}

	if ( m_sCONFTING.bFIGHT && m_sCONFTING.sOption.bSCHOOL )
	{
		//	Note : 학교간 대련 마크 표시 Update.
		//!! SF_TODO
		//m_pInterface->SetAcademyUpdateNumber(
		//	m_pGaeaClient->GetMyPartyClient()->GetNConfrontMember(), (int) m_sCONFTING.setConftMember.size());
	}

	return S_OK;
}

HRESULT GLCharacter::SelectTargetUpdate( const bool bUsingSelectInfo )
{
	// Note : 대상을 선택했을 경우
	if( m_sSelectID.GaeaId != EMTARGET_NULL )
	{
		ClientActor* pACTTAR = m_pGaeaClient->GetCopyActor ( m_sSelectID );
		if ( pACTTAR && (pACTTAR->IsNonTargetable() == FALSE) )
		{
			D3DXVECTOR3 vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );			
			if( m_sSelectID.emCrow == CROW_PC )
			{
				GLCharClient *pChar = (GLCharClient *)pACTTAR;
				vScale = -pChar->GetMinOrg() / 6.0f;				
			}
			if( m_sSelectID.emCrow == CROW_NPC || m_sSelectID.emCrow == CROW_MOB )
			{
				GLCrowClient *pCrow = (GLCrowClient *)pACTTAR;
				vScale = -pCrow->GetMinOrg() / 6.0f;				
			}else if( m_sSelectID.emCrow == CROW_MATERIAL )
			{
				GLMaterialClient *pMaterial = (GLMaterialClient *)pACTTAR;
				vScale = -pMaterial->GetMinOrg() / 6.0f;
			}else if( m_sSelectID.emCrow == CROW_SUMMON )
			{
				GLSummonClient *pSummon = (GLSummonClient *)pACTTAR;
				vScale = -pSummon->GetMinOrg() / 6.0f;
			}
			float fScale = max(vScale.x,vScale.z);

			D3DXMATRIX matCircleEffect, matArrowEffect;

			D3DXVECTOR3 vPosition	= pACTTAR->GetPosition();
			float		fBodyHeight = pACTTAR->GetBodyHeight()*1.4f/*bjju.temp*/;

			D3DXMatrixTranslation ( &matCircleEffect, vPosition.x, vPosition.y, vPosition.z );
			D3DXMatrixTranslation ( &matArrowEffect,  vPosition.x, vPosition.y + fBodyHeight, vPosition.z );

			m_sSelectID.vPos = pACTTAR->GetPosition();

			EMCROWCOLORTYPE		emColorType			 = m_pGaeaClient->GetCrowIdentifyColorType( m_sSelectID );
			GAMEHELP_EFFECTTYPE emGameHelpEffectType = EMTARGET_EFFECT_NULL;

			if ( bUsingSelectInfo )
			{
				switch ( emColorType )
				{
				case EMCROWCOLORTYPE_ENEMY:		emGameHelpEffectType = EMTARGET_EFFECT_ENEMY;		break;
				case EMCROWCOLORTYPE_NPC:		emGameHelpEffectType = EMTARGET_EFFECT_NPC;			break;
				case EMCROWCOLORTYPE_ALLIANCE:	emGameHelpEffectType = EMTARGET_EFFECT_ALLIANCE;	break;
				};
			}
			else
			{
				emGameHelpEffectType = EMTARGET_EFFECT_DEFAULT;
			}

			DxEffGameHelp::GetInstance().UpdateTargetEffect( emGameHelpEffectType, m_sSelectID,
				matCircleEffect, matArrowEffect, D3DXVECTOR3( fScale, fScale, fScale ) );


			if ( bUsingSelectInfo && pACTTAR->IsHideNaming() == false )
			{
				const EMCROW _emCrow = pACTTAR->GetCrow();
				switch ( _emCrow )
				{
				case CROW_SUMMON:
					{
						GLSummonClient* pSummonClient = MIN_BOOST::polymorphic_downcast<GLSummonClient*>(pACTTAR);
						if ( pSummonClient->m_dwOwnerGaeaID == m_pGaeaClient->GETMYGAEAID() )
						{
							// [shhan][2015.01.28]
							//	내 소환수는 정보를 보지 않도록 한다.
							//	이걸 하지 않으면 TARGETINFO가 표시중인 상황에서 내 소환수에 선택할 시 이전 UI 가 남아있으면서 허공에 떠도는 문제가 발생할 수 있음.
							//!! SF_TODO
							//m_pInterface->ResetTargetInfoSelect();
							GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();

						}
						else
						{
							//!! SF_TODO
							//m_pInterface->SetTargetInfoSelect( m_sSelectID );
							//GfxInterfaceBase::GetInstance()->GetOldInterface()->SetTargetInfoSelect(m_sSelectID);
							GfxInterfaceBase::GetInstance()->SetTargetInfoSelect(m_sSelectID);

							
						}
					}
					break;
				default:
					//!! SF_TODO
					//m_pInterface->SetTargetInfoSelect( m_sSelectID );
					GfxInterfaceBase::GetInstance()->SetTargetInfoSelect(m_sSelectID);

					break;
				};
			}

		}else{
			DxEffGameHelp::GetInstance().BlindTargetEff();

			DxSkinChar* pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform);
			if ( pRenderChar == NULL )
				pRenderChar = m_pSkinChar;

			m_sSelectID.RESET();
			m_ReservationAction.Clear();

			if ( bUsingSelectInfo )
			{
				//!! SF_TODO
				//m_pInterface->ResetTargetInfoSelect();
				GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();
			}

			GLMSG::SNETPC_SELECT_TARGET NetMsg;
			NetMsg.emTarCrow = CROW_MOB;
			NetMsg.dwTarID = GAEAID_NULL;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
	else
	{
		DxEffGameHelp::GetInstance().BlindTargetEff();

		if ( bUsingSelectInfo )
		{
			//!! SF_TODO
			//m_pInterface->ResetTargetInfoSelect();
			GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();

		}
	}

	return S_OK;
}

//! 범위지정 스킬
HRESULT GLCharacter::ScopeSkillUpdate()
{
	SKILLID sSkillID = GetScopeSkillRun();
	DxEffGameHelp::GetInstance().BlindScopeEff();

	if ( sSkillID == SKILLID_NULL )
	{
		return S_FALSE;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSkillID.mid(), sSkillID.sid() );

	if ( !pSkill ) 
	{
		SetScopeSkillRun( SKILLID_NULL );
		return E_FAIL;
	}

	//! 커서가 윈도우 안에 있을경우만
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
	{
		return S_FALSE;
	}

	switch ( pSkill->m_sBASIC.emIMPACT_REALM )
	{
	default:
		{
			SetScopeSkillRun( SKILLID_NULL );
			return E_FAIL;
		}

	case REALM_SELF:
	case REALM_ZONE:
	case REALM_KEEP_ZONE:
		//! 원형
		{		
			bool bEnable = true;
			float fRadius = 0.0f;
			D3DXVECTOR3 vCollPos;

			if ( !SkillActivationInfoCircle( sSkillID, vFromPt, vTargetPt, bEnable, vCollPos, fRadius) )
			{
				return E_FAIL;
			}

			//! 위치 업데이트
			D3DXMATRIX matPos;
			D3DXMatrixTranslation ( &matPos, vCollPos.x, vCollPos.y+0.1f, vCollPos.z );

			PLANE_PROPERTY sProperty;
			sProperty.m_fWidth	= fRadius*2.0f;
			sProperty.m_fHeight = fRadius*2.0f;
			sProperty.m_vNormal	= D3DXVECTOR3(0.0f,1.0f,0.0f);

			if ( bEnable )
			{
				sProperty.m_dwColor = 0xFFA0A0FF; /*bjju.temp*/
			}
			else
			{
				sProperty.m_dwColor = 0xFFF01010; /*bjju.temp*/
			}

			//! CIRCLE_PROPERTY는 색이 변해도 버텍스버퍼를 새로 만들지 않는다.
			CIRCLE_PROPERTY sProperty_OutLine;
			sProperty_OutLine.m_fRadius			 = fRadius;
			sProperty_OutLine.m_fVertexPerDegree = 0.5f;
			sProperty_OutLine.m_fOutLine		 = 1.0f;		/*bjju.temp*/
			sProperty_OutLine.m_dwOutLineColor	 = 0xFFFFFFFF;	/*bjju.temp*/

			DxEffGameHelp::GetInstance().UpdateScopePlaneEffect( sProperty, sProperty_OutLine, matPos );
		}
		break;
		
	case REALM_FANWIZE:
		//! 부채꼴
		{

			//! 위치 업데이트
			D3DXVECTOR3 vOrgPos = GetPosition();
			D3DXVECTOR3 vPos	= vOrgPos;
			vPos.y += (GetBodyHeight()*0.5f);

			D3DXMATRIX matPos;
			D3DXMatrixTranslation ( &matPos, vPos.x, vPos.y, vPos.z );

			//! 방향 업데이트.
			D3DXVECTOR3 vDir;
			float fRadius = 0.0f;
			float fDegree = 0.0f;
			if ( !SkillActivationInfoFanwise( sSkillID, vFromPt, vTargetPt, vDir, fRadius, fDegree ) )
			{
				return E_FAIL;
			}

			float fThetaY = DXGetThetaYFromDirection ( vDir, D3DXVECTOR3(0.0f, 0.0f, 1.0f) );
			D3DXMATRIX matRotY;
			D3DXMatrixRotationY( &matRotY, fThetaY );

			//! 최종 합성
			D3DXMATRIX matWorld;
			matWorld = matRotY * matPos;

			FANWISE_PROPERTY sProperty;
			sProperty.m_fRadius			 = fRadius;
			sProperty.m_fDegree			 = fDegree;
			sProperty.m_fVertexPerDegree = 180.0f/sProperty.m_fDegree;
			sProperty.m_dwColor			 = 0xFFA0A0FF; /*bjju.temp*/
			DxEffGameHelp::GetInstance().UpdateFanwiseEffect( sProperty, matWorld );
		}
		break;
	}

	return S_OK;
}



// *****************************************************
// Desc: 게이트 조사
// *****************************************************
DWORD GLCharacter::DetectGate()
{
	if (IsActState(EM_REQ_GATEOUT))
        return UINT_MAX;

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return UINT_MAX;

	DxLandGateMan *pLandGateMan = pLandMClient->GetLandGateMan();
	if (!pLandGateMan)
        return UINT_MAX;
		
	if (pLandMClient->m_fAge < 3.0f)
        return UINT_MAX;

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate(GetPosition());
	if (!pLandGate)
        return UINT_MAX;
		
	if (pLandGate->GetFlags() & DxLandGate::GATE_OUT)
        return pLandGate->GetGateID();

	return UINT_MAX;
}

// *****************************************************
// Desc: 게이트의 이름 조사
// *****************************************************
void GLCharacter::DetectGateToMapName (MAPNAMEVECTOR& mapNameVector)
{
	mapNameVector.clear();
    CString strReturn("");
	if (IsActState(EM_REQ_GATEOUT))
        return;

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();
	if (!pLandMClient)
        return;

	DxLandGateMan *pLandGateMan = pLandMClient->GetLandGateMan();
	if (!pLandGateMan)
        return;
		
	if (pLandMClient->m_fAge < 3.0f)
        return;

	PDXLANDGATE pLandGate = pLandGateMan->DetectGate(GetPosition());
	if (!pLandGate)
        return;

    // 이벤트 게이트 일떄는 이름을 출력하지 않는다.
    if (pLandGate->GetEvent())
        return;
	
	if ( pLandGate->GetFlags()&DxLandGate::GATE_OUT )
	{
		for ( DWORD _i(0); _i < pLandGate->GetNGateToMapID(); ++_i )
		{
			SNATIVEID sToMapID = pLandGate->GetToMapID(_i);			
			SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(sToMapID);
			if ( pMapNode )
			{
				const DWORD nGateNameIndex(pLandGate->GetGateNameIndex(_i));
				CString strMapName(m_pGaeaClient->GetMapName( sToMapID ));
				if ( nGateNameIndex == -1 )
					mapNameVector.push_back(strMapName);
				else
				{
					CString strGateToName(ID2GAMEWORD(pLandGate->GetGateNameKey(_i), nGateNameIndex));
					mapNameVector.push_back(strMapName + "," + strGateToName);
				}
			}
		}		
	}
}

float GLCharacter::GetMoveVelo()
{
    return GLOGICEX::GetMoveVelocity(
        IsActState(EM_ACT_RUN),
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO,
        GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO,
        m_sVehicle.m_bUseBoost ? true : false,
        GLCHARLOGIC::GETMOVEVELO(),
        GLCHARLOGIC::GETMOVE_ITEM(),
        m_fMoveVeloLimit_Min,
        m_fMoveVeloLimit_Max,
        IsCDMSafeTime(),
        GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED);
/*
	float fDefaultVelo = IsSTATE(EM_ACT_RUN) ? GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fRUNVELO : GLCONST_CHAR::cCONSTCLASS[m_CHARINDEX].fWALKVELO;
	float fMoveVelo = 0.0f;
	if (m_sVehicle.m_bUseBoost)
	{
		fMoveVelo = fDefaultVelo * ((GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM()) * 1.5f);
	}
	else
	{
		fMoveVelo = fDefaultVelo * (GLCHARLOGIC::GETMOVEVELO() + GLCHARLOGIC::GETMOVE_ITEM());
	}

	if (IsCDMSafeTime())
        fMoveVelo = fMoveVelo * GLCONST_CHAR::fCDM_SAFE_TIME_MOVE_SPEED;

	return fMoveVelo;
*/
}

void GLCharacter::SetLockMovement(const bool bLock)
{
	m_bLockMoveMent = bLock;
	if ( bLock == false )
		return;

	m_ReservationAction.Clear();
}

void GLCharacter::VietnamCalculate()
{
	const int nMax1 = 180;
	const int nMax2 = 300;
	/*const int nMax1 = 30;
	const int nMax2 = 50;*/
	//!! SF_TODO
	//m_pInterface->SET_VNGAINTYPE_GAUGE( 0, nMax2 );
   	if( m_sVietnamSystem.loginTime == 0 ) return;
	if( m_dwVietnamGainType == GAINTYPE_EMPTY ) 
	{
		//!! SF_TODO
		//m_pInterface->SET_VNGAINTYPE_GAUGE( nMax2, nMax2 );
		return;
	}

	CTimeSpan gameTime( 0, (int)m_sVietnamSystem.gameTime / 60, (int)m_sVietnamSystem.gameTime % 60, 0 );


	CTime loginTime   = m_sVietnamSystem.loginTime;
	const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
	CTimeSpan crtGameSpan;
	crtGameSpan			  = gameTime + ( crtTime - loginTime );

	int totalMinute  = (int)crtGameSpan.GetTotalMinutes();
	int totalSecond  = (int)crtGameSpan.GetTotalSeconds();

	CDebugSet::ToView ( 1, "Vietnam total Time M %d S %d", totalMinute, totalSecond );
	CDebugSet::ToView ( 2, "GameTime H %d M %d S %d Value %d", gameTime.GetTotalHours(), gameTime.GetTotalMinutes(), gameTime.GetTotalSeconds(),
						m_sVietnamSystem.gameTime );
	CDebugSet::ToView ( 3, "CrtTime Year %d Mon %d D %d H %d M %d S %d LoginTime Year %d Mon %d D %d H %d M %d S %d", 
							crtTime.GetYear(), crtTime.GetMonth(), crtTime.GetDay(), crtTime.GetHour(), crtTime.GetMinute(), crtTime.GetSecond(), 
							loginTime.GetYear(), loginTime.GetMonth(), loginTime.GetDay(), loginTime.GetHour(), loginTime.GetMinute(), loginTime.GetSecond() );


	if( m_dwVietnamGainType == GAINTYPE_MAX )
	{
		if( totalMinute > nMax1 )
			totalMinute = nMax1;
	}else if( m_dwVietnamGainType == GAINTYPE_HALF )
	{
		if( totalMinute < nMax1 )
			totalMinute = nMax1;
		if( totalMinute > nMax2 )
			totalMinute = nMax2;
	}

	if( totalMinute < 0 )  totalMinute = 0;
	
	//!! SF_TODO
	//m_pInterface->SET_VNGAINTYPE_GAUGE( totalMinute, nMax2 );
	
	
	//	m_sVietnamSystem.currentGameTime = totalHours;
	//	m_sVietnamSystem.currentGameTime = totalMinutes;
	// 테스트는 초로 저장..
}

void GLCharacter::EventCalculate()
{
	if( m_bEventStart == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Start FALSE" );	
		return;
	}
	if( m_bEventApply == FALSE )
	{
//		CDebugSet::ToView ( 1, "Event Apply FALSE" );	
		return;
	}

	const CTime& crtTime = m_pGaeaClient->GetCurrentTime();
	CTime startTime = m_tLoginTime;
	CTimeSpan crtGameSpan;
	crtGameSpan					 = ( crtTime - startTime );

	if( m_bEventBuster == FALSE )
	{
		m_RemainEventTime  = m_EventStartTime - (int)crtGameSpan.GetTotalSeconds();
		m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime )
		{
//			m_bEventBuster = TRUE;	
			m_RemainEventTime = 0;
		}
	}

	if( m_bEventBuster == TRUE )
	{
		m_RemainEventTime  = 0;
		m_RemainBusterTime = (m_EventStartTime + m_EventBusterTime) - (int)crtGameSpan.GetTotalSeconds();
		if( m_RemainBusterTime > m_EventBusterTime )
			m_RemainBusterTime = m_EventBusterTime;

		if( crtGameSpan.GetTotalSeconds() >= m_EventStartTime + m_EventBusterTime )
		{
//			m_bEventBuster = FALSE;
//			m_tLoginTime   = crtTime.GetTime();

			m_RemainEventTime  = m_EventStartTime;
			m_RemainBusterTime = m_EventBusterTime;
		}
	}

	

	
	/*CDebugSet::ToView ( 1, "gameSpan: %d ;; bEventBuster: %d ;; startTime: %d ;; busterTime: %d", 
					    (int)crtGameSpan.GetTotalSeconds(), m_bEventBuster, m_EventStartTime, m_EventBusterTime );

	CDebugSet::ToView ( 2, "RemainEventTime: %d RemainBusterTime: %d", m_RemainEventTime, m_RemainBusterTime );*/

	
}

void GLCharacter::UpdateSpecialSkill( float fElapsedTime )
{
	const D3DXVECTOR3& _vPosition = GetPosition();
	// 변신 스킬 체크
	if (m_dwTransformSkill != SNATIVEID::ID_NULL && m_dwTransformSkill < SKILLFACT_SIZE)
	{
		if( m_sSKILLFACT[m_dwTransformSkill].IsSpecialSkill( SKILL::EMSSTYPE_TRANSFORM ) )
		{
			// 쓰러지는 모션이면 변신해제
			STARGETID sTargetID(CROW_PC,m_dwGaeaID,_vPosition);
			if( isAction( GLAT_FALLING ) )
			{
				FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC,m_dwGaeaID,_vPosition), m_pSkinChar, m_sSKILLFACT[m_dwTransformSkill].sNATIVEID );
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
							DxEffGroupPlayer::GetInstance().NewEffGroup ( pSkill->m_sSPECIAL_SKILL.strEffectName.c_str(), matTrans, &sTargetID, TRUE, TRUE );
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
				UpdateSuit(TRUE);				
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

				UpdateSuit(TRUE);
				m_dwTransformSkill = SNATIVEID::ID_NULL;
				m_IdSkillTransform = NATIVEID_NULL();
			}			
		}
	}
	
}

void GLCharacter::StartDealyAction( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill )
{
	m_pWorkingSkill->StartDealyAction( sSkill, pOwnerChar, pTargetData, bSubSkill );
}

void GLCharacter::SET_RECORD_CHAT()
{
	if( m_bRecordChat ) return;
	m_bRecordChat = TRUE;
	//!! SF_TODO
	//m_strRecordChat		= m_pInterface->GET_RECORD_CHAT();
	m_recordStartTime	= m_pGaeaClient->GetCurrentTime();

	PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_START") );
}

void GLCharacter::UPDATE_RECORD_CHAT( bool bCloseClient /* = FALSE */ )
{
	if (!m_bRecordChat)
        return;
	const CTime& curTime = m_pGaeaClient->GetCurrentTime();

//	서버 타임으로 변환( TimeZone 계산 )
//	m_pGaeaClient->GetConvertServerTime( curTime );
	
	CTimeSpan timeSpan = curTime - m_recordStartTime;
	if( timeSpan.GetMinutes() >= 2 || bCloseClient ) 
	{
		//!! SF_TODO
		//m_strRecordChat += m_pInterface->GET_RECORD_CHAT();

		{
			TCHAR szPROFILE[MAX_PATH] = {0};
			TCHAR szFullPathFileName[MAX_PATH] = {0};
			SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

            const SUBPATH* pPath = m_pGaeaClient->GetSubPath();
			StringCchCopy( szFullPathFileName, MAX_PATH, szPROFILE );
			StringCchCat( szFullPathFileName, MAX_PATH, pPath->SaveRoot().c_str()); //SUBPATH::SAVE_ROOT );
			CreateDirectory( szFullPathFileName, NULL );

			CHAR  szFileName[MAX_PATH] = {0};
			sprintf_s( szFileName, MAX_PATH, "chat_[%d%d%d%d%d].txt", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 
													      curTime.GetHour(), curTime.GetMinute() );

			StringCchCat ( szFullPathFileName, MAX_PATH, szFileName );

			m_bRecordChat   = FALSE;

			CFile file;
			if ( ! file.Open( _T(szFullPathFileName), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary ) )// 파일 열기
			{

				CDebugSet::ToListView ( "RECORD_CHAT_FAILED" );
				return;
			}

			CString strSaveChat;			
			for( int i = 0; i < m_strRecordChat.GetLength(); i++ )
			{
				char szTempChar = m_strRecordChat.GetAt(i) ^ 0x2139;
				strSaveChat += szTempChar;
			}

			int nLength = strSaveChat.GetLength();
			file.Write(&nLength, sizeof(int));
			file.Write(strSaveChat.GetString(), nLength);
			file.Close();


			m_strRecordChat = "";
		}

		if( !bCloseClient )
		{
			PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("RECORD_CHAT_END") );
		}

	}
}

void GLCharacter::UpdateMapEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	if( pLand->m_bClubBattle ) 
	{
		if ( !m_pGaeaClient->IsClubBattle() ) 
		{
			DISABLEALLMAPEFF();
			return;
		}
	}
	else
	{
		DISABLEALLMAPEFF();
		return;
	}
}

void GLCharacter::UpdateLandEffect()
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if( !pLand ) return;

	if( !pLand->GetUseLevelEtcFunc(EMETCFUNC_LANDEFFECT) ) 
	{
		DISABLEALLLANDEFF();
		return;
	}


	// 모든 버프를 취소해야할 경우의 처리
	const D3DXVECTOR3& vPosition = GetPosition();
	for( DWORD i = 0; i < m_vecLandEffect.size(); i++ )
	{
		SLANDEFFECT_VALUE landEffect = m_vecLandEffect[i];
		if( !landEffect.IsUse() ) continue;
		if( landEffect.emLandEffectType == EMLANDEFFECT_CANCEL_ALLBUFF )
		{
			if( m_dwTransformSkill != SNATIVEID::ID_NULL )
			{
				m_dwTransformSkill = SNATIVEID::ID_NULL;
				UpdateSuit(TRUE);
			}
			for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
			{
				FACTEFF::DeleteSkillFactEffect(STARGETID(CROW_PC, m_dwGaeaID, vPosition), m_pSkinChar, GetSkillFactId(i));
				RESETSKEFF(i);
			}

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );

			return;
		}
	}
}

HRESULT GLCharacter::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;

	UPDATE_RECORD_CHAT();
    
//#if defined(VN_PARAM) //vietnamtest%%%
    if (m_ServiceProvider == SP_VIETNAM)
	    VietnamCalculate();
//#endif

#ifndef CH_PARAM_USEGAIN
	EventCalculate();
#endif

	//	파티채팅 제한시간 업데이트
	if ( m_fPartyRecruitTime <= GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME )
		m_fPartyRecruitTime += fElapsedTime;

	//  1초에 한번씩 업데이트 한다.
    m_fCheckActivityItem += fElapsedTime;
    if ( ActivityBaseData::GetInstance()->m_bSync && m_fCheckActivityItem >= 1.0f )
    {
        UpdateActivityItem();
        m_fCheckActivityItem = 0.0f;
    }

	// 인던 재한 시간에 관한 업데이트
	{
		MAPENTRYINFO_ITER iter = m_mapEntryLimitInfo.begin();
		for( ; iter != m_mapEntryLimitInfo.end();)
		{		
			if( iter->second.IsEntryLimitEnd() )
			{
				m_mapEntryLimitInfo.erase(iter++);
			}else{
				++iter;
			}

		}
	}

	//부스트 사용시간 체크
	if( m_sVehicle.m_bUseBoost )
	{
		m_sVehicle.m_fBoostTime += fElapsedTime;
	
		if( m_sVehicle.m_fBoostTime > 6.0f )
		{
			m_sVehicle.m_fBoostTime = 6.0f;
		}
	}

	if( m_sVehicle.m_bUseBoostDelay )
	{
		m_sVehicle.m_fBoostDelay -= fElapsedTime;

		if( m_sVehicle.m_fBoostDelay <= 0.0f )
		{
			m_sVehicle.m_fBoostDelay = 0.0f;
		}
	}

	//운전자가 부스터 사용중인지 체크
	if( m_emVehicle == EMVEHICLE_PASSENGER )
	{
		std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );

		if( pCharClient )
		{
			if( pCharClient->VehicleUseBoost() )
			{
				DxPostEffect::GetInstance().RadialBlurActiveOn();
			}
			else
			{
                DxPostEffect::GetInstance().RadialBlurActiveOff();
			}
		}
	}

	// 메일 상태 업데이트
	m_pGaeaClient->GetPostClient()->FrameMove( fTime, fElapsedTime );

    // SNS 업데이트
    m_pGaeaClient->GetSNSClient()->FrameMove( fTime, fElapsedTime );

    // 출석부 업데이트
    m_pGaeaClient->GetAttendance()->FrameMove( fTime, fElapsedTime );

    // 필드 점령전 업데이트
    m_pGaeaClient->GetCaptureTheField()->FrameMove( fTime, fElapsedTime );

    //  아이템 정렬 관련 시간 업데이트.
    m_fLimitTimeItemSort -= fElapsedTime;
    if ( m_fLimitTimeItemSort < 0.0f )
        m_fLimitTimeItemSort = 0.0f;

    if ( m_bSendItemSortMsg )
    {
        m_fItemSortMsgDelayTime -= fElapsedTime;
        if ( m_fItemSortMsgDelayTime < 0.0f )
            SetEnableSendItemSortMsg();
    }

	// 만약 기다리고 있다면
	if ( IsActState(EM_ACT_WAITING) )
	{
		if ( ++m_dwWAIT > EMWAIT_COUNT )
		{
			m_dwWAIT = 0;
			ReSetActState(EM_ACT_WAITING);

			//	Note : 맵에 진입함을 서버에 알려줌. ( 주위 환경 정보를 받기 위해서. )
			//
			GLMSG::SNETREQ_LANDIN NetMsgLandIn(m_emDoActWaitTypePrev,m_emDoActWaitType);
			m_pGaeaClient->NETSENDTOAGENT(&NetMsgLandIn);
			CDebugSet::ToListView ( "GLMSG::SNETREQ_LANDIN" );

			//	Note : 서버에 준비되었음을 알림.
			//
			GLMSG::SNETREQ_READY NetMsgReady;
			m_pGaeaClient->NETSENDTOAGENT(&NetMsgReady);
			CDebugSet::ToListView ( "GLMSG::SNETREQ_READY" );

			//	Note : 서버에 친구 리스트 전송 요청.
            //m_pGaeaClient->GetFriendClient()->ReqFriendList();

			// PET
			// 팻이 활동중에 맵을 이동했다면 다시 불러준다.
			if ( m_bIsPetActive )
			{
				 ReqReGenPet ();
				m_bIsPetActive = FALSE;
			}

			/*if ( m_bIsVehicleActive && m_sVehicle.IsActiveValue() ) 
			{
				if ( ReqSetVehicle( true ) == S_OK ) 
				{
					m_bIsVehicleActive = FALSE;
				}
			}*/

			// 탈것 장착하고 있다면 정보 요청 
			if ( m_PutOnItems[SLOT_VEHICLE].GetNativeID() != NATIVEID_NULL()  )
			{
				ReqVehicleUpdate();
			}

			// 인벤의 장착하고 있는 탈것 정보 요청
			if ( !m_bRecivedVehicleItemInfo ) 
			{
				ReqVehicleInvenUpdate();
				m_bRecivedVehicleItemInfo = true;
			}

			// 들고있는 팻카드 정보 요청
			if ( !m_bReceivedPetCardInfo )
			{
				// PET
				// 들고있는 팻카드의 정보를 요청한다.
				ReqPetCardInfo ();

//#if defined KRT_PARAM || defined _RELEASED
#if defined _RELEASED
				// PET
				// 시효만료로 사라진 팻카드의 팻정보 요청
				ReqPetReviveInfo ();
#endif
				m_bReceivedPetCardInfo = true;
			}
		}
	}

	//	Note : 살아있을 때만 갱신되는 것들.
	//

	// Note : 도시락 사용 가능한 지역이면 모든 LUNCHBOX를 Enable 한다
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	BOOL bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
	if ( IsValidBody() && !m_sCONFTING.IsCONFRONTING() && !bLunchBoxForbid )
	{
		SETENABLEALLLUNCHBOX ( TRUE );
	}
	else
	{
		// Note : 모든 LUNCHBOX를 Disable 한다
		SETENABLEALLLUNCHBOX ( FALSE );
	}

	//	Note : 플래이어 조작 처리.
	//		
	GLInputManager::GetInstance().FrameMove( fTime, fElapsedTime );

	if ( IsValidBody() )
	{
		if ( IsCDMSafeTime() )
			m_fCDMSafeTime -= fElapsedTime;		

		m_fQUEST_TIMER += fElapsedTime;

		if ( m_fQUEST_TIMER > 60.0f )
		{
			if ( GLUseFeatures::GetInstance().IsUsingRnattendancePage() )
			{
				ResetRnAttendaceQuest();
			}
			m_cQuestPlay.FrameMove ( fTime, fElapsedTime );
			m_fQUEST_TIMER = 0;
		}
		UpdateMacroState(fElapsedTime);
		UpdateActState();

		//DxInputDevice &dxInputDev = DxInputDevice::GetInstance();

		//DWORD dwF7(DXKEY_IDLE);
		//DWORD dwF8(DXKEY_IDLE);

		//dwF7 = dxInputDev.GetKeyState( DIK_F7 );
		//dwF8 = dxInputDev.GetKeyState( DIK_F8 );


		//// test for tutorial. by luxes.
		//if ( dwF7 & DXKEY_PRESSED )
		//{
		//	m_pGaeaClient->GetTutorial()->SetTutorialStep( GLBaseTutorial::ONE, GLBaseTutorial::ONE_ );
		//}

		//if ( dwF8 & DXKEY_PRESSED  )
		//{
		//	//m_pGaeaClient->GetTutorial()->SetTutorialStep( GLBaseTutorial::TWO, GLBaseTutorial::ONE_ );
		//	m_pGaeaClient->GetTutorial()->NextSubStep();
		//}
		//

		//	Note : 체력 상태 갱신.
		//
		float fCONFT_POINT_RATE(1.0f);
		if ( m_sCONFTING.IsCONFRONTING() )
			fCONFT_POINT_RATE = m_sCONFTING.sOption.fHP_RATE;

		// 지형 효과 업데이트
		UpdateLandEffect();

		//	맵효과 이펙트
		UpdateMapEffect();

        // 스턴중이라도 밀고당기기 액션 중이라면 움직임을 멈추지 않는다
        if ( m_bSTATE_STUN && !isAction( GLAT_PUSHPULL ) )
        {
            ResetReservationAction();
            m_sActionID.RESET();
			
            if ( ClientActor::PathIsActive() && m_emVehicle != EMVEHICLE_PASSENGER )
            {
                ClientActor::StopActorMove();
                TurnAction( GLAT_SHOCK );
            }
        }

		m_sAirborne.update(fElapsedTime);
		GLCHARLOGIC::UPDATE_DATA ( fTime, fElapsedTime, TRUE, fCONFT_POINT_RATE );

		if( RF_FEIGN_DEATH( m_EffSkillVarSet).IsOn() )	
		{
			if( !IsActState( EM_ACT_FEIGN_DIE) ){
				SetActState( EM_ACT_FEIGN_DIE );
				TurnAction( GLAT_FEIGN_FALLING );
			}
		}
	
		if ( m_bCLUB_CERTIFY )
		{
			m_fCLUB_CERTIFY_TIMER += fElapsedTime;

			float fRER = m_fCLUB_CERTIFY_TIMER / GLCONST_CHAR::fCDCERTIFY_TIME;
			if ( fRER > 1.0f )
				fRER = 1.0f;

			//!! SF_TODO
			//m_pInterface->SET_CONFT_CONFIRM_PERCENT ( fRER );
		}		

		if( !IsCtrlBlockBody() )
		{
			if( ( !m_pGaeaClient->GetTutorial()->IsTutorial() ) ||
				( m_pGaeaClient->GetTutorial()->IsTutorial() && m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() ) )
			{
				PlayerUpdate ( fTime, fElapsedTime );
				ReservedActionUpdate ( fTime );
			} //if
		} //if

		if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
		{
			if ( !m_pGaeaClient->GetTutorial()->IsCanMoveInTutorial() )
			{	// 튜토리얼로 인해, 플레이어 조작이 잠겨있으면, 리액션 초기화.
				// Memo : 혹시 몰라, 특정 스텝일 경우에만 리액션 초기화 시킨다.
				//if ( m_pGaeaClient->GetTutorial()->IsAttackStep()
				//	&& m_pGaeaClient->GetTutorial()->IsSubThreeStep()
				//	|| m_pGaeaClient->GetTutorial()->IsAttackStep()
				//	&& m_pGaeaClient->GetTutorial()->IsSubFiveStep() )
				//{
				ResetReservationAction();
				//}
			}

			m_pGaeaClient->GetTutorial()->FrameMove( fElapsedTime, this );
		}

		if( m_bMiniMapMove && isAction( GLAT_MOVE ) )
		{
			m_fMiniMapMoveTime += fElapsedTime;
			if( m_fMiniMapMoveTime >= 1.0f )
			{
				m_fMiniMapMoveTime = 0.0f;

				// 이거 호출해주면 미니맵 클릭 이동 시 서버와 동기화 차이에 의한
				// 강제 롤백 현상은 수정할 수 있지만 서버에 부담이 된다
				LargeMapMoveTo( &m_sActionID.vPos, TRUE );
			}
		}
	}

	//// 선택 대상 업데이트
	//SelectTargetUpdate();

	if ( !IsActState(EM_ACT_CONTINUEMOVE) )
	{
		if (m_sLastMsgMove != m_sLastMsgMoveSend)
		{
			m_fLastMsgMoveSend = fTime;
			m_sLastMsgMoveSend = m_sLastMsgMove;
			m_pGaeaClient->NETSENDTOFIELD( &m_sLastMsgMoveSend );
		}
	}

	//	Note : 클럽마크 버전 확인.
	//
	m_pGaeaClient->ReqClubMarkInfo(m_pMyClub->DbNum(), m_pMyClub->m_dwMarkVER);

	//	트레이드 상태 갱신.
	m_pGaeaClient->GetTradeClient()->FrameMove ( fTime, fElapsedTime );

	//	Note : 타겟이 무효화 되는지 점검한다.
	//
	ClientActor* pTarget = m_pGaeaClient->GetCopyActor ( m_sActionID );
	if ( !pTarget )
	{
		m_sActionID.GaeaId = EMTARGET_NULL;
		if ( isAction(GLAT_ATTACK) )
			TurnAction ( GLAT_IDLE );
	}

	//	Note : Gate 검사.
	//
	DWORD dwDetectGate = DetectGate ();
	if ( dwDetectGate!=UINT_MAX )
	{
		m_pGaeaClient->SetGateOpen( TRUE );
	}

	//// Note : 헤어를 Edit 해야 할 경우만 변경한다.
	//if( m_bEnableHairSytle )
	//	HairStyleUpdate();
	//if( m_bEnableHairColor )	
	//	HairColorUpdate();
	//if( m_bEnableFaceStyle )
	//	FaceStyleUpdate();

	m_pWorkingSkill->FrameMove( fElapsedTime );

	if( m_pSkinCharMulti )
	{
		BOOL bUpdateSuit = FALSE;
		BOOL bIsCloneSkill = IsCheckedSkillFlagSpecial(EMSPECA_CLONE);
		bUpdateSuit = m_pSkinCharMulti->SetUseSkinCharMulti( bIsCloneSkill, m_sClone.GetClonNum());

		if ( m_pSkinCharMulti->IsUpdateSuit() && bUpdateSuit )
			UpdateSuit( TRUE, TRUE );

		if ( !bIsCloneSkill )
			m_sClone.Reset();
	}

	//UpdateAnimation ( fTime, fElapsedTime, GetAction() );
	UpdateAction (fTime, fElapsedTime );

	// [AnimationMove]
	SkinAniControl_AnimationMove();

	// [shhan][2015.01.27] RefreshPosition 를 안해주니 PUSHPULL 될 시 캐릭터 형상과 이름, 타켓팅 되는 위치가 따로 분리되는 문제가 생겼다.
	//						m_vPosition 것을 없앨 경우는 PUSHPULL Test 도 같이 해봐야 할것 같음.
	ClientActor::RefreshPosition(0.f);

	//const D3DXVECTOR3& vPosition = ClientActor::GetPositionActor();	//RefreshPosition(0.0f);//GetPosition();

	FACTEFF::UpdateSkillEffect( m_pGaeaClient, STARGETID(CROW_PC,m_dwGaeaID,GetPositionActor()), m_pSkinChar, m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS, TRUE );	// 스킬 이펙트 업데이트.

	UpdateSkillEff( fTime, fElapsedTime );
	UpdateSpecialSkill( fElapsedTime );

	// 여기서 DxSkinChar 의 최종 FrameMove 가 이루어 진다.
	// 이 시점 부터 렌더링 끝날 때 까지 Part 를 변경하는 작업이 이루어지면 문제가 발생한다.
	UpdateAnimation ( fTime, fElapsedTime, GetAction() );

	//	?Item 지속 화면 효과. on/off
	if ( m_emOldQuestionType!=m_sQITEMFACT.emType )
	{
		m_emOldQuestionType = m_sQITEMFACT.emType;

		switch ( m_sQITEMFACT.emType )
		{
		case QUESTION_NONE:
			{
				//	?Item 화면 효과.
				DxEffProjMan::GetInstance().EnableGetItemEFF ( FALSE );

				//	?Item BGM.
				if ( !pLand )		break;

				DxLandMan* pLandMan = pLand->GetLandMan();
				if ( !pLandMan )		break;

				const CString &strBgm = pLandMan->GetBgmFile();
				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm );
				DxBgmSound::GetInstance().Play();
			}
			break;

		case QUESTION_SPEED_UP:
		case QUESTION_CRAZY:
		case QUESTION_ATTACK_UP:
		case QUESTION_EXP_UP:
		case QUESTION_LUCKY:
		case QUESTION_SPEED_UP_M:
		case QUESTION_MADNESS:
		case QUESTION_ATTACK_UP_M:
			{
				if ( RANPARAM::bBuff )
					DxEffProjMan::GetInstance().EnableGetItemEFF ( TRUE );

				const std::string &strBgm = GLCONST_CHAR::strQITEM_BGM[m_sQITEMFACT.emType];
				if ( strBgm.empty() || strBgm=="null" )		break;

				const char *szCurBgm = DxBgmSound::GetInstance().GetFile();
				if ( strBgm==szCurBgm )	break;

				DxBgmSound::GetInstance().ForceStop ();
				DxBgmSound::GetInstance().ClearFile ();

				DxBgmSound::GetInstance().SetFile ( strBgm.c_str() );
				DxBgmSound::GetInstance().Play();
			}
			break;
		};
	}

	//	if( m_sQITEMFACT.IsACTIVE() ) m_sQITEMFACT.fTime -= fElapsedTime;

	//	Note : 현제 행동 상태 확인용.
	//
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		if( m_pSkinChar )
		{	
			CDebugSet::ToView(5,CGameTextMan::GetInstance().GetCommentText("ANI_MAINTYPE", m_pSkinChar->GETCURMTYPE()).GetString());
			CDebugSet::ToView(5,CGameTextMan::GetInstance().GetCommentText("ANI_SUBTYPE", m_pSkinChar->GETCURSTYPE()).GetString());
		} 
	}
	else
	{
		if( m_pSkinChar )
		{	
			CDebugSet::ToView ( 5, COMMENT::ANI_MAINTYPE[m_pSkinChar->GETCURMTYPE()].c_str() );
			CDebugSet::ToView ( 6, COMMENT::ANI_SUBTYPE[m_pSkinChar->GETCURSTYPE()].c_str() );
		} 
	}//if

	// 아래의 COMMENT는 데이터에서 읽어오지않는다;
	CDebugSet::ToView ( 8, COMMENT::ACTIONTYPE[GetAction()].c_str() );
	CDebugSet::ToView ( 9, "char pos : %4.1f, %4.1f, %4.1f", ClientActor::GetPositionActor().x, ClientActor::GetPositionActor().y, ClientActor::GetPositionActor().z );

	// 추적 유저일 경우에 정보들을 남긴다.
	/*if( m_bTracingUser )
	{
		PrintTracingUserWindowInfo();
		PrintTracingUserProcessInfo( fElapsedTime );
	}*/


	if ( m_pGaeaClient->IsCTFPlayer() )
	{
		m_dwActionLimit |= (EMACTION_LIMIT_PET | EMACTION_LIMIT_DRUG_HP | EMACTION_LIMIT_CARD);
	}



	//////////////////////////////////////////////////////////////////////////
	// Skin 관련 위치를 사용한다. ( vPositionOfSkin ) ( airbone 적용된 값 )
	//////////////////////////////////////////////////////////////////////////

	const D3DXVECTOR3& vPositionOfSkin = ClientActor::GetPositionActor() + D3DXVECTOR3(0.0f, m_sAirborne.getHeight(), 0.0f);

	m_vMax = vPositionOfSkin + m_vMaxOrg;
	m_vMin = vPositionOfSkin + m_vMinOrg;

	//////////////////////////////////////////////////////////////////////////
	//			ActorWorldLight 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	GLLandManClient* const pLandManClient = m_pGaeaClient->GetLandManClient();
	if ( pLandManClient )
	{
		m_spWorldLight->InitData( pLandManClient->GetLandManSp() );
		NSSkinAniThread::AddActorWorldLight( m_spWorldLight, vPositionOfSkin );
	}

	return S_OK;
}

HRESULT GLCharacter::FrameMoveForTool( float fTime, float fElapsedTime )
{
	HRESULT hr=S_OK;	

	//	Note : 에니메이션, 스킨 업데이트.
	//
//	UpdateAnimation ( fTime, fElapsedTime );


	////	Note : 스킬 이펙트 업데이트.
	////

	//InitializeActorMove(D3DXVECTOR3( 0.0f, 0.0f, 0.0f ));
	//
	//const D3DXVECTOR3 vDirect = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
	//ClientActor::SetDirect(vDirect);

	////	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	////
	//D3DXMATRIX matTrans, matYRot;
	//D3DXMatrixTranslation ( &matTrans, 0.0f, 0.0f, 0.0f );
	//float fThetaY = DXGetThetaYFromDirection ( vDirect, GetDirectOrig() );
	//D3DXMatrixRotationY ( &matYRot, fThetaY );

	//SetTransMatrix(D3DXMATRIX(matYRot * matTrans));

	// [shhan][2014.09.22] EffectTool 에서 기본 캐릭터 방향을 변경해 달라고 해서 작업함.
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	if ( !m_pSkinChar->ISENDANIM() )			
		m_pSkinChar->FrameMove ( fTime, fElapsedTime, FALSE, matIdentity );

	m_vMax = m_vMaxOrg;
	m_vMin = m_vMinOrg;
	return S_OK;
}

void GLCharacter::PrintTracingUserProcessInfo( float fElapsedTime )
{


		// 5분마다 한번씩
	if( m_fPrintProcessInfoMsgGap == -10.0f || m_fPrintProcessInfoMsgGap > 600 )
	{
		m_fPrintProcessInfoMsgGap = 0.0f;

		// 프로세스 스냅샷 핸들을 얻는다.
		HANDLE h_snap_shot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

		// 스냅샷 되었을때 프로세스 주소내에 열거된 리스트를 저장하는 구조체
		PROCESSENTRY32 entry_data;
		entry_data.dwSize = sizeof(entry_data);

		// 첫번째 프로세스의 정보를 얻는다.
		char continue_flag = Process32First(h_snap_shot, &entry_data);         
		while(continue_flag)
		{
			char temp[256] = {0, };

			char name[MAX_PATH];
			unsigned int id;
			unsigned int thread_counts;
			unsigned int parent_id;
			unsigned int priority;
			PROCESS_MEMORY_COUNTERS pmc;            

			// 프로세스 아이디를 이용하여 해당 프로세스 핸들 값을 얻는다.
			HANDLE hwnd = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,entry_data.th32ProcessID);

			if(hwnd != NULL){
				// 정상적으로 프로세스를 열었다면..
				// 프로세스의 경로명을 얻는다.
				// 정상적으로 경로명을 얻었다면 해당 경로명을 그렇지 못했다면 파일명만을 출력한다.
				if(GetModuleFileNameEx(hwnd, NULL, temp, 256)) 	strcpy_s(name,MAX_PATH,temp);
				else strcpy_s(name, MAX_PATH, entry_data.szExeFile);				
				GetProcessMemoryInfo( hwnd, &pmc, sizeof(pmc));
				CloseHandle(hwnd);
			} else {
				// 정상적으로 프로세스를 열지 못했다면..
				strcpy_s(name, MAX_PATH, entry_data.szExeFile);

			}
			id = entry_data.th32ProcessID;
			thread_counts = entry_data.cntThreads;
			parent_id = entry_data.th32ParentProcessID;
			priority = entry_data.pcPriClassBase;
			

			GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
			TracingMsg.nUserNum  = GetUserID();
			TracingMsg.SetAccount(m_szUID);
			CString strTemp;
			int memory = (int)(pmc.WorkingSetSize / 1024);
			strTemp.Format(
                "ProcessInfo, ProcessName[%s], Memory[%dKB], Thread count[%d]",
                name, memory, thread_counts);

			TracingMsg.SetLog(strTemp.GetString());
			m_pGaeaClient->NETSENDTOAGENT(&TracingMsg);

			// 다음 프로세스의 정보를 얻는다.
			continue_flag = Process32Next(h_snap_shot, &entry_data);
		}

		CloseHandle(h_snap_shot);
	}
    else
    {
		m_fPrintProcessInfoMsgGap += fElapsedTime;
	}
}

void GLCharacter::PrintTracingUserWindowInfo()
{
	BYTE dwID[] = { INVENTORY_WINDOW, CHARACTER_WINDOW, SKILL_WINDOW, PARTY_WINDOW, QUEST_WINDOW, CLUB_WINDOW, FRIEND_WINDOW, MAP_WINDOW };

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
		dwID[ 0 ] = INVENTORY_WINDOW_RENEW;

	for( BYTE i = 0; i < 8; i++ )
	{
		//!! SF_TODO
		//m_bNewVisibleTracingUI[i] = (0!=m_pInterface->UiIsVisibleGroup( (UIGUID)dwID[i] ));
		if( m_bOldVisibleTracingUI[i] != m_bNewVisibleTracingUI[i] )
		{
			m_bOldVisibleTracingUI[i] = m_bNewVisibleTracingUI[i];
			if( m_bNewVisibleTracingUI[i] == TRUE )
			{
				GLMSG::NET_LOG_UPDATE_TRACINGCHAR TracingMsg;
				TracingMsg.nUserNum  = GetUserID();
				TracingMsg.SetAccount(m_szUID);
				CString strTemp;

				switch (i)
				{
				case 0:
					strTemp.Format( "Open Inventory Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 1:
					strTemp.Format( "Open Character Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 2:
					strTemp.Format( "Open Skill Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 3:
					strTemp.Format( "Open Party Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 4:
					strTemp.Format( "Open Quest Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 5:
					strTemp.Format( "Open Club Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 6:
					strTemp.Format( "Open Friend Window, [%d][%s]", GetUserID(), m_szName );
					break;
				case 7:
					strTemp.Format( "Open Mini Map, [%d][%s]", GetUserID(), m_szName );
					break;
				}
				TracingMsg.SetLog(strTemp.GetString());

				m_pGaeaClient->NETSENDTOAGENT(&TracingMsg);
			}
		}
	}
}

float GLCharacter::GetDirection ()
{
	return DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
}

void GLCharacter::CalculateMatrix( const D3DXVECTOR3& vPosition )
{
	//	Note : 현재 위치와 방향으로 Transform 메트릭스 계산.
	//
	std::tr1::shared_ptr<GLCharClient> pCharClient;
	D3DXMATRIX matTrans, matYRot, matPassenger;
	D3DXVECTOR3 vPos;
	DxVehicle* pSkinChar = NULL;	
	if( m_emVehicle )
	{
		for( WORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
		{
			if (m_sVehicle.PassengerId(i) == m_dwGaeaID)
			{
				if( i == 0 )
				{
					pSkinChar = m_sVehicle.m_pSkinChar;
				}
				else
				{
					pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );
					GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
					if( pCharClient )
					{
						vPos = pCharClient->GetPosition();
						ClientActor::InitializeActorMove( vPos );
					}
				}
				break;
			}
		}

		if( pSkinChar )
		{
			D3DXVECTOR3 vFrontHighPos, vFrontLowPos, vBackHighPos, vBackLowPos;
			D3DXVECTOR3 vAnswerFrontPos, vAnswerBackPos;
			DWORD dwCollID;
			BOOL bFrontColl, bBackColl;
			D3DXVECTOR3 vAngle;
			const D3DXVECTOR3& vDirect = GetDirect();
			const D3DXVECTOR3& vDirectOrig = GetDirectOrig();

			vFrontHighPos = vFrontLowPos = vPosition + ( vDirect * pSkinChar->m_vMax.x );
			vFrontHighPos.y += 10.0f;
			vFrontLowPos.y -= 10.0f;

			vBackHighPos = vBackLowPos = vPosition + ( - vDirect * pSkinChar->m_vMax.x );
			vBackHighPos.y += 10.0f;
			vBackLowPos.y -= 10.0f;

			ClientActor::IsCollision(vFrontHighPos, vFrontLowPos, vAnswerFrontPos, dwCollID, bFrontColl);
			ClientActor::IsCollision(vBackHighPos, vBackLowPos, vAnswerBackPos, dwCollID, bBackColl);			

			PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
			DxLandMan* pLandMan(NULL);
			if ( pLand )
			{
				pLandMan = pLand->GetLandMan();
			}

			D3DXMATRIX matTrans;
			if ( pLandMan && pSkinChar->GetFootColl()->GetMatrix( matTrans, pLandMan, vPosition, GetDirect(), GetDirectOrig(), const_cast< NavigationMesh* >( GetParentMesh() ) ) )
			{
				m_matBikeTrans = matTrans;
				m_matBikeTrans._41 = vPosition.x;
				m_matBikeTrans._42 = vPosition.y;
				m_matBikeTrans._43 = vPosition.z;
			}
			else
			{
				if( bFrontColl && bBackColl )
				{
					vAngle = vAnswerFrontPos - vAnswerBackPos;				
					D3DXVec3Normalize( &vAngle, &vAngle );				

					FLOAT fTheta = (vAngle.y * D3DX_PI / 3.0f );
					D3DXMatrixRotationX( &m_matBikeTrans, fTheta );

					D3DXMATRIX matRot;
					fTheta = DXGetThetaYFromDirection( vDirect, vDirectOrig );
					D3DXMatrixRotationY( &matRot, fTheta );
					D3DXMatrixMultiply( &m_matBikeTrans, &m_matBikeTrans, &matRot );

					m_matBikeTrans._41 = vPosition.x;
					m_matBikeTrans._42 = vPosition.y;
					m_matBikeTrans._43 = vPosition.z;
				}
				else
				{				
					D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
					const float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
					D3DXMatrixRotationY ( &matYRot, fThetaY );

					m_matBikeTrans = matYRot * matTrans;
				}
			}
		}
		else
		{
			const D3DXVECTOR3& vDirect = GetDirect();
			const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
			D3DXMATRIX matTrans, matYRot;
			D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
			float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
			D3DXMatrixRotationY ( &matYRot, fThetaY );
			SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
		}
	}
	else
	{
		const D3DXVECTOR3& vDirect = GetDirect();
		const D3DXVECTOR3& vDirectOrig = GetDirectOrig();
		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
		float fThetaY = DXGetThetaYFromDirection ( vDirect, vDirectOrig );
		D3DXMatrixRotationY ( &matYRot, fThetaY );
		SetTransMatrix(D3DXMATRIX(matYRot * matTrans));
	}

	//std::tr1::shared_ptr<GLCharClient> pCharClient;
	//const SPassengerData* pPassengerData = NULL;
	//D3DXMATRIX matYRot, matPassenger;
	//SITEM* pItem = GET_SLOT_ITEMDATA ( SLOT_VEHICLE );

	//if( pItem )
	//{
	//	if( m_emVehicle == EMVEHICLE_DRIVER && pItem->sVehicle.emVehicleType != VEHICLE_TYPE_BOARD )
	//	{
	//		// Vehicle의 운전자가 현재 자신인지 확인한다.
	//		if ( m_dwGaeaID != m_sVehicle.PassengerId(0) )
	//			return;

	//		// Skin 이 만들어졌는지 확인한다.
	//		DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;
	//		if ( !pVehicle )
	//			return;

	//		// PassengerMatrix Data 가 정상인지 아닌지 확인한다.
	//		if ( !pVehicle->IsValidPassengerMatrixData() )
	//			return;

	//		//pVehicle->UpdatePassengerPos();

	//		for( DWORD i = 0; i < MAX_PASSENGER_COUNT; i++ )
	//		{
	//			// PassengerData 가 정상인지 확인한다.
	//			pPassengerData = pVehicle->GetPassengerData(i);
	//			if( !pPassengerData )
	//				continue;

	//			if( i == 0 )	// 운전자, 자기 자신
	//			{
	//				SetVehiclePassengerID( i );
	//				SetTransMatrix(pPassengerData->matWorld);
	//			}
	//			else
	//			{
	//				pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(i) );
	//				if( pCharClient )
	//				{
	//					pCharClient->SetVehiclePassengerID( i );
	//					pCharClient->SetTransMatrix( pPassengerData->matWorld );
	//				}
	//			}
	//		}
	//	}
	//	else if( m_emVehicle == EMVEHICLE_PASSENGER )
	//	{
	//		// 탑승자 입장이라면, 위치좌표는 그 Vehicle 의 운전자쪽에서 계산이 되도록 한다.
	//		// 여기에서는 Vehicle 정보가 없기 때문이다.
	//		return;
	//	}
	//	else
	//	{
	//		const D3DXVECTOR3& vPosition = GetPosition();
	//		D3DXMATRIX matTrans = GetTransMatrix();
	//		D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//		float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//		D3DXMatrixRotationY ( &matYRot, fThetaY );

	//		const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//		SetTransMatrix(matRotTrans);
	//	}

	//}
	//else
	//{
	//	if( m_emVehicle )
	//		return;

	//	const D3DXVECTOR3& vPosition = GetPosition();
	//	D3DXMATRIX matTrans = GetTransMatrix();
	//	D3DXMatrixTranslation ( &matTrans, vPosition.x, vPosition.y, vPosition.z );
	//	float fThetaY = DXGetThetaYFromDirection ( GetDirect(), GetDirectOrig() );
	//	D3DXMatrixRotationY ( &matYRot, fThetaY );

	//	const D3DXMATRIX matRotTrans = matYRot * matTrans;
	//	SetTransMatrix(matRotTrans);
	//}
}

// Vehicle. 내가 어느 위치에 앉아 있는가~?
void GLCharacter::SetVehiclePassengerID( DWORD dwID )
{
	m_dwVehiclePassengerID = dwID;
}

DWORD GLCharacter::GetVehiclePassengerID() const
{
	return m_dwVehiclePassengerID;
}

void GLCharacter::SkinAniControl_AnimationMove()
{
	if( m_bFirstAnimationMove )
	{
		if( !m_pSkinChar->IsAnimationMove() )
		{
			m_bFirstAnimationMove = FALSE;

			// 서버 쪽에 내위치 옮겨주기.
			GLMSG::SNETPC_ANIMOVE_END_CF NetMsg;
			NetMsg.m_vCurPos = GetPositionActor();
			m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
		}
	}
	else
	{
		if( m_pSkinChar->IsAnimationMove() )
		{
			m_bFirstAnimationMove = TRUE;

			const DxSkinAnimation* pSkinAnimation = m_pSkinChar->GETCURANIM();
			if ( pSkinAnimation )
			{
				// 서버 쪽에 내위치 옮겨주기.
				GLMSG::SNETPC_ANIMOVE_START_CF NetMsg;
				NetMsg.m_fAngleY = DXGetThetaYFromDirection( GetDirect(), GetDirectOrig() );
				m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
			}
		}
	}

	// 애니메이션에서 캐릭터를 이동시켜줬을 수도 있으니 확인하자.
	if( m_pSkinChar->IsAnimationMove() )
	{
		D3DXVECTOR3 vPos;
		D3DXMATRIX matRotate( GetTransMatrix() );
		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		D3DXVec3TransformCoord( &vPos, &m_pSkinChar->GetAnimationMovePos(), &matRotate );

		////  [8/4/2013 gbgim];
		//// - 애니메이션 무브일 경우 퍼센테이지 이동 옵션이 있다면;
		////   추가 퍼센테이지 만큼 이동시킨다;
		//GLSkillMain* pSkillMain = m_pSkillPlayer->GetSkillMain();
		//if ( pSkillMain != NULL && IsACTION( GLAT_SKILL ) )
		//{
		//	if ( pSkillMain->IsAnimMoveDistRatioControl() == TRUE )
		//	{
		//		vPos *= pSkillMain->GetAnimMoveDistRatio();
		//	}
		//}

		vPos += GetPositionActor();
		SetPositionActor( vPos );
	}
}

void GLCharacter::UpdateCharPos_RenderChar( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	// 보드일 경우 m_sVehicle.m_pSkinChar 가 NULL 일 수도 있다.
	if( m_emVehicle==EMVEHICLE_DRIVER && m_sVehicle.m_pSkinChar )
	{
		RenderMyChar( pd3dDevice, m_sVehicle.m_pSkinChar, TRUE, bHALF_VISIBLE );
	}

	DxSkinChar* pRenderChar = NULL;
	std::vector<DxSkinChar*> vecRenderCharTest;	
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform )) == NULL )
	{
		pRenderChar = m_pSkinChar;
	}

	RenderMyChar( pd3dDevice, pRenderChar, TRUE, bHALF_VISIBLE );
	
}

void GLCharacter::UpdateCharPos_RenderChar_SoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
{
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

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

void GLCharacter::UpdateCharPos_RenderChar_MaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bHALF_VISIBLE )
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

void GLCharacter::RenderMyChar( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bOpaqueState, BOOL bHALF_VISIBLE )
{
	if ( !pRenderChar )
		return;

	// Effect 툴에서는 검사하지 않는다.
	if ( !g_bEFFECT_TOOL )
	{
		if ( m_spWorldLight->IsNotCalculateColor() )
			return;
	}

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
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 0.5f  );
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
				pRenderChar->RenderFixed( pd3dDevice, TRUE, 1.f  );
			}
			else
			{
				pRenderChar->SetPhysX_Cloth( RENDERPARAM::g_emPhysXCloth >= PHYSX_CLOTH_LEVEL_MY  ? TRUE : FALSE );
				pRenderChar->RenderMaterialOpaque ( pd3dDevice, 1.f  );
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

	if ( m_pSkinCharMulti && m_emVehicle!=EMVEHICLE_DRIVER && !m_sVehicle.m_pSkinChar  )
		m_pSkinCharMulti->RenderMyChar( pd3dDevice, TRUE, bHALF_VISIBLE, m_spWorldLight->m_vDiffuseResult, m_spWorldLight->m_vPointLightResult, TRUE );
}

void GLCharacter::RenderMyCharMaterialEffect( LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharBase* pRenderChar, BOOL bHALF_VISIBLE )
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

HRESULT GLCharacter::Render ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	DxSkinChar* pRenderChar = NULL;
	std::vector<DxSkinChar*> vecRenderChar;
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
	{
		pRenderChar = m_pSkinChar;
	}

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]이 참일 경우 반투명하게 표현해야함.
	//		
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		if ( bHALF_VISIBLE )
		{
			PutPassiveEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str(), &GetDirect() );
		}
		else
		{
			OutEffect ( pRenderChar, GLCONST_CHAR::strHALFALPHA_EFFECT.c_str() );
		}
	}

	if( RENDERPARAM::bClickEffect )
	{
		STARGETID sTargetID(CROW_PC,m_CharDbNum,m_sActionID.vPos);
		
		if(isAction(GLAT_MOVE) && m_emVehicle != EMVEHICLE_PASSENGER )
		{
			D3DXMATRIX matEffect;
			D3DXMatrixTranslation ( &matEffect, m_sActionID.vPos.x, m_sActionID.vPos.y, m_sActionID.vPos.z );

			DxEffGameHelp::GetInstance().UpdateGameHelpEffect( EMCLICK_EFFECT, matEffect, sTargetID );
		}
		else
		{
			DxEffGameHelp::GetInstance().ResetClickEffect();
			DxEffGameHelp::GetInstance().BlindEff( EMCLICK_EFFECT );
		}
	}

	//// WorldEdit 에서 쓰이는 DxLandMan 값도 사용되어야 해서 참조값으로 받아서 사용하게 됨.
	//if ( pLandMan )
	//{
	//	CalculateAmbientColor( pd3dDevice, pLandMan, GetPosition() );
	//}

	//if ( m_sVehicle.m_pSkinChar && m_emVehicle == EMVEHICLE_DRIVER )
	//{
	//	//if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	//	//{
	//	//	m_sVehicle.m_pSkinChar->RenderFixed( pd3dDevice, TRUE, 1.f, m_matBikeTrans );
	//	//}
	//	//else
	//	//{
	//	//	m_sVehicle.m_pSkinChar->RenderMaterialOpaque( pd3dDevice, 1.f, m_matBikeTrans );
	//	//	m_sVehicle.m_pSkinChar->RenderMaterialHardAlpha( pd3dDevice, 1.f );
	//	//}
	//	//
	//	//m_sVehicle.m_pSkinChar->UpdatePassengerPos();

	//	if ( pLandMan )
	//	{
	//		m_sVehicle.m_pSkinChar->CalculateAmbientColor( pd3dDevice, pLandMan, D3DXVECTOR3(m_matBikeTrans._41,m_matBikeTrans._42,m_matBikeTrans._43) );
	//	}
	//}

    //
 	UpdateCharPos_RenderChar( pd3dDevice, bHALF_VISIBLE );

	if( dxincommand::bDISP_POSITION )
	{
		EDITMESHS::RENDERSPHERE(
			pd3dDevice,
			m_vServerPos,
			D3DXVECTOR3( 2.0f, 2.0f, 2.0f ),
			TRUE,
			NULL,
			D3DCOLOR_ARGB( 255, 0, 255, 0 ) );
		
		EDITMESHS::RENDERSPHERE(
			pd3dDevice,
			ClientActor::GetPositionActor(),
			D3DXVECTOR3( 3.0f, 3.0f, 3.0f ),
			FALSE,
			NULL,
			D3DCOLOR_ARGB( 255, 255, 0, 0 ) );
	}

	//D3DXVECTOR3 vMax(m_vPos.x+MAX_VIEWRANGE, m_vPos.y+20, m_vPos.z+MAX_VIEWRANGE);
	//D3DXVECTOR3 vMin(m_vPos.x-MAX_VIEWRANGE, m_vPos.y-20, m_vPos.z-MAX_VIEWRANGE);
	//EDITMESHS::RENDERAABB ( pd3dDevice,  vMax, vMin );

	return S_OK;
}

HRESULT GLCharacter::RenderSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]이 참일 경우 반투명하게 표현해야함.
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);

	UpdateCharPos_RenderChar_SoftAlpha( pd3dDevice, bHALF_VISIBLE );

	return S_OK;
}

HRESULT GLCharacter::RenderMaterialSkinEffect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		return S_OK;

	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : m_bSkillFacts[EMSPECA_INVISIBLE]이 참일 경우 반투명하게 표현해야함.
	bool bHALF_VISIBLE = IsCheckedSkillFlagSpecial(EMSPECA_INVISIBLE) || IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE);

	UpdateCharPos_RenderChar_MaterialEffect( pd3dDevice, bHALF_VISIBLE );

	return S_OK;
}

HRESULT GLCharacter::RenderShadow ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bWorldSystemOLD )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if ( IsActState(EM_REQ_VISIBLEOFF) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : 그림자 랜더링.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bWorldSystemOLD );
		//m_sVehicle.m_pSkinChar->RenderShadow( pd3dDevice, m_matBikeTrans, TRUE );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )
	{
		//if ( m_emVehicle == EMVEHICLE_PASSENGER )
		//{
		//	const WORD i = m_sVehicle.GetPassengerIndex( m_dwGaeaID );
		//	std::tr1::shared_ptr<GLCharClient> pCharClient = m_pGaeaClient->GetChar( m_sVehicle.PassengerId(0) );			
		//	if ( pCharClient )
		//		DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, pCharClient->VehicleSkinChar()->m_sPassengerData[i].matWorld, pd3dDevice, bUsedMaterialSystem );
		//}
		//else
		//{
			DxShadowMap::GetInstance().RenderShadowCharMob ( pRenderChar, GetTransMatrix(), pd3dDevice, bWorldSystemOLD );
		//}
	}

    return S_OK;
}

HRESULT GLCharacter::RenderShadow_SpotLight ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bUsedMaterialSystem, DWORD dwSlot )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
		return S_FALSE;

	if ( IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE) )
		return S_FALSE;

	if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : 그림자 랜더링.
	//
	if ( m_sVehicle.m_pSkinChar )
	{
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( m_sVehicle.m_pSkinChar, m_matBikeTrans, pd3dDevice, bUsedMaterialSystem, dwSlot );
	}

	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )
		DxShadowMap::GetInstance().RenderShadowCharMob_SpotLight( pRenderChar, GetTransMatrix(), pd3dDevice, bUsedMaterialSystem, dwSlot );

	return S_OK;
}

HRESULT GLCharacter::RenderReflect( LPDIRECT3DDEVICEQ pd3dDevice, float fWaterHeight )
{
	if ( IsActState(EM_REQ_VISIBLENONE) )
        return S_FALSE;

    if ( IsActState(EM_REQ_VISIBLEOFF + EM_ACT_OBSERVE) )
        return S_FALSE;

    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_FALSE;

	//	Note : 반사 랜더링.
	//
	DxSkinChar* pRenderChar = NULL;		
	if ( (pRenderChar = m_pTransformSet->GetTransformSkinChar(m_sHALLUCINATE.dwTransformID, m_sHALLUCINATE.bTransform)) == NULL )
		pRenderChar = m_pSkinChar;		

	if ( pRenderChar )	
	{
		DxEnvironment::GetInstance().RenderRefelctChar( pRenderChar, pd3dDevice, fWaterHeight );
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		DxEnvironment::GetInstance().RenderRefelctChar( m_sVehicle.m_pSkinChar, pd3dDevice, fWaterHeight );
	}


	return S_OK;
}

HRESULT GLCharacter::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InitDeviceObjects( pd3dDevice );
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->InitDeviceObjects( pd3dDevice );
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->InitDeviceObjects( pd3dDevice );
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->InitDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT GLCharacter::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->RestoreDeviceObjects ( pd3dDevice );
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->RestoreDeviceObjects ( pd3dDevice );
	}

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->RestoreDeviceObjects ( m_pd3dDevice );
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT GLCharacter::InvalidateDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->InvalidateDeviceObjects ();
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->InvalidateDeviceObjects ();
	}

	if( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->InvalidateDeviceObjects();
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT GLCharacter::DeleteDeviceObjects ()
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->DeleteDeviceObjects ();
	}

	if ( m_pTransformSet )
	{
		m_pTransformSet->DeleteDeviceObjects ();
	}

	if ( m_sVehicle.m_pSkinChar )
	{
		m_sVehicle.m_pSkinChar->DeleteDeviceObjects();
	}

	//if ( m_pSkinCharMulti )
	//	m_pSkinCharMulti->DeleteDeviceObjects();

	return S_OK;
}

EMELEMENT GLCharacter::GET_ITEM_ELMT ()
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

void GLCharacter::StartAttackProc()
{
	PLANDMANCLIENT		pLAND = m_pGaeaClient->GetActiveMap();
	if( pLAND && pLAND->IsPeaceZone() )
		return;

	if( IsActState(EM_ACT_PEACEMODE) )
		ReqTogglePeaceMode();

	if( m_pSkinChar )
		m_pSkinChar->ResetStrikeAttachBone();

	m_nattSTEP		= 0;
	m_fattTIMER		= 0.0f;

	EMSLOT			emRHand			= GetCurRHand();
	EMSLOT			emLHand			= GetCurLHand();
	EMANI_SUBTYPE	emANISUBTYPE	= CHECK_ATTACK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );
	DWORD			dwAMOUNT		= m_pSkinChar->GETANIAMOUNT ( AN_ATTACK, emANISUBTYPE );

	m_dwANISUBSELECT = m_dwANISUBCOUNT++;
	if( m_dwANISUBCOUNT >= dwAMOUNT )
	{
		m_dwANISUBCOUNT = 0;
	}

	//	Note : 공격 메시지 발생.
	GLMSG::SNETPC_ATTACK NetMsg;
	NetMsg.emTarCrow	= m_sActionID.emCrow;
	NetMsg.dwTarID		= m_sActionID.GaeaId;
	NetMsg.dwAniSel		= m_dwANISUBSELECT;
	NetMsg.dwFlags		= NULL;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_bAttackable = FALSE;
}

void GLCharacter::AttackEffect ( const SANIMSTRIKE &sStrikeEff )
{
	BOOL bOk = m_pGaeaClient->ValidCheckTarget ( m_sActionID );
	if ( !bOk )	return;

	ClientActor* pActor = m_pGaeaClient->GetCopyActor ( m_sActionID );
	if ( pActor )	pActor->ReceiveSwing ();

	//	타겟의 위치.
	D3DXVECTOR3 vTARPOS = m_pGaeaClient->GetTargetPos ( m_sActionID );
	const D3DXVECTOR3& vPosition = GetPosition();

	D3DXVECTOR3 vDir = vTARPOS - vPosition;
	vDir.y = 0.f;
	D3DXVec3Normalize ( &vDir, &vDir );

	vTARPOS.y += 15.0f;

	D3DXVECTOR3 vTarDir = vTARPOS - vPosition;
	float fLength = D3DXVec3Length ( &vTarDir );

	D3DXVec3Normalize ( &vTarDir, &vTarDir );
	D3DXVECTOR3 vTARPAR = vPosition + vTarDir*fLength * 10.0f;

	//	타격 위치를 알아냄.
	STRIKE::SSTRIKE sStrike;

	bool bAttBone = false;
	if ( m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
	{
		bAttBone = !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->m_strAttBoneName.empty();
	}

	if ( bAttBone )
	{		
		if ( !m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece] )
			return;

		PDXATTBONE pDxAttBone = m_pSkinChar->m_pAttBoneArray[sStrikeEff.m_emPiece]->GetAttBone();
		if ( !pDxAttBone ) 
			return;

		STRIKE::CSELECTOR Selector ( vTARPOS, pDxAttBone->m_AttBoneArray );
		bOk = Selector.SELECT ( (EMPIECECHAR)ATTBONE_BODY, sStrike );
		if ( !bOk )	return;
	}
	else
	{
		STRIKE::CSELECTOR Selector ( vTARPAR, m_pSkinChar->m_PartArray );
		bOk = Selector.SELECT ( sStrikeEff.m_emPiece, sStrike );
		if ( !bOk )	return;
	}


	EMSLOT emSlot = PIECE_2_SLOT(sStrikeEff.m_emPiece);
	if( emSlot==SLOT_TSIZE )		return;

	//	오른손 무기만 적용되게 한다. 듀얼 CPS 때문이다. 
	//	LHAND 도구 설정했는데 무기는 오른손에만 있을경우 때문이다.
//	if( emSlot==SLOT_RHAND )		emSlot = GetCurRHand();
//	else if( emSlot==SLOT_LHAND )	emSlot = GetCurLHand();	
	emSlot = GetCurRHand();
    
	D3DXMATRIX matEffect;
	D3DXMatrixTranslation ( &matEffect, sStrike.vPos.x, sStrike.vPos.y, sStrike.vPos.z );

	STARGETID sTargetID = m_sActionID;
	sTargetID.vPos = vTARPOS;

	//	Note : 무기의 속성 살펴봄.
	//
	EMELEMENT emELEMENT = GET_ITEM_ELMT ();

	if ( VALID_SLOT_ITEM(emSlot) )
	{
		SITEM *pItem = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emSlot).GetNativeID());
		if ( pItem )
		{
			if ( !ISLONGRANGE_ARMS( pItem->sSuitOp.emAttack ) )
			{
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )	strEFFECT = GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT);

				DxEffGroupPlayer::GetInstance().NewEffGroup
				(
					strEFFECT.c_str(),
					matEffect,
					&sTargetID,
					TRUE,
					TRUE
				);

				DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
			}
			else
			{
				std::string strEFFECT = pItem->GetTargetEffect();
				if ( strEFFECT.empty() )
				{
					const GLITEM_ATT emItemAtt = CONVERT_ITEMATT( pItem->sSuitOp.emAttack );
					switch ( emItemAtt )
					{
					case ITEMATT_BOW:
						strEFFECT = GLCONST_CHAR::strSTRIKE_ARROW_EFFECT;
						break;

					case ITEMATT_PISTOL:
					case ITEMATT_RAILGUN:
					case ITEMATT_PORTALGUN:
						strEFFECT = GLCONST_CHAR::strSTRIKE_BULLET_EFFECT;
						break;
					}
				}

				if ( false == strEFFECT.empty() )
				{
					SPDXEFFSINGLEGROUP spEffSingleG = DxEffGroupPlayer::GetInstance().NewEffGroup(
						strEFFECT.c_str(),
						matEffect,
						&sTargetID,
						TRUE,
						TRUE );

					if ( spEffSingleG.get() )
					{
						spEffSingleG->AddEffAfter( EFFASINGLE, GLCONST_CHAR::GETSTRIKE_EFFECT( emELEMENT ) );
						spEffSingleG->AddEffAfter( EFFABODY, std::string( pItem->GetTargBodyEffect() ) );
						spEffSingleG->AddEffAfter( EFFABODY, GLCONST_CHAR::strAMBIENT_EFFECT );
					}
					else
					{
						DxEffSinglePropGMan::GetInstance().LoadEffectGProp(
							GLCONST_CHAR::GETSTRIKE_EFFECT( emELEMENT ),
							TRUE,
							TRUE );
					}
				}
			}
		}
	}
	else
	{
		//	보이지 않는 타갯일 경우 타격 이팩트는 생략됨.
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			GLCONST_CHAR::GETSTRIKE_EFFECT(emELEMENT),
			matEffect,
			&sTargetID,
			TRUE, 
			TRUE
		);

		DxEffGroupPlayer::GetInstance().NewEffBody ( GLCONST_CHAR::strAMBIENT_EFFECT.c_str(), &sTargetID, &vDir );
	}
}

BOOL GLCharacter::AttackProc ( float fElapsedTime )
{
	const float fAccumElapsed = (GLCHARLOGIC::GETATTVELO() + GLCHARLOGIC::GETATT_ITEM()) * fElapsedTime * UNITANIKEY_PERSEC;
	m_fattTIMER += fAccumElapsed;
	//GASSERT(m_pSkinChar->GETCURANIMNODE());

	//PANIMCONTNODE pAnicontNode = m_pSkinChar->GETCURANIMNODE();
	//GASSERT(pAnicontNode);
	//DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	const DWORD dwThisKey = static_cast<DWORD>(m_fattTIMER);
	const DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;

	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( m_pGaeaClient->IsVisibleCV(m_sActionID) )
			{
				//	Note : 공격시 이팩트 발휘..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}
			m_nattSTEP++;
		}
	}

	return TRUE;
}

BOOL GLCharacter::AttackVehicleProc ( float fElapsedTime )
{
	m_fattTIMER += fElapsedTime * GETATTVELO ();
	//GASSERT(m_sVehicle.m_pSkinChar->GETCURANIMNODE());

	//PANIMCONTNODE pAnicontNode = m_sVehicle.m_pSkinChar->GETCURANIMNODE();
	//if ( !pAnicontNode )
	//	return FALSE;

	//GASSERT(pAnicontNode);
	//DxSkinAnimation* pAnimCont = pAnicontNode->m_pDxSkinAni;

	const DxSkinAnimation* pAnimCont = m_sVehicle.m_pSkinChar->GETCURANIM();
	if ( !pAnimCont )
		return FALSE;

	if ( pAnimCont->m_wStrikeCount == 0 )	return FALSE;

	DWORD dwThisKey = DWORD(m_fattTIMER*UNITANIKEY_PERSEC);

	DWORD dwStrikeKey = pAnimCont->m_sStrikeEff[m_nattSTEP].m_dwFrame;
	if ( m_nattSTEP < pAnimCont->m_wStrikeCount )
	{
		m_sVehicle.m_pSkinChar->UpdateStrikeAttachBone( dwThisKey, m_nattSTEP );
		if ( dwThisKey>=dwStrikeKey )
		{
			if ( m_pGaeaClient->IsVisibleCV(m_sActionID) )
			{
				//	Note : 공격시 이팩트 발휘..
				//
				AttackEffect ( pAnimCont->m_sStrikeEff[m_nattSTEP] );
			}

			m_nattSTEP++;
		}
	}
	return TRUE;
}

BOOL GLCharacter::IsReActionableEnemyCrowPC( const ClientActor* pTARGET , const bool bEnableForcedPkMode /* = m_bForcedPKMode */ )
{
    std::tr1::shared_ptr<GLCharClient>	pCHAR = m_pGaeaClient->GetChar( pTARGET->GetGaeaID() );
    if( !pCHAR )
        return false;

    SDROP_CHAR&		sCHARDATA_TAR = pCHAR->GetCharData();
    if( IsSafeZone() || pTARGET->IsSafeZone() )
        return false;

    //	대련시 공격 가능.;
    if( ISCONFRONT_TAR( STARGETID(CROW_PC, pTARGET->GetGaeaID()) ) )
        return true;

    // 파티원 공격 불가;
    if ( m_pGaeaClient->GetMyPartyClient()->isMember(pTARGET->GetGaeaID()) )
        return false;

    PLANDMANCLIENT	pLand			= m_pGaeaClient->GetActiveMap();
    if( pLand ) 			//	맵관련 속성 체크;
    {
        // 선도전;
        if( pLand->m_bClubBattle )
        {
            return !m_pMyClub->IsAllianceGuild_CHIEF( pCHAR->GETCLUBID() );
        }
        else if( pLand->m_bClubDeathMatch && m_pGaeaClient->IsClubDeathMatch() )
        {
            return !(m_pMyClub->DbNum() == pCHAR->GETCLUBID());
        }
        else if( pLand->IsPKMap() ) 
        {
            return !(m_pMyClub->DbNum() != 0 && m_pMyClub->DbNum() == pCHAR->GETCLUBID() );
        }
        else if( pLand->IsClubBattleZone() )
        {
            if( m_pMyClub->IsBattle( pCHAR->GETCLUBID() ) ||
                m_pMyClub->IsBattleAlliance( pCHAR->GETALLIANCEID() ) ) 
            {
                return true;
            } //if
        }
        else if( m_pGaeaClient->IsCTFPlayer() )
        {
            return (m_wSchool != pCHAR->GetCharSchool());
        }
    } //if

    // 적대적일시 공격 가능;
    if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
        return true;

    // PK제한 레벨;
    if( sCHARDATA_TAR.m_Base.wLevel <= GLCONST_CHAR::nPK_LIMIT_LEVEL )
        return false;

    // PK서버인가?;
    if( m_pGaeaClient->IsPKServer() )
    {
        if( m_wSchool != sCHARDATA_TAR.m_Base.wSchool && !m_pMyClub->IsAllianceGuild_CHIEF(sCHARDATA_TAR.m_Base.dwGuild ) )
            return true;
    } //if

    // 성향 이벤트일경우.;
    if( m_pGaeaClient->IsBRIGHTEVENT() )
        return true;

    //	학교간 프리 PK.;
    if( !GLCONST_CHAR::bSCHOOL_FREE_PK_Z_FORCED ) 
    {
        bool bSCHOOL_FREEPK = m_pGaeaClient->IsSchoolFreePk();

        if( bSCHOOL_FREEPK && m_wSchool != sCHARDATA_TAR.m_Base.wSchool )
            return true;
    } //if

    // 강제 PK모드;
    if ( bEnableForcedPkMode )
    {
        return m_bForcedPKMode;
    }

    return false;
} //GLCharacter::IsReActionableEnemyCrowPC

BOOL GLCharacter::IsReActionableEnemy( const ClientActor* pTARGET )
{
	if( pTARGET->IsNonDamaged() )		// 공격 불가능일 경우
		return false;	

	switch( pTARGET->GetCrow() )
	{
	case CROW_PC:
		return IsReActionableEnemyCrowPC( pTARGET );

	case CROW_SUMMON:
	case CROW_MOB:
		return !( m_sCONFTING.IsCONFRONTING() || IsSafeZone() );
	}; //switch

	return FALSE;
} //GLCharacter::IsReActionableEnemy

BOOL GLCharacter::IsReActionableNoEnemyCrowPC( const ClientActor* pTARGET )
{
    //자신 자신인가?
    if ( m_dwGaeaID == pTARGET->GetGaeaID() )
        return TRUE;

    //상대방 클라이언트 케릭터
    std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar( pTARGET->GetGaeaID() );
    if( !pCLIENT )
        return FALSE;

	// 점령전 중일때 같은 학원에게만 가능
	if ( m_pGaeaClient->IsCTFPlayer() )
	{
        return (m_wSchool == pCLIENT->GetCharSchool());
	} //if

    //PVP 지역에서는 아군인지 검사
	PLANDMANCLIENT		pLand	= m_pGaeaClient->GetActiveMap();
	if( pLand && 
		( pLand->m_bClubBattle		|| 
		  pLand->m_bClubDeathMatch	|| 
		  pLand->IsFreePK() ) )
	{
        return m_pGaeaClient->GetMyClub()->IsAllianceGuild_CHIEF(pCLIENT->GETCLUBID());
	}  //if

	   //  대련중일시에는 상대방이 같은 대련에 속한 아군일 경우에게만 가능;
    if ( m_sCONFTING.IsCONFRONTING() )
    {
        return ISCONFRONT_ALLIANCE(pCLIENT->GetGaeaID());
    }

    //	적대적인 대상인지 검사
    if ( IS_PLAYHOSTILE ( pTARGET->GetCharID() ) )
        return FALSE;

    return TRUE;
} //GLCharacter::IsReActionableNoEnemyCrowPC

BOOL GLCharacter::IsReActionableNoEnemy( const ClientActor* pTARGET )
{
	switch ( pTARGET->GetCrow() )
	{
	case CROW_PC:
		return IsReActionableNoEnemyCrowPC( pTARGET );
	
	case CROW_NPC:
		return !m_sCONFTING.IsCONFRONTING();

	case CROW_MATERIAL:
		return TRUE;
	}; //switch

	return FALSE;
} //GLCharacter::IsReActionableNoEnemy

// *****************************************************
// Desc: 엑션을 취할 수 있는 대상인지 check
// *****************************************************
// EMIMPACT_SIDE_PARTY   : 아군에게 => 파티원에게;
// EMIMPACT_SIDE_ENEMY   : 적에게   => 적대적 대상에게;
// EMIMPACT_SIDE_ANYBODY : 모두에게 => 플레이어에게;
const BOOL GLCharacter::IsReActionable( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide /*= EMIMPACT_SIDE_ENEMY*/)
{
    return IsReActionable(sTargetID, emTargetSide, m_bForcedPKMode);
}

const BOOL GLCharacter::IsReActionable( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction)
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);
    GLLandManClient* const pLandMan = m_pGaeaClient->GetLandManClient();
    if ( pLandMan == NULL || pTARGET == NULL )
        return FALSE;

	if ( pTARGET->GetCrow() == CROW_SUMMON )			// 타겟이 소환수라면 대상 타겟검사를 소환수의 주인으로 바꾼다
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTargetID.GaeaId);
		if (!spSummon)
			return false;

		pTARGET = m_pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
		if (!pTARGET)
			return false;
	} //if

	if( pTARGET->GetCrow() == CROW_PC && pTARGET->IsNonTargetable() )
	{
		return false;
	}

    Faction::ManagerClient* const pFactionMan = pLandMan->GetFactionManager();
    if ( pFactionMan )
    {
        const Faction::EMRELATION_TYPE _relation = pFactionMan->getRelationClient(this, pTARGET);
        if ( _relation != Faction::EMRELATION_ERROR )
        {
            switch ( emTargetSide )
            {
            case EMIMPACT_SIDE_ENEMY:  // 적에게 : 적대적인 대상에게 상호작용일때 true;
                if (_relation == Faction::EMRELATION_ENEMY )
                    return TRUE;

                if ( ( _relation == Faction::EMRELATION_NEUTRAL_ENEMY ) && ( _bForceAction ) )  // 적대적인 중립관계의 대상에게는 강제공격으로 공격 가능;
                    return TRUE;

                //	대련자 공격 가능.;
                if( ISCONFRONT_TAR( sTargetID ) )
                    return true;

                // 적대행위자 공격 가능;
                if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
                    return true;

                return false;

            case EMIMPACT_SIDE_PARTY:  // 파티에게 : 파티원인 대상에게 상호작용일때 true;
			case EMIMPACT_SIDE_OUR:

				// 팩션으로 판별한다;
				if ( _relation == Faction::EMRELATION_ALLY )
					return TRUE;

				return FALSE;

				// 파티원으로 판별하지 않는다;
                /*if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
                    return TRUE;*/

            case EMIMPACT_SIDE_ANYBODY: // 모두에게 : 모든 대상에게 true;
                //코드가 자신을 속일지라도 헷갈리지 말자.. EMIMPACT_SIDE_ANYBODY는 모든대상이 아니다.. 플레이어와 NPC만이다....;
                if ( ( pTARGET->GetCrow() == CROW_PC ) || ( pTARGET->GetCrow() == CROW_NPC ) )
                    return TRUE;

                return false;

            default:
                break;
            }
        }
    }

    return IsReActionableNonFaction(sTargetID, emTargetSide, _bForceAction);
} 

const BOOL GLCharacter::IsReActionableNonFaction( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction )
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);

    if( !pTARGET )
        return false;

    if ( pTARGET->GetCrow() == CROW_SUMMON )			// 타겟이 소환수라면 대상 타겟검사를 소환수의 주인으로 바꾼다
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = m_pGaeaClient->GetSummon(sTargetID.GaeaId);
        if (!spSummon)
            return false;

        pTARGET = m_pGaeaClient->GetCopyActor(STARGETID(CROW_PC, spSummon->m_dwOwnerGaeaID));
        if (!pTARGET)
            return false;
    } //if

    switch ( emTargetSide )
    {
    case EMIMPACT_SIDE_PARTY:
        if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
            return TRUE;
        break;

    case EMIMPACT_SIDE_ENEMY :
        {
            if ( ( sTargetID.emCrow == CROW_PC) && ( _bForceAction == false ) )
                return IsReActionableEnemyCrowPC(pTARGET,false);

            return IsReActionableEnemy(pTARGET);
        }

    case EMIMPACT_SIDE_ANYBODY :
        {
            switch ( pTARGET->GetCrow() )
            {
            //PC의 경우에는 적대,비적대 상관없이 검사
            case CROW_PC:
                if( IsReActionableEnemyCrowPC(pTARGET) || IsReActionableNoEnemyCrowPC(pTARGET) )
                    return TRUE;

            //NPC의 경우에는 대련중이 아닐때만 가능
            case CROW_NPC:
                return !m_sCONFTING.IsCONFRONTING();
                //그 외 경우는 불가능
            default:
                break;
            }
        }
        break;

    case EMIMPACT_SIDE_OUR :
        return IsReActionableNoEnemy(pTARGET);

    default:
        return FALSE;
    }
    return FALSE;
}

void GLCharacter::ClearReservationAction(void)
{
	m_ReservationAction.Clear();
}

// bForced;
// 현재 러닝 스킬이 예약되어있는 상태에서 이동하라는 명령이 내려지면;
// 캔슬이되어야하는데 캔슬이 안됨;
void GLCharacter::ResetReservationAction(bool bForced)
{
	GLREACTION_ITER		iterReaction = m_ReservationAction.FrontIter();
	GLREACTION_ITER		iterEndReaction = m_ReservationAction.EndIter();

	bool bSkillCancle = false;
	while( iterReaction != iterEndReaction )
	{
		GLREACTION&			sReaction = (*iterReaction);
		if( sReaction.GETTYPE() == REACT_SKILL )
		{
			GLREACTION_SKILL&	sSkillReaction = (*iterReaction);
			PGLSKILL			pSkillData = GLSkillMan::GetInstance().GetData( sSkillReaction.GetSKILLID().wMainID, sSkillReaction.GetSKILLID().wSubID );
			GASSERT( pSkillData );

			if( bForced == false && (pSkillData->m_sBASIC.dwFlags & FLAG_SB_RUNNING_CAST) )
			{
				iterReaction++;
				continue;
			} //if
		} //if
		bSkillCancle = true;
		iterReaction = m_ReservationAction.EraseReaction( iterReaction );
	} 
	if ( bSkillCancle == true )
	{
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ReservationSkillCancel_All);
	}
	//while
} //GLCharacter::ResetReservationAction

bool GLCharacter::IsMoveAble()
{
	if( m_emVehicle == EMVEHICLE_PASSENGER )
		return false;

	return true;
}

BOOL GLCharacter::ISCONFRONT_TAR ( const STARGETID &sTargetID )
{
	if ( sTargetID.emCrow!=CROW_PC )	return FALSE;

	if ( m_sCONFTING.IsFIGHTING() )
	{
		switch ( m_sCONFTING.emTYPE )
		{
		case EMCONFT_ONE:
			return ( sTargetID.GaeaId==m_sCONFTING.dwTAR_ID );
			break;

		case EMCONFT_PARTY:
			{
				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(sTargetID.GaeaId);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;

		case EMCONFT_GUILD:
			{
				std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar ( sTargetID.GaeaId );
				if (!pCHAR)
                    return FALSE;

				SCONFTING_CLT::CONFT_MEM_ITER iter = m_sCONFTING.setConftMember.find(pCHAR->GetCharData().m_Base.dwCharID);
				return ( iter!= m_sCONFTING.setConftMember.end() );
			}
			break;
		};
	}

	return FALSE;
}

BOOL GLCharacter::ISCONFRONT_ALLIANCE( const DWORD dwGaeaID )
{
    std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar(dwGaeaID);
    if ( !pChar )
        return FALSE;

    if ( pChar->GetCrow() != CROW_PC )
        return FALSE;

    if ( m_sCONFTING.IsFIGHTING() )
    {
        switch ( m_sCONFTING.emTYPE )
        {
        case EMCONFT_ONE:
            //개인 대련에서 아군은 없음;
            return FALSE;
            break;

        case EMCONFT_PARTY:
            {
                //원정대일 경우는 고려하지 않음 (기획팀에서 절대 원정대 대련은 없다고 못박음);
                //같은 파티인가?;
                return m_pGaeaClient->GetMyPartyClient()->isMember(dwGaeaID);
            }
            break;

        case EMCONFT_GUILD:
            {
                //같은 길드인가?;
                return m_pGaeaClient->GetMyClub()->IsMember(dwGaeaID);
            }
            break;
        };
    }

    return FALSE;
}

void GLCharacter::DELCONFRONT_MEMBER ( DWORD dwID )
{
	switch ( m_sCONFTING.emTYPE )
	{
	case EMCONFT_ONE:
		break;

	case EMCONFT_PARTY:
		{
			//	Note : 자기 파티원일 경우.
			m_pGaeaClient->GetMyPartyClient()->ResetConfrontMember(dwID);

			//	Note : 상대방 파티원일 경우.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;

	case EMCONFT_GUILD:
		{
			//	Note : 자기 클럽원일 경우.
			m_pMyClub->DELCONFT_MEMBER ( dwID );

			//	Note : 상대방 클럽원일 경우.
			m_sCONFTING.DEL_CONFT_MEMBER ( dwID );
		}
		break;
	};

	return;
}

bool GLCharacter::IsSchoolConfting()
{
	if ( m_sCONFTING.IsFIGHTING() && m_sCONFTING.sOption.bSCHOOL )
	{
		return true;
	}

	return false;
}

UINT GLCharacter::GetConftMemberCount()
{
	return static_cast<UINT>(m_sCONFTING.setConftMember.size());
}

void GLCharacter::NewConftBoundEffect ( const EMCONFT_TYPE emCONFT )
{
	//	Note : 이팩트 설정.
	//
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );

	D3DXMATRIX matTrans, matRotY;
	D3DXVECTOR3 vDist;
	D3DXMatrixIdentity ( &matTrans );

	const float fPositionY = GetPosition().y;
	float vPos1 = fPositionY + 100.0f;
	float vPos2 = fPositionY - 100.0f;

	const D3DXVECTOR3 vConfrontPosition = D3DXVECTOR3(m_sCONFTING.vPosition.x, 0.0f, m_sCONFTING.vPosition.z);
	for ( float fRotY=0.0f; fRotY<(D3DX_PI*2.0f); fRotY += (D3DX_PI*2.0f/24.0f) )
	{
		vDist = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		D3DXMatrixRotationY(&matRotY,fRotY);
		D3DXVec3TransformCoord(&vDist,&vDist,&matRotY);

		float fCONFT_DIST(GLCONST_CHAR::fCONFRONT_ONE_DIST);
		switch ( emCONFT )
		{
		case EMCONFT_ONE:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_ONE_DIST;
			break;
		case EMCONFT_PARTY:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_PY_DIST;
			break;

		case EMCONFT_GUILD:
			fCONFT_DIST = GLCONST_CHAR::fCONFRONT_CLB_DIST;
			break;
		};

		vDist = vConfrontPosition + (vDist * fCONFT_DIST);		

		BOOL bCollision(FALSE);
		D3DXVECTOR3 vCollision;
		DWORD dwCollisionID;

		// 소수점 정밀도 관련 문제 확인 IsCollision
		boost::shared_ptr<NavigationMesh> spNaviMesh = m_pGaeaClient->GetActiveMap()->GetNaviMesh();
		if ( spNaviMesh )
		{
			spNaviMesh->IsCollision(vDist+D3DXVECTOR3(0,vPos1,0),vDist+D3DXVECTOR3(0,vPos2,0),vCollision,dwCollisionID,bCollision);
		}

		matTrans._41 = vDist.x;
		matTrans._42 = vCollision.y;		
		matTrans._43 = vDist.z;

		DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCONFRONT_BOUND.c_str(), matTrans, &TARGETID_NULL, TRUE, TRUE );
	}
}

void GLCharacter::DelConftBoundEffect ()
{
	DxEffGroupPlayer::GetInstance().DeleteAllEff ( GLCONST_CHAR::strCONFRONT_BOUND.c_str() );
}

BOOL GLCharacter::IsVaildTradeInvenSpace ()
{
	GLInventory &sTradeBoxTar = m_pGaeaClient->GetTradeClient()->GetTarTradeBox();

	GLInventory sInvenTemp;
	sInvenTemp.Assign ( m_cInventory );

	//	Note : 자신의 교환 목록에 올라간 아이템을 인밴에서 제외.
	//
	GLInventory &sTradeBoxMy = m_pGaeaClient->GetTradeClient()->GetMyTradeBox();
	const GLInventory::CELL_MAP* pItemListMy = sTradeBoxMy.GetItemList();

	GLInventory::CELL_MAP_CITER iter = pItemListMy->begin();
	GLInventory::CELL_MAP_CITER iter_end = pItemListMy->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		BOOL bOk = sInvenTemp.DeleteItem ( pTradeItem->wBackX, pTradeItem->wBackY );
		if ( !bOk )
			return FALSE;
	}

	//	Note : 상대방의 교환 물품이 들어 갈수 있는지 검사.
	//
	const GLInventory::CELL_MAP* pItemListTar = sTradeBoxTar.GetItemList();

	iter = pItemListTar->begin();
	iter_end = pItemListTar->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pTradeItem = (*iter).second;

		SITEM *pItem = GLogicData::GetInstance().GetItem ( pTradeItem->sItemCustom.GetNativeID() );
		if ( !pItem )
			return FALSE;

		WORD wPosX = 0, wPosY = 0;
		BOOL bOk = sInvenTemp.FindInsertable ( wPosX, wPosY );
		if ( !bOk )
			return FALSE;

		bOk = sInvenTemp.InsertItem ( pTradeItem->sItemCustom, wPosX, wPosY );
		if ( !bOk )
			return FALSE;
	}
	
	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------
//								H	a	i	r				S	t	y	l	e	 &&	 C	o	l	o	r
//----------------------------------------------------------------------------------------------------------------------------------
//void GLCharacter::HairStyleInitData()
//{
//	//m_bEnableHairSytle = TRUE;
//	
//	//m_wHairStyleTEMP = m_wHair;
//
//	//// 컬러 변화 중일 경우 건들지 않는다.
//	//if( !m_bEnableHairColor )
//	//{
//	//	m_wHairColorTEMP = m_wHairColor;
//	//}
//}
//
//void GLCharacter::HairColorInitData()
//{
//	//m_bEnableHairColor = TRUE;
//
//	//m_wHairColorTEMP = m_wHairColor;
//
//	//// 스타일 변화 중일 경우 건들지 않는다.
//	//if( !m_bEnableHairSytle )
//	//{
//	//	m_wHairStyleTEMP = m_wHair;
//	//}
//}

void GLCharacter::HairStyleChange( WORD wStyle )
{
	//m_wHairStyleTEMP = wStyle; 

	HairStyleUpdate( wStyle );
}

void GLCharacter::HairColorChange( WORD wColor )
{ 
	//m_wHairColorTEMP = wColor; 

	HairColorUpdate( wColor );
}

void GLCharacter::HairStyleUpdate( WORD wStyle )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// 헤어스타일.
	if ( sCONST.dwHAIRNUM > wStyle )
	{
		std::string strHAIR_CPS = sCONST.strHAIR_CPS[wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HAIR);		//	현재 장착 스킨.

		if( pCharPart && strcmp( strHAIR_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece( strHAIR_CPS.c_str(), m_pd3dDevice, NULL, NULL, EMSCD_ALL_THREAD );
		}
	}

	//// Note : 이것을 호출하면서.. 머리형이 원상태로 바뀌어 버린다.
	////		그래서 FALSE 호출로 머리모양을 안바뀌게 한다.
	//UpdateSuit( FALSE, TRUE );
}

void GLCharacter::HairColorUpdate( WORD wColor )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

    if ( m_pSkinChar )
	    m_pSkinChar->SetHairColor( wColor );
}

//void GLCharacter::FaceStyleInitData()
//{
//	//m_bEnableFaceStyle = TRUE;
//	
//	m_wFaceStyleTEMP = m_wFace;
//}

void GLCharacter::FaceStyleChange( WORD wStyle )
{
	//m_wFaceStyleTEMP = wStyle; 

	FaceStyleUpdate( wStyle );
}

void GLCharacter::FaceStyleUpdate( WORD wStyle )
{
	EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
	DxSkinCharData* pSkinChar = DxSkinCharDataContainer::GetInstance().FindData ( GLCONST_CHAR::szCharSkin[emIndex] );
	if ( !pSkinChar )	return;

	const GLCONST_CHARCLASS &sCONST = GLCONST_CHAR::cCONSTCLASS[emIndex];

	// 얼굴
	if ( sCONST.dwHEADNUM > wStyle )
	{
		std::string strHEAD_CPS = sCONST.strHEAD_CPS[wStyle];
	
		PDXCHARPART pCharPart = NULL;
		pCharPart = m_pSkinChar->GetPiece(PIECE_HEAD);		//	현재 장착 스킨.

		if ( pCharPart && strcmp( strHEAD_CPS.c_str(), pCharPart->GetFileName() ) )
		{
			m_pSkinChar->SetPiece ( strHEAD_CPS.c_str(), m_pd3dDevice, 0x0, 0, EMSCD_ALL_THREAD );
		}
	}

	//// Note : 이것을 호출하면서.. 머리형이 원상태로 바뀌어 버린다.
	////		그래서 FALSE 호출로 머리모양을 안바뀌게 한다.
	//UpdateSuit( TRUE, FALSE );
}

BOOL GLCharacter::IsMarketClick( DWORD dwGaeaID )
{
	MAPMARKETCLICK_ITER iter = m_mapMarketClick.find( dwGaeaID ) ;
	if( iter!=m_mapMarketClick.end() )
	{
		return TRUE;
	}
	return FALSE;
}

void GLCharacter::InsertMarketClick( DWORD dwGaeaID )
{
	MAPMARKETCLICK_ITER iter = m_mapMarketClick.find( dwGaeaID ) ;
	if( iter!=m_mapMarketClick.end() )
	{
		return;
	}
	m_mapMarketClick.insert( std::make_pair( dwGaeaID, dwGaeaID ) );
}

void GLCharacter::InitAllSkillFactByMoveMap ()
{
	// CP도 초기화
//	m_sCP.SetData( 0, MAX_CP );
	
	m_dwTransformSkill = SNATIVEID::ID_NULL;

	FactEffectProcessing( EMFACTEFFECTPROCESSING_MOVEMAP );
}

D3DXVECTOR3 GLCharacter::GetPosBodyHeight ()
{ 
	if( NULL == m_pSkinChar )
		return GetPosition();

	if ( m_emVehicle != EMVEHICLE_OFF && m_sVehicle.m_emTYPE == VEHICLE_TYPE_BOARD ) 
		return GetPosition() + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight() + 10.0f, 0.0f);
	else 
		return GetPosition() + D3DXVECTOR3(0.0f, m_pSkinChar->m_fHeight + m_sAirborne.getHeight(), 0.0f);
}

LONGLONG GLCharacter::GetCalcTaxiCharge( int nSelectMap, int nSelectStop )
{
	GLTaxiStation& sTaxiStation = GLTaxiStation::GetInstance();

	STAXI_MAP* pTaxiMap = sTaxiStation.GetTaxiMap( nSelectMap );
	if ( !pTaxiMap )
	{
		GASSERT ( 0 && "pTaxiMap를 찾을수 없습니다." );
		return 0;
	}

	STAXI_STATION* pSTATION = pTaxiMap->GetStation( nSelectStop );
	if ( !pSTATION )
	{
		GASSERT ( 0 && "STATION을 찾을 수 없습니다." );
		return 0;
	}

	DWORD dwCurMapID = m_pGaeaClient->GetActiveMapID().getGaeaMapID().Id();
	LONGLONG dwCharge = sTaxiStation.GetBasicCharge();

	if ( pSTATION->dwMAPID != dwCurMapID )
		dwCharge += pSTATION->dwMapCharge;

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( pLand )
	{
		volatile double fSHOP_RATE = GetBuyRate();

        dwCharge = dwCharge * static_cast<LONGLONG>(fSHOP_RATE) / 100;
	}

	return dwCharge;
}

bool GLCharacter::IsCoolTime(const SNATIVEID& sNativeID)
{
	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
	__time64_t tCoolTime = GetMaxCoolTime(sNativeID);
	if (tCurTime < tCoolTime)
        return true;
    else
	    return false;
}

bool GLCharacter::CheckCoolTime(const SNATIVEID& sNativeID)
{
	if (IsCoolTime(sNativeID))
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
		if (!pItem)
            return true;
		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(
                ID2GAMEINTEXT("ITEM_COOLTIME"),
                pItem->GetName()));
		return true;
	}
	return false;
}

__time64_t GLCharacter::GetMaxCoolTime(const SNATIVEID& sNativeID)
{
	//	아이템 정보 가져옴.
	SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
	if (!pItem)
        return 0;

	const ITEM_COOLTIME& pCoolTimeType = GetCoolTime( (DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE );
	const ITEM_COOLTIME& pCoolTimeID = GetCoolTime( pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID );		
	return (pCoolTimeType.tCoolTime > pCoolTimeID.tCoolTime) ? pCoolTimeType.tCoolTime : pCoolTimeID.tCoolTime;
}

const ITEM_COOLTIME& GLCharacter::GetCoolTime(const SNATIVEID& sNativeID) const
{
	//	아이템 정보 가져옴.
	const SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
	if (!pItem)
    {
        static ITEM_COOLTIME CoolTimeTemp;
        return CoolTimeTemp;
    }

	const ITEM_COOLTIME& pCoolTimeType = GetCoolTime((DWORD) pItem->sBasicOp.emItemType, EMCOOL_ITEMTYPE);
	const ITEM_COOLTIME& pCoolTimeID = GetCoolTime(pItem->sBasicOp.sNativeID.dwID, EMCOOL_ITEMID);
	return (pCoolTimeType.tCoolTime > pCoolTimeID.tCoolTime) ? pCoolTimeType : pCoolTimeID;
}

const ITEM_COOLTIME& GLCharacter::GetCoolTime(DWORD dwCoolID, EMCOOL_TYPE emCoolType) const
{
    static ITEM_COOLTIME CoolTimeTemp;
    
	if (emCoolType == EMCOOL_ITEMID)
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeID.find(dwCoolID);
		if (pos==m_mapCoolTimeID.end())
            return CoolTimeTemp;
        else		
		    return pos->second;
	}
	else if ( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_CITER pos = m_mapCoolTimeType.find(dwCoolID);
		if (pos == m_mapCoolTimeType.end())
            return CoolTimeTemp;
        else
		    return pos->second;
	}
	else
    {
		return CoolTimeTemp;
    }
}

bool GLCharacter::SetCoolTime ( ITEM_COOLTIME& sCoolTime, EMCOOL_TYPE emCoolType )
{
	if ( emCoolType == EMCOOL_ITEMID )
	{
		COOLTIME_MAP_ITER iter = m_mapCoolTimeID.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeID.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeID.insert(
                COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
	}
	else if( emCoolType == EMCOOL_ITEMTYPE )
	{
		COOLTIME_MAP_ITER iter = m_mapCoolTimeType.find(sCoolTime.dwCoolID);
        if (iter != m_mapCoolTimeType.end())
        {
            ITEM_COOLTIME& CoolTime = iter->second;
            CoolTime = sCoolTime;
        }
        else
        {
            m_mapCoolTimeType.insert(
                COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
	}

	GfxInterfaceBase::GetInstance()->SetItemSlotCoolDown( SNATIVEID( sCoolTime.dwID ) );

	return true;
}

const SITEMCUSTOM GLCharacter::GET_ITEM_MIX( int nIndex )
{
	SITEMCUSTOM sItemCustom( NATIVEID_NULL() );

	if( !m_sProductMaterialPos[nIndex].VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sProductMaterialPos[nIndex].wPosX, m_sProductMaterialPos[nIndex].wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

void GLCharacter::SetItemMixMoveItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	if( m_sPreInventoryItem.VALID() )
	{
		for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			if ( m_sProductMaterialPos[i].wPosX == m_sPreInventoryItem.wPosX &&
				m_sProductMaterialPos[i].wPosY == m_sPreInventoryItem.wPosY )
			{
				m_sPreInventoryItem.RESET();
				return;
			}

		}

		m_sProductMaterialPos[nIndex].SET( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
		
		m_sPreInventoryItem.RESET();
	}
	
	return;
}

void GLCharacter::ReSetItemMixItem( int nIndex )
{
	if ( nIndex >= ITEMMIX_ITEMNUM || m_sPreInventoryItem.VALID() )
	{
		m_sPreInventoryItem.RESET();
		return;
	}

	m_sProductMaterialPos[nIndex].RESET();
}

void GLCharacter::ResetItemMix()
{
	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		m_sProductMaterialPos[i].RESET();
	}
	m_sPreInventoryItem.RESET();
}

bool GLCharacter::ValidItemMixOpen()
{
	//!! SF_TODO
	/*if (m_pInterface->UiIsVisibleGroup(ITEM_MIX_INVEN_WINDOW) ||
        m_pInterface->UiIsVisibleGroup(ITEM_MIX_WINDOW))
    {
		return true;
    }
    else*/
    {
	    return false;
    }
}

void GLCharacter::CloseCostumeStat()
{
	m_bCostumeStats = false; 
	m_sPreInventoryItem.RESET();
}

bool GLCharacter::ValidCostumeStatOpen()
{
	//!! SF_TODO
	//if (m_pInterface->UiIsVisibleGroup(COSTUME_STATS_WINDOW) )
	{
		//return true;
	}
	//else
	{
		return false;
	}
}

bool GLCharacter::IsOpenNPCMarket ()
{
	return GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_NpcTalk_IsMarketTabOpen );
}

DWORD GLCharacter::GetItemMixMoney()
{
	SProductRecipe sProductRecipe;

	for( int i = 0; i < ITEMMIX_ITEMNUM; ++i ) 
	{
		sProductRecipe.sMeterialItem[i].sNID = GET_ITEM_MIX( i ).GetNativeID();

		if( sProductRecipe.sMeterialItem[i].sNID != NATIVEID_NULL() )
			sProductRecipe.sMeterialItem[i].nNum = (BYTE)GET_ITEM_MIX( i ).wTurnNum;
	}

	GLProductRecipeMan::GetInstance().SortMeterialItem( sProductRecipe );

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	const SProductRecipe* pProductRecipe;
	if (pGlobalStage->IsEmulator())
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeEmulator(sProductRecipe);
	else
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeClient(sProductRecipe);

	if (!pProductRecipe)
        return UINT_MAX;
    else
	    return pProductRecipe->dwPrice;
}

bool GLCharacter::ValidWindowOpen()
{
	if (ValidRebuildOpen() ||
        ValidGarbageOpen() ||
        ValidItemMixOpen() ||
		ValidCostumeStatOpen() ||
        IsOpenMiniGame() ||
        m_pGaeaClient->GetTradeClient()->Valid())
    {
        return true;
    }
    else
    {
	    return false;
    }
}

bool GLCharacter::GetLinkDataName( const SLINK_TYPE& sLinkType, DWORD dwData, CString& strLinkName, DWORD& dwColor )
{
	CString strTemp;

	switch ( sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
			case ITEM_LINK_PUTON:
			case ITEM_LINK_USER_STORAGE:
			case ITEM_LINK_CLUB_STORAGE:
			case ITEM_LINK_CROW_SALE:
			case ITEM_LINK_NORMAL:
				{
					// 아이템 정보 가져오기;
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = dwData;

					SINVENITEM* pInvenItem = m_cInventory.GetItem( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( NULL == pInvenItem )
						return false;

					const SITEMCUSTOM& sItemCustom = pInvenItem->CustomItem();

					SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
					if ( NULL == pItem )
						return false;
					
					// 링크 이름 세팅;
					strTemp.Format("%s%s%s", strLINK_LEFT_SIMBOL, sItemCustom.GETNAME().c_str(), strLINK_RIGHT_SIMBOL);					
					strLinkName = strTemp;
                    //아이템 링크별로 색상이 다르도록 수정 
					//dwColor = COMMENT::ITEMCOLOR[pItem->sBasicOp.emLevel];
                    dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];

					return true;
				}
				break;
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
				{
                    CString strLocation("");

                    // 지역 정보 표시
                    if ( m_pGaeaClient )
                    {
                        strLocation = m_pGaeaClient->GetMapName(dwData);
                    }

					strTemp.Format( "%s%s, %s%s", strLINK_LEFT_SIMBOL, strLocation, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 1 ), strLINK_RIGHT_SIMBOL );					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			case PARTY_LINK_ACCEPT:
				{
					strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 2 ), strLINK_RIGHT_SIMBOL );	
					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			case PARTY_LINK_REFUSE:
				{
					strTemp.Format( "%s%s%s", strLINK_LEFT_SIMBOL, ID2GAMEWORD( "CHAT_LINK_PARTY_RECRUIT", 3 ), strLINK_RIGHT_SIMBOL );			
					
					strLinkName = strTemp;
					dwColor = NS_UITEXTCOLOR::CHAT_LINK_PARTY_RECRUIT;

					return true;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;
}

bool GLCharacter::GetLinkData( const SLINK_DATA_BASIC& sLinkDataBasic, CString& strLinkName, DWORD& dwColor )
{
	CString strTemp;

	switch ( sLinkDataBasic.sLinkType.wMType ) 
	{
	case ITEM_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case ITEM_LINK_INVEN:
				{
					if ( false == GetLinkDataName( sLinkDataBasic.sLinkType, sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			case ITEM_LINK_PUTON:
				{
					EMSLOT emSlot = (EMSLOT)sLinkDataBasic.dwData1;
					if ( emSlot >= SLOT_TSIZE ) break;
					if ( !VALID_SLOT_ITEM( emSlot )) break;

					const SITEMCUSTOM &sItemCustom = m_PutOnItems[emSlot];					

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;

				}
				break;
			case ITEM_LINK_USER_STORAGE:
				{
					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;
					
					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pInvenItem )	break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, pInvenItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			case ITEM_LINK_CLUB_STORAGE:
				{
					if (!m_pMyClub->IsValid())
                        break;
					if (m_pMyClub->MasterCharDbNum() != m_CharDbNum)
                        break;

					DWORD dwChannel = sLinkDataBasic.dwData1;
					if ( dwChannel >= MAX_CLUBSTORAGE )
                        break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pInvenItem = m_pMyClub->FindPosItem(dwChannel, sInvenPos.wPosX, sInvenPos.wPosY);
					if (!pInvenItem)
                        break;

					if (!GetLinkDataName(sLinkDataBasic.sLinkType, pInvenItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor)) 
						break;

					return true;
				}
				break;
			case ITEM_LINK_CROW_SALE:
				{
					
					DWORD dwNpcGlobID = ( sLinkDataBasic.dwData1 & 0xFFFF0000 ) >> 16;
					DWORD dwChannel = ( sLinkDataBasic.dwData1 & 0x0000FFFF );

					PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
					if ( !pLand )
                        break;
					std::tr1::shared_ptr<GLCrowClient> spCrow = pLand->GetCrow(dwNpcGlobID);
					if (!spCrow)
                        break;

					if (spCrow->m_pCrowData->GetSaleNum() <= dwChannel)
                        break;

					const GLInventory *pInven = spCrow->m_pCrowData->GetSaleInven(dwChannel);
					if (!pInven)
                        break;

					SINVEN_POS sInvenPos;
					sInvenPos.dwPos = sLinkDataBasic.dwData2;
					
					SINVENITEM* pSaleItem = pInven->GetItem ( sInvenPos.wPosX, sInvenPos.wPosY );
					if ( !pSaleItem ) break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, pSaleItem->sItemCustom.GetBoxWrappedItemID().dwID, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;

			case ITEM_LINK_NORMAL :
				{
					if( false == GetLinkDataName( sLinkDataBasic.sLinkType,
						sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			}
		}
		break;
	case PARTY_LINK:
		{
			switch ( sLinkDataBasic.sLinkType.wSType ) 
			{
			case PARTY_LINK_RECRUIT:
			case PARTY_LINK_ACCEPT:
			case PARTY_LINK_REFUSE:
				{	
					GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
					if ( (pMyParty->isValid() == true) && (pMyParty->isMaster(m_dwGaeaID) == false) )
                        break;

					if ( !GetLinkDataName( sLinkDataBasic.sLinkType, sLinkDataBasic.dwData1, strLinkName, dwColor ) ) 
					{
						break;
					}

					return true;
				}
				break;
			}
		}
		break;
	case QUEST_LINK:
	case SKILL_LINK:
	case CROW_LINK:
	case MAP_LINK:
		{
		}
		break;
	}

	return false;
}

bool GLCharacter::IsPOSTBOXUSEABLE ( const EMPOSTBOXID_TYPE emIDType, const SNATIVEID& sPostBoxID )
{
	// Note : 우편함 사용 불가능이면
    if ( !GLUseFeatures::GetInstance().IsUsingPost() )
	{
		return false;
	}

	if ( !IsValidBody() )
	{
		return false;
	}

	if ( emIDType == EMPOSTBOXID_TYPE_NONE || sPostBoxID == NATIVEID_NULL() )
	{
		return false;
	}

	// Note : 우편함 아이디가 있다면
	switch ( emIDType )
	{
	default:
		return false;

	case EMPOSTBOXID_TYPE_NPC:
		{
			PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

			if ( !pLandMan )
			{
				return false;
			}

			// 우체통과의 거리 체크
			std::tr1::shared_ptr<GLCrowClient> spTarget =  pLandMan->GetCrow(sPostBoxID.dwID);
			if (!spTarget || !spTarget->m_pCrowData)
			{
				return false;
			}

			if (spTarget->m_pCrowData->m_emCrow != CROW_NPC || 
                spTarget->m_pCrowData->m_emNPCType != NPC_TYPE_POSTBOX )
			{
				return false;
			}

			const D3DXVECTOR3 &vTarPos = spTarget->GetPosition();

			float fMoveDist = FLT_MAX;
			float fDist = D3DXVec3Length ( &D3DXVECTOR3(GetPosition() - vTarPos) );
			if ( isAction(GLAT_MOVE) )
                fMoveDist = D3DXVec3Length ( &D3DXVECTOR3(m_sActionID.vPos-vTarPos) );

			// 우편 이용이 가능한 범위
			WORD wTakeRange   = spTarget->GetBodyRadius() + GETBODYRADIUS() + 30;
			WORD wTakeAbleDis = (WORD)(wTakeRange + GLCONST_POST::fPOSTBOX_DISTANCE);

			// 지금 위치가 대화가 불가능 하면
			if ( fDist>wTakeAbleDis )
			{
				return false;
			}
		}
		break;

	case EMPOSTBOXID_TYPE_ITEM_CARD:
		{
			SITEM *pITEM = GLogicData::GetInstance().GetItem ( sPostBoxID );
			if ( !pITEM )
			{
				return false;
			}

			// rm #1559 우편연결카드 수량 1개시 우편UI 뜨지 않는 문제..
			// 이미 차감후에 아래코드 검사하면 아무것도 없어서 무조건 false 임.
			// 이미 서버에서 정상 처리되어 오기때문에 여기서 이걸 할필요 없을것으로 보임.
			//SINVENITEM* pInvenItem = m_cInventory.FindItem( ITEM_POSTBOX_CARD );
			//if ( !pInvenItem )
			//{
			//	return false;
			//}
		}
		break;
	}

	return true;
}

//  대련 종료시 처리 작업
void GLCharacter::EndConfting()
{
    m_sCONFTING.RESET();
    //DisableDebuffSkillFact();
	DelConftBoundEffect();
}

void GLCharacter::ResetLimitTimeItemSort()
{
    m_fLimitTimeItemSort = fLIMIT_TIME_ITEM_SORT;
}

void GLCharacter::SetEnableSendItemSortMsg()
{
    m_bSendItemSortMsg = false;
    m_fItemSortMsgDelayTime = fMAX_ITEM_SORT_MSG_DELAY_TIME;
}

bool GLCharacter::IsEnalbeItemSort()
{
	if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_IsInvenItemSort ) )
	{
		return false;
	}

    if ( m_fLimitTimeItemSort > 0.0f ||
		 m_bSendItemSortMsg )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "ITEM_SORT_DELAY_TIME" ) );
        return false;
	}

    return true;
}

void GLCharacter::SetDisableSendItemSortMsg()
{
    m_bSendItemSortMsg = true;
    m_fItemSortMsgDelayTime = fMAX_ITEM_SORT_MSG_DELAY_TIME;
}

void GLCharacter::UpdateActivityItem()
{
    if ( !GLUseFeatures::GetInstance().IsUsingActivity() )
    {
        return;
    }

    MMACTIVITIES& mmActivities = ActivityBaseData::GetInstance()->GetData();

    ENUM_ACTIVITY_MID nMid = ACTIVITY_MID_TAKE; 

    bool m_bUpdateActivity = false;

    for ( ITER_MMACTIVITIES pos = mmActivities.lower_bound(nMid); pos != mmActivities.upper_bound(nMid); ++pos )
    {
        Activity* pActivity = &pos->second;

        if ( !pActivity->GetValidate() ) 
            continue;

        if ( pActivity->GetStatus() != ACTIVITY_STATUS_INPROGRESS ) 
            continue;

        DWORD dwCount(0);
        DWORD dwTargetID(0);
        for ( size_t i = 0; i < pActivity->m_vecTargetID.size(); i++ )
        {
            const DWORD _itemCount = GETHAVEITEM(pActivity->m_vecTargetID[i]);
            if ( dwCount < _itemCount )
            {
                dwCount = _itemCount;
                dwTargetID = pActivity->m_vecTargetID[i];
            }
        }
        DWORD dwPreValue  = pActivity->GetValue();
        DWORD dwBaseValue = pActivity->m_nValueBase;

        //  값이 변경되었다.
        if ( dwPreValue != dwCount ) 
        {
            pActivity->SetValue( dwCount );
            m_bUpdateActivity = true;

            // 진행 텍스트 출력
            if ( dwPreValue < dwCount && dwCount < dwBaseValue )
            {
				//!! SF_TODO
                /*if ( m_pInterface->IsActivityProgressDisplay() )
                {
                    m_pInterface->ProgressDisplayShow( UI::EMPROGRESSDISPLAY_TYPE_ACTIVITY_PROGRESS, 
                        sc::string::format( ID2GAMEINTEXT( "ACTIVITY_UPDATE_PROGRESS_PROGRESSDISPLAY" ), pActivity->GetTitle(), dwCount, dwBaseValue ) );
                }*/
				if ( strlen(pActivity->GetTitle().c_str()) != 0 )
				{
					GLWidgetScript::GetInstance().LuaCallFunc< void >(
						NSWIDGET_SCRIPT::g_strFunc_StudentRecord_ProgressMessege
						, "-s, -n, -n", pActivity->GetTitle().c_str(), dwCount, dwBaseValue);
				}
            }
        }

        if ( dwCount >= pActivity->m_nValueBase ) 
        {
            // 요청 처리
            GLMSG::SNET_ACTIVITY_DONE_ITEMTAKE NetMsg;
            NetMsg.dwActivityID = pActivity->m_id.dwID;
            NetMsg.dwItemID = dwTargetID;
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

            // 임시로 완료 처리
            pActivity->SetStatus( ACTIVITY_STATUS_ORDERING );  
        }

    }


    if ( m_bUpdateActivity ) 
    {
		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_StudentRecord_Update_Act );
    }
}

void GLCharacter::ResetRnAttendaceQuest()
{
	//! 퀘스트 포기.
	GLQUESTPROG* pQuestProg = NULL;

	GLQuestPlay::MAPQUEST_ITER iter = m_cQuestPlay.GetQuestProc().begin();
	GLQuestPlay::MAPQUEST_ITER iter_end = m_cQuestPlay.GetQuestProc().end();
	for ( ; iter!=iter_end;  )
	{
		pQuestProg = (*iter).second;

		GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pQuestProg->m_sNID.dwID );
		if ( !pQUEST )
		{
			++iter;
			continue;
		}

		CTime TToday = CTime::GetCurrentTime();

		if (pQUEST->m_sAttendanceOption.nType >= EMATTENDANCE_TYPE_CONNECTION &&
			pQUEST->m_sAttendanceOption.nType < EMATTENDANCE_TYPE_SIZE)
		{
			CTime CQuestStartTime(pQuestProg->m_tStartTime);

			if ( CQuestStartTime.GetDay() != TToday.GetDay() )
			{
				pQuestProg ->DoGIVEUP();

				//	완료된 리스트에 추가.
				m_cQuestPlay.InsertEnd(*pQuestProg, false);

				//	진행정보 삭제.
				//m_cQuestPlay.DeleteProc ( pQuestProg->m_sNID.dwID  );

				 m_cQuestPlay.DeleteMobProc( pQuestProg->m_sNID.dwID );
				 m_cQuestPlay.DeleteItemUseProc(pQuestProg->m_sNID.dwID);
				 SAFE_DELETE ( (*iter).second );
				 m_cQuestPlay.GetQuestProc().erase ( iter++ );
//				return true;
			}
			else
			{
				++iter;
			}
		}
		else
		{
			++iter;
		}
	}
}

BOOL GLCharacter::CHECK_ACTIVITY_POINT ( const SITEM& sItem )
{
    //  활동 요구 포인트 체크
    if ( sItem.sBasicOp.wReqActPointDW || sItem.sBasicOp.wReqActPointUP )
    {
        int emType = sItem.sBasicOp.emReqActivityType;
        if ( m_llActivityPoint[emType] < sItem.sBasicOp.wReqActPointDW ) return FALSE;
        if ( sItem.sBasicOp.wReqActPointUP && m_llActivityPoint[emType] >  sItem.sBasicOp.wReqActPointUP ) return FALSE;
    }

    return TRUE;
}

BOOL GLCharacter::CHECK_CONTRIBUTION_POINT( const SITEM& sItem )
{
	//  활동 기여도 체크
	if ( sItem.sBasicOp.dwReqContribution )
	{
		if ( (DWORD)m_nContributionPoint < sItem.sBasicOp.dwReqContribution )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void GLCharacter::MsgPremiumState(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PREMIUM_STATE *pNetMsg = (GLMSG::SNETPC_PREMIUM_STATE *) pMsg;

    if ( pNetMsg->bPREMIUM==false && m_bPREMIUM==true )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PREMIUMSERVICE_END") );
    }

    m_bPREMIUM = pNetMsg->bPREMIUM;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_UpdatePremium );
}

void GLCharacter::MsgGetCharedItemFromDbFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_GET_CHARGED_ITEM_FC* pNetMsg = (GLMSG::NET_GET_CHARGED_ITEM_FC*)pMsg;
    switch (pNetMsg->emFB)
    {
    case EMREQ_CHARGEDITEM_FROMDB_FB_END:	
		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Inventory_BankAllSlotUpdate );
        break;

    case EMREQ_CHARGEDITEM_FROMDB_FB_OK:
        ADDSHOPPURCHASE(pNetMsg->szPurKey, pNetMsg->nidITEM);
        break;

    case EMREQ_CHARGEDITEM_FROMDB_FB_END_FIRST_JOIN:
        break;
    }
}

void GLCharacter::MsgChargedItemGetFb(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CHARGED_ITEM_GET_FB *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_GET_FB *)pMsg;
    switch ( pNetNsg->emFB )
    {
    case EMCHARGED_ITEM_GET_FB_FAIL:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::PALEGREEN, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_FAIL") );
        break;
    case EMCHARGED_ITEM_GET_FB_OK:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_OK") );
        break;
    case EMCHARGED_ITEM_GET_FB_NOITEM:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOITEM") );
        break;
    case EMCHARGED_ITEM_GET_FB_NOINVEN:
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOINVEN") );
        break;
    };
}

void GLCharacter::MsgChargedItemDel(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_CHARGED_ITEM_DEL *pNetNsg = (GLMSG::SNET_CHARGED_ITEM_DEL *)pMsg;
    DELSHOPPURCHASE ( pNetNsg->dwID );

	SNATIVEID sPos( pNetNsg->dwID );
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_Inventory_BankSlotUpdate,
		"-n, -n", sPos.wMainID, sPos.wSubID );
}

void GLCharacter::MsgPlayerKillingAdd(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PLAYERKILLING_ADD *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_ADD *) pMsg;

    PLANDMANCLIENT	pLand			= m_pGaeaClient->GetActiveMap();	
    bool 			bBRIGHTEVENT    = m_pGaeaClient->IsBRIGHTEVENT();
    bool 			bSCHOOLFREEPK   = m_pGaeaClient->IsSchoolFreePk();
    bool 			bADD			= false;

    if( !( bBRIGHTEVENT || bSCHOOLFREEPK || pLand->m_bClubBattle || pLand->m_bClubDeathMatch ) )
        bADD = ADD_PLAYHOSTILE( pNetMsg->dwCharID, pNetMsg->bBAD, GLCONST_CHAR::fPK_JUSTNESS_TIME );

    // 학원간 자유피케 진행중이며 상대가 같은 학교일경우 적대행위 대상자로 구분하여 관리함.
    if( bSCHOOLFREEPK && m_wSchool == pNetMsg->wSchoolID )
        bADD = ADD_PLAYHOSTILE( pNetMsg->dwCharID, pNetMsg->bBAD, pNetMsg->fTime );

    // 리소스가 만들어 질때까지 주석 처리 2012.4.13
	//InsertAdditionalIconTimer( SKILL_ADDITIONAL_BATTLE, pNetMsg->fTime );

    if( bADD )
    {
		if( !pNetMsg->bBAD )
		{
			if( pNetMsg->bClubBattle )
			{
				GLCharClient* pCLIENT = (GLCharClient*) m_pGaeaClient->GetCopyActor( pNetMsg->szName );
				if( pCLIENT ) 
				{
					std::string strText;
					if ( m_pMyClub->IsBattle ( pCLIENT->GETCLUBID() ) )
					{
						strText = sc::string::format( ID2GAMEINTEXT("CLUB_PLAYHOSTILE_ADD"), pCLIENT->GetClubName(), pNetMsg->szName );
						m_pGaeaClient->PrintConsoleTextDlg ( strText ) ;
					}
					else if ( m_pMyClub->IsBattleAlliance ( pCLIENT->GETALLIANCEID() ) )
					{
						strText = sc::string::format( ID2GAMEINTEXT("ALLIANCE_PLAYHOSTILE_ADD"), pCLIENT->GetClubName(), pNetMsg->szName );
						m_pGaeaClient->PrintConsoleTextDlg ( strText ) ;
					} //if..else if
				} //if
			}
			else
			{
				std::string strText = sc::string::format( ID2GAMEINTEXT("PK_PLAYHOSTILE_ADD"), pNetMsg->szName );
				m_pGaeaClient->PrintConsoleTextDlg ( strText );
			} //if..else
		} //if
    } //if
}

void GLCharacter::MsgPlayerKillingDel(NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNETPC_PLAYERKILLING_DEL *pNetMsg = (GLMSG::SNETPC_PLAYERKILLING_DEL *) pMsg;
    DEL_PLAYHOSTILE ( pNetMsg->dwCharID );
}


HRESULT GLCharacter::UpdateClientState ( GLMSG::SNETPC_UPDATE_STATE *pNetMsg )
{
    //pNetMsg->DECODE ( m_dwGaeaID );
    m_sHP = pNetMsg->sHP;
    m_sMP = pNetMsg->sMP;
    m_sSP = pNetMsg->sSP;

    m_fTempVarHP = 0.0f;
    m_fTempVarMP = 0.0f;
    m_fTempVarSP = 0.0f;

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

    return S_OK;
}

// 서버 객체와 동일해야한다;
void GLCharacter::UpdateActState(void)
{
	if ( IsActState( EM_ACT_EMERGENCY ) )
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()));
		if ( GLCONST_CHAR::fEmergency_CharHPR < fHPRate )
		{
			ReSetActState( EM_ACT_EMERGENCY );
		}
	}
	else
	{
		const float fHPRate(static_cast<float>(GETHP())/static_cast<float>(GETMAXHP()));
		if ( fHPRate <= GLCONST_CHAR::fEmergency_CharHPR )
		{
			SetActState( EM_ACT_EMERGENCY );
		}
	}
}

/*void GLCharacter::DisableDebuffSkillFact()
{
    for ( int i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i )
    {
        if (i >= SKILLFACT_SIZE || m_sSKILLFACT[i].sNATIVEID == NATIVEID_NULL())
            continue;

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[i].sNATIVEID );
        if ( !pSkill )
            continue;

        // 적이 건 스킬 효과만 제거
        if ( pSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY  )
        {
            FACTEFF::DeleteSkillFactEffect ( STARGETID(CROW_PC, m_dwGaeaID, GetPosition()), m_pSkinChar, m_sSKILLFACT[i].sNATIVEID );

			m_pGaeaClient->PartyMemberRemoveBuff( m_dwGaeaID, m_sSKILLFACT[ i ].sNATIVEID.dwID, i );
            RESETSKEFF( i );
        }
    }
}*/

void GLCharacter::ReceiveDamage(const int nDamage, DWORD dwDamageFlag, WORD wSchool, const STARGETID& sACTOR)
{
    if ( dwDamageFlag & DAMAGE_TYPE_SHOCK )	ReceiveShock();

    //	Note : 화면에 타격 값 출력.
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nDamage, dwDamageFlag, 
		UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );

    if ( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW )
    {
        // 강한타격 이펙트
		const D3DXVECTOR3& vPosition = GetPosition();
        D3DXVECTOR3 vDIR = sACTOR.vPos - vPosition;

        D3DXVECTOR3 vDIR_ORG(1,0,0);
        float fdir_y = DXGetThetaYFromDirection ( vDIR, vDIR_ORG );

        D3DXMATRIX matTrans;
        D3DXMatrixRotationY ( &matTrans, fdir_y );
        matTrans._41 = vPosition.x;
        matTrans._42 = vPosition.y + 10.0f;
        matTrans._43 = vPosition.z;

        //	Note : 자기 위치 이펙트 발생시킴.
        DxEffGroupPlayer::GetInstance().NewEffGroup ( GLCONST_CHAR::strCRUSHING_BLOW_EFFECT.c_str(), matTrans, &sACTOR, TRUE, TRUE );
    }

    //	Note : 타격값 반영.
    GLCHARLOGIC::RECEIVE_DAMAGE ( nDamage );

	m_pGaeaClient->logDamageRecord(sACTOR, nDamage);

    if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
    {
		//!! SF_TODO
        //if ( !m_pInterface->UiIsVisibleGroup( TUTORIAL_DAMAGE_ALARM ) )
        //    m_pInterface->UiShowGroupTop( TUTORIAL_DAMAGE_ALARM );
    }

    //	Note : 방어스킬의 이펙트가 있을때 발동시킴.
    SKT_EFF_HOLDOUT ( sACTOR, dwDamageFlag );

    // 피격시 내구도 감소
    // 공격 상대가 사람인지
    // 일반공격시 클라이언트에게 전달되는 메시지
    if( sACTOR.emCrow == CROW_PC )
        DurabilityDropHit( abs( nDamage ), TRUE );
    else
        DurabilityDropHit( abs( nDamage ), FALSE );

	// UI 에 캐릭터가 피해를 받았음을 알린다;
	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ReceiveDamage_Character );
}

void GLCharacter::ReceiveHeal(const int nHeal, DWORD dwHealFlag, WORD wSchool, const STARGETID& sACTOR)
{
	//	Note : 화면에 타격 값 출력.
	D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, nHeal, dwHealFlag, 
		UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );

	//	Note : 타격값 반영.
	GLCHARLOGIC::RECEIVE_HEAL(nHeal);
}

void GLCharacter::ReceiveBlocking(const STARGETID& sACTOR)
{
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, 
		UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
}

void GLCharacter::ReceiveAVoid( const STARGETID& sACTOR )
{
    D3DXVECTOR3 vPos = GetPosBodyHeight();
	GfxInterfaceBase::GetInstance()->AddDamageDisplay( vPos, 0, DAMAGE_TYPE_NONE, 
		UI_UNDERATTACK, sACTOR, STARGETID( CROW_PC, GetGaeaID() ) );
}

void GLCharacter::ReceiveSwing()
{
    if (IsValidBody() && !IsCtrlBlockBody())
        m_pSkinChar->DOSHOCKMIX();
}

void GLCharacter::ReceiveShock()
{
    if (IsValidBody() && !IsCtrlBlockBody())
        TurnAction(GLAT_SHOCK);
}

// Note : bjju. 상황에 따라 Effect를 처리하는 함수
void GLCharacter::FactEffectProcessing ( EMFACTEFFPROCESSING emFactEffCase )
{
    switch ( emFactEffCase )
    {
    case EMFACTEFFECTPROCESSING_DESTRUCTOR:
    case EMFACTEFFECTPROCESSING_MOVEMAP:
        {
            FACTEFF::DeleteAllEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS );

            for (size_t i=0; i<EMBLOW_MULTI; ++i)
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

            for (size_t i=0; i<SKILLFACT_SIZE; ++i)
                m_sSKILLFACT[i].RESET();

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );

			NSEffectPlay::DeleteALL();
        }
        break;

    case EMFACTEFFECTPROCESSING_DEAD:
    case EMFACTEFFECTPROCESSING_REBIRTH:
        {
			RESET_SKILL_DATA();

            FACTEFF::DeleteEffect ( STARGETID(GetCrow(), GetGaeaID(), GetPosition()), GetSkinChar(), m_sSKILLFACT, SKILLFACT_SIZE, m_sSTATEBLOWS );

            for (size_t i=0; i<EMBLOW_MULTI; ++i)
                m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;

			RemoveSkillFactofNormalAll();
            //for (size_t i=SKILLFACT_INDEX_NORMAL_START; i<SKILLFACT_INDEX_NORMAL_END; ++i)
            //    RESETSKEFF(i);

            for (size_t i=SKILLFACT_INDEX_LUNCHBOX_START; i<SKILLFACT_INDEX_EXP_LUNCHBOX_END; ++i)
                SetSkillFact(i, FALSE);

            for (size_t i=SKILLFACT_INDEX_SYSTEM_START; i<SKILLFACT_INDEX_SYSTEM_END; ++i)
                SetSkillFact(i, FALSE);

			m_pGaeaClient->PartyMemberRemoveAllBuff( m_dwGaeaID );
        }
        break;
    };
}

const WORD GLCharacter::FindQuickSkillSlot( const SKILLID& sSkillID )
{
	//! 배운 스킬이 아닐 경우 취소됨.
	if ( !ISLEARNED_SKILL(sSkillID.sSKILLID) )
		return USHRT_MAX;

	WORD wTab = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_CurrentTab );
	WORD wSlotCount = GLWidgetScript::GetInstance().LuaCallFunc< WORD >( NSWIDGET_SCRIPT::g_strFunc_SQS_TabSlotCount );

	UINT nBeginIndex = wTab * wSlotCount;
	UINT nIndex		 = nBeginIndex;

	for( UINT i=0; i<EMSKILLQUICK_SIZE; ++i, ++nIndex )
	{
		if ( EMSKILLQUICK_SIZE <= nIndex )
		{
			nIndex %= EMSKILLQUICK_SIZE;
		}
		
		if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
		{
			if ( sSkillID.sSKILLID == m_sChangeQUICK[nIndex] )
			{
				return static_cast<WORD>(nIndex);
			}
		}
		else
		{
			if ( sSkillID.sSKILLID == m_sSKILLQUICK[nIndex] )
			{
				return static_cast<WORD>(nIndex);
			}
		}
	}

	return USHRT_MAX;
}

void GLCharacter::ResetSelectID()
{
    if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) || IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
    {
        return;
    }
	m_setTAR_SELECTED.clear();
	m_sSelectID.RESET();
}

void GLCharacter::SetSelectID( const STARGETID& sTarget, const bool bResetSelected )
{
    STARGETID sTargetID = sTarget;
    if ( IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
    {
        sTargetID = m_sTaunt.sTauntTargetID;
    }
	
	if ( bResetSelected )
	{
		m_setTAR_SELECTED.clear();
	}

	TARIDRC_SET_ITER finditer = m_setTAR_SELECTED.find( sTargetID );

	if ( finditer != m_setTAR_SELECTED.end() )
	{
		STARGETIDREFCOUNTER& sTargetID = (*finditer);
		++sTargetID.nRefCount;
	}
	else
	{
		m_setTAR_SELECTED.insert( sTargetID );
	}

	m_sSelectID = sTargetID;

	//! 셀렉트 타겟 리스트를 리셋하는 갯수 (최대 관리갯수)
	{
		UINT nNumManaged = GLInputManager::GetInstance().GetNumManagedSelectTarget();
		
		if ( nNumManaged != 0 && nNumManaged <= static_cast<UINT>(m_setTAR_SELECTED.size()) )
		{
			m_setTAR_SELECTED.clear();
		}	
	}
}

STARGETID* GLCharacter::FindForcedTarget( VEC_SK_TAR* pDetectMap )
{
	for( unsigned int i = 0; i < pDetectMap->size(); i++ )
	{
		const STARGETID&	targetid = (*pDetectMap)[i];
		if( targetid.emCrow == CROW_PC && targetid.GaeaId != m_dwGaeaID )
		{
			GLCharClient*	pTarget = (GLCharClient*)m_pGaeaClient->GetCopyActor( targetid );
			if ( !pTarget )
				continue;

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
				return &(*pDetectMap)[i];

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ))
				return &(*pDetectMap)[i];
		} //if
	} //for

	return NULL;
} //GLCharacter::FindForcedTarget

const STARGETID GLCharacter::GetSelectForcedTargetAround( const UINT nDegree, const float fRange )
{
	VEC_SK_TAR*	pDetectMap = m_pGaeaClient->DetectCrowAll( 0xFFFFFFFF, GetPosition(), fRange );
	if ( !pDetectMap )
		return TARGETID_NULL;

	for( unsigned int i = 0; i < pDetectMap->size(); i++ )
	{
		const STARGETID&	targetid = (*pDetectMap)[i];
		if( targetid.emCrow == CROW_PC && targetid.GaeaId != m_dwGaeaID )
		{
			GLCharClient*	pTarget = (GLCharClient*)m_pGaeaClient->GetCopyActor( targetid );
			if ( !pTarget )
				continue;

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_TAUNT ) )
				return (*pDetectMap)[i];

			if( pTarget->IsCheckedSkillFlagSpecial( EMSPECA_PROVOKE ) )
				return (*pDetectMap)[i];
		} //if
	} //for

	return TARGETID_NULL;
} //GLCharacter::GetSelectForcedTargetAround

BOOL GLCharacter::SelectForcedTarget()
{
	const STARGETID		selectId = GetSelectForcedTargetAround();
	if( selectId.ISNULL() )
		return FALSE;

	m_sSelectID = selectId;

	return TRUE;
} //GLCharacter::SelectForcedTarget

const STARGETID GLCharacter::GetSelectTargetAround( const EMFIND_TAR		emFindTar, 
												    const D3DXVECTOR3&		vPosition, 
												    const float				fRange, 
												    const UINT				nDegree )
{
	
	float fApplyAngle  = D3DXToRadian(static_cast<float>(nDegree)) * 0.5f;

	DWORD emCrow = NULL;

	bool bANY(false), bENEMY(false), bOUR(false), bDIE(false);

	switch ( emFindTar )
	{
	default:
	case ENFIND_TAR_NULL:
		emCrow = ( CROW_EX_PC | CROW_EX_MOB | CROW_EX_SUMMON );
		bENEMY = true;
		bANY   = true;
		bOUR   = true;
		break;
	case EMFIND_TAR_ENEMY:
		emCrow = ( CROW_EX_PC | CROW_EX_MOB | CROW_EX_SUMMON );
		bENEMY = true;
		break;
	case EMFIND_TAR_ANY:
		emCrow = ( CROW_EX_PC );
		bANY = true;
		break;
	case EMFIND_TAR_OUR:
		emCrow = ( CROW_EX_PC );
		bOUR = true;
		break;
	case EMFIND_TAR_ANY_DIE:
		emCrow = ( CROW_EX_PC );
		bANY = true;
		bDIE = true;
		break;
	case EMFIND_TAR_OUR_DIE:
		emCrow = ( CROW_EX_PC );
		bOUR = true;
		bDIE = true;
		break;
	};

	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	VEC_SK_TAR *pDetectMap = m_pGaeaClient->DetectCrowAll ( emCrow, vPosition, fRange );
	if ( !pDetectMap )
		return TARGETID_NULL;

	//! 몹타입,거리 순서대로 뽑아낸다
	std::sort ( pDetectMap->begin(), pDetectMap->end(), std_afunc::CCompareSelectTargetDist(m_setTAR_SELECTED, vPosition) );

	VEC_SK_ITER iter		= pDetectMap->begin();
	VEC_SK_ITER iterEnd  = pDetectMap->end();

	for ( ; iter!=iterEnd; ++iter )
	{
		STARGETID& sTarget = (*iter);
		if ( sTarget.ISNULL() )
			continue;

		////! 선택된 타겟 리스트를 검사한다.
		//TARID_SET_ITER finditer = m_setTAR_SELECTED.find(sTarget);

		//if ( finditer != m_setTAR_SELECTED.end() )
		//{
		//	continue;
		//}

		//! 사정거리에 들지 못하면 제외
		if ( nDegree < 360 )
		{
			D3DXVECTOR3 vTarDir = sTarget.vPos - vPosition;
			float fDir = DXGetThetaYFromDirection ( vTarDir, GetDirect() );

			if ( fApplyAngle < fabs(fDir) )
				continue;
		}

		ClientActor* pTarget = m_pGaeaClient->GetCopyActor( sTarget );
		if ( !pTarget )
			continue;

		//! 카메라에 보이는지
		if ( !COLLISION::IsCollisionVolume ( CV, 
			D3DXVECTOR3(pTarget->GetPosition() + pTarget->GetMaxOrg()), 
			D3DXVECTOR3(pTarget->GetPosition() + pTarget->GetMinOrg()) ) )
		{
			continue;
		}

		//! 죽은 타겟을 체크할지
		if ( bDIE != (pTarget->IsValidBody() ? false : true) )
			continue;

		//! 플레이어인지 체크
		if ( (bANY || bOUR) && (sTarget.emCrow == CROW_PC) )
			continue;

		//! 파티인지 체크
		if ( bOUR && (m_pGaeaClient->GetMyPartyClient()->isMember(sTarget.GaeaId) == true) )
			continue;

        if ( bANY && !IsReActionable(sTarget, EMIMPACT_SIDE_ANYBODY) )
            continue;

        if ( bENEMY && !IsReActionable(sTarget, EMIMPACT_SIDE_ENEMY) )
            continue;
        
        if ( bOUR && !IsReActionable(sTarget, EMIMPACT_SIDE_OUR) )
            continue;

		return sTarget;
	}

	return TARGETID_NULL;
}

//! 캐릭터 주변의 타겟을 가져온다
// 각도는 현재 화면이 바라보는 방향을 기준으로 검사한다
const STARGETID GLCharacter::GetSelectTargetBodyAround( const EMFIND_TAR emFindTar, const float fRange, const UINT nDegree )
{
	return GetSelectTargetAround( emFindTar, GetPosition(), fRange, nDegree );
}

//! 마우스 주변의 타겟을 가져온다
const STARGETID	GLCharacter::GetSelectTargetMouseAround( const EMFIND_TAR emFindTar, const float fRange )
{
	D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPort::GetInstance().GetFromPt();
	if ( !DxViewPort::GetInstance().GetMouseTargetPosWnd(vTargetPt) )
		return TARGETID_NULL;
	
	//! 커서에 올려진 캐릭터가 있다면 그 타겟을 기준으로 검사
	STARGETID& sCursorID = GetCursorID();
	if ( !sCursorID.ISNULL() )
		return GetSelectTargetAround( emFindTar, sCursorID.vPos, fRange, 360 );

	//! 캐릭터가 서있는 평면을 기준으로 픽킹 위치를 가져온다
	D3DXVECTOR3 vOrgPos = GetPosition();
	D3DXVECTOR3 vPos	= vOrgPos;

	D3DXPLANE plCollision;
	D3DXVECTOR3 vCollision;
	D3DXPlaneFromPointNormal( &plCollision, &vOrgPos, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	if ( D3DXPlaneIntersectLine(&vCollision, &plCollision, &vFromPt, &vTargetPt) )
		return GetSelectTargetAround( emFindTar, vCollision, fRange, 360 );
	
	return TARGETID_NULL;
}

void GLCharacter::OffLine()
{
	m_pGaeaClient->GetTexasHoldemManager()->Offline();
}

void GLCharacter::GetItemCoolDownTime( const SNATIVEID& sID, float& fCoolDownTime, float& fCoolDownRatio )
{
	if( false == IsCoolTime( sID ) )
		return;

	const ITEM_COOLTIME sItemCoolTime = GetCoolTime( sID );
	if( true == sItemCoolTime.IsZeroTime() )
		return;

	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
	__time64_t tUseTime = sItemCoolTime.tUseTime;
	__time64_t tCoolTime = sItemCoolTime.tCoolTime;

	float fCoolTime = static_cast< float >( tCoolTime - tUseTime );

	fCoolDownTime = fCoolTime - (tCurTime - tUseTime);
	if( fCoolTime == 0.0f )
		fCoolTime = 1.0f;

	fCoolDownRatio = 1.0f - (fCoolDownTime / fCoolTime);
}

const bool GLCharacter::IsExistReservationAction(EMREACTION emFind)
{
	return m_ReservationAction.IsExistReaction(emFind);
}

const BOOL GLCharacter::IsNonTargetable( ) const
{
	if( m_sHALLUCINATE.bNonTargetable || RF_FEIGN_DEATH(m_EffSkillVarSet).IsSelectDisable() )
		return TRUE;

	return FALSE;
}

void GLCharacter::SummonStateVisibleUpdate(void)
{
	bool bHire(m_sBasicSummonID != SNATIVEID(false));
	// UI on/off;
	GLWidgetScript::GetInstance().LuaCallFunc<void>( 
		NSWIDGET_SCRIPT::g_strFunc_SetVisibleSummonState
		, "-b, -b", m_sSummonable.bSummonable, bHire );
}

LuaTable GLCharacter::GetSummonHPInfo(void)
{
	LuaTable tbHPInfo( GLWidgetScript::GetInstance().GetLuaState() );

	// 저장된 고용 소환수 정보;
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sBasicSummonID );
	// 실제 소환된 소환수 정보;
	std::tr1::shared_ptr< GLSummonClient > spSummon =
		m_pGaeaClient->GetSummon(m_sSummonable.hireInfo.dwGaeaID);
	if ( spSummon )
	{
		tbHPInfo.set( 1, spSummon->GETHP() );
		tbHPInfo.set( 2, spSummon->GETMAXHP() );
	}
	return tbHPInfo;
}

const char* GLCharacter::GetSummonName(void)
{
	// 저장된 고용 소환수 정보;
	PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( m_sBasicSummonID );
	if ( pCrowData )
		return pCrowData->GetName();

	return NULL;
}

void GLCharacter::UpdateSkillEff( float fTime, float fElapsedTime )
{
	/// 변장 ///////////////////////////////////////////////////
	BOOL bOn   = RF_DISGUISE( m_EffSkillVarSet ).IsOn();
	UINT Index = RF_DISGUISE( m_EffSkillVarSet ).GetIndex();

	SITEMCUSTOM sLHandItem = m_PutOnItems[ SLOT_LHAND ];
	SITEM* pItem = GLItemMan::GetInstance().GetItem( sLHandItem.GetRealNativeID().dwID );
	if ( pItem )
	{
		// Table ID 가 같을 경우에만 Index를 변경한다;
		// 데이터를 늘리지 않기 위하여 Pet 정보를 이용한다;
		if ( RF_DISGUISE( m_EffSkillVarSet ).GetTableID() == pItem->sPet.sPetID.Mid() )
			Index = pItem->sPet.sPetID.Sid();
	}

	if( m_pTransformSet->AutoResetDisting( Index, bOn ) ) {
		Stop_Disguise();
	}
	else {
		Play_Disguise( Index, bOn );
	}
	/// 스크린 효과 ///////////////////////////////////////////
	for( int i = 0; i < GLCONST_CHAR::SCREEN_EFFECT_NUM; i++ )
	{
		if( m_SkillSTrow.AutoReset(i, RF_SCREEN_THROW( m_EffSkillVarSet ).IsOn(i)) ) {
			Stop_ScreenThrow(i);
		}
		else {			
			Play_ScreenThrow( i, RF_SCREEN_THROW( m_EffSkillVarSet ).GetTargetID( i ) );
		}
	}
	/// 카메라 치기 ////////////////////////////////////////////
	WORD wEffNum = RF_CAMERA_HIT( m_EffSkillVarSet ).GetEffNum();

	if( m_SkillCHit.AutoReset( wEffNum, RF_CAMERA_HIT( m_EffSkillVarSet ).IsOn()) ) {
		Stop_CameraHit( wEffNum );
	}
	else {
		Play_CameraHit( wEffNum );
	}
	/// 도발 ////////////////////////////////////////////
	if( RF_PROVOKE( m_EffSkillVarSet ).IsOn() ) {
		const STARGETID& sID = RF_PROVOKE( m_EffSkillVarSet ).GetHostID();

		if( !sID.ISNULL() && sID != m_sSelectID ) SetSelectID( sID );

		UpdateSkillRandomBuild( fTime, fElapsedTime );
	}
}

BOOL GLCharacter::IsSelectTargetLock()
{
	if( IsCheckedSkillFlagSpecial( EMSPECA_SCREEN_THROW ) ) return TRUE;

	return FALSE;
}

const BOOL GLCharacter::IsReActionableOver( const STARGETID &sTargetID, const EMIMPACT_SIDE emTargetSide, const bool _bForceAction)
{
    ClientActor *pTARGET = m_pGaeaClient->GetCopyActor(sTargetID);
    GLLandManClient* const pLandMan = m_pGaeaClient->GetLandManClient();
    if ( pLandMan == NULL || pTARGET == NULL )
        return FALSE;

    Faction::ManagerClient* const pFactionMan = pLandMan->GetFactionManager();
    if ( pFactionMan )
    {
        const Faction::EMRELATION_TYPE _relation = pFactionMan->getRelationClient(this, pTARGET);
        if ( _relation != Faction::EMRELATION_ERROR )
        {
            switch ( emTargetSide )
            {
            case EMIMPACT_SIDE_ENEMY:  // 적에게 : 적대적인 대상에게 상호작용일때 true;
                if (_relation == Faction::EMRELATION_ENEMY )
                    return TRUE;

                if ( ( _relation == Faction::EMRELATION_NEUTRAL_ENEMY ) && ( _bForceAction ) )  // 적대적인 중립관계의 대상에게는 강제공격으로 공격 가능;
                    return TRUE;

                //	대련자 공격 가능.;
                if( ISCONFRONT_TAR( sTargetID ) )
                    return true;

                // 적대행위자 공격 가능;
                if( IS_PLAYHOSTILE( pTARGET->GetCharID() ) )
                    return true;

                return false;

            case EMIMPACT_SIDE_PARTY:  // 파티에게 : 파티원인 대상에게 상호작용일때 true;
			case EMIMPACT_SIDE_OUR:

				// 팩션으로 판별한다;
				if ( _relation == Faction::EMRELATION_ALLY )
					return TRUE;

				return FALSE;

				// 파티원으로 판별하지 않는다;
                /*if ( m_pGaeaClient->GetMyPartyClient()->isMember(sTargetID.GaeaId) )
                    return TRUE;*/

            case EMIMPACT_SIDE_ANYBODY: // 모두에게 : 모든 대상에게 true;
                //코드가 자신을 속일지라도 헷갈리지 말자.. EMIMPACT_SIDE_ANYBODY는 모든대상이 아니다.. 플레이어와 NPC만이다....;
                if ( ( pTARGET->GetCrow() == CROW_PC ) || ( pTARGET->GetCrow() == CROW_NPC ) )
                    return TRUE;

                return false;

            default:
                break;
            }
        }
    }

    return IsReActionableNonFaction(sTargetID, emTargetSide, _bForceAction);
} 
