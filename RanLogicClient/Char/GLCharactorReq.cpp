#include "pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../enginelib/DxTools/DxClubMan.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"

#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/stl_Func.h"
#include "../../RanLogic/Transport/GLBusStation.h"
#include "../../RanLogic/Transport/GLBusData.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../../RanLogic/Land/GLMapList.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Quest/GLQUEST.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/NpcTalk/NpcTalkCondition.h"
#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/Skill/GLRnSkillLearnSetting.h"
#include "../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../../RanLogic/ItemPeriodExtension/ItemPeriodExtension.h"
#include "../../RanLogic/Character/CharacterSlot/GLCharSlotMan.h"
#include "../../RanUI/OldUI/Interface/ChatType.h"
#include "../../RanLogicClient/CostumeStat/CotStatClient.h"
#include "../../RanLogicClient/CapturedTheField/GLCaptureTheFieldClient.h"

#include "../../MfcExLib/RanFilter.h"

#include "../Land/GLLandManClient.h"
#include "../Pet/GLPetClient.h"
#include "../Stage/DxGlobalStage.h"
#include "../Party/GLPartyClient.h"
#include "../Friend/GLFriendClient.h"
#include "../Effect/GLFactEffect.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"
#include "../StaticHoldItem.h"
#include "../NpcTalk/GLNpcTalkClient.h"
#include "../Crow/GLCrowRenList.h"

// for test by luxes.
#include <time.h>
#include <Mmsystem.h>

#include "../RanLogicClient/Transport/GLTaxiStationClient.h"
#include "../../RanLogic/Prison/LimitPrison.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool GLCharacter::ReqMacroModeOnoff(bool bOn)
{
	if ( bOn )
	{
		if ( GLUseFeatures::GetInstance().IsUsingMacro_LimitTime() && m_fRemainMacroTime <= 0.0f )
		{
			PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_ACTIVETEXT",0).GetString());
			return false;
		}

		GLMSG::SNET_MACROMODE_ONOFF_CF msg;
		msg.bOn = true;
		m_pGaeaClient->NETSENDTOFIELD(&msg);
	}
	else
	{
		GLMSG::SNET_MACROMODE_ONOFF_CF msg;
		msg.bOn = false;
		m_pGaeaClient->NETSENDTOFIELD(&msg);
	}

	return true;
}

void GLCharacter::ReqToggleRun ()
{
	if ( IsActState(EM_ACT_RUN) )
	{
		ReSetActState(EM_ACT_RUN);

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetRun, 
			"-b", false );
	}
	else
	{
		SetActState(EM_ACT_RUN);		

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetRun, 
			"-b", true );
	}	

	ClientActor::SetMaxSpeed(GetMoveVelo());

	GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
	if ( pMyPet->IsVALID () )
	{
		pMyPet->SetMoveState ( IsActState(EM_ACT_RUN) );
	}

	//! �޼��� �߻�.
	//
	GLMSG::SNETPC_MOVESTATE NetMsg;
	NetMsg.dwActState = GetActState();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLCharacter::ReqTogglePeaceMode ()
{
	if ( IsActState(EM_ACT_PEACEMODE) && !m_emVehicle )
		ReSetActState(EM_ACT_PEACEMODE);
	else
		SetActState(EM_ACT_PEACEMODE);

	m_fIdleTime = 0.0f;

	//! �޼��� �߻�.
	//
	GLMSG::SNETPC_ACTSTATE NetMsg(GetActState());
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLCharacter::ReqVisibleNone ()
{
	SetActState(EM_REQ_VISIBLENONE);

	//! (������Ʈ����) �޼��� �߻�.
	//
	GLMSG::SNETPC_ACTSTATE NetMsg(GetActState());
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLCharacter::ReqVisibleOff ()
{
	SetActState(EM_REQ_VISIBLEOFF);

	//! (������Ʈ����) �޼��� �߻�.
	//
	GLMSG::SNETPC_ACTSTATE NetMsg(GetActState());
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

void GLCharacter::ReqVisibleOn ()
{
	ReSetActState(EM_REQ_VISIBLENONE);
	ReSetActState(EM_REQ_VISIBLEOFF);

	//! (������Ʈ����) �޼��� �߻�.
	//
	GLMSG::SNETPC_ACTSTATE NetMsg(GetActState());
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
}

// *****************************************************
// Desc: �ⱸ ������ ��û
// *****************************************************
void GLCharacter::ReqGateOut (const DWORD dwIndex)
{
	if ( IsDie() )
		return;

	if ( IsActState(EM_REQ_GATEOUT) )
		return;

	if ( IsActState(EM_ACT_WAITING) )
		return;

	DWORD dwGateID = DetectGate ();
	if ( dwGateID==UINT_MAX )
		return;

	PLANDMANCLIENT pLandMClient = m_pGaeaClient->GetActiveMap();	
	if ( !pLandMClient )
		return;

	DxLandGateMan *pLandGateMan = pLandMClient->GetLandGateMan();
	if ( !pLandGateMan )
		return;

	PDXLANDGATE pLandGate = pLandGateMan->FindLandGate ( dwGateID );
	if ( !pLandGate )
		return;

	const SNATIVEID toMapID = pLandGate->GetToMapID(dwIndex);
	const DWORD dwToGateID = pLandGate->GetToGateID(dwIndex);

	const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( toMapID );
	if ( !pMapNode )
		return;

	/* GM ĳ������ ��� ���� ����. �������� üũ�ϵ��� ��.
	if ( pMapNode->IsExpeditionMap() && (GetPartyID().isValidExpedition() == false) )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT("EMREQUIRE_EXPEDITION"));
		return;
	}
	*/


	//	TODO :  +, quest, ��Ƽ�� üũ�� ���� ����, Ŭ���̾�Ʈ������ ó���ϰ� ����. �������� ������ ���߿�.
	//
	//	TODO : �ⱸ ��� ������ �˻��Ͽ��� ������ ���� ������Ʈ �������� Ȯ���� �� �ִ�
	//		�ɸ��� ������ �Ѱ谡 �����Ƿ� Ŭ���̾�Ʈ������ Ȯ�εǰ� ����.
	//

	EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);
	
	GLLevelFileClient cLevelFile(m_pGaeaClient);
	BOOL bOk = cLevelFile.LoadFile ( pMapNode->m_LevelFile.c_str(), NULL );
	if ( !bOk )
		return;

	const SLEVEL_REQUIRE* pRequire = cLevelFile.GetLevelRequire ();
	emReqFail = pRequire->ISCOMPLETE ( this );
	if ( emReqFail != EMREQUIRE_COMPLETE )
	{		
		CString strText;
		switch ( emReqFail )
		{
		case EMREQUIRE_LEVEL:
			{
				if (pRequire->m_signLevel == EMSIGN_FROMTO)
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(
                            ID2GAMEINTEXT("EMREQUIRE_LEVEL2"),
                            pRequire->m_wLevel,
                            pRequire->m_wLevel2));
				}
				else
				{
					std::string strSIGN;
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLevel).GetString() );
					else
						strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLevel].c_str());
						
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::DISABLE,
						sc::string::format(
						ID2GAMEINTEXT("EMREQUIRE_LEVEL"),
						pRequire->m_wLevel,
						strSIGN.c_str()));
				}
			}
			break;

		case EMREQUIRE_ITEM:
			{
				SITEM *pItem = GLogicData::GetInstance().GetItem ( pRequire->m_sItemID );
				if ( pItem )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_ITEM"), pItem->GetName()));
				}
			}
			break;

		case EMREQUIRE_SKILL:
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pRequire->m_sSkillID );
				if ( pSkill )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(
                            ID2GAMEINTEXT("EMREQUIRE_SKILL"),
                            pSkill->GetName()));
				}
			}
			break;

		case EMREQUIRE_LIVING:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLiving).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLiving].c_str());
			
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(
                        ID2GAMEINTEXT("EMREQUIRE_LIVING"),
                        pRequire->m_nLiving, strSIGN));
			}
			break;

		case EMREQUIRE_BRIGHT:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signBright).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signBright].c_str());

				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(
                        ID2GAMEINTEXT("EMREQUIRE_BRIGHT"),
                        pRequire->m_nBright, strSIGN.c_str()));
			}
			break;

		case EMREQUIRE_QUEST_COM:
			{
				CString strQUEST = "quest";
				GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( pRequire->m_sComQuestID.dwID );
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();
					}
					m_pGaeaClient->PrintMsgTextDlg(
						NS_UITEXTCOLOR::DISABLE,
						sc::string::format(ID2GAMEINTEXT("EMREQUIRE_QUEST_COM"), strQUEST.GetString()));
				}
			}
			break;

		case EMREQUIRE_QUEST_ACT:
			{
				CString strQUEST = "quest";
				GLQUEST* pQUEST = GLQuestMan::GetInstance().Find(pRequire->m_sActQuestID.dwID);
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();
					}
				}
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(
                        ID2GAMEINTEXT("EMREQUIRE_QUEST_ACT"),
                        strQUEST.GetString()));
			}
			break;

		default:
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MAP_REQUIRE_FAIL") );
			break;
		};

		//! �ⱸ ��� ������ �ȵ� ���. GM level �̻��� ��� ���� ����.
		//
		if ( m_dwUserLvl < USER_USER_GM )
			return;
	}

    DxGlobalStage* pGloblaStage = m_pGaeaClient->GetGlobalStage();
	if ( !pGloblaStage->IsEmulator() ) 
	{
		switch ( pMapNode->GetInstanceType() )
		{
		case SMAPNODE::EMINSTANCE_OLD_INSTANCE:
			{
				cLevelFile.SetUseInstantMap(GLLevelFile::EMINSTANCE_OLD_INSTANCE);

				bool bInPartyMember = false;
				GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
				if ( pMyParty->isMember(m_dwGaeaID) )
				{
					DWORD dwPartyNum = pMyParty->GetNMember();
					for (DWORD _i = 0; _i < dwPartyNum; _i++)
					{
						GLPartySlotClient* pMember = pMyParty->GetMember(_i);

						if ( !pMember || pMember->m_dwGaeaID == m_dwGaeaID )
							continue;

						if ( toMapID == pMember->m_sMapID.getBaseMapID() )
						{
							bInPartyMember = true;
							break;
						}
					}
				}

				if ( !bInPartyMember )
				{
					if ( cLevelFile.GetUseLevelEtcFunc(EMETCFUNC_REENTRYLIMIT) )
					{
						MAPENTRYINFO_ITER mapIter = m_mapEntryLimitInfo.find(toMapID.dwID);
						if ( mapIter != m_mapEntryLimitInfo.end() )
						{
							SENTRYLIMITINFO entryInfo = mapIter->second;

							int nRemainMinute = entryInfo.GetEntryLimitMinutes();
							m_pGaeaClient->PrintMsgTextDlg(
								NS_UITEXTCOLOR::DISABLE,
								sc::string::format(
								ID2GAMEINTEXT("REQ_CREATE_INSTANT_MAP_FAIL_REENTRYLIMIT"), 
								m_pGaeaClient->GetMapName( toMapID ),
								nRemainMinute));
							return;
						}
					}
				}
				//! �ν��Ͻ� �� ���� ��û
				//
				GLMSG::SNET_CREATE_INSTANT_MAP_REQ NetMsg;
				NetMsg.dwGateID = dwGateID;
				NetMsg.dwGateToIndex = dwIndex;
				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				SetActState(EM_REQ_GATEOUT);
			}
			return;	
		case SMAPNODE::EMINSTANCE_NEW_INSTANCE:
			break;
		}
	}

	PVP::GLCaptureTheFieldClient* pCTFClient = m_pGaeaClient->GetCaptureTheField();
	if ( pCTFClient )
	{
		if ( true == pCTFClient->IsCTFMap_PVP( toMapID ) )
		//if( toMapID == SNATIVEID((DWORD)221) )
		{
			m_pGaeaClient->GetCaptureTheField()->SetProgressCTF( PVP::ENUM_CTF_TYPE_POWERSTATION );
		}
	}
	
	//! �ⱸ ������ ��û �޽��� �߻�.
	//
	GLMSG::SNETREQ_GATEOUT_REQ NetMsg;
	NetMsg.dwGateID = dwGateID;
	NetMsg.dwGateToIndex = dwIndex;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	SetActState(EM_REQ_GATEOUT);
}

bool GLCharacter::ReqGESTURE ( int nMOTION, bool bCOMMAND )
{
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
    if ( pLand == NULL )
        return false;

    // �������¿����� ���Ұ�;
    if ( !pLand->IsPeaceZone() && !IsActState(EM_ACT_PEACEMODE) )
    {
        // ��ɾ� ��뿡 ���� �������� ���� ��ȭ��� ��ȯ���� �ν�;
        if ( bCOMMAND && isAction(GLAT_IDLE) ) 
        {
            m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_GESTURE_NOT_PEACEMODE") );
            ReqTogglePeaceMode();
        }
        return true;
    }
    // Ż�� ž�����϶� �Ұ�; // ����������� ��� �Ұ�; // ��� ���°� �ƴҰ�� �Ұ�;
    if ( ( m_emVehicle ) || ( !IsValidBody() ) || ((isAction(GLAT_IDLE) == false) && (isAction(GLAT_TALK) == false)) )
    {
        if ( bCOMMAND )
        {
            m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_GESTURE_NOT_STATE") );
        }
        return true;
    }

	if( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
	{
		if ( bCOMMAND )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MSG_GESTURE_NOT_STATE") );
		}
		return true;
	}

    //! �ش� ������ ���ϸ��̼��� ������ ����.
    const PANIMCONTNODE pNODE = m_pSkinChar->GETANI ( AN_GESTURE, EMANI_SUBTYPE(nMOTION) );
    if ( !pNODE )
        return false;

	//! �����ĸ� ����.
	m_dwANISUBGESTURE = (DWORD) nMOTION;
	TurnAction(GLAT_TALK);

	//! ������ �޽��� ����.
	GLMSG::SNETPC_REQ_GESTURE NetMsg;
	NetMsg.dwID = m_dwANISUBGESTURE;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return true;
}

bool GLCharacter::IsInsertToInven(std::tr1::shared_ptr<CItemClientDrop> spItemDrop)
{
	GASSERT(spItemDrop&&"GLChar::IsItemToInven()");
	if (!spItemDrop)
        return false;

	SITEM *pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
	if (!pItem)
        return false;

	if ( pItem->ISPILE() )
	{
		//	��ħ �������� ���.
		WORD wPILENUM = pItem->sDrugOp.wPileNum;
		SNATIVEID sNID = pItem->sBasicOp.sNativeID;

		//! �ֱ� ��û�� �����ۼ�. ( �ܿ���. )
		//
		WORD wREQINSRTNUM = ( spItemDrop->sItemClient.wTurnNum );

		BOOL bITEM_SPACE = TRUE;
//#if defined(VN_PARAM) //vietnamtest%%%
		if (m_ServiceProvider == SP_VIETNAM)
		{
			if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
			{
				bITEM_SPACE = m_cVietnamInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM );
			}
            else
            {
				bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM );
			}
		}
		else
		{
//#else
			bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM );
		}
//#endif
		if ( !bITEM_SPACE )
			return false;
	}
	else
	{
		WORD wPosX, wPosY;
		BOOL bOk = TRUE;
//#if defined(VN_PARAM) //vietnamtest%%%
		if (m_ServiceProvider == SP_VIETNAM)
		{
			if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
			{
				bOk = m_cVietnamInventory.FindInsertable ( wPosX, wPosY );
			}
			else
			{
				bOk = m_cInventory.FindInsertable ( wPosX, wPosY );
			}
		}
		else
		{
//#else
			bOk = m_cInventory.FindInsertable ( wPosX, wPosY );
		}
//#endif
		if ( !bOk )
			return false;
	}

	return true;
}

bool GLCharacter::IsInsertToInven(const SITEM* pItem)
{
	if (!pItem)
		return false;

	WORD wPosX, wPosY;
	BOOL bOk = TRUE;
	//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
		{
			bOk = m_cVietnamInventory.FindInsertable ( wPosX, wPosY );
		}
		else
		{
			bOk = m_cInventory.FindInsertable ( wPosX, wPosY );
		}
	}
	else
	{
		//#else
		bOk = m_cInventory.FindInsertable ( wPosX, wPosY );
	}
	//#endif
	if ( !bOk )
		return false;

	return true;
}

//! �ʵ� ������(��) ������.
HRESULT GLCharacter::ReqFieldTo ( const STARGETID &sTargetID, bool bPet )
{
	if ( HaveHoldItem () )
		return E_FAIL;

	if ( ValidWindowOpen() )
		return E_FAIL;
	
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( !pLAND )
		return E_FAIL;

	if (m_emVehicle != EMVEHICLE_OFF)
		return E_FAIL;

	//if ( sTargetID.emCrow==CROW_ITEM )
	//{
	//	if ( pLAND->ISITEM_PICKDELAY ( sTargetID.dwID ) )	return E_FAIL;
	//}
	//else if ( sTargetID.emCrow==CROW_MONEY )
	//{
	//	if ( pLAND->ISMONEY_PICKDELAY ( sTargetID.dwID ) )	return E_FAIL;
	//}

	//BOOL bInventoryOpen = FALSE;
	//bInventoryOpen = m_pInterface->IsInventoryWindowOpen ();
	//if ( sTargetID.emCrow==CROW_ITEM && bInventoryOpen )
	//{
	//	//	�޽��� �߻�.
	//	GLMSG::SNETPC_REQ_FIELD_TO_HOLD NetMsg;
	//	NetMsg.dwGlobID = sTargetID.dwID;
	//	m_pGaeaClient->NETSENDTOFIELD ( &NetMsg );

	//	//	���� �޽��� ���� ����.
	//	pLAND->SETITEM_PICKDELAY ( sTargetID.dwID );
	//}
	//else
	{
		//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//
		// ���Ȯ��
		if ( !IsValidBody() )
            return E_FAIL;

		//	�Ÿ� üũ
		const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

		//	�Ÿ� üũ
		D3DXVECTOR3 vPos;

		if ( bPet )	
		{
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet->IsVALID() )	
				vPos = pMyPet->GetPosition();
		}	
		else 
			vPos = GetPosition();

		D3DXVECTOR3 vDistance = vPos - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wTakeRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wTakeAbleDis = wTakeRange + 15;

		if ( fDistance>wTakeAbleDis )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAKE_FB_DISTANCE") );			
			return E_FAIL;
		}

		if ( sTargetID.emCrow==CROW_ITEM )
		{
            std::tr1::shared_ptr<CItemClientDrop> spItemDrop = pLAND->GetItem(sTargetID.GaeaId);
			if (!spItemDrop)
                return E_FAIL;

			SITEM *pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID);
			if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_GetQBoxLock ) )
			{
				//SITEM *pItem = GLogicData::GetInstance().GetItem(spItemDrop->sItemClient.sNativeID); Q�ڽ� üũ�� ���� �������� �̵���
				if ( pItem != NULL && pItem->sBasicOp.emItemType==ITEM_QITEM )
				{
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("QBOX_OPTION_DISABLE_MSG") );
					return E_FAIL;
				}
			}

			//! �κ��� ������ �ִ��� �˻�.
			//
			BOOL bOk = IsInsertToInven(spItemDrop);

			//Q�ڽ� �ϰ�� �κ��丮�� �������־ ������ �޽����� ������
			if ( pItem != NULL && pItem->sBasicOp.emItemType==ITEM_QITEM )
				bOk = TRUE;

			if ( !bOk )
			{
////#if defined(VN_PARAM) //vietnamtest%%%
//				if (m_ServiceProvider == SP_VIETNAM)
//					if ( m_dwVietnamGainType == GAINTYPE_EMPTY )
//						return E_FAIL;
////#endif
//                PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_13") ); 
//                
//				//	�ι��� ��������.
//				//	�޽��� �߻�.
//				GLMSG::SNETPC_REQ_FIELD_TO_HOLD NetMsg;
//				NetMsg.dwGlobID = sTargetID.GaeaId;
//				m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
//				�κ��丮�� �������� ���� �� ���¿��� �ٴۿ� �������� ȹ�� �Ͽ� 
//				Ȧ�� �Ǿ� �ִ� �������� ������� ���� ����
				
				if (IsPartyMem())
				{
					GLMSG::SNETPC_REQ_FIELD_TO_INVEN NetMsg;
					NetMsg.emCrow = sTargetID.emCrow;
					NetMsg.dwID = sTargetID.GaeaId;
					NetMsg.bPet	= bPet;
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				}

					PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_13") );
				return S_OK;
			}

			//	���� �޽��� ���� ����.
			// pLAND->SETITEM_PICKDELAY ( sTargetID.dwID );
		}
		else if ( sTargetID.emCrow==CROW_MONEY )
		{
            std::tr1::shared_ptr<CMoneyClientDrop> spMoney = m_pGaeaClient->GetActiveMap()->GetMoney(sTargetID.GaeaId);
			if (!spMoney)
                return E_FAIL;

			//	���� �޽��� ���� ����.
			// pLAND->SETMONEY_PICKDELAY ( sTargetID.dwID );
		}

		//	�޽��� �߻�.
		GLMSG::SNETPC_REQ_FIELD_TO_INVEN NetMsg;
		NetMsg.emCrow = sTargetID.emCrow;
		NetMsg.dwID = sTargetID.GaeaId;
		NetMsg.bPet	= bPet;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}

	return S_OK;
}

//! �κ��丮 ������ �鶧, ������, ��ȯ�Ҷ�, ��ĥ��.
HRESULT GLCharacter::ReqInvenTo( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);

	//! Ʈ���̵� Ȱ��ȭ�ÿ�.
	//
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
	{
		if ( pInvenItem )
            m_pGaeaClient->GetTradeClient()->SetPreItem( SINVEN_POS( pInvenItem->wPosX, pInvenItem->wPosY ) );
		else
            m_pGaeaClient->GetTradeClient()->ReSetPreItem();

		return S_OK;
	}

	if ( ValidCostumeStatOpen() || ValidRebuildOpen() || ValidGarbageOpen() || ValidItemMixOpen() || IsPostBoxWindowOpen() || IsPrivateMarketSerchWindowOpen() )	// ITEMREBUILD_MARK
	{
		if ( m_sPreInventoryItem.wPosX == wPosX && m_sPreInventoryItem.wPosY == wPosY )
		{
			m_sPreInventoryItem.RESET();
		}
		else
		{
			if ( pInvenItem )
				m_sPreInventoryItem.SET( wPosX, wPosY );
			else
				m_sPreInventoryItem.RESET();
		}

		return S_OK;
	}

    if ( HaveHoldItem() )
    {
        GLMSG::SNETPC_REQ_HOLD_TO_INVEN NetMsg;
        NetMsg.wPosX = wPosX;
        NetMsg.wPosY = wPosY;
        NetMsg.bUseVietnamInven = (m_ServiceProvider == SP_VIETNAM ? GET_HOLD_ITEM().bVietnamGainItem : FALSE);
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

        return S_OK;
    }

    if ( !holditem::HaveHoldItem() && !pInvenItem )
    {
        holditem::ReleaseHoldItem();

		return E_FAIL;
    }

	if ( holditem::HaveHoldItem() && pInvenItem )
	{
/*
#if defined(VN_PARAM) //vietnamtest%%%
		const SITEMCUSTOM& sCustom = GET_HOLD_ITEM();
		
		if ( !sCustom.bVietnamGainItem )
#endif
		{
			GLMSG::SNETPC_REQ_INVEN_EX_HOLD NetMsg;
			NetMsg.wPosX = pInvenItem->wPosX;
			NetMsg.wPosY = pInvenItem->wPosY;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
*/

        if ( m_ServiceProvider != SP_VIETNAM || ( m_ServiceProvider == SP_VIETNAM && !holditem::GetHoldItem().bVietnamGainItem ) )
		{
            switch ( holditem::WhenHoldItem() )
            {
            case holditem::INVENTORY:
                {
                    if ( wPosX == holditem::GetHoldPosX() &&
                        wPosY == holditem::GetHoldPosY() )
                    {
                        holditem::ReleaseHoldItem();

                        return S_OK;
                    }

                    GLMSG::SNETPC_REQ_INVEN_EX_INVEN NetMsg;
                    NetMsg.wPosX = pInvenItem->wPosX;
                    NetMsg.wPosY = pInvenItem->wPosY;
                    NetMsg.wHoldPosX = holditem::GetHoldPosX();
                    NetMsg.wHoldPosY = holditem::GetHoldPosY();
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::STORAGE:
                {
                    DWORD dwChannel, dwNPCGlobalID;
                    holditem::GetData( dwChannel, dwNPCGlobalID );

                    GLMSG::SNETPC_REQ_STORAGE_EX_INVEN NetMsg;
                    NetMsg.dwChannel = dwChannel;
                    NetMsg.dwNpcGlobID = dwNPCGlobalID;
                    NetMsg.wPosX = pInvenItem->wPosX;
                    NetMsg.wPosY = pInvenItem->wPosY;
                    NetMsg.wHoldPosX = holditem::GetHoldPosX();
                    NetMsg.wHoldPosY = holditem::GetHoldPosY();
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::WEAR_SLOT:
                {
                    HRESULT hr;
                    hr = ReqInvenToSpecificSlot( static_cast<EMSLOT>(holditem::GetHoldPosX()), pInvenItem->wPosX, pInvenItem->wPosY );

                    if ( hr != S_OK )
                        return hr;
                }
                break;

            case holditem::PET_SLOT:
                {
                    EMSUIT emSuit = static_cast<EMSUIT>( holditem::GetHoldPosX() );

                    GLMSG::SNETPET_REQ_INVEN_EX_SLOT NetMsg( emSuit );
                    NetMsg.wHoldPosX = pInvenItem->wPosX;
                    NetMsg.wHoldPosY = pInvenItem->wPosY;
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::VEHICLE_SLOT:
                {
                    EMSUIT emSuit = static_cast<EMSUIT>( holditem::GetHoldPosX() );

                    GLMSG::SNET_VEHICLE_REQ_INVEN_EX_SLOT NetMsg;
                    NetMsg.emSuit = emSuit;
                    NetMsg.wPosX =  pInvenItem->wPosX;
                    NetMsg.wPosY =  pInvenItem->wPosY;
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::CLUB_STORAGE:
                {
                    // Note : agent�� field �� ������� �������� ��ȯ�ؾ��ؼ�, ������ ���� ������ ����.
                    /*DWORD dwChannel, dwDummy;
                    holditem::GetData( dwChannel, dwDummy );

                    GLMSG::NET_CLUB_STORAGE_EX_INVEN_CA SendData;
                    SendData.m_Channel = static_cast<WORD>(dwChannel);
                    SendData.m_ClubStoragePosX = holditem::GetHoldPosX();
                    SendData.m_ClubStoragePosY = holditem::GetHoldPosY();
                    SendData.m_InvenPosX = pInvenItem->wPosX;
                    SendData.m_InvenPosY = pInvenItem->wPosY;
                    msgpack::sbuffer SendBuffer;
                    msgpack::pack(SendBuffer, SendData);
                    m_pGaeaClient->NETSENDTOAGENT(NET_MSG_CLUB_ITEM_EXCHANGE_TO_INVEN_CA, SendBuffer, true);*/
                }
                break;
            };
		}

        holditem::ReleaseHoldItem();
	}
	else if ( pInvenItem )
	{
		holditem::HoldItem( holditem::INVENTORY, pInvenItem->CustomItem(), wPosX, wPosY );
	}
	else if ( holditem::HaveHoldItem() )
	{
		//
		//mjeon		
		//�� �κ��� ifdef VN_PARAM ������ �ڵ�� �������ؼ� ������. ���� �ڵ�� History ����.
		//
        const SITEMCUSTOM& sCustom = holditem::GetHoldItem();

        if ( m_ServiceProvider == SP_VIETNAM )
        {
            if ( sCustom.bVietnamGainItem && m_dwVietnamInvenCount <= 0 )
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_VIETNAM_ITEMGET_FAILED") );

                holditem::ReleaseHoldItem();

                return E_FAIL;		
            }
        }

		//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//
        SITEM* pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().GetNativeID() );
		GASSERT(pItem&&"������ �����Ͱ� �������� ����");

		BOOL bOk = m_cInventory.IsInsertable( wPosX, wPosY );
		if ( !bOk )
		{
            holditem::ReleaseHoldItem();
			//	�ι��� ��������.
			return E_FAIL;
		}

        switch ( holditem::WhenHoldItem() )
        {
        case holditem::INVENTORY:
            {
                GLMSG::SNETPC_REQ_INVEN_TO_INVEN NetMsg;
                NetMsg.wPosX = wPosX;
                NetMsg.wPosY = wPosY;
                NetMsg.wHoldPosX = holditem::GetHoldPosX();
                NetMsg.wHoldPosY = holditem::GetHoldPosY();
                NetMsg.bUseVietnamInven = (m_ServiceProvider == SP_VIETNAM ? sCustom.bVietnamGainItem : FALSE);

                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
            break;

        case holditem::STORAGE:
            {
                DWORD dwChannel, dwNPCGlobalID;
                holditem::GetData( dwChannel, dwNPCGlobalID );

                GLMSG::SNETPC_REQ_STORAGE_TO_INVEN NetMsg;
                NetMsg.dwChannel = dwChannel;
                NetMsg.dwNpcGlobID = dwNPCGlobalID;
                NetMsg.wPosX = wPosX;
                NetMsg.wPosY = wPosY;
                NetMsg.wHoldPosX = holditem::GetHoldPosX();
                NetMsg.wHoldPosY = holditem::GetHoldPosY();
                NetMsg.bUseVietnamInven = (m_ServiceProvider == SP_VIETNAM ? sCustom.bVietnamGainItem : FALSE);

                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
            break;

        case holditem::WEAR_SLOT:
            {
                GLMSG::SNETPC_REQ_SLOT_TO_INVEN NetMsg;
                NetMsg.wPosX = wPosX;
                NetMsg.wPosY = wPosY;
                NetMsg.emFromSlot = static_cast<EMSLOT>( holditem::GetHoldPosX() );
                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
            break;

        case holditem::PET_SLOT:
            {
                EMSUIT emSuit = static_cast<EMSUIT>( holditem::GetHoldPosX() );

                GLMSG::SNETPET_REQ_SLOT_TO_INVEN NetMsg(emSuit);
                NetMsg.wPosX = wPosX;
                NetMsg.wPosY = wPosY;
                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
            break;

        case holditem::VEHICLE_SLOT:
            {
                EMSUIT emSuit = static_cast<EMSUIT>( holditem::GetHoldPosX() );

                GLMSG::SNET_VEHICLE_REQ_SLOT_TO_INVEN NetMsg;
                NetMsg.emSuit = emSuit;
                NetMsg.wPosX = wPosX;
                NetMsg.wPosY = wPosY;
                m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            }
            break;

        case holditem::CLUB_STORAGE:
            {
                DWORD dwChannel, dwDummy;
                holditem::GetData( dwChannel, dwDummy );

                GLMSG::NET_CLUB_STORAGE_TO_INVEN_CA SendData;
                SendData.m_Channel = static_cast<WORD>(dwChannel);
                SendData.m_PosX = wPosX;
                SendData.m_PosY = wPosY;
                SendData.m_HoldPosX = holditem::GetHoldPosX();
                SendData.m_HoldPosY = holditem::GetHoldPosY();
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
                m_pGaeaClient->NETSENDTOAGENT(NET_MSG_CLUB_STORAGE_TO_INVEN_CA, SendBuffer, true);
            }
            break;
        };

        holditem::ReleaseHoldItem();
	}

	return S_OK;
}

//! ��Ʈ�� Ž�� ���� �κ��丮 ������ �鶧, ������, ��ȯ�Ҷ�, ��ĥ��.
HRESULT GLCharacter::ReqVNInvenTo( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( ValidWindowOpen() )
        return E_FAIL;	

	SINVENITEM* pInvenItem = m_cVietnamInventory.FindPosItem(wPosX, wPosY);
	if ( !HaveHoldItem () && !pInvenItem )
        return E_FAIL;


	const SITEMCUSTOM& sCustom = GET_HOLD_ITEM();


	if ( HaveHoldItem () && pInvenItem )
	{
		if ( sCustom.bVietnamGainItem )
		{
			GLMSG::SNETPC_REQ_VNGAIN_EX_HOLD NetMsg;
			NetMsg.wPosX = pInvenItem->wPosX;
			NetMsg.wPosY = pInvenItem->wPosY;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
	else if ( pInvenItem )
	{
		GLMSG::SNETPC_REQ_VNGAIN_TO_HOLD NetMsg;
		NetMsg.wPosX = pInvenItem->wPosX;
		NetMsg.wPosY = pInvenItem->wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
	else if ( HaveHoldItem () && sCustom.bVietnamGainItem )
	{
		//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//
		SITEM* pItem = GLogicData::GetInstance().GetItem ( GET_HOLD_ITEM().GetNativeID() );
		GASSERT(pItem&&"������ �����Ͱ� �������� ����");

		BOOL bOk = m_cVietnamInventory.IsInsertable ( wPosX, wPosY );
		if ( !bOk )
		{
			//	�ι��� ��������.
			return E_FAIL;
		}

		GLMSG::SNETPC_REQ_HOLD_TO_VNGAIN NetMsg;
		NetMsg.wPosX = wPosX;
		NetMsg.wPosY = wPosY;

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
	return S_OK;
}

//! ��Ʈ�� �κ��丮���� ������ ��ư���� �������� �ű� ���
HRESULT GLCharacter::ReqVietemInvenTo (WORD wPosX, WORD wPosY )
{

	if ( m_dwVietnamInvenCount <= 0 )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_VIETNAM_ITEMGET_FAILED") );
		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cVietnamInventory.FindPosItem(wPosX, wPosY);

	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

	WORD wInsertPosX(0), wInsertPosY(0);
	BOOL bITEM_SPACE(FALSE);
	bITEM_SPACE = m_cInventory.FindInsertable ( wInsertPosX, wInsertPosY );

	if ( !bITEM_SPACE )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOINVEN") );
		return E_FAIL;
	}

	//	���� ��û �޽���.
	GLMSG::SNETPC_REQ_VNINVEN_TO_INVEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! ��Ʈ�� �κ��丮�� ������ ��ü ����
HRESULT GLCharacter::ReqVNInveReset ()
{
	m_cVietnamInventory.DeleteItemAll();

	GLMSG::SNETPC_REQ_VNGAIN_INVEN_RESET NetMsg;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqLockBoxUnlock(WORD wPosX, WORD wPosY)
{
	if ( !IsValidBody() )
		return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
		return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if (NULL == pItem)
		return E_FAIL;

	if (ITEM_LOCK_BOX != pItem->sBasicOp.Type())
		return E_FAIL;

	if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		return E_FAIL;

	SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
		return E_FAIL;

	if ( pHold->sBasicOp.emItemType != ITEM_LOCK_BOX_KEY )
		return E_FAIL;

	GLMSG::SNET_INVEN_LOCKBOX_UNLOCK NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wHoldPosX = holditem::GetHoldPosX();
	NetMsg.wHoldPosY = holditem::GetHoldPosY();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqLockBoxOpen(WORD wPosX, WORD wPosY)
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
		return E_FAIL;
	}
	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_LOCK_BOX )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
		return E_FAIL;
	}

	if ( !pItem->sRandomBox.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_EMPTY") );
		return E_FAIL;
	}

	//! ������ ���� ������ ��û.
	//
	GLMSG::SNET_INVEN_LOCKBOX_OPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::SelectiveformBoxOpen(WORD wPosX, WORD wPosY)
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_SELECTIVEFORM_BOX )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("SELFORMBOX_MSG", 0) );
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("SELFORMBOX_MSG", 1));
		return E_FAIL;
	}

	m_nSelFormBoxPosX = wPosX;
	m_nSelFormBoxPosY = wPosY;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_SelformBox_SetBoxID,
		"-n, -n",
		pItem->BasicId().Mid(), pItem->BasicId().Sid());
}

// *****************************************************
// Desc: ������ ��þƮ �õ�
// *****************************************************
void GLCharacter::ReqTLGrinding()
{
	GLMSG::SNET_INVEN_TL_GRINDING NetMsg;

	NetMsg.wPosX	 = m_wInvenPosX1;
	NetMsg.wPosY	 = m_wInvenPosY1;
	NetMsg.wHoldPosX = m_wInvenPosX2;
	NetMsg.wHoldPosY = m_wInvenPosY2;

	NetMsg.eResult = EMREQ_TEMP_ENCHANT_EXIST;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

HRESULT GLCharacter::ReqTLGrinding( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
		return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
		return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem ) 	return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem ) return E_FAIL;

	if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		return E_FAIL;

	SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold ) return S_FALSE;

	switch ( GRINDING_TL_CONDITION_CHECK(pInvenItem->sItemCustom, pHold,  holditem::GetHoldItem(), m_pGaeaClient->GetCurrentTime()) )
	{
	case EMGRINDINGCONDITION_MAX        :
	case EMGRINDINGCONDITION_FAILED     : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_FAIL") );     return E_FAIL;
	case EMGRINDINGCONDITION_TIME_LIMIT :
	case EMGRINDINGCONDITION_NOITEM     : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_NOT") );      return E_FAIL;
	case EMGRINDINGCONDITION_DEFCLASS   : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_NOT_ITEM") ); return E_FAIL;
	case EMGRINDINGCONDITION_MIN        : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_GRADE_NOT") );		  return E_FAIL;
	case EMGRINDINGCONDITION_RE_ENCHANT : 
		{
			m_wInvenPosX1 = wPosX;
			m_wInvenPosY1 = wPosY;
			m_wInvenPosX2 = holditem::GetHoldPosX();
			m_wInvenPosY2 = holditem::GetHoldPosY();

			//!! SF_TODO
			//m_pInterface->DoModal( ID2GAMEINTEXT("TEMP_ENCHANT_EXIST"), UI::MODAL_QUESTION, UI::OKCANCEL, UI::MODAL_TEMP_ENCHANT );		
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_TempEnchant,
				"-n, -n", wPosX, wPosY );

			holditem::ReleaseHoldItem();
		}
		return S_OK;
	}

	GLMSG::SNET_INVEN_TL_GRINDING NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wHoldPosX = holditem::GetHoldPosX();
	NetMsg.wHoldPosY = holditem::GetHoldPosY();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	holditem::ReleaseHoldItem();

	return S_OK;
}
HRESULT GLCharacter::ReqGrinding( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
        return S_FALSE;

	if ( pHold->sBasicOp.emItemType != ITEM_GRINDING )
        return S_FALSE;

	switch ( GRINDING_CONDITION_CHECK(pInvenItem->sItemCustom, pHold,  holditem::GetHoldItem()) )
	{
	default:
	case EMGRINDINGCONDITION_FAILED:
		return S_FALSE;

	case EMGRINDINGCONDITION_SUCCESS:
		{
			//Blank
		}
		break;
	case EMGRINDINGCONDITION_TLGRIND_ENCHANT :
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TEMP_ENCHANT_NOT_ITEM") );
			return S_FALSE;
		}
		break;
	case EMGRINDINGCONDITION_NOITEM:
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_NOITEM") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_MAX:
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_MAX") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_NOTBEST:
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_NOT_BEST") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_NOTHIGH:
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_NOT_HIGH") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_NOTBESTITEM:
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_NOT_BESTITEM") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_NOTNUM:
		{
            WORD wNUM = GRINDING_CONSUME_NUM(pInvenItem->sItemCustom, pHold, holditem::GetHoldItem());
            std::string strText = sc::string::format( ID2GAMEINTEXT("GRINDING_NOT_NUM"), wNUM );
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, strText );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_DEFCLASS:
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_DEFCLASS") );
			return S_FALSE;
		}

	case EMGRINDINGCONDITION_MIN:
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_MIN") );
			return S_FALSE;
		}
		break;

	case EMGRINDINGCONDITION_RANDOM_MAX:
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("GRINDING_RANDOM_MAX") );
			return S_FALSE;
		}
		break;

	};

	//! ������ ���� �õ� ��û.
	//
	GLMSG::SNET_INVEN_GRINDING NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! �Ⱓ �����ϱ�;
HRESULT GLCharacter::ReqPeriodExtend(WORD wPosX, WORD wPosY)
{
	if ( !IsValidBody() || m_pGaeaClient->GetTradeClient()->Valid())
		return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY)
		, *pHoldItem = m_cInventory.FindPosItem(holditem::GetHoldPosX(), holditem::GetHoldPosY());
	if ( !pInvenItem )
		return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		return E_FAIL;

	SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
		return S_FALSE;

	if ( pHold->sBasicOp.emItemType != ITEM_PERIODEXTANSION )
		return S_FALSE;

	//! �Ⱓ���� �õ�;
	GLMSG::SNET_INVEN_PERIODEXTEND NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wHoldPosX = holditem::GetHoldPosX();
	NetMsg.wHoldPosY = holditem::GetHoldPosY();

//	stringformat( "%1% : %2%(%3%)", ID2GAMEWORD_QUEST, *itr, strAdditionalExplan)
//	ID2GAMEINTEXT("ITEM_PERIODEX_CONFIRM"), pItem->GetName(), 

	std::string strMsg;
	__time64_t tPrev, tNext;
	DWORD dwTestFB(ItemPeriodExtension::Manager::GetInstance().DoTest(pHoldItem->CustomItem(), pInvenItem->CustomItem(), tPrev, tNext));
	switch ((ItemPeriodExtension::EMPERIODEX_TESTFB)dwTestFB)
	{
	case ItemPeriodExtension::EMPERIODEX_FB_FAIL:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL") );
			return S_FALSE;
		}
	case ItemPeriodExtension::EMPERIODEX_FB_FAIL_DUPLICATION:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL_DUPLICATION") );
			return S_FALSE;
		}
	case ItemPeriodExtension::EMPERIODEX_FB_FAIL_LIMITCOUNT:
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_PERIODEX_FAIL_LIMITCOUNT") );
			return S_FALSE;
		}
	}

	{
		std::string strPrevTime, strNextTime;
		CTime cPrevTime(tPrev), cNextTime(tNext);
		m_pGaeaClient->GetConvertServerTime( cPrevTime );
		if ( cPrevTime.GetYear() > 1970 )
		{
			strPrevTime = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cPrevTime.GetYear ()%2000), cPrevTime.GetMonth(), cPrevTime.GetDay(), cPrevTime.GetHour(), cPrevTime.GetMinute() );
		}

		m_pGaeaClient->GetConvertServerTime( cNextTime );
		if ( cNextTime.GetYear() > 1970 )
		{
			strNextTime = sc::string::format( ID2GAMEWORD("ITEM_EXPIRE_DATE"),
				(cNextTime.GetYear ()%2000), cNextTime.GetMonth(), cNextTime.GetDay(), cNextTime.GetHour(), cNextTime.GetMinute() );
		}

		strPrevTime = NSWIDGET_SCRIPT::ToHtmlColorText( 
			strPrevTime, 
			NS_UITEXTCOLOR::DARKORANGE );
		strNextTime = NSWIDGET_SCRIPT::ToHtmlColorText( 
			strNextTime, 
			NS_UITEXTCOLOR::GREEN );

		if ( dwTestFB == ItemPeriodExtension::EMPERIODEX_FB_SUCCESS_BASE )
		{
			strMsg = sc::string::format( ID2GAMEINTEXT("ITEM_PERIODEX_CONFIRM")
				, pItem->GetName()
				, strPrevTime.c_str()
				, strNextTime.c_str() );
		}
		else //if ( dwTestFB == ItemPeriodExtension::EMPERIODEX_FB_SUCCESS_COSTUME )
		{
			SITEM* pItemDisguise = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.nidDISGUISE );
			if ( !pItemDisguise )
				return E_FAIL;

			strMsg = sc::string::format( ID2GAMEINTEXT("ITEM_PERIODEX_CONFIRM")
				, pItemDisguise->GetName()
				, strPrevTime.c_str()
				, strNextTime.c_str() );
		}
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_PeriodExConfirm,
		"-n, -n, -n, -n, -s"
		, holditem::GetHoldPosX()
		, holditem::GetHoldPosY(), wPosX, wPosY
		, strMsg.c_str());

	holditem::ReleaseHoldItem();
	return S_OK;
}

HRESULT GLCharacter::ReqDisguise( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

    if ( !holditem::HaveHoldItem() )
        return E_FAIL;

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
        return S_FALSE;

	// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	if ( !CHECK_ACTIVITY_POINT( *pHold ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ERR_ACTIVITY") );
        return S_FALSE;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pHold ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ERR_CONTRIBUTION") );
        return S_FALSE;
    }

	//if ( !pHold->sBasicOp.IsDISGUISE() )
	if ( !( holditem::GetHoldItem().IsDisguise() != NULL ) )
        return S_FALSE;

	//if ( pItem->sBasicOp.IsDISGUISE() )
	if ( pInvenItem->sItemCustom.IsDisguise() != NULL )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_FAIL") );
		return S_FALSE;
	}

	if ( ( (pHold->sBasicOp.dwReqCharClass & pItem->sBasicOp.dwReqCharClass) == NULL)
		 ||(pHold->sSuitOp.emSuit != pItem->sSuitOp.emSuit) )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_DEFSUIT") );
		return S_FALSE;
	}
	
	if ( pHold->sBasicOp.emItemType != ITEM_SUIT )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_NOTSUIT") );
		return S_FALSE;
	}

	// ���� Ÿ�Կ� �ڽ�Ƭ�� ���� �� �ֳĴ� ���ǰ� �Դ�;
	// 	if ( (pHold->sSuitOp.emAttack != pItem->sSuitOp.emAttack ) && ( pHold->sSuitOp.emAttack & pItem->sSuitOp.emAttack ) == NULL )
	// 	{
	// 		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_DEFSUIT") );
	// 		return S_FALSE;
	// 	}

	if ( pInvenItem->sItemCustom.nidDISGUISE!=SNATIVEID(false) )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DISGUISE_FB_ALREADY") );
		return S_FALSE;
	}



	GLMSG::SNET_INVEN_DISGUISE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

HRESULT GLCharacter::ReqCleanser ( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	if ( m_pGaeaClient->GetTradeClient()->Valid() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

	//if ( pHold->sBasicOp.emItemType!=ITEM_CLEANSER )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NOCLEANSER") );
	//	return S_FALSE;
	//}

	if ( pInvenItem->sItemCustom.nidDISGUISE==SNATIVEID(false) )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_CLEANSER_FB_NONEED") );
		return S_FALSE;
	}

	GLMSG::SNET_INVEN_CLEANSER NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

HRESULT GLCharacter::ReqBoxWrapping(WORD wPosX, WORD wPosY)
{
    if ( !IsValidBody() )
        return E_FAIL;
    if ( m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem )
        return E_FAIL;

    if ( !holditem::HaveHoldItem() )
        return E_FAIL;

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
    if ( !pHold )
        return S_FALSE;

    if ( pInvenItem->sItemCustom.IsBoxWrappable() == false )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_NOTALLOWED") );
        return S_FALSE;
    }

    if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_NOTALLOWED") );
        return S_FALSE;
    }

    if ( pInvenItem->sItemCustom.IsBoxWrapped() )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_NOTALLOWED") );
        return S_FALSE;
    }

	if( pInvenItem->sItemCustom.IsTempEnchant() )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXWRAPPING_FB_NOTALLOWED") );
		return S_FALSE;
	}

    GLMSG::SNET_INVEN_BOX_WRAPPING NetMsg;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

    return S_OK;
}

HRESULT GLCharacter::ReqBoxUnwrapping(WORD wPosX, WORD wPosY)
{
    if ( !IsValidBody() )
        return E_FAIL;
    if ( m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;
    //if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    if ( pInvenItem->sItemCustom.IsBoxWrapped() == false )
    {
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXUNWRAPPING_FB_FAIL") );
        return S_FALSE;
    }

    GLMSG::SNET_INVEN_BOX_UNWRAPPING NetMsg;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

HRESULT GLCharacter::ReqCharCard ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

	//if ( pItem->sBasicOp.emItemType!=ITEM_CHARACTER_CARD )	return E_FAIL;

	GLMSG::SNET_INVEN_CHARCARD NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! â�� ī�� ���.
HRESULT GLCharacter::ReqStorageCard ( WORD wPosX, WORD wPosY, WORD wSTORAGE )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )					return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_STORAGE_CARD )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_NOITEM") );
	//	return E_FAIL;
	//}

	if ( wSTORAGE < 1 || wSTORAGE>=(EMSTORAGE_CHANNEL-1) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGECARD_FB_INVNUM") );
		return E_FAIL;
	}

	GLMSG::SNET_INVEN_STORAGECARD NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wSTORAGE = wSTORAGE;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqInvenLineCard ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_INVEN_CARD )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_NOITEM") );
	//	return E_FAIL;
	//}

	if ( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		if ( m_wINVENLINE >= GLCONST_CHAR::wInvenExtendLine )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_MAXLINE") );
			return E_FAIL;
		}
	}
	else
	{
		if ( m_wINVENLINE >= EInvenY - GLCONST_CHAR::wInvenDefaultLine - EInvenPremiumLine )
		{
			m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_INVENLINE_FB_MAXLINE") );
			return E_FAIL;
		}
	}

	GLMSG::SNET_INVEN_INVENLINE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRemodelOpenCard ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )					return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SINVENITEM* pFirstInvenItem = m_cInventory.FindItem( pInvenItem->GetNativeID() );
	if( NULL == pFirstInvenItem )
		return E_FAIL;

	wPosX = pFirstInvenItem->wPosX;
	wPosY = pFirstInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pFirstInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	GLMSG::SNET_INVEN_REMODELOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		if (!m_sStampItem.VALID())
		{
			WORD wStampCardPosition[2] = {USHRT_MAX, USHRT_MAX};
			GetItemPosInInven( ITEM_STAMP_CARD, wStampCardPosition[0], wStampCardPosition[1] );
			NetMsg.wStampCardPosX = wStampCardPosition[0];
			NetMsg.wStampCardPosY = wStampCardPosition[1];
		}
		else
		{
			NetMsg.wStampCardPosX = m_sStampItem.wPosX;
			NetMsg.wStampCardPosY = m_sStampItem.wPosY;
		}
	}	

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqGabargeOpenCard ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_REMODELOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	m_ItemGarbagePosX = wPosX;
	m_ItemGarbagePosY = wPosY;

	GLMSG::SNET_INVEN_GARBAGEOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}


HRESULT GLCharacter::ReqStorageOpenCard ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	// â�� ���������� â�� ����ī�带 ����� �� ����.
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >( 
		NSWIDGET_SCRIPT::g_strFunc_Locker_IsOpen ) )
        return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if (!pInvenItem)
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_STORAGE_CONNECT )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_STORAGEOPEN_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLMSG::SNET_INVEN_STORAGEOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

bool FindStorageOpenNpc( GLGaeaClient* pGaeaClient, GLCharacter* pChar, DWORD& nNpcID, DWORD& nTalkID )
{
	GLLandManClient* pLand = pGaeaClient->GetActiveMap();
	if ( !pLand )
		return false;
	
	const DWORD nPartyMember = pGaeaClient->GetMyPartyClient()->GetNMember();

	return pLand->FindStorageOpenNpc(pChar->GetPosition(), pChar, nPartyMember, nNpcID, nTalkID);
}

bool FindStorageConnectItem( GLInventory& cInventory, WORD& nPosX, WORD& nPosY )
{
	const GLInventory::CELL_MAP* pMap = cInventory.GetItemList();
	if ( !pMap )
		return false;

	nPosX = USHRT_MAX;
	nPosY = USHRT_MAX;

	BOOST_FOREACH( const GLInventory::CELL_MAP::value_type& it, *pMap )
	{
        const SINVENITEM* pInvenItem = it.second;

		SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->Id() );
		if ( !pItem )
			continue;

		if ( pItem->BasicType() != ITEM_STORAGE_CONNECT )
			continue;

		nPosX = pInvenItem->wPosX;
		nPosY = pInvenItem->wPosY;

		// ���Ѻ� �����ۺ��� ����ϵ��� ����
		//if ( pItem->IsTIMELMT() )
		if ( pInvenItem->sItemCustom.IsTimeLimit() )
			return true;
	}

	if ( nPosX != USHRT_MAX && nPosY != USHRT_MAX )
		return true;

	return false;
}

bool GLCharacter::ReqStorageOpenFromInven()
{
	DWORD nNpcID = GAEAID_NULL;
	DWORD nTalkID = 0;

	bool bFindNpc = FindStorageOpenNpc( m_pGaeaClient, this, nNpcID, nTalkID );
	if ( bFindNpc )
	{
		if( E_FAIL == ReqNpcTalkBasic( nNpcID, SNpcTalk::EM_STORAGE, nTalkID ) )
			return false;

		return true;
	}
	else
	{
		WORD nPosX = 0;
		WORD nPosY = 0;

		if ( FindStorageConnectItem( m_cInventory, nPosX, nPosY ) )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_Locker_CanUseOpenCard,
				"-n, -n", nPosX, nPosY );

			return true;
		}
	}

	return false;
}

HRESULT GLCharacter::ReqStorageCloseCard()
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;

    m_bUseStorageCard = false;

	GLMSG::SNET_INVEN_STORAGECLOSE NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketSearchOpen(WORD wPosX, WORD wPosY)
{
    //�̹� ���λ��� �˻�â�� ���������� ���
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarketSearch_IsOpen ) )
    {
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_ALREADY) );
        return E_FAIL;
    }

    //���λ��� �˻����� ����� ���ѵ� ���ΰ�?
    private_market::GLPrivateMarketDefine* pPrivateMarketDefine = private_market::GLPrivateMarketDefine::Instance();
    if ( !pPrivateMarketDefine || !pPrivateMarketDefine->CanPurchase( m_pGaeaClient->GetActiveMapID().getGaeaMapID().dwID ) )
    {
        m_pGaeaClient->PrivateMarketError( private_market::eNotPurchaseMap );
        return false;
    }

    //������ ��ȿ�� �˻�
    const SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    const SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem )
        return E_FAIL;
    if ( pItem->sBasicOp.emItemType != ITEM_PMARKET_SEARCH_CARD )
    {
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_PMARKET_SEARCH_CARD_FB", EMREQ_PMARKET_SEARCH_OPEN_FB_FAIL) );
        return E_FAIL;
    }
    
    //���λ��� �˻� ī�� ��� ��û
    GLMSG::SNET_INVEN_PMARKET_SEARCH_OPEN_CF NetMsg;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

HRESULT GLCharacter::ReqPMarketSearchClose()
{
    GLMSG::SNET_INVEN_PMARKET_SEARCH_CLOSE NetMsg;
    m_pGaeaClient->NETSENDTOFIELD( &NetMsg );

    return S_OK;
}

HRESULT GLCharacter::ReqPremiumSet(WORD wPosX, WORD wPosY)
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	const SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if (!pInvenItem)
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem(pInvenItem->sItemCustom.GetNativeID());
	if (!pItem)
	{
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::DISABLE,
            ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOITEM"));
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_PREMIUMSET )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_PREMIUMSET_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLMSG::SNET_INVEN_PREMIUMSET NetMsg(wPosX, wPosY);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	return S_OK;
}

HRESULT GLCharacter::ReqInvenHairChange ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_HAIR )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLMSG::SNETPC_INVEN_HAIR_CHANGE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: �Ӹ� ��Ÿ�� ����
// *****************************************************
HRESULT GLCharacter::ReqInvenHairStyleChange ( WORD wHairStyle )
{
	GLMSG::SNETPC_INVEN_HAIRSTYLE_CHANGE NetMsg;
	NetMsg.wPosX	  = m_wwInvenPos_HairStyle.wA;
	NetMsg.wPosY	  = m_wwInvenPos_HairStyle.wB;
	NetMsg.wHairStyle = wHairStyle;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	//m_wInvenPosX1 = 0;
	//m_wInvenPosY1 = 0;
	m_wwInvenPos_HairStyle.Init();

	return S_OK;
}

// *****************************************************
// Desc: �Ӹ� ��Ÿ�� ����
// *****************************************************
HRESULT GLCharacter::InvenHairStyleChange ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_HAIR_STYLE )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StyleHairUse );

	//m_wInvenPosX1 = wPosX;
	//m_wInvenPosY1 = wPosY;
	m_wwInvenPos_HairStyle = WORDWORD( wPosX, wPosY );

	return S_OK;
}

// *****************************************************
// Desc: �Ӹ� ���� ����
// *****************************************************
HRESULT GLCharacter::ReqInvenHairColorChange ( WORD wHairColor )
{
	GLMSG::SNETPC_INVEN_HAIRCOLOR_CHANGE NetMsg;
	NetMsg.wPosX	  = m_wwInvenPos_HairColor.wA;
	NetMsg.wPosY	  = m_wwInvenPos_HairColor.wB;
	NetMsg.wHairColor = wHairColor;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	//m_wInvenPosX2 = 0;
	//m_wInvenPosY2 = 0;
	m_wwInvenPos_HairColor.Init();

	return S_OK;
}

// *****************************************************
// Desc: �Ӹ� ���� ����
// *****************************************************
HRESULT GLCharacter::InvenHairColorChange ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_HAIR_COLOR )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_HAIR_CHANGE_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_UseHairColorCard );

	//m_wInvenPosX2 = wPosX;
	//m_wInvenPosY2 = wPosY;
	m_wwInvenPos_HairColor = WORDWORD( wPosX, wPosY );

	return S_OK;
}

//! �ڽ�Ƭ ���� ����. by luxes.
HRESULT GLCharacter::ReqInvenCostumColorChange( DWORD dwMainColor, DWORD dwSubColor )
{
	GLMSG::SNETPC_INVEN_COSTUM_COLOR_CHANGE NetMsg;

	NetMsg.dwMainColor	= dwMainColor;
	NetMsg.dwSubColor	= dwSubColor;

	NetMsg.emSlot		= m_emCostumSelectSlot;

    NetMsg.wPosX		= holditem::GetHoldPosX();
    NetMsg.wPosY		= holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wInvenPosX2 = 0;
	m_wInvenPosY2 = 0;

	return S_OK;
}

HRESULT GLCharacter::InvenCostumColorChange( EMSLOT emSlot, bool bRestrict /* = true */ )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SITEM* pSlotItem = GET_SLOT_ITEMDATA( emSlot );
	if ( !pSlotItem )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	//if ( !pSlotItem->sBasicOp.IsDISGUISE() )
	if ( !( m_PutOnItems[ emSlot ].IsDisguise() != NULL ) )
	{
		if ( m_PutOnItems[ emSlot ].nidDISGUISE != NATIVEID_NULL() )
		{
			SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emSlot ].nidDISGUISE );

			if ( pDisguiseItem )
			{
				//if ( !pDisguiseItem->sBasicOp.IsDISGUISE() )
				if ( !( m_PutOnItems[ emSlot ].IsDisguise() != NULL ) )
				{
                    holditem::ReleaseHoldItem();

					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSLOT_COTUM_CHANGE_FB_CAN_NOT_USE_ITEM") );

					return E_FAIL;
				}
			}
		}
		else
		{
            holditem::ReleaseHoldItem();
			//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSLOT_COTUM_CHANGE_FB_CAN_NOT_USE_ITEM") );

			return E_FAIL;
		}
	}

	/*if ( pSlotItem->sSuitOp.emSuit != SUIT_HEADGEAR && pSlotItem->sSuitOp.emSuit != SUIT_UPPER
		&& pSlotItem->sSuitOp.emSuit != SUIT_LOWER && pSlotItem->sSuitOp.emSuit != SUIT_FOOT )*/
	if ( !pSlotItem->IsColorChange() )
	{
		if ( m_PutOnItems[ emSlot ].nidDISGUISE != NATIVEID_NULL() )
		{
			SITEM* pDisguiseItem = GLogicData::GetInstance().GetItem( m_PutOnItems[ emSlot ].nidDISGUISE );

			if ( pDisguiseItem )
			{
				if ( !pDisguiseItem->IsColorChange() )
				{
                    holditem::ReleaseHoldItem();
					//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
					m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSLOT_COTUM_CHANGE_FB_CAN_NOT_USE_ITEM") );

					return E_FAIL;
				}
			}
		}
		else
		{
            holditem::ReleaseHoldItem();
			//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSLOT_COTUM_CHANGE_FB_CAN_NOT_USE_ITEM") );

			return E_FAIL;
		}
	}

	PDXCHARPART pCharPart = m_pSkinChar->GetPiece( SLOT_2_PIECE( emSlot ) );
	if ( pCharPart )
	{	// Memo : overlay ȿ���� �ȵ������� ���� �� �� ����. by luxes.
		DWORD dwMainColor = 0, dwSubColor = 0;
		pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

		if ( dwMainColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) ||
			dwSubColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
		{
            holditem::ReleaseHoldItem();
			//	Memo : �÷� ������ �� �� ���� ������ �Դϴ�. by luxes.
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSLOT_COTUM_CHANGE_FB_CAN_NOT_USE_ITEM") );

			return E_FAIL;
		}
	}

	m_emCostumSelectSlot = emSlot;
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CostumeColor_ItemUse, 
		"-b", bRestrict );

	//! ��� â�� �� �� ��� �ִ� �������� ����.
    if ( holditem::HaveHoldItem() )
	{
        holditem::ReleaseHoldItem();

        // TODO : Hold Item ���ֱ�.

		//if (m_ServiceProvider == SP_VIETNAM)
		//{	
  //          const SITEMCUSTOM& sCustom = holditem::GetHoldItem();

		//	if ( !sCustom.bVietnamGainItem || ( sCustom.bVietnamGainItem && m_dwVietnamInvenCount > 0 ) )	
		//	{
		//		//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//		//
		//		SITEM* pItem = GLogicData::GetInstance().GetItem( sCustom.Id() );
		//		GASSERT(pItem&&"������ �����Ͱ� �������� ����");

		//		WORD wPosX, wPosY;

		//		BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY );
		//		if ( !bOk )
		//		{
  //                  holditem::ReleaseHoldItem();
		//			// ���� ������ ����.
		//			return E_FAIL;
		//		}

		//		bOk = m_cInventory.IsInsertable( wPosX, wPosY );
		//		if ( !bOk )
		//		{
  //                  holditem::ReleaseHoldItem();
		//			//	�ι��� ��������.
		//			return E_FAIL;
		//		}

		//		m_wInvenPosX2 = wPosX;
		//		m_wInvenPosY2 = wPosY;

		//		GLMSG::SNETPC_REQ_INVEN_TO_INVEN NetMsg;
		//		NetMsg.wPosX = wPosX;
		//		NetMsg.wPosY = wPosY;	
		//		NetMsg.bUseVietnamInven = (m_ServiceProvider == SP_VIETNAM ? sCustom.bVietnamGainItem : FALSE);
	
		//		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

  //              holditem::ReleaseHoldItem();
		//	}	
		//	else
		//	{
  //              holditem::ReleaseHoldItem();

		//		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_VIETNAM_ITEMGET_FAILED") );
		//		return E_FAIL;		
		//	}
		//}
		//else
		//{
		//	//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//	//
  //          SITEM* pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
		//	GASSERT(pItem&&"������ �����Ͱ� �������� ����");

		//	WORD wPosX, wPosY;

		//	BOOL bOk = m_cInventory.FindInsertable( wPosX, wPosY );
		//	if ( !bOk )
		//	{
  //              holditem::ReleaseHoldItem();
		//		// ���� ������ ����.
		//		return E_FAIL;
		//	}

		//	bOk = m_cInventory.IsInsertable( wPosX, wPosY );
		//	if ( !bOk )
		//	{
  //              holditem::ReleaseHoldItem();
		//		//	�ι��� ��������.
		//		return E_FAIL;
		//	}

		//	m_wInvenPosX2 = wPosX;
		//	m_wInvenPosY2 = wPosY;

		//	GLMSG::SNETPC_REQ_INVEN_TO_INVEN NetMsg;
		//	NetMsg.wPosX = wPosX;
		//	NetMsg.wPosY = wPosY;
		//	NetMsg.bUseVietnamInven = FALSE;

		//	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

  //          holditem::ReleaseHoldItem();
		//}
	}

	return S_OK;
}

//! Ż�� ���� ����.
HRESULT GLCharacter::ReqInvenVehicleColorChange ( DWORD dwMainColor[], DWORD dwSubColor[] )
{
	GLMSG::SNETPC_INVEN_VEHICLE_COLOR_CHANGE NetMsg;
	for( INT i = 0; i < ACCE_TYPE_SIZE; i++ )
	{
		NetMsg.dwMainColor[ i ] = dwMainColor[ i ];
		NetMsg.dwSubColor[ i ] = dwSubColor[ i ];
	}
	NetMsg.wPosX	  = m_wInvenPosX2;
	NetMsg.wPosY	  = m_wInvenPosY2;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wInvenPosX2 = 0;
	m_wInvenPosY2 = 0;

	return S_OK;
}

HRESULT GLCharacter::InvenVehicleColorChange ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	if ( m_emVehicle != EMVEHICLE_DRIVER ) 
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_BIKE_CHANGE_FB_NOT_DRIVER") );

		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	SITEM* pVehicle = GET_SLOT_ITEMDATA ( SLOT_VEHICLE );	
	if ( !pVehicle )
		return E_FAIL;

	std::string strText = "";
	const VEHICLE_TYPE& emType = pVehicle->VehicleType();

	if ( emType != pItem->sVehicle.emVehicleType )
	{
		strText = sc::string::format( ID2GAMEINTEXT("EMINVEN_VEHICLE_CHANGE_FB_NOT_VEHICLE") , COMMENT::VEHICLE_TYPE[pItem->sVehicle.emVehicleType].c_str() );
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, strText );
		return E_FAIL;
	}

	if ( pVehicle->IsColorChange() == false )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_BIKECOLOR_CHANGE_FB_BADITEM") );
		return E_FAIL;		
	}

	if( VEHICLE_TYPE_NONE == emType )
		strText = ID2GAMEWORD( "VEHICLE_TEXT_STATIC", 3 );
	else
		strText = COMMENT::VEHICLE_TYPE[ emType ];
	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_VehicleColor_ItemUse,
		"-s", strText.c_str() );

	m_wInvenPosX2 = wPosX;
	m_wInvenPosY2 = wPosY;

	return S_OK;
}

//! �󱼽�Ÿ�� ����.
HRESULT GLCharacter::ReqInvenFaceChange ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_FACE )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_NOITEM") );
	//	return E_FAIL;
	//}

	GLMSG::SNETPC_INVEN_FACE_CHANGE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: �� ��Ÿ�� ����
// *****************************************************
HRESULT GLCharacter::ReqInvenFaceStyleChange ( WORD wFaceStyle )
{
	GLMSG::SNETPC_INVEN_FACESTYLE_CHANGE NetMsg;
	NetMsg.wPosX	  = m_wwInvenPos_FaceStyle.wA;
	NetMsg.wPosY	  = m_wwInvenPos_FaceStyle.wB;
	NetMsg.wFaceStyle = wFaceStyle;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	//m_wInvenPosX1 = 0;
	//m_wInvenPosY1 = 0;
	m_wwInvenPos_FaceStyle.Init();

	return S_OK;
}

// *****************************************************
// Desc: �� ��Ÿ�� ����
// *****************************************************
HRESULT GLCharacter::InvenFaceStyleChange ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_FACE_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StyleFaceUse );

	//m_wInvenPosX1 = wPosX;
	//m_wInvenPosY1 = wPosY;	
	m_wwInvenPos_FaceStyle = WORDWORD( wPosX, wPosY );

	return S_OK;
}

HRESULT GLCharacter::InvenGenderChange ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_GENDER_CHANGE )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_ITEMTYPE") );
		return E_FAIL;
	}

/*
	//	�ذ��� ���� ���� �ǰ� Ǯ�����ϴ�. 
	if ( m_emClass == GLCC_EXTREME_M || m_emClass == GLCC_EXTREME_W )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_GENDER_CHANGE_FB_BADCLASS") );
		return E_FAIL;
	}
*/

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StyleGenderUse );

	m_wInvenPosX1 = wPosX;
	m_wInvenPosY1 = wPosY;

	return S_OK;
}

HRESULT GLCharacter::ReqInvenGenderChange ( WORD wFace, WORD wHair )
{
	GLMSG::SNETPC_INVEN_GENDER_CHANGE NetMsg;
	NetMsg.wPosX = m_wInvenPosX1;
	NetMsg.wPosY = m_wInvenPosY1;
	NetMsg.wFace = wFace;	
	NetMsg.wHair = wHair;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wInvenPosX1 = 0;
	m_wInvenPosY1 = 0;

	return S_OK;
}

HRESULT GLCharacter::ReqInvenSchoolChange ( WORD wPosX, WORD wPosY )
{
	if( GetClubID() != CLUB_NULL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_CLUB_ERROR") );
		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_NOITEM") );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType == ITEM_SCHOOL_CHANGE_0 || pItem->sBasicOp.emItemType == ITEM_SCHOOL_CHANGE_1 || pItem->sBasicOp.emItemType == ITEM_SCHOOL_CHANGE_2 )
	{
		GLMSG::SNETPC_INVEN_SCHOOL_CHANGE NetMsg;
		NetMsg.wPosX = wPosX;
		NetMsg.wPosY = wPosY;

		switch(  pItem->sBasicOp.emItemType )
		{
		case ITEM_SCHOOL_CHANGE_0:
			NetMsg.wSchool = 0;
			break;
		case ITEM_SCHOOL_CHANGE_1:
			NetMsg.wSchool = 1;
			break;
		case ITEM_SCHOOL_CHANGE_2:
			NetMsg.wSchool = 2;
			break;
		default:
			NetMsg.wSchool = 65535;
			break;
		}
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;
	}
	else
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_SCHOOL_CHANGE_FB_ITEMTYPE") );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenSchoolChange ( WORD wSchool )
{
	GLMSG::SNETPC_INVEN_SCHOOL_CHANGE NetMsg;
	NetMsg.wPosX = m_wInvenPosX1;
	NetMsg.wPosY = m_wInvenPosY1;
	NetMsg.wSchool = wSchool;	
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wInvenPosX1 = 0;
	m_wInvenPosY1 = 0;

	return S_OK;
}


HRESULT GLCharacter::InvenRename ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )						return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )	
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_NOITEM") );
		return E_FAIL;
	}

	//if ( pItem->sBasicOp.emItemType!=ITEM_RENAME )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_BADITEM") );
	//	return E_FAIL;
	//}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CharRename );

	m_wInvenPosX1 = wPosX;
	m_wInvenPosY1 = wPosY;

	return S_OK;
}

HRESULT GLCharacter::ReqInvenRename ( const char* szCharName )
{
	if ( !szCharName )
		return S_FALSE;

	CString strTEMP( szCharName );

	// �±��� ���� ���� üũ
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
        if ( !STRUTIL::CheckThaiString(strTEMP) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_THAICHAR_ERROR"));
		    return S_FALSE;
	    }   
    }

	// ��Ʈ�� ���� ���� üũ 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if ( STRUTIL::CheckVietnamString( strTEMP ) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMINVEN_RENAME_FB_VNCHAR_ERROR"));
		    return S_FALSE;
	    }
    }


	BOOL bFILTER0 = STRUTIL::CheckString( strTEMP );
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strTEMP );
	if ( bFILTER0 || bFILTER1 )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CHARACTER_BADNAME") );
		return S_FALSE;
	}

    if (m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST)
    {
        if (!sc::string::goodChaNameKorea(std::string(strTEMP.GetString())))
        {
            m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEEXTEXT("CREATE_CHARACTER_NAME_ERROR_KOREA"));
            return FALSE;
        }
    }

	GLMSG::SNETPC_INVEN_RENAME NetMsg;
	NetMsg.wPosX = m_wInvenPosX1;
	NetMsg.wPosY = m_wInvenPosY1;
	StringCchCopy ( NetMsg.szName, CHAR_SZNAME, szCharName );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wInvenPosX1 = 0;
	m_wInvenPosY1 = 0;
	return S_OK;
}

//! ��ų ���� ����.
HRESULT GLCharacter::ResetSkillStats ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	m_wInvenPosX1 = wPosX;
	m_wInvenPosY1 = wPosY;
	
	RequestOpenResetSkillStats( wPosX, wPosY );

	return S_OK;
}

HRESULT GLCharacter::RequestOpenResetSkillStats( const WORD wPosX, const WORD wPosY  )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_NOITEM") );
		return E_FAIL;
	}

	lua_tinker::table tbData( GLWidgetScript::GetInstance().GetLuaState() );
	lua_tinker::table tbEvenlyStats( GLWidgetScript::GetInstance().GetLuaState() );
	lua_tinker::table tbReturnItem( GLWidgetScript::GetInstance().GetLuaState() );

	tbEvenlyStats.set( "pow", 0 );
	tbEvenlyStats.set( "str", 0 );
	tbEvenlyStats.set( "spi", 0 );
	tbEvenlyStats.set( "dex", 0 );
	tbEvenlyStats.set( "sta", 0 );

	CString strInfo;
	bool bSelectable = false;
	DWORD dwGiftSkill = 0;
	WORD wReturnSkillPoint = 0;	
	WORD wReturnStats = pItem->sDrugOp.dwCureVolume;	
	if ( wReturnStats )
	{
		switch ( pItem->sBasicOp.emItemType )
		{
			// �� ������ ��
		case ITEM_SKP_STAT_RESET_A:
			// Note : ��ü ���� �ʱ�ȭ�� ���.
			if ( wReturnStats == USHRT_MAX )
			{
				strInfo.Format(
					ID2GAMEINTEXT( "STATSRESET_INFO_HEAD_TEXT_ALLSTATS" ),
					ID2GAMEWORD( "CHARACTER_SLOT_NAME", GET_CURRENT_CHARACTER_SLOT() ) );

				wReturnStats = m_sStats.wPow + m_sStats.wDex + m_sStats.wSpi + m_sStats.wStr + m_sStats.wSta;
			}
			else
			{
				strInfo.Format(
					ID2GAMEINTEXT( "STATSRESET_INFO_HEAD_TEXT_NONSELECT" ),
					ID2GAMEWORD( "CHARACTER_SLOT_NAME", GET_CURRENT_CHARACTER_SLOT() ),
					wReturnStats );
			}
			WORD wEvenlyReset[EMSIZE];
			memset( wEvenlyReset, 0, sizeof(WORD)*EMSIZE);
			GET_EVENLY_DECREASE_STATS( wEvenlyReset, wReturnStats );
			tbEvenlyStats.set( "pow", wEvenlyReset[EMPOW] );
			tbEvenlyStats.set( "str", wEvenlyReset[EMSTR] );
			tbEvenlyStats.set( "spi", wEvenlyReset[EMSPI] );
			tbEvenlyStats.set( "dex", wEvenlyReset[EMDEX] );
			tbEvenlyStats.set( "sta", wEvenlyReset[EMSTA] );
			break;

			// ������ �ž� (���� ���� ����)
		case ITEM_SKP_STAT_RESET_B:
			{
				strInfo.Format(
					ID2GAMEINTEXT( "STATSRESET_INFO_HEAD_TEXT_SELECTABLE" ),
					ID2GAMEWORD( "CHARACTER_SLOT_NAME", GET_CURRENT_CHARACTER_SLOT() ),
					wReturnStats );
				bSelectable = true;
			}
			break;

		default:
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RESET_SKST_FB_NOITEM") );
			return E_FAIL;
		}
	}
	else
	{
		strInfo.Format(
			ID2GAMEINTEXT( "STATSRESET_INFO_HEAD_TEXT_SKILLONLY" ),
			ID2GAMEWORD( "CHARACTER_SLOT_NAME", GET_CURRENT_CHARACTER_SLOT() ) );
	}

	//	Note : ��ų ����Ʈ �ʱ�ȭ.
	//	Note : ����Ʈ���� ȹ���� ��ų ����Ʈ.
	GLQuestPlay::MAPQUEST&     sQuestEnd = m_cQuestPlay.GetQuestEnd();
	GLQuestPlay::MAPQUEST_ITER iter_quest       = sQuestEnd.begin();
	GLQuestPlay::MAPQUEST_ITER iter_quest_end       = sQuestEnd.end();
	for ( ; iter_quest!=iter_quest_end; ++iter_quest )
	{
		GLQUESTPROG *pPROG = (*iter_quest).second;
		GLQUEST *pQUEST = GLQuestMan::GetInstance().Find( pPROG->m_sNID.dwID );
		if ( !pQUEST )
			continue;

		dwGiftSkill += pQUEST->m_dwGiftSKILLPOINT;
	}

	EMCHARINDEX emINDEX = CharClassToIndex(m_emClass);
	const SCHARDATA2& sCHARDATA = GLCONST_CHAR::GET_CHAR_DATA2(m_wSchool,emINDEX);
	if( emINDEX == GLCI_EXTREME_M || emINDEX == GLCI_EXTREME_W )
	{
		wReturnSkillPoint = (WORD)(sCHARDATA.m_dwSkillPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_2ndSKILL_P + dwGiftSkill);
	}
	else
	{
		wReturnSkillPoint = (WORD)(sCHARDATA.m_dwSkillPoint + (m_wLevel-sCHARDATA.m_wLevel)*GLCONST_CHAR::wLVL_SKILL_P + dwGiftSkill);
	}

	wReturnSkillPoint -= static_cast<WORD>(m_dwSkillPoint);

	// Note : ��ȯ ������

	WORD wReturnItem(0);
	WORD wGRADE[UI::EMSTATSRESETWINDOW_SKILL_GRADE];
	memset(wGRADE,0,sizeof(WORD)*UI::EMSTATSRESETWINDOW_SKILL_GRADE);

	SKILL_MAP_ITER iter_skill = m_ExpSkills.begin();
	SKILL_MAP_ITER iter_skill_end = m_ExpSkills.end();

	SCHARSKILL sTempSkill;

	for ( ; iter_skill!=iter_skill_end; ++iter_skill )
	{
		//SNATIVEID sNID ( (*iter_skill).first );

		CHANGE_CHARSKILL( (*iter_skill).second, sTempSkill );

		SNATIVEID sNID ( sTempSkill.sNativeID.dwID );

		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData( sNID );
		if ( !pSKILL )									continue;
		if ( UI::EMSTATSRESETWINDOW_SKILL_GRADE_MIN > pSKILL->m_sBASIC.dwGRADE )	continue;

		wGRADE[pSKILL->m_sBASIC.dwGRADE] += 1;
		++wReturnItem;
	}

	CString strTemp;
	int nTableIndex = 1;

	for( WORD i = 0; i < UI::EMSTATSRESETWINDOW_SKILL_GRADE; ++i )
	{
		if( 0 == wGRADE[ i ] )
			continue;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( 
			SNATIVEID( UI::EMSTATSRESETWINDOW_SKILL_MARBLE_MID, i ) );
		if( NULL == pItem )
			continue;

		strTemp.Format( "%d%s", wGRADE[ i ], ID2GAMEWORD( "STATSRESET_RETURN_ITEM_TEXT_NUM" ) );

		lua_tinker::table tbItem( GLWidgetScript::GetInstance().GetLuaState() );
		tbItem.set( 1, pItem->GetName() );
		tbItem.set( 2, strTemp.GetBuffer() );
		tbReturnItem.set( nTableIndex++, tbItem );
	}

	tbData.set( "emItemType", pItem->sBasicOp.emItemType );
	tbData.set( "dwOldDrugStatsCount", pItem->sDrugOp.dwCureVolume );
	tbData.set( "strTopText", strInfo.GetString() );
	tbData.set( "bSelectable", bSelectable );
	tbData.set( "wReturnStats", wReturnStats );
	tbData.set( "wStatsPoint", m_wStatsPoint );
	tbData.set( "wReturnSkillPoint", wReturnSkillPoint );
	tbData.set( "wReturnItem", wReturnItem );

	GLWidgetCallFuncValue args[ 3 ];
	args[ 0 ].SetTable( &tbData );
	args[ 1 ].SetTable( &tbEvenlyStats );
	args[ 2 ].SetTable( &tbReturnItem );	

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_StatsReset_Open, args, 3 );

	return S_OK;
}

HRESULT GLCharacter::RequestResetSkillStats( lua_tinker::table tbStats, const WORD wMaxAmount, const EMITEM_TYPE emItemType )
{
	GLMSG::SNET_INVEN_RESET_SKST NetMsg;
	NetMsg.wPosX      = m_wInvenPosX1;
	NetMsg.wPosY      = m_wInvenPosY1;
	NetMsg.wAmount    = wMaxAmount;
	NetMsg.emItemType = emItemType;

	NetMsg.wResetStats[EMPOW] = tbStats.get< int >("pow");
	NetMsg.wResetStats[EMSTR] = tbStats.get< int >("str");
	NetMsg.wResetStats[EMSPI] = tbStats.get< int >("spi");
	NetMsg.wResetStats[EMDEX] = tbStats.get< int >("dex");
	NetMsg.wResetStats[EMSTA] = tbStats.get< int >("sta");

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqResetSkillStats ( WORD* pStats, const WORD wMaxAmount, const EMITEM_TYPE emItemType )
{
	GLMSG::SNET_INVEN_RESET_SKST NetMsg;
	NetMsg.wPosX      = m_wInvenPosX1;
	NetMsg.wPosY      = m_wInvenPosY1;
	NetMsg.wAmount    = wMaxAmount;
	NetMsg.emItemType = emItemType;

	if ( pStats )
	{
		memcpy(NetMsg.wResetStats, pStats, sizeof(WORD)*EMSIZE);
	}

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::RequestResetSkillStatsBySkillUI()
{
	GLMSG::SNET_SKILL_RESET_CF NetMsg;
	m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
	return S_OK;
}

HRESULT GLCharacter::ReqPostBoxOpenCard ( WORD wPosX, WORD wPosY )
{
	if (!IsValidBody())
        return S_FALSE;

	// �� �� �ִ��� �˻�
	if( false == m_pGaeaClient->GetPostClient()->IsWidgetOpenAble( true ) )
		return S_FALSE;
	
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	m_wInvenPosX1 = wPosX = pInvenItem->wPosX;
	m_wInvenPosY1 = wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_POSTBOX_CARD )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_POSTBOX_OPEN_CARD_FB_NOITEM") );
		return E_FAIL;
	}

	GLMSG::SNETPC_INVEN_POSTBOX_OPEN_CARD NetMsg;
	NetMsg.wPosX = m_wInvenPosX1;
	NetMsg.wPosY = m_wInvenPosY1;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	/*
	// ĳ���� â�� ���������� ���ش�.
	if ( m_pInterface->UiIsVisibleGroup( CHARACTER_WINDOW ) ) m_pInterface->UiHideGroup( CHARACTER_WINDOW );
	// ��ų â�� ���������� ���ش�.
	if ( m_pInterface->UiIsVisibleGroup( SKILL_WINDOW ) )     m_pInterface->UiHideGroup( SKILL_WINDOW );
	*/


	return S_OK;
}

HRESULT GLCharacter::ReqDialogueWindowOpen(const SNATIVEID& sNPCID, const DWORD dwGaeaID, const EMCROW emCrow )
{
	if ( !IsValidBody() )
		return S_FALSE;
	//!! SF_TODO
	//if ( m_pInterface->UiIsVisibleGroup(DIALOGUE_WINDOW) )
	//	return S_FALSE;

	m_NpcLastClick = dwGaeaID;

	GLMSG::SNETPC_DIALOGUEWINDOW_OPEN NetMsg;
	NetMsg.sNPCID   = sNPCID;
	NetMsg.dwGaeaID = dwGaeaID;
	NetMsg.emCrow   = emCrow;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPostBoxOpen ( const SNATIVEID& sTargetID )
{
	if ( !IsValidBody() )
        return S_FALSE;
	
	// �� �� �ִ��� �˻�
	if( false == m_pGaeaClient->GetPostClient()->IsWidgetOpenAble( true ) )
		return S_FALSE;

	if ( !IsPOSTBOXUSEABLE( EMPOSTBOXID_TYPE_NPC, sTargetID ) )
		return S_FALSE;

	GLMSG::SNETPC_POSTBOX_OPEN NetMsg;
	NetMsg.sPostBoxID = sTargetID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPostBoxClose ()
{
	if( false == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::CF_PostBox_IsOpen ) )
	{
		return S_FALSE;
	}

	m_sPOSTBOXID	  = NATIVEID_NULL();
	m_emPOSTBOXIDTYPE = EMPOSTBOXID_TYPE_NONE;

	GLMSG::SNETPC_POSTBOX_CLOSE NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! ��Ʈ�� ������, ����ġ ȹ�� ī�� ���
HRESULT GLCharacter::ReqInvenVietnamGet ( WORD wPosX, WORD wPosY, bool bGetExp )
{
//	if ( m_dwVietnamGainType == GAINTYPE_EMPTY ) return E_FAIL;
//	if ( m_dwVietnamGainType == GAINTYPE_HALF ) return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		if ( bGetExp )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_VIETNAM_EXPGET_FB_NOITEM") );
		}
        else
        {
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_VIETNAM_ITEMGET_FB_NOITEM") );
		}
		return E_FAIL;
	}

	GLMSG::SNETPC_INVEN_VIETNAM_INVENGET NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.bGetExp = bGetExp;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! �κ��丮 ������ ����Ҷ� ( ���ñ�, ��ų���� �� ).
ESlotHold GLCharacter::ReqInvenDrug(WORD wPosX, WORD wPosY)
{
	if (!IsValidBody())
		return ESlotHold::Release;

	if (ValidWindowOpen())
        return ESlotHold::Release;

	if ( IsProducting() == true )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::RED,
			ID2GAMEINTEXT( "INVEN_LOCK_MSG" ) );
		return ESlotHold::Release;
	}

	const SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if (!pInvenItem)
        return ESlotHold::Release;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetRealNativeID() );	
	if (!pItem)
        return ESlotHold::Release;
	
    SITEM* pHold = NULL;

    if ( holditem::IsHoldPlace( holditem::INVENTORY) )
        pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );

    if (pHold) // �������� ��� Ŭ��
    {
        // �ڽ�Ƭ�� ��� ���� ��� �ڽ�Ƭ ���� �õ�;
        // �ڽ�Ƭ�� ���� �������ǰ� ��Ÿ���� ���� �ʴ´�;
        if ( holditem::GetHoldItem().IsDisguise() != NULL )
        {
            ReqDisguise(wPosX, wPosY);
            return ESlotHold::Release;
        }

        // ���� ���� Ȯ��
        if (!SIMPLE_CHECK_ITEM( holditem::GetHoldItem() ))
            return ESlotHold::Release;

        if ( CheckCoolTime( pHold->sBasicOp.sNativeID ) )
            return ESlotHold::Release;

        return ReqInvenDrugHoldItem(pHold, pItem, wPosX, wPosY);
    }
    else
    {
        // ���� ���� Ȯ��
        if (!SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ))
            return ESlotHold::Release;

        if (CheckCoolTime( pItem->sBasicOp.sNativeID ))
            return ESlotHold::Release;

        ReqInvenDrugNotHoldItem(pItem, wPosX, wPosY, pInvenItem);
    }

	return ESlotHold::Release;
}

EMITEM_TYPE GLCharacter::GetInvenItemType( WORD wPosX, WORD wPosY )
{
    const SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if (!pInvenItem)
        return ITEM_NSIZE;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if ( pItem == NULL )
        return ITEM_NSIZE;

    return pItem->BasicType();
}

// 2015. 02. 11
// ������ �κ� �������� �ִ� 180���������� BYTE�� ��������� �κ��� 5���� �����ʿ� ���� �������� �ִ� 300���� �þ�� ���� WORD�� ����
// �̿� ���� ���۰����� ������ ������(1024)�� �ʰ��Կ� ���� ��Ŷ�� �����ؼ� �����°����� ����
HRESULT GLCharacter::ReqInvenItemSort()
{
    if ( !IsEnalbeItemSort() )
        return E_FAIL;

    if ( !IsValidBody() )
        return E_FAIL;

    if ( IsPostBoxWindowOpen() )
        return E_FAIL;

    if ( m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;

    if ( holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    ResetLimitTimeItemSort();

    GLInventory* pInvenOrigin = NULL;

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		pInvenOrigin = &m_cVietnamInventory;
	}
	else
	{
//#else
		pInvenOrigin = &m_cInventory;
	}
//#endif

    if ( !pInvenOrigin )
        return E_FAIL;

    const GLInventory::CELL_MAP* mapInvenItemOrigin = pInvenOrigin->GetItemList();
    if ( mapInvenItemOrigin->empty() )
        return E_FAIL;

    GLInventory::CELL_MAP mapInvenItemClone;
    mapInvenItemClone.insert( mapInvenItemOrigin->begin(), mapInvenItemOrigin->end() );

    GLInventory cInvenTemp;
    cInvenTemp.SetAddLine( pInvenOrigin->GETAddLine(), pInvenOrigin->IsLimit() );
    cInvenTemp.SetState( pInvenOrigin->GetSizeX(), pInvenOrigin->GetSizeY() );

	// ��Ŷ �����µ� ���� ��Ŷ���δ� ������ ��������� ������ �Ұ����ؼ� ���� ��Ŷ�� Temp �뵵�� ���
    GLMSG::SNETPC_REQ_ITEM_SORT NetMsg;

    while ( !mapInvenItemClone.empty() )
    {
        GLInventory::CELL_MAP_ITER itr = mapInvenItemClone.begin();
        GLInventory::CELL_MAP_ITER endItr = mapInvenItemClone.end();

        GLInventory::CELL_KEY selectKey = itr->first;

        SINVENITEM* pSelectInvenItem = itr->second;
        if ( !pSelectInvenItem )
            return E_FAIL;

        SITEM* pSelectItem = GLogicData::GetInstance().GetItem( pSelectInvenItem->sItemCustom.GetNativeID() );
        if ( !pSelectItem )
            return E_FAIL;

        EMITEM_TYPE emSelectItemType = pSelectItem->sBasicOp.emItemType;
        EMITEMLEVEL emSelectItemLevel = pSelectItem->sBasicOp.emLevel;
        EMSUIT emSelectItemSuit = pSelectItem->sSuitOp.emSuit;

        int iSelItemFirstPriotiry = FIRST_PRIORITY_ITEMSORT[ emSelectItemType ];
        int iSelItemSecondPriority = SECOND_PRIORITY_ITEMSORT[ emSelectItemLevel ];
        int iSelItemAdditionalPriority = ADDTIONAL_PRIORITY_ITEMSORT[ emSelectItemSuit ];

        for ( ++itr; itr != endItr; itr++ )
        {
            GLInventory::CELL_KEY targetKey = itr->first;

            SINVENITEM* pTargetInvenItem = itr->second;
            if ( !pTargetInvenItem )
                return E_FAIL;

            SITEM* pTargetItem = GLogicData::GetInstance().GetItem( pTargetInvenItem->sItemCustom.GetNativeID() );
            if ( !pTargetItem )
                return E_FAIL;

            EMITEM_TYPE emTargetItemType = pTargetItem->sBasicOp.emItemType;
            EMITEMLEVEL emTargetItemLevel = pTargetItem->sBasicOp.emLevel;
            EMSUIT emTargetItemSuit = pTargetItem->sSuitOp.emSuit;

            int iTarItemFirstPriotiry = FIRST_PRIORITY_ITEMSORT[ emTargetItemType ];
            int iTarItemSecondPriority = SECOND_PRIORITY_ITEMSORT[ emTargetItemLevel ];
            int iTarItemAdditionalPriority = ADDTIONAL_PRIORITY_ITEMSORT[ emTargetItemSuit ];

            if ( iTarItemFirstPriotiry > iSelItemFirstPriotiry )
                continue;

            bool bFirstEqual = false;
            bool bAddtionalCheck = false;
            if ( iTarItemFirstPriotiry == iSelItemFirstPriotiry )
            {
                if ( iTarItemFirstPriotiry == FIRST_PRIORITY_ITEMSORT[ ITEM_SUIT ] )
                    bAddtionalCheck = true;
                else
                    bFirstEqual = true;
            }

            if ( bAddtionalCheck )
            {
                if ( iTarItemAdditionalPriority > iSelItemAdditionalPriority )
                    continue;

                if ( iTarItemAdditionalPriority == iSelItemAdditionalPriority )
                    bFirstEqual = true;
            }

            bool bSecondEqual = false;
            if ( bFirstEqual )
            {
                if ( iTarItemSecondPriority > iSelItemSecondPriority )
                    continue;

                if ( iTarItemSecondPriority == iSelItemSecondPriority )
                    bSecondEqual = true;
            }

            if ( bSecondEqual )
            {
                if (  pTargetItem->sBasicOp.sNativeID.dwID >= pSelectItem->sBasicOp.sNativeID.dwID )
                    continue;
            }

            selectKey = targetKey;
            pSelectInvenItem = pTargetInvenItem;
            pSelectItem = pTargetItem;

            emSelectItemType = pSelectItem->sBasicOp.emItemType;
            emSelectItemLevel = pSelectItem->sBasicOp.emLevel;
            emSelectItemSuit = pSelectItem->sSuitOp.emSuit;

            iSelItemFirstPriotiry = FIRST_PRIORITY_ITEMSORT[ emSelectItemType ];
            iSelItemSecondPriority = SECOND_PRIORITY_ITEMSORT[ emSelectItemLevel ];
            iSelItemAdditionalPriority = ADDTIONAL_PRIORITY_ITEMSORT[ emSelectItemSuit ];
        }

        GLInventory::CELL_MAP_ITER findItr = mapInvenItemClone.find( selectKey );
        if ( findItr == mapInvenItemClone.end() )
            return E_FAIL;

        mapInvenItemClone.erase( findItr );

        WORD wTargetPosX(0);
        WORD wTargetPosY(0);

        BOOL bInsertItemOK = cInvenTemp.InsertItemGetPos(
			pSelectInvenItem->sItemCustom,
            wTargetPosX, wTargetPosY );

        if ( !bInsertItemOK )
            return E_FAIL;

        if ( selectKey.first == wTargetPosX && selectKey.second == wTargetPosY )
            continue;

		WORD nOrigin = WORD( selectKey.first + selectKey.second * EInvenX );
		WORD nTarget = WORD( wTargetPosX + wTargetPosY * EInvenX );

        if( NetMsg.ADD_MOVE_ITEM( nOrigin, nTarget ) == false )
			return E_FAIL;
    }

    NetMsg.bHasPile = cInvenTemp.HasPileItem();

    if ( NetMsg.nMoveCount <= 0 && NetMsg.bHasPile == false )
        return E_FAIL;

	GLMSG::SNETPC_REQ_ITEM_SORT_START NetMsgStart;
	m_pGaeaClient->NETSENDTOFIELD( &NetMsgStart );

	int nMsgCount = (NetMsg.nMoveCount / 100) + ((NetMsg.nMoveCount % 100 == 0) ? 0 : 1);
	int nCount = 0;

	for( int i = 0; i < nMsgCount; i++ )
	{
		GLMSG::SNETPC_REQ_ITEM_SORT_DATA NetMsgData;

		nCount = 0;
		NetMsgData.nStartIdx = i*100;
		
		for( int j = 0; j < 100; j++ )
		{
			NetMsgData.sMoveItem[j] = NetMsg.sMoveItem[j + NetMsgData.nStartIdx];
			nCount++;
		}

		NetMsgData.nCurCount = nCount;

		m_pGaeaClient->NETSENDTOFIELD( &NetMsgData );
	}


	GLMSG::SNETPC_REQ_ITEM_SORT_END NetMsgEnd(NetMsg.nMoveCount, NetMsg.bHasPile);
    m_pGaeaClient->NETSENDTOFIELD( &NetMsgEnd );

    SetDisableSendItemSortMsg();

    return S_OK;
}

DWORD GLCharacter::GetAmountActionQ(WORD wSLOT)
{
	if (m_pGaeaClient->GetTradeClient()->Valid())
        return 0;
	if (EMACTIONQUICK_SIZE <= wSLOT)
        return 0;
	
	const SACTION_SLOT &sACTION = m_sACTIONQUICK[wSLOT];
	return m_cInventory.CountTurnItem ( sACTION.sNID );
}

//! ���� �����Կ� �ִ� ���� ����. ( ��ǰ�� ��� ���ñ� ).
void GLCharacter::ReqActionQ( WORD wSLOT )
{	
	if (m_pGaeaClient->GetTradeClient()->Valid())
        return;
	if (EMACTIONQUICK_SIZE <= wSLOT)
        return;
	
	const SACTION_SLOT& sACTION = m_sACTIONQUICK[wSLOT];
	if (!sACTION.VALID())
        return;

    SNATIVEID NativeId = sACTION.GetNativeID();

	switch (sACTION.wACT)
	{
	case EMACT_SLOT_DRUG:
        ReqActionQ_SlotDrug(NativeId);
		break;
    default:
        break;
	};
}

void GLCharacter::ReqActionQ_SlotDrug( SNATIVEID NativeId )
{
    const SINVENITEM* pInvenItem = m_cInventory.FindItem(NativeId);
    if (!pInvenItem)
        return;

    const SITEM* pITEM = GLogicData::GetInstance().GetItem(NativeId);
    if (!pITEM)
        return;

    if (pITEM->sBasicOp.emItemType != ITEM_EFFECT_RELEASE_CURE &&
		pITEM->sBasicOp.emItemType != ITEM_CURE && 
        pITEM->sBasicOp.emItemType != ITEM_TELEPORT_CARD &&		
        pITEM->sBasicOp.emItemType != ITEM_RECALL && 
        pITEM->sBasicOp.emItemType != ITEM_PET_CARD)
    {
        return;
    }

    ReqInvenDrug(pInvenItem->wPosX, pInvenItem->wPosY);
}

//! �κ� ������ ���� ��û
HRESULT GLCharacter::ReqInvenToSlot(WORD wPosX, WORD wPosY)
{
    if ( !IsValidBody() )               
        return S_FALSE;

    if ( ValidWindowOpen() )            
        return S_FALSE;

    if ( m_sCONFTING.IsCONFRONTING() )	
        return S_FALSE;

    //! �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
    //
    if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
        return S_FALSE;

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

    //	�������� �˻�.
    if ( !ACCEPT_ITEM( pInvenItem->sItemCustom ) )
    {
        return S_FALSE;
    }

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

	const SNATIVEID& itemID(pInvenItem->sItemCustom.GetNativeID());
    SITEM* pItem = GLogicData::GetInstance().GetItem( itemID );
    if ( !pItem )
        return E_FAIL;

    EMSLOT emSlot = SLOT_TSIZE;

    // ���� ��ġ ����
    switch ( pItem->sBasicOp.emItemType )
    {
    case ITEM_SUIT:
    case ITEM_INC_GRINDINGRATE:
    case ITEM_ANTI_RESET:
    case ITEM_ANTI_DISAPPEAR:
    case ITEM_REVIVE:
        {
            switch ( pItem->sSuitOp.emSuit )
            {
            case SUIT_HEADGEAR:
				emSlot = SLOT_HEADGEAR;
				break;
            case SUIT_UPPER:
				emSlot = SLOT_UPPER;
				break;
            case SUIT_LOWER:
				emSlot = SLOT_LOWER;
				break;
            case SUIT_HAND:
				emSlot = SLOT_HAND;
				break;
            case SUIT_FOOT:
				emSlot = SLOT_FOOT;
				break;
            case SUIT_NECK:
				emSlot = SLOT_NECK;
				break;
            case SUIT_WRIST:
				emSlot = SLOT_WRIST;
				break;
            case SUIT_FINGER:
                {
                    if ( VALID_SLOT_ITEM(SLOT_LFINGER) && !VALID_SLOT_ITEM(SLOT_RFINGER) )
					{ // �����հ����� ����ִٸ� �����հ����� ����;
						emSlot = SLOT_RFINGER; 
					}
					else
					{ // �׿��� ��쿡�� ��� �޼հ��� ����;
						emSlot = SLOT_LFINGER;
					}
                }
                break;
            case SUIT_WAISTBAND:
				emSlot = SLOT_WAISTBAND;
				break;
            case SUIT_EARRING:
				emSlot = SLOT_EARRINGS;
                break;
			case SUIT_DECORATION:
				emSlot = SLOT_DECORATION;
				break;
            case SUIT_ACCESSORIES:
                {
                    if ( VALID_SLOT_ITEM(SLOT_LACCESSORIES) && !VALID_SLOT_ITEM(SLOT_RACCESSORIES) )
                    { // �����ʸ� ����ִٸ� �����ʿ� ����;
						emSlot = SLOT_RACCESSORIES;                     
                    }
					else
					{ // �׿��� ��쿡�� ��� ���� ����;
						emSlot = SLOT_LACCESSORIES;
					}
                }
                break;

            case SUIT_HANDHELD:
                {
                    if ( pItem->sSuitOp.IsBOTHHAND() )
                    {
                        emSlot = GetCurRHand();
                    }
                    else if ( pItem->sSuitOp.emHand == HAND_BOTH )
                    {
                        emSlot = GetCurRHand();

                        if ( VALID_SLOT_ITEM(GetCurRHand()) )
                        { 
                            if ( !VALID_SLOT_ITEM(GetCurLHand()) )
                            {
                                emSlot = GetCurLHand();
                            }
                        }
                    }
                    else if ( pItem->sSuitOp.emHand&HAND_LEFT )
                    {
                        emSlot = GetCurLHand();
                    }
                    else if ( pItem->sSuitOp.emHand&HAND_RIGHT )
                    {
                        emSlot = GetCurRHand();
                    }
                }
                break;
            }
        }
        break;

    case ITEM_ARROW:
    case ITEM_CHARM:
    case ITEM_BULLET:
	case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
        {
            emSlot = GetCurLHand();
        }
        break;

    case ITEM_VEHICLE:
        {
            emSlot = SLOT_VEHICLE;

            if ( m_emVehicle == EMVEHICLE_PASSENGER )
            {
                m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_SUBSEAT_IMPOSSIBLE") );
                return S_FALSE;
            }
        }
        break;

    default:
        {
            return E_FAIL;
        }
        break;
    }

	const EMSLOTCHECK_RESULT emSlotCheck=CHECKSLOT_ITEM( pInvenItem->sItemCustom.GetNativeID(), emSlot );
    if ( emSlotCheck != EMSLOTCHECK_OK )
    {
		PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SLOTTOINVEN_ERROR", emSlotCheck) ); 
        return S_FALSE;
    }

    return ReqInvenToSpecificSlot( emSlot, wPosX, wPosY );
}

//! �κ� ������ ���� �������� ���� ��û
HRESULT GLCharacter::ReqInvenToSpecificSlot( EMSLOT emSlot, WORD wPosX, WORD wPosY )
{
    if ( !IsValidBody() )               
        return S_FALSE;

    if ( ValidWindowOpen() )            
        return S_FALSE;

    if ( m_sCONFTING.IsCONFRONTING() )	
        return S_FALSE;

    //! �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
    //
    if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
        return S_FALSE;

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

	const SITEMCUSTOM& sIvenCustomItem = pInvenItem->sItemCustom;

    //	�������� �˻�.
    if ( !ACCEPT_ITEM( sIvenCustomItem ) )
    {
        return S_FALSE;
    }

	if( sIvenCustomItem.IsBoxWrapped() )
	{
		return S_FALSE;
	}

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    SITEM* pItem = GLogicData::GetInstance().GetItem( sIvenCustomItem.GetNativeID() );
    if ( !pItem )
        return E_FAIL;

    if ( EMSLOTCHECK_OK != CHECKSLOT_ITEM( sIvenCustomItem.GetNativeID(), emSlot ) )
    {
        return S_FALSE;
    }

    // �ش� ���Կ� �������� �ִٸ�
    const SITEM* pInvenData = GLogicData::GetInstance().GetItem ( sIvenCustomItem.GetNativeID() );
    if ( !pInvenData )
        return E_FAIL;

    SITEMCUSTOM sSlotItem = GET_SLOT_ITEM( emSlot );

    UINT        nToInvenNums = 0;

    if ( sSlotItem.GetNativeID() != NATIVEID_NULL() )
    {
        if ( !VALID_SLOT_ITEM(emSlot) )
            return E_FAIL;

        const SITEM* pSlotData = GLogicData::GetInstance().GetItem ( sSlotItem.GetNativeID() );

        // ���� �Ҽ� �ִ� ���������� ����.
        bool bMERGE(false);
        bMERGE = (pInvenData->sBasicOp.sNativeID==pSlotData->sBasicOp.sNativeID);
        if (bMERGE)	bMERGE = ( pInvenData->ISPILE() );
        if (bMERGE) bMERGE = ( sSlotItem.wTurnNum<pInvenData->sDrugOp.wPileNum );
        if (bMERGE) bMERGE = ( sIvenCustomItem.wTurnNum<pInvenData->sDrugOp.wPileNum );
        if ( bMERGE )
        {
            // ���� ������ ������
            WORD wTurnNum = sIvenCustomItem.wTurnNum + sSlotItem.wTurnNum;

            if ( wTurnNum <= pInvenData->sDrugOp.wPileNum )
            {
                // ������ â�� �ִ� �������� ������ �����ۿ� ������ ������ ������ ���
            }
            else
            {
                // ������ â�� �ִ� �������� ������ �����ۿ� �����ϰ� ���� ���
                nToInvenNums++;
            }
        }
        else
        {
            // ���� ������ �������� �ƴ� ���
            nToInvenNums++;
        }
    }

    //	�տ� ��� ������ ��� �ٸ� ���� �������� �˻��Ѵ�
    if ( pInvenData->sSuitOp.emSuit==SUIT_HANDHELD )
    {
        EMSLOT emToHand      = emSlot == GetCurLHand() ? GetCurLHand() : GetCurRHand ();
        EMSLOT emAnotherHand = emSlot == GetCurLHand() ? GetCurRHand() : GetCurLHand ();

        const SITEM *pITEM_TO      = pInvenData;
        SITEM *pITEM_ANOTHER = NULL;

        if ( VALID_SLOT_ITEM(emAnotherHand) )  
            pITEM_ANOTHER = GLogicData::GetInstance().GetItem(GET_SLOT_ITEM(emAnotherHand).GetNativeID());

        if ( pITEM_ANOTHER )
        {
            BOOL bSlotRelease = true;

			const EMCHARINDEX emClass = GETCHARINDEX();
            switch ( pITEM_ANOTHER->sBasicOp.emItemType )
            {
                // �ٸ� ������ �Ҹ� �������ϰ��
            case ITEM_ARROW:
				{
					if ( GLCI_ARCHER_M == emClass || GLCI_ARCHER_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_BOW:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

            case ITEM_CHARM:
				{
					if ( GLCI_SPIRIT_M == emClass || GLCI_SPIRIT_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_SPEAR:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

            case ITEM_BULLET: 
                {
					if ( GLCI_SCIENTIST_M == emClass || GLCI_SCIENTIST_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_PISTOL:
						case ITEMATT_RAILGUN:
						case ITEMATT_PORTALGUN:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
                }
                break;

			case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
				{
					if ( GLCI_ACTOR_M == emClass || GLCI_ACTOR_W == emClass )
					{
						switch ( pITEM_TO->sSuitOp.emAttack )
						{
						case ITEMATT_ACTOR_HAMMER:
						case ITEMATT_ACTOR_SHIELD:
						case ITEMATT_ACTOR_UMBRELLA:
						case ITEMATT_WEAPON_STR:
						case ITEMATT_WEAPON_DEX:
						case ITEMATT_WEAPON_SPI:
							{
								bSlotRelease = false;
							}
							break;
						};
					}
				}
				break;

                // �ٸ� ������ ������ ���
            case ITEM_SUIT:
                {
                    // �Ѵ� �Ѽ� ������
                    if ( (!(pITEM_TO->sSuitOp.emSuit==SUIT_HANDHELD) && pITEM_TO->sSuitOp.IsBOTHHAND()) &&
                        !((pITEM_ANOTHER->sSuitOp.emSuit==SUIT_HANDHELD) && pITEM_ANOTHER->sSuitOp.IsBOTHHAND()) )
                    {
                        bSlotRelease = false;
                    }
                    else
                    {
                        switch ( pITEM_TO->sBasicOp.emItemType )
                        {
                            // Ÿ���� �Ҹ��̶��
                            // �ٸ����Ⱑ ��ȿ���� �˻�� CHECKSLOT_ITEM���� �Ѵ�.
                        case ITEM_ARROW:
                        case ITEM_CHARM:
                        case ITEM_BULLET: 
						case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
                            {
                                bSlotRelease = false;
                            }
                            break;
                        };
                    }
                }
                break;
            }

            if ( bSlotRelease ) 
            {
                nToInvenNums++;
            }
        }
    }

    WORD wInvenPosX = 0;
    WORD wInvenPosY = 0;

    // �κ��� �� ���� ����
    DWORD dwEmptyCount = m_cInventory.GetAmountEmpty( FALSE );

    WORD wVALIDY = m_cInventory.GetValidCellY( FALSE );

    // �����Ϸ��� �����ۿ�, �������� �������� ���� �� �ִٸ�, �� ���� ������ +1 �� ȿ���̴�
    if ( wPosY < wVALIDY )
    {
        dwEmptyCount += 1;
    }

    // �ʿ��� �κ��� ĭ ����, �����Ϸ��� �������� ������ �� ���ٸ� ����
    if ( dwEmptyCount < nToInvenNums )
    {
        PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PLAYINTERFACE_13") ); 
        return S_FALSE;
    }

    GLMSG::SNETPC_REQ_INVEN_TO_SLOT NetMsg;
    NetMsg.emToSlot = emSlot;
    NetMsg.wPosX = pInvenItem->wPosX;
    NetMsg.wPosY = pInvenItem->wPosY;
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

HRESULT GLCharacter::ReqInvenCure(WORD wPosX, WORD wPosY)
{
    // �Լ� ������ ���� �ڸ��� �Ű���;
    // ������ �ʿ���� FindPosItem���� ����� ����ȭ �ʿ�;

    // pk ����� ���θ� ��� �̻��� ��� ȸ������ ����� ���´�.
    DWORD dwPK_LEVEL = GET_PK_LEVEL();
    if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
    {
        m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_PK_LEVEL") );
        return E_FAIL;
    }

    // ����� ���ǿ� ���� ���� ��� ����;
    if ( m_sCONFTING.IsFIGHTING() && !m_sCONFTING.IsRECOVE() )
    {			
        m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("CONFRONT_RECOVE"), 0));
        return E_FAIL;
    }

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
    if ( !pInvenItem )
        return E_FAIL;

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem || ( pItem->sBasicOp.emItemType!=ITEM_CURE && pItem->sBasicOp.emItemType!=ITEM_EFFECT_RELEASE_CURE ) )
        return E_FAIL;

    PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap ();
    if ( !pLandManClient )
        return E_FAIL;

    const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pLandManClient->GetBaseMapID ().dwID );
    if ( pMapNode == NULL ) 
        return E_FAIL;

    EMITEM_DRUG DrugType = pItem->DrugType();
    if (DrugType != ITEM_DRUG_NUNE)
    {
        if (ReqInvenDrugDrug(DrugType))
            return E_FAIL;

        if ( pMapNode->IsNewInstanceMap() == false )
        {
            if (pMapNode->IsBlockDrugItem(pItem->BasicType()))
            {
                m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PROHIBIT_ITEM_MESSAGE") );
                return E_FAIL;
            }
        }

		bool bCheck = true;
        // ü�� ȸ�� �������� ����Ҽ� ���� ��Ȳ�̴�.
        switch(DrugType)
        {
        case ITEM_DRUG_HP:
        case ITEM_DRUG_HP_MP:
        case ITEM_DRUG_HP_MP_SP:
        case ITEM_DRUG_HP_CURE:
        case ITEM_DRUG_HP_MP_SP_CURE:
            {
                if ( IsActionLimit( EMACTION_LIMIT_DRUG_HP ) )
                {
                    m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_DRUG") );
                    return E_FAIL;
                }
            }
            break;
		case ITEM_DRUG_EFFECT_RELEASE :
			{
				bCheck = false;
			}
			break;
        };

        //	�������� ����Ҽ� ���� ��Ȳ�̴�.
        if ( IsActionLimit( EMACTION_LIMIT_DRUG ) && bCheck )
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_DRUG") );
            return E_FAIL;
        }

        //	�������� ����Ҽ� ���� ��Ȳ�̴�.
        if ( IsActionLimit( EMACTION_LIMIT_ITEM ) && bCheck )
        {
            m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
            return E_FAIL;
        }

        GLMSG::SNETPC_REQ_INVENDRUG NetMsg(wPosX, wPosY, true);
        NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
    return S_OK;
}

//! ��ų ���� ��û. ( �κ� ����������. )
HRESULT GLCharacter::ReqInvenSkill ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )					return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	// �ʱ�ȭ�� ���� ���¿��� ��ų�� �����Ҽ� ����
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_StatsReset_IsOpen ) )
	{
		return S_FALSE;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_SKILL )
        return E_FAIL;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL(sSKILL_ID) )
	{
		//	�̹� ������ ��ų.
		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL( sSKILL_ID );
	if ( emSKILL_LEARNCHECK != EMSKILL_LEARN_OK )
	{
		//	��ų ���� �䱸 ������ �������� ���մϴ�.
		return E_FAIL;
	}

	//! ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPC_REQ_LEARNSKILL NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqInvenItemDecompose(WORD wPosX, WORD wPosY)
{
	if (!IsValidBody())
		return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return E_FAIL;

	if ( !pInvenItem->sItemCustom.IsDecompose() )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_DECOMPOSE_MESSAGE", EMREQ_ITEMDECOMPOSE_FB_NOT_SPLIT) );
		return E_FAIL;
	}

	if ( GetInvenMoney() < GLCONST_CHAR::nDecomposeCost )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("EMREQ_DECOMPOSE_MESSAGE", EMREQ_ITEMDECOMPOSE_FB_NOT_ENOUGH_MONEY) );
		return E_FAIL;
	}

	GLMSG::SNETPC_REQ_INVEN_ITEMDECOMPOSE_CF NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	return S_OK;
}

HRESULT GLCharacter::ReqSkillUp(const SNATIVEID& skill_id)
{
	if (!IsValidBody())
        return E_FAIL;

	EMSKILL_LEARNCHECK emSKILL_LVLUPCHECK = EMSKILL_LEARN_UNKNOWN;
	SCHARSKILL* pCHARSKILL = GETLEARNED_SKILL(skill_id);
	if ( !pCHARSKILL )
	{
		//	���������� �߻� �� �� ���� ��Ȳ.
		return E_FAIL;
	}

	emSKILL_LVLUPCHECK = CHECKLEARNABLE_SKILL(skill_id);
	if ( emSKILL_LVLUPCHECK!=EMSKILL_LEARN_OK )
	{
		//	���� ������ �����մϴ�.
		return E_FAIL;
	}

	GLMSG::SNETPC_REQ_SKILLUP NetMsg;
	NetMsg.skill_id = skill_id;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRnSkillUp( std::vector<SNATIVEID>& vecSkillID )
{
    if (!IsValidBody())
        return E_FAIL;

    DWORD dwUsingAllMoney = 0;
    // Note : 12.05.02 ���� �������� �ӽ÷� ��ų�� �ø��� ������ ����
    //  ������ ��ų �������̳� ���� �ִ� ������ ��� UI���� �����Ѵ�.
    BOOST_FOREACH( SNATIVEID& sSkillID, vecSkillID )
    {
        // �߸��� SKILL ID
        if ( sSkillID == NATIVEID_NULL() )
            return E_FAIL;

		// �������� ��ų�� �ƴ϶�� ��ų������ ���� �� ������ �����Ѵ�;
		if ( false == GSCL_INST.IsOriginalSkill( sSkillID ) )
		{
			GLSKILL* pChangeSkill = GLSkillMan::GetInstance().GetData( sSkillID.Mid(), sSkillID.Sid() );
			if ( NULL == pChangeSkill )
				return E_FAIL;

			// ���� �� ��ų�� ������ ���� �����Ѵ�;
			if ( true == pChangeSkill->m_sEXT_DATA.idDisguiseSkill.IsValidNativeID() )
				sSkillID = pChangeSkill->m_sEXT_DATA.idDisguiseSkill;
		}

        // ��ų�� ���� �ܰ��ΰ�? ������ �ø��� �ܰ��ΰ�?
        SCHARSKILL* pCHARSKILL = GETLEARNED_SKILL( sSkillID );
        if ( pCHARSKILL == NULL )
        {         
            bool bUsingScrool = GLogicData::GetInstance().GetRnSkillUseScrool( sSkillID );
            // ��ũ���� ����Ѵٸ� ��ų ��� �� ����.
            if ( bUsingScrool )
               return E_FAIL;

			// ó�� ���� ��ų�̸� ���Ż����̸� ��ų������ ������ ������ �����Ѵ�;
			/*if ( RF_DISGUISE( m_EffSkillVarSet ).IsOn() )
			{
				GLSKILL* pChangeSkill = GLSkillMan::GetInstance().GetData( sSkillID.Mid(), sSkillID.Sid() );
				if ( NULL == pChangeSkill )
					return E_FAIL;

				sSkillID = pChangeSkill->m_sEXT_DATA.idDisguiseSkill;
			}*/
        }
    }
        
    std::vector<GLMSG::SNETPC_REQ_RNSKILLUP> vecNetMsg;
    vecNetMsg.push_back( GLMSG::SNETPC_REQ_RNSKILLUP() );
    int nIndex=0;

	SNATIVEID sTemp;

    BOOST_FOREACH( const SNATIVEID& sSkillID, vecSkillID )
    {
        GLMSG::SNETPC_REQ_RNSKILLUP& NetMsg = vecNetMsg[nIndex];
				
		ChangeToOrgSkill( sSkillID, sTemp );

        if ( NetMsg.AddSkillID( sTemp ) == FALSE )
        {
            vecNetMsg.push_back( GLMSG::SNETPC_REQ_RNSKILLUP() );
            vecNetMsg[++nIndex].AddSkillID( sTemp );
        }
    }

    BOOST_FOREACH( GLMSG::SNETPC_REQ_RNSKILLUP& NetMsg, vecNetMsg )
    {
        if ( 0 < NetMsg.GetCount() )
        {
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
            continue;
        }
    }
    
    return S_OK;
}

HRESULT GLCharacter::ReqReCall ( WORD wPosX, WORD wPosY )
{
	//	�ɸ��� ���� ���°� �ƴ� ���.
	//if ( !IsValidBody() )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_CONDITION") );
	//	return E_FAIL;
	//}
	
	//	�ŷ����� ���.
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_CONDITION") );
	//	return E_FAIL;
	//}

	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
		return E_FAIL;
	}

	SITEM* pVehicleItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
	if ( pVehicleItem )
	{
        if (m_emVehicle == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem(pVehicleItem->VehicleType()))
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_BIKE") );
			return E_FAIL;                        
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_BIKE") );
			return E_FAIL;
		}
	}

	if ( m_sCONFTING.IsCONFRONTING() ||							// ��� ������ ���.
		 m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] )		// �������� ��ų������..
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_CONDITION") );
		return E_FAIL;
	}

	//! pk ����� ������ ��� �̻��� ��� ��ȯ ī���� ����� ���´�.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_PK_LEVEL") );
		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_ITEM") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_RECALL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_ITEM") );
		return E_FAIL;
	}

	if ( pItem->sDrugOp.emDrug!=ITEM_DRUG_CALL_SCHOOL &&
		pItem->sDrugOp.emDrug!=ITEM_DRUG_CALL_REGEN &&
		pItem->sDrugOp.emDrug!=ITEM_DRUG_CALL_LASTCALL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_ITEM") );
		return E_FAIL;
	}

    PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();
    if ( !pLandManClient )
        return E_FAIL;

    const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID() );
    if ( !pMapNode )
        return E_FAIL;

    if ( pMapNode->IsBlockDrugItem( pItem->BasicType() ) )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PROHIBIT_ITEM_MESSAGE") );

        return E_FAIL;
    }

	if( PRISONLIMIT.IsLimitStatus(pMapNode, UserLevel(), PML_WARP) )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("WARF_PRISION_LIMIT") );
		return E_FAIL;
	}

	GLMSG::SNETPC_REQ_INVEN_RECALL NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// ��ȯ�� ��� ��û - Ư�������� �̵�
HRESULT GLCharacter::ReqTeleport ( WORD wPosX, WORD wPosY )
{	
	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
		return E_FAIL;
	}


	//	��� ������ ���.
	if ( m_sCONFTING.IsCONFRONTING() ||
		 m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] )		// �������� ��ų������..)
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_CONDITION") );
		return E_FAIL;
	}

	//! pk ����� ������ ��� �̻��� ��� ��ȯ ī���� ����� ���´�.
	//
	DWORD dwPK_LEVEL = GET_PK_LEVEL();
	if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_PK_LEVEL") );
		return E_FAIL;
	}
	SITEM* pVehicleItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
	if ( pVehicleItem )
	{
        if (m_emVehicle == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem(pVehicleItem->VehicleType()))
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_BIKE") );
			return E_FAIL;                        
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_BIKE") );
			return E_FAIL;
		}
	}
	

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_ITEM") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_TELEPORT_CARD )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_ITEM") );
		return E_FAIL;
	}

	if ( pItem->sDrugOp.emDrug!=ITEM_DRUG_CALL_TELEPORT )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_TELEPORT_FB_ITEM") );
		return E_FAIL;
	}

    PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();
    if ( !pLandManClient )
        return E_FAIL;

    const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID() );
    if ( !pMapNode )
        return E_FAIL;

    if ( pMapNode->IsBlockDrugItem( pItem->BasicType() ) )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PROHIBIT_ITEM_MESSAGE") );

        return E_FAIL;
    }

	if( PRISONLIMIT.IsLimitStatus(pMapNode, UserLevel(), PML_WARP) )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("WARF_PRISION_LIMIT") );
		return E_FAIL;
	}

	//!! SF_TODO
	//if ( ValidRebuildOpen() )	// ITEMREBUILD_MARK
	//	m_pInterface->SetItemRebuildWindowClose();

	//!! SF_TODO
	//if ( ValidGarbageOpen() )	// ������
	//	m_pInterface->CloseItemGarbageWindow();

	//!! SF_TODO
	//if ( ValidItemMixOpen() )
	//	m_pInterface->SetItemMixWindowClose();

	GLMSG::SNETPC_REQ_INVEN_TELEPORT NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;

}

HRESULT GLCharacter::ReqBoxOpen ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )	return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_BOX )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOBOX") );
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_EMPTY") );
		return E_FAIL;
	}

	// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
        //	Ȱ�� ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_ERR_ACTIVITY") );
        return E_FAIL;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_ERR_CONTRIBUTION") );
        return E_FAIL;
    }

	//! �κ��� ���� ���� ����.
	//
	GLInventory cInvenTemp;
	cInvenTemp.Assign( m_cInventory );

	int nBoxSize = min(ITEM::SBOX::ITEM_SIZE, (int)pItem->sBox.vecItems.size());
	for ( int i=0; i<nBoxSize; ++i )
	{
		SITEMCUSTOM sCUSTOM( pItem->sBox.vecItems[i].nidITEM );

		if ( sCUSTOM.GetNativeID()==SNATIVEID(false) )
			continue;

		SITEM *pITEM = GLogicData::GetInstance().GetItem ( sCUSTOM.GetNativeID() );
		if ( !pITEM )
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_INVALIDITEM") );
			return E_FAIL;
		}

		BOOL bOK = cInvenTemp.InsertItem ( sCUSTOM );
		if ( !bOK )
		{
			//! �κ��� ������ ���� ������ �Ǵܵ�.
			//
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOTINVEN") );
			return E_FAIL;
		}
	}

	//! ������ ���� ������ ��û.
	//
	GLMSG::SNET_INVEN_BOXOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRandumBoxOpen ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )	return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_RANDOMITEM )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_FAIL") );
		return E_FAIL;
	}

	if ( !pItem->sRandomBox.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_RANDOMBOXOPEN_FB_EMPTY") );
		return E_FAIL;
	}

	//! ������ ���� ������ ��û.
	//
	GLMSG::SNET_INVEN_RANDOMBOXOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPetSkinPackOpen ( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

	if ( pHold->sBasicOp.emItemType != ITEM_PET_SKIN_PACK )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_FAIL") );
		return E_FAIL;
	}


	if ( !pHold->sPetSkinPack.VALID() )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_EMPTY") );
		return E_FAIL;
	}

	// ���� ���� �Ǿ� ���� ���� ���
	GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
	if ( pMyPet == NULL || !pMyPet->IsVALID() )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_PET_FAIL") );
		return E_FAIL;
	}

	if ( pInvenItem->sItemCustom.PetDbNum() != pMyPet->DbNum() )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_DIFFER_CARD_FAIL") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_PET_CARD )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_PETSKINPACK_FB_PETCARD_FAIL") );
		return E_FAIL;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_UsePetSkinCard,
		"-dw, -n, -n, -n, -n",
		pHold->sBasicOp.sNativeID.dwID,
		wPosX, wPosY, 
		holditem::GetHoldPosX(), holditem::GetHoldPosY() );
	
	//! ������ ���� ������ ��û.
	//
	/*GLMSG::SNETPET_SKINPACKOPEN NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD ( &NetMsg );*/
	return S_OK;
}

//! ���ö�(���Ĺ���) ��� ��û.
HRESULT GLCharacter::ReqInvenLunchBox ( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	// ��ý� �Ұ���
	if ( m_sCONFTING.IsCONFRONTING() )
	{
		return E_FAIL;
	}

	// ���ö� ��� �Ұ��� ���� Ȯ��
	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	BOOL bLunchBoxForbid = pLand ? pLand->IsLunchBoxForbid() : TRUE;
	if ( bLunchBoxForbid )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::RED, ID2GAMEINTEXT("LUNCHBOX_DONT_USE_AREA") );
		return S_FALSE;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_LUNCHBOX )
        return E_FAIL;

	// ���� ���� Ȯ��
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
	{
		return S_FALSE;
	}

	//! ���ö� ����� ��û�մϴ�.
	//
	GLMSG::SNETPC_REQ_INVEN_LUNCHBOX NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqStagePass(WORD wPosX, WORD wPosY)
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);

	//! ������ ����
	if ( !pInvenItem )
	{
		//m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	//! ��밡�ɸ� Ȯ��
	if ( !m_pGaeaClient->IsInstantMap() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INFINITYSTAIRS_ITEM_STAGE_PASS_NOT_USE_MAP"));
		return E_FAIL;
	}

	//! Note : ������ ���� Ȯ��
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_STAGE_PASS )
	{
		return E_FAIL;
	}

	int nStage = static_cast<int>(pItem->sDrugOp.dwStage);

	//! �̵� ���� �������� Ȯ��
	if ( m_pGaeaClient->GetThisStage() >= nStage )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INFINITYSTAIRS_ITEM_STAGE_PASS_NOT_USE_STAGE"));
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	//! �������� �н��� ����� ��û�մϴ�.
	//
	GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_CF NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqEnterNumCharge(WORD wPosX, WORD wPosY)
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);

	//! ������ ����
	if ( !pInvenItem )
	{
		//m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_BOXOPEN_FB_NOITEM") );
		return E_FAIL;
	}

	//! ��밡�ɸ� Ȯ��
	//if ( !m_pGaeaClient->IsInstantMap() )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INFINITYSTAIRS_ITEM_STAGE_PASS_NOT_USE_MAP"));
	//	return E_FAIL;
	//}

	//! Note : ������ ���� Ȯ��
	const SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_ENTER_NUM_CHARGE )
	{
		return E_FAIL;
	}

	int nStage = static_cast<int>(pItem->sDrugOp.dwLv);

	//! ���� ���� Ƚ�� Ȯ��
	//if ( m_pGaeaClient->GetThisStage() >= nStage )
	//{
	//	m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("INFINITYSTAIRS_ITEM_STAGE_PASS_NOT_USE_STAGE"));
	//	return E_FAIL;
	//}


	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;



	//@ ���� ������, ĳ���� DB ID + MAPID(main + sub)

	//root.SetMap(233, 0);			// ��ID
	
	//MAP_TABLE& table = m_pGaeaClient->GetThisMapTable();


	//! �δ� ����Ƚ�� ������ ����� ��û�մϴ�.
	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_CF NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	//msgpack::sbuffer PackBuffer;
	//msgpack::pack(PackBuffer, NetMsg);
	//m_pGaeaClient->NETSENDTOAGENT(NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_CF, PackBuffer);
 

	return S_OK;
}

HRESULT GLCharacter::ReqConsignmentSlotExpiration( WORD wPosX, WORD wPosY )
{
    SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);

    //! ������ ����
    if ( !pInvenItem )
        return E_FAIL;

    //! Note : ������ ���� Ȯ��
    const SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );

    if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_CONSIGMENT_SALE_EXTEND_CARD )
        return E_FAIL;

    GLMSG::SNETPC_REQ_CONSIGNEMENT_SLOT_EXPIRATION NetMsg;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

HRESULT GLCharacter::ReqDisJunction( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_FAIL") );
		return E_FAIL;
	}

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

	if ( pHold->sBasicOp.emItemType!=ITEM_DISJUNCTION )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_FAIL") );
		return S_FALSE;
	}

	if ( pInvenItem->sItemCustom.nidDISGUISE==SNATIVEID(false) )
	{
        holditem::ReleaseHoldItem();

		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_DISJUNCTION_FB_NONEED") );
		return S_FALSE;
	}

	//! ������ �ڽ��� �и� ��û.
	//
	GLMSG::SNET_INVEN_DISJUNCTION NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

//! ���� �κ��丮�� ������ ����Ҷ�. by luxes.
HRESULT GLCharacter::ReqSlotConsume( EMSLOT emSlot )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( ValidWindowOpen() )
        return E_FAIL;

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().GetNativeID() );

    // �տ� ���� ����� ���
    if ( pHold )
	{
		//! �ڽ�Ƭ �÷� ���� ī�带 ����� ���. by luxes.
        switch ( pHold->sBasicOp.emItemType )
        {
            case ITEM_COSTUM_COLOR_CH_CD:
		    {
			    //bool bRestrict = pHold->sBasicOp.IsRestrict();
				bool bRestrict = ( holditem::GetHoldItem().IsRestrict() != NULL );
			    InvenCostumColorChange( emSlot, bRestrict );
		    }
            break;
        };
	}
    // ���� �ʾ��� ��� ������ �������� ����
    else
    {
        if ( !VALID_SLOT_ITEM(emSlot) )	    
            return S_FALSE;

        if ( ValidWindowOpen() )		    
            return S_FALSE;

        //! ���� �����ִ� �߿��� ���ⱳü ����
        //if ( IsOpenNPCMarket () )
        //    return S_FALSE;

        if ( m_sCONFTING.IsCONFRONTING() )	
            return S_FALSE;

        //! �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
        //
        if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
            return S_FALSE;

        if ( m_emVehicle == EMVEHICLE_PASSENGER && emSlot == SLOT_VEHICLE )
        {
            m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_SUBSEAT_IMPOSSIBLE") );
            return S_FALSE;
        }

        SITEM* pItem = GET_SLOT_ITEMDATA(emSlot);
        if ( !pItem )
			return E_FAIL;

        WORD wInvenPosX, wInvenPosY;
        BOOL bOk = m_cInventory.FindInsertable( wInvenPosX, wInvenPosY );
        if ( !bOk )
        {
            //	�ι��丮�� ���� ������ ����.
            return S_FALSE;
        }

        //	�������� ���⸦ ���ٸ� �޼յ� ���� �����Ѵ�
        if ( emSlot == GetCurRHand() )
        {
            EMSLOT emLHand = GetCurLHand();

            if ( VALID_SLOT_ITEM(emLHand) )
            {
                SITEM* pLHandItem = GET_SLOT_ITEMDATA( emLHand );

                switch ( pLHandItem->sBasicOp.emItemType )
                {
                case ITEM_ARROW:
                case ITEM_CHARM:
                case ITEM_BULLET:
				case ITEM_CHANGE_ACTORS_DISGUISE_INFO:
                    {
                        bOk = m_cInventory.ValidCheckInsrt( 2, false );
                        if ( !bOk )
                        {
                            //	�ι��丮�� ���� ������ ����.
                            return S_FALSE;
                        }
                    }
                    break;
                };
            }
        }

        GLMSG::SNETPC_REQ_SLOT_TO_INVEN NetMsg;
        NetMsg.wPosX = wInvenPosX;
        NetMsg.wPosY = wInvenPosY;
        NetMsg.emFromSlot = emSlot;
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }

	return S_OK;
}

//! �������� �����ϰų� ���ø�.
HRESULT GLCharacter::ReqSlotTo( EMSLOT emSlot )
{
    if ( !holditem::HaveHoldItem() && !VALID_SLOT_ITEM(emSlot) )
    {
        holditem::ReleaseHoldItem();

		return E_FAIL;
    }

	if (ValidWindowOpen())
    {
        holditem::ReleaseHoldItem();

		return E_FAIL;
    }

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
        if ( holditem::HaveHoldItem() && holditem::GetHoldItem().bVietnamGainItem )
        {
            holditem::ReleaseHoldItem();

			return E_FAIL;
        }
	}
//#endif


	//! �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
	//
	if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
    {
        holditem::ReleaseHoldItem();

		return E_FAIL;
    }

	if ( m_emVehicle == EMVEHICLE_PASSENGER && emSlot == SLOT_VEHICLE )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BIKE_SUBSEAT_IMPOSSIBLE") );

        holditem::ReleaseHoldItem();

		return E_FAIL;
	}

	//	SLOT <-> HOLD ( ���� �� ���� )
	bool bEMPTY_SLOT = !ISEMPTY_SLOT(holditem::GetHoldItem().GetNativeID(),emSlot);
    if ( holditem::HaveHoldItem() && bEMPTY_SLOT )
	{
		BOOL bOk;
		bOk = ACCEPT_ITEM( holditem::GetHoldItem() );
		if ( !bOk )
		{
            holditem::ReleaseHoldItem();
			//	�������� �˻�.
			return E_FAIL;
		}

		const EMSLOTCHECK_RESULT emSlotCheck=CHECKSLOT_ITEM( holditem::GetHoldItem().GetNativeID(), emSlot );
		if ( emSlotCheck == EMSLOTCHECK_FAILED )
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SLOTTOINVEN_ERROR", emSlotCheck) ); 
            holditem::ReleaseHoldItem();
			//	�ش罽�԰� ���� �ʽ��ϴ�.
			return E_FAIL;
		}
		else
		if( holditem::GetHoldItem().IsBoxWrapped() )
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SLOTTOINVEN_ERROR", EMSLOTCHECK_FAILED) ); 
			holditem::ReleaseHoldItem();
			//	�ش罽�԰� ���� �ʽ��ϴ�.
			return E_FAIL;
		}

        HRESULT hResult = ReqInvenToSpecificSlot( emSlot, holditem::GetHoldPosX(), holditem::GetHoldPosY() );

        holditem::ReleaseHoldItem();

        if ( hResult != S_OK )
            return hResult;
	}
	//	SLOT -> HOLD ( ���� )
	else if ( VALID_SLOT_ITEM(emSlot) )
	{
        holditem::HoldItem( holditem::WEAR_SLOT, GET_SLOT_ITEM( emSlot), emSlot, 0 );
	}
	//	SLOT <- HOLD ( ���� )
    else if ( holditem::HaveHoldItem() )
	{
		BOOL bOk;
		bOk = ACCEPT_ITEM( holditem::GetHoldItem() );
		if ( !bOk )
		{
            holditem::ReleaseHoldItem();
			//	�������� �˻�.
			return E_FAIL;
		}

		const EMSLOTCHECK_RESULT emSlotCheck=CHECKSLOT_ITEM( holditem::GetHoldItem().GetNativeID(), emSlot );
		if ( emSlotCheck != EMSLOTCHECK_OK )
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SLOTTOINVEN_ERROR", emSlotCheck) ); 
			holditem::ReleaseHoldItem();
			//	�ش罽�԰� ���� �ʽ��ϴ�.
			return E_FAIL;
		}
		else
		if( holditem::GetHoldItem().IsBoxWrapped() )
		{
			PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("SLOTTOINVEN_ERROR", EMSLOTCHECK_FAILED) ); 
			holditem::ReleaseHoldItem();
			//	�ش罽�԰� ���� �ʽ��ϴ�.
			return E_FAIL;
		}

        if ( holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            GLMSG::SNETPC_REQ_INVEN_TO_SLOT NetMsg;
            NetMsg.emToSlot = emSlot;
            NetMsg.wPosX = holditem::GetHoldPosX();
            NetMsg.wPosY = holditem::GetHoldPosY();
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }

        holditem::ReleaseHoldItem();
	}

	return S_OK;
}

HRESULT GLCharacter::ReqSlotChange()
{
	if ( IsReqSwapArms() )
	{
		return E_FAIL;
	}

	//! ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
	//
	if ( isAction(GLAT_SKILL, GLAT_SKILL_WAIT) )
	{
		return E_FAIL;
	}

	GLMSG::SNETPC_REQ_SLOT_CHANGE NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_bReqSwapArms = TRUE;

	return S_OK;
}

//! ������ �ٴڿ� ����.
HRESULT GLCharacter::ReqHoldToField( const D3DXVECTOR3 &vPos )
{
	if ( CheckHoldToField( vPos ) == E_FAIL )
        return E_FAIL;

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
        return E_FAIL;

	SINVENITEM* pHoldInvenItem = m_cInventory.FindPosItem(holditem::GetHoldPosX(), holditem::GetHoldPosY());
	if ( !pHoldInvenItem )
		return false;

	if( pHoldInvenItem->wPosX != holditem::GetHoldPosX() || pHoldInvenItem->wPosY != holditem::GetHoldPosY()  )
	{
		sc::writeLogError(std::string("Inven Item POS ERROR"));
		return false;
	}

	GLMSG::SNETPC_REQ_INVEN_TO_FIELD NetMsg;
	NetMsg.vPos = vPos;
	NetMsg.sItem = holditem::GetHoldItem();
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();

//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
        const SITEMCUSTOM& sHoldItem = holditem::GetHoldItem();

		if ( sHoldItem.bVietnamGainItem )
		{
			NetMsg.bVietnamItem = sHoldItem.bVietnamGainItem;
		}
	}
//#endif

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

void GLCharacter::ReqHoldToFieldForWidget( const char* szSlotID, int nHoldItemPlace, WORD wSlotPosX, WORD wSlotPosY,
										   float fPosX, float fPosY, float fPosZ )
{
	if( NULL == szSlotID )
		return;

	SITEMCUSTOM sItem;
	if( false == GfxInterfaceBase::GetInstance()->GetSlotItem( szSlotID, sItem ) )
		return;

	holditem::HoldItem( holditem::HOLD_PLACE_TYPE( nHoldItemPlace ), sItem, wSlotPosX, wSlotPosY );

	ReqHoldToField( D3DXVECTOR3( fPosX, fPosY, fPosZ ) );
}

HRESULT GLCharacter::CheckHoldToField( const D3DXVECTOR3 &vPos )
{
    if ( !holditem::HaveHoldItem() )	
        return E_FAIL;

	if ( ValidWindowOpen() )
		return E_FAIL;	

    const SITEMCUSTOM& sHoldItem = holditem::GetHoldItem();
	SITEM* pItem = GLogicData::GetInstance().GetItem( sHoldItem.GetNativeID() );
	if ( pItem == NULL )
        return E_FAIL;

	//	�ŷ��ɼ�
	//if ( !pItem->sBasicOp.IsTHROW() )
	if ( !( sHoldItem.IsThrow() != NULL ) )
        return E_FAIL;

	const SMAPNODE* const pMapNode(m_pGaeaClient->FindMapNode( m_pGaeaClient->GetActiveMap()->GetBaseMapID().dwID));
	if ( pMapNode == NULL )
		return E_FAIL;

	if ( pMapNode->IsDropBlockItem() )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ITEM_NOT_DROP_AREA") );
		return E_FAIL;
	}

	switch ( pItem->sBasicOp.emItemType )
	{
	case ITEM_PET_CARD:
		{
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
			if ( pMyPet->IsVALID() && sHoldItem.PetDbNum() == pMyPet->DbNum() )
			{
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PET_CARD_NOT_DROP_AREA") );
				return E_FAIL;
			}
		}
		break;
	case ITEM_QITEM:
		{
			if ( pMapNode->IsQBoxDrop() == false ) 
			{
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("QBOX_NOT_DROP_AREA") );
				return E_FAIL;
			}
		}
		break;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqHoldToFieldFireCracker ( const D3DXVECTOR3 &vPos )
{
    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	if ( ValidWindowOpen() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    SITEM* pITEM = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pITEM )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	if ( pITEM->sBasicOp.emItemType!=ITEM_FIRECRACKER )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	if ( pITEM->sBasicOp.strTargetEffect.empty() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

	GLMSG::SNETPC_REQ_FIRECRACKER NetMsg;
	NetMsg.vPOS = vPos;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

bool GLCharacter::IsNpcPileItem ( SNATIVEID sNID_NPC, DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( ValidWindowOpen() )
		return false;	

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sNID_NPC );

	//	���� NPC�� �������� �ʽ��ϴ�.
	if ( !pCrowData )
		return false;

	if ( pCrowData->GetSaleNum() <= dwChannel )
		return false;

	GLInventory *pInven = pCrowData->GetSaleInven ( dwChannel );
	if ( !pInven )
		return false;

	SINVENITEM* pSaleItem = pInven->FindPosItem(wPosX, wPosY);
	if ( !pSaleItem )
		return false;
		
	SNATIVEID sBUYNID = pSaleItem->sItemCustom.GetNativeID();
	SITEM* pItem = GLogicData::GetInstance().GetItem ( sBUYNID );

	return ( pItem->ISPILE() != FALSE );
}

bool GLCharacter::IsRestartPossible()
{
	GLLandManClient* pLandMan = m_pGaeaClient->GetActiveMap();
	if( NULL == pLandMan )
		return false;

	const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode( pLandMan->GetBaseMapID().dwID );
	if (pMapNode == NULL)
        return false;
    else
	    return !pMapNode->IsRestrt();
}

// *****************************************************
// Desc: �������� �����ϰų�/�Ⱦƹ���
// *****************************************************
HRESULT GLCharacter::ReqNpcTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wBuyNum, EMCROW emCrow )
{   
	if ( ValidWindowOpen() )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    // ���� NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

	switch( emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr<GLSummonClient> spCrow = pLandMan->GetSummon(dwNPCGlobalID);

			if (!spCrow )
			{
				holditem::ReleaseHoldItem();

				return E_FAIL;
			}

			if( spCrow->IsSummonNpcActionable(GetPosition(), GETBODYRADIUS()) == false ||
				spCrow->GetSummonType() != SUMMON_TYPE_NPC ) {

				holditem::ReleaseHoldItem();

				return E_FAIL;
			}
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);

			if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
			{
				holditem::ReleaseHoldItem();

				return E_FAIL;
			}
		}
		break;
	default : return E_FAIL;
	}

    const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( sNPCID );
    if ( !pCrowData )
    {
        holditem::ReleaseHoldItem();
        //	���� NPC�� �������� �ʽ��ϴ�.
        return E_FAIL;
    }

	//	�������� �ȱ� ����.
    if ( holditem::HaveHoldItem() )
	{
        if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            holditem::ReleaseHoldItem();

            return E_FAIL;
        }

        SITEM* pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );

		//	�ŷ��ɼ�
		//if ( !pItem->sBasicOp.IsSALE() )
		if ( !( holditem::GetHoldItem().IsSale() != NULL ) )
		{
            holditem::ReleaseHoldItem();
			//	�ȱ� �������� ����.
			return E_FAIL;
		}

        if ( holditem::GetHoldItem().IsGM_GENITEM() )
		{
            holditem::ReleaseHoldItem();
			//	�ȱ� �������� ����.
			return E_FAIL;
		}

		// ��ī���ϰ��
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
            const SITEMCUSTOM& sHoldItem = holditem::GetHoldItem();
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet->IsVALID () && sHoldItem.PetDbNum() == pMyPet->DbNum() )
			{
                holditem::ReleaseHoldItem();

				return E_FAIL;
			}
		}

		//	�ȱ� �޽���.
		GLMSG::SNETPC_REQ_SALE_TO_NPC NetMsg;
		NetMsg.sNID = sNPCID;
		NetMsg.dwNpcGlobID = dwNPCGlobalID;
		NetMsg.emCrow = emCrow;
        NetMsg.wHoldPosX = holditem::GetHoldPosX();
        NetMsg.wHoldPosY = holditem::GetHoldPosY();
        NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_SALE_TO_NPC)-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

        holditem::ReleaseHoldItem();
	}
	//	������ ��� ����.
	else
	{
		GASSERT(wBuyNum>0&&"������ ���� ��û ������ 1�� �̻��̿��� �Ѵ�.");

		if ( pCrowData->GetSaleNum() <= dwChannel )
        {
            holditem::ReleaseHoldItem();

            return E_FAIL;
        }

		GLInventory *pInven = pCrowData->GetSaleInven ( dwChannel );
		if ( !pInven )
        {
            holditem::ReleaseHoldItem();

			return E_FAIL;
        }

		GLInventorySale* pSale = dynamic_cast<GLInventorySale*>(pInven);

		if( pSale && pSale->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_1 ) 
		{
			holditem::ReleaseHoldItem();

			return E_FAIL;
		}

		SINVENITEM* pSaleItem = pInven->FindPosItem(wPosX, wPosY);
		if ( !pSaleItem )
		{
            holditem::ReleaseHoldItem();
			//	����� �ϴ� �������� �����ϴ�.
			return E_FAIL;
		}
		
		SNATIVEID sBUYNID = pSaleItem->sItemCustom.GetNativeID();
		SITEM* pItem = GLogicData::GetInstance().GetItem( sBUYNID );

        LONGLONG llNpcSellPrice = 0;

        volatile LONGLONG llPrice = 0;
        volatile double fSHOP_RATE = m_pGaeaClient->GetCharacter()->GetBuyRate();

		LONGLONG dwNpcPrice = pCrowData->GetNpcSellPrice(pItem->sBasicOp.sNativeID.dwID);
        if ( dwNpcPrice == 0 )
        {								
            llNpcSellPrice = static_cast<LONGLONG>( pItem->sBasicOp.dwBuyPrice );
            llPrice = llNpcSellPrice * static_cast<LONGLONG>(fSHOP_RATE) / 100;
        }
        else
        {
            llNpcSellPrice = dwNpcPrice;
            llPrice = llNpcSellPrice;								
        }


		if ( GetInvenMoney() < static_cast<LONGLONG>(llPrice*wBuyNum) )
		{
            holditem::ReleaseHoldItem();
			//	���� �����մϴ�.
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOMONEY") );
			return E_FAIL;
		}

		/*
		// ������ ���Žÿ��� ����Ʈ�� �⿩���� Ȯ������ �ʵ��� ���� 
		// ���� ���ǿ��� �˻��ϹǷ� ������ ���� ����
        if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
        {
            holditem::ReleaseHoldItem();
            //	Ȱ�� ����Ʈ�� �����մϴ�.
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOPOINT") );
			return E_FAIL;
        }

		if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
		{
            holditem::ReleaseHoldItem();
			//	�⿩���� �����մϴ�.
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOCONTRIBUTIONPOINT") );
			return E_FAIL;
		}
		*/

		//! �κ��� ���� ������ �ִ��� �˻�.
		//
		BOOL bITEM_SPACE(FALSE);
		if ( pItem->ISPILE() )
		{
			//	��ħ �������� ���.
			WORD wPILENUM = pItem->sDrugOp.wPileNum;
			WORD wREQINSRTNUM = ( wBuyNum * pItem->GETAPPLYNUM() );
			bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sBUYNID, wPILENUM );
		}
		else
		{
			GASSERT(wBuyNum==1&&"��ħ�� �Ұ����� �������� 1������ ���� �����մϴ�.");

			//	�Ϲ� �������� ���.
			WORD wInsertPosX(0), wInsertPosY(0);
			bITEM_SPACE = m_cInventory.FindInsertable ( wInsertPosX, wInsertPosY );
		}

		if ( !bITEM_SPACE )
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOSPACE") );
			return E_FAIL;
		}

		//	���� ��û �޽���.
		GLMSG::SNETPC_REQ_BUY_FROM_NPC NetMsg;
		NetMsg.sNID = sNPCID;
		NetMsg.dwNpcGlobID = dwNPCGlobalID;
		NetMsg.dwChannel = dwChannel;
		NetMsg.wPosX = pSaleItem->wPosX;
		NetMsg.wPosY = pSaleItem->wPosY;
		NetMsg.wBuyNum = wBuyNum;
		NetMsg.emCrow  = emCrow;
        NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_BUY_FROM_NPC)-sizeof(unsigned int));

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		//!! SF_TODO
        //m_pInterface->ResetCartListInDialogueWindowMarketPage( dwChannel, pSaleItem->wPosX, pSaleItem->wPosY );

        holditem::ReleaseHoldItem();
	}

	return S_OK;
}

HRESULT GLCharacter::ReqExChangeNpcTo( const GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC* pData )
{
	if ( ValidWindowOpen() || pData == NULL )
	{
		holditem::ReleaseHoldItem();

		return E_FAIL;
	}

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

	switch( pData->emCrow )
	{
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr<GLSummonClient> spCrow = pLandMan->GetSummon(pData->dwNpcGlobID);

			if (!spCrow)
			{
				holditem::ReleaseHoldItem();

				return E_FAIL;
			}

			if( spCrow->IsSummonNpcActionable(GetPosition(), GETBODYRADIUS()) == false ||
				spCrow->GetSummonType() != SUMMON_TYPE_NPC )
				return E_FAIL;
		}
		break;
	case CROW_NPC :
		{
			std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(pData->dwNpcGlobID);

			if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
			{
				holditem::ReleaseHoldItem();

				return E_FAIL;
			}
		}
		break;
	}

	const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( pData->sNID );
	if ( !pCrowData )
	{
		holditem::ReleaseHoldItem();
		//	���� NPC�� �������� �ʽ��ϴ�.
		return E_FAIL;
	}

	if ( holditem::HaveHoldItem() )
	{
		if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		{
			holditem::ReleaseHoldItem();

			return E_FAIL;
		}

		SITEM* pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );

		//	�ŷ��ɼ�
		//if ( !pItem->sBasicOp.IsSALE() )
		if ( !( holditem::GetHoldItem().IsSale() != NULL ) )
		{
			holditem::ReleaseHoldItem();
			//	�ȱ� �������� ����.
			return E_FAIL;
		}

		if ( holditem::GetHoldItem().IsGM_GENITEM() )
		{
			holditem::ReleaseHoldItem();
			//	�ȱ� �������� ����.
			return E_FAIL;
		}

		// ��ī���ϰ��
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			const SITEMCUSTOM& sHoldItem = holditem::GetHoldItem();
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet->IsVALID () && sHoldItem.PetDbNum() == pMyPet->DbNum() )
			{
				holditem::ReleaseHoldItem();

				return E_FAIL;
			}
		}

		//	�ȱ� �޽���.
		GLMSG::SNETPC_REQ_SALE_TO_NPC NetMsg;
		NetMsg.sNID = pData->sNID;
		NetMsg.dwNpcGlobID = pData->dwNpcGlobID;
		NetMsg.wHoldPosX = holditem::GetHoldPosX();
		NetMsg.wHoldPosY = holditem::GetHoldPosY();
		NetMsg.emCrow    = pData->emCrow;
		NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_SALE_TO_NPC)-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		holditem::ReleaseHoldItem();

		return S_OK; 
	}

	/*
	if ( pCrowData->GetSaleNum() <= pData->dwChannel )
	{
		holditem::ReleaseHoldItem();

		return E_FAIL;
	}
	*/

	//GLInventorySale *pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven ( pData->dwChannel ));

	// ��ȯŸ���� �ƴϸ� ����
	//if ( !pInven || pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_2 )
	//{
	//	holditem::ReleaseHoldItem();
	//  return E_FAIL;
	//}

	GLInventorySale *pInven = NULL;

	BOOL	bSendPacket = FALSE;

	GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC NetMsg;

	NetMsg.sNID			= pData->sNID;
	//NetMsg.dwChannel	= pData->dwChannel;
	NetMsg.dwNpcGlobID  = pData->dwNpcGlobID;
	NetMsg.emCrow		= pData->emCrow;
	

	for( int i = 0; i < GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC::MAX_ITEM_LIST; ++i )
	{
		if ( pCrowData->GetSaleNum() <= pData->itemlist[i].dwChannel )
			continue;
	
		pInven = dynamic_cast<GLInventorySale*>(pCrowData->GetSaleInven ( pData->itemlist[i].dwChannel ));

		if( pInven == NULL ) continue;

		if ( pInven->GetSaleFuncType() != GLInventorySale::CROW_SALE_V_2 )	continue;

		SINVENITEM* pSaleItem = pInven->FindPosItem( pData->itemlist[i].wPosX, pData->itemlist[i].wPosY);

		if ( pSaleItem )
		{
			//SITEM* pItem	  = GLogicData::GetInstance().GetItem( sBUYNID );
			SNATIVEID sBUYNID = pSaleItem->GetNativeID();
			
			EEXTYPE eType = pInven->GetExChangeType(sBUYNID, pData->itemlist[i].wPosX, pData->itemlist[i].wPosY);

			std::vector<sExNeed> rList;

			// ��ȯ�� �ʿ��� ������ �ִ��� Ȯ��
			if( !pInven->GetIsNeedConditionHave( rList, &m_cInventory, sBUYNID, pData->itemlist[i].wPosX, pData->itemlist[i].wPosY, PointShopPoint(), m_nLiving, pData->itemlist[i].wBuyNum )  )
			{
				holditem::ReleaseHoldItem();

				switch( eType )
				{
				case EX_ITEM_TYPE  : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_ITEM_EXCHANGE") );   break;
				case EX_POINT_TYPE : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_RANPT_EXCHANGE") );  break;
				case EX_LVP_TYPE   : m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_LVPT_EXCHANGE") );   break;
				}
			}
			else 
			{
				if( pInven->IsUserExInsertEnable(&m_cInventory, sBUYNID, pData->itemlist[i].wPosX, pData->itemlist[i].wPosY, 0, 0, pData->itemlist[i].wBuyNum ) ) {

					NetMsg.itemlist[i].dwChannel = pData->itemlist[i].dwChannel;
					NetMsg.itemlist[i].wPosX   = pData->itemlist[i].wPosX;
					NetMsg.itemlist[i].wPosY   = pData->itemlist[i].wPosY;
					NetMsg.itemlist[i].wBuyNum = pData->itemlist[i].wBuyNum;

					//!! SF_TODO
					//m_pInterface->ResetCartListInDialogueWindowMarketPage( pData->dwChannel, pSaleItem->wPosX, pSaleItem->wPosY );

					bSendPacket = TRUE;
				}
				else
				{
					m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("BUYITEM_NOSPACE") );
				}											
			}
		}		
	}

	if( bSendPacket ) {

		NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_EXCHANGE_BUY_FROM_NPC)-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);		

		holditem::ReleaseHoldItem();		

		return S_OK;
	}

	holditem::ReleaseHoldItem();

	return E_FAIL;

}

// *****************************************************
// Desc: �κ��� �������� �Ⱦƹ���
// *****************************************************
HRESULT GLCharacter::ReqNpcSaleTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, WORD wPosX, WORD wPosY, EMCROW emCrow )
{
    if ( ValidWindowOpen() )
        return E_FAIL;	

    // ���� NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();

	switch( emCrow )
	{
	case CROW_NPC :
		{
			std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);

			if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
				return E_FAIL;
		}
		break;
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr<GLSummonClient> spCrow = pLandMan->GetSummon(dwNPCGlobalID);

			if (!spCrow )
				return E_FAIL;

			if( spCrow->IsSummonNpcActionable(GetPosition(), GETBODYRADIUS()) == false || 
				spCrow->GetSummonType() != SUMMON_TYPE_NPC )
				return E_FAIL;
		}
		break;
	}

    const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sNPCID );
    if ( !pCrowData )
    {
        //	���� NPC�� �������� �ʽ��ϴ�.
        return E_FAIL;
    }

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        // �ش� ��ġ�� �������� �������� �ʽ��ϴ�.
        return E_FAIL;
    }

    //	�������� �ȱ� ����.
    SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->Id() );

    //	�ŷ��ɼ�
    //if ( !pItem->sBasicOp.IsSALE() )
	if ( !( pInvenItem->sItemCustom.IsSale() != NULL ) )
    {
        //	�ȱ� �������� ����.
        return E_FAIL;
    }

    if ( pInvenItem->sItemCustom.IsGM_GENITEM() )
    {
        //	�ȱ� �������� ����.
        return E_FAIL;
    }

    // ��ī���ϰ��
    if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
    {
        const SITEMCUSTOM& sHoldItem = pInvenItem->sItemCustom;
        GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
        if ( pMyPet->IsVALID () && sHoldItem.PetDbNum() == pMyPet->DbNum() )
        {
            return E_FAIL;
        }
    }

    //	�ȱ� �޽���.
    GLMSG::SNETPC_REQ_SALE_TO_NPC NetMsg;
    NetMsg.dwNpcGlobID = dwNPCGlobalID;
    NetMsg.sNID = sNPCID;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
	NetMsg.emCrow = emCrow;
    NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_SALE_TO_NPC)-sizeof(unsigned int));
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

// *****************************************************
// Desc: �κ��� �������� ���콺 ��Ŭ������ �Ⱦƹ���.
// *****************************************************
HRESULT GLCharacter::ReqNpcDirectSaleTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, WORD wPosX, WORD wPosY, EMCROW emCrow )
{
    if (wPosX == USHRT_MAX || wPosY == USHRT_MAX)
        return E_FAIL;

    if ( ValidWindowOpen() )						
        return E_FAIL;	

    // ���� NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap();

	switch( emCrow )
	{
	case CROW_NPC :
		{
			std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
			if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
				return E_FAIL;
		}
		break;
	case CROW_SUMMON :
		{
			std::tr1::shared_ptr<GLSummonClient> spCrow = pLandMan->GetSummon(dwNPCGlobalID);
			if (!spCrow )
				return E_FAIL;

			if( spCrow->IsSummonNpcActionable(GetPosition(), GETBODYRADIUS()) == false || 
				spCrow->GetSummonType() != SUMMON_TYPE_NPC )
				return E_FAIL;
		}
		break;
	}
    
    const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData( sNPCID );
    if ( !pCrowData )
    {
        //	���� NPC�� �������� �ʽ��ϴ�.
        return E_FAIL;
    }

    SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
    {
        // �ش� ��ġ�� �������� �������� �ʽ��ϴ�.
        return E_FAIL;
    }

    //	�������� �ȱ� ����.
    SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->Id() );

    //	�ŷ��ɼ�
    //if ( !pItem->sBasicOp.IsSALE() )
	if ( !( pInvenItem->sItemCustom.IsSale() != NULL ) )
    {
        //	�ȱ� �������� ����.
        return E_FAIL;
    }

    if ( pInvenItem->sItemCustom.IsGM_GENITEM() )
    {
        //	�ȱ� �������� ����.
        return E_FAIL;
    }

    // ��ī���ϰ��
    if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
    {
        const SITEMCUSTOM& sHoldItem = pInvenItem->sItemCustom;
        GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
        if ( pMyPet->IsVALID() && sHoldItem.PetDbNum() == pMyPet->DbNum() )
        {
            return E_FAIL;
        }
    }

    //	�ȱ� �޽���.
    GLMSG::SNETPC_REQ_SALE_TO_NPC NetMsg;
    NetMsg.dwNpcGlobID = dwNPCGlobalID;
    NetMsg.sNID = sNPCID;
    NetMsg.wPosX = wPosX;
    NetMsg.wPosY = wPosY;
	NetMsg.emCrow = emCrow;
    NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_SALE_TO_NPC)-sizeof(unsigned int));
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

//! Desc: ���� ������ ���� DB���� ��������
HRESULT GLCharacter::ReqItemBankInfo(bool bFirstJoin)
{
    DWORD CurrentTime = GetTickCount();
    DWORD DiffTime = CurrentTime - m_cInvenCharged.GetLastReqTime();
    if ( DiffTime > 60000 || bFirstJoin )
    {
        // 1 �п� �ѹ����� ȣ�Ⱑ��
	    GLMSG::NET_GET_CHARGED_ITEM_CF NetMsg(bFirstJoin);
	    //NetMsg.dwCharID = m_CharDbNum;
	    //NetMsg.SetUserID(m_szUID);

	    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

        m_cInvenCharged.SetLastReqTime(CurrentTime);
	    m_cInvenCharged.DeleteItemAll();
	    m_mapChargedKey.clear();

		return S_OK;
    }

	return S_FALSE;
}

HRESULT GLCharacter::ReqCheckRebuySupplies( bool bCheck )
{
    GLMSG::SNET_CHECK_REBUY_SUPPLIES NetMsg( bCheck );
    m_pGaeaClient->NETSENDTOAGENT( &NetMsg );

    return S_OK;
}

HRESULT GLCharacter::ReqChargedItemTo ( WORD wPosX, WORD wPosY )
{
	SINVENITEM *pINVENITEM = m_cInvenCharged.GetItem(wPosX, wPosY);
	if ( !pINVENITEM )
		return E_FAIL;

    SITEM* pItem =  GLogicData::GetInstance().GetItem( pINVENITEM->sItemCustom.GetNativeID() );
    if ( !pItem )
		return E_FAIL;

    WORD wOutPosX, wOutPosY;
    BOOL bOK = m_cInventory.FindInsertable ( wOutPosX, wOutPosY );
    if ( !bOK )
    {
        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHARGED_ITEM_GET_FB_NOINVEN") );
        return S_FALSE;
    }

	SNATIVEID nidPOS(wPosX,wPosY);
	MAPSHOP_KEY_ITER iter = m_mapChargedKey.find ( nidPOS.dwID );
	if ( m_mapChargedKey.end()==iter )
		return E_FAIL;

	std::string strPurKey = (*iter).second;
	
	//! ������ �������� ��û.
	//
	GLMSG::SNET_CHARGED_ITEM_GET NetMsg;
	NetMsg.dwID = nidPOS.dwID;
	StringCchCopy ( NetMsg.szPurKey, PURKEY_LENGTH+1, strPurKey.c_str() );
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

void GLCharacter::ReqLevelUp ()
{
	GLMSG::SNETPC_REQ_LEVELUP NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLCharacter::ReqStatsUp ( EMSTATS emStats )
{
//	if ( m_wStatsPoint==0 )	return;
	GASSERT(m_wStatsPoint > 0);
	GLMSG::SNETPC_REQ_STATSUP NetMsg;
	NetMsg.emStats = emStats;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLCharacter::ReqStatsUp(WORD* emStatsArray)
{
	GASSERT(m_wStatsPoint > 0);
	GLMSG::SNETPC_REQ_STATSUP_EX NetMsg;
	std::copy(emStatsArray,emStatsArray+EMSIZE,NetMsg.emStatsArray);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}


HRESULT GLCharacter::ReqSkillQuickSet(const SNATIVEID& skill_id, const WORD wSLOT)
{
	if (EMSKILLQUICK_SIZE <= wSLOT)
        return E_FAIL;

	//! ��� ��ų�� �ƴ� ��� ��ҵ�.
	if (!ISLEARNED_SKILL(skill_id))
        return E_FAIL;

	SNATIVEID sTempID;

	ChangeToOrgSkill( skill_id, sTempID );

	GLMSG::SNETPC_REQ_SKILLQUICK_SET NetMsg;
	NetMsg.wSLOT = wSLOT;
	//NetMsg.skill_id = skill_id;
	NetMsg.skill_id = sTempID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	// [shhan][2014.12.08] ��ų�� Tray �� ���� ��� �� �ִϸ��̼��� �̸� �ε��� �ϵ��� �Ѵ�.
	{
		EMCHARINDEX emIndex = CharClassToIndex(m_emClass);
		DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().FindData( GLCONST_CHAR::szCharSkin[emIndex] );
		if (!pSkinCharData)
			return S_OK;

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( skill_id );
		if ( pSkill )	//	��ų�� ������ ���
		{
			pSkinCharData->LoadBIN_HaveToAnimationForMyChar( pSkill->m_sEXT_DATA.emANIMTYPE, pSkill->m_sEXT_DATA.emANISTYPE, TRUE );
		}
	}

	return S_OK;
}

HRESULT GLCharacter::ReqSkillQuickReSet ( const WORD wSLOT )
{
	if ( EMSKILLQUICK_SIZE <= wSLOT )	return E_FAIL;

	//! Ŭ���̾�Ʈ�� ������ �����.
	//m_sSKILLQUICK[wSLOT] = NATIVEID_NULL();

	GLMSG::SNETPC_REQ_SKILLQUICK_RESET NetMsg;
	NetMsg.wSLOT = wSLOT;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! ������ ���Կ� ���/����.
HRESULT GLCharacter::ReqItemQuickSet( const WORD wSLOT )
{
	if ( EMACTIONQUICK_SIZE <= wSLOT )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

    const SITEMCUSTOM& sITEM = holditem::GetHoldItem();
	
//#if defined(VN_PARAM) //vietnamtest%%%
	if (m_ServiceProvider == SP_VIETNAM)
	{
		if ( sITEM.bVietnamGainItem )
        {
            holditem::ReleaseHoldItem();

			return S_FALSE;
        }
	}
//#endif

	SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEM.GetNativeID() );
	if ( !pITEM )
    {
        holditem::ReleaseHoldItem();

        return S_FALSE;
    }

	if ( pITEM->sBasicOp.emItemType != ITEM_EFFECT_RELEASE_CURE &&
		 pITEM->sBasicOp.emItemType != ITEM_CURE &&
		 pITEM->sBasicOp.emItemType != ITEM_RECALL &&
		 pITEM->sBasicOp.emItemType != ITEM_TELEPORT_CARD &&
		 pITEM->sBasicOp.emItemType != ITEM_PET_CARD )
    {
        holditem::ReleaseHoldItem();

		 return S_FALSE;
    }

	//! Ŭ���̾�Ʈ�� ���Կ� �ٷ� �־���.
	//m_sACTIONQUICK[wSLOT].wACT = EMACT_SLOT_DRUG;
	//m_sACTIONQUICK[wSLOT].sNID = sITEM.sNativeID;

	//! ������ ���Կ� �� ���� ����.
	GLMSG::SNETPC_REQ_ACTIONQUICK_SET NetMsg;
	NetMsg.wSLOT = wSLOT;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();
	NetMsg.wACT = EMACT_SLOT_DRUG;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();

	return S_OK;
}

HRESULT GLCharacter::ReqItemQuickReSet ( const WORD wSLOT )
{
	if ( EMACTIONQUICK_SIZE <= wSLOT )
        return E_FAIL;

	//m_sACTIONQUICK[wSLOT].RESET ();

	GLMSG::SNETPC_REQ_ACTIONQUICK_RESET NetMsg;
	NetMsg.wSLOT = wSLOT;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! ��ų ���Կ� ���/����.
HRESULT GLCharacter::ReqSkillRunSet ( const WORD wSLOT )
{
	if ( EMSKILLQUICK_SIZE <= wSLOT )
        return E_FAIL;


	const SNATIVEID& skill_id = RF_DISGUISE( m_EffSkillVarSet ).IsOn() ? m_sChangeQUICK[wSLOT] : m_sSKILLQUICK[wSLOT];

	//const SNATIVEID &skill_id = m_sSKILLQUICK[wSLOT];
	if ( skill_id==NATIVEID_NULL() )
        return E_FAIL;

	//! ��� ��ų�� �ƴ� ��� ��ҵ�.
	if ( !ISLEARNED_SKILL(skill_id) )
        return E_FAIL;

	m_sRunSkill = skill_id;

	GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE NetMsg(wSLOT);
    NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_SKILLQUICK_ACTIVE)-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_LastSkillID,
		"-dw", m_sRunSkill.dwID ); 

	return S_OK;
}

HRESULT GLCharacter::ReqSkillRunReSet ()
{
	m_sRunSkill = NATIVEID_NULL();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_SQS_LastSkillID,
		"-dw", m_sRunSkill.dwID ); 

	return S_OK;
}



HRESULT GLCharacter::ReqGetStorageSpecificItem(DWORD itemType)
{
	GLMSG::SNETPC_REQ_GETSTORAGE_SPECIFIC_ITEM NetMsg;
	NetMsg.dwItemType = itemType;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}


// *****************************************************
// Desc: ������ â�� ���� ��û
// *****************************************************
HRESULT GLCharacter::ReqGetStorage ( DWORD dwChannel, DWORD dwNpcGlobID )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( IsVALID_STORAGE(dwChannel) )
        return S_OK;

    // NPC ��ȿ�� üũ
    if ( !m_bUseStorageCard )
    {
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);

        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	//! ������ ��û.
	//
	GLMSG::SNETPC_REQ_GETSTORAGE NetMsg;
	NetMsg.dwChannel = dwChannel;
	NetMsg.dwNpcGlobID = dwNpcGlobID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

bool GLCharacter::ReqGetStorageForWidget( DWORD dwChannel )
{
	if( S_OK == ReqGetStorage( dwChannel, GLNpcTalkClient::GetInstance().GetNpcGlobalID() ) )
		return true;

	return false;
}


// *****************************************************
// Desc: â�� ������ �鶧, ������, ��ȯ�Ҷ�, ��ĥ��.
// *****************************************************
HRESULT GLCharacter::ReqStorageTo( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( !IsVALID_STORAGE(dwChannel) )
        return S_OK;
	if ( ValidWindowOpen() )
        return E_FAIL;	

    // NPC ��ȿ�� üũ
    if ( !m_bUseStorageCard )
    {
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
        {
            holditem::ReleaseHoldItem();

            return E_FAIL;
        }

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        {
            holditem::ReleaseHoldItem();

            return E_FAIL;
        }
    }

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(wPosX, wPosY);
    if ( !holditem::HaveHoldItem() && !pInvenItem )
    {
        holditem::ReleaseHoldItem();

        return E_FAIL;
    }


	if ( holditem::HaveHoldItem() ) 
	{
        SITEM *pITEM = GLogicData::GetInstance().GetItem ( holditem::GetHoldItem().GetNativeID() );
		if ( !pITEM )
        {
            holditem::ReleaseHoldItem();

            return S_FALSE;
        }
		
		//	�ŷ��ɼ�
		//if ( !pITEM->sBasicOp.IsEXCHANGE() )
		if ( holditem::GetHoldItem().IsNotStorage() )
        {
            holditem::ReleaseHoldItem();

            return S_FALSE;
        }
	}


	bool bKEEP = IsKEEP_STORAGE(dwChannel);

	if ( holditem::HaveHoldItem() && pInvenItem )
	{
/*
#if defined(VN_PARAM) //vietnamtest%%%
		if ( !GET_HOLD_ITEM().bVietnamGainItem  )
#endif
*/
		if (m_ServiceProvider != SP_VIETNAM || (m_ServiceProvider == SP_VIETNAM && ! holditem::GetHoldItem().bVietnamGainItem))
		{
			if ( !bKEEP )
			{
				//! �������� ���� �� ���� ���..
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("STORAGE_SPAN_END") );

                holditem::ReleaseHoldItem();

				return E_FAIL;
			}

            switch ( holditem::WhenHoldItem() )
            {
            case holditem::INVENTORY:
                {
                    GLMSG::SNETPC_REQ_INVEN_EX_STORAGE NetMsg;
                    NetMsg.dwChannel = dwChannel;
                    NetMsg.dwNpcGlobID = dwNPCGlobalID;
                    NetMsg.wPosX = pInvenItem->wPosX;
                    NetMsg.wPosY = pInvenItem->wPosY;
                    NetMsg.wHoldPosX = holditem::GetHoldPosX();
                    NetMsg.wHoldPosY = holditem::GetHoldPosY();
                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::STORAGE:
                {
					DWORD dwHoldChannel, dwHoldNPCGlobalID;
					holditem::GetData( dwHoldChannel, dwHoldNPCGlobalID );

                    if ( dwChannel == dwHoldChannel &&
						wPosX == holditem::GetHoldPosX() &&
                        wPosY == holditem::GetHoldPosY() )
                    {
                        holditem::ReleaseHoldItem();

                        return S_OK;
                    }

					GLMSG::SNETPC_REQ_STORAGE_TO_STORAGE NetMsg;
					NetMsg.dwChannel = dwChannel;
					NetMsg.dwNpcGlobID = dwNPCGlobalID;
					NetMsg.wPosX = wPosX;
					NetMsg.wPosY = wPosY;
					NetMsg.dwHoldChannel = dwHoldChannel;
					NetMsg.dwHoldNpcGlobID = dwHoldNPCGlobalID;
					NetMsg.wHoldPosX = holditem::GetHoldPosX();
					NetMsg.wHoldPosY = holditem::GetHoldPosY();

					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;
            };
		}

        holditem::ReleaseHoldItem();
	}
	else if ( pInvenItem )
	{
        holditem::HoldItem( holditem::STORAGE, pInvenItem->CustomItem(), wPosX, wPosY );
        holditem::SetData( dwChannel, dwNPCGlobalID );
	}
	else if ( holditem::HaveHoldItem() )
	{
/*
#if defined(VN_PARAM) //vietnamtest%%%
		if ( !GET_HOLD_ITEM().bVietnamGainItem  )
#endif
*/
        if (m_ServiceProvider != SP_VIETNAM || (m_ServiceProvider == SP_VIETNAM && !holditem::GetHoldItem().bVietnamGainItem ))
		{
			if ( !bKEEP )
			{
				//! �������� ���� �� ���� ���..
				m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("STORAGE_SPAN_END") );

                holditem::ReleaseHoldItem();

				return E_FAIL;
			}

			//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
			//
			SITEM* pItem = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().GetNativeID() );
			GASSERT(pItem&&"������ �����Ͱ� �������� ����");

			BOOL bOk = m_cStorage[dwChannel].IsInsertable( wPosX, wPosY );
			if ( !bOk )
			{
                holditem::ReleaseHoldItem();
				//	�ι��� ��������.
				return E_FAIL;
			}

            switch ( holditem::WhenHoldItem() )
            {
            case holditem::INVENTORY:
                {
                    GLMSG::SNETPC_REQ_INVEN_TO_STORAGE NetMsg;
                    NetMsg.dwChannel = dwChannel;
                    NetMsg.dwNpcGlobID = dwNPCGlobalID;
                    NetMsg.wPosX = wPosX;
                    NetMsg.wPosY = wPosY;
                    NetMsg.wHoldPosX = holditem::GetHoldPosX();
                    NetMsg.wHoldPosY = holditem::GetHoldPosY();

                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            case holditem::STORAGE:
                {
                    DWORD dwHoldChannel, dwHoldNPCGlobalID;
                    holditem::GetData( dwHoldChannel, dwHoldNPCGlobalID );

                    GLMSG::SNETPC_REQ_STORAGE_TO_STORAGE NetMsg;
                    NetMsg.dwChannel = dwChannel;
                    NetMsg.dwNpcGlobID = dwNPCGlobalID;
                    NetMsg.wPosX = wPosX;
                    NetMsg.wPosY = wPosY;
                    NetMsg.dwHoldChannel = dwHoldChannel;
                    NetMsg.dwHoldNpcGlobID = dwHoldNPCGlobalID;
                    NetMsg.wHoldPosX = holditem::GetHoldPosX();
                    NetMsg.wHoldPosY = holditem::GetHoldPosY();

                    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
                }
                break;

            };
		}

        holditem::ReleaseHoldItem();
	}

	return S_OK;
}


// *****************************************************
// Desc: ��ų ���� ��û. ( â�� ����������. )
// *****************************************************
HRESULT GLCharacter::ReqStorageSkill ( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( !IsVALID_STORAGE(dwChannel) )
        return S_OK;
	if ( ValidWindowOpen() )
        return E_FAIL;	

	// �ʱ�ȭ�� ���� ���¿��� ��ų�� �����Ҽ� ����
	if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
		NSWIDGET_SCRIPT::g_strFunc_StatsReset_IsOpen ) )
	{
		return S_FALSE;
	}

    // NPC ��ȿ�� üũ
    if ( !m_bUseStorageCard )
    {
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_SKILL )
        return E_FAIL;

	SNATIVEID sSKILL_ID = pItem->sSkillBookOp.sSkill_ID;

	if ( ISLEARNED_SKILL(sSKILL_ID) )
	{
		//	�̹� ������ ��ų.
		return E_FAIL;
	}

	EMSKILL_LEARNCHECK emSKILL_LEARNCHECK = CHECKLEARNABLE_SKILL( sSKILL_ID );
	if ( emSKILL_LEARNCHECK != EMSKILL_LEARN_OK )
	{
		//	��ų ���� �䱸 ������ �������� ���մϴ�.
		return E_FAIL;
	}

	//! ��ų ������ ��û�մϴ�.
	//
	GLMSG::SNETPC_REQ_LEARNSKILL_STORAGE NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.dwNpcGlobID = dwNPCGlobalID;
	
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: â�� ������ ����Ҷ� ( ���ñ�, ��ų���� �� ).
// *****************************************************
HRESULT GLCharacter::ReqStorageDrug ( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( !IsVALID_STORAGE(dwChannel) )
        return S_OK;
	if ( ValidWindowOpen() )
        return E_FAIL;	

    // NPC ��ȿ�� üũ
    if ( !m_bUseStorageCard )
    {
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	if ( CheckCoolTime( pInvenItem->sItemCustom.GetNativeID() ) )
        return E_FAIL;


	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
        return E_FAIL;

	// ���� ���� Ȯ��
	if ( !SIMPLE_CHECK_ITEM( pInvenItem->sItemCustom ) )
        return S_FALSE;

	if ( pItem->sBasicOp.emItemType == ITEM_CURE || 
		 pItem->sBasicOp.emItemType == ITEM_EFFECT_RELEASE_CURE )
	{
		//! pk ����� ���θ� ��� �̻��� ��� ȸ������ ����� ���´�.
		//
		DWORD dwPK_LEVEL = GET_PK_LEVEL();
		if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_DRUG_ENABLE_LEVEL )
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_PK_LEVEL") );
			return E_FAIL;
		}

		if ( m_sCONFTING.IsFIGHTING() && !m_sCONFTING.IsRECOVE() )
		{			
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::DISABLE,
                sc::string::format(ID2GAMEINTEXT("CONFRONT_RECOVE"), 0));
			return E_FAIL;
		}

		if ( pItem->sDrugOp.emDrug!=ITEM_DRUG_NUNE )
		{
			switch ( pItem->sDrugOp.emDrug )
			{
			case ITEM_DRUG_HP:
				if ( m_sHP.nMax == m_sHP.nNow )
                    return S_FALSE;
				break;

			case ITEM_DRUG_MP:
				if ( m_sMP.nMax == m_sMP.nNow )
                    return S_FALSE;
				break;

			case ITEM_DRUG_SP:
				if ( m_sSP.nMax == m_sSP.nNow )	
                    return S_FALSE;
				break;

			case ITEM_DRUG_CP:
				if ( m_sCP.wMax == m_sCP.wNow )
                    return S_FALSE;
				break;

			case ITEM_DRUG_HP_MP:
				if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nMax == m_sMP.nNow) )
                    return S_FALSE;
				break;
			case ITEM_DRUG_MP_SP:
				if ( (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nMax == m_sSP.nNow) )
                    return S_FALSE;
				break;

			case ITEM_DRUG_HP_MP_SP:
				if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nMax == m_sSP.nNow) )
                    return S_FALSE;
				break;

			case ITEM_DRUG_CURE:
				if ( !ISSTATEBLOW() )
                    return S_FALSE;
				break;

			case ITEM_DRUG_HP_CURE:
				if ( (m_sHP.nMax == m_sHP.nNow) && !ISSTATEBLOW() )
                    return S_FALSE;
				break;

			case ITEM_DRUG_HP_MP_SP_CURE:
				if ( (m_sHP.nMax == m_sHP.nNow) && (m_sMP.nMax == m_sMP.nNow) && (m_sSP.nMax == m_sSP.nNow) && !ISSTATEBLOW() )
                    return S_FALSE;
				break;
			};

			bool bCheck = true;
            // ü�� ȸ�� �������� ����Ҽ� ���� ��Ȳ�̴�.
            switch(pItem->sDrugOp.emDrug)
            {
            case ITEM_DRUG_HP:
            case ITEM_DRUG_HP_MP:
            case ITEM_DRUG_HP_MP_SP:
            case ITEM_DRUG_HP_CURE:
            case ITEM_DRUG_HP_MP_SP_CURE:
                {
                    if ( IsActionLimit( EMACTION_LIMIT_DRUG_HP ) )
                    {
                        m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_DRUG") );
                        return E_FAIL;
                    }
                }
                break;
			case ITEM_DRUG_EFFECT_RELEASE :
				{
					bCheck = false;
				}
				break;
            };

			//	�������� ����Ҽ� ���� ��Ȳ�̴�.
			if ( IsActionLimit( EMACTION_LIMIT_ITEM ) && bCheck)
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
				return E_FAIL;
			}

			//	�������� ����Ҽ� ���� ��Ȳ�̴�.
			if ( IsActionLimit( EMACTION_LIMIT_DRUG ) && bCheck)
			{
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMACTION_LIMIT_DRUG") );
				return E_FAIL;
			}

			GLMSG::SNETPC_REQ_STORAGEDRUG NetMsg;
			NetMsg.wPosX = wPosX;
			NetMsg.wPosY = wPosY;
			NetMsg.dwNpcGlobID = dwNPCGlobalID;

			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
	else if ( pItem->sBasicOp.emItemType ==ITEM_SKILL )
	{
		ReqStorageSkill ( sNPCID, dwNPCGlobalID, dwChannel, wPosX, wPosY );
	}
	else if ( pItem->sBasicOp.emItemType ==ITEM_PET_SKILL )
	{
		m_pGaeaClient->GetPetClient()->ReqLearnSkill ( dwChannel, wPosX, wPosY );
	}
	
	return S_OK;
}

// *****************************************************
// Desc: â�� �� �ֱ�.
// *****************************************************
HRESULT GLCharacter::ReqStorageSaveMoney ( SNATIVEID sNPCID, DWORD dwNPCGlobalID, LONGLONG lnMoney )
{
//#if !defined(KR_PARAM) && !defined(KRT_PARAM)
//	if (m_lnMoney%UINT_MAX < lnMoney)
//       return E_FAIL;
//#endif
	if (GetInvenMoney() < lnMoney)
        return E_FAIL;
	if (ValidWindowOpen())
        return E_FAIL;	

    if ( !m_bUseStorageCard )
    {
        // NPC ��ȿ�� üũ
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY NetMsg;
	NetMsg.lnMoney = lnMoney;
	NetMsg.dwNpcGlobID = dwNPCGlobalID;
    NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_STORAGE_SAVE_MONEY)-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

void GLCharacter::ReqStorageSaveMoneyForWidget( LONGLONG lnMoney )
{
	ReqStorageSaveMoney( GLNpcTalkClient::GetInstance().GetNpcID(), 
		GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
		lnMoney );
}

// *****************************************************
// Desc: â�� �� ������.
// *****************************************************
HRESULT GLCharacter::ReqStorageDrawMoney ( SNATIVEID sNPCID, DWORD dwNPCGlobalID, LONGLONG lnMoney )
{
//#if !defined(KR_PARAM) && !defined(KRT_PARAM)
//	if (m_lnStorageMoney%UINT_MAX < lnMoney)
//      return E_FAIL;
//#endif
	if (GetStorageMoney() < lnMoney)
        return E_FAIL;
	if (ValidWindowOpen())
        return E_FAIL;	

    if ( !m_bUseStorageCard )
    {
        // NPC ��ȿ�� üũ
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY NetMsg;
	NetMsg.lnMoney = lnMoney;
	NetMsg.dwNpcGlobID = dwNPCGlobalID;
    NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_STORAGE_DRAW_MONEY)-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	return S_OK;
}

void GLCharacter::ReqStorageDrawMoneyForWidget( LONGLONG lnMoney )
{
	ReqStorageDrawMoney( GLNpcTalkClient::GetInstance().GetNpcID(), 
		GLNpcTalkClient::GetInstance().GetNpcGlobalID(),
		lnMoney );
}

//! �� �ٴڿ� ����.
HRESULT GLCharacter::ReqMoneyToField ( LONGLONG lnMoney )
{
	if (!GLCONST_CHAR::bMONEY_DROP2FIELD)
        return E_FAIL;
	if (lnMoney == 0)
        return E_FAIL;
//#if !defined(KR_PARAM) && !defined(KRT_PARAM)
//	if (m_lnMoney%UINT_MAX < lnMoney)
//      return E_FAIL;
//#endif
	if (GetInvenMoney() < lnMoney)
        return E_FAIL;
	if (ValidWindowOpen())
        return E_FAIL;	

	D3DXVECTOR3 vDir(1,0,0);
	BOOL bCollision = FALSE;
	DWORD dwCollisionID;
	D3DXVECTOR3 vCollisionPos(0,0,0);

	//srand(GetTickCount());
	float fBaseY = (rand()%32) * (D3DX_PI*2)/32.0f;

	for ( float fRotY = 0.0f; fRotY<D3DX_PI*2; fRotY += 0.2f )
	{
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY ( &matRotY, fRotY+fBaseY );
		D3DXVec3TransformCoord ( &vDir, &vDir, &matRotY );

		D3DXVec3Normalize ( &vDir, &vDir );
		D3DXVECTOR3 vDropPos = GetPosition() + vDir*float(GLCONST_CHAR::wBODYRADIUS+12);

		m_pGaeaClient->GetActiveMap()->GetNaviMesh()->IsCollision
		(
			vDropPos + D3DXVECTOR3(0,+5,0),
			vDropPos + D3DXVECTOR3(0,-5,0),
			vCollisionPos,
			dwCollisionID,
			bCollision
		);
	}

	if ( !bCollision )
	{
		m_pGaeaClient->GetActiveMap()->GetNaviMesh()->IsCollision
		(
			GetPosition() + D3DXVECTOR3(0,+5,0),
			GetPosition() + D3DXVECTOR3(0,-5,0),
			vCollisionPos,
			dwCollisionID,
			bCollision
		);
	}

	if ( bCollision )
	{
		GLMSG::SNETPC_REQ_MONEY_TO_FIELD NetMsg;
		NetMsg.vPos = vCollisionPos;
		NetMsg.lnMoney = lnMoney;
        //NetMsg.Crc32 = sc::string::getCrc32(&NetMsg, sizeof(GLMSG::SNETPC_REQ_MONEY_TO_FIELD)-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}

	return S_OK;
}

//! �ŷ��� �ݾ� �ֱ�.
//HRESULT GLCharacter::ReqTradeMoney ( LONGLONG lnMoney )
//{
//	if ( !m_pGaeaClient->GetTradeClient()->Valid () )	return E_FAIL;
//
//	//! ���� �ݾ��� ������� �˻�.
//	//
//	if ( lnMoney > m_lnMoney )		return E_FAIL;
//	
//	//! ��ȯ�� �ݾ� ���� MSG.
//	//
//	GLMSG::SNET_TRADE_MONEY NetMsg;
//	NetMsg.lnMoney = lnMoney;
//	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
//
//	return S_OK;
//}

//! �ŷ��� ������ �ֱ�/����/��ȯ.
HRESULT GLCharacter::ReqTradeBoxTo( WORD wPosX, WORD wPosY )
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid() )
        return E_FAIL;

	SINVENITEM* pTradeItem = m_pGaeaClient->GetTradeClient()->FindPosItem(wPosX, wPosY);	//	���� ��ġ�� Ʈ���̵� ��ǰ�� ��ϵǾ� ����.
	SINVEN_POS sPreTradeItem = m_pGaeaClient->GetTradeClient()->GetPreItem();					//	���� ��� �������� ����.

	//! �ش� ��ġ�� �̹� ��ϵ� �������� �ְ� ����ϰ��� �ϴ� �������� ���� ���.
	//
	if ( sPreTradeItem.VALID() && pTradeItem )
	{
		//	����ϰ��� �ϴ� ������.
		SINVENITEM* pResistItem = m_cInventory.GetItem( sPreTradeItem.wPosX, sPreTradeItem.wPosY );
		if ( !pResistItem )	
            return E_FAIL;

		//	�̹� ��ϵǾ� �ִ��� �˻�.
		SINVENITEM* pAlready = m_pGaeaClient->GetTradeClient()->FindItemBack( pResistItem->wBackX, pResistItem->wBackY );
		if ( pAlready )
		{
			m_pGaeaClient->GetTradeClient()->ReSetPreItem();
			return E_FAIL;
		}

		SITEM *pItem = GLogicData::GetInstance().GetItem( pResistItem->sItemCustom.GetNativeID() );
		if ( !pItem )
			return E_FAIL;

		//	�ŷ��ɼ�
		//if ( !pItem->sBasicOp.IsEXCHANGE() )
		if ( !( pResistItem->sItemCustom.IsExchange() != NULL ) )
			return E_FAIL;

		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			// ����ִ� �������� ��ī���̸� ���� Ȱ��ȭ �Ǿ� ������
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
			if ( pMyPet->IsVALID() && pResistItem->sItemCustom.PetDbNum() == pMyPet->DbNum() )
			{
				return E_FAIL;
			}
		}

		if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && pResistItem->sItemCustom.m_VehicleDbNum != 0 )
		{
			return E_FAIL;
		}

		//! ���� �����۰� ���ο� �������� ��ȯ�Ͽ� ��ġ�� ��� �� �� �ִ��� �˻�.
		//
		BOOL bOk = m_pGaeaClient->GetTradeClient()->IsExInsertable( wPosX, wPosY );
		if ( !bOk )
			return FALSE;

		//! ���� �������� ����Ʈ���� ����.
		//
		GLMSG::SNET_TRADE_ITEM_REMOVE NetMsgReMove;
		NetMsgReMove.wPosX = pTradeItem->wPosX;
		NetMsgReMove.wPosY = pTradeItem->wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsgReMove);

		//! ���ο� �������� ����Ʈ�� ���.
		GLMSG::SNET_TRADE_ITEM_REGIST NetMsgRegist;
		NetMsgRegist.wPosX = wPosX;
		NetMsgRegist.wPosY = wPosY;
		NetMsgRegist.wInvenX = sPreTradeItem.wPosX;
		NetMsgRegist.wInvenY = sPreTradeItem.wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsgRegist);

		m_pGaeaClient->GetTradeClient()->ReSetPreItem();
	}
	//! ����� �������� �ִ� ���.
	//
	else if ( sPreTradeItem.VALID() )
	{
		//	����ϰ��� �ϴ� ������.
		SINVENITEM* pResistItem = m_cInventory.GetItem( sPreTradeItem.wPosX, sPreTradeItem.wPosY );
		if ( !pResistItem )	
            return E_FAIL;

		//	�̹� ��ϵǾ� �ִ��� �˻�.
		SINVENITEM* pAlready = m_pGaeaClient->GetTradeClient()->FindItemBack( sPreTradeItem.wPosX, sPreTradeItem.wPosY );
		if ( pAlready )
		{
			m_pGaeaClient->GetTradeClient()->ReSetPreItem();
			return E_FAIL;
		}

		//! �ش���ġ�� ��� �� �� �ִ��� �˻�.
		//
		SITEM *pItem = GLogicData::GetInstance().GetItem( pResistItem->sItemCustom.GetNativeID() );
		if ( !pItem )
			return E_FAIL;

		//	�ŷ��ɼ�
		//if ( !pItem->sBasicOp.IsEXCHANGE() )
		if ( !( pResistItem->sItemCustom.IsExchange() != NULL ) )
			return E_FAIL;

		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			// ����ִ� �������� ��ī���̸� ���� Ȱ��ȭ �Ǿ� ������
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient();
			if ( pMyPet->IsVALID () && pResistItem->sItemCustom.PetDbNum() == pMyPet->DbNum() )
			{
				return E_FAIL;
			}
		}

		if ( pItem->sBasicOp.emItemType == ITEM_VEHICLE && pResistItem->sItemCustom.m_VehicleDbNum != 0 )
		{
			return E_FAIL;
		}

		BOOL bOk = m_pGaeaClient->GetTradeClient()->IsInsertable( wPosX, wPosY );
		if ( !bOk )
			return E_FAIL;

		//! ���ο� �������� ����Ʈ�� ���.
		GLMSG::SNET_TRADE_ITEM_REGIST NetMsgRegist;
		NetMsgRegist.wPosX = wPosX;
		NetMsgRegist.wPosY = wPosY;
		NetMsgRegist.wInvenX = sPreTradeItem.wPosX;
		NetMsgRegist.wInvenY = sPreTradeItem.wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsgRegist);

		m_pGaeaClient->GetTradeClient()->ReSetPreItem();
	}
	//! ����Ʈ���� ������ �������� �ִ°��.
	//
	else if ( pTradeItem )
	{
		GLMSG::SNET_TRADE_ITEM_REMOVE NetMsgReMove;
		NetMsgReMove.wPosX = pTradeItem->wPosX;
		NetMsgReMove.wPosY = pTradeItem->wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsgReMove);
	}
	
	return S_OK;
}

// �ŷ� ���. by luxes.
HRESULT GLCharacter::ReqTradeLock()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
        return E_FAIL;
	if ( m_pGaeaClient->GetTradeClient()->GetMyTrade().IsLock() )
        return E_FAIL; // ��� �ִٸ� �� ����� ����.


	GLMSG::SNET_TRADE_LOCK NetMsg;
	NetMsg.dwGaeaID = m_dwGaeaID;
    NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}
/*********************************INCLUDE_SCALEFORM �۾� ������ ����*********************************/
//! �ŷ� ����.
HRESULT GLCharacter::ReqTradeAgree ()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
        return E_FAIL;

	if ( !m_pGaeaClient->GetTradeClient()->IsAgreeAble() )
	{
		//! ������ �ŷ� ������ �� �� �����ϴ�. X`s �ľ� �ٽ� �õ� �Ͽ� �ֽʽÿ�.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_AGREE_TIME") );
		return E_FAIL;
	}

	//! �ŷ��� ���� �����۵��� ������ ������ �ִ��� �˻�.
	//
	if ( !IsVaildTradeInvenSpace () )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("TRADE_AGREE_FAIL_MYINVEN") );
		return E_FAIL;
	}

	GLMSG::SNET_TRADE_AGREE NetMsg;
    NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	
	return S_OK;
}

//! �ŷ� ���.
HRESULT GLCharacter::ReqTradeCancel ()
{
	if ( !m_pGaeaClient->GetTradeClient()->Valid () )
        return E_FAIL;
	
	GLMSG::SNET_TRADE_CANCEL NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}
/*************************************�̱���***********************************************/
//! ��Ȱ ��ġ ���� ��û.
HRESULT GLCharacter::ReqReGenGate ( DWORD dwNpcGlobID )
{
    GLMSG::SNETPC_REQ_REGEN_GATE NetMsg(dwNpcGlobID);

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
	if (!spCrow)
        goto _REQ_FAIL;

    // NPC ��ȿ�� üũ
    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        goto _REQ_FAIL;

	//! ��Ȱ��ġ ���� ��û.
	//

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;

_REQ_FAIL:
	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_GATE_FAIL") );
	return E_FAIL;
}

// *****************************************************
// Desc: ��Ȱ�ϱ�
// *****************************************************
HRESULT GLCharacter::ReqResurrect(const EMRESURRECTION_TYPE _resurrectType)
{
    //EMSERVICE_PROVIDER ServiceProvider = m_pGaeaClient->GetServiceProvider();

    if ( !isAction(GLAT_DIE) )
        return E_FAIL;

    //	�����¿� ��.
    m_dwWAIT = 0;
    SetActState(EM_ACT_DIE);

    if ( (_resurrectType == EM_RESURRECTION_SPECIAL) && (m_pGaeaClient->CanRecoveryExp() == false) )
        return ReqResurrect(EM_RESURRECTION_NOMAL);

    if ( (_resurrectType == EM_RESURRECTION_SPECIAL_ITEM) && (FindRevive() == NULL) )
        return ReqResurrect(EM_RESURRECTION_NOMAL);

    GLMSG::NET_CHAR_REQ_RESURRECT_CF NetMsg(_resurrectType);
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	//m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	// ��Ʋ�ξ� �̺�Ʈ�϶� �������� ������ ����
	if ( GLCONST_CHAR::bBATTLEROYAL )
	{
		ReSetActState(EM_REQ_VISIBLEOFF);

		//! (������Ʈ����) �޼��� �߻�.
		//
		GLMSG::SNETPC_ACTSTATE NetMsg(GetActState());
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
    return S_OK;
}

// ����ġ ������ ��û�Ѵ� Npc
HRESULT GLCharacter::ReqRecoveryExpNpc ( DWORD dwNpcGlobID )
{    
    // ����ġȸ��_����_Npc
    if (m_pGaeaClient->CanRecoveryExpNpc())
    {
        // NPC ��ȿ�� üũ
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;

        GLMSG::SNETPC_REQ_RECOVERY_NPC NetMsg;
        NetMsg.dwNpcGlobID = dwNpcGlobID;
        m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
    return S_OK;
}

HRESULT GLCharacter::ReqGetReExpNpc ( DWORD dwNpcGlobID )
{    
    // ����ġȸ��_����_Npc
    if ( m_pGaeaClient->CanRecoveryExpNpc() )
    {
        // ������ ����ġ�� ��û
	    GLMSG::SNETPC_REQ_GETEXP_RECOVERY_NPC NetMsg;
	    NetMsg.dwNpcGlobID = dwNpcGlobID;
	    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    }
	return S_OK;
}

HRESULT GLCharacter::ReqCure ( DWORD dwNpcGlobID, DWORD dwTalkID )
{
	GLMSG::SNETPC_REQ_CURE NetMsg(dwTalkID, dwNpcGlobID);

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
	if (!spCrow)
        goto _REQ_FAIL;

    // NPC ��ȿ�� üũ
    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        goto _REQ_FAIL;

	//! ġ�� ��û.
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;

_REQ_FAIL:
	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CURE_FAIL") );

	return S_OK;
}

//! ĳ���� ���� ����. ( stats, skill )
//
HRESULT GLCharacter::ReqCharReset ( DWORD dwNpcGlobID )
{
	EMREGEN_CHARRESET_FB emFB = EMREGEN_CHARRESET_FAIL;
	WORD wPosX(0), wPosY(0);
	GLMSG::SNETPC_REQ_CHARRESET NetMsg( dwNpcGlobID );

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
	if (!spCrow)
        goto _REQ_FAIL;

    // NPC ��ȿ�� üũ
    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        goto _REQ_FAIL;

	bool bITEM = m_cInventory.GetCharResetItem(wPosX, wPosY);
	if ( !bITEM )
	{
		emFB = EMREGEN_CHARRESET_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	//! ĳ���� ���� ����.
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;

_REQ_FAIL:
	
	switch ( emFB )
	{
	case EMREGEN_CHARRESET_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CHARRESET_FAIL") );
		break;

	case EMREGEN_CHARRESET_ITEM_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREGEN_CHARRESET_ITEM_FAIL") );
		break;
	};

	return S_OK;
}

//! NPC�� item�� ��ȯ A:npc���� �ִ°͵�, b:npc���Լ� �޴°�.
HRESULT GLCharacter::ReqItemTrade(DWORD dwNpcGlobID, DWORD dwTalkID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk)
{

	EMNPC_ITEM_TRADE_FB emFB = EMNPC_ITEM_TRADE_FAIL;
	WORD wPosX(0), wPosY(0);
	GLMSG::SNETPC_REQ_NPC_ITEM_TRADE NetMsg(dwNpcGlobID, dwTalkID);
	GLInventory cInventory;

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
	if (!spCrow)
        goto _REQ_FAIL;

    // ���� NPC ��ȿ�� üũ
    if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        goto _REQ_FAIL;

	if ( !spNpcTalk )									
        goto _REQ_FAIL;

	if ( !spNpcTalk->m_vecNeedItem.size() )
	{
		emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
		goto _REQ_FAIL;
	}

	SNpcTalk::VEC_ITEM_NUM& vecNeedItem = spNpcTalk->m_vecNeedItem;
	SNpcTalk::VEC_ITEM_NUM& vecTradeItem = spNpcTalk->m_vecTradeItem;

	// Need Add New Item Con.
	BOOL bOK[MAX_NEEDITEM_COUNT] = { TRUE, TRUE, TRUE, TRUE, TRUE };

	for ( int i = 0; i < (int)vecNeedItem.size(); ++i ) 
	{
		bOK[i] = ISHAVEITEM ( vecNeedItem[i].sNativeID, vecNeedItem[i].dwNum );
	}

	for ( int i = 0; i < MAX_NEEDITEM_COUNT; ++i ) 
	{
		if ( !bOK[i] ) 
		{
			emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
			goto _REQ_FAIL;
		}
	}

	//	������� �����ϰ� ������ �ִ��� üũ�Ѵ�.
	//	���� ����� ��������� Ŭ���̾�Ʈ���� �ѹ��� üũ�ϴ°� ������ ����.
	cInventory.Assign( m_cInventory );

    //	�ϴ� �ʿ��Ѿ������� ã�Ƽ� �κ��丮���� ���� �Ѵ�.
	for ( int i = 0; i < (int)vecNeedItem.size(); ++i ) 
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( vecNeedItem[i].sNativeID );
		if ( !pItem ) goto _REQ_FAIL; 

		int nNum = vecNeedItem[i].dwNum * pItem->GETAPPLYNUM();
		bool bCheck = true;
		while ( bCheck )
		{
			SINVENITEM* pInvenItem = cInventory.FindItem ( vecNeedItem[i].sNativeID );
			if ( !pInvenItem )
			{
				emFB = EMNPC_ITEM_TRADE_ITEM_FAIL;
				goto _REQ_FAIL;
			}

			if ( nNum < pInvenItem->sItemCustom.wTurnNum ) 
			{
				pInvenItem->sItemCustom.wTurnNum -= nNum;
				nNum = 0;
			}
			else
			{
				nNum -= pInvenItem->sItemCustom.wTurnNum;
				cInventory.DeleteItem( pInvenItem->wPosX, pInvenItem->wPosY );
			}

			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_Inventory_InvenSlotUpdate,
				"-n, -n", pInvenItem->wPosX, pInvenItem->wPosY );

			if ( nNum <= 0 )
                bCheck = false;
		}
	}

	//	���� �������� �־��. �κ��丮�� ������...
	for ( int i = 0; i < (int)vecTradeItem.size(); ++i ) 
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem( vecTradeItem[i].sNativeID );
		if ( !pItem )
            goto _REQ_FAIL; 
		
		if ( pItem->ISPILE() ) 
		{
			//	��ħ �������� ���.
			SITEMCUSTOM sITEM_NEW(pItem->sBasicOp.sNativeID);
			sITEM_NEW.wTurnNum = (WORD)(vecTradeItem[i].dwNum * pItem->GETAPPLYNUM());	

			//	�κ��� ���������� ���� ��� ��ҵ�.
			if ( !cInventory.InsertItemEx ( sITEM_NEW ) )
			{
				emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
				goto _REQ_FAIL; 
			}

		}
		else
		{
			for ( int j = 0; j < (int)vecTradeItem[i].dwNum; ++j ) 
			{
				SITEMCUSTOM sITEM_NEW(pItem->sBasicOp.sNativeID); 
				if ( !cInventory.InsertItem ( sITEM_NEW ) ) 
				{
					emFB = EMNPC_ITEM_TRADE_INSERT_FAIL;
					goto _REQ_FAIL; 
				}				
			}			
		}
	}

	//! ������ ��ȯ ��û.
	//
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;

_REQ_FAIL:
	switch ( emFB )
	{
	case EMNPC_ITEM_TRADE_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_FAIL") );
		break;
	case EMNPC_ITEM_TRADE_ITEM_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_ITEM_FAIL") );
		break;
	case EMNPC_ITEM_TRADE_INSERT_FAIL:
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMNPC_ITEM_TRADE_INSERT_FAIL") );
		break;

	};

	return S_OK;
}

HRESULT GLCharacter::ReqHireSummon ( DWORD dwNpcGlobID, DWORD dwTalkID, std::tr1::shared_ptr<SNpcTalk> spNpcTalk )
{
	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr< GLCrowClient > spCrow =
		pLandMan->GetCrow( dwNpcGlobID );
	if ( !spCrow )
		return S_OK;

	// ���� NPC ��ȿ�� üũ;
	if ( !spCrow ||
		!spCrow->IsNpcActionable( GetPosition(), GETBODYRADIUS() ) )
		return S_OK;

	if ( !spNpcTalk )						
		return S_OK;

	SNATIVEID sHireSummon = spNpcTalk->m_sHireSummon;
	if ( sHireSummon.Id() == SNATIVEID::ID_NULL )
		return S_OK;

	// ��� ��ȯ�� ����;
	m_pGaeaClient->ChangeBasicHireSummon( sHireSummon.Mid(), sHireSummon.Sid() );

	return S_OK;
}

// *****************************************************
// Desc: ģ������ �̵�(ģ���̵�ī��)
// *****************************************************
HRESULT	GLCharacter::Req2Friend ( const char *szNAME )
{
	std::tr1::shared_ptr<SFRIEND> pFRIEND = m_pGaeaClient->GetFriendClient()->GetData(szNAME);
	if ( !pFRIEND )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EM2FRIEND_FB_FAIL") );
		return S_FALSE;
	}

	SINVENITEM *pINVENITEM = m_cInventory.FindItem ( ITEM_2FRIEND );
	if ( !pINVENITEM )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EM2FRIEND_FB_NO_ITEM") );
		return S_FALSE;
	}

	SITEM* pVehicleItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
	if ( pVehicleItem )
	{
        if (m_emVehicle == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem(pVehicleItem->VehicleType()))
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_BIKE") );
			return S_FALSE;                        
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_RECALL_FB_BIKE") );
			return S_FALSE;
		}
	}

	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
		return E_FAIL;
	}

	PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();
	if ( !pLandManClient )
		return E_FAIL;

	const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID() );
	if ( !pMapNode )
		return E_FAIL;

	if( PRISONLIMIT.IsLimitStatus(pMapNode, UserLevel(), PML_WARP) )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("WARF_PRISION_LIMIT") );
		return E_FAIL;
	}

	//! ģ������ ���� ��û.
	GLMSG::SNETPC_2_FRIEND_REQ NetMsg;
	StringCchCopy(NetMsg.szFRIEND_NAME, CHAR_SZNAME, pFRIEND->m_ChaName.c_str());
	NetMsg.wItemPosX = pINVENITEM->wPosX;
	NetMsg.wItemPosY = pINVENITEM->wPosY;
	NetMsg.dwItemID = pINVENITEM->Id();
	//NetMsg.dwGaeaID = m_dwGaeaID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

bool GLCharacter::IsInvenSplitItem ( WORD wPosX, WORD wPosY, bool bVietnamInven )
{
	SINVENITEM* pInvenItem = NULL;
	if ( bVietnamInven )
	{
		pInvenItem = m_cVietnamInventory.FindPosItem(wPosX, wPosY);
	}
    else
    {
		pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	}
	if ( !pInvenItem )
        return false;

	//! ������ ���� ��������.
	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	GASSERT(pItem&&"������ �����Ͱ� �������� ����");

	bool bSPLIT(false);
	bSPLIT = ( pItem->ISINSTANCE() );
	if ( !bSPLIT )	
        return FALSE;
	bSPLIT = ( pInvenItem->sItemCustom.wTurnNum>1 );

	return bSPLIT;
}

bool GLCharacter::IsInvenDecomposeItem( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return false;

	//! ������ ���� ��������.
	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	GASSERT(pItem&&"������ �����Ͱ� �������� ����");

	if ( !pItem || !pInvenItem->CustomItem().IsDecompose() )
		return false;

	return true;
}

bool GLCharacter::IsStorageSplitItem ( DWORD dwChannel, WORD wPosX, WORD wPosY )
{
	GASSERT(EMSTORAGE_CHANNEL>dwChannel);
	if ( EMSTORAGE_CHANNEL<=dwChannel )
        return false;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return false;

	//! ������ ���� ��������.
	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	GASSERT(pItem&&"������ �����Ͱ� �������� ����");

	bool bSPLIT(false);
	bSPLIT = ( pItem->ISPILE() );
	if ( !bSPLIT )	
        return FALSE;
	bSPLIT = ( pInvenItem->sItemCustom.wTurnNum>1 );

	return bSPLIT;
}

//! �κ��丮 - ��ħ ������ �и�.
HRESULT GLCharacter::ReqInvenSplit ( WORD wPosX, WORD wPosY, WORD wSplitNum )
{
	if ( !IsInvenSplitItem(wPosX,wPosY) )
        return E_FAIL;
	
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	if ( pInvenItem->sItemCustom.wTurnNum <= wSplitNum )
	{
		return E_FAIL;
	}

	//! ������ �и� �޽��� ����.
	GLMSG::SNETPC_REQ_INVEN_SPLIT NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wSplit = wSplitNum;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: â�� - ��ħ ������ �и�.
// *****************************************************
HRESULT GLCharacter::ReqStorageSplit ( SNATIVEID sNPCID, DWORD dwNPCGlobalID, DWORD dwChannel, WORD wPosX, WORD wPosY, WORD wSplitNum )
{
	if ( !IsStorageSplitItem(dwChannel,wPosX,wPosY) )
        return E_FAIL;

	SINVENITEM* pInvenItem = m_cStorage[dwChannel].FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return false;
	
	if ( pInvenItem->sItemCustom.wTurnNum <= wSplitNum )
	{
		return E_FAIL;
	}

    if ( !m_bUseStorageCard )
    {
        // NPC ��ȿ�� üũ
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	//! ������ �и� �޽��� ����.
	GLMSG::SNETPC_REQ_STORAGE_SPLIT NetMsg;
	NetMsg.dwChannel = dwChannel;
	NetMsg.dwNpcGlobID = dwNPCGlobalID;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.wSplit = wSplitNum;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

//! ��� ��û.
//
HRESULT GLCharacter::ReqConflict ( DWORD dwID, const SCONFT_OPTION &sOption )
{
	if ( !IsValidBody() )
        return E_FAIL;
	if ( IsPostBoxWindowOpen() )
        return S_FALSE; 

	std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar ( dwID );
	if ( !pChar )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_FAIL") );
		return E_FAIL;
	}

	if ( m_pGaeaClient->GetActiveMap()->IsPeaceZone() )
	{
		//! ��ȭ ���������� ����� �Ұ����մϴ�.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PEACE") );
		return E_FAIL;
	}

	if ( !sOption.VALID_OPT() )	
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_OPTION") );
		return E_FAIL;
	}

	if ( m_sCONFTING.IsCONFRONTING() )
	{
		//! �̹� ������̿��� ��� ��û�� �Ұ����մϴ�.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_ALREADY_ME") );
		return E_FAIL;
	}

	if ( m_emVehicle || pChar->VehicleState() )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_BIKE_FAIL") );
		return E_FAIL;
	}
		
	if ( m_pGaeaClient->GetMyPartyClient()->isMember(dwID) )
	{
		//! ���� �Ҽ��� ��Ƽ���̿��� ��� �Ұ����մϴ�.
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_PARTY") );
		return E_FAIL;
	}

	if ( GetPartyID().isValidExpedition() || pChar->GetPartyID().isValidExpedition() )
	{
		//! ������ �߿� ����� �Ұ��� �մϴ�;
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_EXPEDITION") );
		return E_FAIL;
	}

    const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( m_pGaeaClient->GetActiveMapID().getBaseMapID() );
    if ( pMapNode && !pMapNode->IsSparring() )	
    {
        //! �� ���������� ����� �Ұ��� �մϴ�.
        m_pGaeaClient->PrintMsgTextDlg ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCONFRONT_NONCONFRONTMODE") );
        return E_FAIL;
    }

	//! Ŭ�� ����� ���.
	if ( IsClubMaster() && pChar->IsClubMaster() )
	{
		GLMSG::SNETPC_REQ_CONFRONT NetMsg(EMCONFT_GUILD,dwID,sOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	//! ��Ƽ ����� ���.
	else if ( IsPartyMaster() && pChar->IsPartyMaster() )
	{
		GLMSG::SNETPC_REQ_CONFRONT NetMsg(EMCONFT_PARTY,dwID,sOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	//! ���� ����� ���.
	else
	{
		if ( IsPartyMem() )
		{
			//! ��Ƽ�常 '��Ƽ���'�� ��û�� �� �ֽ��ϴ�.
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCONFRONT_MENOTMASTER") );
			return E_FAIL;
		}

		GLMSG::SNETPC_REQ_CONFRONT NetMsg(EMCONFT_ONE,dwID,sOption);
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}

	return S_OK;
}

void GLCharacter::ReqPickupNearItem()
{
	BOOL bMove = FALSE;
	D3DXVECTOR3 vMoveTo;

	SACTIONTAR sACTAR = FindNearItem ();
	STARGETID sTARID = sACTAR.sTARID;

	if ( sACTAR.emACTAR==EMACTAR_ITEM )
	{
		GLREACTION_ITEM sReaction;
		sReaction.SetTARID(sTARID);
		sReaction.SetPOS(GetActionID().vPos);
		sReaction.SETPRINTFAILED(true);
		ItemReaction(sReaction);
	}
}

void GLCharacter::ReqPickupItem( struct CROWREN* pDispName )
{
	if( NULL == pDispName )
		return;

	if( CROW_ITEM != pDispName->m_emCROW && CROW_MONEY != pDispName->m_emCROW )
		return;

	STARGETID sTargetID( pDispName->m_emCROW, pDispName->m_dwID );
	
	GLREACTION_ITEM sReaction;
	sReaction.SetTARID( sTargetID );
	sReaction.SetPOS( pDispName->m_vPOS );
	sReaction.SETPRINTFAILED( true );
	sReaction.SetManualFind( true );
	ItemReaction( sReaction );
}

void GLCharacter::ReqPartyMemberSkillReaction( DWORD dwGaeaID )
{
	GLPartyClient* pPartyClient = m_pGaeaClient->GetMyPartyClient();
	if ( pPartyClient == NULL )
		return;

	if ( pPartyClient->GetMemberIndex(dwGaeaID) == DEFSLOT_NULL )
		return;

	STARGETID sTargetID(CROW_PC, dwGaeaID);
	m_pGaeaClient->GetCharacter()->ReqSkillReaction(sTargetID);
}

void GLCharacter::ReqSkillReaction ( STARGETID sTID )
{
	if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
		return;

	m_ReservationAction.Clear();

	BOOL bMove(FALSE);
	D3DXVECTOR3 vMoveTo;

	PGLSKILL pRunSkill = GLSkillMan::GetInstance().GetData ( m_sRunSkill );
	if ( !pRunSkill )
		return;
	if ( pRunSkill->m_sBASIC.emIMPACT_SIDE == EMIMPACT_SIDE_ENEMY )
		return;

	SetDefenseSkill( false );

	ClientActor* pCOPY = m_pGaeaClient->GetCopyActor(sTID);
	if ( !pCOPY )
        return;

	sTID.vPos = pCOPY->GetPosition();

	GLREACTION_SKILL sReaction;
	sReaction.SetSKILLID(m_sRunSkill);
	sReaction.SetTARID(sTID);
	sReaction.SetPOS(GetActionID().vPos);
	sReaction.SETPRINTFAILED(true);
	SkillReaction( sReaction );
}

HRESULT GLCharacter::ReqQuestStart ( DWORD dwNpcGlobID, DWORD dwTalkID, DWORD dwQUESTID )
{
    // NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
    std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
    if (!spCrow)
        return E_FAIL;

    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        return E_FAIL;

	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
		return S_FALSE;
	}

	// QQQ : Quest;
	const DWORD nPartyMember = m_pGaeaClient->GetPartyManager()->GetNMember();
	const DWORD nMember = nPartyMember ? nPartyMember : 1;
	if ( pQUEST->m_dwBeginPartyMemNum && (pQUEST->m_dwBeginPartyMemNum > nMember) )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, sc::string::format(ID2GAMEINTEXT("EMQUEST_START_FB_MINIMUM_PLAYER"), pQUEST->m_dwBeginPartyMemNum));
		return S_FALSE;
	}

	if ( pQUEST->m_dwLimitPartyMemNum && (pQUEST->m_dwLimitPartyMemNum < nMember) )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, sc::string::format(ID2GAMEINTEXT("EMQUEST_START_FB_MAXIMUM_PLAYER"), pQUEST->m_dwLimitPartyMemNum));
		return S_FALSE;
	}
	

	//	�̹� �������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( pQUEST_PROG )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
		return S_FALSE;
	}

	//	�̹� �Ϸ��� ����Ʈ���� �˻�.
	GLQUESTPROG* pQUEST_END = m_cQuestPlay.FindEnd ( dwQUESTID );
	if ( pQUEST_END )
	{
		if ( !pQUEST_END->m_bCOMPLETE )
		{
			//	����(����)�� ����Ʈ�� �ٽ� �õ� �Ұ����� ���.
			if ( !pQUEST->IsAGAIN() )
			{
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONAGAIN") );
				return S_FALSE;
			}
		}
		else
		{
			//	������ �õ� �������� �˻�.
			if ( !pQUEST->IsREPEAT() )
			{
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
				return S_FALSE;
			}

			if ( pQUEST->IsDaily() )
			{
				CTime cTime = CTime::GetCurrentTime();
				CTime cTimeEnd(pQUEST_END->m_tEndTime);

				if (cTime.GetDay() == cTimeEnd.GetDay() &&
					cTime.GetMonth() == cTimeEnd.GetMonth() &&
					cTime.GetYear() == cTimeEnd.GetYear() )
				{
					m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_DONREPEAT") );
					return S_FALSE;
				}
			}
		}
	}

	if ( GetInvenMoney() < pQUEST->m_dwBeginMoney )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_NEEDMONEY") );
		return S_FALSE;
	}

	//! quest ���� ������ ��û.
	//
	GLMSG::SNETPC_REQ_QUEST_START NetMsg(dwNpcGlobID, dwTalkID, dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqQuestStepNpcTalk ( DWORD dwNpcGlobID, DWORD dwTalkID, DWORD dwQUESTID, DWORD dwQUESTSTEP )
{
    // NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
    std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
    if (!spCrow)
        return E_FAIL;

    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        return E_FAIL;

	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_FAIL") );
		return S_FALSE;
	}

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMQUEST_START_FB_ALREADY") );
		return S_FALSE;
	}

	//! quest ���� ������ ��û.
	//
	GLMSG::SNET_QUEST_STEP_NPC_TALK NetMsg(dwNpcGlobID, dwTalkID, dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqQuestGiveUp ( DWORD dwQUESTID )
{
	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )
        return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )	
        return S_FALSE;

	if ( !pQUEST->IsGIVEUP() )
        return S_FALSE;

	//! quest ���� ������ ��û.
	//
	GLMSG::SNET_QUEST_PROG_GIVEUP NetMsg(dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqQuestREADINGReset ( DWORD dwQUESTID )
{
	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )	
        return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )	
        return S_FALSE;

	if ( !pQUEST_PROG->IsReqREADING() )
        return S_FALSE;
	pQUEST_PROG->ResetReqREADING ();

	//! quest �о����� ������ �˸�.
	//
	GLMSG::SNETPC_QUEST_PROG_READ NetMsg(dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqQuestComplete ( DWORD dwQUESTID )
{
	//! ����Ʈ ������ ���� �˻�.
	//
	GLQUEST* pQUEST = GLQuestMan::GetInstance().Find ( dwQUESTID );
	if ( !pQUEST )	
        return S_FALSE;

	//	�������� ����Ʈ ���� ����.
	GLQUESTPROG* pQUEST_PROG = m_cQuestPlay.FindProc ( dwQUESTID );
	if ( !pQUEST_PROG )	
        return S_FALSE;

	if ( !pQUEST_PROG->CheckCOMPLETE() )
        return S_FALSE;

	//! quest �Ϸ� ��û.
	//
	GLMSG::SNETPC_REQ_QUEST_COMPLETE NetMsg(dwQUESTID);
	m_pGaeaClient->NETSENDTOFIELD (&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqBusStation ( DWORD dwNpcGlobID, DWORD dwSTATION, EMCROW eCrow )
{
	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();

	if( eCrow == CROW_SUMMON )
	{
		std::tr1::shared_ptr<GLSummonClient> spCrow = pLandMan->GetSummon(dwNpcGlobID);

		if (!spCrow)
			return S_OK;

		// NPC ��ȿ�� üũ
		if (!spCrow || !spCrow->IsSummonNpcActionable(GetPosition(), GETBODYRADIUS()))
			return S_OK;

		if( spCrow->GetSummonType() != SUMMON_TYPE_NPC )
			return S_OK;
	}
	else
	{	
		std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);

		if (!spCrow)
			return S_OK;

		// NPC ��ȿ�� üũ
		if (!spCrow || !spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
			return S_OK;
	}

	//! ������ id�� ��Ȯ���� �˻�.
	//
	SSTATION* pSTATION = GLBusStation::GetInstance().GetStation ( dwSTATION );
	if ( !pSTATION )
        return S_OK;

	SMAPNODE *pNODE = m_pGaeaClient->FindMapNode(SNATIVEID(pSTATION->dwMAPID));
	if ( !pNODE )	
        return S_OK;
	
	//! pk ����� ������ ��� �̻��� ��� ���� ����� ���´�.
	//
	//DWORD dwPK_LEVEL = GET_PK_LEVEL();
	//if ( dwPK_LEVEL != UINT_MAX && dwPK_LEVEL>GLCONST_CHAR::dwPK_RECALL_ENABLE_LEVEL )
	//{
	//	m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBUS_TAKE_PK_LEVEL") );
	//	return E_FAIL;
	//}

	EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);

	GLLevelFileClient cLevelFile(m_pGaeaClient);
	BOOL bOk = cLevelFile.LoadFile(pNODE->m_LevelFile.c_str(), NULL);
	if (!bOk)
        return S_OK;

	const SLEVEL_REQUIRE* pRequire = cLevelFile.GetLevelRequire ();
	emReqFail = pRequire->ISCOMPLETE(this);
	if (emReqFail != EMREQUIRE_COMPLETE)
	{		
		CString strText;
		switch ( emReqFail )
		{
		case EMREQUIRE_LEVEL:
			{
				if (pRequire->m_signLevel == EMSIGN_FROMTO)
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_LEVEL2"), pRequire->m_wLevel, pRequire->m_wLevel2));
				}
				else
				{
					std::string strSIGN;
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLevel).GetString() );
					else
						strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLevel].c_str());

					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_LEVEL"), pRequire->m_wLevel, strSIGN.c_str()));
				}
			}
			break;

		case EMREQUIRE_ITEM:
			{
				SITEM *pItem = GLogicData::GetInstance().GetItem ( pRequire->m_sItemID );
				if ( pItem )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_ITEM"), pItem->GetName()));
				}
			}
			break;

		case EMREQUIRE_SKILL:
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pRequire->m_sSkillID );
				if ( pSkill )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_SKILL"), pSkill->GetName()));
				}
			}
			break;

		case EMREQUIRE_LIVING:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLiving).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLiving].c_str());

				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_LIVING"), pRequire->m_nLiving, strSIGN.c_str()));
			}
			break;

		case EMREQUIRE_BRIGHT:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signBright).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signBright].c_str());

				strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signBright].c_str());				
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_BRIGHT"), pRequire->m_nBright, strSIGN.c_str()));
			}
			break;

		case EMREQUIRE_QUEST_COM:
			{
				CString strQUEST = "quest";
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pRequire->m_sComQuestID.dwID );
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();                
					}
				}

				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_QUEST_COM"), strQUEST.GetString()));
			}
			break;

		case EMREQUIRE_QUEST_ACT:
			{
				CString strQUEST = "quest";
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pRequire->m_sComQuestID.dwID );
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();                
					}
				}	            
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_QUEST_ACT"), strQUEST.GetString()));
			}
			break;

		default:
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MAP_REQUIRE_FAIL") );
			break;
		};

		//! �ⱸ ��� ������ �ȵ� ���. GM level �̻��� ��� ���� ����.
		//
		if ( m_dwUserLvl < USER_USER_GM )
            return S_OK;
	}

	//! ���� Ƽ���� �ִ��� �˻�.
	//
	SINVENITEM* pITEM_TICKET = m_cInventory.FindItem ( ITEM_TICKET );
	if ( !pITEM_TICKET )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBUS_TAKE_TICKET") );
		return S_OK;
	}

	//! ���� ����.
	//
	GLMSG::SNETPC_REQ_BUS NetMsg;
	NetMsg.wPosX = pITEM_TICKET->wPosX;
	NetMsg.wPosY = pITEM_TICKET->wPosY;
	NetMsg.dwNpcGlobID = dwNpcGlobID;
	NetMsg.dwSTATION_ID = dwSTATION;
	NetMsg.emCrow = eCrow;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

	//! �ý� ���� ��û.
HRESULT GLCharacter::ReqTaxiStation ( WORD wPosX, WORD wPosY, int nSelectMap, int nSelectStop )
{
	//! ������ id�� ��Ȯ���� �˻�.
	//
	STAXI_MAP* pTaxiMap = GLTaxiStation::GetInstance().GetTaxiMap ( nSelectMap );
	if ( !pTaxiMap )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_MAPFAIL") );
		return S_OK;
	}

	STAXI_STATION* pStation = pTaxiMap->GetStation( nSelectStop );
	if ( !pStation )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_STATIONFAIL") );
		return S_OK;
	}

	SMAPNODE *pNODE = m_pGaeaClient->FindMapNode(SNATIVEID(pStation->dwMAPID));
	if ( !pNODE )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_MAPFAIL") );
		return S_OK;
	}

	SITEM* pVehicleItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
	if ( pVehicleItem )
	{
        if (m_emVehicle == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem(pVehicleItem->VehicleType()))
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_BIKE") );
			return E_FAIL;                        
		}
		else if ( m_emVehicle == EMVEHICLE_PASSENGER )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_BIKE") );
			return E_FAIL;
		}
	}

	EMREQFAIL emReqFail(EMREQUIRE_COMPLETE);

	GLLevelFileClient cLevelFile(m_pGaeaClient);
	BOOL bOk = cLevelFile.LoadFile ( pNODE->m_LevelFile.c_str(), NULL );
	if ( !bOk )	
        return S_OK;

	const SLEVEL_REQUIRE* pRequire = cLevelFile.GetLevelRequire ();
	emReqFail = pRequire->ISCOMPLETE ( this );
	if ( emReqFail != EMREQUIRE_COMPLETE )
	{
		CString strText;
		switch ( emReqFail )
		{
		case EMREQUIRE_LEVEL:
			{
				if (pRequire->m_signLevel == EMSIGN_FROMTO)
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format( ID2GAMEINTEXT("EMREQUIRE_LEVEL2"), pRequire->m_wLevel, pRequire->m_wLevel2));
				}
				else
				{
					std::string strSIGN;
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
						strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLevel).GetString() );
					else
						strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLevel].c_str());

					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_LEVEL"), pRequire->m_wLevel, strSIGN.c_str()));
				}
			}
			break;

		case EMREQUIRE_ITEM:
			{
				SITEM *pItem = GLogicData::GetInstance().GetItem ( pRequire->m_sItemID );
				if ( pItem )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_ITEM"), pItem->GetName()));
				}
			}
			break;

		case EMREQUIRE_SKILL:
			{
				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pRequire->m_sSkillID );
				if ( pSkill )
				{					
					m_pGaeaClient->PrintMsgTextDlg(
                        NS_UITEXTCOLOR::DISABLE,
                        sc::string::format(ID2GAMEINTEXT("EMREQUIRE_SKILL"), pSkill->GetName()));
				}
			}
			break;

		case EMREQUIRE_LIVING:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signLiving).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signLiving].c_str());

				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format( ID2GAMEINTEXT("EMREQUIRE_LIVING"), pRequire->m_nLiving, strSIGN.c_str()));
			}
			break;

		case EMREQUIRE_BRIGHT:
			{
				std::string strSIGN;
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					strSIGN = ID2GAMEINTEXT( CGameTextMan::GetInstance().GetCommentText("CDT_SIGN_ID", pRequire->m_signBright).GetString() );
				else
					strSIGN = ID2GAMEINTEXT(COMMENT::CDT_SIGN_ID[pRequire->m_signBright].c_str());

				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_BRIGHT"), pRequire->m_nBright, strSIGN.c_str()));
			}
			break;

		case EMREQUIRE_QUEST_COM:
			{
				CString strQUEST = "quest";
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pRequire->m_sComQuestID.dwID );
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();                
					}
				}			
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMREQUIRE_QUEST_COM"), strQUEST.GetString()));
			}
			break;

		case EMREQUIRE_QUEST_ACT:
			{
				CString strQUEST = "quest";
				GLQUEST *pQUEST = GLQuestMan::GetInstance().Find ( pRequire->m_sComQuestID.dwID );
				if (pQUEST)
				{
					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("QN_%03d_%03d", pQUEST->m_sNID.wMainID, pQUEST->m_sNID.wSubID);
						// CGameTextMan::GetInstance().GetQuestText(strKey).GetString();
						strQUEST = CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString();
					}
					else
					{
						strQUEST = pQUEST->GetTITLE();                
					}
				}	            
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format( ID2GAMEINTEXT("EMREQUIRE_QUEST_ACT"), strQUEST.GetString()));
			}
			break;

		default:
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("MAP_REQUIRE_FAIL") );
			break;
		};

		//! �ⱸ ��� ������ �ȵ� ���. GM level �̻��� ��� ���� ����.
		//
		if ( m_dwUserLvl < USER_USER_GM )
            return S_OK;
	}

	//! �ý� ī�尡 �ִ��� �˻�.
	//
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType != ITEM_TAXI_CARD )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_TICKET") );
		return E_FAIL;
	}

    PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();
    if ( !pLandManClient )
        return E_FAIL;

    const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID().dwID );
    if ( !pMapNode )
        return E_FAIL;

    if ( pMapNode->IsBlockDrugItem( pItem->BasicType() ) )
    {
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("PROHIBIT_ITEM_MESSAGE") );

        return E_FAIL;
    }

	LONGLONG lnCharge = GetCalcTaxiCharge( nSelectMap, nSelectStop );

	if ( GetInvenMoney() < lnCharge )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_MONEY") );
		return S_FALSE;
	}

	//! ���� ����.
	//
	GLMSG::SNETPC_REQ_TAXI NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.dwSelectMap = nSelectMap;
	NetMsg.dwSelectStop = nSelectStop;
	NetMsg.dwGaeaID = m_dwGaeaID;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqLoudSpeaker(const char* szChat, const VEC_LINK_DATA_BASIC& vecLinkDataBasic)
{
    if (!szChat)
        return E_FAIL;

	WORD wPosX(0), wPosY(0);

	if ( m_dwUserLvl < USER_USER_GM )
	{
		SINVENITEM* pINVENITEM = m_cInventory.FindItem ( ITEM_LOUDSPEAKER );
		if ( !pINVENITEM )
		{
			//! Ȯ���� �������� �������� �ʽ��ϴ�.
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_NOITEM") );
			return E_FAIL;
		}

		if ( m_sPMarket.IsOpen() ) 
		{
			WORD wTurn = (WORD)m_cInventory.CountTurnItem( pINVENITEM->sItemCustom.GetNativeID() );
			WORD wMarketTurn = (WORD)m_sPMarket.GetItemTurnNum( pINVENITEM->sItemCustom.GetNativeID() ) ;

			if ( wTurn <= wMarketTurn ) 
			{
				//! Ȯ���� �������� �������� �ʽ��ϴ�.
				m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_LOUDSPEAKER_NOITEM") );
				return E_FAIL;
			}
		}

		wPosX = pINVENITEM->wPosX;
		wPosY = pINVENITEM->wPosY;
	}

    if (strlen(szChat) >= CHAT_MSG_SIZE)
        return E_FAIL;

	// Ȯ���� ��� ��û
	GLMSG::SNETPC_CHAT_LOUDSPEAKER SendMsg;
	SendMsg.m_PosX = wPosX;
	SendMsg.m_PosY = wPosY;
	SendMsg.m_Msg  = szChat;
    SendMsg.m_vLinkDataBasic = vecLinkDataBasic;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendMsg);
	
	m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LOUDSPEAKER, SendBuffer);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketTitle ( const char* szTitle )
{
	if (m_sPMarket.IsOpen())
        return S_FALSE;

    if (!szTitle)
        return S_FALSE;

	// �ʱ�ȭ�� ���Ѵ�.
	DoPrivateMarketClose();

	// Ÿ��Ʋ ���� ��û
	GLMSG::SNETPC_PMARKET_TITLE NetMsg;
    NetMsg.SetTitle(std::string(szTitle));
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketRegItem ( WORD wPosX, WORD wPosY, LONGLONG llMoney, DWORD dwNum )
{
	if ( m_sPMarket.IsOpen() )
        return S_FALSE;

	SINVENITEM *pINVENITEM = m_cInventory.GetItem(wPosX, wPosY);
	if ( !pINVENITEM )
        return S_FALSE;

	//! ����� �� �ִ� �ѵ��� �Ѿ�� �ֽ��ϴ�.
	if ( m_sPMarket.GetItemNum() >= EPrivateMarketTotalSlot )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_MAXNUM") );
		return S_FALSE;
	}

	// ����� �� �ִ� �ݾ� �ѵ��� �Ѿ�� �ֽ��ϴ�.
	if ( llMoney > LL_LIMITED_A_UNIT_PRICE || llMoney < 0 ) // 0 ���� ���� ��� int�� ������ �Ѿ�� ��
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("PRIVATE_LIMITED_A_UINIT_PRICE") );
		return S_FALSE;
	}

	SNATIVEID nidITEM = pINVENITEM->sItemCustom.GetNativeID();

	SITEM *pITEM = GLogicData::GetInstance().GetItem ( nidITEM );
	if ( !pITEM )	
        return S_FALSE;

	//	�ŷ��ɼ�
	//if ( !pITEM->sBasicOp.IsEXCHANGE() )
	if ( !( pINVENITEM->sItemCustom.IsExchange() != NULL ) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( 
			NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_NOSALE") );
		return S_FALSE;
	}

	// ��ī���̸鼭 ���� Ȱ��ȭ �Ǿ� ������ ������ ����� �� ����.
	if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
	{
		// ����ִ� �������� ��ī���̸� ���� Ȱ��ȭ �Ǿ� ������
		GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
		if ( pMyPet->IsVALID () && pINVENITEM->sItemCustom.PetDbNum() == pMyPet->DbNum() )
		{
			return E_FAIL;
		}
	}

	if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && pINVENITEM->sItemCustom.m_VehicleDbNum != 0 )
	{
		return E_FAIL;
	}

	//! �̹� ��ϵ� ���������� �˻�.
	bool bREGPOS = m_sPMarket.IsRegInvenPos ( SNATIVEID(wPosX,wPosY) );
	if ( bREGPOS )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_REGITEM") );
		return S_FALSE;
	}

	//! ��ħ ������ ��� ������ ������ �������� �̹� ��ϵǾ� �ִ��� �˻�.
	if ( pITEM->ISPILE() )
	{
		bool bREG = m_sPMarket.IsRegItem ( nidITEM );
		if ( bREG )
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_REGITEM") );
			return S_FALSE;
		}

		//! �����ϰ� �ִ� ���� ��ŭ �Ǹ� ����. // - �ϰ�쿡�� int�� ���� �Ѱ����� �ִ밹���� ����
		DWORD dwTURN = m_cInventory.CountTurnItem ( nidITEM );
		if ( dwNum >= dwTURN || dwNum < 0  )
		{
			dwNum = dwTURN;
		}
	}

	if ( !pITEM->ISPILE() )
	{
		dwNum = 1;
	}
	
	SNATIVEID sSALEPOS;
	bool bPOS = m_sPMarket.FindInsertPos ( nidITEM, sSALEPOS );
	if ( !bPOS )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_REGITEM_FB_MAXNUM") );
		return S_FALSE;
	}

	// ���� �������� ���� ���� ������� �������� ���� �־���ϱ� ������ ���� �ϳ� ���ش�.
	if( ITEM_PRIVATEMARKET == pITEM->BasicType() &&
		dwNum == m_cInventory.CountTurnItem( ITEM_PRIVATEMARKET ) )
	{
		--dwNum;
		if( 0 == dwNum )
			return S_FALSE;
	}

	GLMSG::SNETPC_PMARKET_REGITEM NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	NetMsg.llMoney = llMoney;
	NetMsg.dwNum = dwNum;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketDisItem ( WORD wPosX, WORD wPosY )
{
	if ( m_sPMarket.IsOpen() )	
        return S_FALSE;

	SNATIVEID sSALEPOS(wPosX,wPosY);
	const SSALEITEM* pSALEITEM = m_sPMarket.GetItem ( sSALEPOS );
	if ( !pSALEITEM )
        return S_FALSE;

	GLMSG::SNETPC_PMARKET_DISITEM NetMsg;
	NetMsg.sSALEPOS = sSALEPOS;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketOpen ( BOOL bOnlyUI )
{
	if ( m_sPMarket.IsOpen() )
        return S_FALSE;

	//! �Ǹſ����� ����� �������� 
	if ( m_sPMarket.GetItemNum() == 0 )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_OPEN_FB_EMPTY") );
		return S_FALSE;
	}

	if ( m_wPMPosX==USHRT_MAX )
        return S_FALSE;
	if ( m_wPMPosY==USHRT_MAX )
        return S_FALSE;

	// Ŭ�󿡼� ��� �ֵ��� ����.
	m_sPMarket.SetOnlyUI( bOnlyUI );

	//! ���λ��� ���� ��û.
	GLMSG::SNETPC_PMARKET_OPEN NetMsg;
	NetMsg.wPosX = m_wPMPosX;
	NetMsg.wPosY = m_wPMPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	m_wPMPosX = (USHRT_MAX);
	m_wPMPosY = (USHRT_MAX);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketClose ()
{
	if ( !m_sPMarket.IsOpen() )
        return S_FALSE;

	DoPrivateMarketClose();

	GLMSG::SNETPC_PMARKET_CLOSE NetMsg;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketInfo ( DWORD dwGaeaID )
{
	std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar ( dwGaeaID );
	if ( !pCLIENT )
        return S_FALSE;

	bool bOPEN = pCLIENT->m_sPMarket.IsOpen();
	if ( !bOPEN )
	{
		return S_FALSE;
	}

	//! ���� ���� ��û�� ���� ������ ����.
	pCLIENT->m_sPMarket.DoMarketInfoRelease();

	std::string	strName = pCLIENT->m_strName;
	std::string strTitle = pCLIENT->m_sPMarket.GetTitle();
	const DWORD dwChaDBNum = pCLIENT->CharDbNum();

	// �����	
	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_PrivateMarket_ReqPMInfo,
		"-s, -s, -n", strName.c_str(), strTitle.c_str(), dwChaDBNum );

	//! ���� ���� ��û.
	GLMSG::SNETPC_PMARKET_ITEM_INFO NetMsg;
	NetMsg.dwGaeaID = dwGaeaID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketInfoRelease( DWORD dwGaeaID )
{
	std::tr1::shared_ptr< GLCharClient > pCLIENT = m_pGaeaClient->GetChar( dwGaeaID );
	if( NULL == pCLIENT )
        return S_FALSE;

	pCLIENT->m_sPMarket.DoMarketInfoRelease();

	// �ٸ���� ���λ����� ������ �ӽ� ���ŵ� ��ī�� ������ �������ش�.
	m_mapPETCardInfoTemp.clear();
	m_mapVEHICLEItemInfoTemp.clear();

	// ���λ��� ���÷��� ��� ���� ����
	GfxInterfaceBase::GetInstance()->ToggleOffPrivateMarketDisplay( dwGaeaID );

	return S_OK;
}

HRESULT GLCharacter::ReqPMarketBuy ( DWORD dwGaeaID, WORD wPosX, WORD wPosY, DWORD dwNum )
{
	std::tr1::shared_ptr<GLCharClient> pCLIENT = m_pGaeaClient->GetChar ( dwGaeaID );
	if ( !pCLIENT )
        return S_FALSE;

	SNATIVEID sSALEPOS(wPosX,wPosY);

	const SSALEITEM* pSALEITEM = pCLIENT->m_sPMarket.GetItem ( sSALEPOS );
	if ( !pSALEITEM )
        return S_FALSE;

	SITEM *pITEM = GLogicData::GetInstance().GetItem ( pSALEITEM->sITEMCUSTOM.GetNativeID() );
	if ( !pITEM )
        return S_FALSE;

	if ( pSALEITEM->bSOLD )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_SOLD") );
		return S_FALSE;
	}

	if ( !pITEM->ISPILE() )
	{
		dwNum = 1;
	}

	if ( pSALEITEM->dwNUMBER < dwNum )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NUM") );
		return S_FALSE;
	}

	if ( GetInvenMoney() < (dwNum*pSALEITEM->llPRICE) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_LOWMONEY") );
		return S_FALSE;
	}

	BOOL bITEM_SPACE(FALSE);
	if ( pITEM->ISPILE() )
	{
		WORD wPILENUM = pITEM->sDrugOp.wPileNum;
		SNATIVEID sNID = pITEM->sBasicOp.sNativeID;
		WORD wREQINSRTNUM = (WORD) dwNum;

		bITEM_SPACE = m_cInventory.ValidPileInsrt ( wREQINSRTNUM, sNID, wPILENUM );
	}
	else
	{
		WORD wPosX(0), wPosY(0);
		bITEM_SPACE = m_cInventory.FindInsertable ( wPosX, wPosY );
	}

	if ( !bITEM_SPACE )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPMARKET_BUY_FB_NOINVEN") );
		return E_FAIL;
	}

	GLMSG::SNETPC_PMARKET_BUY NetMsg;
	NetMsg.dwGaeaID = dwGaeaID;
	NetMsg.dwNum = dwNum;
	NetMsg.sSALEPOS = sSALEPOS;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqClubNew ( DWORD dwNpcGlobID, const char* szClubName )
{
	if  ( szClubName==NULL )
        return S_FALSE;

	CString strText;

	if ( m_ClubDbNum!=CLUB_NULL )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_ALREADY") );
		return S_FALSE;
	}

	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( pMyParty->isValid() == false )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMASTER") );
		return S_FALSE;
	}

	if (pMyParty->GetNMember() < GLCONST_CHAR::dwCLUB_PARTYNUM)
	{		
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMEMBER"), GLCONST_CHAR::dwCLUB_PARTYNUM));
		return S_FALSE;
	}

	if ( !szClubName || (strlen(szClubName) == 0) )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_FAIL") );
		return S_FALSE;
	}

	if ( m_wLevel < GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl )
	{		
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_MASTER_LVL"), GLogicData::GetInstance().GetClubRankData(0).m_dwMasterLvl));
		return S_FALSE;
	}

	if ( m_nLiving < int ( GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint ) )
	{		
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_AVER_LIVING"), GLogicData::GetInstance().GetClubRankData(0).m_dwLivingPoint));
		return S_FALSE;
	}

	if ( GetInvenMoney() < GLogicData::GetInstance().GetClubRankData(0).m_dwPay )
	{		
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_NOTMONEY"), GLogicData::GetInstance().GetClubRankData(0).m_dwPay));
		return S_FALSE;
	}


	CString strTEMP( szClubName ); 

	// �±��� ���� ���� üũ
    if ( RENDERPARAM::emLangSet == language::THAILAND ) 
    {
        if ( !STRUTIL::CheckThaiString(strTEMP) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_THAICHAR_ERROR"));
		    return S_FALSE;
	    }
    }

	// ��Ʈ�� ���� ���� üũ 
    if ( RENDERPARAM::emLangSet == language::VIETNAM ) 
    {
	    if ( STRUTIL::CheckVietnamString( strTEMP ) )
	    {
		    m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMCLUB_NEW_FB_VNCHAR_ERROR"));
		    return S_FALSE;
	    }
    }

	BOOL bFILTER0 = STRUTIL::CheckString( strTEMP );;
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter ( szClubName );
	if ( bFILTER0 || bFILTER1 || ( strTEMP != szClubName ) )
	{		
		m_pGaeaClient->PrintMsgTextDlg(
            NS_UITEXTCOLOR::NEGATIVE,
            sc::string::format(ID2GAMEINTEXT("EMCLUB_NEW_FB_BADNAME"), szClubName));
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_NEW NetMsg;
	NetMsg.dwNpcGlobID = dwNpcGlobID;
	NetMsg.SetClubName(szClubName);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

void GLCharacter::ReqClubNewForWidget( const char* szClubName )
{
	if( NULL == szClubName )
		return;

	ReqClubNew( GLNpcTalkClient::GetInstance().GetNpcGlobalID(), szClubName );
}

HRESULT GLCharacter::ReqGuidCommission ( DWORD dwNpcGlobID, float fRATE )
{
	if ( m_ClubDbNum==CLUB_NULL )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_NOTCLUB") );
		return S_FALSE;
	}

	if ( !m_pMyClub->IsMaster(m_CharDbNum) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_NOTCLUB") );
		return S_FALSE;
	}

	PLANDMANCLIENT pLand = m_pGaeaClient->GetActiveMap();
	if ( !pLand )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_FAIL") );
		return S_FALSE;
	}

	std::tr1::shared_ptr<GLCrowClient> spCROW = pLand->GetCrow(dwNpcGlobID);
	if (!spCROW)
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_FAIL") );
		return S_FALSE;
	}

	if ( fRATE < 0 )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_RANGE") );
		return S_FALSE;
	}

	if ( fRATE > GLCONST_CHAR::fMAX_COMMISSION )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMGUIDCOMMISSION_FB_RANGE") );
		return S_FALSE;
	}

	GLMSG::SNET_CLUB_GUID_COMMISSION NetMsg;
	NetMsg.fCommission = fRATE;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;
}

void GLCharacter::ReqGuidCommissionForWidgt( float fRate )
{
	ReqGuidCommission( GLNpcTalkClient::GetInstance().GetNpcGlobalID(), fRate );
}

HRESULT GLCharacter::ReqGarbageOpen()
{
	InitGarbageData();
	OpenGarbage();
	
    return S_OK;
}

HRESULT GLCharacter::ReqGarbageMoveItem()
{
	if ( m_sPreInventoryItem.VALID() )
	{
		// �տ� �� �������� ���� �������̾�� �ٲ� �� �ִ�
		SITEMCUSTOM sPreItem = GET_PREHOLD_ITEM();
		SITEM* pItem = GLogicData::GetInstance().GetItem( sPreItem.GetNativeID() );
        if (!pItem)
            return E_FAIL;

		// ��ī���ϰ��
		if ( pItem->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			GLPetClient* pMyPet = m_pGaeaClient->GetPetClient ();
			if ( pMyPet && pMyPet->IsVALID () && sPreItem.PetDbNum() == pMyPet->DbNum() )
			{
				m_sPreInventoryItem.RESET();
				return E_FAIL;
			}
		}


		//if ( pItem && pItem->sBasicOp.IsGarbage() )
		if ( pItem )
		{
			if ( sPreItem.IsGarbage() != NULL )
				m_sGarbageItem.SET( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
			else
				m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGARBAGE_RESULT_FB_MSG", 3 ) );
		}
		
		m_sPreInventoryItem.RESET();
	}
	
	return (m_sGarbageItem.VALID()? S_OK : E_FAIL);
}

HRESULT GLCharacter::ReqGarbageResult()
{
	
	if ( !m_sGarbageItem.VALID() )
		return S_FALSE;

	GLMSG::SNET_GARBAGE_RESULT NetMsg;

	NetMsg.wPosX = m_sGarbageItem.wPosX;
	NetMsg.wPosY = m_sGarbageItem.wPosY;
	
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	
	return S_OK;
}

HRESULT GLCharacter::ReqGarbageClose()
{
	InitGarbageData();
	CloseGarbage();

	return S_OK;
}

BOOL GLCharacter::GET_GARBAGE_ITEM( SITEMCUSTOM& sItemCustom )	// ITEMREBUILD_MARK
{
	if ( !m_sGarbageItem.VALID() )
		return FALSE;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sGarbageItem.wPosX, m_sGarbageItem.wPosY );
	if ( !pResistItem )
		return FALSE;

	sItemCustom = pResistItem->sItemCustom;
	return TRUE;
}

VOID GLCharacter::InitGarbageData()
{
	m_sGarbageItem.RESET();
	m_sPreInventoryItem.RESET();
}

HRESULT GLCharacter::ReqRebuildOpen( SNATIVEID sNPCID, DWORD dwNPCGlobalID )	// ITEMREBUILD_MARK
{
    //if ( !m_bUseRemodelCard )
	if ( EMTYPE_ITEM_REMODEL_NONE == m_emTypeItemRemodelState )
    {
        // NPC ��ȿ�� üũ
        PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
        std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNPCGlobalID);
        if (!spCrow)
            return E_FAIL;

        if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
            return E_FAIL;
    }

	RebuildCard::GetInstance().InitRebuild();
	OpenRebuild();

	GLMSG::SNET_REBUILD_RESULT NetMsg;

	NetMsg.emResult = EMREBUILD_RESULT_OPEN;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildMoveItem(const bool bMsg)
{
	GLMSG::SNET_REBUILD_MOVE_ITEM NetMsg;

	if ( m_sPreInventoryItem.VALID() )
	{
		// ����â�� �������� �ִµ� �������� ��ü�Ѵٸ� ����ī�带 ��������
		if( m_sRebuildItem.VALID() == true && m_sStampItem.VALID() == true )
		{
			GLMSG::SNET_REBUILD_MOVE_ITEM NetMsg(GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP);
			RebuildCard::GetInstance().ResetFixedOption();
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}

		// �տ� �� �������� ���� �������̾�� �ٲ� �� �ִ�
		SITEMCUSTOM sPreItem = GET_PREHOLD_ITEM();
		SITEM* pItem = GLogicData::GetInstance().GetItem( sPreItem.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_SUIT )
		{
			// �����ɼ� ������ �����Ǿ� �־�� �����ϴ�
			if ( strlen( pItem->sRandomOpt.szNAME ) > 3 )
			{
				NetMsg.wPosX = m_sPreInventoryItem.wPosX;
				NetMsg.wPosY = m_sPreInventoryItem.wPosY;
			}
		}
		// ����Ǿ��ٸ� ������ �����ʾƾ��Ѵ�;
		if ( sPreItem.IsBoxWrapped() )
			return E_FAIL;

		//!! SF_TODO
		//m_pInterface->SetRebuildWindowPreInvenItem( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
		m_sPreInventoryItem.RESET();
	}
	else if ( m_sRebuildItem.VALID() && bMsg == true )
	{
		SITEMCUSTOM sRebuildItem = GET_REBUILD_ITEM();
		SITEM* pItem = GLogicData::GetInstance().GetItem( sRebuildItem.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_SUIT )
		{
			// �����ɼ� ������ �����Ǿ� �־�� �����ϴ�
			if ( strlen( pItem->sRandomOpt.szNAME ) > 3 )
			{
				NetMsg.wPosX = m_sRebuildItem.wPosX;
				NetMsg.wPosY = m_sRebuildItem.wPosY;
			}
		}
	}

	const DWORD nStampCard(m_pGaeaClient->GetCharacter()->GetCountItemInInven( ITEM_STAMP_CARD ));
	if ( nStampCard == 0 )
		m_sStampItem.RESET();
	
	RebuildCard::GetInstance().ResetFixedOption();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildMoveStamp()
{
	GLMSG::SNET_REBUILD_MOVE_ITEM NetMsg(GLMSG::SNET_REBUILD_MOVE_ITEM::EMITEM_STAMP);

	if ( m_sPreInventoryItem.VALID() )
	{		
		SITEMCUSTOM sPreItem = GET_PREHOLD_ITEM();
		SITEMCUSTOM sSelectedItem = GET_REBUILD_ITEM();
		SITEM* pStampItem = GLogicData::GetInstance().GetItem( sPreItem.GetNativeID() );
		SITEM* pItem = GLogicData::GetInstance().GetItem( sSelectedItem.GetNativeID() );

		// �� �������� ����ī�带 ����� �� ���� ��츦 üũ�Ѵ�;
		switch ( m_emTypeItemRemodelState )
		{
		case EMTYPE_ITEM_REMODEL_NONE:
			{
				return S_FALSE;
			}
			break;

		case EMTYPE_ITEM_REMODEL_RANDOMOPTION:
			{
				// ���õ� �����ɼ��� ���� �������� ��� ����ī�带 ����� �� ����;
				if ( sSelectedItem.IsSetRandOpt() == FALSE )
					return S_FALSE;
			}
			break;

		case EMTYPE_ITEM_REMODEL_ADDSTATS:
			{
				// ���õ� ��������ɼ��� ���� �������� ��� ����ī�带 ����� �� ����;
				if ( sSelectedItem.sAddonInfo.IsEmpty() == true )
					return S_FALSE;
			}
			break;

		case EMTYPE_ITEM_REMODEL_BASICSTATS:
			{
				if ( NULL == pItem )
					return S_FALSE;

				// ���ݷ�, ���� �� �� �ϳ��� �����Ұ����� ��� ����ī�带 ����� �� ����;
				if ( ( false == pItem->IsEditableBasicAttackDamage() ) ||
					( false == pItem->IsEditableBasicDefence() ) )
				{
					return S_FALSE;
				}
			}
			break;

		case EMTYPE_ITEM_REMODEL_ITEMSKILL:
			{
				// ������ ��ų�� ��� ����ī�带 ����� �� ����;
				return S_FALSE;
			}
			break;
		}

		if( pStampItem && pStampItem->sBasicOp.emItemType == ITEM_STAMP_CARD )
		{
			NetMsg.wPosX = m_sPreInventoryItem.wPosX;
			NetMsg.wPosY = m_sPreInventoryItem.wPosY;
		}
		//!! SF_TODO
		//m_pInterface->SetRebuildWindowPreStampItem( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
		m_sPreInventoryItem.RESET();
	}

	RebuildCard::GetInstance().ResetFixedOption();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildInputMoney( LONGLONG i64InputMoney )
{
	GLMSG::SNET_REBUILD_INPUT_MONEY NetMsg;

	NetMsg.i64InputMoney = (LONGLONG)max( 0, i64InputMoney );
	NetMsg.i64InputMoney = (LONGLONG)min( NetMsg.i64InputMoney, GetInvenMoney() );

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT	GLCharacter::ReqRebuild_SelectCache(void)
{
	if ( m_sBackUpRebuildInvenItem.GetNativeID() == NATIVEID_NULL() )
		return E_FAIL;

	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.emResult = EMREBUILD_TRY_SELECT_INTO_CACHE;
	sNetMsg.emTypeRemodel = m_emTypeItemRemodelState;
	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	return S_OK;
}

HRESULT GLCharacter::ReqChangeRebuildType( DWORD dwNPCGlobalID, bool bKeepOrigin )
{
	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.dwNpcGlobID = dwNPCGlobalID;
	sNetMsg.emResult = bKeepOrigin ? EMREBUILD_CHANGE_REBUILDTYPE_KEEPORIGIN_CACHERESULT : EMREBUILD_CHANGE_REBUILDTYPE_APPLYRESULT_CACHEORIGINAL;	
	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	return S_OK;
}

HRESULT GLCharacter::ReqRebuildTry ( SNATIVEID sNPCID, DWORD dwNPCGlobalID )
{
	if ( false == IsRebuildItem( dwNPCGlobalID ) )
		return E_FAIL;

	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.dwNpcGlobID = dwNPCGlobalID;
	sNetMsg.emResult = EMREBUILD_RESULT_TRY;	
	sNetMsg.emTypeRemodel = m_emTypeItemRemodelState;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		sNetMsg.fixOption = m_fixOption;
		sNetMsg.wFixBasicStatsIndex = RebuildCard::GetInstance().GetFixedBasicStatIndex();
	}

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildResult( SNATIVEID sNPCID, DWORD dwNPCGlobalID )
{
	if ( false == IsRebuildItem( dwNPCGlobalID ) )
		return E_FAIL;

	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.dwNpcGlobID = dwNPCGlobalID;
	sNetMsg.emResult = EMREBUILD_RESULT_SUCCESS;	
	sNetMsg.emTypeRemodel = m_emTypeItemRemodelState;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		sNetMsg.fixOption = m_fixOption;
		sNetMsg.wFixBasicStatsIndex = RebuildCard::GetInstance().GetFixedBasicStatIndex();
	}

	RebuildCard::GetInstance().ResetFixedOption();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildCancel ( SNATIVEID sNPCID, DWORD dwNPCGlobalID )
{
	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.dwNpcGlobID = dwNPCGlobalID;
	sNetMsg.emResult = EMREBUILD_RESULT_CANCEL;	
	sNetMsg.emTypeRemodel = m_emTypeItemRemodelState;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		sNetMsg.fixOption = m_fixOption;
		sNetMsg.wFixBasicStatsIndex = RebuildCard::GetInstance().GetFixedBasicStatIndex();
	}

	RebuildCard::GetInstance().ResetFixedOption();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildDirectResult( SNATIVEID sNPCID, DWORD dwNPCGlobalID )
{
	if ( false == IsRebuildItem( dwNPCGlobalID ) )
		return E_FAIL;

	GLMSG::SNET_REBUILD_RESULT sNetMsg;
	sNetMsg.dwNpcGlobID = dwNPCGlobalID;
	sNetMsg.emResult = EMREBUILD_RESULT_DIRECTSUCCESS;	
	sNetMsg.emTypeRemodel = m_emTypeItemRemodelState;

	if ( GLUseFeatures::GetInstance().IsUsingStampCard() == true )
	{
		sNetMsg.fixOption = m_fixOption;
		sNetMsg.wFixBasicStatsIndex = RebuildCard::GetInstance().GetFixedBasicStatIndex();
	}

	RebuildCard::GetInstance().ResetFixedOption();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );

	return S_OK;
}

HRESULT GLCharacter::ReqRebuildClose()
{
	RebuildCard::GetInstance().InitRebuild();

//	Notify( CHAR_REBUILD_ITEM, &m_sRebuildItem );

	CloseRebuild();

	GLMSG::SNET_REBUILD_RESULT NetMsg;

	NetMsg.emResult = EMREBUILD_RESULT_CLOSE;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    //m_bUseRemodelCard = false;
	m_emTypeItemRemodelState = EMTYPE_ITEM_REMODEL_NONE;

	return S_OK;
}

HRESULT GLCharacter::ReqUsePetCard ( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )	
        return E_FAIL;

	// ������̸� ���� ��ȯ�� �� ����.
	if ( m_sCONFTING.IsCONFRONTING() )	
        return E_FAIL;

    //	���� ����Ҽ� ���� ��Ȳ�̴�.
    if ( IsActionLimit(EMACTION_LIMIT_PET) )
    {
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ACTIONLIMIT") );

        return E_FAIL;
    }

	PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap ();
	if ( !pLandManClient )
		return E_FAIL;

	const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pLandManClient->GetBaseMapID ().dwID );
	if ( pMapNode == NULL ) 
		return E_FAIL;

	// �����԰��ɿ��� üũ
	if (!pMapNode->IsPetActivity())
	{
		m_pGaeaClient->PrintMsgText ( 
						NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_INVALIDZONE") );
		return E_FAIL;
	}

	if ( IsCheckedSkillFlagSpecial(EMSPECA_RELEASE_PET) )
	{
		// ���� ȿ���� �ɷ� �ִ�
		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		// ������ ����
		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		// ������ ����
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType != ITEM_PET_CARD )
	{
		// ��ī�� �ƴϴ�
		return E_FAIL;
	}

	// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
    {
		//	Ȱ�� ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ERR_ACTIVITY") );
        return E_FAIL;
    }

    if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
    {
        //	�⿩ ����Ʈ�� �����մϴ�.
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ERR_CONTRIBUTION") );
        return E_FAIL;
    }
	
	m_llPetCardGenNum	  = pInvenItem->sItemCustom.guid;
	m_sPetCardNativeID    = pInvenItem->sItemCustom.GetNativeID();
	m_cPetCardGenType     = pInvenItem->sItemCustom.cGenType;

	GLMSG::SNETPET_REQ_USEPETCARD NetMsg(wPosX,wPosY);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqReGenPet ()
{
	if ( !IsValidBody() )	
        return E_FAIL;

    //	���� ����Ҽ� ���� ��Ȳ�̴�.
    if ( IsActionLimit(EMACTION_LIMIT_PET) )
    {
        m_pGaeaClient->PrintMsgText ( 
            NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("EMPET_USECARD_FB_ACTIONLIMIT") );

        return E_FAIL;
    }

	PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap ();
	if ( !pLandManClient )	
        return E_FAIL;

	const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pLandManClient->GetBaseMapID ().dwID );
	if ( pMapNode == NULL )
        return E_FAIL;

	if (!pMapNode->IsPetActivity())	
        return E_FAIL;

	SINVENITEM* pInvenItem = m_cInventory.FindItemByGenNumber ( m_llPetCardGenNum );
	if ( !pInvenItem ) 
        return E_FAIL;

	// !!!! �� ��� ���ǿ� �⿩���� Ư��Ȱ�� ����Ʈ�� �˻��ϹǷ� ��ȯ�� ���� ���Ǵ°��� ���� �˻����� ����
	
	//// RM #840 ������ �������ǿ� Ư��Ȱ�� �� �⿩���� ���ּ� �Ϻ� �������� ������������ ���Ǿ ���� (�ڽ�, �ڽ�Ƭ ��)
	//if ( !CHECK_ACTIVITY_POINT( *pItem ) ) 
	//       return E_FAIL;

	//   if ( !CHECK_CONTRIBUTION_POINT( *pItem ) ) 
	//       return E_FAIL;
	
	GLMSG::SNETPET_REQ_USEPETCARD NetMsg(pInvenItem->wPosX,pInvenItem->wPosY);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: DB���� ������ ���� ���� ��û
// *****************************************************
HRESULT	GLCharacter::ReqPetReviveInfo ()
{
	GLMSG::SNETPET_REQ_PETREVIVEINFO NetMsg(m_CharDbNum);
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

// *****************************************************
// Desc: DB���� ������ �� ������û
// *****************************************************
HRESULT GLCharacter::ReqPetRevive ( DWORD dwPetID )
{
	GLMSG::SNETPET_REQ_REVIVE NetMsg(dwPetID, m_wInvenPosX1, m_wInvenPosY1);
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosX = holditem::GetHoldPosY();
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);	

    holditem::ReleaseHoldItem();

	return S_OK;
}

// *****************************************************
// Desc: ��ī���� ���� ��û
// *****************************************************
HRESULT	GLCharacter::ReqPetCardInfo ()
{
	const GLInventory::CELL_MAP* pMapItem = m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
	GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.PetDbNum() != 0 )
		{
			GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(pInvenItem->sItemCustom.PetDbNum());
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}

	for ( WORD i = 0; i < EMSTORAGE_CHANNEL; ++i )
	{
		if ( m_bStorage[i] )
		{
			const GLInventory::CELL_MAP* pMapItem = m_cStorage[i].GetItemList();
			GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
			GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
			for ( ; iter!=iter_end; ++iter )
			{
				SINVENITEM* pInvenItem = (*iter).second;
				SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
				if ( pItem && pItem->sBasicOp.emItemType == ITEM_PET_CARD && pInvenItem->sItemCustom.PetDbNum() != 0  )
				{
					GLMSG::SNETPET_REQ_PETCARDINFO NetMsg(pInvenItem->sItemCustom.PetDbNum());
					m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
				}
			}
		}
	}
	
	return S_OK;
}
/*
HRESULT GLCharacter::ReqSetPhoneNumber ( const TCHAR * szPhoneNumber )
{
	if ( !szPhoneNumber )
        return S_FALSE;

	GLMSG::SNETPC_PHONE_NUMBER NetMsg;

	StringCchCopy ( NetMsg.szPhoneNumber, SMS_RECEIVER, szPhoneNumber );

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}
*/
HRESULT GLCharacter::ReqSendSMS( int nCharID, const TCHAR * szPhoneNumber, const TCHAR * szSmsMsg )
{
	if ( !szPhoneNumber )
        return S_FALSE;
	if ( !szSmsMsg )	
        return S_FALSE;

	SINVENITEM *pINVENITEM = m_cInventory.FindItem ( ITEM_SMS );
	if ( !pINVENITEM )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMSMS_FB_NO_ITEM") );
		return S_FALSE;
	}

	GLMSG::SNETPC_SEND_SMS NetMsg;

	NetMsg.dwReceiveChaNum = nCharID;
	StringCchCopy ( NetMsg.szPhoneNumber, SMS_RECEIVER, szPhoneNumber );
	StringCchCopy ( NetMsg.szSmsMsg, SMS_LENGTH, szSmsMsg );
	NetMsg.wItemPosX = pINVENITEM->wPosX;
	NetMsg.wItemPosY = pINVENITEM->wPosY;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::SetVehicle ( bool bActive )
{
	// ĳ������ ���� ���¸� Ż�Ϳ� �����ؼ� �ʱ�ȭ �Ѵ�.
 	if ( bActive )
	{
		if ( m_emVehicle )
			return E_FAIL;

		m_emVehicle = EMVEHICLE_DRIVER;		
		
		int emType = m_sVehicle.m_emTYPE ;
		m_emANISUBTYPE = (EMANI_SUBTYPE) (AN_SUB_HOVERBOARD + emType) ;
		
		// AgentServer ������ ���ؼ� �ٽ� �޼����� �����ش�
		// Field�� �̹� ���ŵ� ������...
		if ( !IsActState(EM_ACT_PEACEMODE) ) 
			ReqTogglePeaceMode();

		// ž�� ����Ʈ �߰�
		STARGETID sTargetID(CROW_PC, m_dwGaeaID, GetPosition());
		DxEffGroupPlayer::GetInstance().NewEffGroup
		(
			GLCONST_CHAR::strVEHICLE_GEN_EFFECT.c_str(),
			GetTransMatrix(),
			&sTargetID, 
			TRUE,
			TRUE
		);

		SITEMCUSTOM& sItemCustom = m_PutOnItems[SLOT_VEHICLE];

		SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if (!pItem)
            return E_FAIL;	
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::IDCOLOR,
            sc::string::format(ID2GAMEINTEXT("VEHICLE_SET_FB_OK"), pItem->GetName()));

		DxViewPort::GetInstance().SetVehicleCamera();
	}
	else
	{
		if ( !m_emVehicle )
            return E_FAIL;

		m_emVehicle = EMVEHICLE_OFF;				

		EMSLOT emRHand = GetCurRHand();
		EMSLOT emLHand = GetCurLHand();
		m_emANISUBTYPE = CHECK_ANISUB ( m_pITEMS[emRHand], m_pITEMS[emLHand], GETCHARINDEX() );

		SITEMCUSTOM& sItemCustom = m_PutOnItems[SLOT_VEHICLE];
		SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCustom.GetNativeID() );
		if ( pItem )
		{		
			m_pGaeaClient->PrintMsgText(
                NS_UITEXTCOLOR::IDCOLOR,
                sc::string::format(ID2GAMEINTEXT("VEHICLE_RESET_FB_OK"),pItem->GetName()));
		}
		
		DxViewPort::GetInstance().SetGameCamera();
	}

	GLCHARLOGIC::INIT_DATA ( FALSE, FALSE );
	UpdateSuit( TRUE );
	ReSelectAnimation();

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_BasicInfo_Update );

	return S_OK;

}

HRESULT GLCharacter::ReqSetVehicle(  bool bActive )
{
	// Ż���� Ȱ��ȭ
	if (bActive)
	{
		if ( IsProducting() )
		{
			PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_DONT_T_INVEN" ) ); 
			return E_FAIL;
		}

		// ������̸� Ż���� Ȱ��ȭ ��ų�� ����.
		if (m_sCONFTING.IsCONFRONTING())
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true Confront"));
            return E_FAIL;
        }

		if (!m_sVehicle.IsActiveValue())
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true Active value"));
            return E_FAIL;
        }

		if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT)  || 
			 isAction(GLAT_FALLING, GLAT_TALK, GLAT_GATHERING) || 
			 isAction(GLAT_FEIGN_FALLING))
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true isAction"));
            return E_FAIL;
        }

		if ( m_emVehicle == EMVEHICLE_PASSENGER )
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true m_emVehicle"));
            return E_FAIL;
        }

		// �� üũ 
		PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap ();
		if ( !pLandManClient )
        {
           // m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true !pLandManClient"));
            return E_FAIL;
        }

		const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( pLandManClient->GetBaseMapID ().dwID );
		if (!pMapNode)
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle true !pMapNode"));
            return E_FAIL;
        }

		// �����԰��ɿ��� üũ
		if ( !pMapNode->IsVehicleActivity() )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_MAP_FAIL") );
			return S_OK;
		}


		// ����Ŭ ���� ���� üũ 		
		SITEMCUSTOM& sItemCostom = m_PutOnItems[SLOT_VEHICLE];
		if ( sItemCostom.GetNativeID() == NATIVEID_NULL() )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
			return E_FAIL;
		}

		SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCostom.GetNativeID() );
		if ( !pItem || pItem->sBasicOp.emItemType != ITEM_VEHICLE )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
			return E_FAIL;		
		}

		if ( m_sVehicle.IsNotEnoughFull() )
		{
			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NOTENOUGH_OIL") );
			return E_FAIL;	
		}

        if (m_bSkillFactsSpecial[EMSPECA_VEHICLE_GETOFF] ) // ���� : ���� ȿ�����̸� Ż���� Ȱ��ȭ ��ų�� ����.
        {
            m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VEHICLE_SET_FB_FAIL_DEBUFF"));
            return E_FAIL;
        }
		
        if ( m_bSkillFactsSpecial[EMSPECA_TURN_STATE_BATTLE] ) // ���� ��.
        {
            m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("VEHICLE_SET_FB_FAIL_BATTLE"));
            return E_FAIL;
        }

		if ( m_bSkillFactsSpecial[EMSPECA_DISGUISE] )
		{
			return E_FAIL;
		}
						
		// ������ ��û
		GLMSG::NET_CHAR_ACTIVE_VEHICLE_CF NetMsg(true);
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		return S_OK;
		
	}
	// Ż���� ��Ȱ��ȭ
	else 
	{
		if ( !m_sVehicle.IsActiveValue() )
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle false Vehicle IsActiveValue"));
            return E_FAIL;
        }

		if ( m_emVehicle == EMVEHICLE_PASSENGER )
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle false Passenger"));
            return E_FAIL;
        }

		if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) || isAction(GLAT_FALLING) )
        {
            //m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::NEGATIVE, std::string("ReqSetVehicle false IsACTION"));
            return E_FAIL;
        }

		// ����Ŭ ���� ���� üũ 
		SITEMCUSTOM& sItemCostom = m_PutOnItems[SLOT_VEHICLE];
		if ( sItemCostom.GetNativeID() == NATIVEID_NULL() )
		{
			//m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
			return E_FAIL;
		}

		SITEM* pItem = GLogicData::GetInstance().GetItem( sItemCostom.GetNativeID() );
		if ( !pItem || pItem->sBasicOp.emItemType != ITEM_VEHICLE )
		{
			//m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::NEGATIVE, ID2GAMEINTEXT("VEHICLE_SET_FB_NO_ITEM") );
			return E_FAIL;		
		}

		// ������ ��û
		GLMSG::NET_CHAR_ACTIVE_VEHICLE_CF NetMsg(false);

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		return S_OK;		
		
	}

	return S_OK;
}

HRESULT GLCharacter::ReqVehicleUpdate()
{
	if ( m_sVehicle.IsActiveValue() )
        return E_FAIL;

	// ����Ŭ ���� ���� üũ 
	SITEMCUSTOM& sItemCostom = m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCostom.GetNativeID() == NATIVEID_NULL() )
	{
		return E_FAIL;
	}

	GLMSG::SNETPC_GET_VEHICLE NetMsg;
	NetMsg.m_ItemID = sItemCostom.GetNativeID();

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;
}

void GLCharacter::ReqVehicleChangeAccessory( EMSUIT emSUIT )
{
	if ( m_pGaeaClient->GetTradeClient()->Valid() )
		return;

	//! �������̳� ��ų ���� �߿� ���� ������ ���� �� �� ���ٰ� ��.
	//
	if ( isAction(GLAT_ATTACK, GLAT_SKILL, GLAT_SKILL_WAIT) )
    {
        holditem::ReleaseHoldItem();

		return;
    }

    SITEMCUSTOM sHoldItemCustom = holditem::GetHoldItem();
	SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype( emSUIT );

	//	SLOT <-> INVEN
	if ( sHoldItemCustom.GetNativeID() != NATIVEID_NULL() && sSlotItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
		if ( !pHoldItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_VehicleColor_IsOpen ) )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		if ( !m_sVehicle.CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) )
        {
            holditem::ReleaseHoldItem();

			return;
        }
		
		SITEM* pVehicleItem = GLogicData::GetInstance().GetItem( m_sVehicle.m_sVehicleID );
		if ( !pVehicleItem )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		if ( pVehicleItem->sVehicle.bNotUseParts )
		{
            holditem::ReleaseHoldItem();

			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_ACC_NOT_USE_PARTS") );
			return;
		}
	
		SITEM* pSlotItem = GLogicData::GetInstance().GetItem( sSlotItemCustom.GetNativeID() );
		if ( !pSlotItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		if ( !m_pGaeaClient->GetCharacter()->ACCEPT_ITEM ( sHoldItemCustom ) )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != pSlotItem->sSuitOp.emSuit )
        {
            holditem::ReleaseHoldItem();

            return;
        }

        if ( holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            GLMSG::SNET_VEHICLE_REQ_INVEN_EX_SLOT NetMsg;
            NetMsg.emSuit = emSUIT;
            NetMsg.wPosX = holditem::GetHoldPosX();
            NetMsg.wPosY = holditem::GetHoldPosY();
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }

        holditem::ReleaseHoldItem();
	}
	// INVEN -> SLOT
	else if ( sHoldItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		SITEM* pHoldItem = GLogicData::GetInstance().GetItem( sHoldItemCustom.GetNativeID() );
		if ( !pHoldItem ) 
		{
            holditem::ReleaseHoldItem();
			// �Ϲ� ����
			return;
		}

		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_VehicleColor_IsOpen ) )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		if ( !m_sVehicle.CheckSlotItem( pHoldItem->sBasicOp.sNativeID, emSUIT ) )
        {
            holditem::ReleaseHoldItem();

            return;
        }

		SITEM* pVehicleItem = GLogicData::GetInstance().GetItem ( m_sVehicle.m_sVehicleID );
		if ( !pVehicleItem )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		if ( pVehicleItem->sVehicle.bNotUseParts )
		{
            holditem::ReleaseHoldItem();

			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_ACC_NOT_USE_PARTS") );
			return;
		}


		if ( !m_pGaeaClient->GetCharacter()->ACCEPT_ITEM ( sHoldItemCustom ) )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		// Ÿ���� �ٸ���
		if ( pHoldItem->sSuitOp.emSuit != emSUIT )
        {
            holditem::ReleaseHoldItem();

            return;
        }

        if ( holditem::IsHoldPlace( holditem::INVENTORY ) )
        {
            GLMSG::SNET_VEHICLE_REQ_INVEN_TO_SLOT NetMsg;
            NetMsg.emSuit = emSUIT;
            NetMsg.wHoldPosX = holditem::GetHoldPosX();
            NetMsg.wHoldPosY = holditem::GetHoldPosY();
            m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
        }

        holditem::ReleaseHoldItem();
	}
	// HOLD
	else if ( sSlotItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		if( true == GLWidgetScript::GetInstance().LuaCallFunc< bool >(
			NSWIDGET_SCRIPT::g_strFunc_VehicleColor_IsOpen ) )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		SITEM* pVehicleItem = GLogicData::GetInstance().GetItem ( m_sVehicle.m_sVehicleID );
		if ( !pVehicleItem )
		{
            holditem::ReleaseHoldItem();

			return;
		}

		if ( pVehicleItem->sVehicle.bNotUseParts )
		{
            holditem::ReleaseHoldItem();

			m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_ACC_NOT_USE_PARTS") );
			return;
        }

        holditem::HoldItem( holditem::VEHICLE_SLOT, sSlotItemCustom, emSUIT, 0 );
	}

	return;

}



void GLCharacter::ReqVehicleRemoveSlotItem( EMSUIT emSUIT )
{
	WORD wPosX(0), wPosY(0);
	SITEMCUSTOM	sSlotItemCustom = m_sVehicle.GetSlotitembySuittype ( emSUIT );

	SITEM* pSlotItem = GLogicData::GetInstance().GetItem ( sSlotItemCustom.GetNativeID() );
	if ( !pSlotItem ) 
	{
		// �Ϲ� ����
		return;
	}

	BOOL bOk = m_cInventory.FindInsertable ( wPosX, wPosY );
	if ( !bOk )
	{
		//	�ι��� ��������.
		return;
	}

	GLMSG::SNET_VEHICLE_REQ_REMOVE_SLOTITEM NetMsg;
	NetMsg.emSuit = emSUIT;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
}

void GLCharacter::ReqVehicleGiveBattery( WORD wPosX, WORD wPosY )
{
	// ��Ȱ�� ���¿����� ��Ḧ �� �� �ִ�.
	//if ( !IsVALID() )							return;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	// ����Ŭ ������ ���� üũ
	if ( pItem->sBasicOp.emItemType != ITEM_VEHICLE )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    if ( !holditem::HaveHoldItem() )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
    {
        holditem::ReleaseHoldItem();

        return;
    }

    SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	// ���� ���� üũ
	if ( pHold->sBasicOp.emItemType != ITEM_VEHICLE_OIL )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	// ����Ŭ ������ ���ٸ� ���
	if ( pInvenItem->sItemCustom.m_VehicleDbNum == 0 )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	if ( pItem->sVehicle.emVehicleType != pHold->sVehicle.emVehicleType )
    {
        holditem::ReleaseHoldItem();

        return;
    }

	SVEHICLEITEMINFO sVehicle;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if (!pGlobalStage->IsEmulator())
	{
		// ����Ŭ ������ ������ ������ ���� ����Ŭ�̴�.
		VEHICLEITEMINFO_MAP_ITER iter = m_mapVEHICLEItemInfo.find( pInvenItem->sItemCustom.m_VehicleDbNum );
		if ( iter==m_mapVEHICLEItemInfo.end() )
        {
            holditem::ReleaseHoldItem();

            return;
        }

		sVehicle = (*iter).second;
	}
	else
	{
		sVehicle.m_nFull  = m_sVehicle.m_nFull;
		sVehicle.m_emTYPE = m_sVehicle.m_emTYPE;
	}

	// ���ᰡ ������ �ִٸ�
	if ( sVehicle.m_nFull >= 1000 )
    {
        holditem::ReleaseHoldItem();

        return;
    }


	GLMSG::SNET_VEHICLE_REQ_GIVE_BATTERY NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
    NetMsg.wHoldPosX = holditem::GetHoldPosX();
    NetMsg.wHoldPosY = holditem::GetHoldPosY();

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    holditem::ReleaseHoldItem();
}

void GLCharacter::ReqVehicleInvenUpdate()
{
	const GLInventory::CELL_MAP* pMapItem = m_cInventory.GetItemList();
	GLInventory::CELL_MAP_CITER iter = pMapItem->begin();
	GLInventory::CELL_MAP_CITER iter_end = pMapItem->end();
	for ( ; iter!=iter_end; ++iter )
	{
		SINVENITEM* pInvenItem = (*iter).second;
		SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && pInvenItem->sItemCustom.m_VehicleDbNum != 0 )
		{
			GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
			NetMsg.dwVehicleID = pInvenItem->sItemCustom.m_VehicleDbNum;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}

	// ����Ŭ ���� ���� üũ 
	SITEMCUSTOM& sItemCustom = m_PutOnItems[SLOT_VEHICLE];
	if ( sItemCustom.GetNativeID() != NATIVEID_NULL() )
	{
		SITEM* pItem = GLogicData::GetInstance().GetItem ( sItemCustom.GetNativeID() );
		if ( pItem && pItem->sBasicOp.emItemType == ITEM_VEHICLE && sItemCustom.m_VehicleDbNum != 0 )
		{
			GLMSG::SNET_VEHICLE_REQ_ITEM_INFO NetMsg;
			NetMsg.dwVehicleID = sItemCustom.m_VehicleDbNum;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		}
	}
}

bool GLCharacter::ReqQBoxEnableState( bool bQboxEnable )
{
	if ( GetPartyID().isValidParty() )
	{
		if ( IsPartyMaster() )
		{
			GLMSG::SNET_QBOX_OPTION_REQ_AG NetMsg(m_dwGaeaID, bQboxEnable);
			m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
		}
		else
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >( 
				NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetQBoxLock,
				"-b", !bQboxEnable );
			return false;
		}
	}
	return true;
}

void GLCharacter::ReqItemShopOpen( bool bOpen )		// ItemShop Open/Close �뺸
{
//#if defined ( JP_PARAM ) || defined(_RELEASED)	// JAPAN Item Shop
	EMSERVICE_PROVIDER sp = m_pGaeaClient->GetServiceProvider();

	if (sp == SP_JAPAN)
	{	
		if ( m_bItemShopOpen == bOpen )
			return;
		
		GLMSG::SNETPC_OPEN_ITEMSHOP	NetMsg;
		NetMsg.bOpen = bOpen;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
		
		m_bItemShopOpen = bOpen;	
		
		//!! SF_TODO
		/*if (m_bItemShopOpen)
			m_pInterface->ItemShopAddShopIcon(m_dwGaeaID);
		else
			m_pInterface->ItemShopDelShopIcon(m_dwGaeaID);*/
	}
//#endif
}

HRESULT GLCharacter::ReqSkillHoldReset( SNATIVEID& sSkillID, WORD wSlot )
{
	if (wSlot >= SKILLFACT_SIZE)
		return E_FAIL;

	if (m_sSKILLFACT[wSlot].sNATIVEID != sSkillID)
		return E_FAIL;

	GLMSG::SNETPC_REQ_SKILLHOLD_RS NetMsgReq;
	NetMsgReq.dwID       = m_dwGaeaID;
	NetMsgReq.sSKILLID	 = sSkillID;
	NetMsgReq.wRESETSLOT = wSlot;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsgReq);

	return S_OK;
}

void GLCharacter::ReqSkillHoldResetForWidget( DWORD dwID, WORD wSlot )
{
	SNATIVEID sID( dwID );
	ReqSkillHoldReset( sID, wSlot );
}

HRESULT GLCharacter::ReqAttendList ( bool bDay )
{
	if ( m_bReqAttendList && !bDay )
        return S_OK;

	m_bReqAttendList = true;

	//! �޽��� ����.
	GLMSG::SNETPC_REQ_ATTENDLIST NetMsg;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqAttendance()
{
	//	�������ӽð� üũ
	const CTime& cCurTime = m_pGaeaClient->GetCurrentTime();
	CTime cDayTime( cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(),0,0,0 );
	CTime cLoginTime;
	if ( m_tAttendLogin < cDayTime.GetTime()  ) cLoginTime = cDayTime;
	else cLoginTime = m_tAttendLogin;

	CTimeSpan cGameTime = cCurTime - cLoginTime;
	if ( cGameTime.GetTotalMinutes() < m_dwAttendTime ) 
	{		
		m_pGaeaClient->PrintMsgText(
            NS_UITEXTCOLOR::DISABLE,
            sc::string::format(ID2GAMEINTEXT("EMREQ_ATTEND_FB_ATTENTIME"),m_dwAttendTime));
		return S_FALSE;
	}

	//! �޽��� ����.
	GLMSG::SNETPC_REQ_ATTENDANCE NetMsg;
	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	return S_OK;
}


HRESULT GLCharacter::InvenUseTaxiCard( WORD wPosX, WORD wPosY )
{
	//	�������� ����Ҽ� ���� ��Ȳ�̴�.
	if ( IsActionLimit( EMACTION_LIMIT_ITEM | EMACTION_LIMIT_CARD ) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_DRUG_LIMIT_ITEM") );
		return E_FAIL;
	}

	SITEM* pVehicleItem = GET_SLOT_ITEMDATA( SLOT_VEHICLE );	
	if (pVehicleItem)
	{
        if (m_emVehicle == EMVEHICLE_DRIVER && !vehicle::CanUseTeleportItem(pVehicleItem->VehicleType()))
		{
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_BIKE"));
			return E_FAIL;                        
		}
		else if (m_emVehicle == EMVEHICLE_PASSENGER)
		{
			m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_BIKE"));
			return E_FAIL;
		}
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMTAXI_TAKE_TICKET") );
		return E_FAIL;
	}
	
	GLTaxiStationClient::GetInstance().OpenTaxiCard( wPosX, wPosY );

	return S_OK;
}

HRESULT GLCharacter::InvenUseNpcRecall( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NOITEM") );
		return E_FAIL;
	}

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_NPC_RECALL_FB_NOITEM") );
		return E_FAIL;
	}


	//	���� ��û

	GLMSG::SNET_INVEN_NPC_RECALL NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqItemMix( DWORD dwNpcGlobID )
{
    // NPC ��ȿ�� üũ
    PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
    std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);
    if (!spCrow)
        return E_FAIL;

    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        return E_FAIL;

	SINVENITEM	sInvenItem[ ITEMMIX_ITEMNUM ];
	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		sInvenItem[i].sItemCustom = GET_ITEM_MIX( i );
		sInvenItem[i].wPosX = m_sProductMaterialPos[i].wPosX;
		sInvenItem[i].wPosY = m_sProductMaterialPos[i].wPosY;
	}

	GLProductRecipeMan::GetInstance().SortInvenItem( sInvenItem );	

	SProductRecipe sProductRecipe;
	for( int i=0; i<ITEMMIX_ITEMNUM; ++i ) 
	{
		sProductRecipe.sMeterialItem[i].sNID = GET_ITEM_MIX( i ).GetNativeID();

		if ( sProductRecipe.sMeterialItem[i].sNID != NATIVEID_NULL() )
			sProductRecipe.sMeterialItem[i].nNum = (BYTE)GET_ITEM_MIX( i ).wTurnNum;
	}

	GLProductRecipeMan::GetInstance().SortMeterialItem( sProductRecipe );

	const SProductRecipe* pProductRecipe;

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if (pGlobalStage->IsEmulator())
	{
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeEmulator( sProductRecipe );
	}
	else
	{
		pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipeClient( sProductRecipe );
	}
	
	if ( !pProductRecipe ) 
	{
		//	���� ������ ����
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX") );
		//!! SF_TODO
		//m_pInterface->SetItemMixResult( ID2GAMEINTEXT("EMITEM_MIX_FB_NOMIX") );
		return E_FAIL;
	}

	if ( GetInvenMoney() < sProductRecipe.dwPrice ) 
	{
		//	�ݾ� ����
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMITEM_MIX_FB_NOMONEY") );
		//!! SF_TODO
		//m_pInterface->SetItemMixResult( ID2GAMEINTEXT("EMITEM_MIX_FB_NOMONEY") );
		return E_FAIL;
	}

	GLMSG::SNET_INVEN_ITEM_MIX sNetMsg;
	sNetMsg.dwKey = pProductRecipe->dwKey;
	for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
	{
		sNetMsg.sInvenPos[i].wPosX = sInvenItem[i].wPosX;
		sNetMsg.sInvenPos[i].wPosY = sInvenItem[i].wPosY;
	}
	sNetMsg.dwNpcGlobID = dwNpcGlobID;
	m_pGaeaClient->NETSENDTOFIELD(&sNetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqGathering( const STARGETID& sTargetID )
{
	PLANDMANCLIENT pLAND = m_pGaeaClient->GetActiveMap();
	if ( !pLAND )
		return E_FAIL;

	{
		//! �޽��� �۽����� ��ȿ������ �̸� �˻���.
		//
		// ���Ȯ��
		if ( !IsValidBody() )	return E_FAIL;
		if ( isAction( GLAT_GATHERING ) )	return E_FAIL;

		//	�Ÿ� üũ
		const D3DXVECTOR3 &vTarPos = sTargetID.vPos;

		//	�Ÿ� üũ
		const D3DXVECTOR3& vPosition = GetPosition();

		D3DXVECTOR3 vDistance = vPosition - vTarPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		WORD wTarBodyRadius = 4;
		WORD wGatherRange = wTarBodyRadius + GETBODYRADIUS() + 2;
		WORD wGatherAbleDis = wGatherRange + 10;

		if ( fDistance>wGatherAbleDis )
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_DISTANCE") );			
			return E_FAIL;
		}

		if ( sTargetID.emCrow!=CROW_MATERIAL )	
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMREQ_GATHER_FB_NOTTYPE") );			
			return E_FAIL;
		}
		
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = pLAND->GetMaterial( sTargetID.GaeaId );
		if (!spMaterial)
		{
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMGATHER_FB_NOCROW") );			
			return E_FAIL;		
		}

		//	�޽��� �߻�.
		GLMSG::SNETPC_REQ_GATHERING NetMsg;
		NetMsg.dwTargetID = sTargetID.GaeaId;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}

	return S_OK;
}

HRESULT GLCharacter::ReqCancelGathering()
{
	//!! SF_TODO
	//m_pInterface->UiHideGroup( GATHER_GAUGE );
	return S_OK;
}


HRESULT GLCharacter::ReqNpcTalkBasic ( DWORD dwNpcGlobID, DWORD nAction, DWORD dwTalkID )
{
	GLMSG::SNETPC_REQ_NPCTALK_BASIC NetMsg;

	PLANDMANCLIENT pLandMan = m_pGaeaClient->GetActiveMap ();
	std::tr1::shared_ptr<GLCrowClient> spCrow = pLandMan->GetCrow(dwNpcGlobID);

	if (!spCrow)	
        return E_FAIL;

    // NPC ��ȿ�� üũ
    if (!spCrow->IsNpcActionable(GetPosition(), GETBODYRADIUS()))
        return E_FAIL;

	//! NPC_TALK ��û
	NetMsg.dwNpcGlobID = dwNpcGlobID;
	NetMsg.nAction = nAction;
	NetMsg.dwTalkID = dwTalkID;
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqPartyLinkJoin ( DWORD dwMasterID, int nLinkIndex)
{		
	if ( m_pGaeaClient->GetMyPartyClient()->isValid() == true )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_JOIN_FB_REJOIN") );
		return E_FAIL;
	}

	if ( dwMasterID == m_dwGaeaID )
        return E_FAIL;

	//! ��Ƽ�� ���� ��û.
	//
	GLMSG::SNET_PARTY_LINK_JOIN NetMsg(dwMasterID);

	m_pGaeaClient->NETSENDTOAGENT(&NetMsg);

	//	�ѹ��� ��û �Ҽ� �ְ� �����͸� �����صд�.		
	GLChatMan::GetInstance().SetPartyLinkOnce( nLinkIndex );

	return S_OK;
}

HRESULT GLCharacter::ReqPartyLinkLure ( bool bLure, const DWORD dwHashKey, const DWORD dwTimeKey, const DWORD dwGaeaID )
{
	GLPartyClient* const pMyParty = m_pGaeaClient->GetMyPartyClient();
	if ( (pMyParty->isValid() == true) && (pMyParty->isMaster(m_dwGaeaID) == false) )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMPARTY_LINK_LURE_FB_NOMASTER") );
		return E_FAIL;
	}

	// ��Ƽ ����
	if ( bLure ) 
	{
		SPartyOption sOption(RANPARAM::dwPARTY_GET_ITEM, RANPARAM::dwPARTY_GET_MONEY);

		//! ��Ƽ�� ���� ��û.
		//
		GLMSG::SNET_PARTY_LINK_LURE NetMsg(dwHashKey, dwGaeaID, dwTimeKey, sOption );	

		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}
	else
	{
		GLMSG::SNET_PARTY_LINK_REFUSE NetMsg( dwGaeaID );
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);
	}

	return S_OK;
}

HRESULT GLCharacter::ReqSendChatMsg ( DWORD dwChatType, const char* szName, const char* szMsg, const VEC_LINK_DATA_BASIC& vecLinkDataBasic )
{
	GLMSG::NET_CHAT_LINK NetMsgChat;	

	//for ( DWORD i = 0; i < vecLinkDataBasic.size(); ++i ) 
	//{
		NetMsgChat.sLinkDataBasic = vecLinkDataBasic;
	//}

	switch ( dwChatType ) 
	{
	case CHAT_CONST::CHAT_NORMAL:		
		{
			NetMsgChat.m_ChatType	 = CHAT_TYPE_NORMAL;
			NetMsgChat.m_ChaName = szName;
			NetMsgChat.m_ChatMsg = szMsg;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
			m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_PRIVATE:		
		{
			NetMsgChat.m_ChatType	 = CHAT_TYPE_PRIVATE;
			NetMsgChat.m_ChaName = szName;
			NetMsgChat.m_ChatMsg = szMsg;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_PARTY:
		{
			NetMsgChat.m_ChatType    = CHAT_TYPE_PARTY;
			NetMsgChat.m_ChatMsg = szMsg;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_MASTER:
		{
			NetMsgChat.m_ChatType    = CHAT_TYPE_MASTER;
			NetMsgChat.m_ChatMsg = szMsg;

			msgpack::sbuffer SendBuffer;
			msgpack::pack(SendBuffer, NetMsgChat);
			m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_GUILD:
		{
			NetMsgChat.m_ChatType	 = CHAT_TYPE_GUILD;
			NetMsgChat.m_ChatMsg = szMsg;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_ALLIANCE:
		{
			NetMsgChat.m_ChatType    = CHAT_TYPE_ALLIANCE;
			NetMsgChat.m_ChatMsg = szMsg;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_AREA:
		{
			if (!GLCONST_CHAR::bCHAT_EXTEND)
                return S_FALSE;
			
			if (GetInvenMoney() < GLCONST_CHAR::dwCHAT_AREA_CHARGE)
			{
				m_pGaeaClient->PrintMsgTextDlg(NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_AREA_NOMONEY"));
				return S_FALSE;
			}	

			// ����ä�� ����û
			NetMsgChat.m_ChatType = CHAT_TYPE_AREA;
			NetMsgChat.m_ChatMsg  = szMsg;
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);

		}
		break;
	case CHAT_CONST::CHAT_PARTY_RECRUIT:
		{
			if ( !GLCONST_CHAR::bCHAT_EXTEND ) return S_FALSE;
			
			if ( GetInvenMoney() < GLCONST_CHAR::dwCHAT_PARTY_RECRUIT_CHARGE )
			{
				m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_NOMONEY") );
				return S_FALSE;
			}

			if ( m_fPartyRecruitTime < GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME )
			{
				int nTime = (int)GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME;				
				m_pGaeaClient->PrintMsgTextDlg(
                    NS_UITEXTCOLOR::DISABLE,
                    sc::string::format(ID2GAMEINTEXT("EMCHAT_PARTY_RECRUIT_TIME"), nTime / 60));
				return S_FALSE;
			}

			// ��Ƽã�� ��� ��û
			NetMsgChat.m_ChatType = CHAT_TYPE_PARTY_RECRUIT;
			NetMsgChat.m_ChatMsg  = szMsg;
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	case CHAT_CONST::CHAT_TOALL:	
		//	�ٸ� �Լ����� ó��
		return this->ReqLoudSpeaker(szMsg, vecLinkDataBasic);

	case CHAT_CONST::CHAT_FACTION:
		{
			NetMsgChat.m_ChatType = CHAT_TYPE_FACTION;
			NetMsgChat.m_ChaName = szName;
			NetMsgChat.m_ChatMsg = szMsg;

			msgpack::sbuffer SendBuffer;
			msgpack::pack( SendBuffer, NetMsgChat );
			m_pGaeaClient->NETSENDTOFIELD( NET_MSG_CHAT_LINK, SendBuffer );
		}
		break;

	case CHAT_CONST::CHAT_SYSTEM:
		{
			NetMsgChat.m_ChatType	 = CHAT_TYPE_GLOBAL;
			NetMsgChat.m_ChatMsg = szMsg;
			
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, NetMsgChat);
            m_pGaeaClient->NETSENDTOFIELD(NET_MSG_CHAT_LINK, SendBuffer);
		}
		break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

//! �ν�Ʈ ���� ��û
HRESULT GLCharacter::ReqInvenBikeBoost ( WORD wPosX, WORD wPosY )
{
	//if ( !IsValidBody() )					return E_FAIL;
	//if ( m_pGaeaClient->GetTradeClient()->Valid() )	return E_FAIL;
	//if ( ValidRebuildOpen() )	return E_FAIL;	// ITEMREBUILD_MARK

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )	
        return E_FAIL;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_BIKE_BOOST_CARD )
	{
        m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_NOT_BOOST_CARD") );
		return E_FAIL;
	}

	if ( m_sVehicle.m_dwBoosterID != 0 )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_NOT_BOOST_LEARNING") );
		return E_FAIL;
	}

	SITEM* pVehicleItem = GLogicData::GetInstance().GetItem ( GET_SLOT_ITEM( SLOT_VEHICLE ).GetNativeID() );
	if ( !pVehicleItem )
        return E_FAIL;

	if ( pVehicleItem->sVehicle.emVehicleType != pItem->sVehicle.emVehicleType )
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_NOT_BOOST_DIFFRENT_TYPE") );
		return E_FAIL;
	}

	if ( pVehicleItem->sVehicle.emPartsType != pItem->sVehicle.emPartsType ) 
	{
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMBIKE_NOT_BOOST_DIFFRENT_BIKE") );
		return E_FAIL;
	}

	//! �ν�Ʈ ������ ��û�մϴ�.
	//
	GLMSG::SNETPC_REQ_BIKE_BOOST NetMsg;
	NetMsg.wPosX = wPosX;
	NetMsg.wPosY = wPosY;

	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}

HRESULT GLCharacter::ReqChangeTitle( DWORD dwIndex, bool bSync )
{
    GLMSG::SNET_ACTIVITY_CHANGE_BADGE NetMsg;

    // ���� ����
    if ( dwIndex == TITLE_INDEX_NONE )
    {
        TITLEINFO title;
        if ( !m_TitleManager.GetCurrentTitle ( title ) )
        {
            return E_FAIL;
        }

        //   ���� ���°ɷ� ����
        if ( !m_TitleManager.Select( dwIndex ) )
            return E_FAIL;
        
        NetMsg.nIndex = dwIndex;
    }
    else
    {
        TITLEINFO* pTitle = m_TitleManager.GetTitleInfo( dwIndex );
        if ( !pTitle )
            return E_FAIL;

        if ( pTitle->bSelected && !bSync)
            return E_FAIL;

        
        if ( !m_TitleManager.Select( dwIndex ) )
            return E_FAIL;
          
        NetMsg.nIndex = dwIndex;
    }

    RANPARAM::dwCharacterTitleIndex = dwIndex;

    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

    return S_OK;
}

HRESULT GLCharacter::ReqClickTrigger( const STARGETID& _target )
{
    GLMSG::NET_INSTANCE_CLICK_TRIGGER NetMsg(_target);
    m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
    return S_OK;
}

HRESULT GLCharacter::ReqPostBoxSendPageAttachItem()
{
	SITEMCUSTOM sITEMCUSTOM = GET_PREHOLD_ITEM();

	if ( sITEMCUSTOM.GetNativeID() != NATIVEID_NULL () )
	{
		const SITEM* pITEM = GLogicData::GetInstance().GetItem ( sITEMCUSTOM.GetNativeID() );
		if ( !pITEM )
			return E_FAIL;

		// ��ī���̸鼭 ���� Ȱ��ȭ �Ǿ� ������ �ŷ��� �� ����.
		if ( pITEM->sBasicOp.emItemType == ITEM_PET_CARD )
		{
			GLPetClient* pPet = m_pGaeaClient->GetPetClient();
			if ( pPet && pPet->IsVALID() && sITEMCUSTOM.PetDbNum() == pPet->DbNum())
			{
				m_pGaeaClient->PrintMsgText( 
					NS_UITEXTCOLOR::DISABLE, 
					ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_TRADING" ) );
				return E_FAIL;
			}
		}

		// Ż���̸� �ŷ��Ҽ� ����.
		if ( pITEM->sBasicOp.emItemType == ITEM_VEHICLE && sITEMCUSTOM.m_VehicleDbNum != 0 )
		{
			m_pGaeaClient->PrintMsgText( 
				NS_UITEXTCOLOR::DISABLE, 
				ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_TRADING" ) );
			return E_FAIL;
		}

		// ���� �׽�Ʈ �� ���Ǽ��� ���� GM�� �ŷ� �ɼ�, �Ⱓ�� ������ üũ���� �ʵ��� ������
		if ( m_dwUserLvl < USER_USER_GM )
		{
			//	�ŷ��ɼ�
			// GMCharEdit �� ���� �������� ������ �Ұ����ϴ�.
			if ( !( sITEMCUSTOM.IsExchange() != NULL ) || sITEMCUSTOM.IsGM_GENITEM() )
			{
				m_pGaeaClient->PrintMsgText( 
					NS_UITEXTCOLOR::DISABLE, 
					ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_TRADING" ) );
				return E_FAIL;
			}

			// �ð��� ������ Ȯ��
			if ( sITEMCUSTOM.IsTimeLimit() )
			{
				m_pGaeaClient->PrintMsgText( 
					NS_UITEXTCOLOR::DISABLE, 
					ID2GAMEINTEXT( "POSTBOX_ERROR_MSG_SEND_ITEM_LIMIT" ) );
				return E_FAIL;
			}
		}

		SetPostItemPos( GetPreHoldItemPos() );
		SetPostItem( sITEMCUSTOM );
		m_sPreInventoryItem = SINVEN_POS();

		return S_OK;
	}

	return E_FAIL;
}

void GLCharacter::ReqCostumeStatCToS()
{
	if ( !IsValidBody() )
		return;

	CCostumeStatClient* csClient = m_pGaeaClient->GetCostumeClient();

	if( csClient == NULL ) return;

	const SINVENITEM* pHoldInvenItem  = csClient->GetSrcItem();
	const SINVENITEM* pApplyInvenItem = csClient->GetDstItem();

	if( pHoldInvenItem == NULL || pApplyInvenItem == NULL ) return;

	SITEM* pHoldItem  = GLogicData::GetInstance().GetItem( pHoldInvenItem->GetNativeID()  );
	SITEM* pApplyItem = GLogicData::GetInstance().GetItem( pApplyInvenItem->GetNativeID() );

	GLMSG::SNET_INVEN_COSTUME_STATS NetMsg;

	if( csClient->GetData( NetMsg.sInvestInfo ) )
	{
		NetMsg.wPosX     = pApplyInvenItem->wPosX;
		NetMsg.wPosY     = pApplyInvenItem->wPosY;
		NetMsg.wHoldPosX = pHoldInvenItem->wPosX;
		NetMsg.wHoldPosY = pHoldInvenItem->wPosY;

		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
	}
}

void GLCharacter::ReqCostumeStat( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
		return;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return;

	if ( !holditem::HaveHoldItem() )
		return;

	if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		return;

	SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
		return;
	
	SINVENITEM* pHoldItem = m_cInventory.FindPosItem(holditem::GetHoldPosX(), holditem::GetHoldPosY());

	if ( pInvenItem->sItemCustom.nidDISGUISE == NATIVEID_NULL() && !pInvenItem->sItemCustom.IsDisguise() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG1") );
		return;
	}

	if( !COSSDATA.IsPartsData(pItem->sSuitOp.emSuit))
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG5") );
		return;
	}

	if( pInvenItem->sItemCustom.nidDISGUISE != NATIVEID_NULL() && pInvenItem->sItemCustom.tDISGUISE != 0) {

		CTime tTime(pInvenItem->sItemCustom.tDISGUISE + pInvenItem->sItemCustom.GetPeriodExDisguise());
		CTime tStatTime = CTime::GetCurrentTime();

		tStatTime += pHold->sDrugOp.tDuration;

		m_pGaeaClient->GetConvertServerTime(tTime);
		m_pGaeaClient->GetConvertServerTime(tStatTime);

		if( tStatTime >= tTime )
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG4") );
			holditem::ReleaseHoldItem();
			return;
		}
	}
	else
	{
		CTime tTime(pInvenItem->sItemCustom.tBORNTIME);
		CTime tStatTime = CTime::GetCurrentTime();

		tTime     += pItem->sDrugOp.tTIME_LMT;
		tStatTime += pHold->sDrugOp.tDuration;
		
		m_pGaeaClient->GetConvertServerTime(tTime);
		m_pGaeaClient->GetConvertServerTime(tStatTime);
		
		if( tStatTime >= tTime && pInvenItem->sItemCustom.IsTimeLimit(SITEMCUSTOM::CHECKFLAGOP_NORMAL))
		{
			m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG4") );
			holditem::ReleaseHoldItem();
			return;
		}
	}

	CCostumeStatClient* pCs = m_pGaeaClient->GetCostumeClient();
	if( NULL != pCs && false == pCs->SetItemInfo( pHoldItem, pInvenItem ) )
	{
		holditem::ReleaseHoldItem();
		return;
	}

	if( pInvenItem->CustomItem().IsCSApplied() )
	{
		WORD  wLvPoint = 1;

		if( pHoldItem && pInvenItem )
			wLvPoint = COSSDATA.GetPartsEnableMaxPt( pItem->sSuitOp.emSuit, (WORD)pHold->sDrugOp.dwCureVolume);

		if( wLvPoint == 0 )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Error,
				"-n", (int) NSCostumeStat::EMError_NoPoint );
		}
		else
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Use,
				"-n, -n, -n",
				(int) NSCostumeStat::EMQuestion_Replace, wPosX, wPosY );
		}
		
		holditem::ReleaseHoldItem();
		return;
	}

	if( pItem->IsBasicStatExist() )
	{
		WORD  wLvPoint = 1;

		if( pHoldItem && pInvenItem )
			wLvPoint = COSSDATA.GetPartsEnableMaxPt( pItem->sSuitOp.emSuit, (WORD)pHold->sDrugOp.dwCureVolume);

		if( wLvPoint == 0 )
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Error,
				"-n", (int) NSCostumeStat::EMError_NoPoint );
		}
		else
		{
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Use,
				"-n, -n, -n",
				(int) NSCostumeStat::EMQuestion_Expiration, wPosX, wPosY );
		}
		
		holditem::ReleaseHoldItem();
		return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Use,
		"-n, -n, -n", -1, wPosX, wPosY );

	holditem::ReleaseHoldItem();
}

HRESULT GLCharacter::ReqJumpingLevelUp ( int nExp )
{
	GLMSG::SNETPC_JUMPING_LEVLEUP_BYEXP NetMsg;
	NetMsg.m_nExp = nExp;	
	m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

	return S_OK;
}


HRESULT GLCharacter::ReqJumpingLevelUp ( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_NOITEM") );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType == ITEM_EXP_CARD )
	{
		GLMSG::SNETPC_REQ_JUMPINGLEVELUP_CA NetMsg;
		NetMsg.wPosX = wPosX;
		NetMsg.wPosY = wPosY;
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);

		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;
	}
	else
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CARD_TYPEERROR") );
		return E_FAIL;
	}

	return S_OK;
}

void GLCharacter::ReqCostumeStatRelease( WORD wPosX, WORD wPosY )
{
	if ( !IsValidBody() )
		return;

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
		return;

	wPosX = pInvenItem->wPosX;
	wPosY = pInvenItem->wPosY;

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
		return;

	if ( !holditem::HaveHoldItem() )
		return;

	if ( !holditem::IsHoldPlace( holditem::INVENTORY ) )
		return;

	SITEM* pHold = GLogicData::GetInstance().GetItem( holditem::GetHoldItem().Id() );
	if ( !pHold )
		return;

	SINVENITEM* pHoldItem = m_cInventory.FindPosItem(holditem::GetHoldPosX(), holditem::GetHoldPosY());

	if( !pInvenItem->CustomItem().IsCSApplied() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("ERROR_COSTUME_MSG7") );
		holditem::ReleaseHoldItem();
		return;
	}
	else
	{
		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;
		m_wInvenPosX2 = holditem::GetHoldPosX();
		m_wInvenPosY2 = holditem::GetHoldPosY();

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_CostumeStat_Release,
			"-n, -n, -n, -n",
			wPosX, wPosY, holditem::GetHoldPosX(), holditem::GetHoldPosY() );

		holditem::ReleaseHoldItem();

		return;
	}


	holditem::ReleaseHoldItem();
}

void GLCharacter::ReqCostumeStatReleaseOK( WORD wPosX, WORD wPosY, WORD wHoldPosX, WORD wHoldPosY )
{
	GLMSG::SNET_INVEN_COSTUME_RELEASE NetMsg;

	NetMsg.wPosX	 = wPosX;
	NetMsg.wPosY	 = wPosY;
	NetMsg.wHoldPosX = wHoldPosX;
	NetMsg.wHoldPosY = wHoldPosY;
	NetMsg.Crc32 = sc::string::getCrc32( &NetMsg, NetMsg.Size() - sizeof( unsigned int ) );

	m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
}

HRESULT GLCharacter::ReqSelformBoxSelect(WORD wSelItemPos)
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(m_nSelFormBoxPosX, m_nSelFormBoxPosY);
	if ( !pInvenItem )
		return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem || pItem->sBasicOp.emItemType!=ITEM_SELECTIVEFORM_BOX )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("SELFORMBOX_MSG", 0) );
		return E_FAIL;
	}

	if ( !pItem->sBox.VALID() )
	{
		m_pGaeaClient->PrintMsgTextDlg ( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD("SELFORMBOX_MSG", 0));
		return E_FAIL;
	}

	GLMSG::SNET_INVEN_SELFORMBOX_SELECTOPEN netMsg;
	netMsg.nBoxPosX = m_nSelFormBoxPosX;
	netMsg.nBoxPosY = m_nSelFormBoxPosY;
	netMsg.nSelPos = wSelItemPos;

	m_pGaeaClient->NETSENDTOFIELD(&netMsg);
	m_nSelFormBoxPosX = m_nSelFormBoxPosY = -1;
	return S_OK;
}

HRESULT GLCharacter::ReqInvenExpCompressor( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMPRESSOR_NOTEXIST") );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType == ITEM_EXP_COMPERSSOR  )
	{
		/*GLMSG::SNETPC_EXP_COMPRESSOR_REQ NetMsg;
		NetMsg.wPosX = wPosX;
		NetMsg.wPosY = wPosY;	
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);*/

		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_UseExpCompressorItem,
			"-n, -n", 
			wPosX, wPosY );
	}
	else
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_COMORESSOR_ITEMTYPE") );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenExpCapsule( WORD wPosX, WORD wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem(wPosX, wPosY);
	if ( !pInvenItem )
        return E_FAIL;

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULE_NOTEXIST") );
		return E_FAIL;
	}

	if ( pItem->sBasicOp.emItemType == ITEM_EXP_CAPSULE  )
	{
		/*GLMSG::SNETPC_EXP_CAPSULE_REQ NetMsg;
		NetMsg.wPosX = wPosX;
		NetMsg.wPosY = wPosY;	
		m_pGaeaClient->NETSENDTOFIELD(&NetMsg);*/

		m_wInvenPosX1 = wPosX;
		m_wInvenPosY1 = wPosY;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_UseExpCapsuleItem,
			"-n, -n", 
			wPosX, wPosY );
	}
	else
	{
		m_pGaeaClient->PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("EMINVEN_EXP_CAPSULER_ITEMTYPE") );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenChangeItemSkill ( WORD _wPosX, WORD _wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( _wPosX, _wPosY );
	if ( NULL == pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	if ( ITEM_CHANGE_ITEMSKILL_CARD == pItem->sBasicOp.emItemType )
	{
		GLMSG::SNET_INVEN_CHANGE_ITEMSKILL_UIOPEN sNetMsg;

		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	}
	else
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMINVEN_EXP_CARD_TYPEERROR" ) );

		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenChangeBasicStats ( WORD _wPosX, WORD _wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( _wPosX, _wPosY );
	if ( NULL == pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	if ( ITEM_CHANGE_BASICSTATS_CARD == pItem->sBasicOp.emItemType )
	{
		GLMSG::SNET_INVEN_CHANGE_BASICSTATS_UIOPEN sNetMsg;

		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	}
	else
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMINVEN_EXP_CARD_TYPEERROR" ) );

		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenChangeAddStats ( WORD _wPosX, WORD _wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( _wPosX, _wPosY );
	if ( NULL == pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMREQ_REMODELOPEN_FB_NOITEM" ) );

		return E_FAIL;
	}

	if ( ITEM_CHANGE_ADDSTATS_CARD == pItem->sBasicOp.emItemType )
	{
		GLMSG::SNET_INVEN_CHANGE_ADDSTATS_UIOPEN sNetMsg;

		m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
	}
	else
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMINVEN_EXP_CARD_TYPEERROR" ) );

		return E_FAIL;
	}

	return S_OK;
}

HRESULT GLCharacter::ReqInvenUnlockCharacterSlot ( WORD _wPosX, WORD _wPosY )
{
	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( _wPosX, _wPosY );
	if ( NULL == pInvenItem )
	{
		m_pGaeaClient->PrintMsgTextDlg(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "TEXT_REQ_UNLOCK_CHARACTER_SLOT_NOITEM_MESSAGE" ) );

        return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
	if ( NULL == pItem )
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "TEXT_REQ_UNLOCK_CHARACTER_SLOT_NOITEM_MESSAGE" ) );

		return E_FAIL;
	}

	if ( ITEM_UNLOCK_CHARACTER_SLOT == pItem->sBasicOp.emItemType )
	{
		m_wInvenPosX1 = _wPosX;
		m_wInvenPosY1 = _wPosY;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_UseUnlockCharacterSlotItem,
			"-n, -n", 
			_wPosX, _wPosY );
	}
	else
	{
		m_pGaeaClient->PrintMsgText(
			NS_UITEXTCOLOR::DISABLE,
			ID2GAMEINTEXT( "EMINVEN_EXP_CARD_TYPEERROR" ) );

		return E_FAIL;
	}

	return S_OK;
}



void GLCharacter::ReqLockCharSlot ( const WORD _wCharSlotNum )
{
	if ( NULL == m_pGaeaClient )
		return;

	if ( true == m_arrLockSlotState[ _wCharSlotNum ] )
	{
		GLMSG::SNETPC_RESPONSE_LOCK_CHARSLOT_FC sNetMsg( EMCHARSLOT_LOCK_ALREADY );
		sNetMsg.dwChaNum = CharDbNum();

		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return;
	}

	GLMSG::SNETPC_REQUEST_LOCK_CHARSLOT_CF sNetMsg( _wCharSlotNum );
	sNetMsg.dwChaNum = CharDbNum();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

void GLCharacter::ReqUnlockCharSlot ( const WORD _wCharSlotNum )
{
	if ( NULL == m_pGaeaClient )
		return;

	if ( false == m_arrLockSlotState[ _wCharSlotNum ] )
	{
		GLMSG::SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC sNetMsg( EMCHARSLOT_UNLOCK_ALREADY );
		sNetMsg.dwChaNum = CharDbNum();

		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return;
	}

	GLMSG::SNETPC_REQUEST_UNLOCK_CHARSLOT_CF sNetMsg( _wCharSlotNum );
	sNetMsg.dwChaNum = CharDbNum();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

void GLCharacter::ReqChangeCharSlot ( const WORD _wCharSlotNum )
{
	if ( NULL == m_pGaeaClient )
		return;

	if ( false == IsChangeCharSlot( _wCharSlotNum ) )
		return;

	GLMSG::SNETPC_REQUEST_CHANGE_CHARSLOT_CF sNetMsg( _wCharSlotNum );
	sNetMsg.dwChaNum = CharDbNum();

	m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
}

const bool GLCharacter::IsChangeCharSlot ( const WORD _wCharSlotNum )
{
	if ( NULL == m_pGaeaClient )
		return false;

	GLMSG::SNETPC_RESPONSE_CHANGE_CHARSLOT_FC sNetMsg( EMCHARSLOT_CHANGE_FAIL );
	sNetMsg.dwChaNum = CharDbNum();

	if ( false == GLUseFeatures::GetInstance().IsUsingSelectCharSlot() )
	{
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	if ( _wCharSlotNum < 0 || EMCHAR_SLOT_DATA_SIZE <= _wCharSlotNum )
	{
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ���µǾ� ���� ���� �������� Change �� �� ����;
	if ( GLCharSlotMan::Instance()->GetOpenSlotCount() <= _wCharSlotNum )
	{
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ���� �������δ� ������ �� ����;
	if ( m_wSlotIndex == _wCharSlotNum )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_SAMESLOT;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ���� ���� ĳ���� ���� ����Ұ� ���̶�� ������ �� ����;
	PLANDMANCLIENT pLandManClient = m_pGaeaClient->GetActiveMap();
	if ( NULL == pLandManClient )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_LIMITMAP;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	const SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode( pLandManClient->GetBaseMapID ().dwID );
	if ( NULL == pMapNode || false == pMapNode->IsChangeCharSlot() )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_LIMITMAP;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ��Ÿ�� üũ�� Ŭ���̾�Ʈ���� ���� ���� �ʴ´�;


	// ĳ���Ͱ� ���� ���¶�� ������ �� ����;
	if ( true == IsDie() )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_DIE;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ĳ���Ͱ� �׼����̶�� ������ �� ����;
	if ( false == isAction( GLAT_IDLE ) )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_ACTION;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	// ������ ����ִ� �������� Ȯ���Ѵ�;
	if ( true == m_arrLockSlotState[ _wCharSlotNum ] )
	{
		sNetMsg.emFlag = EMCHARSLOT_CHANGE_LOCK;
		m_pGaeaClient->NETSENDTOMYSELF( &sNetMsg );

		return false;
	}

	return true;
}

void GLCharacter::SendCustomMessage ( const DWORD _dwDestType
									, const DWORD _dwInstanceDungeonID
									, const DWORD _dwParam1
									, const DWORD _dwParam2
									, const DWORD _dwParam3
									, const DWORD _dwParam4 )
{
	GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE sNetMsg( _dwParam1, _dwParam2, _dwParam3, _dwParam4 );
	sNetMsg.emDestType = _dwDestType;
	sNetMsg.dwDestID = _dwInstanceDungeonID;
	sNetMsg.emSrcType = InstanceSystem::EM_MESSAGE_USER;
	sNetMsg.dwSrcID = CharDbNum();

	switch ( _dwDestType )
	{
	case InstanceSystem::EM_MESSAGE_MY_INSTANCE:
		{
			m_pGaeaClient->NETSENDTOFIELD( &sNetMsg );
		}
		break;

	case InstanceSystem::EM_MESSAGE_FIELD_INSTANCE:
	case InstanceSystem::EM_MESSAGE_AGENT_BASE:
		{
			m_pGaeaClient->NETSENDTOAGENT( &sNetMsg );
		}
		break;
	default:
		break;
	}
}

HRESULT GLCharacter::InvenClubRename( WORD wPosX, WORD wPosY )
{
	if ( !m_pMyClub )
	{
		return E_FAIL;
	}

	SINVENITEM* pInvenItem = m_cInventory.FindPosItem( wPosX, wPosY );
	if ( !pInvenItem )
	{
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_WRONG_ITEM );
		return E_FAIL;
	}

	SITEM* pItem = GLogicData::GetInstance().GetItem ( pInvenItem->sItemCustom.GetNativeID() );
	if ( !pItem )
	{
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_WRONG_ITEM );
		return E_FAIL;
	}

	if ( CLUB_NULL == m_pMyClub->m_DbNum )
	{
		return S_FALSE;
	}

	if ( !m_pMyClub->IsMaster( m_CharDbNum ) )
	{
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_AUTH );
		return S_FALSE;
	}

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ClubRename );

	m_wInvenPosX1 = wPosX;
	m_wInvenPosY1 = wPosY;

	return S_OK;
}

HRESULT GLCharacter::ReqInvenClubRename ( const char* szClubName )
{
	// -----------------------------------------------------------------------------------------------------------------
	// ���ڿ� üũ
	if ( !szClubName )
	{
		return E_FAIL;
	}

	CString strClubName( szClubName );

	size_t NameLength = strClubName.GetLength();
	if ( NameLength == 0 )
	{
		// �̸��� �Է��ϼ���.
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_NONAME );
		return S_FALSE;
	}
	else if ( NameLength < 4 )
	{
		// �ʹ� ª���ϴ�.
		// �ѱ۱��� 2���� �̻� �Ǿ��, �������� 4���� �̻� �Ǿ��
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_TOO_SHORT );
		return S_FALSE;
	}
	else if ( NameLength > ( CHAR_SZNAME - 1 ) )
	{
		// �ʹ� ���.
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_TOO_LONG );
		return S_FALSE;
	}

	// ��ĭ üũ
	if ( strClubName.FindOneOf(" ") != -1 ) 
	{
		// �� ĭ�� ������ �ȵ˴ϴ�.
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_BLANK );
		return S_FALSE;
	}

	BOOL bFILTER0 = STRUTIL::CheckString( strClubName );;
	BOOL bFILTER1 = CRanFilter::GetInstance().NameFilter( strClubName );
	if ( bFILTER0 || bFILTER1 )
	{
		// ����� �� ���� ���ڰ� ���ԵǾ� �ֽ��ϴ�.
		m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_BAD_NAME );
		return S_FALSE;
	}

	if ( m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_OFFICE_TEST )
	{
		if ( !sc::string::goodChaNameKorea( std::string( strClubName.GetString() ) ) )
		{
			m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_BAD_NAME );
			return S_FALSE;
		}
	}

	// �±��� ���� ���� üũ
	if ( RENDERPARAM::emLangSet == language::THAILAND ) 
	{
		if ( !STRUTIL::CheckThaiString( strClubName ) )
		{
			m_pGaeaClient->PrintClubRenameFB( CLUB_RENAME_FB_BAD_NAME );
			return S_FALSE;
		}
	}
	// -----------------------------------------------------------------------------------------------------------------

	GLMSG::SNET_CLUB_RENAME_CF NetMsg;
	NetMsg.SetClubName( szClubName );
	NetMsg.wPosX = m_wInvenPosX1;
	NetMsg.wPosY = m_wInvenPosY1;
	m_pGaeaClient->NETSENDTOFIELD( &NetMsg );

	m_wInvenPosX1 = 0;
	m_wInvenPosY1 = 0;

	return S_OK;
}
