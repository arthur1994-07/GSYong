
#include "pch.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./AutomaticSkillMacro.h"
#include "./ReturnByOptionMacro.h"
#include "./NearestTargetTraceMacro.h"
#include "./CloseMacro.h"

#include "./MacroManager.h"

GLMacroManager& GLMacroManager::GetInstance(void)
{
	static GLMacroManager s_MacroManager;
	return s_MacroManager;
}

GLMacroManager::GLMacroManager(void)
: m_pGaeaClient(NULL)
, m_pCharacter(NULL)
, m_pActiveMacro(NULL)
, m_dwMacroState(MACRO::EMMACRO_STATE_NONE)
{
}

GLMacroManager::~GLMacroManager(void)
{
	ResetMacro();
}

void GLMacroManager::OneTimeSceneInit(void)
{
	InitMacro();
	/*--FIX MACRO ONLY USE F4 SID--*/
	//AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE, 0);
	//AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE, 1);
	//AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE, 2);
	AddMessage(MACRO::EMMACRO_MSG_AUTOMATICSKILLTAB_ENABLE, 3);

	GLWidgetScript::GetInstance().LuaCallFunc<void>( 
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_MacroStateUpdate);
}

void GLMacroManager::InitMacro(void)
{
	m_pCharacter = m_pGaeaClient->GetCharacter();

	// ResetMacro();

	if ( m_vecMacroModel.empty() == true )
	{
		BaseMacro* pMacro = NULL;	// MACRO::TYPE
		{
			// �ڵ� ��ų ���-��ũ��;
			pMacro = new AutomaticSkillMacro(m_pGaeaClient);
			m_vecMacroModel.push_back(pMacro);

			// ��ȯ-��ũ��;
			pMacro = new ReturnByOptionMacro(m_pGaeaClient);
			m_vecMacroModel.push_back(pMacro);

			// Ÿ�� ���� ����;
			pMacro = new NearestTargetTraceMacro(m_pGaeaClient);
			NearestTargetTraceMacro* pTraceMacro = static_cast<NearestTargetTraceMacro*>(pMacro);
			pTraceMacro->SetTraceState_ClientOnCall();
			m_vecMacroModel.push_back(pMacro);

			// ��ũ�� ����;
			pMacro = new CloseMacro(m_pGaeaClient);
			m_vecMacroModel.push_back(pMacro);
		}
	}

	bool bMacroUsed[MACRO::MACRO_SIZE] = { true, true, false, true };
	for ( INT i = 0; i < m_vecMacroModel.size(); ++i )
		m_vecMacroModel[i]->Init(bMacroUsed[i]);

	m_pActiveMacro = m_vecMacroModel[MACRO::MACRO_AUTOMATICSKILLUSE];
	m_dwMacroState = MACRO::EMMACRO_STATE_NONE;

	// �˶� �ʱ�ȭ;
	{
		struct MM_SORT
		{
		public:
			bool operator()( INT lhs, INT rhs )		{ return ( lhs > rhs ? true : false ); }
		};

		m_deqMacroRemainTimeAlram.clear();
		const INT nAlarmSize(GLCONST_CHAR::vecMacroRemainTimeAlarm.size())
			, nRemainTime((INT)m_pCharacter->GetRemainMacroTime());

		BOOST_FOREACH(const INT nAlarmTime, GLCONST_CHAR::vecMacroRemainTimeAlarm)
		{
			if ( nAlarmTime > 0 && nAlarmTime <= nRemainTime )
				m_deqMacroRemainTimeAlram.push_back(nAlarmTime);
		}

		std::sort(m_deqMacroRemainTimeAlram.begin()
			, m_deqMacroRemainTimeAlram.end()
			, MM_SORT());
	}
}

void GLMacroManager::Active(void)
{
	this->InitMacro();
	// ù Ȱ��ȭ-��ų �ڵ� ���;
	m_pCharacter->SetActState(EM_ACT_MACROMODE);
	if ( m_vecMacroModel.empty() == false )
	{
		m_pActiveMacro = m_vecMacroModel.front();
	}
	// ��ũ�� �ý��ۿ��� Ÿ������ ������ �� �ֵ����Ѵ�;
	m_pCharacter->ResetSelectID();
}

