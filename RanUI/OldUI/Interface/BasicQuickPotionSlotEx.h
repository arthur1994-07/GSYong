//	���� ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ : 
//	�α�
//		[2003.11.24]
//			@ �ۼ�
//

#pragma	once

#include "BasicQuickPotionSlot.h"

class	CInnerInterface;
class	CBasicTextBox;

class CBasicQuickPotionSlotEx : public CBasicQuickPotionSlot
{
public:
	CBasicQuickPotionSlotEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicQuickPotionSlotEx ();

public:
	void	CreateSubControl ( int nIndex );
	void	SetShotcutText( CString strTemp );

public:
	virtual void ResetText ();

public:
	UIRECT	GetAbsPosPotionImage ();
	CBasicTextBox* m_pTextBox;

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

protected:
	CInnerInterface* m_pInterface;
};