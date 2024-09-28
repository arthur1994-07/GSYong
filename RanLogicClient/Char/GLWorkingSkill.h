#ifndef GLWORKINGSKILL_H_
#define GLWORKINGSKILL_H_

class GLCharacter;

/**
    <�����ڰ� �� �ּ��� �ƴ϶� ��Ȯ���� ���� ���� ����>

    ��Ƽ ��ų�� �ϳ��� �ִϿ� ���� ������ ��ų�� �ߵ� ��Ű�� �����̴�.
    �̷��� �����ν� ũ�� ���⵵�� ������ �ʰ� ���� ��ų�� �ϴ� ������ ��Ƽ�� Ȱ�� �� �� �ְ� �ȴ�.

    ��Ƽ ��ų�̶�� �ؼ� �ϳ��� ��ų �ִϰ� �ߵ� �ǰ� ������ ��ų �ִϰ� �ߵ� �Ǵ°� �ƴԿ� ���� �Ѵ�.
    �ϳ��� �ִϿ� ���ӵ� ��ų�� �ߵ��� �ǵ��ϰ� �������.

    �׷��Ƿ� ���� ��ų���� �ִϿ� ������ ���� �ʴ� �������� ��ų�鸸 ���� �ϴ�.

    �׸��� Ŭ�󿡼��� �ִϿ� ���� Ÿ����, �����͵��� ���� �ǹǷ� 
    ���� ��ų���� �ִϴ� ���� ����Ǵ� �ִ�(ó�� ��ų �ִ�)���� �ִ� ���̰� �۾ƾ� ������ ���� �Ѵ�.
    (���� ���� �ִϴ� ��� ���� �ʰ� �ű⼭ Ÿ���̳� ����Ʈ ������ ������ ����.)

    ���� �� �ǵ��� ���� �ʰ� ��� �Ǵ� �͵��� �����Ƿ� ���� �ؾ� �Ѵ�.
    �׷��� �� �ǵ��� ��ȹ���� ���߱⵵ ���� ���� �ʴ�. ���� ��ų ��Ȱ���� �ϱ⵵ ���� �ʴ�. �̰��� ó�� ��ų�� ũ�� ���ӵǱ� �����̴�.
    �׷��Ƿ� ���� �̷� �ǵ��� �˰� �����丵�̳� �籸���� �ؾ� �� ���̴�.

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
