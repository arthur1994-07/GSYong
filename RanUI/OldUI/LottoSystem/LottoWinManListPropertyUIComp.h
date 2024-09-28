#pragma once

#include "./LottoUIDateType.h"

#include "../Util/UIGroupHelper.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;

// 당첨자 리스트 박스의 각 Property;
class CLottoWinManListPropertyUIComp : public CUIGroupHelper
{
public:
	CLottoWinManListPropertyUIComp( CInnerInterface* pInterface
		, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoWinManListPropertyUIComp();

public:
	virtual void CreateSubControl();

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	void SetPropertyInfo( const LottoSystem::LOTTO_WINMAN_LIST_PROPERTY_MEMBER propertyInfo );

private:
	CInnerInterface* m_pInterface;

	CBasicTextBox* m_pRankingTextBox;
	CBasicTextBox* m_pNameTextBox;
	CBasicTextBox* m_pCountTextBox;
	CBasicTextBox* m_pWinMoneyTextBox;
};