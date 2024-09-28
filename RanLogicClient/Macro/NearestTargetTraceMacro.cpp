
#include "pch.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./NearestTargetTraceMacro.h"

#include "./MacroManager.h"

#include "../SigmaCore/String/StringFormat.h"

NearestTargetTraceMacro::NearestTargetTraceMacro(GLGaeaClient* pGaeaClient)
: BaseMacro(pGaeaClient)
, m_dwTraceState(TRACE_NONE)
, m_vTargetPosition(0.0f, 0.0f, 0.0f)
, m_dwTraceStartState(TRACE_NONE)
{
	m_MacroType = MACRO::MACRO_NEARESTTARGETTRACE;
	// 전이 가능 매크로 등록;
	m_vecTransferableType.push_back(MACRO::MACRO_RETURNBYOPTION);
	m_vecTransferableType.push_back(MACRO::MACRO_AUTOMATICSKILLUSE);
	m_vecTransferableType.push_back(MACRO::MACRO_NEARESTTARGETTRACE);
}

NearestTargetTraceMacro::~NearestTargetTraceMacro(void)
{
}

void NearestTargetTraceMacro::DoInit(void)
{
	m_dwTraceState = m_dwTraceStartState;
}

void NearestTargetTraceMacro::DoUpdate(float fTime, float fElapsedTime)
{
}

void NearestTargetTraceMacro::DoApply(void)
{
	switch ( m_dwTraceState )
	{
	case TRACE_NONE:
		{
			GLMSG::SNET_MACROMODE_REQNEARESTCROWINFO_CF NetMsg;
			m_pGaeaClient->NETSENDTOFIELD(&NetMsg);
			m_dwTraceState = TRACE_READY;
		}
		break;
	case TRACE_READY:
		{
			m_vTargetPosition = m_pGaeaClient->GetMacroTraceTargetPos();

			GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
			m_dwTraceState = pCharacter->PlayerGotoLocation(m_vTargetPosition) == true ? TRACE_ACTION:TRACE_NONE;
		}
		break;
	}
}

void NearestTargetTraceMacro::DoEndProcedure(void)
{
	m_dwTraceState = m_dwTraceStartState;
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->ResetActionID();
}

bool NearestTargetTraceMacro::CheckTransferable(void)
{
	// 또들어왔다면 건너뛴다;
	// 이동 위치정보가 전송되어 전이될때는 이 검사를하지않는다;
// 	if ( GLMacroManager::GetInstance().GetActiveType() == m_MacroType )
// 		return false;

	return true;
}

bool NearestTargetTraceMacro::CheckEnd(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	const bool bCharCheck(pCharacter->GetAction() == GLAT_IDLE || pCharacter->GetAction() == GLAT_MOVE),
		bMacroCheck(GLMacroManager::GetInstance().IsMacroTransferable(MACRO::MACRO_RETURNBYOPTION) || GLMacroManager::GetInstance().IsMacroTransferable(MACRO::MACRO_AUTOMATICSKILLUSE)),
		bTraceEndCheck(m_dwTraceState == TRACE_ACTION && pCharacter->GetAction() == GLAT_IDLE);
	return (bTraceEndCheck||(bCharCheck && bMacroCheck));
}