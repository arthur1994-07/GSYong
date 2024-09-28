#include "pch.h"
#include <boost/cast.hpp>
#include "../ActorState/GLActorState.h"
#include "GLActor.h"

GLActor::GLActor(const EMCROW emCrow, const bool bServer)
: m_emCrow(emCrow)
, m_dwGaeaID(GAEAID_NULL)
, m_vPosition(0.0f, 0.0f, 0.0f)
, m_vDirect(0.0f, 0.0f, -1.0f)
, m_vDirectOrig(0.0f, 0.0f, -1.0f)
, m_bLockMoveMent(false)
{
	ActorState* pState = NULL;
	if (emCrow == CROW_PC)
	{
		if ( bServer == true )
			pState = boost::polymorphic_downcast<ActorState*> (new ActorStateCharacterServer);
		else
			pState = boost::polymorphic_downcast<ActorState*> (new ActorStateCharacterClient);
	}
	else
		pState = new ActorState;

	m_pActorState = pState;
	D3DXMatrixIdentity(&this->m_matTrans);
}
GLActor::~GLActor(void)
{	
	SAFE_DELETE(m_pActorState);

	RemoveAllComponent();
}

// ActorState;
void GLActor::SetActState(const DWORD dwState)
{
	m_pActorState->SetActState(dwState);
}
void GLActor::ReSetActState(const DWORD dwState)
{
	m_pActorState->ReSetActState(dwState);
}

const bool GLActor::IsActState(const DWORD dwState) const
{
	return m_pActorState->IsActState(dwState);
}
const DWORD GLActor::GetActState(void) const
{
	return m_pActorState->GetActState();
}

void GLActor::SetAction(const EMACTIONTYPE emAction)
{
	m_pActorState->SetAction(emAction);
}

const EMACTIONTYPE& GLActor::GetAction(void) const
{
	return m_pActorState->GetAction();
}

const bool GLActor::isAction(const EMACTIONTYPE emAction) const
{
	return m_pActorState->isAction(emAction);
}
const bool GLActor::isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1) const
{
	return m_pActorState->isAction(emAction0, emAction1);
}
const bool GLActor::isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1, const EMACTIONTYPE emAction2) const
{
	return m_pActorState->isAction(emAction0, emAction1, emAction2);
}
const bool GLActor::IsValidBody(void) const
{
	return m_pActorState->IsValidBody();
}
const bool GLActor::IsDie(void) const
{
	return m_pActorState->IsDie();
}
const bool GLActor::IsIdle(void) const
{
	return m_pActorState->IsIdle();
}

// ActorState;
const Faction::GLActorID GLActor::GetActorID() const
{
	DWORD dwID = GAEAID_NULL;
	dwID = ( GetCrow() == CROW_PC ) ? GetCharID() : GetGaeaID(); 
	return Faction::GLActorID( GetCrow(), dwID );
}

const EMCROW GLActor::GetCrow(void) const
{
	return m_emCrow;
}
const DWORD GLActor::GetGaeaID(void) const
{
	return m_dwGaeaID;
}
void GLActor::SetGaeaID(const DWORD dwGaeaID)
{
	m_dwGaeaID = dwGaeaID;
}

void GLActor::Reset(void)
{
	_ResetActorMove();
	m_pActorState->RESET();

	RemoveAllChildCrow();
}