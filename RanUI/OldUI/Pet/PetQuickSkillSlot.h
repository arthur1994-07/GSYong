#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class CPetSkillImage;
class CBasicTextBox;
class GLGaeaClient;

class CPetQuickSkillSlot : public CUIGroup
{
protected:
	enum
	{
		QUICK_SKILL_MOUSE_OVER = NO_ID + 1,
	};

public:
	CPetQuickSkillSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CPetQuickSkillSlot();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual	void	CreateSubControl ( const WORD wSlot );

public:
	UIRECT	GetAbsPosSkillImage();

protected:	
	void	CreateSkillImage ();
	void	CreateSkillStatic ();
	void	CreateMouseOver ();
	void	UpdateSlot( SNATIVEID sNativeID );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CPetSkillImage*		m_pSkillImage;
	CBasicTextBox*		m_pSkillStatic;

protected:
	CUIControl*			m_pSkillMouseOver;

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	WORD				m_wSlot;
};