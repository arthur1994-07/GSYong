
#pragma once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"

class CInnerInterface;
class CBasicProgressBar;
class CBasicTextBox;
class CItemImage;
class GLGaeaClient;

class CRearcarWindow : public CUIWindowEx
{
private:
	enum
	{
		SLOT_PARTS_FRAME = ET_CONTROL_NEXT,	// 골조;
		SLOT_PARTS_SADDLE,		// 안장;
		SLOT_PARTS_TIRE,		// 바퀴;
		SLOT_PARTS_ROOF,		// 지붕장식;
		REARCAR_BOOST_OVER,
		VEHICLE_BATTERY_TEXT,
		SLOT_PARTS_SIZE = 4,
	};

	// 슬롯 정보;
	CItemImage*			m_pItemDisplay[SLOT_PARTS_SIZE];
	CUIControl*			m_pSlotDisplay[SLOT_PARTS_SIZE];
	CUIControl*			m_pBackground;		// 슬롯 뒤 배경 이미지;
	SITEMCUSTOM			m_ItemCustom[SLOT_PARTS_SIZE];

	// 탈것 정보;
	CBasicVarTextBox*	m_pVehicleInfo;	
	CBasicTextBox*		m_pVehicleName;
	CBasicTextBox*		m_pVehicleType;
	CBasicTextBox*		m_pVehicleParts;
	CBasicVarTextBox*	m_pInfo;

	// 배터리;
	CBasicTextBox*		m_pBatteryText;
	CBasicProgressBar*	m_pBatteryPBar;
	int					m_nBatteryBack;

	// 부스트;
	CUIControl*			m_pIconOver;
	CBasicTextBox*		m_pExplainText;
	CBasicTextBox*		m_pBoostText;
	CBasicTextBox*		m_pLearning;

	CUIControl*			m_pMouseOver;
	CUIControl*			m_pOver;

protected:
	GLGaeaClient*	m_pGaeaClient;

public:
	CRearcarWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CRearcarWindow();

public:
	void CreateSubControl();
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBox * CreateTextBox( char* szKeyword, CD3DFontPar* pFont );
	CUIControl*	CreateControl( const char* szControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CItemImage*	CreateItemImage( const char* szControl, UIGUID ControlID );
	void SetName( const std::string & strName );
	void SetType( const std::string & strType );
	void SetBattery( int nNOW );

	void LoadItem(int nIndex, const SITEMCUSTOM& ref_sItemCustom);
	void UnLoadItem(int nIndex);
	SITEMCUSTOM& GetItem(int nIndex) { return m_ItemCustom[nIndex]; }
	EMSUIT IMAGE2EMSLOT ( UIGUID ControlID );
};