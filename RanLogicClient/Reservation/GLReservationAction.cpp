#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "../../EngineLib/DeviceMan.h"

#include "./GLReservationAction.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Tutorial/GLBaseTutorial.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLReservationAction::GLReservationAction( GLGaeaClient* pGaeaClient )
	: m_emLastReaction(REACT_NULL)
	, m_pGaeaClient(pGaeaClient)
{
	//Blank
}

GLReservationAction::~GLReservationAction()
{
	// [2014.09.03][shhan] boost::fast_pool_allocator<GLREACTION> ����ϸ鼭 Memory Leak �߻�. ���� �ľ� �ȵ�.
	//boost::singleton_pool<boost::fast_pool_allocator_tag, sizeof(GLREACTION)>::release_memory();
}

void GLReservationAction::Clear()
{
	m_ReactionList.clear();
	m_emLastReaction = REACT_NULL;
}

void GLReservationAction::Clear( const EMREACTION emReaction )
{
	GLREACTION_ITER iter = m_ReactionList.begin();
	for ( ; iter != m_ReactionList.end(); )
	{
		GLREACTION& sReaction = (*iter);
		if ( sReaction.GETTYPE() == emReaction )
		{
			iter = m_ReactionList.erase(iter);
			continue;
		}

		++iter;
	}
}

//! ��Ͻ� ���ǻ���
//  bREGISTATION ������ ��������ڷ� ���簡 ���� �ʱ⋚����
//  �� stl::list�� ������ ���� ������ ����� �մϴ�.

GLREACTION_ITER GLReservationAction::PriorityRegisterReaction( const GLREACTION& sReaction )
{
	if ( sReaction.ISNULL() )
	{
		sc::writeLogError(sc::string::format("GLReservationAction::PriorityRegisterReaction() - sReaction.ISNULL() - Reaction = %1%", 
			sReaction.GETTYPE()) );
		GASSERT( !"GLReservationAction::PriorityRegisterReaction() - sReaction.ISNULL()" );
		return m_ReactionList.end();
	}

	//! ��� ���� üũ
	{
		if ( sReaction.ISREGISTATION() )
		{
			sc::writeLogError(sc::string::format("GLReservationAction::PriorityRegisterReaction() - sReaction.ISREGISTATION() - Reaction = %1%", 
				sReaction.GETTYPE()) );
			GASSERT( !"GLReservationAction::PriorityRegisterReaction() - sReaction.ISREGISTATION()" );
			return m_ReactionList.end();
		}
	}

	if ( m_ReactionList.empty() )
	{
		m_emLastReaction = sReaction.GETTYPE();
	}

	GLREACTION_ITER iterNew = m_ReactionList.insert( m_ReactionList.begin(), sReaction );
	GLREACTION& sNewReaction = (*iterNew);

	sNewReaction.SETREGISTATION(true);

	//! ���� ���� üũ
	{
		if ( sNewReaction.ISPRESERVATION() && !IsAllowedPreservation(sNewReaction) )
		{
			sNewReaction.SETPRESERVATION(false);
		}
	}

	return ++iterNew;
}

GLREACTION_ITER GLReservationAction::RegisterReaction( const GLREACTION& sReaction )
{
	if ( sReaction.ISNULL() )
	{
		sc::writeLogError(sc::string::format("GLReservationAction::RegisterReaction(const GLREACTION&) - sReaction.ISNULL() - Reaction = %1%", 
			sReaction.GETTYPE()) );
		GASSERT( !"GLReservationAction::RegisterReaction() - sReaction.ISNULL()" );
		return m_ReactionList.end();
	}

	//! ��� ���� üũ
	{
		if ( sReaction.ISREGISTATION() )
		{
			sc::writeLogError(sc::string::format("GLReservationAction::RegisterReaction(const GLREACTION&) - sReaction.ISREGISTATION() - Reaction = %1%", 
				sReaction.GETTYPE()) );
			GASSERT( !"GLReservationAction::RegisterReaction() - sReaction.ISREGISTATION()" );
			return m_ReactionList.end();
		}
	}

	m_emLastReaction = sReaction.GETTYPE();

	GLREACTION_ITER iterNew = m_ReactionList.insert( m_ReactionList.end(), sReaction );
	GLREACTION& sNewReaction = (*iterNew);

	sNewReaction.SETREGISTATION(true);

	//! ���� ���� üũ
	{
		if ( sNewReaction.ISPRESERVATION() && !IsAllowedPreservation(sNewReaction) )
		{
			sNewReaction.SETPRESERVATION(false);
		}
	}

	return ++iterNew;
}

