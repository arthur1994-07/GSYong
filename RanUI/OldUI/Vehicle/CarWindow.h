#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"

class CInnerInterface;
class CBasicProgressBar;
class CBasicTextBox;
class CItemImage;
class GLGaeaClient;

class CCarWindow : public CUIWindowEx
{
private:
	enum
	{
		// SLOT_PARTS_SKIN = 0,
		SLOT_PARTS_BUMPER = ET_CONTROL_NEXT,
		SLOT_PARTS_ENGINE,
		SLOT_PARTS_TIRE,
		SLOT_PARTS_STICKER,
		SLOT_PARTS_BACKDECORATION,
		CAR_BOOST_OVER,
		VEHICLE_BATTERY_TEXT,
		SLOT_PARTS_SIZE = 5,
	};

	CUIControl*			m_pBackGround;
	CUIControl*			m_pSlotDisplay[SLOT_PARTS_SIZE];
	CItemImage*			m_pItemDisplay[SLOT_PARTS_SIZE];
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

	// 부스트;
	CUIControl*			m_pIconOver;
	CBasicTextBox*		m_pExplainText;
	CBasicTextBox*		m_pBoostText;
	CBasicTextBox*		m_pLearning;

	CUIControl*			m_pMouseOver;
	CUIControl*			m_pOver;	
	
	int	m_nBatteryBack;

protected:
	GLGaeaClient* m_pGaeaClient;

public:
	CCarWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCarWindow();

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