
#pragma once

struct MACRO::ReservedSkillID_Macro;
class BaseMacro;

class AutomaticSkillMacro : public BaseMacro
{
private:
	// 스킬 사용시 스킬 정보를 캐릭터 정보에 직접 접근하지않는다;
	// 편의상 스킬 정보를 가지고있는다;
	std::list<MACRO::ReservedSkillID_Macro>	m_lstReservedSkill;
	SNATIVEID	m_sNIDLasSkill;
	INT			m_nLastUseSkillTab;

	enum {	EMSKILLQUICKTAB_SIZE = EMSKILLQUICK_SIZE/EMSKILLQUICKTAB_SLOTSIZE,	};
	bool m_bUseSkillTab[EMSKILLQUICKTAB_SIZE];

public:
	AutomaticSkillMacro(GLGaeaClient* pGaeaClient);
	virtual ~AutomaticSkillMacro(void);

private:
	virtual void DoInit(void) override;
	virtual void DoUpdate(float fTime, float fElapsedTime) override;
	virtual void DoApply(void) override;
	virtual void DoEndProcedure(void) override;
	virtual bool CheckTransferable(void) override;
	virtual bool CheckEnd(void) override;

	virtual bool CheckSkill(const SNATIVEID& sSkillID) const;

public:
	virtual void SetSkillTabEnable(WORD wSlotNumber, bool bEnable);
	virtual bool IsSkillTabEnable(WORD wSlotNumber);
	virtual INT GetCheckTabCount(void);

	// 스킬 퀵슬롯의 스킬 정보가 갱신될 경우 m_lstReservedSkill 재 초기화가 필요하다;
	virtual void SlotInfoUpdate(void);
};