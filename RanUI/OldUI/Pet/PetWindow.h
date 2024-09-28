#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Pet/GLPet.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_WEARSLOT1 = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CBasicProgressBar;
class CBasicTextBox;
class CItemImage;
class CBasicComboBox;
class CBasicComboBoxRollOver;
class CPetQuickSkillSlot;
class CInnerInterface;
struct SITEMCUSTOM;
class GLGaeaClient;

class CPetWindow : public CUIWindowEx
{
	enum
	{
		SLOT_IMAGE_TYPE1 = ET_CONTROL_NEXT,
		SLOT_IMAGE_TYPE2,

		PET_SKILL_SLOT0,
		PET_SKILL_SLOT1,

		SATIETY_TEXT,
		LOYALTY_TEXT,

		SKILL_COMBO_OPEN0,
		SKILL_COMBO_OPEN1,
		SKILL_COMBO_ROLLOVER0,
		SKILL_COMBO_ROLLOVER1,

		SKILL_BUTTON,

		SKILL_COMBO_OPEN_START     = SKILL_COMBO_OPEN0,
		SKILL_COMBO_ROLLOVER_START = SKILL_COMBO_ROLLOVER0,

		SLOT_IMAGE_SIZE = 2
	};


	static const D3DCOLOR COLOR_SKILLSLOT_ENABLE;
	static const D3DCOLOR COLOR_SKILLSLOT_DISABLE;

public:
	CBasicTextBox* m_pPetName;
	CBasicTextBox* m_pPetType;
	CBasicTextBox* m_pPetSkillComboBoxStatic[MAX_PETSKILLSLOT];
	CUIControl*	m_pMouseOver;

	CUIControl*	m_pSlotDisplay[SLOT_IMAGE_SIZE];
	CItemImage*	m_pSlotImage[SLOT_IMAGE_SIZE];
	SITEMCUSTOM	m_ItemCustomArray[SLOT_IMAGE_SIZE];

	CBasicTextBox *	m_pSatietyText;
	CBasicTextBox *	m_pLoyaltyText;

	CBasicProgressBar * m_pSatiety;
	CBasicProgressBar * m_pLoyalty;

	CBasicComboBox * m_pcbSkill[MAX_PETSKILLSLOT];
	CBasicComboBoxRollOver * m_pcbSkillRollOver[MAX_PETSKILLSLOT];

	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

public:
	CPetWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPetWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

private:
	CBasicTextBox * CreateTextBox( char* szKeyword, CD3DFontPar* pFont );
	CUIControl*	CreateControl( const char* szControl );
	CItemImage*	CreateItemImage( const char* szControl, UIGUID ControlID );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom );
	void UnLoadItem ( int nIndex );
	SITEMCUSTOM& GetItem ( int nIndex )			{ return m_ItemCustomArray[nIndex]; }

private:
	int m_nSatietyBack;
	WORD m_wLoyaltyBack;

	void SetName( const std::string & strName );
	void SetType( const std::string & strType );
	void SetSatiety( int nNOW );
	//void SetLoyalty( WORD wNOW, WORD wMAX );

public:
	EMSUIT IMAGE2EMSLOT ( UIGUID ControlID );
	void SetSkillCombo();

};