GLREACTION_ITER GLReservationAction::RegisterReaction( GLREACTION_ITER iterWhere, const GLREACTION& sReaction )
{
	if ( sReaction.ISNULL() )
	{
		sc::writeLogError(sc::string::format("GLReservationAction::RegisterReaction(GLREACTION_ITER,const GLREACTION&) - sReaction.ISNULL() - Reaction = %1%", 
			sReaction.GETTYPE()) );
		GASSERT( !"GLReservationAction::RegisterReaction() - sReaction.ISNULL()" );
		return m_ReactionList.end();
	}

	//! ��� ���� üũ
	{
		if ( sReaction.ISREGISTATION() )
		{
			sc::writeLogError(sc::string::format("GLReservationAction::RegisterReaction(GLREACTION_ITER,const GLREACTION&) - sReaction.ISREGISTATION() - Reaction = %1%", 
				sReaction.GETTYPE()) );
			GASSERT( !"GLReservationAction::RegisterReaction() - sReaction.ISREGISTATION()" );
			return m_ReactionList.end();
		}
	}

	if ( m_ReactionList.empty() )
	{
		return RegisterReaction( sReaction );
	}

	if ( iterWhere == m_ReactionList.end() )
	{
		return RegisterReaction( sReaction );
	}

	GLREACTION_ITER iterNew = m_ReactionList.insert( iterWhere, sReaction );
	GLREACTION& sNewReaction = (*iterNew);

	sNewReaction.SETREGISTATION(true);

	//! ���� ���� üũ
	{
		if ( sNewReaction.ISPRESERVATION() && !IsAllowedPreservation(sNewReaction) )
		{
			sNewReaction.SETPRESERVATION(false);
		}
	}

	return ++iterNew;
}

//! ���׼��� ���Ͻ�Ų��
//  ���� �ش� ���׼��� ���� �ݺ��̰�, �̹� ��� �Ǿ��ٸ� ��� ��Ű�� �ʴ´�.
//
GLREACTION_ITER GLReservationAction::ReRegisterReaction( GLREACTION_ITER iterWhere, const GLREACTION& sReaction )
{
	if ( sReaction.ISNULL() )
	{
		return m_ReactionList.end();
	}

	if ( sReaction.ISPRESERVATION() && sReaction.ISREGISTATION() )
	{
		return m_ReactionList.end();
	}

	return RegisterReaction(iterWhere, GLREACTION(sReaction));
}

GLREACTION_ITER GLReservationAction::EraseReaction( GLREACTION_ITER iterWhere )
{
	if ( m_ReactionList.empty() )
	{
		return m_ReactionList.end();
	}

	if ( iterWhere == m_ReactionList.end() )
	{
		return m_ReactionList.end(); 
	}

	return m_ReactionList.erase( iterWhere );
}

GLREACTION_ITER GLReservationAction::Find_Skill_Reaction( const SNATIVEID& sSkillID )
{
	GLREACTION_ITER iter = m_ReactionList.begin();
	for ( ; iter != m_ReactionList.end(); ++iter )
	{
		GLREACTION& sReaction = (*iter);
		if ( sReaction.GETTYPE() == REACT_SKILL )
		{
			GLREACTION_SKILL& sSkillReaction = (*iter);
			if ( sSkillReaction.GetSKILLID() == sSkillID )
			{
				return iter;
			}
		}
	}

	return m_ReactionList.end();
}

GLREACTION_ITER GLReservationAction::Find_First_Of_Attack_Reaction()
{
	GLREACTION_ITER iter = m_ReactionList.begin();
	for ( ; iter != m_ReactionList.end(); ++iter )
	{
		GLREACTION& sReaction = (*iter);
		if ( sReaction.GETTYPE() == REACT_MOB || sReaction.GETTYPE() == REACT_PVP )
		{
			return iter;
		}
	}

	return m_ReactionList.end();
}

GLREACTION_ITER GLReservationAction::Find_First_Of_Reaction( const EMREACTION emFind )
{
	return Find_First_Of_Reaction( m_ReactionList.begin(), emFind );
}

