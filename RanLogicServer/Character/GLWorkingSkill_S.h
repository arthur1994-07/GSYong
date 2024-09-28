#ifndef _GL_WORKINGSKILL_H_
#define _GL_WORKINGSKILL_H_

#include <deque>

//	데미지 큐
struct SSTRIKE
{
	bool bHit;
	bool bLowSP;

	SSTRIKE() : bHit(false), bLowSP(false) { }
	SSTRIKE( WORD _wDivKey, bool _bHit, bool _bLowSP ) : bHit(_bHit), bLowSP(_bLowSP) { }
};

struct HitTrack
{
	DWORD		dwTick;
}; //HitTrack

class GLChar;

class GLWorkingSkill_s
{
public:
	GLWorkingSkill_s(GLWorkingSkill_s* m_pParent = NULL);
	~GLWorkingSkill_s();

	inline PGLSKILL					GetSkillData()									{ return m_pSkillData; }
	inline BOOL						IsEmpty()										{ return ( m_pSkillData == NULL ); }
	inline BOOL						CheckClass( EMSKILLCLASS skillClass )			{ return ( m_sSkill.sNativeID.wMainID == skillClass ); }
	inline SNATIVEID				GetId()											{ return m_sSkill.sNativeID; }
	inline WORD						GetCurStep()									{ return m_wCurTrack; }
	inline WORD						GetHitSize()									{ return m_sHITARRAY.size(); }
	
	void		Clear();

	BOOL		ApplyAllSkill();
	BOOL		HitDamage( const DWORD dwThisKey, const SANIATTACK &sAniAttack );
	DWORD		GetNumTargetInSkill();
	BOOL		SendToClientNoTarget();

	BOOL		Start( const SCHARSKILL& sSkill, GLChar* pOwnerChar, BOOL bLowSP, const STARGETID* pTargetID, const WORD wTARNUM, const STARID* pTARIDS, const bool bSubSkill );
	BOOL		MoveNext();
	void		FrameMove( float fElapsedTime );

	BOOL		IsApplyEnd();
	BOOL		IsApplyEnd(SNATIVEID sSkillIdx);

private:
	void		SetSkill( const SCHARSKILL sSkill, GLChar* pOwnerChar );
	void		SetSkillTarget( const STARGETID* pTargetID, const WORD wTARNUM, const STARID* pTARIDS );
	//bool		SetItemSkillTarget( sItemSkill& info, sItemSkill& Next, const STARGETID* pTargetID );
	bool		SetItemSkillTarget( sItemSkill& info, const STARGETID* pTargetID, GLChar* pChar );
	BOOL		ApplySkill();
	BOOL		ApplyItemSkillRun( GLChar* pChar );
	BOOL		ApplyItemSkillRunII();
	void		ApplyItemSkill( std::vector<sItemSkill>& info, GLChar* pChar );
	void		SetDelLock( bool bLock ) { m_bDelLock = bLock; }

private:
	typedef std::vector<DWORD>				HITLIST;
	typedef HITLIST::iterator				HITLIST_ITER;

	SCHARSKILL				m_sSkill;
	PGLSKILL				m_pSkillData;
	GLChar*					m_pCharacter;
	SANIATTACK*				m_pAniAttack;
	STARGETID				m_TargetID;
	WORD					m_wTARNUM;
	STARID					m_sTARIDS[EMTARGET_NET];

	WORD					m_wCurTrack;
	int						m_nStartSubSkillStep;
	SSTRIKE					m_strike;
	HITLIST					m_sHITARRAY;

	float					m_fAccumTime;
	float					m_fSkillAniTime;

	GLWorkingSkill_s*		m_pNext;
	GLWorkingSkill_s*		m_pParent;

	bool					m_bDelLock;
	
	std::vector<sItemSkill> m_vItemSkList;
}; //class GLWorkingSkill_s

struct SSKILLACTEX;
class GLGaeaServer;
bool		CheckPullIn( ServerActor* pActor, ServerActor* pTarget, float fRate, float& fDist );
bool		CheckKnockBack( ServerActor* pActor, ServerActor* pTarget, const float fRate );
bool 		GetPullSpeed( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, float& fSpeed, float& fDist );
bool 		ApplySkillTargetPullin( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX );
bool		ApplySkillTargetKnockback( ServerActor* pActor, ServerActor* pTarget, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX );
bool 		ApplySkillTargetOnward( ServerActor* pActor, ServerActor* pTarget, const STARGETID& sSelectTARID, const SKILL::SSPEC& sSKILL_SPEC, SSKILLACTEX& sSKILLACTEX );
void		ApplySkillTargetStun( ServerActor* pACTOR );
bool		ApplySkillSelfTeleport(GLGaeaServer* const pGaeaServer, ServerActor* pActor, PGLSKILL pSkill, const WORD wSKILL_LVL, const SKILL::SSPEC& sSKILL_SPEC, const STARGETID& TargetID, const WORD& wTARNUM, const STARID* sTARIDS );
bool		ApplySkillSelfSwapPos( ServerActor* pActor, PGLSKILL pSkill, const WORD wSKILL_LVL, const SKILL::SSPEC& sSKILL_SPEC, const WORD& wTARNUM, const STARID* sTARIDS, GLLandMan* pLandMan );
bool		ApplySkillSelfHallucinate( GLChar* pChar, PGLSKILL pSkill, const WORD wSKILL_LVL, const SKILL::SSPEC& sSKILL_SPEC);

#endif // _GL_WORKINGSKILL_H_
