
#include "pch.h"
#include "../GLGaeaClient.h"

#include "./MacroDefine.h"
#include "./BaseMacro.h"

BaseMacro::BaseMacro(GLGaeaClient* pGaeaClient)
: m_pGaeaClient(pGaeaClient)
, m_MacroType(MACRO::MACRO_NONE)
, m_bUsable(false)
{
}

BaseMacro::~BaseMacro(void)
{
}

void BaseMacro::Init(bool bUsable)
{
	m_bUsable = bUsable;
	this->DoInit();
}

void BaseMacro::FrameMove( float fTime, float fElapsedTime )
{
	this->DoUpdate(fTime, fElapsedTime);
}

void BaseMacro::Apply(void)
{
	this->DoApply();
}

void BaseMacro::EndProcedure(void)
{
	this->DoEndProcedure();
}

bool BaseMacro::IsTransferable(void)
{
	if ( m_bUsable == false )
		return false;

	return this->CheckTransferable();
}

bool BaseMacro::IsEnd(void)
{
	return this->CheckEnd();
}