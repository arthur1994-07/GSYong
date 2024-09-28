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

	bool m_bComsumeSplit;			// �Ҹ� �������� ������ �и� ��ų�� ���;
	int m_nConsumeSplitPosX;				// �Ҹ� ������;
	int m_nConsumeSplitPosY;				// �Ҹ� ������;

	bool m_bItemDecompose;				// ������ ����;
	int m_nItemDecomposeX;			// ������ ���� ��ǥ;
	int m_nItemDecomposeY;			// ������ ���� ��ǥ;

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

	void InvenMouseLBDOWN();								// �κ��丮 ���츣 ��Ŭ��
	void InvenMouseLBUP( const SINVENITEM* pInvenItem );	// �κ��丮 ���콺 ��Ŭ��
	void InvenMouseRBUP( const SINVENITEM* pInvenItem );	// �κ��丮 ���콺 ��Ŭ��
	const bool InvenMsg( EMINVEN_USE_ITEM emInvenMsg );			// �κ��丮 ����ó�� �޽���

	bool IsPrivateMarketBlock( const SINVENITEM* pInvenItem );	// ���λ����� �ø� ���������� �˻�
	bool IsMessageBoxItem( const SINVENITEM* pInvenItem, const EMITEM_TYPE& emItemType );	// �޽����ڽ� �ʿ��� ���������� �˻�

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