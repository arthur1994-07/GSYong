#ifndef GLWORKINGSKILL_H_
#define GLWORKINGSKILL_H_

class GLCharacter;

/**
    <개발자가 단 주석이 아니라 정확하지 않을 수도 있음>

    멀티 스킬은 하나의 애니에 연속 적으로 스킬을 발동 시키기 위함이다.
    이렇게 함으로써 크게 복잡도를 높이지 않고도 현재 스킬이 하는 동작을 멀티로 활용 할 수 있게 된다.

    멀티 스킬이라고 해서 하나의 스킬 애니가 발동 되고 다음이 스킬 애니가 발동 되는게 아님에 주의 한다.
    하나의 애니에 연속된 스킬의 발동을 의도하고 만들었다.

    그러므로 다음 스킬들은 애니에 영향을 받지 않는 버프류의 스킬들만 가능 하다.

    그리고 클라에서는 애니에 따라 타격점, 이펙터등이 종속 되므로 
    다음 스킬들의 애니는 현재 재생되는 애니(처음 스킬 애니)보다 애니 길이가 작아야 실행이 보장 한다.
    (실제 다음 애니는 재생 되지 않고 거기서 타격이나 이펙트 정보만 가져다 쓴다.)

    현재 이 의도에 맞지 않게 사용 되는 것들이 있으므로 주의 해야 한다.
    그런데 이 의도에 기획팀이 맞추기도 현재 쉽지 않다. 또한 스킬 재활용을 하기도 쉽지 않다. 이것은 처음 스킬에 크게 종속되기 때문이다.
    그러므로 향후 이런 의도를 알고 리팩토링이나 재구현을 해야 할 것이다.

 */
class GLWorkingSkill
{
public:
	GLWorkingSkill(GLGaeaClient* const pGaeaClient);
	~GLWorkingSkill();

	inline PGLSKILL					GetSkillData()									{ return m_pSkillData; }
	inline GLWorkingSkill*			GetNextSkill()									{ return m_pNext; }
	inline BOOL						IsEmpty()										{ return ( m_pSkillData == NULL ); }
	inline SNATIVEID				GetId()											{ return m_sSkill.sNativeID; }

	void		Clear();

	BOOL		CheckBasicFlag( DWORD dwCheckFlag );

	void		SendSkillMsg();
	
	void		Start( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill );
	void		StartDealyAction( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar, const SKILLTARGETDATA* pTargetData, bool bSubSkill );
	BOOL		MoveNext();

	void		FrameMove( float fElapsedTime );

protected:
	bool		GetArrivalPos( D3DXVECTOR3& vPos, BOOL& bOriglnPos );

	BOOL		SetSkill( const SCHARSKILL& sSkill, GLCharacter* pOwnerChar );
	void		SetSkillTarget( const SKILLTARGETDATA* pTargetData );

	void		SkillEffectZone( const SKILLTARGETDATA& sTargetData );
	void 		SkillEffectSpecific( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff );
	void 		SkillEffectUnit( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff );
	void		SkillEffectTarget( const SKILLTARGETDATA& sTargetData, const SANIMSTRIKE &sStrikeEff );
	void		SkillEffectTargetChange( const SANIMSTRIKE &sStrikeEff );

	void 		SkillEffectSelf( const SANIMSTRIKE &sStrikeEff );

private:
	GLGaeaClient* const m_pGaeaClient;
	SCHARSKILL			m_sSkill;
	PGLSKILL			m_pSkillData;
	SKILLTARGETDATA		m_sTargetData;
	GLCharacter*		m_pCharacter;
	DxSkinAnimation*	m_pAnimCont;
	float		        m_fAccumTime;
    float               m_fSkillAniTime;
	int			        m_nStep;	

	SNATIVEID	        m_idSubSkill;
	int				    m_nStartSubSkillStep;
	
	GLWorkingSkill*		m_pNext;
}; //class GLWorkingSkill

#endif //GLWORKINGSKILL_H_
