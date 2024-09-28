#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharData.h"

class CInnerInterface;
class CBasicProgressBar;
class CSkillImage;
class GLGaeaClient;

class CSkillTimeUnit : public CUIGroup
{

public:

	static const D3DCOLOR DIFFUSE_ENABLE  = 0xFFFFFFFF;
	static const D3DCOLOR DIFFUSE_DISABLE = 0xFF555555;

	enum
	{
		SKILLTIMEUNIT_1 = NO_ID + 1,
		SKILLTIMEUNIT_2,
		SKILLTIMEUNIT_3,
	};

public:
	CSkillTimeUnit(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillTimeUnit ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetLeftTime ( const float& fLeftTime );
	void	SetLifeTime ( const float& fLeftTime );
	void	SetAdditional( const DWORD dwOption, const float fTimeLength );
	void	SetSkill ( const DWORD& dwID, const WORD& wLevel, const WORD& wSlot, const float& fLifeTime, const CString& strSkillName );
	void	SetEnable ( const BOOL bEnable );
	void    SetDetailVisible ( const BOOL bEnable );

private:
	SNATIVEID m_sSKILLID;
	WORD      m_wSKILLLEVEL;
	WORD      m_wSKILLSLOT;

	float	  m_fLEFT_TIME;
	float	  m_fLIFE_TIME;
	BOOL      m_bENABLE;
	BOOL      m_bDETAILVISBLE;

private:
	CString				m_strSkillName;
	CBasicProgressBar*	m_pProgressBar;
	CSkillImage*		m_pSkillImage;

protected:
	CInnerInterface* m_pInterface;

};