void GLMacroManager::ResetMacro(void)
{
	std::vector<BaseMacro*>::iterator itrOper = m_vecMacroModel.begin();
	for ( ; itrOper != m_vecMacroModel.end(); )
	{
		BaseMacro* pBase = *itrOper;
		switch (pBase->GetMacroType())
		{
		case MACRO::MACRO_AUTOMATICSKILLUSE:
			{
				AutomaticSkillMacro* pAutomaticSkillMacro = static_cast<AutomaticSkillMacro*>(pBase);
				delete pAutomaticSkillMacro;
			}
			break;
		case MACRO::MACRO_RETURNBYOPTION:
			{
				ReturnByOptionMacro* pReturnByOptionMacro = static_cast<ReturnByOptionMacro*>(pBase);
				delete pReturnByOptionMacro;
			}
			break;
		case MACRO::MACRO_NEARESTTARGETTRACE:
			{
				NearestTargetTraceMacro* pNearestTargetTraceMacro = static_cast<NearestTargetTraceMacro*>(pBase);
				delete pNearestTargetTraceMacro;
			}
			break;
		case MACRO::MACRO_CLOSE:
			{
				CloseMacro* pCloseMacro = static_cast<CloseMacro*>(pBase);
				delete pCloseMacro;
			}
			break;
		}
		itrOper = m_vecMacroModel.erase(itrOper);
	}
	m_vecMacroModel.clear();
	m_pActiveMacro = NULL;
	m_dwMacroState = MACRO::EMMACRO_STATE_NONE;
}

void GLMacroManager::FrameMove(float fTime, float fElapsedTime)
{
	this->MsgProcedure();
	this->Update(fTime, fElapsedTime);
}

void GLMacroManager::Update(float fTime, float fElapsedTime)
{
	// Ȱ��ȭ üũ;
	if ( IsActive() == false )
		return;

	// �˶� ������Ʈ;
	UpdateAlarm();

	// ���� �������� ���� ���̰ɸ� ���¶��;
	if ( IsLockByUserInput() == true )
	{
		if ( m_pCharacter->GetAction() == GLAT_IDLE && m_pCharacter->IsEmptyReservationAction() == true )
		{
			ReSetState(MACRO::EMMACRO_STATE_LOCK_USERINPUT);
		}
		return;
	}

	// ���� ������ ��ũ�ΰ� ������ �ٷ� �����Ѵ�;
	// ���� �켱������ ��ũ�� ���ο��� �����Ǿ��־���Ѵ�;
	// �� �켱������ ��Ȳ������ ���δٸ� ������ AI�� ���������� �ʹ�;
	// ��ũ�� ���� ó���� pMacro:EndProcedure -> Manager:Procedure -> pMacro-change;
	if ( m_pActiveMacro->IsEnd() )
	{
		// ���� ��ũ�� ��ó��;
		m_pActiveMacro->EndProcedure();
		const MACRO::VEC_TYPE& vecTransferT = m_pActiveMacro->GetTransferableType();
		for ( MACRO::VEC_TYPE::const_iterator itrMacroCur = vecTransferT.begin()
			; itrMacroCur != vecTransferT.end(); ++itrMacroCur )
		{
			const DWORD dwType = *itrMacroCur;
			if ( m_vecMacroModel[dwType]->IsTransferable() == true )
			{
				m_pActiveMacro = m_vecMacroModel[dwType];
				m_pActiveMacro->Apply();
				break;
			}
		}
	}

	{
		m_pActiveMacro->FrameMove(fTime, fElapsedTime);
	}
}

void GLMacroManager::UpdateAlarm(void)
{
	if ( m_deqMacroRemainTimeAlram.empty() )
		return;

	const INT nAlarmTime(m_deqMacroRemainTimeAlram[0]);
	if ((INT)m_pCharacter->GetRemainMacroTime() <= nAlarmTime)
	{
		m_deqMacroRemainTimeAlram.pop_front();
		m_pGaeaClient->PrintConsoleText(sc::string::format(ID2GAMEWORD("MACRO_REMAINTIMEALARM_SENTENSE"), nAlarmTime));
	}
}

bool GLMacroManager::IsActive(void) const
{
	return (m_dwMacroState&MACRO::EMMACRO_STATE_ACTIVE);
}

bool GLMacroManager::IsLockByUserInput(void) const
{
	return (m_dwMacroState&MACRO::EMMACRO_STATE_LOCK_USERINPUT);
}

bool GLMacroManager::IsMacroTransferable(DWORD dwMacroType) const
{
	return m_vecMacroModel[dwMacroType]->IsTransferable();
}

DWORD GLMacroManager::GetActiveType(void)
{
	return m_pActiveMacro->GetMacroType();
}