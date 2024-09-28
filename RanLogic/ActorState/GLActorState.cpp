#include "pch.h"

#include "GLActorState.h"

ActorState::ActorState(void) : 
m_dwActState(0),
m_emAction(GLAT_IDLE)
{
}
ActorState::~ActorState(void)
{
}

void ActorState::RESET(void)
{
	this->m_dwActState = 0;
	this->m_emAction = GLAT_IDLE;
}

void ActorState::SetAction(const EMACTIONTYPE emAction)
{
	this->m_emAction = emAction;
}
void ActorState::SetActState(const DWORD dwState)
{
	this->m_dwActState |= dwState; 
}
void ActorState::ReSetActState(const DWORD dwState)
{
	this->m_dwActState &= ~dwState;
}

const bool ActorState::IsActState(const DWORD dwState) const
{
	return this->m_dwActState & dwState;
}
const bool ActorState::isAction(const EMACTIONTYPE emAction) const
{
	return (this->m_emAction == emAction);
}
const bool ActorState::isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1) const
{
	return (this->m_emAction == emAction0) || (this->m_emAction == emAction1);
}
const bool ActorState::isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1, const EMACTIONTYPE emAction2) const
{
	return (this->m_emAction == emAction0) || (this->m_emAction == emAction1) || (this->m_emAction == emAction2);
}
const bool ActorState::IsValidBody(void) const
{
	return isAction(GLAT_FALLING, GLAT_DIE) ? false : true;	//this->m_emAction < GLAT_FALLING;
}
const bool ActorState::IsDie(void) const
{
	return (this->m_emAction == GLAT_DIE);
}

const bool ActorState::IsIdle(void) const
{
	return ( this->m_emAction == GLAT_IDLE );
}

const DWORD ActorState::GetActState(void) const
{
	return this->m_dwActState;
}
const EMACTIONTYPE& ActorState::GetAction(void) const
{
	return this->m_emAction;
}


const bool ActorStateCharacterClient::IsValidBody(void) const
{
	if ( IsActState(EM_ACT_WAITING | EM_REQ_GATEOUT | EM_ACT_DIE) )
		return FALSE;	

	return ActorState::IsValidBody();
}
const bool ActorStateCharacterClient::IsDie(void) const
{	
	return !IsActState(EM_ACT_DIE) && isAction(GLAT_DIE);
}

const bool ActorStateCharacterServer::IsValidBody(void) const
{
	if ( IsActState(EM_ACT_WAITING | EM_REQ_GATEOUT | EM_ACT_DIE) )
		return false;	

	return ActorState::IsValidBody();
}
const bool ActorStateCharacterServer::IsDie(void) const
{
	if ( IsActState(EM_ACT_WAITING) )
		return false;

	if ( isAction(GLAT_DIE) == true )
		return true;

	if ( IsActState(EM_ACT_DIE) == true )
		return true;

	return false;
}
