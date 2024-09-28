//	아이템 슬롯
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../Feature/../Feature/CostumColorInfo.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_ITEMSLOT = UIMSG_USER1;
const DWORD UIMSG_LBMUP_ITEMSLOT = UIMSG_USER2;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CItemImage;
class CBasicTextBox;
class CCostumColorInfo;
class GLGaeaClient;

const DWORD UIMSG_EXCHANGE_LIST = UIMSG_USER9;

class CItemSlot : public CUIGroup
{
protected:
static	const	int	nOUTOFRANGE;

public:
	enum
	{
		nLIMIT_COLUMN = 10
		//nLIMIT_COLUMN = EInvenX
	};

protected:
	int		m_nIndex;
	int		m_nMaxColumn;

	bool	m_bBLOCK;
	bool	m_bAutoBorder;

public:
	CUIControl*		m_pMouseOver;
	CUIControl*		m_pEmphasisSlot[ nLIMIT_COLUMN ];
	CUIControl*		m_pWindowOpenBox;

protected:
	CBasicTextBox*	m_pNumberBoxArray[nLIMIT_COLUMN];

protected:
	CItemImage*			m_pItemImageArray[nLIMIT_COLUMN];	
	SINVENITEM			m_InvenItem[nLIMIT_COLUMN];
	CUIControl*			m_pBlock;
	CUIControl*			m_pBlockImage[ nLIMIT_COLUMN ];
	CCostumColorInfo*	m_pColorInfo[ nLIMIT_COLUMN ]; // by luxes.

	//	bool	m_bTOP_CONTROL;

private:
	enum
	{
		ITEM_IMAGE0 = NO_ID + 1,
		ITEM_IMAGE1,
		ITEM_IMAGE2,
		ITEM_IMAGE3,
		ITEM_IMAGE4,
		ITEM_IMAGE5,
		ITEM_IMAGE6,
		ITEM_IMAGE7,
		ITEM_IMAGE8,
		ITEM_IMAGE9,

		ITEM_COLOR_INFO0,
		ITEM_COLOR_INFO1,
		ITEM_COLOR_INFO2,
		ITEM_COLOR_INFO3,
		ITEM_COLOR_INFO4,
		ITEM_COLOR_INFO5,
		ITEM_COLOR_INFO6,
		ITEM_COLOR_INFO7,
		ITEM_COLOR_INFO8,
		ITEM_COLOR_INFO9,
	};

public:
	CItemSlot(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CItemSlot ();

protected:
	bool		  m_bExchangeSlot;
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ( int nMaxColumn, BOOL bNumberUse = FALSE );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CItemImage*		CreateItemImage ( const char* szControl, UIGUID ControlID );
	void	CreateMouseOver ( char* szControl );	
	void	CreateNumberBox ();

	CCostumColorInfo*	CreateColorInfo( const char* szControl, UIGUID ControlID ); // by luxes.

private:
	CBasicTextBox*	CreateNumberBox ( const char* szControl );

public:
	void	SetItemImage ( int nIndex, SINVENITEM& ref_InvenItem );
	const SINVENITEM* GetItemImage(int nIndex) const;
	void	ResetItemImage ( int nIndex );

    CItemImage* GetItemImageStruct( int nColumn );

public:
	void	SetItemIndex ( int nIndex )			{ m_nIndex = nIndex; }
	int		GetItemIndex ( )					{ return m_nIndex; }

	void	SetExchangeSlot( bool bValue ) { m_bExchangeSlot = bValue; }
	bool	IsExchangeSlot() { return m_bExchangeSlot; }

public:
	void	SetNumber ( int nIndex, int nNumber, int nMaxNumber );
	void	ResetNumber ( int nIndex );

	void	SetBLOCK ( bool bXImage = false );
	void	ResetBLOCK ( bool bXImage = false );
	bool	IsBLOCK ()							{ return m_bBLOCK; }

public:
	void SetDiffuse( int nIndex, DWORD nColor );
	void SetDiffuseAlpha( int nIndex, BYTE nAlpha );
	void SetBorder( int nX, bool bBorder = false );
	void ResetBorder();

	void SetWindowOpenBox( int nX, BOOL bShow = FALSE );

	DWORD GetDiffuseAlpha( int nIndex );

protected:
	CInnerInterface* m_pInterface;

};