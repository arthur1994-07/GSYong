#pragma once

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "./GLReaction.h"

class GLGaeaClient;

class GLReservationAction
{
public:
	GLReservationAction( GLGaeaClient* pGaeaClient );
	~GLReservationAction();

public:
	void Clear();
	void Clear( const EMREACTION emReaction );

public:
	GLREACTION_ITER PriorityRegisterReaction( const GLREACTION& sReaction ); // 우선 예약
	GLREACTION_ITER RegisterReaction( const GLREACTION& sReaction );
	GLREACTION_ITER RegisterReaction( GLREACTION_ITER iterWhere, const GLREACTION& sReaction );
	GLREACTION_ITER ReRegisterReaction( GLREACTION_ITER iterWhere, const GLREACTION& sReaction ); // 리액션 재 예약시 사용

public:
	GLREACTION_ITER EraseReaction( GLREACTION_ITER iterWhere );
	
public:
	GLREACTION_ITER Find_Skill_Reaction( const SNATIVEID& sSkillID );
	GLREACTION_ITER Find_First_Of_Attack_Reaction();
	GLREACTION_ITER Find_First_Of_Reaction( const EMREACTION emFind );
	GLREACTION_ITER Find_First_Of_Reaction( GLREACTION_ITER iterBegin, const EMREACTION emFind );
	GLREACTION_ITER Find_Last_Of_Reaction( const EMREACTION emFind );

public:
	void FeedbackReaction( const GLREACTION& sFeedBack );

public:
	const bool IsAllowedPreservation ( const GLREACTION& sReaction );

public:
	GLREACTION_ITER EndIter()   { return m_ReactionList.end(); }
	GLREACTION_ITER FrontIter() { return m_ReactionList.begin(); }

public:
	const bool IsEmpty() const { return m_ReactionList.empty(); }
	void PopFront( OUT GLREACTION& sReaction );
	void PopFront();
	GLREACTION& Front();
	
	bool IsExistReaction(EMREACTION emFind);

private:
	GLREACTION_LIST m_ReactionList;
	EMREACTION	    m_emLastReaction;	// 최근 예약한 행동.

private:
	GLGaeaClient*  m_pGaeaClient;
};
