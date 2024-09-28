#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Skill/GLSkill.h"

class CInnerInterface;
class CPetSkillPage;
class GLGaeaClient;

class CPetSkillWindow : public CUIWindowEx
{
protected:
	enum
	{
		PET_SKILL_WINDOW_PAGE = ET_CONTROL_NEXT
	};

public:
	CPetSkillWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPetSkillWindow();

public:
	void CreateSubControl();
	virtual	void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);
	virtual void SetVisibleSingle( BOOL bVisible );

private:
	CPetSkillPage* m_pPage;

protected:
	GLGaeaClient* m_pGaeaClient;

};
