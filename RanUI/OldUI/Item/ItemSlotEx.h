//	������ ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.12.6]
//			@ �ۼ�
//

#pragma	once

#include "../Item/ItemSlot.h"

class CInnerInterface;

class CItemSlotEx : public CItemSlot
{
public:
	CItemSlotEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CItemSlotEx ();

public:
	void	CreateSubTradeInventory ( int nMaxColumn );
	void	CreateSubTrade ( int nMaxColumn );

private:
	CUIControl*	CreateFlipImage ( const char* szFlip );

public:
	void	SetFlipItem ( int nIndex, BOOL bFlip );
	BOOL	IsFlipItem ( int nIndex );

private:
	CUIControl*		m_pFlipImage[nLIMIT_COLUMN];
};