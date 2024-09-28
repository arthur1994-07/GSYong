#ifndef _SKILL_CA_H_
#define _SKILL_CA_H_

#pragma once
#include "../Character/GLCharData.h"

struct SSKILLFACT;

// ĳ���Ͱ� ������ �ִ� SSKILLFACT ����Ʈ�� ������ �˻��ϴ°� �Ⱦ ����Ʈ�� ���� ������ ����.
class CSkillCAIdx
{
public :
	friend class CSkillCAInterface;
	
	enum{ MAX_MEM_SIZE = 10 };
public :
	CSkillCAIdx();
	~CSkillCAIdx();

	void	SetData( SNATIVEID sSkill, const SKILL::SSPEC_CA* pData, int iFactSlot, SNATIVEID sOrginSkill, int iSkillLv = 0);
	void	ResetData();
	void    ResetEffList();
	void	AllReset();

	void	SetUseCA( bool bUse ) { m_bUse = bUse; }
	void	SuccessCA ( void ) { m_bSuccess = true; }

	SNATIVEID				GetSkillID() { return m_SkillID; }
	SNATIVEID				GetOriginSkillID() { return m_OrignSkill; }
	const SKILL::SSPEC_CA*  GetCaData() const { return m_pcCAData; }

	inline bool	IsSimPacAddon() { return m_Pact.size() > 0 ? true : false; }
	inline bool	IsSpecddon()    { return m_Apply.size() > 0 ? true : false; }

	bool	IsUseCA() { return m_bUse; }
	bool	IsSuccess ( void ) { return m_bSuccess; }

	// �ڽſ��� ����� ȿ��.
	std::vector<SKILL::SSPEC_ADDON*>   m_Apply;
	std::vector<SKILL::SIMPACT_ADDON*> m_Pact;

	int    m_nFactSlot;
	int    m_nSkillLv;

	bool   IsAlive( WORD wSlot );

protected :	
	SNATIVEID					m_OrignSkill;
	SNATIVEID					m_SkillID;  // �ݰݽð������� �ߵ���ų ��ų	
	const SKILL::SSPEC_CA*	    m_pcCAData; // �ݰݼ��� ������
	bool						m_bUse;     // �ݰݻ�� ����
	bool						m_bSuccess;	// �ݰݼ��� ����
};

class GLChar;

class CSkillCAInterface
{
public :
	CSkillCAInterface();
	~CSkillCAInterface();

public :
	
	bool	  Cal_LastApplyValueII( SNATIVEID sAttackID, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd );
	void	  Cal_SideApply( const PGLSKILL pSkill, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd );
	STARGETID Cal_LastDamage( SNATIVEID sAttackID, WORD wAttLv, CSkillCAIdx* pAtt, CSkillCAIdx* pTar, const STARGETID& sAt, const STARGETID& sTa, bool bSkillEnd );
	
private :
	void GetCAEffList( CSkillCAIdx* psCA, std::vector<int>& rfList );
	bool IsCAEnable( const PGLSKILL pSkill );
	
};

#endif