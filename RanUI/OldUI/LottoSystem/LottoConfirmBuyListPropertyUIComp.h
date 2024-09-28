#pragma once

#include "./LottoBuyListPropertyUIComp.h"

class CInnerInterface;
class EngineDeviceMan;

class CSwapImage;
class CBasicTextBox;

// 구매 리스트 Property;
class CLottoConfirmBuyListPropertyUIComp : public CLottoBuyListPropertyUIComp
{
public:
	enum CONFIRM_STATE
	{
		ESTATE_INCONFIRM,
		ESTATE_CONFIRM
	};

	enum
	{
		EConfirmRanking = NO_ID + 1
	};

public:
	CLottoConfirmBuyListPropertyUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoConfirmBuyListPropertyUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void SetPropertyState( CLottoBuyListPropertyUIComp::PROPERTY_STATE state );

public:
	void ConfirmStateOn();
	void ConfirmStateOff();

private:
	CBasicTextBox* m_pConfirmRanking;

	CSwapImage* m_pNumImage[ LottoSystem::LOTTO_SLOT_NUM_CONT ];

	CONFIRM_STATE m_confirmState;
};