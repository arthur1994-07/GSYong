#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class CPetSkillImage;
class CBasicTextBox;
class GLGaeaClient;

class CPetSkillSlot : public CUIGroup
{
protected:
	enum
	{
		SKILL_SLOT_IMAGE = NO_ID + 1
	};

public:
	CPetSkillSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CPetSkillSlot();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ( SNATIVEID sNativeID );

public:
	void		SetNativeSkillID ( SNATIVEID sNativeID )			{ m_sNativeID = sNativeID; }
	SNATIVEID	GetNativeSkillID ()									{ return m_sNativeID; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	CUIControl*		CreateControl ( char* szControl );
	CPetSkillImage*	CreateSkillImage ();
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );

private:
	CPetSkillImage*	m_pSkillImage;
	CBasicTextBox*	m_pTextBox;
	CUIControl*		m_pNotLearnImage;

	CString		m_strLine1;
	SNATIVEID	m_sNativeID;
	BOOL		m_bLearnSkill;
};