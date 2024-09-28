#pragma once

#include "./LottoBuyListPropertyUIComp.h"

class RnButton;

class CLottoBasicBuyListPropertyUIComp : public CLottoBuyListPropertyUIComp
{
public:
	enum
	{
		EBuyBuyyon = NO_ID + 1
	};

public:
	CLottoBasicBuyListPropertyUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBasicBuyListPropertyUIComp();

public:
	virtual void CreateSubControl();
	virtual void SetVisibleSingle( BOOL bVisible );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void SetPropertyState( CLottoBuyListPropertyUIComp::PROPERTY_STATE state );

private:
	RnButton* m_pBuyButton;
};