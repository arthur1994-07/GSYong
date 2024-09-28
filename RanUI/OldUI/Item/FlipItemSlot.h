#pragma	once

#include "../Item/ItemSlot.h"

class CInnerInterface;
class GLGaeaClient;

class CFlipItemSlot : public CItemSlot
{
public:
	CFlipItemSlot ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CFlipItemSlot ();

	void	CreateFlipImage ( int nMaxColumn, bool bColor = false, DWORD dwColor = -1 );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*	CreateFlipImage ( const char* szFlip, bool bColor, DWORD dwColor );

public:
	void	SetFlipItem ( WORD wIndex, BOOL bFlip );
	BOOL	IsFlipItem ( WORD wIndex );

private:
	CUIControl*		m_pFlipImage[nLIMIT_COLUMN];

};