GLREACTION_ITER GLReservationAction::Find_First_Of_Reaction( GLREACTION_ITER iterBegin, const EMREACTION emFind )
{
	GLREACTION_ITER iter = iterBegin;
	for ( ; iter != m_ReactionList.end(); ++iter )
	{
		GLREACTION& sReaction = (*iter);
		if ( sReaction.GETTYPE() == emFind )
		{
			return iter;
		}
	}

	return m_ReactionList.end();
}

GLREACTION_ITER GLReservationAction::Find_Last_Of_Reaction( const EMREACTION emFind )
{
	GLREACTION_RITER riter = m_ReactionList.rbegin();
	for ( ; riter != m_ReactionList.rend(); ++riter )
	{
		GLREACTION& sReaction = (*riter);
		if ( sReaction.GETTYPE() == emFind )
		{
			return ((++riter).base());
		}
	}

	return m_ReactionList.end();
}


bool GLReservationAction::IsExistReaction(EMREACTION emFind)
{
	GLREACTION_ITER iter = m_ReactionList.begin();
	for ( ; iter != m_ReactionList.end(); ++iter )
	{
		GLREACTION& sReaction = (*iter);
		if ( sReaction.GETTYPE() == emFind )
		{
			return true;
		}
	}
	return false;
}

void GLReservationAction::PopFront( OUT GLREACTION& sReaction )
{
	if ( m_ReactionList.empty() )
	{
		return;
	}

	sReaction = m_ReactionList.front();
	sReaction.SETREGISTATION(false);

	m_ReactionList.pop_front();

	if ( m_ReactionList.empty() )
	{
		m_emLastReaction = REACT_NULL;
	}
}

void GLReservationAction::PopFront()
{
	if ( m_ReactionList.empty() )
	{
		return;
	}

	m_ReactionList.pop_front();

	if ( m_ReactionList.empty() )
	{
		m_emLastReaction = REACT_NULL;
	}
}

GLREACTION& GLReservationAction::Front()
 { 
	 if ( m_ReactionList.empty() )
	 {
		 static GLREACTION_NULL sNull;
		 return sNull;
	 }

	 return m_ReactionList.front(); 
 }

void GLReservationAction::FeedbackReaction( const GLREACTION& sFeedBack )
{
	if ( m_ReactionList.empty() )
	{
		return;
	}

	GLREACTION_ITER iter = m_ReactionList.begin();
	for ( ; iter != m_ReactionList.end(); ++iter )
	{
		GLREACTION& sReaction = (*iter); 

		if ( sReaction.GETTYPE() == sFeedBack.GETTYPE() )
        {
		    switch( sFeedBack.GETTYPE() )
		    {
		    case REACT_TOGGLE_VEHICLE:
			    {
				    const GLREACTION_TOGGLE_VEHICLE& sData = sReaction;
				    const GLREACTION_TOGGLE_VEHICLE& sFB   = sFeedBack;

				    if ( !sData.IsREQ() )
				    {
					    continue;
				    }

				    if ( sData.IsACTIVE() != sFB.IsACTIVE() )
				    {
					    continue;
				    }

				    m_ReactionList.erase(iter);
				    return;
			    }
			    break;

		    case REACT_SWAP_ARMS:
			    {
				    const GLREACTION_SWAP_ARMS& sData = sReaction;

				    if ( !sData.IsREQ() )
				    {
					    continue;
				    }

				    m_ReactionList.erase(iter);
				    return;
			    }
			    break;
		    };
        }
	}
}

const bool GLReservationAction::IsAllowedPreservation( const GLREACTION& sReaction )
{
	//! Ư���� ���׼��� �׻� ���
	switch( sReaction.GETTYPE() )
	{
	case REACT_TOGGLE_VEHICLE:
	case REACT_SWAP_ARMS:
		{
			return true;
		}
	};

	// Ʃ�丮�� ���϶�, Ư����Ȳ���� �Ұ���
	if ( m_pGaeaClient->GetTutorial()->IsTutorial() )
	{	
		if ( ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&& m_pGaeaClient->GetTutorial()->IsSubTwoStep() )
			|| ( m_pGaeaClient->GetTutorial()->IsAttackStep()
			&&  m_pGaeaClient->GetTutorial()->IsSubFourStep() ) )
		{
			return false;
		}
	}

	return true;
}
