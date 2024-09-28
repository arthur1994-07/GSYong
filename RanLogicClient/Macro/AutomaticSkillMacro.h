
#pragma once

struct MACRO::ReservedSkillID_Macro;
class BaseMacro;

class AutomaticSkillMacro : public BaseMacro
{
private:
	// ��ų ���� ��ų ������ ĳ���� ������ ���� ���������ʴ´�;
	// ���ǻ� ��ų ������ �������ִ´�;
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

	// ��ų �������� ��ų ������ ���ŵ� ��� m_lstReservedSkill �� �ʱ�ȭ�� �ʿ��ϴ�;
	virtual void SlotInfoUpdate(void);
};