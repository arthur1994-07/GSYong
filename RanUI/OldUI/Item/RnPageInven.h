#pragma once

#include "./RnPageBase.h"

enum EMINVEN_USE_ITEM;
class GLPrivateMarket;

class CRnPageInven : public CRnPageBase
{
private:
	static DWORD INVEN_UI_HIGHLIGHTSLOT_EVENT;
private:
	bool m_bIsPressingLB;

	bool m_bLock;
	UIGUID m_ToLockID;

	bool m_bValidItem;
	bool m_bMarketOpen;

	bool m_bComsumeSplit;			// 소모 아이템을 낱개로 분리 시킬때 사용;
	int m_nConsumeSplitPosX;				// 소모 아이템;
	int m_nConsumeSplitPosY;				// 소모 아이템;

	bool m_bItemDecompose;				// 아이템 분해;
	int m_nItemDecomposeX;			// 아이템 분해 좌표;
	int m_nItemDecomposeY;			// 아이템 분해 좌표;

	int m_nMouseX;
	int	m_nMouseY;

    WORD m_wMessageBox_ItemPosX;
    WORD m_wMessageBox_ItemPosY;
    enum
    {
        MESSAGEBOX_NOT_UST = 0,
        MESSAGEBOX_CHECK_ITEM_BOX,
		MESSAGEBOX_CHECK_POINT_CARD,
		MESSAGEBOX_CHECK_EXP_ITEM,
    };
public:
	CRnPageInven( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );

private:
	void UpdateBlock();
	void UpdateTradeBlock( GLInventory& cInventory );
	void UpdatePrivateMarketBlock( GLPrivateMarket& sPMarket );

	void InvenItemInfoDisplay( const SINVENITEM* pInvenItem );

	void InvenMouseLBDOWN();								// 인벤토리 마우르 좌클릭
	void InvenMouseLBUP( const SINVENITEM* pInvenItem );	// 인벤토리 마우스 좌클릭
	void InvenMouseRBUP( const SINVENITEM* pInvenItem );	// 인벤토리 마우스 우클릭
	const bool InvenMsg( EMINVEN_USE_ITEM emInvenMsg );			// 인벤토리 예외처리 메시지

	bool IsPrivateMarketBlock( const SINVENITEM* pInvenItem );	// 개인상점에 올린 아이템인지 검사
	bool IsMessageBoxItem( const SINVENITEM* pInvenItem, const EMITEM_TYPE& emItemType );	// 메시지박스 필요한 아이템인지 검사

public:
	void SetLock( bool bLock );
	bool IsLock() { return m_bLock; }

	void SetLock( UIGUID ControlID, bool bLock );
	UIGUID GetToLockID() { return m_ToLockID; }

	bool IsValidLine( int nLine );
	void GetConsumeSplitInvenPos( WORD& nPosX, WORD& nPosY );

	virtual void UIPosToLogicPos( int& nX, int& nY, int nPage );
	virtual void LogicPosToUIPos( int& nX, int& nY, int& nPage );

	virtual void SetVisibleSingle( BOOL bVisible );
	bool IsItemDecompose(void) const	{ return m_bItemDecompose; }
	void GetItemDecomposePos(WORD& wPosX, WORD& wPosY);

private:
	BOOL _UpdateItemDecomposeSlot();
};