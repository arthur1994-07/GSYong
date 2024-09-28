#pragma once

#include "../../../InnerInterface.h"
#include "../../Util/UIWindowEx.h"

class CBasicTextBox;
struct SSUM_ITEM;
namespace ReferChar
{
	struct SCharInfoAddition;
}

class RnCharacterAdditionalWindow : public CUIWindowEx
{
	// control ID
	enum
	{
		STATE_QUESTION_BUTTON = ET_CONTROL_NEXT+1,
		STATE_OVER_RECT_CRITICAL,
		STATE_OVER_RECT_CRUSHING_BLOW,
		STATE_OVER_RECT_MOVE_SPEED,
		STATE_OVER_RECT_ATK_SPEED,
		STATE_OVER_RECT_EXP_MULTIPLE,
		STATE_OVER_RECT_INC_PER_HP,
		STATE_OVER_RECT_INC_PER_MP,
		STATE_OVER_RECT_INC_PER_SP,
		STATE_OVER_RECT_INC_PER_CP,
		STATE_OVER_RECT_INC_INT_HP,
		STATE_OVER_RECT_INC_INT_MP,
		STATE_OVER_RECT_INC_INT_SP,
	};

	//
	enum
	{
		STATE_CRITICAL = 0,
		STATE_CRUSHING_BLOW = 1,
		STATE_MOVE_SPEED = 2,
		STATE_ATK_SPEED = 3,
		STATE_EXP_MULTIPLE = 4,
		STATE_INC_PER_HP = 5,
		STATE_INC_PER_MP = 6,
		STATE_INC_PER_SP = 7,
		STATE_INC_PER_CP = 8,
		STATE_INC_INT_HP = 9,
		STATE_INC_INT_MP = 10,
		STATE_INC_INT_SP = 11,
		STAT_MAX = 12
	};

public:
	RnCharacterAdditionalWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~RnCharacterAdditionalWindow();

public:
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) override;
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;

	void CreateSubControl();
	void SetIsRefer( bool bIsRefer ) { m_bIsRefer = bIsRefer; }
	void StateClearText();

	void RefreshReferChar();

private:
	void CreateOverRext();
	void CreateStateText();

	void SetStateText(  CBasicTextBox* const pTextBox, const int nValue, const bool bColor = false );
	void SetStatePerText( CBasicTextBox* const pTextBox, const float fValue, const bool bColor = false );

	void RefreshBasicInfo( GLCHARLOGIC& pCharLogic, SSUM_ITEM& sSumItem );
	void RefreshUpInfo( GLCHARLOGIC& pCharLogic, GLCHARLOGIC& pUpCharLogic );

	void RefreshReferInfo( ReferChar::SCharInfoAddition& cReferInfo );
	void RefreshReferExInfo( ReferChar::SCharInfoAddition& cReferInfo, GLCHARLOGIC& pUpCharLogic );	

	void ApplyInvenSlotItem( GLCHARLOGIC* pChar );

	void UpdateMyChar();
	void UpdateReferChar();

	void PrintTooltip( UIGUID ControlID, DWORD dwMsg );

private:
	GLGaeaClient*	m_pGaeaClient;

	typedef std::map<UIGUID,CUIControl*> OverRectMap;
	OverRectMap    m_OverRectMap;
	CBasicTextBox* m_pStatValue[STAT_MAX];
	CBasicTextBox* m_pStatUpValue[STAT_MAX];

	typedef std::auto_ptr<GLCHARLOGIC> AP_CharData;
	AP_CharData m_apCharData;

	bool m_bIsRefer;
	D3DXVECTOR2 m_vMoveGap;
};