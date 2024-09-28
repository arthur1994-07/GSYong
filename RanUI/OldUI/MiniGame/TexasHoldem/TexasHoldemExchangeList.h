#pragma once

#include "../../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "TexasHoldemWindowDefine.h"

namespace TexasHoldem
{

	class TexasHoldemExchangeList : public CBasicLineBoxSmart
	{
	private:
		enum
		{
			ID_EXCHANGE_CHIP_TEXTLIST = NO_ID + 1,
			ID_EXCHANGE_MONEY_TEXTLIST,
			ID_PRODUCT_TYPE_LIST_SCROOL,
		};

	public:
		TexasHoldemExchangeList( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface );

		void					CreateSubControl();
		virtual void			Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
		virtual void			TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

		void					SetSeletedRadioType( eExchange_Type emType );
		const int				GetSelectPos() { return m_nSelectedPos; }
		const eExchange_Type	GetSelectType() { return m_eSelectedType; }
		const std::vector<sChipPrice>& GetList(){ return m_PtrVectorExchangeList; }

	public:
		const bool	SetList( const std::vector< sChipPrice >& chipMap );
		void ClearText();

	private:
		inline void ResetTextBox();

	protected:
		GLGaeaClient*			m_pGaeaClient;
		CInnerInterface*		m_pInterface;

		CBasicScrollBarEx*		m_pScrollBar;
		CBasicTextBoxEx*		m_pChipTextBox;
		CBasicTextBoxEx*		m_pMoneyTextBox;
		CBasicLineBoxSmart*		m_pSeleted;
		UIRECT					m_rcSeleted;
		int						m_nSelectedPos;
		bool					m_bSelected;

		eExchange_Type			m_eSelectedType;

		std::vector<sChipPrice>	m_PtrVectorExchangeList;
	};

}