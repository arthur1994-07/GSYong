
#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class	CInnerInterface;
class	CFlipItemSlot;
class	CBasicScrollBarEx;
class	GLInventory;
struct	SINVENITEM;
class	GLGaeaClient;

class SelectiveformBoxPage : public CUIGroup
{
public:
	enum	// æ∆¿Ã≈€ ΩΩ∑‘¿Ã∂Û±‚∫∏¥Ÿ¥¬ ∂Û¿Œ ∞≥≥‰¿Ã ≥¥∞Ÿ¥Ÿ.;
	{
		SELFORMBOXP_MAX_LINE = 6,

		SELFORMBOXP_WIDTH = 6,
		SELFORMBOXP_HEIGHT = 4,
	};

protected:
	enum	// UiID
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT2,
		ITEM_SLOT3,
		ITEM_SLOT4,
		ITEM_SLOT5,
		ITEM_SLOT6,
		ITEM_SLOT7,
		ITEM_SLOT_END = ITEM_SLOT0 + SELFORMBOXP_MAX_LINE

	};

private:
	static	const	int	nSTARTLINE;
	static	const	int	nOUTOFRANGE;

private:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

	CBasicScrollBarEx*	m_pScrollBar;

	CFlipItemSlot*		m_pItemSlotArray[SELFORMBOXP_MAX_LINE];
	CUIControl*			m_pItemSlotDummy[SELFORMBOXP_HEIGHT];
	
	int	m_nCurPos;
	int m_nSelectPosX;
	int m_nSelectPosY;

public:
	SelectiveformBoxPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~SelectiveformBoxPage(void);

public:
	void CreateSubControl(void);
	CUIControl*	CreateControl ( const char* szControl );
	CFlipItemSlot* CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void LoadItemPage(std::vector<ITEM::SBOX_ITEM>& vecItemID);
	void LoadItem(SNATIVEID& sNID);

public:
	BOOL GetSelectPos(INT& nPos) const;

private:	
	void ResetState(void);
	void SetItemSlotRender ( int nStartIndex, int nTotal );
	void ResetAllItemSlotRender( int nTotal );
};