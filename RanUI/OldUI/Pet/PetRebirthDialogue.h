#pragma once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../../../RanLogic/Pet/GLPet.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicComboBox;
class CBasicComboBoxRollOver;
class GLGaeaClient;

class CPetRebirthDialogue : public CUIWindowEx
{
	enum
	{
		NAME_COMBO_OPEN = ET_CONTROL_NEXT,
		NAME_COMBO_ROLLOVER,
		OK_BUTTON,
		CANCLE_BUTTON,
	};
public:
	CPetRebirthDialogue(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CPetRebirthDialogue(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	CUIControl*	CreateControl( const char* szControl );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void SetPetInfo();
	void ClearComboText();

private:
	CBasicTextBox*				m_pPetTextBox;
	CBasicComboBox* 			m_pcbPetName;
	CBasicComboBoxRollOver *	m_pcbPetNameRollOver;

public:
	int		m_nIndex;
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

};