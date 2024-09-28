#pragma once

#include "../../../RanLogic/Inventory/GLInventory.h"

#include "../Util/UIGroupHelperMessageBox.h"
#include "../Util/TapSelectorEx.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class CItemSlot;
class CBasicTextBox;

class CRnPageBase : public CUIGroupHelperMessageBox, private CTapSelectorEx< DEFAULT_RADIO_BUTTON >
{
protected:

	enum
	{
		ESlotX = 6,
		ESlotY = 10,
		EPageMax = 5,
	};

	enum
	{
		ERadioBegin = ID_NEXT,//NO_ID + 1,
		ERadioEnd = ERadioBegin + EPageMax,

		EHelpButton,

		EItemBegin,
		EItemEnd = EItemBegin + ESlotY,

		EPointButton,
		EMoneyButton,

		ELockerButton,
		EExtendButton,
		ETrashButton,
		ESortButton,
		ESplitButton,

		EEndControl,
	};

	struct stWindowOpenPos {

		int nPage;
		int nPosX;
		int nPosY;

		stWindowOpenPos() : nPage(-1), nPosX(-1), nPosY(-1) {}

		void reset() { nPage = -1; nPosX = -1; nPosY = -1; }
	};

protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CItemSlot* m_pItemSlot[ ESlotY ];
	CBasicTextBox* m_pPointTextBox;
	CBasicTextBox* m_pMoneyTextBox;

	int m_nPageMax;
	int m_nPage;

	int m_nFocusX;
	int m_nFocusY;

	stWindowOpenPos m_sWindowOpenPos;

public:
	CRnPageBase( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CRnPageBase();

public:
	virtual void CreateSubControl();
    virtual	void SetVisibleSingle( BOOL bVisible );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg ){};

protected:
	virtual void EventSelectedTap( UIGUID controlID );

	void HelpComment( const char* pText, DWORD nColor );

	void UpdateItemSlot( GLInventory& cInventory );
	void ResetSlotItem( int nX, int nY );
	void SetSlotItem( int nX, int nY, SINVENITEM& sInvenItem );
		
	virtual void UIPosToLogicPos( int& nX, int& nY, int nPage ) = 0;
	virtual void LogicPosToUIPos( int& nX, int& nY, int& nPage ) = 0;

public:
	bool IsCurrentPage ( int& nX, int& nY );
	void ChangePage ( int nPage );
	void SetBorderItemSlot ( int nX, int nY );
	void ResetBorderItemSlot ();
	void SetSlotExchange( int nX, int nY, bool bValue );

	void SetWindowOpenBox( int nX, int nY, BOOL bShow = FALSE );

public:
	const SINVENITEM* GetSlotItem( int nX, int nY );
	const SINVENITEM* GetFocusItem();
// 
// private:
// 	void HighLightSplitItem(void);
// 	void HighLightSlot(int nX, int nY);
};