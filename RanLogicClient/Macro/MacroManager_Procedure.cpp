
#include "pch.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./AutomaticSkillMacro.h"
#include "./ReturnByOptionMacro.h"
#include "./NearestTargetTraceMacro.h"
#include "./MacroManager.h"

void GLMacroManager::MsgProcedure(void)
{
	DWORD type;
	DWORD dwMsg;
	while ( PopMessage(&type, &dwMsg) )
	{
		switch ( type )
		{
		case MACRO::EMMACRO_MSG_REQSERVER_ONOFF:
			{
				const bool bOn = static_cast<bool>(dwMsg);
				// 매크로 활성화시 체크된 스킬탭이 없다면 실행시키지 않는다;
				if ( bOn == true )
				{
					AutomaticSkillMacro* pMacro = static_cast<AutomaticSkillMacro*>(m_vecMacroModel[MACRO::MACRO_AUTOMATICSKILLUSE]);
					if ( pMacro->GetCheckTabCount() == 0 )
						return;
				}
				m_pCharacter->ReqMacroModeOnoff(bOn);
			}
			break;
		case MACRO::EMMACRO_MSG_ONOFF:
			{
				if ( GLUseFeatures::GetInstance().IsUsingMacro() == FALSE )
					return ;

				const SMAPNODE *pMapNode = m_pGaeaClient->FindMapNode ( m_pGaeaClient->GetActiveMapID().getBaseMapID() );
				if ( pMapNode == NULL || pMapNode->IsMacroUsable() == FALSE )
					return ;

				bool bActive = static_cast<bool>(dwMsg);
				if ( IsActive() == true && bActive == false )
				{
					ReSetState(MACRO::EMMACRO_STATE_ACTIVE);
					m_pGaeaClient->PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_TEXT", 1).GetString());
					m_pCharacter->ReSetActState(EM_ACT_MACROMODE);
				}
				else if ( IsActive() == false && bActive == true )
				{
					Active();
					SetState(MACRO::EMMACRO_STATE_ACTIVE);
					m_pGaeaClient->PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_TEXT", 0).GetString());
				}

				GLWidgetScript::GetInstance().LuaCallFunc<void>( 
					NSWIDGET_SCRIPT::g_strFunc_QuickMenu_MacroStateUpdate);
			}
			break;
		case MACRO::EMMACRO_MSG_LOCK_USERINPUT:
			SetState(MACRO::EMMACRO_STATE_LOCK_USERINPUT);
			if ( dwMsg == MACRO::EMMACRO_MSGVALUE_LOCK_USERINPUT_MOVE )
			{
				m_pCharacter->ResetSelectID();
			}
			break;
		case MACRO::EMMACRO_MSG_SERVEMSG_CLOSE:
			{
				m_pGaeaClient->PrintConsoleText(CGameTextMan::GetInstance().GetGameWord("MACRO_TEXT", 1).GetString());
				ReSetState(MACRO::EMMACRO_STATE_ACTIVE);
				m_pCharacter->ReSetActState(EM_ACT_MACROMODE);

				GLWidgetScript::GetInstance().LuaCallFunc<void>( 
					NSWIDGET_SCRIPT::g_strFunc_QuickMenu_MacroStateUpdate);
			}
			break;
		case MACRO::EMMACRO_MSG_RETURNBYOPTION_CHECKDRUG:
			{
				ReturnByOptionMacro* pMacro = static_cast<ReturnByOptionMacro*>(m_vecMacroModel[MACRO::MACRO_RETURNBYOPTION]);
				pMacro->CheckDrug(static_cast<bool>(dwMsg));
			}
			break;
		case MACRO::EMMACRO_MSG_RETURNBYOPTION_CHECKTIME:
			{
				ReturnByOptionMacro* pMacro = static_cast<ReturnByOptionMacro*>(m_vecMacroModel[MACRO::MACRO_RETURNBYOPTION]);
				pMacro->CheckTime(static_cast<bool>(dwMsg));
			}
			break;
		case MACRO::EMMACRO_MSG_TRACETARGETREQ_FEEDBACK:
			{
				const bool bFind(static_cast<bool>(dwMsg));
				if ( bFind == false )
				{
					m_vecMacroModel[MACRO::MACRO_NEARESTTARGETTRACE]->Init(true);
				}
				else
				{
					if ( m_pActiveMacro->GetMacroType() == MACRO::MACRO_NEARESTTARGETTRACE )
						m_pActiveMacro->Apply();
					else
						m_vecMacroModel[MACRO::MACRO_NEARESTTARGETTRACE]->Init(true);
				}
			}
			break;
		case MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE:
		case MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_DISABLE:
			{
				AutomaticSkillMacro* pMacro = static_cast<AutomaticSkillMacro*>(m_vecMacroModel[MACRO::MACRO_AUTOMATICSKILLUSE]);
				pMacro->SetSkillTabEnable(dwMsg,type==MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE);
			}
			break;
		case MACRO::EMMACRO_MSG_AUTOMATICSKILLSLOTINFO_UPDATE:
			{
				AutomaticSkillMacro* pMacro = static_cast<AutomaticSkillMacro*>(m_vecMacroModel[MACRO::MACRO_AUTOMATICSKILLUSE]);
				pMacro->SlotInfoUpdate();
			}
			break;
		case MACRO::EMMACRO_MSG_RESERVEACTIONMOVE_FAIL:
			{
				// 이상황이 되면 스킬을 사용하기위해 타겟으로 접근하다가;
				// 오브젝트에 걸려 이동하지 못하는 상태이다;
				// 시작 자체가 안될 수 있다;
				if ( m_pActiveMacro->GetMacroType() == MACRO::MACRO_AUTOMATICSKILLUSE )
				{
					// 강제 시작;
					if ( m_pCharacter->GetSelectID().emCrow != CROW_NULL )
					{
						m_pActiveMacro->EndProcedure();
						m_pGaeaClient->m_vMacroTracePos = m_pCharacter->GetSelectID().vPos;
						m_pCharacter->ClearReservationAction();
						NearestTargetTraceMacro* pTraceMacro = static_cast<NearestTargetTraceMacro*>(m_vecMacroModel[MACRO::MACRO_NEARESTTARGETTRACE]);
						pTraceMacro->Apply();
					}
				}
			}
			break;
		}
	}
}

bool GLMacroManager::AddMessage(DWORD type, DWORD dwMsg)
{
	MACRO::MACRO_MSG msg;
	msg.type = (MACRO::MACRO_MSGTYPE)type;
	msg.dwMessage = dwMsg;
	m_MsgQueue.push(msg);
	return true;
}

bool GLMacroManager::PopMessage(DWORD* type, DWORD* dwMsg)
{
	if ( !type || !dwMsg )	return false;
	if ( m_MsgQueue.empty() )	return false;
	const MACRO::MACRO_MSG& msg = m_MsgQueue.front();
	*type = msg.type;
	*dwMsg = msg.dwMessage;
	m_MsgQueue.pop();
	return true;
}