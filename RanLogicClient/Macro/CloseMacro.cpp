
#include "pch.h"
#include "../Char/GLCharacter.h"
#include "../GLGaeaClient.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"
#include "./CloseMacro.h"

#include "./MacroManager.h"

CloseMacro::CloseMacro(GLGaeaClient* pGaeaClient)
: BaseMacro(pGaeaClient)
{
	m_MacroType = MACRO::MACRO_CLOSE;
}

CloseMacro::~CloseMacro(void)
{
}

void CloseMacro::DoInit(void)
{
}

void CloseMacro::DoUpdate(float fTime, float fElapsedTime)
{
}

void CloseMacro::DoApply(void)
{
	GLMacroManager::GetInstance().AddMessage(MACRO::EMMACRO_MSG_REQSERVER_ONOFF, false);
}

void CloseMacro::DoEndProcedure(void)
{
}

bool CloseMacro::CheckTransferable(void)
{
	// 이 상태가 콜되는 상태면 무조건 종료되어야되는게 맞다;
	return true;
}

bool CloseMacro::CheckEnd(void)
{
	GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
	if ( pCharacter->GetAction() == GLAT_IDLE && pCharacter->IsEmptyReservationAction() == true )
		return true;

	return false;
}