#include "pch.h"
#include "../RanLogic/ActorState/GLActorState.h"
#include "ClientActor.h"

ClientActor::ClientActor(GLGaeaClient* const pGaeaClient, const EMCROW emCrow) 
: GLActor(emCrow, false)
, m_pGaeaClient(pGaeaClient)
{	
}


ClientActor::~ClientActor(void)
{
	SAFE_DELETE(m_pActorState);
}

void ClientActor::Reset(void)
{
	GLActor::Reset();
	if ( m_pActorState != NULL )
		m_pActorState->RESET();	
	
	m_dwGaeaID = GAEAID_NULL;
}

