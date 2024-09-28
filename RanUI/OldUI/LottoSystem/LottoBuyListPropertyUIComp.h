#pragma once

#include "./LottoUIDateType.h"

#include "../Util/UIGroupHelper.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;
class CBasicButton;
class CSwapImage;

class RnButton;

// 구매 리스트 박스의 각 Property;
class CLottoBuyListPropertyUIComp : public CUIGroupHelper
{
public:
	enum PROPERTY_STATE
	{
		ESTATE_INACTIVE,
		ESTATE_ACTIVE
	};
	
public:
	CLottoBuyListPropertyUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoBuyListPropertyUIComp();

public:
	virtual void CreateSubControl();
	
protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	virtual void SetPropertyState( PROPERTY_STATE state );
	void SetNumText( unsigned int idx, unsigned int num );
	void SetBuyType( LottoSystem::EMLOTTO_BUY_TYPE buyType, unsigned int idx );

public:
	void SetBuyInfo( const LottoSystem::LOTTO_SYSTEM_BUY buyInfo, unsigned int idx );

protected:
	CInnerInterface* m_pInterface;

	CSwapImage* m_pTicketImage;

	CBasicTextBox* m_pBuyType;
	CBasicTextBox* m_pNumText[ LottoSystem::LOTTO_SLOT_NUM_CONT ];

	PROPERTY_STATE m_state;

	LottoSystem::LOTTO_SYSTEM_BUY m_buyInfo;
};